/* GStreamer
 * Copyright (C) <2011> Wim Taymans <wim.taymans@gmail.com>
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

#ifndef __GST_VIDEO_H__
#define __GST_VIDEO_H__

#include <gst/gst.h>

G_BEGIN_DECLS

#include <gst/video/video-enumtypes.h>

/**
 * GstVideoFormat:
 * @GST_VIDEO_FORMAT_UNKNOWN: Unknown or unset video format id
 * @GST_VIDEO_FORMAT_I420: planar 4:2:0 YUV
 * @GST_VIDEO_FORMAT_YV12: planar 4:2:0 YVU (like I420 but UV planes swapped)
 * @GST_VIDEO_FORMAT_YUY2: packed 4:2:2 YUV (Y0-U0-Y1-V0 Y2-U2-Y3-V2 Y4 ...)
 * @GST_VIDEO_FORMAT_UYVY: packed 4:2:2 YUV (U0-Y0-V0-Y1 U2-Y2-V2-Y3 U4 ...)
 * @GST_VIDEO_FORMAT_AYUV: packed 4:4:4 YUV with alpha channel (A0-Y0-U0-V0 ...)
 * @GST_VIDEO_FORMAT_RGBx: sparse rgb packed into 32 bit, space last
 * @GST_VIDEO_FORMAT_BGRx: sparse reverse rgb packed into 32 bit, space last
 * @GST_VIDEO_FORMAT_xRGB: sparse rgb packed into 32 bit, space first
 * @GST_VIDEO_FORMAT_xBGR: sparse reverse rgb packed into 32 bit, space first
 * @GST_VIDEO_FORMAT_RGBA: rgb with alpha channel last
 * @GST_VIDEO_FORMAT_BGRA: reverse rgb with alpha channel last
 * @GST_VIDEO_FORMAT_ARGB: rgb with alpha channel first
 * @GST_VIDEO_FORMAT_ABGR: reverse rgb with alpha channel first
 * @GST_VIDEO_FORMAT_RGB: rgb
 * @GST_VIDEO_FORMAT_BGR: reverse rgb
 * @GST_VIDEO_FORMAT_Y41B: planar 4:1:1 YUV (Since: 0.10.18)
 * @GST_VIDEO_FORMAT_Y42B: planar 4:2:2 YUV (Since: 0.10.18)
 * @GST_VIDEO_FORMAT_YVYU: packed 4:2:2 YUV (Y0-V0-Y1-U0 Y2-V2-Y3-U2 Y4 ...) (Since: 0.10.23)
 * @GST_VIDEO_FORMAT_Y444: planar 4:4:4 YUV (Since: 0.10.24)
 * @GST_VIDEO_FORMAT_v210: packed 4:2:2 10-bit YUV, complex format (Since: 0.10.24)
 * @GST_VIDEO_FORMAT_v216: packed 4:2:2 16-bit YUV, Y0-U0-Y1-V1 order (Since: 0.10.24)
 * @GST_VIDEO_FORMAT_NV12: planar 4:2:0 YUV with interleaved UV plane (Since: 0.10.26)
 * @GST_VIDEO_FORMAT_NV21: planar 4:2:0 YUV with interleaved VU plane (Since: 0.10.26)
 * @GST_VIDEO_FORMAT_GRAY8: 8-bit grayscale (Since: 0.10.29)
 * @GST_VIDEO_FORMAT_GRAY16_BE: 16-bit grayscale, most significant byte first (Since: 0.10.29)
 * @GST_VIDEO_FORMAT_GRAY16_LE: 16-bit grayscale, least significant byte first (Since: 0.10.29)
 * @GST_VIDEO_FORMAT_v308: packed 4:4:4 YUV (Since: 0.10.29)
 * @GST_VIDEO_FORMAT_Y800: same as GST_VIDEO_FORMAT_GRAY8 (Since: 0.10.30)
 * @GST_VIDEO_FORMAT_Y16: same as GST_VIDEO_FORMAT_GRAY16_LE (Since: 0.10.30)
 * @GST_VIDEO_FORMAT_RGB16: rgb 5-6-5 bits per component (Since: 0.10.30)
 * @GST_VIDEO_FORMAT_BGR16: reverse rgb 5-6-5 bits per component (Since: 0.10.30)
 * @GST_VIDEO_FORMAT_RGB15: rgb 5-5-5 bits per component (Since: 0.10.30)
 * @GST_VIDEO_FORMAT_BGR15: reverse rgb 5-5-5 bits per component (Since: 0.10.30)
 * @GST_VIDEO_FORMAT_UYVP: packed 10-bit 4:2:2 YUV (U0-Y0-V0-Y1 U2-Y2-V2-Y3 U4 ...) (Since: 0.10.31)
 * @GST_VIDEO_FORMAT_A420: planar 4:4:2:0 AYUV (Since: 0.10.31)
 * @GST_VIDEO_FORMAT_RGB8_PALETTED: 8-bit paletted RGB (Since: 0.10.32)
 * @GST_VIDEO_FORMAT_YUV9: planar 4:1:0 YUV (Since: 0.10.32)
 * @GST_VIDEO_FORMAT_YVU9: planar 4:1:0 YUV (like YUV9 but UV planes swapped) (Since: 0.10.32)
 * @GST_VIDEO_FORMAT_IYU1: packed 4:1:1 YUV (Cb-Y0-Y1-Cr-Y2-Y3 ...) (Since: 0.10.32)
 * @GST_VIDEO_FORMAT_ARGB64: rgb with alpha channel first, 16 bits per channel (Since: 0.10.33)
 * @GST_VIDEO_FORMAT_AYUV64: packed 4:4:4 YUV with alpha channel, 16 bits per channel (A0-Y0-U0-V0 ...) (Since: 0.10.33)
 * @GST_VIDEO_FORMAT_r210: packed 4:4:4 RGB, 10 bits per channel (Since: 0.10.33)
 *
 * Enum value describing the most common video formats.
 */
typedef enum {
  GST_VIDEO_FORMAT_UNKNOWN,
  GST_VIDEO_FORMAT_I420,
  GST_VIDEO_FORMAT_YV12,
  GST_VIDEO_FORMAT_YUY2,
  GST_VIDEO_FORMAT_UYVY,
  GST_VIDEO_FORMAT_AYUV,
  GST_VIDEO_FORMAT_RGBx,
  GST_VIDEO_FORMAT_BGRx,
  GST_VIDEO_FORMAT_xRGB,
  GST_VIDEO_FORMAT_xBGR,
  GST_VIDEO_FORMAT_RGBA,
  GST_VIDEO_FORMAT_BGRA,
  GST_VIDEO_FORMAT_ARGB,
  GST_VIDEO_FORMAT_ABGR,
  GST_VIDEO_FORMAT_RGB,
  GST_VIDEO_FORMAT_BGR,
  GST_VIDEO_FORMAT_Y41B,
  GST_VIDEO_FORMAT_Y42B,
  GST_VIDEO_FORMAT_YVYU,
  GST_VIDEO_FORMAT_Y444,
  GST_VIDEO_FORMAT_v210,
  GST_VIDEO_FORMAT_v216,
  GST_VIDEO_FORMAT_NV12,
  GST_VIDEO_FORMAT_NV21,
  GST_VIDEO_FORMAT_GRAY8,
  GST_VIDEO_FORMAT_GRAY16_BE,
  GST_VIDEO_FORMAT_GRAY16_LE,
  GST_VIDEO_FORMAT_v308,
  GST_VIDEO_FORMAT_Y800,
  GST_VIDEO_FORMAT_Y16,
  GST_VIDEO_FORMAT_RGB16,
  GST_VIDEO_FORMAT_BGR16,
  GST_VIDEO_FORMAT_RGB15,
  GST_VIDEO_FORMAT_BGR15,
  GST_VIDEO_FORMAT_UYVP,
  GST_VIDEO_FORMAT_A420,
  GST_VIDEO_FORMAT_RGB8_PALETTED,
  GST_VIDEO_FORMAT_YUV9,
  GST_VIDEO_FORMAT_YVU9,
  GST_VIDEO_FORMAT_IYU1,
  GST_VIDEO_FORMAT_ARGB64,
  GST_VIDEO_FORMAT_AYUV64,
  GST_VIDEO_FORMAT_r210
} GstVideoFormat;

#define GST_VIDEO_MAX_PLANES 4
#define GST_VIDEO_MAX_COMPONENTS 4

typedef struct _GstVideoFormatInfo GstVideoFormatInfo;

/**
 * GstVideoFormatFlags:
 * @GST_VIDEO_FORMAT_FLAG_YUV: The video format is YUV, components are numbered
 *   0=Y, 1=U, 2=V.
 * @GST_VIDEO_FORMAT_FLAG_RGB: The video format is RGB, components are numbered
 *   0=R, 1=G, 2=B.
 * @GST_VIDEO_FORMAT_FLAG_GRAY: The video is gray, there is one gray component
 *   with index 0.
 * @GST_VIDEO_FORMAT_FLAG_ALPHA: The video format has an alpha components with
 *   the number 3.
 * @GST_VIDEO_FORMAT_FLAG_LE: The video format has data stored in little
 *   endianness.
 * @GST_VIDEO_FORMAT_FLAG_PALETTE: The video format has a palette.
 * @GST_VIDEO_FORMAT_FLAG_COMPLEX: The video format has a complex layout that
 *   can't be described with the usual information in the #GstVideoFormatInfo.
 *
 * The different video flags that a format info can have.
 */
typedef enum
{
  GST_VIDEO_FORMAT_FLAG_YUV      = (1 << 0),
  GST_VIDEO_FORMAT_FLAG_RGB      = (1 << 1),
  GST_VIDEO_FORMAT_FLAG_GRAY     = (1 << 2),
  GST_VIDEO_FORMAT_FLAG_ALPHA    = (1 << 3),
  GST_VIDEO_FORMAT_FLAG_LE       = (1 << 4),
  GST_VIDEO_FORMAT_FLAG_PALETTE  = (1 << 5),
  GST_VIDEO_FORMAT_FLAG_COMPLEX  = (1 << 6)
} GstVideoFormatFlags;

#define GST_VIDEO_COMP_Y  0
#define GST_VIDEO_COMP_U  1
#define GST_VIDEO_COMP_V  2

#define GST_VIDEO_COMP_R  0
#define GST_VIDEO_COMP_G  1
#define GST_VIDEO_COMP_B  2

#define GST_VIDEO_COMP_A  3

/**
 * GstVideoFormatUnpack:
 * @info: a #GstVideoFormatInfo
 * @dest: a destination array
 * @data: pointers to the data planes
 * @stride: strides of the planes
 * @x: the x position in the image to start from
 * @y: the y position in the image to start from
 * @width: the amount of pixels to unpack.
 *
 * Unpacks @width pixels from the given planes and strides containing data of
 * format @info. The pixels will be unpacked into @dest which each component
 * interleaved. @dest should at least be big enough to hold @width *
 * n_components * size(unpack_format) bytes.
 */
typedef void (*GstVideoFormatUnpack)         (GstVideoFormatInfo *info, gpointer dest,
                                              const gpointer data[GST_VIDEO_MAX_PLANES],
                                              const gint stride[GST_VIDEO_MAX_PLANES],
                                              gint x, gint y, gint width);
/**
 * GstVideoFormatPack:
 * @info: a #GstVideoFormatInfo
 * @src: a source array
 * @data: pointers to the destination data planes
 * @stride: strides of the destination planes
 * @x: the x position in the image to pack to
 * @y: the y position in the image to pack to
 * @width: the amount of pixels to pack.
 *
 * Packs @width pixels from @src to the given planes and strides in the
 * format @info. The pixels from source have each component interleaved
 * and will be packed into the planes in @data.
 */
typedef void (*GstVideoFormatPack)           (GstVideoFormatInfo *info, const gpointer src,
                                              gpointer data[GST_VIDEO_MAX_PLANES],
                                              const gint stride[GST_VIDEO_MAX_PLANES],
                                              gint x, gint y, gint width);

/**
 * GstVideoFormatInfo:
 * @format: #GstVideoFormat
 * @name: string representation of the format
 * @description: use readable description of the format
 * @flags: #GstVideoFormatFlags
 * @bits: The number of bits used to pack data items. This can be less than 8
 *    when multiple pixels are stored in a byte. for values > 8 multiple bytes
 *    should be read according to the endianness flag before applying the shift
 *    and mask.
 * @n_components: the number of components in the video format.
 * @shift: the number of bits to shift away to get the component data
 * @depth: the depth in bits for each component
 * @pixel_stride: the pixel stride of each component. This is the amount of
 *    bytes to the pixel immediately to the right. When bits < 8, the stride is
 *    expressed in bits.
 * @n_planes: the number of planes for this format. The number of planes can be
 *    less than the amount of components when multiple components are packed into
 *    one plane.
 * @plane: the plane number where a component can be found
 * @poffset: the offset in the plane where the first pixel of the components
 *    can be found. If bits < 8 the amount is specified in bits.
 * @w_sub: subsampling factor of the width for the component. Use
 *     GST_VIDEO_SUB_SCALE to scale a width.
 * @h_sub: subsampling factor of the height for the component. Use
 *     GST_VIDEO_SUB_SCALE to scale a height.
 * @unpack_format: the format of the unpacked pixels.
 * @unpack_func: an unpack function for this format
 * @pack_func: an pack function for this format
 *
 * Information for a video format.
 */
struct _GstVideoFormatInfo {
  GstVideoFormat format;
  const gchar *name;
  const gchar *description;
  GstVideoFormatFlags flags;
  guint bits;
  guint n_components;
  guint shift[GST_VIDEO_MAX_COMPONENTS];
  guint depth[GST_VIDEO_MAX_COMPONENTS];
  gint  pixel_stride[GST_VIDEO_MAX_COMPONENTS];
  guint n_planes;
  guint plane[GST_VIDEO_MAX_COMPONENTS];
  guint poffset[GST_VIDEO_MAX_COMPONENTS];
  guint w_sub[GST_VIDEO_MAX_COMPONENTS];
  guint h_sub[GST_VIDEO_MAX_COMPONENTS];

  GstVideoFormat unpack_format;
  GstVideoFormatUnpack unpack_func;
  GstVideoFormatPack pack_func;
};

#define GST_VIDEO_FORMAT_INFO_FORMAT(info)       ((info)->format)
#define GST_VIDEO_FORMAT_INFO_NAME(info)         ((info)->name)
#define GST_VIDEO_FORMAT_INFO_FLAGS(info)        ((info)->flags)

#define GST_VIDEO_FORMAT_INFO_IS_YUV(info)       ((info)->flags & GST_VIDEO_FORMAT_FLAG_YUV)
#define GST_VIDEO_FORMAT_INFO_IS_RGB(info)       ((info)->flags & GST_VIDEO_FORMAT_FLAG_RGB)
#define GST_VIDEO_FORMAT_INFO_IS_GRAY(info)      ((info)->flags & GST_VIDEO_FORMAT_FLAG_GRAY)
#define GST_VIDEO_FORMAT_INFO_HAS_ALPHA(info)    ((info)->flags & GST_VIDEO_FORMAT_FLAG_ALPHA)
#define GST_VIDEO_FORMAT_INFO_IS_LE(info)        ((info)->flags & GST_VIDEO_FORMAT_FLAG_LE)
#define GST_VIDEO_FORMAT_INFO_HAS_PALETTE(info)  ((info)->flags & GST_VIDEO_FORMAT_FLAG_PALETTE)

#define GST_VIDEO_FORMAT_INFO_BITS(info)         ((info)->bits)
#define GST_VIDEO_FORMAT_INFO_N_COMPONENTS(info) ((info)->n_components)
#define GST_VIDEO_FORMAT_INFO_SHIFT(info,c)      ((info)->shift[c])
#define GST_VIDEO_FORMAT_INFO_DEPTH(info,c)      ((info)->depth[c])
#define GST_VIDEO_FORMAT_INFO_PSTRIDE(info,c)    ((info)->pixel_stride[c])
#define GST_VIDEO_FORMAT_INFO_N_PLANES(info)     ((info)->n_planes)
#define GST_VIDEO_FORMAT_INFO_PLANE(info,c)      ((info)->plane[c])
#define GST_VIDEO_FORMAT_INFO_POFFSET(info,c)    ((info)->poffset[c])
#define GST_VIDEO_FORMAT_INFO_W_SUB(info,c)      ((info)->w_sub[c])
#define GST_VIDEO_FORMAT_INFO_H_SUB(info,c)      ((info)->h_sub[c])

#define GST_VIDEO_SUB_SCALE(scale,val)   (-((-((gint)val))>>(scale)))

#define GST_VIDEO_FORMAT_INFO_SCALE_WIDTH(info,c,w)  GST_VIDEO_SUB_SCALE ((info)->w_sub[(c)],(w))
#define GST_VIDEO_FORMAT_INFO_SCALE_HEIGHT(info,c,h) GST_VIDEO_SUB_SCALE ((info)->h_sub[(c)],(h))

#define GST_VIDEO_FORMAT_INFO_DATA(info,planes,comp) \
  (((guint8*)(planes)[info->plane[comp]]) + info->poffset[comp])
#define GST_VIDEO_FORMAT_INFO_STRIDE(info,strides,comp) ((strides)[info->plane[comp]])
#define GST_VIDEO_FORMAT_INFO_OFFSET(info,offsets,comp) \
  (((offsets)[info->plane[comp]]) + info->poffset[comp])

/* format properties */
GstVideoFormat gst_video_format_from_masks           (gint depth, gint bpp, gint endianness,
                                                      gint red_mask, gint green_mask,
                                                      gint blue_mask, gint alpha_mask) G_GNUC_CONST;

GstVideoFormat gst_video_format_from_fourcc          (guint32 fourcc) G_GNUC_CONST;
GstVideoFormat gst_video_format_from_string          (const gchar *format) G_GNUC_CONST;

guint32        gst_video_format_to_fourcc            (GstVideoFormat format) G_GNUC_CONST;
const gchar *  gst_video_format_to_string            (GstVideoFormat format) G_GNUC_CONST;

const GstVideoFormatInfo *
               gst_video_format_get_info             (GstVideoFormat format) G_GNUC_CONST;

typedef struct _GstVideoInfo GstVideoInfo;
typedef struct _GstVideoFrame GstVideoFrame;

/**
 * GstVideoInterlaceMode:
 * @GST_VIDEO_INTERLACE_MODE_PROGRESSIVE: all frames are progressive
 * @GST_VIDEO_INTERLACE_MODE_INTERLEAVED: video is interlaced and all fields
 *     are interlaced in one frame.
 * @GST_VIDEO_INTERLACE_MODE_MIXED: video contains both interlaced and
 *     progressive frames, the buffer flags describe the frame and fields.
 * @GST_VIDEO_INTERLACE_MODE_FIELDS: video is interlaced and fields are stored
 *     separately. Use the id property to get access to the required field.
 *
 * The possible values of the #GstVideoInterlaceMode describing the interlace
 * mode of the stream.
 */
typedef enum {
  GST_VIDEO_INTERLACE_MODE_PROGRESSIVE = 0,
  GST_VIDEO_INTERLACE_MODE_INTERLEAVED,
  GST_VIDEO_INTERLACE_MODE_MIXED,
  GST_VIDEO_INTERLACE_MODE_FIELDS
} GstVideoInterlaceMode;

/**
 * GstVideoFlags:
 * @GST_VIDEO_FLAG_NONE: no flags
 * @GST_VIDEO_FLAG_INTERLACED: The video is interlaced
 * @GST_VIDEO_FLAG_TFF: The video has the top field first
 * @GST_VIDEO_FLAG_RFF: The video has the repeat flag
 * @GST_VIDEO_FLAG_ONEFIELD: one field
 * @GST_VIDEO_FLAG_VARIABLE_FPS: a variable fps is selected, fps_n and fps_d
 * denote the maximum fps of the video
 *
 * Extra video flags
 */
typedef enum {
  GST_VIDEO_FLAG_NONE         = 0,
  GST_VIDEO_FLAG_INTERLACED   = (1 << 0),
  GST_VIDEO_FLAG_TFF          = (1 << 1),
  GST_VIDEO_FLAG_RFF          = (1 << 2),
  GST_VIDEO_FLAG_ONEFIELD     = (1 << 3),
  GST_VIDEO_FLAG_VARIABLE_FPS = (1 << 4)
} GstVideoFlags;

/**
 * GstVideoChroma:
 * @GST_VIDEO_CHROMA_SITE_UNKNOWN: unknown cositing
 * @GST_VIDEO_CHROMA_SITE_NONE: no cositing
 * @GST_VIDEO_CHROMA_SITE_H_COSITED: chroma is horizontally cosited
 * @GST_VIDEO_CHROMA_SITE_V_COSITED: chroma is vertically cosited
 * @GST_VIDEO_CHROMA_SITE_ALT_LINE: choma samples are sited on alternate lines
 * @GST_VIDEO_CHROMA_SITE_COSITED: chroma samples cosited with luma samples
 * @GST_VIDEO_CHROMA_SITE_JPEG: jpeg style cositing, also for mpeg1 and mjpeg
 * @GST_VIDEO_CHROMA_SITE_MPEG2: mpeg2 style cositing
 * @GST_VIDEO_CHROMA_SITE_DV: DV style cositing
 *
 * Various Chroma sitings.
 */
typedef enum {
  GST_VIDEO_CHROMA_SITE_UNKNOWN   =  0,
  GST_VIDEO_CHROMA_SITE_NONE      = (1 << 0),
  GST_VIDEO_CHROMA_SITE_H_COSITED = (1 << 1),
  GST_VIDEO_CHROMA_SITE_V_COSITED = (1 << 2),
  GST_VIDEO_CHROMA_SITE_ALT_LINE  = (1 << 3),
  /* some common chroma cositing */
  GST_VIDEO_CHROMA_SITE_COSITED   = (GST_VIDEO_CHROMA_SITE_H_COSITED | GST_VIDEO_CHROMA_SITE_V_COSITED),
  GST_VIDEO_CHROMA_SITE_JPEG      = (GST_VIDEO_CHROMA_SITE_NONE),
  GST_VIDEO_CHROMA_SITE_MPEG2     = (GST_VIDEO_CHROMA_SITE_H_COSITED),
  GST_VIDEO_CHROMA_SITE_DV        = (GST_VIDEO_CHROMA_SITE_COSITED | GST_VIDEO_CHROMA_SITE_ALT_LINE),
} GstVideoChromaSite;

/**
 * GstVideoColorRange:
 * @GST_VIDEO_COLOR_RANGE_UNKNOWN: unknown range
 * @GST_VIDEO_COLOR_RANGE_0_255: [0..255] for 8 bit components
 * @GST_VIDEO_COLOR_RANGE_16_235: [16..235] for 8 bit components. Chroma has
 *                 [16..240] range.
 *
 * Possible color range values. These constants are defined for 8 bit color
 * values and can be scaled for other bit depths.
 */
typedef enum {
  GST_VIDEO_COLOR_RANGE_UNKNOWN = 0,
  GST_VIDEO_COLOR_RANGE_0_255,
  GST_VIDEO_COLOR_RANGE_16_235
} GstVideoColorRange;

/**
 * GstVideoColorMatrix:
 * @GST_VIDEO_COLOR_MATRIX_UNKNOWN: unknown matrix
 * @GST_VIDEO_COLOR_MATRIX_RGB: identity matrix
 * @GST_VIDEO_COLOR_MATRIX_FCC: FCC color matrix
 * @GST_VIDEO_COLOR_MATRIX_BT709: ITU-R BT.709 color matrix
 * @GST_VIDEO_COLOR_MATRIX_BT601: ITU-R BT.601 color matrix
 * @GST_VIDEO_COLOR_MATRIX_SMPTE240M: SMPTE 240M color matrix
 *
 * The color matrix is used to convert between Y'PbPr and
 * non-linear RGB (R'G'B')
 */
typedef enum {
  GST_VIDEO_COLOR_MATRIX_UNKNOWN = 0,
  GST_VIDEO_COLOR_MATRIX_RGB,
  GST_VIDEO_COLOR_MATRIX_FCC,
  GST_VIDEO_COLOR_MATRIX_BT709,
  GST_VIDEO_COLOR_MATRIX_BT601,
  GST_VIDEO_COLOR_MATRIX_SMPTE240M
} GstVideoColorMatrix;

/**
 * GstVideoTransferFunction:
 * @GST_VIDEO_TRANSFER_UNKNOWN: unknown transfer function
 * @GST_VIDEO_TRANSFER_GAMMA10: linear RGB, gamma 1.0 curve
 * @GST_VIDEO_TRANSFER_GAMMA18: Gamma 1.8 curve
 * @GST_VIDEO_TRANSFER_GAMMA20: Gamma 2.0 curve
 * @GST_VIDEO_TRANSFER_GAMMA22: Gamma 2.2 curve
 * @GST_VIDEO_TRANSFER_BT709: Gamma 2.2 curve with a linear segment in the lower
 *                           range
 * @GST_VIDEO_TRANSFER_SMPTE240M: Gamma 2.2 curve with a linear segment in the
 *                               lower range
 * @GST_VIDEO_TRANSFER_SRGB: Gamma 2.4 curve with a linear segment in the lower
 *                          range
 * @GST_VIDEO_TRANSFER_GAMMA28: Gamma 2.8 curve
 * @GST_VIDEO_TRANSFER_LOG100: Logarithmic transfer characteristic
 *                             100:1 range
 * @GST_VIDEO_TRANSFER_LOG316: Logarithmic transfer characteristic
 *                             316.22777:1 range
 *
 * The video transfer function defines the formula for converting between
 * non-linear RGB (R'G'B') and linear RGB
 */
typedef enum {
  GST_VIDEO_TRANSFER_UNKNOWN = 0,
  GST_VIDEO_TRANSFER_GAMMA10,
  GST_VIDEO_TRANSFER_GAMMA18,
  GST_VIDEO_TRANSFER_GAMMA20,
  GST_VIDEO_TRANSFER_GAMMA22,
  GST_VIDEO_TRANSFER_BT709,
  GST_VIDEO_TRANSFER_SMPTE240M,
  GST_VIDEO_TRANSFER_SRGB,
  GST_VIDEO_TRANSFER_GAMMA28,
  GST_VIDEO_TRANSFER_LOG100,
  GST_VIDEO_TRANSFER_LOG316
} GstVideoTransferFunction;

/**
 * GstVideoColorPrimaries:
 * @GST_VIDEO_COLOR_PRIMARIES_UNKNOWN: unknown color primaries
 * @GST_VIDEO_COLOR_PRIMARIES_BT709: BT709 primaries
 * @GST_VIDEO_COLOR_PRIMARIES_BT470M: BT470M primaries
 * @GST_VIDEO_COLOR_PRIMARIES_BT470BG: BT470BG primaries
 * @GST_VIDEO_COLOR_PRIMARIES_SMPTE170M: SMPTE170M primaries
 * @GST_VIDEO_COLOR_PRIMARIES_SMPTE240M: SMPTE240M primaries
 *
 * The color primaries define the how to transform linear RGB values to and from
 * the CIE XYZ colorspace.
 */
typedef enum {
  GST_VIDEO_COLOR_PRIMARIES_UNKNOWN = 0,
  GST_VIDEO_COLOR_PRIMARIES_BT709,
  GST_VIDEO_COLOR_PRIMARIES_BT470M,
  GST_VIDEO_COLOR_PRIMARIES_BT470BG,
  GST_VIDEO_COLOR_PRIMARIES_SMPTE170M,
  GST_VIDEO_COLOR_PRIMARIES_SMPTE240M
} GstVideoColorPrimaries;

/**
 * GstVideoColorimetry:
 * @range: the color range. This is the valid range for the samples.
 *         It is used to convert the samples to Y'PbPr values.
 * @matrix: the color matrix. Used to convert between Y'PbPr and
 *          non-linear RGB (R'G'B')
 * @transfer: the transfer function. used to convert between R'G'B' and RGB
 * @primaries: color primaries. used to convert between R'G'B' and CIE XYZ
 *
 * Structure describing the color info.
 */
typedef struct {
  GstVideoColorRange        range;
  GstVideoColorMatrix       matrix;
  GstVideoTransferFunction  transfer;
  GstVideoColorPrimaries    primaries;
} GstVideoColorimetry;

/* predefined colorimetry */
#define GST_VIDEO_COLORIMETRY_BT601       "bt601"
#define GST_VIDEO_COLORIMETRY_BT709       "bt709"
#define GST_VIDEO_COLORIMETRY_SMPTE240M   "smpte240m"

gboolean     gst_video_colorimetry_matches     (GstVideoColorimetry *cinfo, const gchar *color);
gboolean     gst_video_colorimetry_from_string (GstVideoColorimetry *cinfo, const gchar *color);
gchar *      gst_video_colorimetry_to_string   (GstVideoColorimetry *cinfo);

/**
 * GstVideoInfo:
 * @finfo: the format info of the video
 * @interlace_mode: the interlace mode
 * @flags: additional video flags
 * @width: the width of the video
 * @height: the height of the video
 * @views: the number of views for multiview video
 * @size: the default size of one frame
 * @chroma_site: a #GstVideoChromaSite.
 * @colorimetry: the colorimetry info
 * @palette: a buffer with palette data
 * @par_n: the pixel-aspect-ratio numerator
 * @par_d: the pixel-aspect-ratio demnominator
 * @fps_n: the framerate numerator
 * @fps_d: the framerate demnominator
 * @offset: offsets of the planes
 * @stride: strides of the planes
 *
 * Information describing image properties. This information can be filled
 * in from GstCaps with gst_video_info_from_caps(). The information is also used
 * to store the specific video info when mapping a video frame with
 * gst_video_frame_map().
 *
 * Use the provided macros to access the info in this structure.
 */
struct _GstVideoInfo {
  const GstVideoFormatInfo *finfo;

  GstVideoInterlaceMode     interlace_mode;
  GstVideoFlags             flags;
  gint                      width;
  gint                      height;
  gsize                     size;
  gint                      views;

  GstVideoChromaSite        chroma_site;
  GstVideoColorimetry       colorimetry;

  GstBuffer                *palette;

  gint                      par_n;
  gint                      par_d;
  gint                      fps_n;
  gint                      fps_d;

  gsize                     offset[GST_VIDEO_MAX_PLANES];
  gint                      stride[GST_VIDEO_MAX_PLANES];
};

/* general info */
#define GST_VIDEO_INFO_FORMAT(i)         (GST_VIDEO_FORMAT_INFO_FORMAT((i)->finfo))
#define GST_VIDEO_INFO_NAME(i)           (GST_VIDEO_FORMAT_INFO_NAME((i)->finfo))
#define GST_VIDEO_INFO_IS_YUV(i)         (GST_VIDEO_FORMAT_INFO_IS_YUV((i)->finfo))
#define GST_VIDEO_INFO_IS_RGB(i)         (GST_VIDEO_FORMAT_INFO_IS_RGB((i)->finfo))
#define GST_VIDEO_INFO_IS_GRAY(i)        (GST_VIDEO_FORMAT_INFO_IS_GRAY((i)->finfo))
#define GST_VIDEO_INFO_HAS_ALPHA(i)      (GST_VIDEO_FORMAT_INFO_HAS_ALPHA((i)->finfo))

#define GST_VIDEO_INFO_INTERLACE_MODE(i) ((i)->interlace_mode)
#define GST_VIDEO_INFO_IS_INTERLACED(i)  ((i)->interlace_mode != GST_VIDEO_INTERLACE_MODE_PROGRESSIVE)
#define GST_VIDEO_INFO_FLAGS(i)          ((i)->flags)
#define GST_VIDEO_INFO_WIDTH(i)          ((i)->width)
#define GST_VIDEO_INFO_HEIGHT(i)         ((i)->height)
#define GST_VIDEO_INFO_SIZE(i)           ((i)->size)
#define GST_VIDEO_INFO_VIEWS(i)          ((i)->views)
#define GST_VIDEO_INFO_PAR_N(i)          ((i)->par_n)
#define GST_VIDEO_INFO_PAR_D(i)          ((i)->par_d)
#define GST_VIDEO_INFO_FPS_N(i)          ((i)->fps_n)
#define GST_VIDEO_INFO_FPS_D(i)          ((i)->fps_d)

/* dealing with GstVideoInfo flags */
#define GST_VIDEO_INFO_FLAG_IS_SET(i,flag) ((GST_VIDEO_INFO_FLAGS(i) & (flag)) == (flag))
#define GST_VIDEO_INFO_FLAG_SET(i,flag)    (GST_VIDEO_INFO_FLAGS(i) |= (flag))
#define GST_VIDEO_INFO_FLAG_UNSET(i,flag)  (GST_VIDEO_INFO_FLAGS(i) &= ~(flag))

/* dealing with planes */
#define GST_VIDEO_INFO_N_PLANES(i)       (GST_VIDEO_FORMAT_INFO_N_PLANES((i)->finfo))
#define GST_VIDEO_INFO_PLANE_OFFSET(i,p) ((i)->offset[p])
#define GST_VIDEO_INFO_PLANE_STRIDE(i,p) ((i)->stride[p])

/* dealing with components */
#define GST_VIDEO_INFO_N_COMPONENTS(i)   GST_VIDEO_FORMAT_INFO_N_COMPONENTS((i)->finfo)
#define GST_VIDEO_INFO_COMP_DEPTH(i,c)   GST_VIDEO_FORMAT_INFO_DEPTH((i)->finfo,c)
#define GST_VIDEO_INFO_COMP_BITS(i,c)    GST_VIDEO_FORMAT_INFO_BITS((i)->finfo,c)
#define GST_VIDEO_INFO_COMP_DATA(i,d,c)  GST_VIDEO_FORMAT_INFO_DATA((i)->finfo,d,c)
#define GST_VIDEO_INFO_COMP_OFFSET(i,c)  GST_VIDEO_FORMAT_INFO_OFFSET((i)->finfo,(i)->offset,c)
#define GST_VIDEO_INFO_COMP_STRIDE(i,c)  GST_VIDEO_FORMAT_INFO_STRIDE((i)->finfo,(i)->stride,c)
#define GST_VIDEO_INFO_COMP_WIDTH(i,c)   GST_VIDEO_FORMAT_INFO_SCALE_WIDTH((i)->finfo,c,(i)->width)
#define GST_VIDEO_INFO_COMP_HEIGHT(i,c)  GST_VIDEO_FORMAT_INFO_SCALE_HEIGHT((i)->finfo,c,(i)->height)
#define GST_VIDEO_INFO_COMP_PLANE(i,c)   GST_VIDEO_FORMAT_INFO_PLANE((i)->finfo,c)
#define GST_VIDEO_INFO_COMP_PSTRIDE(i,c) GST_VIDEO_FORMAT_INFO_PSTRIDE((i)->finfo,c)
#define GST_VIDEO_INFO_COMP_POFFSET(i,c) GST_VIDEO_FORMAT_INFO_POFFSET((i)->finfo,c)

void         gst_video_info_init        (GstVideoInfo *info);

void         gst_video_info_set_format  (GstVideoInfo *info, GstVideoFormat format,
                                         guint width, guint height);

gboolean     gst_video_info_from_caps   (GstVideoInfo *info, const GstCaps  * caps);

GstCaps *    gst_video_info_to_caps     (GstVideoInfo *info);

gboolean     gst_video_info_convert     (GstVideoInfo *info,
                                         GstFormat     src_format,
                                         gint64        src_value,
                                         GstFormat     dest_format,
                                         gint64       *dest_value);

/**
 * GstVideoFrame:
 * @info: the #GstVideoInfo
 * @buffer: the mapped buffer
 * @meta: pointer to metadata if any
 * @id: id of the mapped frame. the id can for example be used to
 *   indentify the frame in case of multiview video.
 * @data: pointers to the plane data
 * @map: mappings of the planes
 *
 * A video frame obtained from gst_video_frame_map()
 */
struct _GstVideoFrame {
  GstVideoInfo info;

  GstBuffer *buffer;
  gpointer   meta;
  gint       id;

  gpointer   data[GST_VIDEO_MAX_PLANES];
  GstMapInfo map[GST_VIDEO_MAX_PLANES];
};

gboolean    gst_video_frame_map           (GstVideoFrame *frame, GstVideoInfo *info,
                                           GstBuffer *buffer, GstMapFlags flags);
gboolean    gst_video_frame_map_id        (GstVideoFrame *frame, GstVideoInfo *info,
                                           GstBuffer *buffer, gint id, GstMapFlags flags);
void        gst_video_frame_unmap         (GstVideoFrame *frame);

gboolean    gst_video_frame_copy          (GstVideoFrame *dest, const GstVideoFrame *src);
gboolean    gst_video_frame_copy_plane    (GstVideoFrame *dest, const GstVideoFrame *src,
                                           guint plane);

/* general info */
#define GST_VIDEO_FRAME_FORMAT(f)         (GST_VIDEO_INFO_FORMAT(&(f)->info))
#define GST_VIDEO_FRAME_WIDTH(f)          (GST_VIDEO_INFO_WIDTH(&(f)->info))
#define GST_VIDEO_FRAME_HEIGHT(f)         (GST_VIDEO_INFO_HEIGHT(&(f)->info))
#define GST_VIDEO_FRAME_SIZE(f)           (GST_VIDEO_INFO_SIZE(&(f)->info))

/* dealing with planes */
#define GST_VIDEO_FRAME_N_PLANES(f)       (GST_VIDEO_INFO_N_PLANES(&(f)->info))
#define GST_VIDEO_FRAME_PLANE_DATA(f,p)   ((f)->data[p])
#define GST_VIDEO_FRAME_PLANE_OFFSET(f,p) (GST_VIDEO_INFO_PLANE_OFFSET(&(f)->info,p))
#define GST_VIDEO_FRAME_PLANE_STRIDE(f,p) (GST_VIDEO_INFO_PLANE_STRIDE(&(f)->info,p))

/* dealing with components */
#define GST_VIDEO_FRAME_N_COMPONENTS(f)   GST_VIDEO_INFO_N_COMPONENTS(&(f)->info)
#define GST_VIDEO_FRAME_COMP_DATA(f,c)    GST_VIDEO_INFO_COMP_DATA(&(f)->info,(f)->data,c)
#define GST_VIDEO_FRAME_COMP_STRIDE(f,c)  GST_VIDEO_INFO_COMP_STRIDE(&(f)->info,c)
#define GST_VIDEO_FRAME_COMP_OFFSET(f,c)  GST_VIDEO_INFO_COMP_OFFSET(&(f)->info,c)
#define GST_VIDEO_FRAME_COMP_WIDTH(f,c)   GST_VIDEO_INFO_COMP_WIDTH(&(f)->info,c)
#define GST_VIDEO_FRAME_COMP_HEIGHT(f,c)  GST_VIDEO_INFO_COMP_HEIGHT(&(f)->info,c)
#define GST_VIDEO_FRAME_COMP_PLANE(f,c)   GST_VIDEO_INFO_COMP_PLANE(&(f)->info,c)
#define GST_VIDEO_FRAME_COMP_PSTRIDE(f,c) GST_VIDEO_INFO_COMP_PSTRIDE(&(f)->info,c)
#define GST_VIDEO_FRAME_COMP_POFFSET(f,c) GST_VIDEO_INFO_COMP_POFFSET(&(f)->info,c)

#define GST_VIDEO_SIZE_RANGE "(int) [ 1, max ]"
#define GST_VIDEO_FPS_RANGE "(fraction) [ 0, max ]"

#if G_BYTE_ORDER == G_LITTLE_ENDIAN
# define GST_VIDEO_NE(s) G_STRINGIFY(s)"_LE"
# define GST_VIDEO_OE(s) G_STRINGIFY(s)"_BE"
#else
# define GST_VIDEO_NE(s) G_STRINGIFY(s)"_BE"
# define GST_VIDEO_OE(s) G_STRINGIFY(s)"_LE"
#endif

#define GST_VIDEO_FORMATS_ALL "{ I420, YV12, YUY2, UYVY, AYUV, RGBx, "  \
    "BGRx, xRGB, xBGR, RGBA, BGRA, ARGB, ABGR, RGB, BGR, Y41B, Y42B, "  \
    "YVYU, Y444, v210, v216, NV12, NV21, GRAY8, GRAY16_BE, GRAY16_LE, " \
    "v308, Y800, Y16, RGB16, BGR16, RGB15, BGR15, UYVP, A420, "         \
    "RGB8_PALETTED, YUV9, YVU9, IYU1, ARGB64, AYUV64, r210 }"

/**
 * GST_VIDEO_CAPS_MAKE:
 * @format: string format that describes the pixel layout, as string
 *     (e.g. "I420", "RGB", "YV12", "YUY2", "AYUV", etc.)
 *
 * Generic caps string for video, for use in pad templates.
 */
#define GST_VIDEO_CAPS_MAKE(format)                                     \
    "video/x-raw, "                                                     \
    "format = (string) " format ", "                                    \
    "width = " GST_VIDEO_SIZE_RANGE ", "                                \
    "height = " GST_VIDEO_SIZE_RANGE ", "                               \
    "framerate = " GST_VIDEO_FPS_RANGE

/* buffer flags */

/**
 * GstVideoBufferFlags:
 * @GST_VIDEO_BUFFER_FLAG_INTERLACED:  Mark #GstBuffer as interlaced
 * @GST_VIDEO_BUFFER_FLAG_TFF:         If the #GstBuffer is interlaced, then the first field
 *                                     in the video frame is the top field.  If unset, the
 *                                     bottom field is first.
 * @GST_VIDEO_BUFFER_FLAG_RFF:         If the #GstBuffer is interlaced, then the first field
 *                                     (as defined by the %GST_VIDEO_BUFFER_TFF flag setting)
 *                                     is repeated.
 * @GST_VIDEO_BUFFER_FLAG_ONEFIELD:    If the #GstBuffer is interlaced, then only the
 *                                     first field (as defined by the %GST_VIDEO_BUFFER_TFF
 *                                     flag setting) is to be displayed.
 *
 * Additional video buffer flags.
 */
typedef enum {
  GST_VIDEO_BUFFER_FLAG_INTERLACED  = (GST_BUFFER_FLAG_LAST << 0),
  GST_VIDEO_BUFFER_FLAG_TFF         = (GST_BUFFER_FLAG_LAST << 1),
  GST_VIDEO_BUFFER_FLAG_RFF         = (GST_BUFFER_FLAG_LAST << 2),
  GST_VIDEO_BUFFER_FLAG_ONEFIELD    = (GST_BUFFER_FLAG_LAST << 3),

  GST_VIDEO_BUFFER_FLAG_LAST        = (GST_BUFFER_FLAG_LAST << 8)
} GstVideoBufferFlags;

/* some helper functions */
gboolean       gst_video_calculate_display_ratio (guint * dar_n,
                                                  guint * dar_d,
                                                  guint   video_width,
                                                  guint   video_height,
                                                  guint   video_par_n,
                                                  guint   video_par_d,
                                                  guint   display_par_n,
                                                  guint   display_par_d);

GstBuffer *    gst_video_parse_caps_palette      (GstCaps * caps);

/* video still frame event creation and parsing */

GstEvent *     gst_video_event_new_still_frame   (gboolean in_still);

gboolean       gst_video_event_parse_still_frame (GstEvent * event, gboolean * in_still);

/* video force key unit event creation and parsing */

GstEvent * gst_video_event_new_downstream_force_key_unit (GstClockTime timestamp,
                                                          GstClockTime stream_time,
                                                          GstClockTime running_time,
                                                          gboolean all_headers,
                                                          guint count);

gboolean gst_video_event_parse_downstream_force_key_unit (GstEvent * event,
                                                          GstClockTime * timestamp,
                                                          GstClockTime * stream_time,
                                                          GstClockTime * running_time,
                                                          gboolean * all_headers,
                                                          guint * count);

GstEvent * gst_video_event_new_upstream_force_key_unit (GstClockTime running_time,
                                                        gboolean all_headers,
                                                        guint count);

gboolean gst_video_event_parse_upstream_force_key_unit (GstEvent * event,
                                                        GstClockTime * running_time,
                                                        gboolean * all_headers,
                                                        guint * count);

gboolean gst_video_event_is_force_key_unit(GstEvent *event);

/* convert/encode video sample from one format to another */

typedef void (*GstVideoConvertSampleCallback) (GstSample * sample, GError *error, gpointer user_data);

void          gst_video_convert_sample_async (GstSample                    * sample,
                                              const GstCaps                * to_caps,
                                              GstClockTime                   timeout,
                                              GstVideoConvertSampleCallback  callback,
                                              gpointer                       user_data,
                                              GDestroyNotify                 destroy_notify);

GstSample *   gst_video_convert_sample       (GstSample     * sample,
                                              const GstCaps * to_caps,
                                              GstClockTime    timeout,
                                              GError       ** error);
G_END_DECLS

#endif /* __GST_VIDEO_H__ */
