/* GStreamer
 * Copyright (C) <1999> Erik Walthinsen <omega@cse.ogi.edu>
 * Library       <2002> Ronald Bultje <rbultje@ronald.bitfreak.net>
 * Copyright (C) 2007 David A. Schleef <ds@schleef.org>
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

#include <string.h>

#include "video.h"
#include "gstvideometa.h"

GST_DEBUG_CATEGORY_EXTERN (GST_CAT_PERFORMANCE);

static int fill_planes (GstVideoInfo * info);

typedef struct
{
  guint32 fourcc;
  GstVideoFormatInfo info;
} VideoFormat;

/* depths: bits, n_components, shift, depth */
#define DPTH0            0, 0, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }
#define DPTH8            8, 1, { 0, 0, 0, 0 }, { 8, 0, 0, 0 }
#define DPTH888          8, 3, { 0, 0, 0, 0 }, { 8, 8, 8, 0 }
#define DPTH8888         8, 4, { 0, 0, 0, 0 }, { 8, 8, 8, 8 }
#define DPTH10_10_10     10, 3, { 0, 0, 0, 0 }, { 10, 10, 10, 0 }
#define DPTH16           16, 1, { 0, 0, 0, 0 }, { 16, 0, 0, 0 }
#define DPTH16_16_16     16, 3, { 0, 0, 0, 0 }, { 16, 16, 16, 0 }
#define DPTH16_16_16_16  16, 4, { 0, 0, 0, 0 }, { 16, 16, 16, 16 }
#define DPTH555          16, 3, { 10, 5, 0, 0 }, { 5, 5, 5, 0 }
#define DPTH565          16, 3, { 11, 5, 0, 0 }, { 5, 6, 5, 0 }

/* pixel strides */
#define PSTR0             { 0, 0, 0, 0 }
#define PSTR1             { 1, 0, 0, 0 }
#define PSTR111           { 1, 1, 1, 0 }
#define PSTR1111          { 1, 1, 1, 1 }
#define PSTR122           { 1, 2, 2, 0 }
#define PSTR2             { 2, 0, 0, 0 }
#define PSTR222           { 2, 2, 2, 0 }
#define PSTR244           { 2, 4, 4, 0 }
#define PSTR444           { 4, 4, 4, 0 }
#define PSTR4444          { 4, 4, 4, 4 }
#define PSTR333           { 3, 3, 3, 0 }
#define PSTR488           { 4, 8, 8, 0 }
#define PSTR8888          { 8, 8, 8, 8 }

/* planes */
#define PLANE_NA          0, { 0, 0, 0, 0 }
#define PLANE0            1, { 0, 0, 0, 0 }
#define PLANE011          2, { 0, 1, 1, 0 }
#define PLANE012          3, { 0, 1, 2, 0 }
#define PLANE0123         4, { 0, 1, 2, 3 }
#define PLANE021          3, { 0, 2, 1, 0 }

/* offsets */
#define OFFS0             { 0, 0, 0, 0 }
#define OFFS013           { 0, 1, 3, 0 }
#define OFFS102           { 1, 0, 2, 0 }
#define OFFS1230          { 1, 2, 3, 0 }
#define OFFS012           { 0, 1, 2, 0 }
#define OFFS210           { 2, 1, 0, 0 }
#define OFFS123           { 1, 2, 3, 0 }
#define OFFS321           { 3, 2, 1, 0 }
#define OFFS0123          { 0, 1, 2, 3 }
#define OFFS2103          { 2, 1, 0, 3 }
#define OFFS3210          { 3, 2, 1, 0 }
#define OFFS031           { 0, 3, 1, 0 }
#define OFFS026           { 0, 2, 6, 0 }
#define OFFS001           { 0, 0, 1, 0 }
#define OFFS010           { 0, 1, 0, 0 }
#define OFFS104           { 1, 0, 4, 0 }
#define OFFS2460          { 2, 4, 6, 0 }

/* subsampling */
#define SUB410            { 0, 2, 2, 0 }, { 0, 2, 2, 0 }
#define SUB411            { 0, 2, 2, 0 }, { 0, 0, 0, 0 }
#define SUB420            { 0, 1, 1, 0 }, { 0, 1, 1, 0 }
#define SUB422            { 0, 1, 1, 0 }, { 0, 0, 0, 0 }
#define SUB4              { 0, 0, 0, 0 }, { 0, 0, 0, 0 }
#define SUB444            { 0, 0, 0, 0 }, { 0, 0, 0, 0 }
#define SUB4444           { 0, 0, 0, 0 }, { 0, 0, 0, 0 }
#define SUB4204           { 0, 1, 1, 0 }, { 0, 1, 1, 0 }

#define MAKE_YUV_FORMAT(name, desc, fourcc, depth, pstride, plane, offs, sub ) \
 { fourcc, {GST_VIDEO_FORMAT_ ##name, G_STRINGIFY(name), desc, GST_VIDEO_FORMAT_FLAG_YUV, depth, pstride, plane, offs, sub } }
#define MAKE_YUVA_FORMAT(name, desc, fourcc, depth, pstride, plane, offs, sub) \
 { fourcc, {GST_VIDEO_FORMAT_ ##name, G_STRINGIFY(name), desc, GST_VIDEO_FORMAT_FLAG_YUV | GST_VIDEO_FORMAT_FLAG_ALPHA, depth, pstride, plane, offs, sub } }

#define MAKE_RGB_FORMAT(name, desc, depth, pstride, plane, offs, sub) \
 { 0x00000000, {GST_VIDEO_FORMAT_ ##name, G_STRINGIFY(name), desc, GST_VIDEO_FORMAT_FLAG_RGB, depth, pstride, plane, offs, sub } }
#define MAKE_RGB_LE_FORMAT(name, desc, depth, pstride, plane, offs, sub) \
 { 0x00000000, {GST_VIDEO_FORMAT_ ##name, G_STRINGIFY(name), desc, GST_VIDEO_FORMAT_FLAG_RGB | GST_VIDEO_FORMAT_FLAG_LE, depth, pstride, plane, offs, sub } }
#define MAKE_RGBA_FORMAT(name, desc, depth, pstride, plane, offs, sub) \
 { 0x00000000, {GST_VIDEO_FORMAT_ ##name, G_STRINGIFY(name), desc, GST_VIDEO_FORMAT_FLAG_RGB | GST_VIDEO_FORMAT_FLAG_ALPHA, depth, pstride, plane, offs, sub } }

#define MAKE_GRAY_FORMAT(name, desc, depth, pstride, plane, offs, sub) \
 { 0x00000000, {GST_VIDEO_FORMAT_ ##name, G_STRINGIFY(name), desc, GST_VIDEO_FORMAT_FLAG_GRAY, depth, pstride, plane, offs, sub } }
#define MAKE_GRAY_LE_FORMAT(name, desc, depth, pstride, plane, offs, sub) \
 { 0x00000000, {GST_VIDEO_FORMAT_ ##name, G_STRINGIFY(name), desc, GST_VIDEO_FORMAT_FLAG_GRAY | GST_VIDEO_FORMAT_FLAG_LE, depth, pstride, plane, offs, sub } }

static VideoFormat formats[] = {
  {0x00000000, {GST_VIDEO_FORMAT_UNKNOWN, "UNKNOWN", "unknown video", 0, DPTH0,
              PSTR0, PLANE_NA,
          OFFS0}},

  MAKE_YUV_FORMAT (I420, "raw video", GST_MAKE_FOURCC ('I', '4', '2', '0'),
      DPTH888, PSTR111,
      PLANE012, OFFS0, SUB420),
  MAKE_YUV_FORMAT (YV12, "raw video", GST_MAKE_FOURCC ('Y', 'V', '1', '2'),
      DPTH888, PSTR111,
      PLANE021, OFFS0, SUB420),
  MAKE_YUV_FORMAT (YUY2, "raw video", GST_MAKE_FOURCC ('Y', 'U', 'Y', '2'),
      DPTH888, PSTR244,
      PLANE0, OFFS013, SUB422),
  MAKE_YUV_FORMAT (UYVY, "raw video", GST_MAKE_FOURCC ('U', 'Y', 'V', 'Y'),
      DPTH888, PSTR244,
      PLANE0, OFFS102, SUB422),
  MAKE_YUVA_FORMAT (AYUV, "raw video", GST_MAKE_FOURCC ('A', 'Y', 'U', 'V'),
      DPTH8888,
      PSTR4444, PLANE0, OFFS1230, SUB4444),
  MAKE_RGB_FORMAT (RGBx, "raw video", DPTH888, PSTR444, PLANE0, OFFS012,
      SUB444),
  MAKE_RGB_FORMAT (BGRx, "raw video", DPTH888, PSTR444, PLANE0, OFFS210,
      SUB444),
  MAKE_RGB_FORMAT (xRGB, "raw video", DPTH888, PSTR444, PLANE0, OFFS123,
      SUB444),
  MAKE_RGB_FORMAT (xBGR, "raw video", DPTH888, PSTR444, PLANE0, OFFS321,
      SUB444),
  MAKE_RGBA_FORMAT (RGBA, "raw video", DPTH8888, PSTR4444, PLANE0, OFFS0123,
      SUB4444),
  MAKE_RGBA_FORMAT (BGRA, "raw video", DPTH8888, PSTR4444, PLANE0, OFFS2103,
      SUB4444),
  MAKE_RGBA_FORMAT (ARGB, "raw video", DPTH8888, PSTR4444, PLANE0, OFFS1230,
      SUB4444),
  MAKE_RGBA_FORMAT (ABGR, "raw video", DPTH8888, PSTR4444, PLANE0, OFFS3210,
      SUB4444),
  MAKE_RGB_FORMAT (RGB, "raw video", DPTH888, PSTR333, PLANE0, OFFS012, SUB444),
  MAKE_RGB_FORMAT (BGR, "raw video", DPTH888, PSTR333, PLANE0, OFFS210, SUB444),

  MAKE_YUV_FORMAT (Y41B, "raw video", GST_MAKE_FOURCC ('Y', '4', '1', 'B'),
      DPTH888, PSTR111,
      PLANE012, OFFS0, SUB411),
  MAKE_YUV_FORMAT (Y42B, "raw video", GST_MAKE_FOURCC ('Y', '4', '2', 'B'),
      DPTH888, PSTR111,
      PLANE012, OFFS0, SUB422),
  MAKE_YUV_FORMAT (YVYU, "raw video", GST_MAKE_FOURCC ('Y', 'V', 'Y', 'U'),
      DPTH888, PSTR244,
      PLANE0, OFFS031, SUB422),
  MAKE_YUV_FORMAT (Y444, "raw video", GST_MAKE_FOURCC ('Y', '4', '4', '4'),
      DPTH888, PSTR111,
      PLANE012, OFFS0, SUB444),
  MAKE_YUV_FORMAT (v210, "raw video", GST_MAKE_FOURCC ('v', '2', '1', '0'),
      DPTH10_10_10,
      PSTR0, PLANE0, OFFS0, SUB422),
  MAKE_YUV_FORMAT (v216, "raw video", GST_MAKE_FOURCC ('v', '2', '1', '6'),
      DPTH16_16_16,
      PSTR488, PLANE0, OFFS026, SUB422),
  MAKE_YUV_FORMAT (NV12, "raw video", GST_MAKE_FOURCC ('N', 'V', '1', '2'),
      DPTH888, PSTR122,
      PLANE011, OFFS001, SUB420),
  MAKE_YUV_FORMAT (NV21, "raw video", GST_MAKE_FOURCC ('N', 'V', '2', '1'),
      DPTH888, PSTR122,
      PLANE011, OFFS010, SUB420),

  MAKE_GRAY_FORMAT (GRAY8, "raw video", DPTH8, PSTR1, PLANE0, OFFS0, SUB4),
  MAKE_GRAY_FORMAT (GRAY16_BE, "raw video", DPTH16, PSTR2, PLANE0, OFFS0, SUB4),
  MAKE_GRAY_LE_FORMAT (GRAY16_LE, "raw video", DPTH16, PSTR2, PLANE0, OFFS0,
      SUB4),

  MAKE_YUV_FORMAT (v308, "raw video", GST_MAKE_FOURCC ('v', '3', '0', '8'),
      DPTH888, PSTR333,
      PLANE0, OFFS012, SUB444),
  MAKE_YUV_FORMAT (Y800, "raw video", GST_MAKE_FOURCC ('Y', '8', '0', '0'),
      DPTH8, PSTR1,
      PLANE0, OFFS0, SUB4),
  MAKE_YUV_FORMAT (Y16, "raw video", GST_MAKE_FOURCC ('Y', '1', '6', ' '),
      DPTH16, PSTR2,
      PLANE0, OFFS0, SUB4),

#if G_BYTE_ORDER == G_LITTLE_ENDIAN
  MAKE_RGB_LE_FORMAT (RGB16, "raw video", DPTH565, PSTR222, PLANE0, OFFS0,
      SUB444),
  MAKE_RGB_LE_FORMAT (BGR16, "raw video", DPTH565, PSTR222, PLANE0, OFFS0,
      SUB444),
  MAKE_RGB_LE_FORMAT (RGB15, "raw video", DPTH555, PSTR222, PLANE0, OFFS0,
      SUB444),
  MAKE_RGB_LE_FORMAT (BGR15, "raw video", DPTH555, PSTR222, PLANE0, OFFS0,
      SUB444),
#else
  MAKE_RGB_FORMAT (RGB16, "raw video", DPTH565, PSTR222, PLANE0, OFFS0, SUB444),
  MAKE_RGB_FORMAT (BGR16, "raw video", DPTH565, PSTR222, PLANE0, OFFS0, SUB444),
  MAKE_RGB_FORMAT (RGB15, "raw video", DPTH555, PSTR222, PLANE0, OFFS0, SUB444),
  MAKE_RGB_FORMAT (BGR15, "raw video", DPTH555, PSTR222, PLANE0, OFFS0, SUB444),
#endif

  MAKE_YUV_FORMAT (UYVP, "raw video", GST_MAKE_FOURCC ('U', 'Y', 'V', 'P'),
      DPTH10_10_10,
      PSTR0, PLANE0, OFFS0, SUB422),
  MAKE_YUVA_FORMAT (A420, "raw video", GST_MAKE_FOURCC ('A', '4', '2', '0'),
      DPTH8888,
      PSTR1111, PLANE0123, OFFS0, SUB4204),
  MAKE_RGBA_FORMAT (RGB8_PALETTED, "raw video", DPTH8888, PSTR1111, PLANE0,
      OFFS0, SUB4444),
  MAKE_YUV_FORMAT (YUV9, "raw video", GST_MAKE_FOURCC ('Y', 'U', 'V', '9'),
      DPTH888, PSTR111,
      PLANE012, OFFS0, SUB410),
  MAKE_YUV_FORMAT (YVU9, "raw video", GST_MAKE_FOURCC ('Y', 'V', 'U', '9'),
      DPTH888, PSTR111,
      PLANE021, OFFS0, SUB410),
  MAKE_YUV_FORMAT (IYU1, "raw video", GST_MAKE_FOURCC ('I', 'Y', 'U', '1'),
      DPTH888, PSTR0,
      PLANE0, OFFS104, SUB411),
  MAKE_RGBA_FORMAT (ARGB64, "raw video", DPTH16_16_16_16, PSTR8888, PLANE0,
      OFFS2460,
      SUB444),
  MAKE_YUVA_FORMAT (AYUV64, "raw video", 0x00000000, DPTH16_16_16_16, PSTR8888,
      PLANE0,
      OFFS2460, SUB444),
  MAKE_YUV_FORMAT (r210, "raw video", GST_MAKE_FOURCC ('r', '2', '1', '0'),
      DPTH10_10_10,
      PSTR444, PLANE0, OFFS0, SUB444),
};

/**
 * SECTION:gstvideo
 * @short_description: Support library for video operations
 *
 * <refsect2>
 * <para>
 * This library contains some helper functions and includes the
 * videosink and videofilter base classes.
 * </para>
 * </refsect2>
 */

/**
 * gst_video_calculate_display_ratio:
 * @dar_n: Numerator of the calculated display_ratio
 * @dar_d: Denominator of the calculated display_ratio
 * @video_width: Width of the video frame in pixels
 * @video_height: Height of the video frame in pixels
 * @video_par_n: Numerator of the pixel aspect ratio of the input video.
 * @video_par_d: Denominator of the pixel aspect ratio of the input video.
 * @display_par_n: Numerator of the pixel aspect ratio of the display device
 * @display_par_d: Denominator of the pixel aspect ratio of the display device
 *
 * Given the Pixel Aspect Ratio and size of an input video frame, and the
 * pixel aspect ratio of the intended display device, calculates the actual
 * display ratio the video will be rendered with.
 *
 * Returns: A boolean indicating success and a calculated Display Ratio in the
 * dar_n and dar_d parameters.
 * The return value is FALSE in the case of integer overflow or other error.
 *
 * Since: 0.10.7
 */
gboolean
gst_video_calculate_display_ratio (guint * dar_n, guint * dar_d,
    guint video_width, guint video_height,
    guint video_par_n, guint video_par_d,
    guint display_par_n, guint display_par_d)
{
  gint num, den;
  gint tmp_n, tmp_d;

  g_return_val_if_fail (dar_n != NULL, FALSE);
  g_return_val_if_fail (dar_d != NULL, FALSE);

  /* Calculate (video_width * video_par_n * display_par_d) /
   * (video_height * video_par_d * display_par_n) */
  if (!gst_util_fraction_multiply (video_width, video_height, video_par_n,
          video_par_d, &tmp_n, &tmp_d))
    goto error_overflow;

  if (!gst_util_fraction_multiply (tmp_n, tmp_d, display_par_d, display_par_n,
          &num, &den))
    goto error_overflow;

  g_return_val_if_fail (num > 0, FALSE);
  g_return_val_if_fail (den > 0, FALSE);

  *dar_n = num;
  *dar_d = den;

  return TRUE;

  /* ERRORS */
error_overflow:
  {
    GST_WARNING ("overflow in multiply");
    return FALSE;
  }
}

static GstVideoFormat
gst_video_format_from_rgb32_masks (int red_mask, int green_mask, int blue_mask)
{
  if (red_mask == 0xff000000 && green_mask == 0x00ff0000 &&
      blue_mask == 0x0000ff00) {
    return GST_VIDEO_FORMAT_RGBx;
  }
  if (red_mask == 0x0000ff00 && green_mask == 0x00ff0000 &&
      blue_mask == 0xff000000) {
    return GST_VIDEO_FORMAT_BGRx;
  }
  if (red_mask == 0x00ff0000 && green_mask == 0x0000ff00 &&
      blue_mask == 0x000000ff) {
    return GST_VIDEO_FORMAT_xRGB;
  }
  if (red_mask == 0x000000ff && green_mask == 0x0000ff00 &&
      blue_mask == 0x00ff0000) {
    return GST_VIDEO_FORMAT_xBGR;
  }

  return GST_VIDEO_FORMAT_UNKNOWN;
}

static GstVideoFormat
gst_video_format_from_rgba32_masks (int red_mask, int green_mask,
    int blue_mask, int alpha_mask)
{
  if (red_mask == 0xff000000 && green_mask == 0x00ff0000 &&
      blue_mask == 0x0000ff00 && alpha_mask == 0x000000ff) {
    return GST_VIDEO_FORMAT_RGBA;
  }
  if (red_mask == 0x0000ff00 && green_mask == 0x00ff0000 &&
      blue_mask == 0xff000000 && alpha_mask == 0x000000ff) {
    return GST_VIDEO_FORMAT_BGRA;
  }
  if (red_mask == 0x00ff0000 && green_mask == 0x0000ff00 &&
      blue_mask == 0x000000ff && alpha_mask == 0xff000000) {
    return GST_VIDEO_FORMAT_ARGB;
  }
  if (red_mask == 0x000000ff && green_mask == 0x0000ff00 &&
      blue_mask == 0x00ff0000 && alpha_mask == 0xff000000) {
    return GST_VIDEO_FORMAT_ABGR;
  }
  return GST_VIDEO_FORMAT_UNKNOWN;
}

static GstVideoFormat
gst_video_format_from_rgb24_masks (int red_mask, int green_mask, int blue_mask)
{
  if (red_mask == 0xff0000 && green_mask == 0x00ff00 && blue_mask == 0x0000ff) {
    return GST_VIDEO_FORMAT_RGB;
  }
  if (red_mask == 0x0000ff && green_mask == 0x00ff00 && blue_mask == 0xff0000) {
    return GST_VIDEO_FORMAT_BGR;
  }

  return GST_VIDEO_FORMAT_UNKNOWN;
}

#define GST_VIDEO_COMP1_MASK_16_INT 0xf800
#define GST_VIDEO_COMP2_MASK_16_INT 0x07e0
#define GST_VIDEO_COMP3_MASK_16_INT 0x001f

#define GST_VIDEO_COMP1_MASK_15_INT 0x7c00
#define GST_VIDEO_COMP2_MASK_15_INT 0x03e0
#define GST_VIDEO_COMP3_MASK_15_INT 0x001f

static GstVideoFormat
gst_video_format_from_rgb16_masks (int red_mask, int green_mask, int blue_mask)
{
  if (red_mask == GST_VIDEO_COMP1_MASK_16_INT
      && green_mask == GST_VIDEO_COMP2_MASK_16_INT
      && blue_mask == GST_VIDEO_COMP3_MASK_16_INT) {
    return GST_VIDEO_FORMAT_RGB16;
  }
  if (red_mask == GST_VIDEO_COMP3_MASK_16_INT
      && green_mask == GST_VIDEO_COMP2_MASK_16_INT
      && blue_mask == GST_VIDEO_COMP1_MASK_16_INT) {
    return GST_VIDEO_FORMAT_BGR16;
  }
  if (red_mask == GST_VIDEO_COMP1_MASK_15_INT
      && green_mask == GST_VIDEO_COMP2_MASK_15_INT
      && blue_mask == GST_VIDEO_COMP3_MASK_15_INT) {
    return GST_VIDEO_FORMAT_RGB15;
  }
  if (red_mask == GST_VIDEO_COMP3_MASK_15_INT
      && green_mask == GST_VIDEO_COMP2_MASK_15_INT
      && blue_mask == GST_VIDEO_COMP1_MASK_15_INT) {
    return GST_VIDEO_FORMAT_BGR15;
  }
  return GST_VIDEO_FORMAT_UNKNOWN;
}

/**
 * gst_video_format_from_masks:
 * @depth: the amount of bits used for a pixel
 * @bpp: the amount of bits used to store a pixel. This value is bigger than
 *   @depth
 * @endianness: the endianness of the masks
 * @red_mask: the red mask
 * @green_mask: the green mask
 * @blue_mask: the blue mask
 * @alpha_mask: the optional alpha mask
 *
 * Find the #GstVideoFormat for the given parameters.
 *
 * Returns: a #GstVideoFormat or GST_VIDEO_FORMAT_UNKNOWN when the parameters to
 * not specify a known format.
 */
GstVideoFormat
gst_video_format_from_masks (gint depth, gint bpp, gint endianness,
    gint red_mask, gint green_mask, gint blue_mask, gint alpha_mask)
{
  GstVideoFormat format;

  /* our caps system handles 24/32bpp RGB as big-endian. */
  if ((bpp == 24 || bpp == 32) && endianness == G_LITTLE_ENDIAN) {
    red_mask = GUINT32_TO_BE (red_mask);
    green_mask = GUINT32_TO_BE (green_mask);
    blue_mask = GUINT32_TO_BE (blue_mask);
    endianness = G_BIG_ENDIAN;
    if (bpp == 24) {
      red_mask >>= 8;
      green_mask >>= 8;
      blue_mask >>= 8;
    }
  }

  if (depth == 30 && bpp == 32) {
    format = GST_VIDEO_FORMAT_r210;
  } else if (depth == 24 && bpp == 32) {
    format = gst_video_format_from_rgb32_masks (red_mask, green_mask,
        blue_mask);
  } else if (depth == 32 && bpp == 32 && alpha_mask) {
    format = gst_video_format_from_rgba32_masks (red_mask, green_mask,
        blue_mask, alpha_mask);
  } else if (depth == 24 && bpp == 24) {
    format = gst_video_format_from_rgb24_masks (red_mask, green_mask,
        blue_mask);
  } else if ((depth == 15 || depth == 16) && bpp == 16 &&
      endianness == G_BYTE_ORDER) {
    format = gst_video_format_from_rgb16_masks (red_mask, green_mask,
        blue_mask);
  } else if (depth == 8 && bpp == 8) {
    format = GST_VIDEO_FORMAT_RGB8_PALETTED;
  } else if (depth == 64 && bpp == 64) {
    format = gst_video_format_from_rgba32_masks (red_mask, green_mask,
        blue_mask, alpha_mask);
    if (format == GST_VIDEO_FORMAT_ARGB) {
      format = GST_VIDEO_FORMAT_ARGB64;
    } else {
      format = GST_VIDEO_FORMAT_UNKNOWN;
    }
  } else {
    format = GST_VIDEO_FORMAT_UNKNOWN;
  }
  return format;
}

/**
 * gst_video_format_from_fourcc:
 * @fourcc: a FOURCC value representing raw YUV video
 *
 * Converts a FOURCC value into the corresponding #GstVideoFormat.
 * If the FOURCC cannot be represented by #GstVideoFormat,
 * #GST_VIDEO_FORMAT_UNKNOWN is returned.
 *
 * Since: 0.10.16
 *
 * Returns: the #GstVideoFormat describing the FOURCC value
 */
GstVideoFormat
gst_video_format_from_fourcc (guint32 fourcc)
{
  switch (fourcc) {
    case GST_MAKE_FOURCC ('I', '4', '2', '0'):
      return GST_VIDEO_FORMAT_I420;
    case GST_MAKE_FOURCC ('Y', 'V', '1', '2'):
      return GST_VIDEO_FORMAT_YV12;
    case GST_MAKE_FOURCC ('Y', 'U', 'Y', '2'):
      return GST_VIDEO_FORMAT_YUY2;
    case GST_MAKE_FOURCC ('Y', 'V', 'Y', 'U'):
      return GST_VIDEO_FORMAT_YVYU;
    case GST_MAKE_FOURCC ('U', 'Y', 'V', 'Y'):
      return GST_VIDEO_FORMAT_UYVY;
    case GST_MAKE_FOURCC ('A', 'Y', 'U', 'V'):
      return GST_VIDEO_FORMAT_AYUV;
    case GST_MAKE_FOURCC ('Y', '4', '1', 'B'):
      return GST_VIDEO_FORMAT_Y41B;
    case GST_MAKE_FOURCC ('Y', '4', '2', 'B'):
      return GST_VIDEO_FORMAT_Y42B;
    case GST_MAKE_FOURCC ('Y', '4', '4', '4'):
      return GST_VIDEO_FORMAT_Y444;
    case GST_MAKE_FOURCC ('v', '2', '1', '0'):
      return GST_VIDEO_FORMAT_v210;
    case GST_MAKE_FOURCC ('v', '2', '1', '6'):
      return GST_VIDEO_FORMAT_v216;
    case GST_MAKE_FOURCC ('N', 'V', '1', '2'):
      return GST_VIDEO_FORMAT_NV12;
    case GST_MAKE_FOURCC ('N', 'V', '2', '1'):
      return GST_VIDEO_FORMAT_NV21;
    case GST_MAKE_FOURCC ('v', '3', '0', '8'):
      return GST_VIDEO_FORMAT_v308;
    case GST_MAKE_FOURCC ('Y', '8', '0', '0'):
    case GST_MAKE_FOURCC ('Y', '8', ' ', ' '):
    case GST_MAKE_FOURCC ('G', 'R', 'E', 'Y'):
      return GST_VIDEO_FORMAT_Y800;
    case GST_MAKE_FOURCC ('Y', '1', '6', ' '):
      return GST_VIDEO_FORMAT_Y16;
    case GST_MAKE_FOURCC ('U', 'Y', 'V', 'P'):
      return GST_VIDEO_FORMAT_UYVP;
    case GST_MAKE_FOURCC ('A', '4', '2', '0'):
      return GST_VIDEO_FORMAT_A420;
    case GST_MAKE_FOURCC ('Y', 'U', 'V', '9'):
      return GST_VIDEO_FORMAT_YUV9;
    case GST_MAKE_FOURCC ('Y', 'V', 'U', '9'):
      return GST_VIDEO_FORMAT_YVU9;
    case GST_MAKE_FOURCC ('I', 'Y', 'U', '1'):
      return GST_VIDEO_FORMAT_IYU1;
    case GST_MAKE_FOURCC ('A', 'Y', '6', '4'):
      return GST_VIDEO_FORMAT_AYUV64;
    default:
      return GST_VIDEO_FORMAT_UNKNOWN;
  }
}

/**
 * gst_video_format_from_string:
 * @format: a format string
 *
 * Convert the @format string to its #GstVideoFormat.
 *
 * Returns: the #GstVideoFormat for @format or GST_VIDEO_FORMAT_UNKNOWN when the
 * string is not a known format.
 */
GstVideoFormat
gst_video_format_from_string (const gchar * format)
{
  guint i;

  g_return_val_if_fail (format != NULL, GST_VIDEO_FORMAT_UNKNOWN);

  for (i = 0; i < G_N_ELEMENTS (formats); i++) {
    if (strcmp (GST_VIDEO_FORMAT_INFO_NAME (&formats[i].info), format) == 0)
      return GST_VIDEO_FORMAT_INFO_FORMAT (&formats[i].info);
  }
  return GST_VIDEO_FORMAT_UNKNOWN;
}


/**
 * gst_video_format_to_fourcc:
 * @format: a #GstVideoFormat video format
 *
 * Converts a #GstVideoFormat value into the corresponding FOURCC.  Only
 * a few YUV formats have corresponding FOURCC values.  If @format has
 * no corresponding FOURCC value, 0 is returned.
 *
 * Since: 0.10.16
 *
 * Returns: the FOURCC corresponding to @format
 */
guint32
gst_video_format_to_fourcc (GstVideoFormat format)
{
  g_return_val_if_fail (format != GST_VIDEO_FORMAT_UNKNOWN, 0);

  if (format >= G_N_ELEMENTS (formats))
    return 0;

  return formats[format].fourcc;
}

const gchar *
gst_video_format_to_string (GstVideoFormat format)
{
  g_return_val_if_fail (format != GST_VIDEO_FORMAT_UNKNOWN, NULL);

  if (format >= G_N_ELEMENTS (formats))
    return NULL;

  return GST_VIDEO_FORMAT_INFO_NAME (&formats[format].info);
}

/**
 * gst_video_format_get_info:
 * @format: a #GstVideoFormat
 *
 * Get the #GstVideoFormatInfo for @format
 *
 * Returns: The #GstVideoFormatInfo for @format.
 */
const GstVideoFormatInfo *
gst_video_format_get_info (GstVideoFormat format)
{
  g_return_val_if_fail (format != GST_VIDEO_FORMAT_UNKNOWN, NULL);
  g_return_val_if_fail (format < G_N_ELEMENTS (formats), NULL);

  return &formats[format].info;
}

/**
 * gst_video_info_init:
 * @info: a #GstVideoInfo
 *
 * Initialize @info with default values.
 */
void
gst_video_info_init (GstVideoInfo * info)
{
  g_return_if_fail (info != NULL);

  memset (info, 0, sizeof (GstVideoInfo));

  info->finfo = &formats[GST_VIDEO_FORMAT_UNKNOWN].info;

  info->views = 1;
  /* arrange for sensible defaults, e.g. if turned into caps */
  info->fps_n = 0;
  info->fps_d = 1;
  info->par_n = 1;
  info->par_d = 1;
}

/**
 * gst_video_info_set_format:
 * @info: a #GstVideoInfo
 * @format: the format
 * @width: a width
 * @height: a height
 *
 * Set the default info for a video frame of @format and @width and @height.
 */
void
gst_video_info_set_format (GstVideoInfo * info, GstVideoFormat format,
    guint width, guint height)
{
  const GstVideoFormatInfo *finfo;

  g_return_if_fail (info != NULL);
  g_return_if_fail (format != GST_VIDEO_FORMAT_UNKNOWN);

  finfo = &formats[format].info;

  info->flags = 0;
  info->finfo = finfo;
  info->width = width;
  info->height = height;

  fill_planes (info);
}

static const gchar *interlace_mode[] = {
  "progressive",
  "interleaved",
  "mixed",
  "fields"
};

static const gchar *
gst_interlace_mode_to_string (GstVideoInterlaceMode mode)
{
  if (mode < 0 || mode >= G_N_ELEMENTS (interlace_mode))
    return NULL;

  return interlace_mode[mode];
}

static GstVideoInterlaceMode
gst_interlace_mode_from_string (const gchar * mode)
{
  gint i;
  for (i = 0; i < G_N_ELEMENTS (interlace_mode); i++) {
    if (g_str_equal (interlace_mode[i], mode))
      return i;
  }
  return GST_VIDEO_INTERLACE_MODE_PROGRESSIVE;
}

typedef struct
{
  const gchar *name;
  GstVideoChromaSite site;
} ChromaSiteInfo;

static const ChromaSiteInfo chromasite[] = {
  {"jpeg", GST_VIDEO_CHROMA_SITE_JPEG},
  {"mpeg2", GST_VIDEO_CHROMA_SITE_MPEG2},
  {"dv", GST_VIDEO_CHROMA_SITE_DV}
};

static GstVideoChromaSite
gst_video_chroma_from_string (const gchar * s)
{
  gint i;
  for (i = 0; i < G_N_ELEMENTS (chromasite); i++) {
    if (g_str_equal (chromasite[i].name, s))
      return chromasite[i].site;
  }
  return GST_VIDEO_CHROMA_SITE_UNKNOWN;
}

static const gchar *
gst_video_chroma_to_string (GstVideoChromaSite site)
{
  gint i;
  for (i = 0; i < G_N_ELEMENTS (chromasite); i++) {
    if (chromasite[i].site == site)
      return chromasite[i].name;
  }
  return NULL;
}

typedef struct
{
  const gchar *name;
  GstVideoColorimetry color;
} ColorimetryInfo;

#define MAKE_COLORIMETRY(n,r,m,t,p) { GST_VIDEO_COLORIMETRY_ ##n, \
  { GST_VIDEO_COLOR_RANGE ##r, GST_VIDEO_COLOR_MATRIX_ ##m, \
  GST_VIDEO_TRANSFER_ ##t, GST_VIDEO_COLOR_PRIMARIES_ ##p } }

static const ColorimetryInfo colorimetry[] = {
  MAKE_COLORIMETRY (BT601, _16_235, BT601, BT709, BT470M),
  MAKE_COLORIMETRY (BT709, _16_235, BT709, BT709, BT709),
  MAKE_COLORIMETRY (SMPTE240M, _16_235, SMPTE240M, SMPTE240M, SMPTE240M),
};

static const ColorimetryInfo *
gst_video_get_colorimetry (const gchar * s)
{
  gint i;

  for (i = 0; i < G_N_ELEMENTS (colorimetry); i++) {
    if (g_str_equal (colorimetry[i].name, s))
      return &colorimetry[i];
  }
  return NULL;
}

#define IS_EQUAL(ci,i) (((ci)->color.range == (i)->range) && \
                        ((ci)->color.matrix == (i)->matrix) && \
                        ((ci)->color.transfer == (i)->transfer) && \
                        ((ci)->color.primaries == (i)->primaries))


/**
 * gst_video_colorimetry_from_string
 * @cinfo: a #GstVideoColorimetry
 * @color: a colorimetry string
 *
 * Parse the colorimetry string and update @cinfo with the parsed
 * values.
 *
 * Returns: #TRUE if @color points to valid colorimetry info.
 */
gboolean
gst_video_colorimetry_from_string (GstVideoColorimetry * cinfo,
    const gchar * color)
{
  const ColorimetryInfo *ci;

  if ((ci = gst_video_get_colorimetry (color))) {
    *cinfo = ci->color;
  } else {
    /* FIXME, split and parse */
    cinfo->range = GST_VIDEO_COLOR_RANGE_16_235;
    cinfo->matrix = GST_VIDEO_COLOR_MATRIX_BT601;
    cinfo->transfer = GST_VIDEO_TRANSFER_BT709;
    cinfo->primaries = GST_VIDEO_COLOR_PRIMARIES_BT709;
  }
  return TRUE;
}

static void
gst_video_caps_set_colorimetry (GstCaps * caps, GstVideoColorimetry * cinfo)
{
  gint i;

  for (i = 0; i < G_N_ELEMENTS (colorimetry); i++) {
    if (IS_EQUAL (&colorimetry[i], cinfo)) {
      gst_caps_set_simple (caps, "colorimetry", G_TYPE_STRING,
          colorimetry[i].name, NULL);
      return;
    }
  }
  /* FIXME, construct colorimetry */
}

/**
 * gst_video_colorimetry_matches:
 * @info: a #GstVideoInfo
 * @color: a colorimetry string
 *
 * Check if the colorimetry information in @info matches that of the
 * string @color.
 *
 * Returns: #TRUE if @color conveys the same colorimetry info as the color
 * information in @info.
 */
gboolean
gst_video_colorimetry_matches (GstVideoColorimetry * cinfo, const gchar * color)
{
  const ColorimetryInfo *ci;

  if ((ci = gst_video_get_colorimetry (color)))
    return IS_EQUAL (ci, cinfo);

  return FALSE;
}

/**
 * gst_video_info_from_caps:
 * @info: a #GstVideoInfo
 * @caps: a #GstCaps
 *
 * Parse @caps and update @info.
 *
 * Returns: TRUE if @caps could be parsed
 */
gboolean
gst_video_info_from_caps (GstVideoInfo * info, const GstCaps * caps)
{
  GstStructure *structure;
  const gchar *s;
  GstVideoFormat format;
  gint width, height, views;
  gint fps_n, fps_d;
  gint par_n, par_d;

  g_return_val_if_fail (info != NULL, FALSE);
  g_return_val_if_fail (caps != NULL, FALSE);
  g_return_val_if_fail (gst_caps_is_fixed (caps), FALSE);

  GST_DEBUG ("parsing caps %" GST_PTR_FORMAT, caps);

  structure = gst_caps_get_structure (caps, 0);

  if (!gst_structure_has_name (structure, "video/x-raw"))
    goto wrong_name;

  if (!(s = gst_structure_get_string (structure, "format")))
    goto no_format;

  format = gst_video_format_from_string (s);
  if (format == GST_VIDEO_FORMAT_UNKNOWN)
    goto unknown_format;

  if (!gst_structure_get_int (structure, "width", &width))
    goto no_width;
  if (!gst_structure_get_int (structure, "height", &height))
    goto no_height;

  gst_video_info_set_format (info, format, width, height);

  if (gst_structure_get_fraction (structure, "framerate", &fps_n, &fps_d)) {
    if (fps_n == 0) {
      /* variable framerate */
      info->flags |= GST_VIDEO_FLAG_VARIABLE_FPS;
      /* see if we have a max-framerate */
      gst_structure_get_fraction (structure, "max-framerate", &fps_n, &fps_d);
    }
    info->fps_n = fps_n;
    info->fps_d = fps_d;
  } else {
    /* unspecified is variable framerate */
    info->fps_n = 0;
    info->fps_d = 1;
  }

  if ((s = gst_structure_get_string (structure, "interlace-mode")))
    info->interlace_mode = gst_interlace_mode_from_string (s);
  else
    info->interlace_mode = GST_VIDEO_INTERLACE_MODE_PROGRESSIVE;

  if (gst_structure_get_int (structure, "views", &views))
    info->views = views;
  else
    info->views = 1;

  if ((s = gst_structure_get_string (structure, "chroma-site")))
    info->chroma_site = gst_video_chroma_from_string (s);
  else
    info->chroma_site = GST_VIDEO_CHROMA_SITE_UNKNOWN;

  if ((s = gst_structure_get_string (structure, "colorimetry")))
    gst_video_colorimetry_from_string (&info->colorimetry, s);
  else
    memset (&info->colorimetry, 0, sizeof (GstVideoColorimetry));

  if (gst_structure_get_fraction (structure, "pixel-aspect-ratio",
          &par_n, &par_d)) {
    info->par_n = par_n;
    info->par_d = par_d;
  } else {
    info->par_n = 1;
    info->par_d = 1;
  }
  return TRUE;

  /* ERROR */
wrong_name:
  {
    GST_ERROR ("wrong name '%s', expected video/x-raw",
        gst_structure_get_name (structure));
    return FALSE;
  }
no_format:
  {
    GST_ERROR ("no format given");
    return FALSE;
  }
unknown_format:
  {
    GST_ERROR ("unknown format '%s' given", s);
    return FALSE;
  }
no_width:
  {
    GST_ERROR ("no width property given");
    return FALSE;
  }
no_height:
  {
    GST_ERROR ("no height property given");
    return FALSE;
  }
}

/**
 * gst_video_info_to_caps:
 * @info: a #GstVideoInfo
 *
 * Convert the values of @info into a #GstCaps.
 *
 * Returns: a new #GstCaps containing the info of @info.
 */
GstCaps *
gst_video_info_to_caps (GstVideoInfo * info)
{
  GstCaps *caps;
  const gchar *format;

  g_return_val_if_fail (info != NULL, NULL);
  g_return_val_if_fail (info->finfo != NULL, NULL);
  g_return_val_if_fail (info->finfo->format != GST_VIDEO_FORMAT_UNKNOWN, NULL);

  format = gst_video_format_to_string (info->finfo->format);
  g_return_val_if_fail (format != NULL, NULL);

  caps = gst_caps_new_simple ("video/x-raw",
      "format", G_TYPE_STRING, format,
      "width", G_TYPE_INT, info->width,
      "height", G_TYPE_INT, info->height,
      "pixel-aspect-ratio", GST_TYPE_FRACTION, info->par_n, info->par_d, NULL);

  gst_caps_set_simple (caps, "interlace-mode", G_TYPE_STRING,
      gst_interlace_mode_to_string (info->interlace_mode), NULL);

  if (info->chroma_site != GST_VIDEO_CHROMA_SITE_UNKNOWN)
    gst_caps_set_simple (caps, "chroma-site", G_TYPE_STRING,
        gst_video_chroma_to_string (info->chroma_site), NULL);

  gst_video_caps_set_colorimetry (caps, &info->colorimetry);

  if (info->views > 1)
    gst_caps_set_simple (caps, "views", G_TYPE_INT, info->views, NULL);

  if (info->flags & GST_VIDEO_FLAG_VARIABLE_FPS && info->fps_n != 0) {
    /* variable fps with a max-framerate */
    gst_caps_set_simple (caps, "framerate", GST_TYPE_FRACTION, 0, 1,
        "max-framerate", GST_TYPE_FRACTION, info->fps_n, info->fps_d, NULL);
  } else {
    /* no variable fps or no max-framerate */
    gst_caps_set_simple (caps, "framerate", GST_TYPE_FRACTION,
        info->fps_n, info->fps_d, NULL);
  }

  return caps;
}

/**
 * gst_video_frame_map_id:
 * @frame: pointer to #GstVideoFrame
 * @info: a #GstVideoInfo
 * @buffer: the buffer to map
 * @id: the frame id to map
 * @flags: #GstMapFlags
 *
 * Use @info and @buffer to fill in the values of @frame with the video frame
 * information of frame @id.
 *
 * When @id is -1, the default frame is mapped. When @id != -1, this function
 * will return %FALSE when there is no GstVideoMeta with that id.
 *
 * All video planes of @buffer will be mapped and the pointers will be set in
 * @frame->data.
 *
 * Returns: %TRUE on success.
 */
gboolean
gst_video_frame_map_id (GstVideoFrame * frame, GstVideoInfo * info,
    GstBuffer * buffer, gint id, GstMapFlags flags)
{
  GstVideoMeta *meta;
  gint i;

  g_return_val_if_fail (frame != NULL, FALSE);
  g_return_val_if_fail (info != NULL, FALSE);
  g_return_val_if_fail (GST_IS_BUFFER (buffer), FALSE);

  if (id == -1)
    meta = gst_buffer_get_video_meta (buffer);
  else
    meta = gst_buffer_get_video_meta_id (buffer, id);

  frame->buffer = buffer;
  frame->meta = meta;

  if (meta) {
    frame->info.flags = meta->flags;
    frame->info.finfo = &formats[meta->format].info;
    frame->info.width = meta->width;
    frame->info.height = meta->height;
    frame->id = meta->id;

    for (i = 0; i < info->finfo->n_planes; i++)
      gst_video_meta_map (meta, i, &frame->map[i], &frame->data[i],
          &frame->info.stride[i], flags);
  } else {
    /* no metadata, we really need to have the metadata when the id is
     * specified. */
    if (id != -1)
      goto no_metadata;

    /* copy the info */
    frame->info = *info;
    frame->id = id;

    gst_buffer_map (buffer, &frame->map[0], flags);

    /* do some sanity checks */
    if (frame->map[0].size < info->size)
      goto invalid_size;

    /* set up pointers */
    for (i = 0; i < info->finfo->n_planes; i++) {
      frame->data[i] = frame->map[0].data + info->offset[i];
    }
  }
  return TRUE;

  /* ERRORS */
no_metadata:
  {
    GST_ERROR ("no GstVideoMeta for id %d", id);
    return FALSE;
  }
invalid_size:
  {
    GST_ERROR ("invalid buffer size %" G_GSIZE_FORMAT " < %" G_GSIZE_FORMAT,
        frame->map[0].size, info->size);
    gst_buffer_unmap (buffer, &frame->map[0]);
    return FALSE;
  }
}

/**
 * gst_video_frame_map:
 * @frame: pointer to #GstVideoFrame
 * @info: a #GstVideoInfo
 * @buffer: the buffer to map
 * @flags: #GstMapFlags
 *
 * Use @info and @buffer to fill in the values of @frame.
 *
 * All video planes of @buffer will be mapped and the pointers will be set in
 * @frame->data.
 *
 * Returns: %TRUE on success.
 */
gboolean
gst_video_frame_map (GstVideoFrame * frame, GstVideoInfo * info,
    GstBuffer * buffer, GstMapFlags flags)
{
  return gst_video_frame_map_id (frame, info, buffer, -1, flags);
}

/**
 * gst_video_frame_unmap:
 * @frame: a #GstVideoFrame
 *
 * Unmap the memory previously mapped with gst_video_frame_map.
 */
void
gst_video_frame_unmap (GstVideoFrame * frame)
{
  GstBuffer *buffer;
  GstVideoMeta *meta;
  gint i;

  g_return_if_fail (frame != NULL);

  buffer = frame->buffer;
  meta = frame->meta;

  if (meta) {
    for (i = 0; i < frame->info.finfo->n_planes; i++) {
      gst_video_meta_unmap (meta, i, &frame->map[i]);
    }
  } else {
    gst_buffer_unmap (buffer, &frame->map[0]);
  }
}

/**
 * gst_video_frame_copy:
 * @dest: a #GstVideoFrame
 * @src: a #GstVideoFrame
 * @plane: a plane
 *
 * Copy the plane with index @plane from @src to @dest.
 *
 * Returns: TRUE if the contents could be copied.
 */
gboolean
gst_video_frame_copy_plane (GstVideoFrame * dest, const GstVideoFrame * src,
    guint plane)
{
  const GstVideoInfo *sinfo;
  GstVideoInfo *dinfo;
  guint w, h, j;
  guint8 *sp, *dp;
  gint ss, ds;

  g_return_val_if_fail (dest != NULL, FALSE);
  g_return_val_if_fail (src != NULL, FALSE);

  sinfo = &src->info;
  dinfo = &dest->info;

  g_return_val_if_fail (dinfo->finfo->format == sinfo->finfo->format, FALSE);
  g_return_val_if_fail (dinfo->width == sinfo->width
      && dinfo->height == sinfo->height, FALSE);
  g_return_val_if_fail (dinfo->finfo->n_planes > plane, FALSE);

  sp = src->data[plane];
  dp = dest->data[plane];

  ss = sinfo->stride[plane];
  ds = dinfo->stride[plane];

  w = MIN (ABS (ss), ABS (ds));
  h = GST_VIDEO_FRAME_COMP_HEIGHT (dest, plane);

  GST_CAT_DEBUG (GST_CAT_PERFORMANCE, "copy plane %d, w:%d h:%d ", plane, w, h);

  for (j = 0; j < h; j++) {
    memcpy (dp, sp, w);
    dp += ds;
    sp += ss;
  }
  return TRUE;
}

/**
 * gst_video_frame_copy:
 * @dest: a #GstVideoFrame
 * @src: a #GstVideoFrame
 *
 * Copy the contents from @src to @dest.
 *
 * Returns: TRUE if the contents could be copied.
 */
gboolean
gst_video_frame_copy (GstVideoFrame * dest, const GstVideoFrame * src)
{
  guint i, n_planes;
  const GstVideoInfo *sinfo;
  GstVideoInfo *dinfo;

  g_return_val_if_fail (dest != NULL, FALSE);
  g_return_val_if_fail (src != NULL, FALSE);

  sinfo = &src->info;
  dinfo = &dest->info;

  g_return_val_if_fail (dinfo->finfo->format == sinfo->finfo->format, FALSE);
  g_return_val_if_fail (dinfo->width == sinfo->width
      && dinfo->height == sinfo->height, FALSE);

  n_planes = dinfo->finfo->n_planes;

  for (i = 0; i < n_planes; i++)
    gst_video_frame_copy_plane (dest, src, i);

  return TRUE;
}

static int
fill_planes (GstVideoInfo * info)
{
  gint width, height;

  width = info->width;
  height = info->height;

  switch (info->finfo->format) {
    case GST_VIDEO_FORMAT_YUY2:
    case GST_VIDEO_FORMAT_YVYU:
    case GST_VIDEO_FORMAT_UYVY:
      info->stride[0] = GST_ROUND_UP_4 (width * 2);
      info->offset[0] = 0;
      info->size = info->stride[0] * height;
      break;
    case GST_VIDEO_FORMAT_AYUV:
    case GST_VIDEO_FORMAT_RGBx:
    case GST_VIDEO_FORMAT_RGBA:
    case GST_VIDEO_FORMAT_BGRx:
    case GST_VIDEO_FORMAT_BGRA:
    case GST_VIDEO_FORMAT_xRGB:
    case GST_VIDEO_FORMAT_ARGB:
    case GST_VIDEO_FORMAT_xBGR:
    case GST_VIDEO_FORMAT_ABGR:
    case GST_VIDEO_FORMAT_r210:
      info->stride[0] = width * 4;
      info->offset[0] = 0;
      info->size = info->stride[0] * height;
      break;
    case GST_VIDEO_FORMAT_RGB16:
    case GST_VIDEO_FORMAT_BGR16:
    case GST_VIDEO_FORMAT_RGB15:
    case GST_VIDEO_FORMAT_BGR15:
      info->stride[0] = GST_ROUND_UP_4 (width * 2);
      info->offset[0] = 0;
      info->size = info->stride[0] * height;
      break;
    case GST_VIDEO_FORMAT_RGB:
    case GST_VIDEO_FORMAT_BGR:
    case GST_VIDEO_FORMAT_v308:
      info->stride[0] = GST_ROUND_UP_4 (width * 3);
      info->offset[0] = 0;
      info->size = info->stride[0] * height;
      break;
    case GST_VIDEO_FORMAT_v210:
      info->stride[0] = ((width + 47) / 48) * 128;
      info->offset[0] = 0;
      info->size = info->stride[0] * height;
      break;
    case GST_VIDEO_FORMAT_v216:
      info->stride[0] = GST_ROUND_UP_8 (width * 4);
      info->offset[0] = 0;
      info->size = info->stride[0] * height;
      break;
    case GST_VIDEO_FORMAT_GRAY8:
    case GST_VIDEO_FORMAT_Y800:
      info->stride[0] = GST_ROUND_UP_4 (width);
      info->offset[0] = 0;
      info->size = info->stride[0] * height;
      break;
    case GST_VIDEO_FORMAT_GRAY16_BE:
    case GST_VIDEO_FORMAT_GRAY16_LE:
    case GST_VIDEO_FORMAT_Y16:
      info->stride[0] = GST_ROUND_UP_4 (width * 2);
      info->offset[0] = 0;
      info->size = info->stride[0] * height;
      break;
    case GST_VIDEO_FORMAT_UYVP:
      info->stride[0] = GST_ROUND_UP_4 ((width * 2 * 5 + 3) / 4);
      info->offset[0] = 0;
      info->size = info->stride[0] * height;
      break;
    case GST_VIDEO_FORMAT_RGB8_PALETTED:
      info->stride[0] = GST_ROUND_UP_4 (width);
      info->offset[0] = 0;
      info->size = info->stride[0] * height;
      break;
    case GST_VIDEO_FORMAT_IYU1:
      info->stride[0] = GST_ROUND_UP_4 (GST_ROUND_UP_4 (width) +
          GST_ROUND_UP_4 (width) / 2);
      info->offset[0] = 0;
      info->size = info->stride[0] * height;
      break;
    case GST_VIDEO_FORMAT_ARGB64:
    case GST_VIDEO_FORMAT_AYUV64:
      info->stride[0] = width * 8;
      info->offset[0] = 0;
      info->size = info->stride[0] * height;
      break;
    case GST_VIDEO_FORMAT_I420:
    case GST_VIDEO_FORMAT_YV12:        /* same as I420, but plane 1+2 swapped */
      info->stride[0] = GST_ROUND_UP_4 (width);
      info->stride[1] = GST_ROUND_UP_4 (GST_ROUND_UP_2 (width) / 2);
      info->stride[2] = info->stride[1];
      info->offset[0] = 0;
      info->offset[1] = info->stride[0] * GST_ROUND_UP_2 (height);
      info->offset[2] = info->offset[1] +
          info->stride[1] * (GST_ROUND_UP_2 (height) / 2);
      info->size = info->offset[2] +
          info->stride[2] * (GST_ROUND_UP_2 (height) / 2);
      break;
    case GST_VIDEO_FORMAT_Y41B:
      info->stride[0] = GST_ROUND_UP_4 (width);
      info->stride[1] = GST_ROUND_UP_16 (width) / 4;
      info->stride[2] = info->stride[1];
      info->offset[0] = 0;
      info->offset[1] = info->stride[0] * height;
      info->offset[2] = info->offset[1] + info->stride[1] * height;
      /* simplification of ROUNDUP4(w)*h + 2*((ROUNDUP16(w)/4)*h */
      info->size = (info->stride[0] + (GST_ROUND_UP_16 (width) / 2)) * height;
      break;
    case GST_VIDEO_FORMAT_Y42B:
      info->stride[0] = GST_ROUND_UP_4 (width);
      info->stride[1] = GST_ROUND_UP_8 (width) / 2;
      info->stride[2] = info->stride[1];
      info->offset[0] = 0;
      info->offset[1] = info->stride[0] * height;
      info->offset[2] = info->offset[1] + info->stride[1] * height;
      /* simplification of ROUNDUP4(w)*h + 2*(ROUNDUP8(w)/2)*h */
      info->size = (info->stride[0] + GST_ROUND_UP_8 (width)) * height;
      break;
    case GST_VIDEO_FORMAT_Y444:
      info->stride[0] = GST_ROUND_UP_4 (width);
      info->stride[1] = info->stride[0];
      info->stride[2] = info->stride[0];
      info->offset[0] = 0;
      info->offset[1] = info->stride[0] * height;
      info->offset[2] = info->offset[1] * 2;
      info->size = info->stride[0] * height * 3;
      break;
    case GST_VIDEO_FORMAT_NV12:
    case GST_VIDEO_FORMAT_NV21:
      info->stride[0] = GST_ROUND_UP_4 (width);
      info->stride[1] = info->stride[0];
      info->offset[0] = 0;
      info->offset[1] = info->stride[0] * GST_ROUND_UP_2 (height);
      info->size = info->stride[0] * GST_ROUND_UP_2 (height) * 3 / 2;
      break;
    case GST_VIDEO_FORMAT_A420:
      info->stride[0] = GST_ROUND_UP_4 (width);
      info->stride[1] = GST_ROUND_UP_4 (GST_ROUND_UP_2 (width) / 2);
      info->stride[2] = info->stride[1];
      info->stride[3] = info->stride[0];
      info->offset[0] = 0;
      info->offset[1] = info->stride[0] * GST_ROUND_UP_2 (height);
      info->offset[2] = info->offset[1] +
          info->stride[1] * (GST_ROUND_UP_2 (height) / 2);
      info->offset[3] = info->offset[2] +
          info->stride[2] * (GST_ROUND_UP_2 (height) / 2);
      info->size = info->offset[3] + info->stride[0];
      break;
    case GST_VIDEO_FORMAT_YUV9:
    case GST_VIDEO_FORMAT_YVU9:
      info->stride[0] = GST_ROUND_UP_4 (width);
      info->stride[1] = GST_ROUND_UP_4 (GST_ROUND_UP_4 (width) / 4);
      info->stride[2] = info->stride[1];
      info->offset[0] = 0;
      info->offset[1] = info->stride[0] * height;
      info->offset[2] = info->offset[1] +
          info->stride[1] * (GST_ROUND_UP_4 (height) / 4);
      info->size = info->offset[2] +
          info->stride[2] * (GST_ROUND_UP_4 (height) / 4);
      break;
    case GST_VIDEO_FORMAT_UNKNOWN:
      GST_ERROR ("invalid format");
      g_warning ("invalid format");
      break;
  }
  return 0;
}

/**
 * gst_video_format_convert:
 * @info: a #GstVideoInfo
 * @src_format: #GstFormat of the @src_value
 * @src_value: value to convert
 * @dest_format: #GstFormat of the @dest_value
 * @dest_value: pointer to destination value
 *
 * Converts among various #GstFormat types.  This function handles
 * GST_FORMAT_BYTES, GST_FORMAT_TIME, and GST_FORMAT_DEFAULT.  For
 * raw video, GST_FORMAT_DEFAULT corresponds to video frames.  This
 * function can be used to handle pad queries of the type GST_QUERY_CONVERT.
 *
 * Since: 0.10.16
 *
 * Returns: TRUE if the conversion was successful.
 */
gboolean
gst_video_info_convert (GstVideoInfo * info,
    GstFormat src_format, gint64 src_value,
    GstFormat dest_format, gint64 * dest_value)
{
  gboolean ret = FALSE;
  int size, fps_n, fps_d;

  g_return_val_if_fail (info != NULL, 0);
  g_return_val_if_fail (info->finfo != NULL, 0);
  g_return_val_if_fail (info->finfo->format != GST_VIDEO_FORMAT_UNKNOWN, 0);
  g_return_val_if_fail (info->size > 0, 0);

  size = info->size;
  fps_n = info->fps_n;
  fps_d = info->fps_d;

  GST_DEBUG ("converting value %" G_GINT64_FORMAT " from %s to %s",
      src_value, gst_format_get_name (src_format),
      gst_format_get_name (dest_format));

  if (src_format == dest_format) {
    *dest_value = src_value;
    ret = TRUE;
    goto done;
  }

  if (src_value == -1) {
    *dest_value = -1;
    ret = TRUE;
    goto done;
  }

  /* bytes to frames */
  if (src_format == GST_FORMAT_BYTES && dest_format == GST_FORMAT_DEFAULT) {
    if (size != 0) {
      *dest_value = gst_util_uint64_scale_int (src_value, 1, size);
    } else {
      GST_ERROR ("blocksize is 0");
      *dest_value = 0;
    }
    ret = TRUE;
    goto done;
  }

  /* frames to bytes */
  if (src_format == GST_FORMAT_DEFAULT && dest_format == GST_FORMAT_BYTES) {
    *dest_value = gst_util_uint64_scale_int (src_value, size, 1);
    ret = TRUE;
    goto done;
  }

  /* time to frames */
  if (src_format == GST_FORMAT_TIME && dest_format == GST_FORMAT_DEFAULT) {
    if (fps_d != 0) {
      *dest_value = gst_util_uint64_scale (src_value,
          fps_n, GST_SECOND * fps_d);
    } else {
      GST_ERROR ("framerate denominator is 0");
      *dest_value = 0;
    }
    ret = TRUE;
    goto done;
  }

  /* frames to time */
  if (src_format == GST_FORMAT_DEFAULT && dest_format == GST_FORMAT_TIME) {
    if (fps_n != 0) {
      *dest_value = gst_util_uint64_scale (src_value,
          GST_SECOND * fps_d, fps_n);
    } else {
      GST_ERROR ("framerate numerator is 0");
      *dest_value = 0;
    }
    ret = TRUE;
    goto done;
  }

  /* time to bytes */
  if (src_format == GST_FORMAT_TIME && dest_format == GST_FORMAT_BYTES) {
    if (fps_d != 0) {
      *dest_value = gst_util_uint64_scale (src_value,
          fps_n * size, GST_SECOND * fps_d);
    } else {
      GST_ERROR ("framerate denominator is 0");
      *dest_value = 0;
    }
    ret = TRUE;
    goto done;
  }

  /* bytes to time */
  if (src_format == GST_FORMAT_BYTES && dest_format == GST_FORMAT_TIME) {
    if (fps_n != 0 && size != 0) {
      *dest_value = gst_util_uint64_scale (src_value,
          GST_SECOND * fps_d, fps_n * size);
    } else {
      GST_ERROR ("framerate denominator and/or blocksize is 0");
      *dest_value = 0;
    }
    ret = TRUE;
  }

done:

  GST_DEBUG ("ret=%d result %" G_GINT64_FORMAT, ret, *dest_value);

  return ret;
}

#define GST_VIDEO_EVENT_STILL_STATE_NAME "GstEventStillFrame"

/**
 * gst_video_event_new_still_frame:
 * @in_still: boolean value for the still-frame state of the event.
 *
 * Creates a new Still Frame event. If @in_still is %TRUE, then the event
 * represents the start of a still frame sequence. If it is %FALSE, then
 * the event ends a still frame sequence.
 *
 * To parse an event created by gst_video_event_new_still_frame() use
 * gst_video_event_parse_still_frame().
 *
 * Returns: The new GstEvent
 * Since: 0.10.26
 */
GstEvent *
gst_video_event_new_still_frame (gboolean in_still)
{
  GstEvent *still_event;
  GstStructure *s;

  s = gst_structure_new (GST_VIDEO_EVENT_STILL_STATE_NAME,
      "still-state", G_TYPE_BOOLEAN, in_still, NULL);
  still_event = gst_event_new_custom (GST_EVENT_CUSTOM_DOWNSTREAM, s);

  return still_event;
}

/**
 * gst_video_event_parse_still_frame:
 * @event: A #GstEvent to parse
 * @in_still: A boolean to receive the still-frame status from the event, or NULL
 *
 * Parse a #GstEvent, identify if it is a Still Frame event, and
 * return the still-frame state from the event if it is.
 * If the event represents the start of a still frame, the in_still
 * variable will be set to TRUE, otherwise FALSE. It is OK to pass NULL for the
 * in_still variable order to just check whether the event is a valid still-frame
 * event.
 *
 * Create a still frame event using gst_video_event_new_still_frame()
 *
 * Returns: %TRUE if the event is a valid still-frame event. %FALSE if not
 * Since: 0.10.26
 */
gboolean
gst_video_event_parse_still_frame (GstEvent * event, gboolean * in_still)
{
  const GstStructure *s;
  gboolean ev_still_state;

  g_return_val_if_fail (event != NULL, FALSE);

  if (GST_EVENT_TYPE (event) != GST_EVENT_CUSTOM_DOWNSTREAM)
    return FALSE;               /* Not a still frame event */

  s = gst_event_get_structure (event);
  if (s == NULL
      || !gst_structure_has_name (s, GST_VIDEO_EVENT_STILL_STATE_NAME))
    return FALSE;               /* Not a still frame event */
  if (!gst_structure_get_boolean (s, "still-state", &ev_still_state))
    return FALSE;               /* Not a still frame event */
  if (in_still)
    *in_still = ev_still_state;
  return TRUE;
}

/**
 * gst_video_parse_caps_palette:
 * @caps: #GstCaps to parse
 *
 * Returns the palette data from the caps as a #GstBuffer. For
 * #GST_VIDEO_FORMAT_RGB8_PALETTED this is containing 256 #guint32
 * values, each containing ARGB colors in native endianness.
 *
 * Returns: a #GstBuffer containing the palette data. Unref after usage.
 * Since: 0.10.32
 */
GstBuffer *
gst_video_parse_caps_palette (GstCaps * caps)
{
  GstStructure *s;
  const GValue *p_v;
  GstBuffer *p;

  if (!gst_caps_is_fixed (caps))
    return NULL;

  s = gst_caps_get_structure (caps, 0);

  p_v = gst_structure_get_value (s, "palette_data");
  if (!p_v || !GST_VALUE_HOLDS_BUFFER (p_v))
    return NULL;

  p = g_value_dup_boxed (p_v);

  return p;
}

#define GST_VIDEO_EVENT_FORCE_KEY_UNIT_NAME "GstForceKeyUnit"

/**
 * gst_video_event_new_downstream_force_key_unit:
 * @timestamp: the timestamp of the buffer that starts a new key unit
 * @stream_time: the stream_time of the buffer that starts a new key unit
 * @running_time: the running_time of the buffer that starts a new key unit
 * @all_headers: %TRUE to produce headers when starting a new key unit
 * @count: integer that can be used to number key units
 *
 * Creates a new downstream force key unit event. A downstream force key unit
 * event can be sent down the pipeline to request downstream elements to produce
 * a key unit. A downstream force key unit event must also be sent when handling
 * an upstream force key unit event to notify downstream that the latter has been
 * handled.
 *
 * To parse an event created by gst_video_event_new_downstream_force_key_unit() use
 * gst_video_event_parse_downstream_force_key_unit().
 *
 * Returns: The new GstEvent
 * Since: 0.10.36
 */
GstEvent *
gst_video_event_new_downstream_force_key_unit (GstClockTime timestamp,
    GstClockTime stream_time, GstClockTime running_time, gboolean all_headers,
    guint count)
{
  GstEvent *force_key_unit_event;
  GstStructure *s;

  s = gst_structure_new (GST_VIDEO_EVENT_FORCE_KEY_UNIT_NAME,
      "timestamp", G_TYPE_UINT64, timestamp,
      "stream-time", G_TYPE_UINT64, stream_time,
      "running-time", G_TYPE_UINT64, running_time,
      "all-headers", G_TYPE_BOOLEAN, all_headers,
      "count", G_TYPE_UINT, count, NULL);
  force_key_unit_event = gst_event_new_custom (GST_EVENT_CUSTOM_DOWNSTREAM, s);

  return force_key_unit_event;
}

/**
 * gst_video_event_new_upstream_force_key_unit:
 * @running_time: the running_time at which a new key unit should be produced
 * @all_headers: %TRUE to produce headers when starting a new key unit
 * @count: integer that can be used to number key units
 *
 * Creates a new upstream force key unit event. An upstream force key unit event
 * can be sent to request upstream elements to produce a key unit. 
 *
 * @running_time can be set to request a new key unit at a specific
 * running_time. If set to GST_CLOCK_TIME_NONE, upstream elements will produce a
 * new key unit as soon as possible.
 *
 * To parse an event created by gst_video_event_new_downstream_force_key_unit() use
 * gst_video_event_parse_downstream_force_key_unit().
 *
 * Returns: The new GstEvent
 * Since: 0.10.36
 */
GstEvent *
gst_video_event_new_upstream_force_key_unit (GstClockTime running_time,
    gboolean all_headers, guint count)
{
  GstEvent *force_key_unit_event;
  GstStructure *s;

  s = gst_structure_new (GST_VIDEO_EVENT_FORCE_KEY_UNIT_NAME,
      "running-time", GST_TYPE_CLOCK_TIME, running_time,
      "all-headers", G_TYPE_BOOLEAN, all_headers,
      "count", G_TYPE_UINT, count, NULL);
  force_key_unit_event = gst_event_new_custom (GST_EVENT_CUSTOM_UPSTREAM, s);

  return force_key_unit_event;
}

/**
 * gst_video_event_is_force_key_unit:
 * @event: A #GstEvent to check
 *
 * Checks if an event is a force key unit event. Returns true for both upstream
 * and downstream force key unit events.
 *
 * Returns: %TRUE if the event is a valid force key unit event
 * Since: 0.10.36
 */
gboolean
gst_video_event_is_force_key_unit (GstEvent * event)
{
  const GstStructure *s;

  g_return_val_if_fail (event != NULL, FALSE);

  if (GST_EVENT_TYPE (event) != GST_EVENT_CUSTOM_DOWNSTREAM &&
      GST_EVENT_TYPE (event) != GST_EVENT_CUSTOM_UPSTREAM)
    return FALSE;               /* Not a force key unit event */

  s = gst_event_get_structure (event);
  if (s == NULL
      || !gst_structure_has_name (s, GST_VIDEO_EVENT_FORCE_KEY_UNIT_NAME))
    return FALSE;

  return TRUE;
}

/**
 * gst_video_event_parse_downstream_force_key_unit:
 * @event: A #GstEvent to parse
 * @timestamp: (out): A pointer to the timestamp in the event
 * @stream_time: (out): A pointer to the stream-time in the event
 * @running_time: (out): A pointer to the running-time in the event
 * @all_headers: (out): A pointer to the all_headers flag in the event
 * @count: (out): A pointer to the count field of the event
 *
 * Get timestamp, stream-time, running-time, all-headers and count in the force
 * key unit event. See gst_video_event_new_downstream_force_key_unit() for a
 * full description of the downstream force key unit event.
 *
 * Returns: %TRUE if the event is a valid downstream force key unit event.
 * Since: 0.10.36
 */
gboolean
gst_video_event_parse_downstream_force_key_unit (GstEvent * event,
    GstClockTime * timestamp, GstClockTime * stream_time,
    GstClockTime * running_time, gboolean * all_headers, guint * count)
{
  const GstStructure *s;
  GstClockTime ev_timestamp, ev_stream_time, ev_running_time;
  gboolean ev_all_headers;
  guint ev_count;

  g_return_val_if_fail (event != NULL, FALSE);

  if (GST_EVENT_TYPE (event) != GST_EVENT_CUSTOM_DOWNSTREAM)
    return FALSE;               /* Not a force key unit event */

  s = gst_event_get_structure (event);
  if (s == NULL
      || !gst_structure_has_name (s, GST_VIDEO_EVENT_FORCE_KEY_UNIT_NAME))
    return FALSE;

  if (!gst_structure_get_clock_time (s, "timestamp", &ev_timestamp))
    return FALSE;               /* Not a force key unit event */
  if (!gst_structure_get_clock_time (s, "stream-time", &ev_stream_time))
    return FALSE;               /* Not a force key unit event */
  if (!gst_structure_get_clock_time (s, "running-time", &ev_running_time))
    return FALSE;               /* Not a force key unit event */
  if (!gst_structure_get_boolean (s, "all-headers", &ev_all_headers))
    return FALSE;               /* Not a force key unit event */
  if (!gst_structure_get_uint (s, "count", &ev_count))
    return FALSE;               /* Not a force key unit event */

  if (timestamp)
    *timestamp = ev_timestamp;

  if (stream_time)
    *stream_time = ev_stream_time;

  if (running_time)
    *running_time = ev_running_time;

  if (all_headers)
    *all_headers = ev_all_headers;

  if (count)
    *count = ev_count;

  return TRUE;
}

/**
 * gst_video_event_parse_upstream_force_key_unit:
 * @event: A #GstEvent to parse
 * @running_time: (out): A pointer to the running_time in the event
 * @all_headers: (out): A pointer to the all_headers flag in the event
 * @count: (out): A pointer to the count field in the event
 *
 * Get running-time, all-headers and count in the force key unit event. See
 * gst_video_event_new_upstream_force_key_unit() for a full description of the
 * upstream force key unit event.
 *
 * Create an upstream force key unit event using  gst_video_event_new_upstream_force_key_unit()
 *
 * Returns: %TRUE if the event is a valid upstream force-key-unit event. %FALSE if not
 * Since: 0.10.36
 */
gboolean
gst_video_event_parse_upstream_force_key_unit (GstEvent * event,
    GstClockTime * running_time, gboolean * all_headers, guint * count)
{
  const GstStructure *s;
  GstClockTime ev_running_time;
  gboolean ev_all_headers;
  guint ev_count;

  g_return_val_if_fail (event != NULL, FALSE);

  if (GST_EVENT_TYPE (event) != GST_EVENT_CUSTOM_UPSTREAM)
    return FALSE;               /* Not a force key unit event */

  s = gst_event_get_structure (event);
  if (s == NULL
      || !gst_structure_has_name (s, GST_VIDEO_EVENT_FORCE_KEY_UNIT_NAME))
    return FALSE;

  if (!gst_structure_get_clock_time (s, "running-time", &ev_running_time))
    return FALSE;               /* Not a force key unit event */
  if (!gst_structure_get_boolean (s, "all-headers", &ev_all_headers))
    return FALSE;               /* Not a force key unit event */
  if (!gst_structure_get_uint (s, "count", &ev_count))
    return FALSE;               /* Not a force key unit event */

  if (running_time)
    *running_time = ev_running_time;

  if (all_headers)
    *all_headers = ev_all_headers;

  if (count)
    *count = ev_count;

  return TRUE;
}
