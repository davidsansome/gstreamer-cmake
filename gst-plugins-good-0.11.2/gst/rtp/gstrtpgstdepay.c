/* GStreamer
 * Copyright (C) <2010> Wim Taymans <wim.taymans@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif

#include <gst/rtp/gstrtpbuffer.h>

#include <string.h>
#include "gstrtpgstdepay.h"

GST_DEBUG_CATEGORY_STATIC (rtpgstdepay_debug);
#define GST_CAT_DEFAULT (rtpgstdepay_debug)

static GstStaticPadTemplate gst_rtp_gst_depay_src_template =
GST_STATIC_PAD_TEMPLATE ("src",
    GST_PAD_SRC,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS_ANY);

static GstStaticPadTemplate gst_rtp_gst_depay_sink_template =
GST_STATIC_PAD_TEMPLATE ("sink",
    GST_PAD_SINK,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS ("application/x-rtp, "
        "media = (string) \"application\", "
        "payload = (int) " GST_RTP_PAYLOAD_DYNAMIC_STRING ", "
        "clock-rate = (int) 90000, " "encoding-name = (string) \"X-GST\"")
    );

#define gst_rtp_gst_depay_parent_class parent_class
G_DEFINE_TYPE (GstRtpGSTDepay, gst_rtp_gst_depay, GST_TYPE_RTP_BASE_DEPAYLOAD);

static void gst_rtp_gst_depay_finalize (GObject * object);

static GstStateChangeReturn gst_rtp_gst_depay_change_state (GstElement *
    element, GstStateChange transition);

static void gst_rtp_gst_depay_reset (GstRtpGSTDepay * rtpgstdepay);
static gboolean gst_rtp_gst_depay_setcaps (GstRTPBaseDepayload * depayload,
    GstCaps * caps);
static GstBuffer *gst_rtp_gst_depay_process (GstRTPBaseDepayload * depayload,
    GstBuffer * buf);

static void
gst_rtp_gst_depay_class_init (GstRtpGSTDepayClass * klass)
{
  GObjectClass *gobject_class;
  GstElementClass *gstelement_class;
  GstRTPBaseDepayloadClass *gstrtpbasedepayload_class;

  GST_DEBUG_CATEGORY_INIT (rtpgstdepay_debug, "rtpgstdepay", 0,
      "Gstreamer RTP Depayloader");

  gobject_class = (GObjectClass *) klass;
  gstelement_class = (GstElementClass *) klass;
  gstrtpbasedepayload_class = (GstRTPBaseDepayloadClass *) klass;

  gobject_class->finalize = gst_rtp_gst_depay_finalize;

  gstelement_class->change_state = gst_rtp_gst_depay_change_state;

  gst_element_class_add_pad_template (gstelement_class,
      gst_static_pad_template_get (&gst_rtp_gst_depay_src_template));
  gst_element_class_add_pad_template (gstelement_class,
      gst_static_pad_template_get (&gst_rtp_gst_depay_sink_template));

  gst_element_class_set_details_simple (gstelement_class,
      "GStreamer depayloader", "Codec/Depayloader/Network",
      "Extracts GStreamer buffers from RTP packets",
      "Wim Taymans <wim.taymans@gmail.com>");

  gstrtpbasedepayload_class->set_caps = gst_rtp_gst_depay_setcaps;
  gstrtpbasedepayload_class->process = gst_rtp_gst_depay_process;
}

static void
gst_rtp_gst_depay_init (GstRtpGSTDepay * rtpgstdepay)
{
  rtpgstdepay->adapter = gst_adapter_new ();
}

static void
gst_rtp_gst_depay_finalize (GObject * object)
{
  GstRtpGSTDepay *rtpgstdepay;

  rtpgstdepay = GST_RTP_GST_DEPAY (object);

  gst_rtp_gst_depay_reset (rtpgstdepay);
  g_object_unref (rtpgstdepay->adapter);

  G_OBJECT_CLASS (parent_class)->finalize (object);
}

static void
store_cache (GstRtpGSTDepay * rtpgstdepay, guint CV, GstCaps * caps)
{
  if (rtpgstdepay->CV_cache[CV])
    gst_caps_unref (rtpgstdepay->CV_cache[CV]);
  rtpgstdepay->CV_cache[CV] = caps;
}

static void
gst_rtp_gst_depay_reset (GstRtpGSTDepay * rtpgstdepay)
{
  guint i;

  gst_adapter_clear (rtpgstdepay->adapter);
  rtpgstdepay->current_CV = 0;
  for (i = 0; i < 8; i++)
    store_cache (rtpgstdepay, i, NULL);
}

static gboolean
gst_rtp_gst_depay_setcaps (GstRTPBaseDepayload * depayload, GstCaps * caps)
{
  GstRtpGSTDepay *rtpgstdepay;
  GstStructure *structure;
  GstCaps *outcaps;
  gint clock_rate;
  gboolean res;
  const gchar *capsenc;
  gchar *capsstr;

  rtpgstdepay = GST_RTP_GST_DEPAY (depayload);

  structure = gst_caps_get_structure (caps, 0);

  if (!gst_structure_get_int (structure, "clock-rate", &clock_rate))
    clock_rate = 90000;
  depayload->clock_rate = clock_rate;

  capsenc = gst_structure_get_string (structure, "caps");
  if (capsenc) {
    gsize out_len;

    capsstr = (gchar *) g_base64_decode (capsenc, &out_len);
    outcaps = gst_caps_from_string (capsstr);
    g_free (capsstr);

    /* we have the SDP caps as output caps */
    rtpgstdepay->current_CV = 0;
    gst_caps_ref (outcaps);
    store_cache (rtpgstdepay, 0, outcaps);
  } else {
    outcaps = gst_caps_new_any ();
  }
  res = gst_pad_set_caps (depayload->srcpad, outcaps);
  gst_caps_unref (outcaps);

  return res;
}

static GstBuffer *
gst_rtp_gst_depay_process (GstRTPBaseDepayload * depayload, GstBuffer * buf)
{
  GstRtpGSTDepay *rtpgstdepay;
  GstBuffer *subbuf, *outbuf = NULL;
  gint payload_len;
  guint8 *payload;
  guint CV;
  GstRTPBuffer rtp = { NULL };

  rtpgstdepay = GST_RTP_GST_DEPAY (depayload);

  gst_rtp_buffer_map (buf, GST_MAP_READ, &rtp);

  payload_len = gst_rtp_buffer_get_payload_len (&rtp);

  if (payload_len <= 8)
    goto empty_packet;

  if (GST_BUFFER_IS_DISCONT (buf)) {
    GST_WARNING_OBJECT (rtpgstdepay, "DISCONT, clear adapter");
    gst_adapter_clear (rtpgstdepay->adapter);
  }

  payload = gst_rtp_buffer_get_payload (&rtp);

  /* strip off header
   *
   *  0                   1                   2                   3
   *  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
   * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   * |C| CV  |D|0|0|0|                  MBZ                          |
   * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   * |                          Frag_offset                          |
   * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   */
  /* frag_offset =
   *   (payload[4] << 24) | (payload[5] << 16) | (payload[6] << 8) | payload[7];
   */

  /* subbuffer skipping the 8 header bytes */
  subbuf = gst_rtp_buffer_get_payload_subbuffer (&rtp, 8, -1);
  gst_adapter_push (rtpgstdepay->adapter, subbuf);

  if (gst_rtp_buffer_get_marker (&rtp)) {
    guint avail;
    GstCaps *outcaps;

    /* take the buffer */
    avail = gst_adapter_available (rtpgstdepay->adapter);
    outbuf = gst_adapter_take_buffer (rtpgstdepay->adapter, avail);

    CV = (payload[0] >> 4) & 0x7;

    if (payload[0] & 0x80) {
      guint b, csize, left, offset;
      GstMapInfo map;
      GstBuffer *subbuf;

      /* C bit, we have inline caps */
      gst_buffer_map (outbuf, &map, GST_MAP_READ);

      /* start reading the length, we need this to skip to the data later */
      csize = offset = 0;
      left = map.size;
      do {
        if (offset >= left) {
          gst_buffer_unmap (outbuf, &map);
          goto too_small;
        }
        b = map.data[offset++];
        csize = (csize << 7) | (b & 0x7f);
      } while (b & 0x80);

      if (left < csize) {
        gst_buffer_unmap (outbuf, &map);
        goto too_small;
      }

      /* parse and store in cache */
      outcaps = gst_caps_from_string ((gchar *) & map.data[offset]);
      store_cache (rtpgstdepay, CV, outcaps);

      /* skip caps */
      offset += csize;
      left -= csize;

      GST_DEBUG_OBJECT (rtpgstdepay,
          "inline caps %u, length %u, %" GST_PTR_FORMAT, CV, csize, outcaps);

      /* create real data buffer when needed */
      if (map.size)
        subbuf =
            gst_buffer_copy_region (outbuf, GST_BUFFER_COPY_ALL, offset, left);
      else
        subbuf = NULL;

      gst_buffer_unmap (outbuf, &map);
      gst_buffer_unref (outbuf);
      outbuf = subbuf;
    }

    /* see what caps we need */
    if (CV != rtpgstdepay->current_CV) {
      /* we need to switch caps, check if we have the caps */
      if ((outcaps = rtpgstdepay->CV_cache[CV]) == NULL)
        goto missing_caps;

      GST_DEBUG_OBJECT (rtpgstdepay,
          "need caps switch from %u to %u, %" GST_PTR_FORMAT,
          rtpgstdepay->current_CV, CV, outcaps);

      /* and set caps */
      if (gst_pad_set_caps (depayload->srcpad, outcaps))
        rtpgstdepay->current_CV = CV;
    }

    if (outbuf) {
      if (payload[0] & 0x8)
        GST_BUFFER_FLAG_SET (outbuf, GST_BUFFER_FLAG_DELTA_UNIT);
    }
  }
  return outbuf;

  /* ERRORS */
empty_packet:
  {
    GST_ELEMENT_WARNING (rtpgstdepay, STREAM, DECODE,
        ("Empty Payload."), (NULL));
    gst_rtp_buffer_unmap (&rtp);
    return NULL;
  }
too_small:
  {
    GST_ELEMENT_WARNING (rtpgstdepay, STREAM, DECODE,
        ("Buffer too small."), (NULL));
    if (outbuf)
      gst_buffer_unref (outbuf);
    gst_rtp_buffer_unmap (&rtp);
    return NULL;
  }
missing_caps:
  {
    GST_ELEMENT_WARNING (rtpgstdepay, STREAM, DECODE,
        ("Missing caps %u.", CV), (NULL));
    if (outbuf)
      gst_buffer_unref (outbuf);
    gst_rtp_buffer_unmap (&rtp);
    return NULL;
  }
}

static GstStateChangeReturn
gst_rtp_gst_depay_change_state (GstElement * element, GstStateChange transition)
{
  GstRtpGSTDepay *rtpgstdepay;
  GstStateChangeReturn ret;

  rtpgstdepay = GST_RTP_GST_DEPAY (element);

  switch (transition) {
    case GST_STATE_CHANGE_READY_TO_PAUSED:
      gst_rtp_gst_depay_reset (rtpgstdepay);
      break;
    default:
      break;
  }

  ret = GST_ELEMENT_CLASS (parent_class)->change_state (element, transition);

  switch (transition) {
    case GST_STATE_CHANGE_PAUSED_TO_READY:
      gst_rtp_gst_depay_reset (rtpgstdepay);
      break;
    default:
      break;
  }
  return ret;
}


gboolean
gst_rtp_gst_depay_plugin_init (GstPlugin * plugin)
{
  return gst_element_register (plugin, "rtpgstdepay",
      GST_RANK_MARGINAL, GST_TYPE_RTP_GST_DEPAY);
}
