/* GStreamer
 * Copyright (C) 2004 Benjamin Otte <in7y118@public.uni-hamburg.de>
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

/**
 * SECTION:element-theoradec
 * @see_also: theoraenc, oggdemux
 *
 * This element decodes theora streams into raw video
 * <ulink url="http://www.theora.org/">Theora</ulink> is a royalty-free
 * video codec maintained by the <ulink url="http://www.xiph.org/">Xiph.org
 * Foundation</ulink>, based on the VP3 codec.
 *
 * <refsect2>
 * <title>Example pipeline</title>
 * |[
 * gst-launch -v filesrc location=videotestsrc.ogg ! oggdemux ! theoradec ! xvimagesink
 * ]| This example pipeline will decode an ogg stream and decodes the theora video. Refer to
 * the theoraenc example to create the ogg file.
 * </refsect2>
 *
 * Last reviewed on 2006-03-01 (0.10.4)
 */

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif

#include "gsttheoradec.h"
#include <gst/tag/tag.h>
#include <gst/video/video.h>
#include <gst/video/gstvideometa.h>
#include <gst/video/gstvideopool.h>

#define GST_CAT_DEFAULT theoradec_debug
GST_DEBUG_CATEGORY_STATIC (GST_CAT_DEFAULT);
GST_DEBUG_CATEGORY_EXTERN (GST_CAT_PERFORMANCE);

#define THEORA_DEF_TELEMETRY_MV 0
#define THEORA_DEF_TELEMETRY_MBMODE 0
#define THEORA_DEF_TELEMETRY_QI 0
#define THEORA_DEF_TELEMETRY_BITS 0

enum
{
  PROP_0,
  PROP_TELEMETRY_MV,
  PROP_TELEMETRY_MBMODE,
  PROP_TELEMETRY_QI,
  PROP_TELEMETRY_BITS
};

static GstStaticPadTemplate theora_dec_src_factory =
GST_STATIC_PAD_TEMPLATE ("src",
    GST_PAD_SRC,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS ("video/x-raw, "
        "format = (string) { I420, Y42B, Y444 }, "
        "framerate = (fraction) [0/1, MAX], "
        "width = (int) [ 1, MAX ], " "height = (int) [ 1, MAX ]")
    );

static GstStaticPadTemplate theora_dec_sink_factory =
GST_STATIC_PAD_TEMPLATE ("sink",
    GST_PAD_SINK,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS ("video/x-theora")
    );

#define gst_theora_dec_parent_class parent_class
G_DEFINE_TYPE (GstTheoraDec, gst_theora_dec, GST_TYPE_ELEMENT);

static void theora_dec_get_property (GObject * object, guint prop_id,
    GValue * value, GParamSpec * pspec);
static void theora_dec_set_property (GObject * object, guint prop_id,
    const GValue * value, GParamSpec * pspec);

static gboolean theora_dec_setcaps (GstTheoraDec * dec, GstCaps * caps);
static gboolean theora_dec_sink_event (GstPad * pad, GstObject * parent,
    GstEvent * event);
static GstFlowReturn theora_dec_chain (GstPad * pad, GstObject * parent,
    GstBuffer * buffer);
static GstStateChangeReturn theora_dec_change_state (GstElement * element,
    GstStateChange transition);
static gboolean theora_dec_src_event (GstPad * pad, GstObject * parent,
    GstEvent * event);
static gboolean theora_dec_src_query (GstPad * pad, GstObject * parent,
    GstQuery * query);
static gboolean theora_dec_src_convert (GstPad * pad, GstFormat src_format,
    gint64 src_value, GstFormat * dest_format, gint64 * dest_value);

#if 0
static const GstFormat *theora_get_formats (GstPad * pad);
#endif
#if 0
static const GstEventMask *theora_get_event_masks (GstPad * pad);
#endif

static gboolean
gst_theora_dec_ctl_is_supported (int req)
{
  /* should return TH_EFAULT or TH_EINVAL if supported, and TH_EIMPL if not */
  return (th_decode_ctl (NULL, req, NULL, 0) != TH_EIMPL);
}

static void
gst_theora_dec_class_init (GstTheoraDecClass * klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
  GstElementClass *gstelement_class = GST_ELEMENT_CLASS (klass);

  gobject_class->set_property = theora_dec_set_property;
  gobject_class->get_property = theora_dec_get_property;

  if (gst_theora_dec_ctl_is_supported (TH_DECCTL_SET_TELEMETRY_MV)) {
    g_object_class_install_property (gobject_class, PROP_TELEMETRY_MV,
        g_param_spec_int ("visualize-motion-vectors",
            "Visualize motion vectors",
            "Show motion vector selection overlaid on image. "
            "Value gives a mask for motion vector (MV) modes to show",
            0, 0xffff, THEORA_DEF_TELEMETRY_MV,
            G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
  }

  if (gst_theora_dec_ctl_is_supported (TH_DECCTL_SET_TELEMETRY_MBMODE)) {
    g_object_class_install_property (gobject_class, PROP_TELEMETRY_MBMODE,
        g_param_spec_int ("visualize-macroblock-modes",
            "Visualize macroblock modes",
            "Show macroblock mode selection overlaid on image. "
            "Value gives a mask for macroblock (MB) modes to show",
            0, 0xffff, THEORA_DEF_TELEMETRY_MBMODE,
            G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
  }

  if (gst_theora_dec_ctl_is_supported (TH_DECCTL_SET_TELEMETRY_QI)) {
    g_object_class_install_property (gobject_class, PROP_TELEMETRY_QI,
        g_param_spec_int ("visualize-quantization-modes",
            "Visualize adaptive quantization modes",
            "Show adaptive quantization mode selection overlaid on image. "
            "Value gives a mask for quantization (QI) modes to show",
            0, 0xffff, THEORA_DEF_TELEMETRY_QI,
            G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
  }

  if (gst_theora_dec_ctl_is_supported (TH_DECCTL_SET_TELEMETRY_BITS)) {
    /* FIXME: make this a boolean instead? The value scales the bars so
     * they're less wide. Default is to use full width, and anything else
     * doesn't seem particularly useful, since the smaller bars just disappear
     * then (they almost disappear for a value of 2 already). */
    g_object_class_install_property (gobject_class, PROP_TELEMETRY_BITS,
        g_param_spec_int ("visualize-bit-usage",
            "Visualize bitstream usage breakdown",
            "Sets the bitstream breakdown visualization mode. "
            "Values influence the width of the bit usage bars to show",
            0, 0xff, THEORA_DEF_TELEMETRY_BITS,
            G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
  }

  gst_element_class_add_pad_template (gstelement_class,
      gst_static_pad_template_get (&theora_dec_src_factory));
  gst_element_class_add_pad_template (gstelement_class,
      gst_static_pad_template_get (&theora_dec_sink_factory));
  gst_element_class_set_details_simple (gstelement_class,
      "Theora video decoder", "Codec/Decoder/Video",
      "decode raw theora streams to raw YUV video",
      "Benjamin Otte <otte@gnome.org>, Wim Taymans <wim@fluendo.com>");

  gstelement_class->change_state = theora_dec_change_state;

  GST_DEBUG_CATEGORY_INIT (theoradec_debug, "theoradec", 0, "Theora decoder");
}

static void
gst_theora_dec_init (GstTheoraDec * dec)
{
  dec->sinkpad =
      gst_pad_new_from_static_template (&theora_dec_sink_factory, "sink");
  gst_pad_set_event_function (dec->sinkpad, theora_dec_sink_event);
  gst_pad_set_chain_function (dec->sinkpad, theora_dec_chain);
  gst_element_add_pad (GST_ELEMENT (dec), dec->sinkpad);

  dec->srcpad =
      gst_pad_new_from_static_template (&theora_dec_src_factory, "src");
  gst_pad_set_event_function (dec->srcpad, theora_dec_src_event);
  gst_pad_set_query_function (dec->srcpad, theora_dec_src_query);
  gst_pad_use_fixed_caps (dec->srcpad);

  gst_element_add_pad (GST_ELEMENT (dec), dec->srcpad);

  dec->telemetry_mv = THEORA_DEF_TELEMETRY_MV;
  dec->telemetry_mbmode = THEORA_DEF_TELEMETRY_MBMODE;
  dec->telemetry_qi = THEORA_DEF_TELEMETRY_QI;
  dec->telemetry_bits = THEORA_DEF_TELEMETRY_BITS;
  dec->gather = NULL;
  dec->decode = NULL;
  dec->queued = NULL;
  dec->pendingevents = NULL;
}

static void
gst_theora_dec_reset (GstTheoraDec * dec)
{
  dec->need_keyframe = TRUE;
  dec->last_timestamp = -1;
  dec->discont = TRUE;
  dec->frame_nr = -1;
  dec->seqnum = gst_util_seqnum_next ();
  dec->dropped = 0;
  dec->processed = 0;
  gst_segment_init (&dec->segment, GST_FORMAT_TIME);

  GST_OBJECT_LOCK (dec);
  dec->proportion = 1.0;
  dec->earliest_time = -1;
  GST_OBJECT_UNLOCK (dec);

  g_list_foreach (dec->queued, (GFunc) gst_mini_object_unref, NULL);
  g_list_free (dec->queued);
  dec->queued = NULL;
  g_list_foreach (dec->gather, (GFunc) gst_mini_object_unref, NULL);
  g_list_free (dec->gather);
  dec->gather = NULL;
  g_list_foreach (dec->decode, (GFunc) gst_mini_object_unref, NULL);
  g_list_free (dec->decode);
  dec->decode = NULL;
  g_list_foreach (dec->pendingevents, (GFunc) gst_mini_object_unref, NULL);
  g_list_free (dec->pendingevents);
  dec->pendingevents = NULL;

  if (dec->tags) {
    gst_tag_list_free (dec->tags);
    dec->tags = NULL;
  }
}

#if 0
static const GstFormat *
theora_get_formats (GstPad * pad)
{
  static GstFormat src_formats[] = {
    GST_FORMAT_DEFAULT,         /* frames in this case */
    GST_FORMAT_TIME,
    GST_FORMAT_BYTES,
    0
  };
  static GstFormat sink_formats[] = {
    GST_FORMAT_DEFAULT,
    GST_FORMAT_TIME,
    0
  };

  return (GST_PAD_IS_SRC (pad) ? src_formats : sink_formats);
}
#endif

#if 0
static const GstEventMask *
theora_get_event_masks (GstPad * pad)
{
  static const GstEventMask theora_src_event_masks[] = {
    {GST_EVENT_SEEK, GST_SEEK_METHOD_SET | GST_SEEK_FLAG_FLUSH},
    {0,}
  };

  return theora_src_event_masks;
}
#endif

static gboolean
theora_dec_src_convert (GstPad * pad,
    GstFormat src_format, gint64 src_value,
    GstFormat * dest_format, gint64 * dest_value)
{
  gboolean res = TRUE;
  GstTheoraDec *dec;
  guint64 scale = 1;

  if (src_format == *dest_format) {
    *dest_value = src_value;
    return TRUE;
  }

  dec = GST_THEORA_DEC (gst_pad_get_parent (pad));

  /* we need the info part before we can done something */
  if (!dec->have_header)
    goto no_header;

  switch (src_format) {
    case GST_FORMAT_BYTES:
      switch (*dest_format) {
        case GST_FORMAT_DEFAULT:
          *dest_value = gst_util_uint64_scale_int (src_value, 8,
              dec->info.pic_height * dec->info.pic_width * dec->output_bpp);
          break;
        case GST_FORMAT_TIME:
          /* seems like a rather silly conversion, implement me if you like */
        default:
          res = FALSE;
      }
      break;
    case GST_FORMAT_TIME:
      switch (*dest_format) {
        case GST_FORMAT_BYTES:
          scale =
              dec->output_bpp * (dec->info.pic_width * dec->info.pic_height) /
              8;
        case GST_FORMAT_DEFAULT:
          *dest_value = scale * gst_util_uint64_scale (src_value,
              dec->info.fps_numerator, dec->info.fps_denominator * GST_SECOND);
          break;
        default:
          res = FALSE;
      }
      break;
    case GST_FORMAT_DEFAULT:
      switch (*dest_format) {
        case GST_FORMAT_TIME:
          *dest_value = gst_util_uint64_scale (src_value,
              GST_SECOND * dec->info.fps_denominator, dec->info.fps_numerator);
          break;
        case GST_FORMAT_BYTES:
          *dest_value = gst_util_uint64_scale_int (src_value,
              dec->output_bpp * dec->info.pic_width * dec->info.pic_height, 8);
          break;
        default:
          res = FALSE;
      }
      break;
    default:
      res = FALSE;
  }
done:
  gst_object_unref (dec);
  return res;

  /* ERRORS */
no_header:
  {
    GST_DEBUG_OBJECT (dec, "no header yet, cannot convert");
    res = FALSE;
    goto done;
  }
}

#if 0
static gboolean
theora_dec_sink_convert (GstPad * pad,
    GstFormat src_format, gint64 src_value,
    GstFormat * dest_format, gint64 * dest_value)
{
  gboolean res = TRUE;
  GstTheoraDec *dec;

  if (src_format == *dest_format) {
    *dest_value = src_value;
    return TRUE;
  }

  dec = GST_THEORA_DEC (gst_pad_get_parent (pad));

  /* we need the info part before we can done something */
  if (!dec->have_header)
    goto no_header;

  switch (src_format) {
    case GST_FORMAT_DEFAULT:
      switch (*dest_format) {
        case GST_FORMAT_TIME:
          *dest_value = _theora_granule_start_time (dec, src_value);
          break;
        default:
          res = FALSE;
      }
      break;
    case GST_FORMAT_TIME:
      switch (*dest_format) {
        case GST_FORMAT_DEFAULT:
        {
          guint rest;

          /* framecount */
          *dest_value = gst_util_uint64_scale (src_value,
              dec->info.fps_numerator, GST_SECOND * dec->info.fps_denominator);

          /* funny way of calculating granulepos in theora */
          rest = *dest_value / dec->info.keyframe_granule_shift;
          *dest_value -= rest;
          *dest_value <<= dec->granule_shift;
          *dest_value += rest;
          break;
        }
        default:
          res = FALSE;
          break;
      }
      break;
    default:
      res = FALSE;
  }
done:
  gst_object_unref (dec);
  return res;

  /* ERRORS */
no_header:
  {
    GST_DEBUG_OBJECT (dec, "no header yet, cannot convert");
    res = FALSE;
    goto done;
  }
}
#endif

static gboolean
theora_dec_src_query (GstPad * pad, GstObject * parent, GstQuery * query)
{
  GstTheoraDec *dec;
  gboolean res = FALSE;

  dec = GST_THEORA_DEC (parent);

  switch (GST_QUERY_TYPE (query)) {
    case GST_QUERY_POSITION:
    {
      gint64 value;
      GstFormat format;
      gint64 time;

      /* parse format */
      gst_query_parse_position (query, &format, NULL);

      time = dec->last_timestamp;
      time = gst_segment_to_stream_time (&dec->segment, GST_FORMAT_TIME, time);

      GST_LOG_OBJECT (dec,
          "query %p: our time: %" GST_TIME_FORMAT, query, GST_TIME_ARGS (time));

      if (!(res =
              theora_dec_src_convert (pad, GST_FORMAT_TIME, time, &format,
                  &value)))
        goto error;

      gst_query_set_position (query, format, value);

      GST_LOG_OBJECT (dec,
          "query %p: we return %" G_GINT64_FORMAT " (format %u)", query, value,
          format);
      break;
    }
    case GST_QUERY_DURATION:
    {
      /* forward to peer for total */
      res = gst_pad_peer_query (dec->sinkpad, query);
      if (!res)
        goto error;

      break;
    }
    case GST_QUERY_CONVERT:
    {
      GstFormat src_fmt, dest_fmt;
      gint64 src_val, dest_val;

      gst_query_parse_convert (query, &src_fmt, &src_val, &dest_fmt, &dest_val);
      if (!(res =
              theora_dec_src_convert (pad, src_fmt, src_val, &dest_fmt,
                  &dest_val)))
        goto error;

      gst_query_set_convert (query, src_fmt, src_val, dest_fmt, dest_val);
      break;
    }
    default:
      res = gst_pad_query_default (pad, parent, query);
      break;
  }
done:

  return res;

  /* ERRORS */
error:
  {
    GST_DEBUG_OBJECT (dec, "query failed");
    goto done;
  }
}

static gboolean
theora_dec_src_event (GstPad * pad, GstObject * parent, GstEvent * event)
{
  gboolean res = TRUE;
  GstTheoraDec *dec;

  dec = GST_THEORA_DEC (parent);

  switch (GST_EVENT_TYPE (event)) {
    case GST_EVENT_SEEK:
    {
      GstFormat format, tformat;
      gdouble rate;
      GstEvent *real_seek;
      GstSeekFlags flags;
      GstSeekType cur_type, stop_type;
      gint64 cur, stop;
      gint64 tcur, tstop;
      guint32 seqnum;

      gst_event_parse_seek (event, &rate, &format, &flags, &cur_type, &cur,
          &stop_type, &stop);
      seqnum = gst_event_get_seqnum (event);
      gst_event_unref (event);

      /* we have to ask our peer to seek to time here as we know
       * nothing about how to generate a granulepos from the src
       * formats or anything.
       * 
       * First bring the requested format to time 
       */
      tformat = GST_FORMAT_TIME;
      if (!(res = theora_dec_src_convert (pad, format, cur, &tformat, &tcur)))
        goto convert_error;
      if (!(res = theora_dec_src_convert (pad, format, stop, &tformat, &tstop)))
        goto convert_error;

      /* then seek with time on the peer */
      real_seek = gst_event_new_seek (rate, GST_FORMAT_TIME,
          flags, cur_type, tcur, stop_type, tstop);
      gst_event_set_seqnum (real_seek, seqnum);

      res = gst_pad_push_event (dec->sinkpad, real_seek);
      break;
    }
    case GST_EVENT_QOS:
    {
      gdouble proportion;
      GstClockTimeDiff diff;
      GstClockTime timestamp;

      gst_event_parse_qos (event, NULL, &proportion, &diff, &timestamp);

      /* we cannot randomly skip frame decoding since we don't have
       * B frames. we can however use the timestamp and diff to not
       * push late frames. This would at least save us the time to
       * crop/memcpy the data. */
      GST_OBJECT_LOCK (dec);
      dec->proportion = proportion;
      dec->earliest_time = timestamp + diff;
      GST_OBJECT_UNLOCK (dec);

      GST_DEBUG_OBJECT (dec, "got QoS %" GST_TIME_FORMAT ", %" G_GINT64_FORMAT,
          GST_TIME_ARGS (timestamp), diff);

      res = gst_pad_push_event (dec->sinkpad, event);
      break;
    }
    default:
      res = gst_pad_push_event (dec->sinkpad, event);
      break;
  }
done:

  return res;

  /* ERRORS */
convert_error:
  {
    GST_DEBUG_OBJECT (dec, "could not convert format");
    goto done;
  }
}

static gboolean
theora_dec_sink_event (GstPad * pad, GstObject * parent, GstEvent * event)
{
  gboolean ret = FALSE;
  GstTheoraDec *dec;

  dec = GST_THEORA_DEC (parent);

  GST_LOG_OBJECT (dec, "handling event");
  switch (GST_EVENT_TYPE (event)) {
    case GST_EVENT_FLUSH_START:
      ret = gst_pad_push_event (dec->srcpad, event);
      break;
    case GST_EVENT_FLUSH_STOP:
      gst_theora_dec_reset (dec);
      ret = gst_pad_push_event (dec->srcpad, event);
      break;
    case GST_EVENT_EOS:
      ret = gst_pad_push_event (dec->srcpad, event);
      break;
    case GST_EVENT_SEGMENT:
    {
      const GstSegment *segment;

      gst_event_parse_segment (event, &segment);

      /* we need TIME format */
      if (segment->format != GST_FORMAT_TIME)
        goto newseg_wrong_format;

      GST_DEBUG_OBJECT (dec, "segment: %" GST_SEGMENT_FORMAT, segment);

      /* now configure the values */
      gst_segment_copy_into (segment, &dec->segment);
      dec->seqnum = gst_event_get_seqnum (event);

      /* We don't forward this unless/until the decoder is initialised */
      if (dec->have_header) {
        ret = gst_pad_push_event (dec->srcpad, event);
      } else {
        dec->pendingevents = g_list_append (dec->pendingevents, event);
        ret = TRUE;
      }
      break;
    }
    case GST_EVENT_CAPS:
    {
      GstCaps *caps;

      gst_event_parse_caps (event, &caps);
      ret = theora_dec_setcaps (dec, caps);
      gst_event_unref (event);
      break;
    }
    case GST_EVENT_TAG:
    {
      if (dec->have_header)
        /* and forward */
        ret = gst_pad_push_event (dec->srcpad, event);
      else {
        /* store it to send once we're initialized */
        dec->pendingevents = g_list_append (dec->pendingevents, event);
        ret = TRUE;
      }
      break;
    }
    default:
      ret = gst_pad_event_default (pad, parent, event);
      break;
  }
done:

  return ret;

  /* ERRORS */
newseg_wrong_format:
  {
    GST_DEBUG_OBJECT (dec, "received non TIME newsegment");
    gst_event_unref (event);
    goto done;
  }
}

static gboolean
theora_dec_setcaps (GstTheoraDec * dec, GstCaps * caps)
{
  GstStructure *s;
  const GValue *codec_data;

  s = gst_caps_get_structure (caps, 0);

  /* parse the par, this overrides the encoded par */
  dec->have_par = gst_structure_get_fraction (s, "pixel-aspect-ratio",
      &dec->par_num, &dec->par_den);

  if ((codec_data = gst_structure_get_value (s, "codec_data"))) {
    if (G_VALUE_TYPE (codec_data) == GST_TYPE_BUFFER) {
      GstBuffer *buffer;
      GstMapInfo map;
      guint8 *ptr;
      gsize left;
      guint offset;

      buffer = gst_value_get_buffer (codec_data);

      offset = 0;
      gst_buffer_map (buffer, &map, GST_MAP_READ);

      ptr = map.data;
      left = map.size;

      while (left > 2) {
        guint psize;
        GstBuffer *buf;

        psize = (ptr[0] << 8) | ptr[1];
        /* skip header */
        ptr += 2;
        left -= 2;
        offset += 2;

        /* make sure we don't read too much */
        psize = MIN (psize, left);

        buf =
            gst_buffer_copy_region (buffer, GST_BUFFER_COPY_ALL, offset, psize);

        /* first buffer is a discont buffer */
        if (offset == 2)
          GST_BUFFER_FLAG_SET (buf, GST_BUFFER_FLAG_DISCONT);

        /* now feed it to the decoder we can ignore the error */
        theora_dec_chain (dec->sinkpad, GST_OBJECT_CAST (dec), buf);

        /* skip the data */
        left -= psize;
        ptr += psize;
        offset += psize;
      }
      gst_buffer_unmap (buffer, &map);
    }
  }

  return TRUE;
}

static GstFlowReturn
theora_handle_comment_packet (GstTheoraDec * dec, ogg_packet * packet)
{
  gchar *encoder = NULL;
  GstTagList *list;

  GST_DEBUG_OBJECT (dec, "parsing comment packet");

  list =
      gst_tag_list_from_vorbiscomment (packet->packet, packet->bytes,
      (guint8 *) "\201theora", 7, &encoder);

  if (!list) {
    GST_ERROR_OBJECT (dec, "couldn't decode comments");
    list = gst_tag_list_new_empty ();
  }
  if (encoder) {
    gst_tag_list_add (list, GST_TAG_MERGE_REPLACE,
        GST_TAG_ENCODER, encoder, NULL);
    g_free (encoder);
  }
  gst_tag_list_add (list, GST_TAG_MERGE_REPLACE,
      GST_TAG_ENCODER_VERSION, dec->info.version_major,
      GST_TAG_VIDEO_CODEC, "Theora", NULL);

  if (dec->info.target_bitrate > 0) {
    gst_tag_list_add (list, GST_TAG_MERGE_REPLACE,
        GST_TAG_BITRATE, dec->info.target_bitrate,
        GST_TAG_NOMINAL_BITRATE, dec->info.target_bitrate, NULL);
  }

  dec->tags = list;

  return GST_FLOW_OK;
}

static GstFlowReturn
theora_negotiate (GstTheoraDec * dec)
{
  GstVideoFormat format;
  GstQuery *query;
  GstBufferPool *pool;
  guint size, min, max;
  GstStructure *config;
  GstCaps *caps;
  GstVideoInfo info, cinfo;

  /* theora has:
   *
   *  frame_width/frame_height : dimension of the encoded frame
   *  pic_width/pic_height : dimension of the visible part
   *  pic_x/pic_y : offset in encoded frame where visible part starts
   */
  GST_DEBUG_OBJECT (dec, "frame dimension %dx%d, PAR %d/%d, fps %d/%d",
      dec->info.frame_width, dec->info.frame_height,
      dec->info.aspect_numerator, dec->info.aspect_denominator,
      dec->info.fps_numerator, dec->info.fps_denominator);
  GST_DEBUG_OBJECT (dec, "picture dimension %dx%d, offset %d:%d",
      dec->info.pic_width, dec->info.pic_height, dec->info.pic_x,
      dec->info.pic_y);

  switch (dec->info.pixel_fmt) {
    case TH_PF_444:
      dec->output_bpp = 24;
      format = GST_VIDEO_FORMAT_Y444;
      break;
    case TH_PF_420:
      dec->output_bpp = 12;     /* Average bits per pixel. */
      format = GST_VIDEO_FORMAT_I420;
      break;
    case TH_PF_422:
      dec->output_bpp = 16;
      format = GST_VIDEO_FORMAT_Y42B;
      break;
    default:
      goto invalid_format;
  }

  if (dec->info.pic_width != dec->info.frame_width ||
      dec->info.pic_height != dec->info.frame_height ||
      dec->info.pic_x != 0 || dec->info.pic_y != 0) {
    GST_DEBUG_OBJECT (dec, "we need to crop");
    dec->need_cropping = TRUE;
  } else {
    GST_DEBUG_OBJECT (dec, "no cropping needed");
    dec->need_cropping = FALSE;
  }

  /* info contains the dimensions for the coded picture before cropping */
  gst_video_info_init (&info);
  gst_video_info_set_format (&info, format, dec->info.frame_width,
      dec->info.frame_height);
  info.fps_n = dec->info.fps_numerator;
  info.fps_d = dec->info.fps_denominator;
  /* calculate par
   * the info.aspect_* values reflect PAR;
   * 0:x and x:0 are allowed and can be interpreted as 1:1.
   */
  if (dec->have_par) {
    /* we had a par on the sink caps, override the encoded par */
    GST_DEBUG_OBJECT (dec, "overriding with input PAR %dx%d", dec->par_num,
        dec->par_den);
    info.par_n = dec->par_num;
    info.par_d = dec->par_den;
  } else {
    /* take encoded par */
    info.par_n = dec->info.aspect_numerator;
    info.par_d = dec->info.aspect_denominator;
  }
  if (info.par_n == 0 || info.par_d == 0) {
    info.par_n = info.par_d = 1;
  }

  /* these values are for all versions of the colorspace specified in the
   * theora info */
  info.chroma_site = GST_VIDEO_CHROMA_SITE_JPEG;
  info.colorimetry.range = GST_VIDEO_COLOR_RANGE_16_235;
  info.colorimetry.matrix = GST_VIDEO_COLOR_MATRIX_BT601;
  info.colorimetry.transfer = GST_VIDEO_TRANSFER_BT709;
  switch (dec->info.colorspace) {
    case TH_CS_ITU_REC_470M:
      info.colorimetry.primaries = GST_VIDEO_COLOR_PRIMARIES_BT470M;
      break;
    case TH_CS_ITU_REC_470BG:
      info.colorimetry.primaries = GST_VIDEO_COLOR_PRIMARIES_BT470BG;
      break;
    default:
      info.colorimetry.primaries = GST_VIDEO_COLOR_PRIMARIES_UNKNOWN;
      break;
  }

  /* remove reconfigure flag now */
  gst_pad_check_reconfigure (dec->srcpad);

  /* for the output caps we always take the cropped dimensions */
  cinfo = info;
  gst_video_info_set_format (&cinfo, GST_VIDEO_INFO_FORMAT (&info),
      dec->info.pic_width, dec->info.pic_height);
  caps = gst_video_info_to_caps (&cinfo);
  gst_pad_set_caps (dec->srcpad, caps);

  /* find a pool for the negotiated caps now */
  query = gst_query_new_allocation (caps, TRUE);

  if (gst_pad_peer_query (dec->srcpad, query)) {
    /* check if downstream supports cropping */
    dec->has_cropping =
        gst_query_has_allocation_meta (query, GST_VIDEO_CROP_META_API_TYPE);
  } else {
    /* not a problem, deal with defaults */
    GST_DEBUG_OBJECT (dec, "didn't get downstream ALLOCATION hints");
    dec->has_cropping = FALSE;
  }

  if (gst_query_get_n_allocation_pools (query) > 0) {
    /* we got configuration from our peer, parse them */
    gst_query_parse_nth_allocation_pool (query, 0, &pool, &size, &min, &max);
  } else {
    pool = NULL;
    size = 0;
    min = max = 0;
  }
  GST_DEBUG_OBJECT (dec, "downstream cropping %d", dec->has_cropping);

  if (pool == NULL) {
    /* we did not get a pool, make one ourselves then */
    pool = gst_video_buffer_pool_new ();
  }

  if (dec->pool)
    gst_object_unref (dec->pool);
  dec->pool = pool;

  if (dec->has_cropping) {
    dec->vinfo = info;
    /* we can crop, configure the pool with buffers of caps and size of the
     * decoded picture size and then crop them with metadata */
    gst_caps_unref (caps);
    caps = gst_video_info_to_caps (&info);
  } else {
    /* no cropping, use cropped videoinfo */
    dec->vinfo = cinfo;
  }
  size = MAX (size, GST_VIDEO_INFO_SIZE (&dec->vinfo));

  config = gst_buffer_pool_get_config (pool);
  gst_buffer_pool_config_set_params (config, caps, size, min, max);
  gst_caps_unref (caps);

  if (gst_query_has_allocation_meta (query, GST_VIDEO_META_API_TYPE)) {
    /* just set the option, if the pool can support it we will transparently use
     * it through the video info API. We could also see if the pool support this
     * option and only activate it then. */
    gst_buffer_pool_config_add_option (config,
        GST_BUFFER_POOL_OPTION_VIDEO_META);
  }

  gst_buffer_pool_set_config (pool, config);
  /* and activate */
  gst_buffer_pool_set_active (pool, TRUE);

  gst_query_unref (query);

  return GST_FLOW_OK;

  /* ERRORS */
invalid_format:
  {
    GST_ERROR_OBJECT (dec, "Invalid pixel format %d", dec->info.pixel_fmt);
    return GST_FLOW_ERROR;
  }
}

static GstFlowReturn
theora_handle_type_packet (GstTheoraDec * dec, ogg_packet * packet)
{
  GstFlowReturn ret = GST_FLOW_OK;
  GList *walk;

  if ((ret = theora_negotiate (dec)) != GST_FLOW_OK)
    goto negotiate_failed;

  /* done */
  dec->decoder = th_decode_alloc (&dec->info, dec->setup);

  if (th_decode_ctl (dec->decoder, TH_DECCTL_SET_TELEMETRY_MV,
          &dec->telemetry_mv, sizeof (dec->telemetry_mv)) != TH_EIMPL) {
    GST_WARNING_OBJECT (dec, "Could not enable MV visualisation");
  }
  if (th_decode_ctl (dec->decoder, TH_DECCTL_SET_TELEMETRY_MBMODE,
          &dec->telemetry_mbmode, sizeof (dec->telemetry_mbmode)) != TH_EIMPL) {
    GST_WARNING_OBJECT (dec, "Could not enable MB mode visualisation");
  }
  if (th_decode_ctl (dec->decoder, TH_DECCTL_SET_TELEMETRY_QI,
          &dec->telemetry_qi, sizeof (dec->telemetry_qi)) != TH_EIMPL) {
    GST_WARNING_OBJECT (dec, "Could not enable QI mode visualisation");
  }
  if (th_decode_ctl (dec->decoder, TH_DECCTL_SET_TELEMETRY_BITS,
          &dec->telemetry_bits, sizeof (dec->telemetry_bits)) != TH_EIMPL) {
    GST_WARNING_OBJECT (dec, "Could not enable BITS mode visualisation");
  }

  dec->have_header = TRUE;

  if (dec->pendingevents) {
    for (walk = dec->pendingevents; walk; walk = g_list_next (walk))
      gst_pad_push_event (dec->srcpad, GST_EVENT_CAST (walk->data));
    g_list_free (dec->pendingevents);
    dec->pendingevents = NULL;
  }

  if (dec->tags) {
    gst_pad_push_event (dec->srcpad, gst_event_new_tag (dec->tags));
    dec->tags = NULL;
  }

  return ret;

  /* ERRORS */
negotiate_failed:
  {
    GST_ERROR_OBJECT (dec, "failed to negotiate");
    return ret;
  }
}

static GstFlowReturn
theora_handle_header_packet (GstTheoraDec * dec, ogg_packet * packet)
{
  GstFlowReturn res;
  int ret;

  GST_DEBUG_OBJECT (dec, "parsing header packet");

  ret = th_decode_headerin (&dec->info, &dec->comment, &dec->setup, packet);
  if (ret < 0)
    goto header_read_error;

  switch (packet->packet[0]) {
    case 0x81:
      res = theora_handle_comment_packet (dec, packet);
      break;
    case 0x82:
      res = theora_handle_type_packet (dec, packet);
      break;
    default:
      /* ignore */
      g_warning ("unknown theora header packet found");
    case 0x80:
      /* nothing special, this is the identification header */
      res = GST_FLOW_OK;
      break;
  }
  return res;

  /* ERRORS */
header_read_error:
  {
    GST_ELEMENT_ERROR (GST_ELEMENT (dec), STREAM, DECODE,
        (NULL), ("couldn't read header packet"));
    return GST_FLOW_ERROR;
  }
}

/* returns TRUE if buffer is within segment, else FALSE.
 * if Buffer is on segment border, it's timestamp and duration will be clipped */
static gboolean
clip_buffer (GstTheoraDec * dec, GstBuffer * buf)
{
  gboolean res = TRUE;
  GstClockTime in_ts, in_dur, stop;
  guint64 cstart, cstop;

  in_ts = GST_BUFFER_TIMESTAMP (buf);
  in_dur = GST_BUFFER_DURATION (buf);

  GST_LOG_OBJECT (dec,
      "timestamp:%" GST_TIME_FORMAT " , duration:%" GST_TIME_FORMAT,
      GST_TIME_ARGS (in_ts), GST_TIME_ARGS (in_dur));

  /* can't clip without TIME segment */
  if (dec->segment.format != GST_FORMAT_TIME)
    goto beach;

  /* we need a start time */
  if (!GST_CLOCK_TIME_IS_VALID (in_ts))
    goto beach;

  /* generate valid stop, if duration unknown, we have unknown stop */
  stop =
      GST_CLOCK_TIME_IS_VALID (in_dur) ? (in_ts + in_dur) : GST_CLOCK_TIME_NONE;

  /* now clip */
  if (!(res = gst_segment_clip (&dec->segment, GST_FORMAT_TIME,
              in_ts, stop, &cstart, &cstop)))
    goto beach;

  /* update timestamp and possibly duration if the clipped stop time is
   * valid */
  GST_BUFFER_TIMESTAMP (buf) = cstart;
  if (GST_CLOCK_TIME_IS_VALID (cstop))
    GST_BUFFER_DURATION (buf) = cstop - cstart;

beach:
  GST_LOG_OBJECT (dec, "%sdropping", (res ? "not " : ""));
  return res;
}

static GstFlowReturn
theora_dec_push_forward (GstTheoraDec * dec, GstBuffer * buf)
{
  GstFlowReturn result = GST_FLOW_OK;

  if (clip_buffer (dec, buf)) {
    if (dec->discont) {
      GST_LOG_OBJECT (dec, "setting DISCONT");
      GST_BUFFER_FLAG_SET (buf, GST_BUFFER_FLAG_DISCONT);
      dec->discont = FALSE;
    }
    result = gst_pad_push (dec->srcpad, buf);
  } else {
    gst_buffer_unref (buf);
  }

  return result;
}

static GstFlowReturn
theora_dec_push_reverse (GstTheoraDec * dec, GstBuffer * buf)
{
  GstFlowReturn result = GST_FLOW_OK;

  dec->queued = g_list_prepend (dec->queued, buf);

  return result;
}

/* Allocate buffer and copy image data into Y444 format */
static GstFlowReturn
theora_handle_image (GstTheoraDec * dec, th_ycbcr_buffer buf, GstBuffer ** out)
{
  gint width, height, stride;
  gint pic_width, pic_height;
  GstFlowReturn result;
  int i, comp;
  guint8 *dest, *src;
  GstVideoFrame frame;
  GstVideoCropMeta *crop;
  gint offset_x, offset_y;

  if G_UNLIKELY
    (gst_pad_check_reconfigure (dec->srcpad))
        if G_UNLIKELY
      ((result = theora_negotiate (dec)) != GST_FLOW_OK)
          goto negotiate_failed;

  result = gst_buffer_pool_acquire_buffer (dec->pool, out, NULL);
  if (G_UNLIKELY (result != GST_FLOW_OK))
    goto no_buffer;

  if G_UNLIKELY
    (!gst_video_frame_map (&frame, &dec->vinfo, *out, GST_MAP_WRITE))
        goto invalid_frame;

  if (!dec->has_cropping) {
    /* we need to crop the hard way */
    offset_x = dec->info.pic_x;
    offset_y = dec->info.pic_y;
    pic_width = dec->info.pic_width;
    pic_height = dec->info.pic_height;
    /* Ensure correct offsets in chroma for formats that need it
     * by rounding the offset. libtheora will add proper pixels,
     * so no need to handle them ourselves. */
    if (offset_x & 1 && dec->info.pixel_fmt != TH_PF_444)
      offset_x--;
    if (offset_y & 1 && dec->info.pixel_fmt == TH_PF_420)
      offset_y--;
  } else {
    /* copy the whole frame */
    offset_x = 0;
    offset_y = 0;
    pic_width = dec->info.frame_width;
    pic_height = dec->info.frame_height;

    if (dec->has_cropping && dec->need_cropping) {
      crop = gst_buffer_add_video_crop_meta (*out);
      /* we can do things slightly more efficient when we know that
       * downstream understands clipping */
      crop->x = dec->info.pic_x;
      crop->y = dec->info.pic_y;
      crop->width = dec->info.pic_width;
      crop->height = dec->info.pic_height;
    }
  }

  /* if only libtheora would allow us to give it a destination frame */
  GST_CAT_TRACE_OBJECT (GST_CAT_PERFORMANCE, dec,
      "doing unavoidable video frame copy");

  for (comp = 0; comp < 3; comp++) {
    width =
        GST_VIDEO_FORMAT_INFO_SCALE_WIDTH (frame.info.finfo, comp, pic_width);
    height =
        GST_VIDEO_FORMAT_INFO_SCALE_HEIGHT (frame.info.finfo, comp, pic_height);
    stride = GST_VIDEO_FRAME_COMP_STRIDE (&frame, comp);
    dest = GST_VIDEO_FRAME_COMP_DATA (&frame, comp);

    src = buf[comp].data;
    src += ((height == pic_height) ? offset_y : offset_y / 2)
        * buf[comp].stride;
    src += (width == pic_width) ? offset_x : offset_x / 2;

    for (i = 0; i < height; i++) {
      memcpy (dest, src, width);

      dest += stride;
      src += buf[comp].stride;
    }
  }
  gst_video_frame_unmap (&frame);

  return GST_FLOW_OK;

  /* ERRORS */
negotiate_failed:
  {
    GST_DEBUG_OBJECT (dec, "could not negotiate, reason: %s",
        gst_flow_get_name (result));
    return result;
  }
no_buffer:
  {
    GST_DEBUG_OBJECT (dec, "could not get buffer, reason: %s",
        gst_flow_get_name (result));
    return result;
  }
invalid_frame:
  {
    GST_DEBUG_OBJECT (dec, "could not map video frame");
    return GST_FLOW_ERROR;
  }
}

static GstFlowReturn
theora_handle_data_packet (GstTheoraDec * dec, ogg_packet * packet,
    GstClockTime outtime, GstClockTime outdur)
{
  /* normal data packet */
  th_ycbcr_buffer buf;
  GstBuffer *out;
  gboolean keyframe;
  GstFlowReturn result;
  ogg_int64_t gp;

  if (G_UNLIKELY (!dec->have_header))
    goto not_initialized;

  /* get timestamp and durations */
  if (outtime == -1)
    outtime = dec->last_timestamp;
  if (outdur == -1)
    outdur = gst_util_uint64_scale_int (GST_SECOND, dec->info.fps_denominator,
        dec->info.fps_numerator);

  /* calculate expected next timestamp */
  if (outtime != -1 && outdur != -1)
    dec->last_timestamp = outtime + outdur;

  /* the second most significant bit of the first data byte is cleared 
   * for keyframes. We can only check it if it's not a zero-length packet. */
  keyframe = packet->bytes && ((packet->packet[0] & 0x40) == 0);
  if (G_UNLIKELY (keyframe)) {
    GST_DEBUG_OBJECT (dec, "we have a keyframe");
    dec->need_keyframe = FALSE;
  } else if (G_UNLIKELY (dec->need_keyframe)) {
    goto dropping;
  }

  GST_DEBUG_OBJECT (dec, "parsing data packet");

  /* this does the decoding */
  if (G_UNLIKELY (th_decode_packetin (dec->decoder, packet, &gp) < 0))
    goto decode_error;

  if (outtime != -1) {
    gboolean need_skip;
    GstClockTime running_time;
    GstClockTime earliest_time;
    gdouble proportion;

    /* qos needs to be done on running time */
    running_time = gst_segment_to_running_time (&dec->segment, GST_FORMAT_TIME,
        outtime);

    GST_OBJECT_LOCK (dec);
    proportion = dec->proportion;
    earliest_time = dec->earliest_time;
    /* check for QoS, don't perform the last steps of getting and
     * pushing the buffers that are known to be late. */
    need_skip = earliest_time != -1 && running_time <= earliest_time;
    GST_OBJECT_UNLOCK (dec);

    if (need_skip) {
      GstMessage *qos_msg;
      guint64 stream_time;
      gint64 jitter;

      GST_DEBUG_OBJECT (dec, "skipping decoding: qostime %"
          GST_TIME_FORMAT " <= %" GST_TIME_FORMAT,
          GST_TIME_ARGS (running_time), GST_TIME_ARGS (earliest_time));

      dec->dropped++;

      stream_time =
          gst_segment_to_stream_time (&dec->segment, GST_FORMAT_TIME, outtime);
      jitter = GST_CLOCK_DIFF (running_time, earliest_time);

      qos_msg =
          gst_message_new_qos (GST_OBJECT_CAST (dec), FALSE, running_time,
          stream_time, outtime, outdur);
      gst_message_set_qos_values (qos_msg, jitter, proportion, 1000000);
      gst_message_set_qos_stats (qos_msg, GST_FORMAT_BUFFERS,
          dec->processed, dec->dropped);
      gst_element_post_message (GST_ELEMENT_CAST (dec), qos_msg);

      goto dropping_qos;
    }
  }

  /* this does postprocessing and set up the decoded frame
   * pointers in our yuv variable */
  if (G_UNLIKELY (th_decode_ycbcr_out (dec->decoder, buf) < 0))
    goto no_yuv;

  if (G_UNLIKELY ((buf[0].width != dec->info.frame_width)
          || (buf[0].height != dec->info.frame_height)))
    goto wrong_dimensions;

  result = theora_handle_image (dec, buf, &out);
  if (result != GST_FLOW_OK)
    return result;

  GST_BUFFER_OFFSET (out) = dec->frame_nr;
  if (dec->frame_nr != -1)
    dec->frame_nr++;
  GST_BUFFER_OFFSET_END (out) = dec->frame_nr;

  GST_BUFFER_TIMESTAMP (out) = outtime;
  GST_BUFFER_DURATION (out) = outdur;

  dec->processed++;

  if (dec->segment.rate >= 0.0)
    result = theora_dec_push_forward (dec, out);
  else
    result = theora_dec_push_reverse (dec, out);

  return result;

  /* ERRORS */
not_initialized:
  {
    GST_ELEMENT_ERROR (GST_ELEMENT (dec), STREAM, DECODE,
        (NULL), ("no header sent yet"));
    return GST_FLOW_ERROR;
  }
dropping:
  {
    GST_WARNING_OBJECT (dec, "dropping frame because we need a keyframe");
    dec->discont = TRUE;
    return GST_FLOW_OK;
  }
dropping_qos:
  {
    if (dec->frame_nr != -1)
      dec->frame_nr++;
    dec->discont = TRUE;
    GST_WARNING_OBJECT (dec, "dropping frame because of QoS");
    return GST_FLOW_OK;
  }
decode_error:
  {
    GST_ELEMENT_ERROR (GST_ELEMENT (dec), STREAM, DECODE,
        (NULL), ("theora decoder did not decode data packet"));
    return GST_FLOW_ERROR;
  }
no_yuv:
  {
    GST_ELEMENT_ERROR (GST_ELEMENT (dec), STREAM, DECODE,
        (NULL), ("couldn't read out YUV image"));
    return GST_FLOW_ERROR;
  }
wrong_dimensions:
  {
    GST_ELEMENT_ERROR (GST_ELEMENT (dec), STREAM, FORMAT,
        (NULL), ("dimensions of image do not match header"));
    return GST_FLOW_ERROR;
  }
}

static GstFlowReturn
theora_dec_decode_buffer (GstTheoraDec * dec, GstBuffer * buf)
{
  ogg_packet packet;
  GstFlowReturn result = GST_FLOW_OK;
  GstClockTime timestamp, duration;
  GstMapInfo map;

  /* make ogg_packet out of the buffer */
  gst_buffer_map (buf, &map, GST_MAP_READ);
  packet.packet = map.data;
  packet.bytes = map.size;
  packet.granulepos = -1;
  packet.packetno = 0;          /* we don't really care */
  packet.b_o_s = dec->have_header ? 0 : 1;
  /* EOS does not matter for the decoder */
  packet.e_o_s = 0;

  GST_LOG_OBJECT (dec, "decode buffer of size %ld", packet.bytes);

  /* save last seem timestamp for interpolating the next timestamps using the
   * framerate when we need to */
  timestamp = GST_BUFFER_TIMESTAMP (buf);
  duration = GST_BUFFER_DURATION (buf);

  GST_DEBUG_OBJECT (dec, "header=%02x, outtime=%" GST_TIME_FORMAT,
      packet.bytes ? packet.packet[0] : -1, GST_TIME_ARGS (timestamp));

  /* switch depending on packet type. A zero byte packet is always a data
   * packet; we don't dereference it in that case. */
  if (packet.bytes && packet.packet[0] & 0x80) {
    if (dec->have_header) {
      GST_WARNING_OBJECT (GST_OBJECT (dec), "Ignoring header");
      goto done;
    }
    result = theora_handle_header_packet (dec, &packet);
  } else {
    result = theora_handle_data_packet (dec, &packet, timestamp, duration);
  }
done:
  gst_buffer_unmap (buf, &map);

  return result;
}

/* For reverse playback we use a technique that can be used for
 * any keyframe based video codec.
 *
 * Input:
 *  Buffer decoding order:  7  8  9  4  5  6  1  2  3  EOS
 *  Keyframe flag:                      K        K
 *  Discont flag:           D        D        D
 *
 * - Each Discont marks a discont in the decoding order.
 * - The keyframes mark where we can start decoding.
 *
 * First we prepend incomming buffers to the gather queue, whenever we receive
 * a discont, we flush out the gather queue.
 *
 * The above data will be accumulated in the gather queue like this:
 *
 *   gather queue:  9  8  7
 *                        D
 *
 * Whe buffer 4 is received (with a DISCONT), we flush the gather queue like
 * this:
 *
 *   while (gather)
 *     take head of queue and prepend to decode queue.
 *     if we copied a keyframe, decode the decode queue.
 *
 * After we flushed the gather queue, we add 4 to the (now empty) gather queue.
 * We get the following situation:
 *
 *  gather queue:    4
 *  decode queue:    7  8  9
 *
 * After we received 5 (Keyframe) and 6:
 *
 *  gather queue:    6  5  4
 *  decode queue:    7  8  9
 *
 * When we receive 1 (DISCONT) which triggers a flush of the gather queue:
 *
 *   Copy head of the gather queue (6) to decode queue:
 *
 *    gather queue:    5  4
 *    decode queue:    6  7  8  9
 *
 *   Copy head of the gather queue (5) to decode queue. This is a keyframe so we
 *   can start decoding.
 *
 *    gather queue:    4
 *    decode queue:    5  6  7  8  9
 *
 *   Decode frames in decode queue, store raw decoded data in output queue, we
 *   can take the head of the decode queue and prepend the decoded result in the
 *   output queue:
 *
 *    gather queue:    4
 *    decode queue:    
 *    output queue:    9  8  7  6  5
 *
 *   Now output all the frames in the output queue, picking a frame from the
 *   head of the queue.
 *
 *   Copy head of the gather queue (4) to decode queue, we flushed the gather
 *   queue and can now store input buffer in the gather queue:
 *
 *    gather queue:    1
 *    decode queue:    4
 *
 *  When we receive EOS, the queue looks like:
 *
 *    gather queue:    3  2  1
 *    decode queue:    4
 *
 *  Fill decode queue, first keyframe we copy is 2:
 *
 *    gather queue:    1
 *    decode queue:    2  3  4
 *
 *  Decoded output:
 *
 *    gather queue:    1
 *    decode queue:    
 *    output queue:    4  3  2
 *
 *  Leftover buffer 1 cannot be decoded and must be discarded.
 */
static GstFlowReturn
theora_dec_flush_decode (GstTheoraDec * dec)
{
  GstFlowReturn res = GST_FLOW_OK;

  while (dec->decode) {
    GstBuffer *buf = GST_BUFFER_CAST (dec->decode->data);

    GST_DEBUG_OBJECT (dec, "decoding buffer %p, ts %" GST_TIME_FORMAT,
        buf, GST_TIME_ARGS (GST_BUFFER_TIMESTAMP (buf)));

    /* decode buffer, prepend to output queue */
    res = theora_dec_decode_buffer (dec, buf);

    /* don't need it anymore now */
    gst_buffer_unref (buf);

    dec->decode = g_list_delete_link (dec->decode, dec->decode);
  }
  while (dec->queued) {
    GstBuffer *buf = GST_BUFFER_CAST (dec->queued->data);

    /* iterate output queue an push downstream */
    res = gst_pad_push (dec->srcpad, buf);

    dec->queued = g_list_delete_link (dec->queued, dec->queued);
  }

  return res;
}

static GstFlowReturn
theora_dec_chain_reverse (GstTheoraDec * dec, gboolean discont, GstBuffer * buf)
{
  GstFlowReturn res = GST_FLOW_OK;

  /* if we have a discont, move buffers to the decode list */
  if (G_UNLIKELY (discont)) {
    GST_DEBUG_OBJECT (dec, "received discont,gathering buffers");
    while (dec->gather) {
      GstBuffer *gbuf;
      guint8 data[1];

      gbuf = GST_BUFFER_CAST (dec->gather->data);
      /* remove from the gather list */
      dec->gather = g_list_delete_link (dec->gather, dec->gather);
      /* copy to decode queue */
      dec->decode = g_list_prepend (dec->decode, gbuf);

      /* if we copied a keyframe, flush and decode the decode queue */
      if (gst_buffer_extract (gbuf, 0, data, 1) == 1) {
        if ((data[0] & 0x40) == 0) {
          GST_DEBUG_OBJECT (dec, "copied keyframe");
          res = theora_dec_flush_decode (dec);
        }
      }
    }
  }

  /* add buffer to gather queue */
  GST_DEBUG_OBJECT (dec, "gathering buffer %p, size %" G_GSIZE_FORMAT, buf,
      gst_buffer_get_size (buf));
  dec->gather = g_list_prepend (dec->gather, buf);

  return res;
}

static GstFlowReturn
theora_dec_chain_forward (GstTheoraDec * dec, gboolean discont,
    GstBuffer * buffer)
{
  GstFlowReturn result;

  result = theora_dec_decode_buffer (dec, buffer);

  gst_buffer_unref (buffer);

  return result;
}

static GstFlowReturn
theora_dec_chain (GstPad * pad, GstObject * parent, GstBuffer * buf)
{
  GstTheoraDec *dec;
  GstFlowReturn res;
  gboolean discont;

  dec = GST_THEORA_DEC (parent);

  /* peel of DISCONT flag */
  discont = GST_BUFFER_IS_DISCONT (buf);

  /* resync on DISCONT */
  if (G_UNLIKELY (discont)) {
    GST_DEBUG_OBJECT (dec, "received DISCONT buffer");
    dec->need_keyframe = TRUE;
    dec->last_timestamp = -1;
    dec->discont = TRUE;
  }

  if (dec->segment.rate > 0.0)
    res = theora_dec_chain_forward (dec, discont, buf);
  else
    res = theora_dec_chain_reverse (dec, discont, buf);

  return res;
}

static GstStateChangeReturn
theora_dec_change_state (GstElement * element, GstStateChange transition)
{
  GstTheoraDec *dec = GST_THEORA_DEC (element);
  GstStateChangeReturn ret;

  switch (transition) {
    case GST_STATE_CHANGE_NULL_TO_READY:
      break;
    case GST_STATE_CHANGE_READY_TO_PAUSED:
      th_info_clear (&dec->info);
      th_comment_clear (&dec->comment);
      GST_DEBUG_OBJECT (dec, "Setting have_header to FALSE in READY->PAUSED");
      dec->have_header = FALSE;
      dec->have_par = FALSE;
      gst_theora_dec_reset (dec);
      break;
    case GST_STATE_CHANGE_PAUSED_TO_PLAYING:
      break;
    default:
      break;
  }

  ret = GST_ELEMENT_CLASS (parent_class)->change_state (element, transition);

  switch (transition) {
    case GST_STATE_CHANGE_PLAYING_TO_PAUSED:
      break;
    case GST_STATE_CHANGE_PAUSED_TO_READY:
      th_info_clear (&dec->info);
      th_comment_clear (&dec->comment);
      th_setup_free (dec->setup);
      dec->setup = NULL;
      th_decode_free (dec->decoder);
      dec->decoder = NULL;
      gst_theora_dec_reset (dec);
      if (dec->pool) {
        gst_buffer_pool_set_active (dec->pool, FALSE);
        gst_object_unref (dec->pool);
        dec->pool = NULL;
      }
      break;
    case GST_STATE_CHANGE_READY_TO_NULL:
      break;
    default:
      break;
  }

  return ret;
}

static void
theora_dec_set_property (GObject * object, guint prop_id,
    const GValue * value, GParamSpec * pspec)
{
  GstTheoraDec *dec = GST_THEORA_DEC (object);

  switch (prop_id) {
    case PROP_TELEMETRY_MV:
      dec->telemetry_mv = g_value_get_int (value);
      break;
    case PROP_TELEMETRY_MBMODE:
      dec->telemetry_mbmode = g_value_get_int (value);
      break;
    case PROP_TELEMETRY_QI:
      dec->telemetry_qi = g_value_get_int (value);
      break;
    case PROP_TELEMETRY_BITS:
      dec->telemetry_bits = g_value_get_int (value);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

static void
theora_dec_get_property (GObject * object, guint prop_id,
    GValue * value, GParamSpec * pspec)
{
  GstTheoraDec *dec = GST_THEORA_DEC (object);

  switch (prop_id) {
    case PROP_TELEMETRY_MV:
      g_value_set_int (value, dec->telemetry_mv);
      break;
    case PROP_TELEMETRY_MBMODE:
      g_value_set_int (value, dec->telemetry_mbmode);
      break;
    case PROP_TELEMETRY_QI:
      g_value_set_int (value, dec->telemetry_qi);
      break;
    case PROP_TELEMETRY_BITS:
      g_value_set_int (value, dec->telemetry_bits);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}
