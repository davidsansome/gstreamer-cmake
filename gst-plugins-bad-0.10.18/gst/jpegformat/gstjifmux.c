/* GStreamer
 *
 * jifmux: JPEG interchange format muxer
 *
 * Copyright (C) 2010 Stefan Kost <stefan.kost@nokia.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

/**
 * SECTION:element-jifmux
 * @short_description: JPEG interchange format writer
 *
 * Writes a JPEG image as JPEG/EXIF or JPEG/JFIF including various metadata.
 *
 * <refsect2>
 * <title>Example launch line</title>
 * |[
 * gst-launch -v videotestsrc num-buffers=1 ! jpegenc ! jifmux ! filesink location=...
 * ]|
 * The above pipeline renders a frame, encodes to jpeg, adds metadata and writes
 * it to disk.
 * </refsect2>
 */
/*
jpeg interchange format:
file header : SOI, APPn{JFIF,EXIF,...}
frame header: DQT, SOF
scan header : {DAC,DHT},DRI,SOS
<scan data>
file trailer: EOI

tests:
gst-launch videotestsrc num-buffers=1 ! jpegenc ! jifmux ! filesink location=test1.jpeg
gst-launch videotestsrc num-buffers=1 ! jpegenc ! taginject tags="comment=\"test image\"" ! jifmux ! filesink location=test2.jpeg
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <string.h>
#include <gst/base/gstbytereader.h>
#include <gst/base/gstbytewriter.h>

#include "gstjifmux.h"

static GstStaticPadTemplate gst_jif_mux_src_pad_template =
GST_STATIC_PAD_TEMPLATE ("src",
    GST_PAD_SRC,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS ("image/jpeg")
    );

static GstStaticPadTemplate gst_jif_mux_sink_pad_template =
GST_STATIC_PAD_TEMPLATE ("sink",
    GST_PAD_SINK,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS ("image/jpeg")
    );

GST_DEBUG_CATEGORY_STATIC (jif_mux_debug);
#define GST_CAT_DEFAULT jif_mux_debug

typedef struct _GstJifMuxMarker
{
  guint8 marker;
  guint16 size;

  const guint8 *data;
  gboolean owned;
} GstJifMuxMarker;

struct _GstJifMuxPrivate
{
  GstPad *srcpad;

  /* list of GstJifMuxMarker */
  GList *markers;
  guint16 scan_size;
  const guint8 *scan_data;
};

static void gst_jif_mux_base_init (gpointer g_class);
static void gst_jif_mux_class_init (GstJifMuxClass * klass);
static void gst_jif_mux_finalize (GObject * object);

static void gst_jif_mux_reset (GstJifMux * self);
static gboolean gst_jif_mux_sink_setcaps (GstPad * pad, GstCaps * caps);
static gboolean gst_jif_mux_sink_event (GstPad * pad, GstEvent * event);
static GstFlowReturn gst_jif_mux_chain (GstPad * pad, GstBuffer * buffer);


static void
gst_jif_type_init (GType type)
{
  static const GInterfaceInfo tag_setter_info = { NULL, NULL, NULL };

  g_type_add_interface_static (type, GST_TYPE_TAG_SETTER, &tag_setter_info);

  GST_DEBUG_CATEGORY_INIT (jif_mux_debug, "jifmux", 0,
      "JPEG interchange format muxer");
}

GST_BOILERPLATE_FULL (GstJifMux, gst_jif_mux, GstElement, GST_TYPE_ELEMENT,
    gst_jif_type_init);

static void
gst_jif_mux_base_init (gpointer g_class)
{
  GstElementClass *element_class = GST_ELEMENT_CLASS (g_class);

  gst_element_class_add_pad_template (element_class,
      gst_static_pad_template_get (&gst_jif_mux_src_pad_template));
  gst_element_class_add_pad_template (element_class,
      gst_static_pad_template_get (&gst_jif_mux_sink_pad_template));
  gst_element_class_set_details_simple (element_class,
      "JPEG stream parser",
      "Codec/Parser/Video",
      "Parse JPEG images into single-frame buffers",
      "Arnout Vandecappelle (Essensium/Mind) <arnout@mind.be>");
}

static void
gst_jif_mux_class_init (GstJifMuxClass * klass)
{
  GObjectClass *gobject_class;

  gobject_class = (GObjectClass *) klass;

  g_type_class_add_private (gobject_class, sizeof (GstJifMuxPrivate));

  gobject_class->finalize = gst_jif_mux_finalize;
}

static void
gst_jif_mux_init (GstJifMux * self, GstJifMuxClass * g_class)
{
  GstPad *sinkpad;

  self->priv = G_TYPE_INSTANCE_GET_PRIVATE (self, GST_TYPE_JIF_MUX,
      GstJifMuxPrivate);

  /* create the sink and src pads */
  sinkpad = gst_pad_new_from_static_template (&gst_jif_mux_sink_pad_template,
      "sink");
  gst_pad_set_chain_function (sinkpad, GST_DEBUG_FUNCPTR (gst_jif_mux_chain));
  gst_pad_set_setcaps_function (sinkpad,
      GST_DEBUG_FUNCPTR (gst_jif_mux_sink_setcaps));
  gst_pad_set_event_function (sinkpad,
      GST_DEBUG_FUNCPTR (gst_jif_mux_sink_event));
  gst_element_add_pad (GST_ELEMENT (self), sinkpad);

  self->priv->srcpad =
      gst_pad_new_from_static_template (&gst_jif_mux_src_pad_template, "src");
  gst_element_add_pad (GST_ELEMENT (self), self->priv->srcpad);
}

static void
gst_jif_mux_finalize (GObject * object)
{
  GstJifMux *self = GST_JIF_MUX (object);

  gst_jif_mux_reset (self);
  G_OBJECT_CLASS (parent_class)->finalize (object);
}

static gboolean
gst_jif_mux_sink_setcaps (GstPad * pad, GstCaps * caps)
{
  GstJifMux *self = GST_JIF_MUX (GST_PAD_PARENT (pad));
  GstStructure *s = gst_caps_get_structure (caps, 0);
  const gchar *variant;

  /* should be {combined (default), EXIF, JFIF} */
  if ((variant = gst_structure_get_string (s, "variant")) != NULL) {
    GST_INFO_OBJECT (self, "muxing to '%s'", variant);
    /* FIXME: do we want to switch it like this or use a gobject property ? */
  }

  return TRUE;
}

static gboolean
gst_jif_mux_sink_event (GstPad * pad, GstEvent * event)
{
  GstJifMux *self = GST_JIF_MUX (GST_PAD_PARENT (pad));
  gboolean ret;

  switch (GST_EVENT_TYPE (event)) {
    case GST_EVENT_TAG:{
      GstTagList *list;
      GstTagSetter *setter = GST_TAG_SETTER (self);
      const GstTagMergeMode mode = gst_tag_setter_get_tag_merge_mode (setter);

      gst_event_parse_tag (event, &list);
      gst_tag_setter_merge_tags (setter, list, mode);
      break;
    }
    default:
      break;
  }
  ret = gst_pad_event_default (pad, event);
  return ret;
}

static void
gst_jif_mux_reset (GstJifMux * self)
{
  GList *node;
  GstJifMuxMarker *m;

  for (node = self->priv->markers; node; node = g_list_next (node)) {
    m = (GstJifMuxMarker *) node->data;

    if (m->owned)
      g_free ((gpointer) m->data);

    g_slice_free (GstJifMuxMarker, m);
  }
  g_list_free (self->priv->markers);
  self->priv->markers = NULL;
}

static GstJifMuxMarker *
gst_jif_mux_new_marker (guint8 marker, guint16 size, const guint8 * data,
    gboolean owned)
{
  GstJifMuxMarker *m = g_slice_new (GstJifMuxMarker);

  m->marker = marker;
  m->size = size;
  m->data = data;
  m->owned = owned;

  return m;
}

static gboolean
gst_jif_mux_parse_image (GstJifMux * self, GstBuffer * buf)
{
  GstByteReader reader = GST_BYTE_READER_INIT_FROM_BUFFER (buf);
  GstJifMuxMarker *m;
  guint8 marker;
  guint16 size;
  const guint8 *data;

  if (!gst_byte_reader_peek_uint8 (&reader, &marker))
    goto error;

  while (marker == 0xff) {
    if (!gst_byte_reader_skip (&reader, 1))
      goto error;

    if (!gst_byte_reader_get_uint8 (&reader, &marker))
      goto error;

    switch (marker) {
      case RST0:
      case RST1:
      case RST2:
      case RST3:
      case RST4:
      case RST5:
      case RST6:
      case RST7:
      case SOI:
        GST_DEBUG_OBJECT (self, "marker = %x", marker);
        m = gst_jif_mux_new_marker (marker, 0, NULL, FALSE);
        self->priv->markers = g_list_prepend (self->priv->markers, m);
        break;
      case EOI:
        GST_DEBUG_OBJECT (self, "marker = %x", marker);
        m = gst_jif_mux_new_marker (marker, 0, NULL, FALSE);
        self->priv->markers = g_list_prepend (self->priv->markers, m);
        goto done;
        break;
      default:
        if (!gst_byte_reader_get_uint16_be (&reader, &size))
          goto error;
        if (!gst_byte_reader_get_data (&reader, size - 2, &data))
          goto error;
        m = gst_jif_mux_new_marker (marker, size - 2, data, FALSE);
        self->priv->markers = g_list_prepend (self->priv->markers, m);

        GST_DEBUG_OBJECT (self, "marker = %2x, size = %u", marker, size);
        break;
    }

    if (marker == SOS) {
      /* remaining size except EOI is scan data */
      self->priv->scan_size = GST_BUFFER_SIZE (buf) - 4 -
          gst_byte_reader_get_pos (&reader);
      if (!gst_byte_reader_get_data (&reader, self->priv->scan_size,
              &self->priv->scan_data))
        goto error;

      GST_DEBUG_OBJECT (self, "scan data, size = %u", self->priv->scan_size);
    }

    if (!gst_byte_reader_peek_uint8 (&reader, &marker))
      goto error;
  }
  GST_INFO_OBJECT (self, "done parsing at 0x%x / 0x%x",
      gst_byte_reader_get_pos (&reader), GST_BUFFER_SIZE (buf));

done:
  self->priv->markers = g_list_reverse (self->priv->markers);
  return TRUE;

error:
  GST_WARNING_OBJECT (self,
      "Error parsing image header (need more that %u bytes available)",
      gst_byte_reader_get_remaining (&reader));
  return FALSE;
}

static gboolean
gst_jif_mux_mangle_markers (GstJifMux * self)
{
  gboolean modified = FALSE;
  const GstTagList *tags;
  GstJifMuxMarker *m;
  GList *node, *file_hdr = NULL, *frame_hdr = NULL, *scan_hdr = NULL;

  /* FIXME: implement me more
   * - update the APP markers
   *   - put any JFIF APP0 first
   *   - the Exif APP1 next, 
   *   - the XMP APP1 next, 
   *   - the PSIR APP13 next,
   *   - followed by all other marker segments
   */

  /* find some reference points where we insert before/after */
  file_hdr = self->priv->markers;
  for (node = self->priv->markers; node; node = g_list_next (node)) {
    m = (GstJifMuxMarker *) node->data;

    switch (m->marker) {
      case DQT:
      case SOF0:
      case SOF1:
      case SOF2:
      case SOF3:
      case SOF5:
      case SOF6:
      case SOF7:
      case SOF9:
      case SOF10:
      case SOF11:
      case SOF13:
      case SOF14:
      case SOF15:
        if (!frame_hdr)
          frame_hdr = node;
        break;
      case DAC:
      case DHT:
      case DRI:
      case SOS:
        if (!scan_hdr)
          scan_hdr = node;
        break;
    }
  }

  /* if we want combined or JFIF */
  /* check if we don't have JFIF APP0 */
  /* insert into self->markers list */
  /* ensure its first */
  /* else */
  /* remove JFIF if exists */

  /* if we want combined or EXIF */
  /* check if we don't have EXIF APP1 */
  /* insert into self->markers list */
  /* else */
  /* remove EXIF if exists */

  /* add jpeg comment */
  tags = gst_tag_setter_get_tag_list (GST_TAG_SETTER (self));
  if (tags) {
    gchar *str = NULL;

    (void) (gst_tag_list_get_string (tags, GST_TAG_COMMENT, &str) ||
        gst_tag_list_get_string (tags, GST_TAG_DESCRIPTION, &str) ||
        gst_tag_list_get_string (tags, GST_TAG_TITLE, &str));

    if (str) {
      /* insert new marker into self->markers list */
      m = gst_jif_mux_new_marker (COM, strlen (str) + 1, (const guint8 *) str,
          TRUE);
      /* this should go before SOS, maybe at the end of file-header */
      self->priv->markers = g_list_insert_before (self->priv->markers,
          frame_hdr, m);

      modified = TRUE;
    }
  }
  return modified;
}

static GstFlowReturn
gst_jif_mux_recombine_image (GstJifMux * self, GstBuffer ** new_buf,
    GstBuffer * old_buf)
{
  GstBuffer *buf;
  GstByteWriter *writer;
  GstFlowReturn fret;
  GstJifMuxMarker *m;
  GList *node;
  guint size = self->priv->scan_size;

  /* iterate list and collect size */
  for (node = self->priv->markers; node; node = g_list_next (node)) {
    m = (GstJifMuxMarker *) node->data;

    /* some markers like e.g. SOI are empty */
    if (m->size) {
      size += 2 + m->size;
    }
    /* 0xff <marker> */
    size += 2;
  }
  GST_INFO_OBJECT (self, "old size: %u, new size: %u",
      GST_BUFFER_SIZE (old_buf), size);

  /* allocate new buffer */
  fret = gst_pad_alloc_buffer_and_set_caps (self->priv->srcpad,
      GST_BUFFER_OFFSET (old_buf), size, GST_PAD_CAPS (self->priv->srcpad),
      &buf);
  if (fret != GST_FLOW_OK)
    goto no_buffer;

  /* copy buffer metadata */
  gst_buffer_copy_metadata (buf, old_buf,
      GST_BUFFER_COPY_FLAGS | GST_BUFFER_COPY_TIMESTAMPS);

  /* memcopy markers */
  writer = gst_byte_writer_new_with_buffer (buf, TRUE);
  for (node = self->priv->markers; node; node = g_list_next (node)) {
    m = (GstJifMuxMarker *) node->data;

    gst_byte_writer_put_uint8 (writer, 0xff);
    gst_byte_writer_put_uint8 (writer, m->marker);

    GST_DEBUG_OBJECT (self, "marker = %2x, size = %u", m->marker, m->size + 2);

    if (m->size) {
      gst_byte_writer_put_uint16_be (writer, m->size + 2);
      gst_byte_writer_put_data (writer, m->data, m->size);
    }

    if (m->marker == SOS) {
      GST_DEBUG_OBJECT (self, "scan data, size = %u", self->priv->scan_size);
      gst_byte_writer_put_data (writer, self->priv->scan_data,
          self->priv->scan_size);
    }
  }
  gst_byte_writer_free (writer);

  *new_buf = buf;
  return GST_FLOW_OK;

no_buffer:
  GST_WARNING_OBJECT (self, "failed to allocate output buffer, flow_ret = %s",
      gst_flow_get_name (fret));
  return fret;
}

static GstFlowReturn
gst_jif_mux_chain (GstPad * pad, GstBuffer * buf)
{
  GstJifMux *self = GST_JIF_MUX (GST_PAD_PARENT (pad));
  guint8 *data = GST_BUFFER_DATA (buf);
  GstFlowReturn fret = GST_FLOW_OK;

  GST_MEMDUMP ("jpeg beg", data, 64);
  GST_MEMDUMP ("jpeg end", &data[GST_BUFFER_SIZE (buf) - 64], 64);

  /* we should have received a whole picture from SOI to EOI
   * build a list of markers */
  if (gst_jif_mux_parse_image (self, buf)) {
    /* modify marker list */
    if (gst_jif_mux_mangle_markers (self)) {
      /* the list was changed, remux */
      GstBuffer *old = buf;
      fret = gst_jif_mux_recombine_image (self, &buf, old);
      gst_buffer_unref (old);
    }
  }

  /* free the marker list */
  gst_jif_mux_reset (self);

  if (fret == GST_FLOW_OK) {
    fret = gst_pad_push (self->priv->srcpad, buf);
  }
  return fret;
}
