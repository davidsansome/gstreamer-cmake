/* GStreamer
 * Copyright (C) <1999> Erik Walthinsen <omega@cse.ogi.edu>
 * Copyright (C) <2002> David A. Schleef <ds@schleef.org>
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
 * SECTION:element-videotestsrc
 *
 * The videotestsrc element is used to produce test video data in a wide variety
 * of formats. The video test data produced can be controlled with the "pattern"
 * property.
 *
 * <refsect2>
 * <title>Example launch line</title>
 * |[
 * gst-launch -v videotestsrc pattern=snow ! ximagesink
 * ]| Shows random noise in an X window.
 * </refsect2>
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include "gstvideotestsrc.h"
#include "gstvideotestsrcorc.h"
#include "videotestsrc.h"

#include <string.h>
#include <stdlib.h>

GST_DEBUG_CATEGORY_STATIC (video_test_src_debug);
#define GST_CAT_DEFAULT video_test_src_debug

#define DEFAULT_PATTERN            GST_VIDEO_TEST_SRC_SMPTE
#define DEFAULT_TIMESTAMP_OFFSET   0
#define DEFAULT_IS_LIVE            FALSE
#define DEFAULT_COLOR_SPEC         GST_VIDEO_TEST_SRC_BT601
#define DEFAULT_FOREGROUND_COLOR   0xffffffff
#define DEFAULT_BACKGROUND_COLOR   0xff000000
#define DEFAULT_HORIZONTAL_SPEED   0

enum
{
  PROP_0,
  PROP_PATTERN,
  PROP_TIMESTAMP_OFFSET,
  PROP_IS_LIVE,
  PROP_K0,
  PROP_KX,
  PROP_KY,
  PROP_KT,
  PROP_KXT,
  PROP_KYT,
  PROP_KXY,
  PROP_KX2,
  PROP_KY2,
  PROP_KT2,
  PROP_XOFFSET,
  PROP_YOFFSET,
  PROP_FOREGROUND_COLOR,
  PROP_BACKGROUND_COLOR,
  PROP_HORIZONTAL_SPEED,
  PROP_LAST
};


#define gst_video_test_src_parent_class parent_class
G_DEFINE_TYPE (GstVideoTestSrc, gst_video_test_src, GST_TYPE_PUSH_SRC);

static void gst_video_test_src_set_pattern (GstVideoTestSrc * videotestsrc,
    int pattern_type);
static void gst_video_test_src_set_property (GObject * object, guint prop_id,
    const GValue * value, GParamSpec * pspec);
static void gst_video_test_src_get_property (GObject * object, guint prop_id,
    GValue * value, GParamSpec * pspec);

static GstCaps *gst_video_test_src_getcaps (GstBaseSrc * bsrc,
    GstCaps * filter);
static gboolean gst_video_test_src_setcaps (GstBaseSrc * bsrc, GstCaps * caps);
static GstCaps *gst_video_test_src_src_fixate (GstBaseSrc * bsrc,
    GstCaps * caps);

static gboolean gst_video_test_src_is_seekable (GstBaseSrc * psrc);
static gboolean gst_video_test_src_do_seek (GstBaseSrc * bsrc,
    GstSegment * segment);
static gboolean gst_video_test_src_query (GstBaseSrc * bsrc, GstQuery * query);

static void gst_video_test_src_get_times (GstBaseSrc * basesrc,
    GstBuffer * buffer, GstClockTime * start, GstClockTime * end);
static gboolean gst_video_test_src_decide_allocation (GstBaseSrc * bsrc,
    GstQuery * query);
static GstFlowReturn gst_video_test_src_fill (GstPushSrc * psrc,
    GstBuffer * buffer);
static gboolean gst_video_test_src_start (GstBaseSrc * basesrc);
static gboolean gst_video_test_src_stop (GstBaseSrc * basesrc);

#define GST_TYPE_VIDEO_TEST_SRC_PATTERN (gst_video_test_src_pattern_get_type ())
static GType
gst_video_test_src_pattern_get_type (void)
{
  static GType video_test_src_pattern_type = 0;
  static const GEnumValue pattern_types[] = {
    {GST_VIDEO_TEST_SRC_SMPTE, "SMPTE 100% color bars", "smpte"},
    {GST_VIDEO_TEST_SRC_SNOW, "Random (television snow)", "snow"},
    {GST_VIDEO_TEST_SRC_BLACK, "100% Black", "black"},
    {GST_VIDEO_TEST_SRC_WHITE, "100% White", "white"},
    {GST_VIDEO_TEST_SRC_RED, "Red", "red"},
    {GST_VIDEO_TEST_SRC_GREEN, "Green", "green"},
    {GST_VIDEO_TEST_SRC_BLUE, "Blue", "blue"},
    {GST_VIDEO_TEST_SRC_CHECKERS1, "Checkers 1px", "checkers-1"},
    {GST_VIDEO_TEST_SRC_CHECKERS2, "Checkers 2px", "checkers-2"},
    {GST_VIDEO_TEST_SRC_CHECKERS4, "Checkers 4px", "checkers-4"},
    {GST_VIDEO_TEST_SRC_CHECKERS8, "Checkers 8px", "checkers-8"},
    {GST_VIDEO_TEST_SRC_CIRCULAR, "Circular", "circular"},
    {GST_VIDEO_TEST_SRC_BLINK, "Blink", "blink"},
    {GST_VIDEO_TEST_SRC_SMPTE75, "SMPTE 75% color bars", "smpte75"},
    {GST_VIDEO_TEST_SRC_ZONE_PLATE, "Zone plate", "zone-plate"},
    {GST_VIDEO_TEST_SRC_GAMUT, "Gamut checkers", "gamut"},
    {GST_VIDEO_TEST_SRC_CHROMA_ZONE_PLATE, "Chroma zone plate",
        "chroma-zone-plate"},
    {GST_VIDEO_TEST_SRC_SOLID, "Solid color", "solid-color"},
    {GST_VIDEO_TEST_SRC_BALL, "Moving ball", "ball"},
    {GST_VIDEO_TEST_SRC_SMPTE100, "SMPTE 100% color bars", "smpte100"},
    {GST_VIDEO_TEST_SRC_BAR, "Bar", "bar"},
    {0, NULL, NULL}
  };

  if (!video_test_src_pattern_type) {
    video_test_src_pattern_type =
        g_enum_register_static ("GstVideoTestSrcPattern", pattern_types);
  }
  return video_test_src_pattern_type;
}

static void
gst_video_test_src_class_init (GstVideoTestSrcClass * klass)
{
  GObjectClass *gobject_class;
  GstElementClass *gstelement_class;
  GstBaseSrcClass *gstbasesrc_class;
  GstPushSrcClass *gstpushsrc_class;
  GstCaps *templ_caps;

  gobject_class = (GObjectClass *) klass;
  gstelement_class = (GstElementClass *) klass;
  gstbasesrc_class = (GstBaseSrcClass *) klass;
  gstpushsrc_class = (GstPushSrcClass *) klass;

  gobject_class->set_property = gst_video_test_src_set_property;
  gobject_class->get_property = gst_video_test_src_get_property;

  g_object_class_install_property (gobject_class, PROP_PATTERN,
      g_param_spec_enum ("pattern", "Pattern",
          "Type of test pattern to generate", GST_TYPE_VIDEO_TEST_SRC_PATTERN,
          DEFAULT_PATTERN, G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
  g_object_class_install_property (gobject_class, PROP_TIMESTAMP_OFFSET,
      g_param_spec_int64 ("timestamp-offset", "Timestamp offset",
          "An offset added to timestamps set on buffers (in ns)", G_MININT64,
          G_MAXINT64, 0, G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
  g_object_class_install_property (gobject_class, PROP_IS_LIVE,
      g_param_spec_boolean ("is-live", "Is Live",
          "Whether to act as a live source", DEFAULT_IS_LIVE,
          G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
  g_object_class_install_property (gobject_class, PROP_K0,
      g_param_spec_int ("k0", "Zoneplate zero order phase",
          "Zoneplate zero order phase, for generating plain fields or phase offsets",
          G_MININT32, G_MAXINT32, 0,
          G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
  g_object_class_install_property (gobject_class, PROP_KX,
      g_param_spec_int ("kx", "Zoneplate 1st order x phase",
          "Zoneplate 1st order x phase, for generating constant horizontal frequencies",
          G_MININT32, G_MAXINT32, 0,
          G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
  g_object_class_install_property (gobject_class, PROP_KY,
      g_param_spec_int ("ky", "Zoneplate 1st order y phase",
          "Zoneplate 1st order y phase, for generating contant vertical frequencies",
          G_MININT32, G_MAXINT32, 0,
          G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
  g_object_class_install_property (gobject_class, PROP_KT,
      g_param_spec_int ("kt", "Zoneplate 1st order t phase",
          "Zoneplate 1st order t phase, for generating phase rotation as a function of time",
          G_MININT32, G_MAXINT32, 0,
          G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
  g_object_class_install_property (gobject_class, PROP_KXT,
      g_param_spec_int ("kxt", "Zoneplate x*t product phase",
          "Zoneplate x*t product phase, normalised to kxy/256 cycles per vertical pixel at width/2 from origin",
          G_MININT32, G_MAXINT32, 0,
          G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
  g_object_class_install_property (gobject_class, PROP_KYT,
      g_param_spec_int ("kyt", "Zoneplate y*t product phase",
          "Zoneplate y*t product phase", G_MININT32, G_MAXINT32, 0,
          G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
  g_object_class_install_property (gobject_class, PROP_KXY,
      g_param_spec_int ("kxy", "Zoneplate x*y product phase",
          "Zoneplate x*y product phase", G_MININT32, G_MAXINT32, 0,
          G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
  g_object_class_install_property (gobject_class, PROP_KX2,
      g_param_spec_int ("kx2", "Zoneplate 2nd order x phase",
          "Zoneplate 2nd order x phase, normalised to kx2/256 cycles per horizontal pixel at width/2 from origin",
          G_MININT32, G_MAXINT32, 0,
          G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
  g_object_class_install_property (gobject_class, PROP_KY2,
      g_param_spec_int ("ky2", "Zoneplate 2nd order y phase",
          "Zoneplate 2nd order y phase, normailsed to ky2/256 cycles per vertical pixel at height/2 from origin",
          G_MININT32, G_MAXINT32, 0,
          G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
  g_object_class_install_property (gobject_class, PROP_KT2,
      g_param_spec_int ("kt2", "Zoneplate 2nd order t phase",
          "Zoneplate 2nd order t phase, t*t/256 cycles per picture", G_MININT32,
          G_MAXINT32, 0, G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
  g_object_class_install_property (gobject_class, PROP_XOFFSET,
      g_param_spec_int ("xoffset", "Zoneplate 2nd order products x offset",
          "Zoneplate 2nd order products x offset", G_MININT32, G_MAXINT32, 0,
          G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
  g_object_class_install_property (gobject_class, PROP_YOFFSET,
      g_param_spec_int ("yoffset", "Zoneplate 2nd order products y offset",
          "Zoneplate 2nd order products y offset", G_MININT32, G_MAXINT32, 0,
          G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
  /**
   * GstVideoTestSrc:foreground-color
   *
   * Color to use for solid-color pattern and foreground color of other
   * patterns.  Default is white (0xffffffff).
   *
   * Since: 0.10.31
   **/
  g_object_class_install_property (gobject_class, PROP_FOREGROUND_COLOR,
      g_param_spec_uint ("foreground-color", "Foreground Color",
          "Foreground color to use (big-endian ARGB)", 0, G_MAXUINT32,
          DEFAULT_FOREGROUND_COLOR,
          G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
  /**
   * GstVideoTestSrc:background-color
   *
   * Color to use for background color of some patterns.  Default is
   * black (0xff000000).
   *
   * Since: 0.10.31
   **/
  g_object_class_install_property (gobject_class, PROP_BACKGROUND_COLOR,
      g_param_spec_uint ("background-color", "Background Color",
          "Background color to use (big-endian ARGB)", 0, G_MAXUINT32,
          DEFAULT_BACKGROUND_COLOR,
          G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  g_object_class_install_property (gobject_class, PROP_HORIZONTAL_SPEED,
      g_param_spec_int ("horizontal-speed", "Horizontal Speed",
          "Scroll image number of pixels per frame (positive is scroll to the left)",
          G_MININT32, G_MAXINT32, DEFAULT_HORIZONTAL_SPEED,
          G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  gst_element_class_set_details_simple (gstelement_class,
      "Video test source", "Source/Video",
      "Creates a test video stream", "David A. Schleef <ds@schleef.org>");

  templ_caps = gst_video_test_src_getcaps (NULL, NULL);
  gst_element_class_add_pad_template (gstelement_class,
      gst_pad_template_new ("src", GST_PAD_SRC, GST_PAD_ALWAYS, templ_caps));
  gst_caps_unref (templ_caps);

  gstbasesrc_class->get_caps = gst_video_test_src_getcaps;
  gstbasesrc_class->set_caps = gst_video_test_src_setcaps;
  gstbasesrc_class->fixate = gst_video_test_src_src_fixate;
  gstbasesrc_class->is_seekable = gst_video_test_src_is_seekable;
  gstbasesrc_class->do_seek = gst_video_test_src_do_seek;
  gstbasesrc_class->query = gst_video_test_src_query;
  gstbasesrc_class->get_times = gst_video_test_src_get_times;
  gstbasesrc_class->start = gst_video_test_src_start;
  gstbasesrc_class->stop = gst_video_test_src_stop;
  gstbasesrc_class->decide_allocation = gst_video_test_src_decide_allocation;

  gstpushsrc_class->fill = gst_video_test_src_fill;
}

static void
gst_video_test_src_init (GstVideoTestSrc * src)
{
  gst_video_test_src_set_pattern (src, DEFAULT_PATTERN);

  src->timestamp_offset = DEFAULT_TIMESTAMP_OFFSET;
  src->foreground_color = DEFAULT_FOREGROUND_COLOR;
  src->background_color = DEFAULT_BACKGROUND_COLOR;
  src->horizontal_speed = DEFAULT_HORIZONTAL_SPEED;

  /* we operate in time */
  gst_base_src_set_format (GST_BASE_SRC (src), GST_FORMAT_TIME);
  gst_base_src_set_live (GST_BASE_SRC (src), DEFAULT_IS_LIVE);
}

static GstCaps *
gst_video_test_src_src_fixate (GstBaseSrc * bsrc, GstCaps * caps)
{
  GstStructure *structure;

  caps = gst_caps_make_writable (caps);

  structure = gst_caps_get_structure (caps, 0);

  gst_structure_fixate_field_nearest_int (structure, "width", 320);
  gst_structure_fixate_field_nearest_int (structure, "height", 240);
  gst_structure_fixate_field_nearest_fraction (structure, "framerate", 30, 1);
  if (gst_structure_has_field (structure, "pixel-aspect-ratio"))
    gst_structure_fixate_field_nearest_fraction (structure,
        "pixel-aspect-ratio", 1, 1);
  if (gst_structure_has_field (structure, "colorimetry"))
    gst_structure_fixate_field_string (structure, "colorimetry", "bt601");
  if (gst_structure_has_field (structure, "chroma-site"))
    gst_structure_fixate_field_string (structure, "chroma-site", "mpeg2");

  if (gst_structure_has_field (structure, "interlaced"))
    gst_structure_fixate_field_boolean (structure, "interlaced", FALSE);

  caps = GST_BASE_SRC_CLASS (parent_class)->fixate (bsrc, caps);

  return caps;
}

static void
gst_video_test_src_set_pattern (GstVideoTestSrc * videotestsrc,
    int pattern_type)
{
  videotestsrc->pattern_type = pattern_type;

  GST_DEBUG_OBJECT (videotestsrc, "setting pattern to %d", pattern_type);

  switch (pattern_type) {
    case GST_VIDEO_TEST_SRC_SMPTE:
      videotestsrc->make_image = gst_video_test_src_smpte;
      break;
    case GST_VIDEO_TEST_SRC_SNOW:
      videotestsrc->make_image = gst_video_test_src_snow;
      break;
    case GST_VIDEO_TEST_SRC_BLACK:
      videotestsrc->make_image = gst_video_test_src_black;
      break;
    case GST_VIDEO_TEST_SRC_WHITE:
      videotestsrc->make_image = gst_video_test_src_white;
      break;
    case GST_VIDEO_TEST_SRC_RED:
      videotestsrc->make_image = gst_video_test_src_red;
      break;
    case GST_VIDEO_TEST_SRC_GREEN:
      videotestsrc->make_image = gst_video_test_src_green;
      break;
    case GST_VIDEO_TEST_SRC_BLUE:
      videotestsrc->make_image = gst_video_test_src_blue;
      break;
    case GST_VIDEO_TEST_SRC_CHECKERS1:
      videotestsrc->make_image = gst_video_test_src_checkers1;
      break;
    case GST_VIDEO_TEST_SRC_CHECKERS2:
      videotestsrc->make_image = gst_video_test_src_checkers2;
      break;
    case GST_VIDEO_TEST_SRC_CHECKERS4:
      videotestsrc->make_image = gst_video_test_src_checkers4;
      break;
    case GST_VIDEO_TEST_SRC_CHECKERS8:
      videotestsrc->make_image = gst_video_test_src_checkers8;
      break;
    case GST_VIDEO_TEST_SRC_CIRCULAR:
      videotestsrc->make_image = gst_video_test_src_circular;
      break;
    case GST_VIDEO_TEST_SRC_BLINK:
      videotestsrc->make_image = gst_video_test_src_blink;
      break;
    case GST_VIDEO_TEST_SRC_SMPTE75:
      videotestsrc->make_image = gst_video_test_src_smpte75;
      break;
    case GST_VIDEO_TEST_SRC_ZONE_PLATE:
      videotestsrc->make_image = gst_video_test_src_zoneplate;
      break;
    case GST_VIDEO_TEST_SRC_GAMUT:
      videotestsrc->make_image = gst_video_test_src_gamut;
      break;
    case GST_VIDEO_TEST_SRC_CHROMA_ZONE_PLATE:
      videotestsrc->make_image = gst_video_test_src_chromazoneplate;
      break;
    case GST_VIDEO_TEST_SRC_SOLID:
      videotestsrc->make_image = gst_video_test_src_solid;
      break;
    case GST_VIDEO_TEST_SRC_BALL:
      videotestsrc->make_image = gst_video_test_src_ball;
      break;
    case GST_VIDEO_TEST_SRC_SMPTE100:
      videotestsrc->make_image = gst_video_test_src_smpte100;
      break;
    case GST_VIDEO_TEST_SRC_BAR:
      videotestsrc->make_image = gst_video_test_src_bar;
      break;
    default:
      g_assert_not_reached ();
  }
}

static void
gst_video_test_src_set_property (GObject * object, guint prop_id,
    const GValue * value, GParamSpec * pspec)
{
  GstVideoTestSrc *src = GST_VIDEO_TEST_SRC (object);

  switch (prop_id) {
    case PROP_PATTERN:
      gst_video_test_src_set_pattern (src, g_value_get_enum (value));
      break;
    case PROP_TIMESTAMP_OFFSET:
      src->timestamp_offset = g_value_get_int64 (value);
      break;
    case PROP_IS_LIVE:
      gst_base_src_set_live (GST_BASE_SRC (src), g_value_get_boolean (value));
      break;
    case PROP_K0:
      src->k0 = g_value_get_int (value);
      break;
    case PROP_KX:
      src->kx = g_value_get_int (value);
      break;
    case PROP_KY:
      src->ky = g_value_get_int (value);
      break;
    case PROP_KT:
      src->kt = g_value_get_int (value);
      break;
    case PROP_KXT:
      src->kxt = g_value_get_int (value);
      break;
    case PROP_KYT:
      src->kyt = g_value_get_int (value);
      break;
    case PROP_KXY:
      src->kxy = g_value_get_int (value);
      break;
    case PROP_KX2:
      src->kx2 = g_value_get_int (value);
      break;
    case PROP_KY2:
      src->ky2 = g_value_get_int (value);
      break;
    case PROP_KT2:
      src->kt2 = g_value_get_int (value);
      break;
    case PROP_XOFFSET:
      src->xoffset = g_value_get_int (value);
      break;
    case PROP_YOFFSET:
      src->yoffset = g_value_get_int (value);
      break;
    case PROP_FOREGROUND_COLOR:
      src->foreground_color = g_value_get_uint (value);
      break;
    case PROP_BACKGROUND_COLOR:
      src->background_color = g_value_get_uint (value);
      break;
    case PROP_HORIZONTAL_SPEED:
      src->horizontal_speed = g_value_get_int (value);
    default:
      break;
  }
}

static void
gst_video_test_src_get_property (GObject * object, guint prop_id,
    GValue * value, GParamSpec * pspec)
{
  GstVideoTestSrc *src = GST_VIDEO_TEST_SRC (object);

  switch (prop_id) {
    case PROP_PATTERN:
      g_value_set_enum (value, src->pattern_type);
      break;
    case PROP_TIMESTAMP_OFFSET:
      g_value_set_int64 (value, src->timestamp_offset);
      break;
    case PROP_IS_LIVE:
      g_value_set_boolean (value, gst_base_src_is_live (GST_BASE_SRC (src)));
      break;
    case PROP_K0:
      g_value_set_int (value, src->k0);
      break;
    case PROP_KX:
      g_value_set_int (value, src->kx);
      break;
    case PROP_KY:
      g_value_set_int (value, src->ky);
      break;
    case PROP_KT:
      g_value_set_int (value, src->kt);
      break;
    case PROP_KXT:
      g_value_set_int (value, src->kxt);
      break;
    case PROP_KYT:
      g_value_set_int (value, src->kyt);
      break;
    case PROP_KXY:
      g_value_set_int (value, src->kxy);
      break;
    case PROP_KX2:
      g_value_set_int (value, src->kx2);
      break;
    case PROP_KY2:
      g_value_set_int (value, src->ky2);
      break;
    case PROP_KT2:
      g_value_set_int (value, src->kt2);
      break;
    case PROP_XOFFSET:
      g_value_set_int (value, src->xoffset);
      break;
    case PROP_YOFFSET:
      g_value_set_int (value, src->yoffset);
      break;
    case PROP_FOREGROUND_COLOR:
      g_value_set_uint (value, src->foreground_color);
      break;
    case PROP_BACKGROUND_COLOR:
      g_value_set_uint (value, src->background_color);
      break;
    case PROP_HORIZONTAL_SPEED:
      g_value_set_int (value, src->horizontal_speed);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

/* threadsafe because this gets called as the plugin is loaded */
static GstCaps *
gst_video_test_src_getcaps (GstBaseSrc * bsrc, GstCaps * filter)
{
  static GstCaps *capslist = NULL;

  if (!capslist) {
    GstCaps *caps;
    GstStructure *structure;
    int i;

    caps = gst_caps_new_empty ();
    for (i = 0; i < n_formats; i++) {
      structure = paint_get_structure (format_list + i);
      gst_structure_set (structure,
          "width", GST_TYPE_INT_RANGE, 1, G_MAXINT,
          "height", GST_TYPE_INT_RANGE, 1, G_MAXINT,
          "framerate", GST_TYPE_FRACTION_RANGE, 0, 1, G_MAXINT, 1, NULL);
      gst_caps_append_structure (caps, structure);
    }

    capslist = caps;
  }

  if (filter)
    return gst_caps_intersect_full (filter, capslist, GST_CAPS_INTERSECT_FIRST);
  else
    return gst_caps_ref (capslist);
}

static gboolean
gst_video_test_src_parse_caps (const GstCaps * caps,
    gint * width, gint * height, gint * fps_n, gint * fps_d,
    GstVideoColorimetry * colorimetry)
{
  const GstStructure *structure;
  GstPadLinkReturn ret;
  const GValue *framerate;
  const gchar *csp;

  GST_DEBUG ("parsing caps");

  structure = gst_caps_get_structure (caps, 0);

  ret = gst_structure_get_int (structure, "width", width);
  ret &= gst_structure_get_int (structure, "height", height);
  framerate = gst_structure_get_value (structure, "framerate");

  if (framerate) {
    *fps_n = gst_value_get_fraction_numerator (framerate);
    *fps_d = gst_value_get_fraction_denominator (framerate);
  } else
    goto no_framerate;

  if ((csp = gst_structure_get_string (structure, "colorimetry")))
    gst_video_colorimetry_from_string (colorimetry, csp);

  return ret;

  /* ERRORS */
no_framerate:
  {
    GST_DEBUG ("videotestsrc no framerate given");
    return FALSE;
  }
}

static gboolean
gst_video_test_src_decide_allocation (GstBaseSrc * bsrc, GstQuery * query)
{
  GstVideoTestSrc *videotestsrc;
  GstBufferPool *pool;
  gboolean update;
  guint size, min, max;
  GstStructure *config;

  videotestsrc = GST_VIDEO_TEST_SRC (bsrc);

  if (gst_query_get_n_allocation_pools (query) > 0) {
    gst_query_parse_nth_allocation_pool (query, 0, &pool, &size, &min, &max);

    /* adjust size */
    size = MAX (size, videotestsrc->info.size);
    update = TRUE;
  } else {
    pool = NULL;
    size = videotestsrc->info.size;
    min = max = 0;
    update = FALSE;
  }

  /* no downstream pool, make our own */
  if (pool == NULL) {
    pool = gst_video_buffer_pool_new ();
  }

  config = gst_buffer_pool_get_config (pool);
  if (gst_query_has_allocation_meta (query, GST_VIDEO_META_API_TYPE)) {
    gst_buffer_pool_config_add_option (config,
        GST_BUFFER_POOL_OPTION_VIDEO_META);
  }
  gst_buffer_pool_set_config (pool, config);

  if (update)
    gst_query_set_nth_allocation_pool (query, 0, pool, size, min, max);
  else
    gst_query_add_allocation_pool (query, pool, size, min, max);

  if (pool)
    gst_object_unref (pool);

  return TRUE;
}

static gboolean
gst_video_test_src_setcaps (GstBaseSrc * bsrc, GstCaps * caps)
{
  struct format_list_struct *format;
  const GstStructure *structure;
  GstVideoTestSrc *videotestsrc;
  GstVideoInfo info;

  videotestsrc = GST_VIDEO_TEST_SRC (bsrc);

  structure = gst_caps_get_structure (caps, 0);

  if (gst_structure_has_name (structure, "video/x-raw")) {
    /* we can use the parsing code */
    if (!gst_video_info_from_caps (&info, caps))
      goto parse_failed;

  } else if (gst_structure_has_name (structure, "video/x-bayer")) {
    if (!gst_video_test_src_parse_caps (caps, &info.width, &info.height,
            &info.fps_n, &info.fps_d, &info.colorimetry))
      goto parse_failed;

    info.size =
        gst_video_test_src_get_size (videotestsrc, info.width, info.height);
  }

  if (!(format = paintinfo_find_by_structure (structure)))
    goto unknown_format;

  /* looks ok here */
  videotestsrc->format = format;
  videotestsrc->info = info;

  GST_DEBUG_OBJECT (videotestsrc, "size %dx%d, %d/%d fps",
      info.width, info.height, info.fps_n, info.fps_d);

  g_free (videotestsrc->tmpline);
  g_free (videotestsrc->tmpline2);
  g_free (videotestsrc->tmpline_u8);
  videotestsrc->tmpline_u8 = g_malloc (info.width + 8);
  videotestsrc->tmpline = g_malloc ((info.width + 8) * 4);
  videotestsrc->tmpline2 = g_malloc ((info.width + 8) * 4);

  return TRUE;

  /* ERRORS */
parse_failed:
  {
    GST_DEBUG_OBJECT (bsrc, "failed to parse caps");
    return FALSE;
  }
unknown_format:
  {
    GST_DEBUG ("videotestsrc format not found");
    return FALSE;
  }
}

static gboolean
gst_video_test_src_query (GstBaseSrc * bsrc, GstQuery * query)
{
  gboolean res;
  GstVideoTestSrc *src;

  src = GST_VIDEO_TEST_SRC (bsrc);

  switch (GST_QUERY_TYPE (query)) {
    case GST_QUERY_CONVERT:
    {
      GstFormat src_fmt, dest_fmt;
      gint64 src_val, dest_val;

      gst_query_parse_convert (query, &src_fmt, &src_val, &dest_fmt, &dest_val);
      res =
          gst_video_info_convert (&src->info, src_fmt, src_val, dest_fmt,
          &dest_val);
      gst_query_set_convert (query, src_fmt, src_val, dest_fmt, dest_val);
      break;
    }
    default:
      res = GST_BASE_SRC_CLASS (parent_class)->query (bsrc, query);
      break;
  }
  return res;
}

static void
gst_video_test_src_get_times (GstBaseSrc * basesrc, GstBuffer * buffer,
    GstClockTime * start, GstClockTime * end)
{
  /* for live sources, sync on the timestamp of the buffer */
  if (gst_base_src_is_live (basesrc)) {
    GstClockTime timestamp = GST_BUFFER_TIMESTAMP (buffer);

    if (GST_CLOCK_TIME_IS_VALID (timestamp)) {
      /* get duration to calculate end time */
      GstClockTime duration = GST_BUFFER_DURATION (buffer);

      if (GST_CLOCK_TIME_IS_VALID (duration)) {
        *end = timestamp + duration;
      }
      *start = timestamp;
    }
  } else {
    *start = -1;
    *end = -1;
  }
}

static gboolean
gst_video_test_src_do_seek (GstBaseSrc * bsrc, GstSegment * segment)
{
  GstClockTime position;
  GstVideoTestSrc *src;

  src = GST_VIDEO_TEST_SRC (bsrc);

  segment->time = segment->start;
  position = segment->position;

  /* now move to the position indicated */
  if (src->info.fps_n) {
    src->n_frames = gst_util_uint64_scale (position,
        src->info.fps_n, src->info.fps_d * GST_SECOND);
  } else {
    src->n_frames = 0;
  }
  if (src->info.fps_n) {
    src->running_time = gst_util_uint64_scale (src->n_frames,
        src->info.fps_d * GST_SECOND, src->info.fps_n);
  } else {
    /* FIXME : Not sure what to set here */
    src->running_time = 0;
  }

  g_assert (src->running_time <= position);

  return TRUE;
}

static gboolean
gst_video_test_src_is_seekable (GstBaseSrc * psrc)
{
  /* we're seekable... */
  return TRUE;
}

static GstFlowReturn
gst_video_test_src_fill (GstPushSrc * psrc, GstBuffer * buffer)
{
  GstVideoTestSrc *src;
  GstClockTime next_time;
  GstVideoFrame frame;

  src = GST_VIDEO_TEST_SRC (psrc);

  if (G_UNLIKELY (src->format == NULL))
    goto not_negotiated;

  /* 0 framerate and we are at the second frame, eos */
  if (G_UNLIKELY (src->info.fps_n == 0 && src->n_frames == 1))
    goto eos;

  GST_LOG_OBJECT (src,
      "creating buffer from pool for frame %d", (gint) src->n_frames);

  if (!gst_video_frame_map (&frame, &src->info, buffer, GST_MAP_WRITE))
    goto invalid_frame;

  src->make_image (src, &frame);

  gst_video_frame_unmap (&frame);

  GST_BUFFER_TIMESTAMP (buffer) = src->timestamp_offset + src->running_time;
  GST_BUFFER_OFFSET (buffer) = src->n_frames;
  src->n_frames++;
  GST_BUFFER_OFFSET_END (buffer) = src->n_frames;
  if (src->info.fps_n) {
    next_time = gst_util_uint64_scale_int (src->n_frames * GST_SECOND,
        src->info.fps_d, src->info.fps_n);
    GST_BUFFER_DURATION (buffer) = next_time - src->running_time;
  } else {
    next_time = src->timestamp_offset;
    /* NONE means forever */
    GST_BUFFER_DURATION (buffer) = GST_CLOCK_TIME_NONE;
  }

  src->running_time = next_time;

  return GST_FLOW_OK;

not_negotiated:
  {
    GST_ELEMENT_ERROR (src, CORE, NEGOTIATION, (NULL),
        ("format wasn't negotiated before get function"));
    return GST_FLOW_NOT_NEGOTIATED;
  }
eos:
  {
    GST_DEBUG_OBJECT (src, "eos: 0 framerate, frame %d", (gint) src->n_frames);
    return GST_FLOW_EOS;
  }
invalid_frame:
  {
    GST_DEBUG_OBJECT (src, "invalid frame");
    return GST_FLOW_OK;
  }
}

static gboolean
gst_video_test_src_start (GstBaseSrc * basesrc)
{
  GstVideoTestSrc *src = GST_VIDEO_TEST_SRC (basesrc);

  src->running_time = 0;
  src->n_frames = 0;

  return TRUE;
}

static gboolean
gst_video_test_src_stop (GstBaseSrc * basesrc)
{
  GstVideoTestSrc *src = GST_VIDEO_TEST_SRC (basesrc);

  g_free (src->tmpline);
  src->tmpline = NULL;
  g_free (src->tmpline2);
  src->tmpline2 = NULL;
  g_free (src->tmpline_u8);
  src->tmpline_u8 = NULL;

  return TRUE;
}

static gboolean
plugin_init (GstPlugin * plugin)
{
  GST_DEBUG_CATEGORY_INIT (video_test_src_debug, "videotestsrc", 0,
      "Video Test Source");

  return gst_element_register (plugin, "videotestsrc", GST_RANK_NONE,
      GST_TYPE_VIDEO_TEST_SRC);
}

GST_PLUGIN_DEFINE (GST_VERSION_MAJOR,
    GST_VERSION_MINOR,
    "videotestsrc",
    "Creates a test video stream",
    plugin_init, VERSION, GST_LICENSE, GST_PACKAGE_NAME, GST_PACKAGE_ORIGIN)
