/* GStreamer Wavpack plugin
 * Copyright (c) 2006 Sebastian Dröge <slomo@circular-chaos.org>
 *
 * gstwavpackstreamreader.h: stream reader used for decoding
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

#ifndef __GST_WAVPACK_STREAM_READER_H__
#define __GST_WAVPACK_STREAM_READER_H__

#include <wavpack/wavpack.h>

typedef struct
{
  guint8 *buffer;
  uint32_t length;
  uint32_t position;
} read_id;

WavpackStreamReader *gst_wavpack_stream_reader_new (void);

#endif
