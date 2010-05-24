/* GStreamer
 * Copyright (C) 2003 Martin Soto <martinsoto@users.sourceforge.net>
 *
 * ac3_padder.h: Pad AC3 frames for use with an SPDIF interface. 
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

#ifndef AC3_PADDER_INC
#define AC3_PADDER_INC

#include <glib.h>


/* Size of an IEC958 padded AC3 frame. */
#define AC3P_IEC_FRAME_SIZE 6144
/* Size of the IEC958 header. */
#define AC3P_IEC_HEADER_SIZE 8
/* Size of the AC3 header. */
#define AC3P_AC3_HEADER_SIZE 7


/* An IEC958 padded AC3 frame. */
typedef struct {
  /* IEC header. */
  guchar header[AC3P_IEC_HEADER_SIZE];

  /* Begin of AC3 header. */
  guchar sync_byte1;
  guchar sync_byte2;

  guchar crc1[2];
  guchar code;
  guchar bsidmod;
  guchar acmod;
  /* End of AC3 header. */
  
  unsigned char data[AC3P_IEC_FRAME_SIZE - AC3P_IEC_HEADER_SIZE 
                     - AC3P_AC3_HEADER_SIZE];
} ac3p_iec958_burst_frame;


/* Possible states for the reading automaton: */

/* Searching for sync byte 1. */
#define AC3P_STATE_SYNC1 1
/* Searching for sync byte 2. */
#define AC3P_STATE_SYNC2 2
/* Reading AC3 header. */
#define AC3P_STATE_HEADER 3
/* Reading packet contents.*/
#define AC3P_STATE_CONTENT 4


/* Events generated by the parse function: */

/* The parser needs new data to be pushed. */
#define AC3P_EVENT_PUSH 1
/* There is a new padded frame ready to read from the padder structure. */
#define AC3P_EVENT_FRAME 2


/* The internal state for the padder. */
typedef struct {
  guint state;       /* State of the reading automaton. */

  guchar *buffer;    /* Input buffer */

  gint buffer_end;   /* End offset, in bytes, of currently valid data in
                        buffer */

  gint buffer_size;  /* Allocated size of buffer */

  gint buffer_cur;   /* Current position in buffer */

  guchar *out_ptr;   /* Output pointer, marking the current
                        position in the output frame. */
  gint bytes_to_copy;
                     /* Number of bytes that still must be copied
                        to the output frame *during this reading
                        stage*. */

  gint ac3_frame_size;
                     /* The size in 16-bit units of the pure AC3 portion
                        of the current frame. */

  gint skipped;      /* Number of bytes skipped while trying to find sync */

  gint rate;         /* Sample rate of ac3 data */

  ac3p_iec958_burst_frame frame;
                     /* The current output frame. */
} ac3_padder;



extern void
ac3p_init(ac3_padder *padder);

extern void
ac3p_clear(ac3_padder *padder);

extern void
ac3p_push_data(ac3_padder *padder, guchar *data, guint size);

extern int
ac3p_parse(ac3_padder *padder);


/**
 * ac3p_frame
 * @padder: The padder structure.
 *
 * Returns: a pointer to the padded frame contained in the padder.
 */
#define ac3p_frame(padder) ((guint8 *) &((padder)->frame))

/**
 * ac3p_frame_size
 * @padder: The padder structure.
 *
 * Returns: the length in bytes of the last read raw AC3 frame.
 */
#define ac3p_frame_size(padder) ((padder)->ac3_frame_size * 2)

#endif
