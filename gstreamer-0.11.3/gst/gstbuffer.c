/* GStreamer
 * Copyright (C) 1999,2000 Erik Walthinsen <omega@cse.ogi.edu>
 *                    2000 Wim Taymans <wtay@chello.be>
 *
 * gstbuffer.c: Buffer operations
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
 * SECTION:gstbuffer
 * @short_description: Data-passing buffer type, supporting sub-buffers.
 * @see_also: #GstPad, #GstMiniObject, #GstBufferPool
 *
 * Buffers are the basic unit of data transfer in GStreamer.  The #GstBuffer
 * type provides all the state necessary to define the regions of memory as
 * part of a stream. Region copies are also supported, allowing a smaller
 * region of a buffer to become its own buffer, with mechanisms in place to
 * ensure that neither memory space goes away prematurely.
 *
 * Buffers are usually created with gst_buffer_new(). After a buffer has been
 * created one will typically allocate memory for it and add it to the buffer.
 * The following example creates a buffer that can hold a given video frame
 * with a given width, height and bits per plane.
 * <example>
 * <title>Creating a buffer for a video frame</title>
 *   <programlisting>
 *   GstBuffer *buffer;
 *   GstMemory *memory;
 *   gint size, width, height, bpp;
 *   ...
 *   size = width * height * bpp;
 *   buffer = gst_buffer_new ();
 *   memory = gst_allocator_alloc (NULL, size, NULL);
 *   gst_buffer_take_memory (buffer, -1, memory);
 *   ...
 *   </programlisting>
 * </example>
 *
 * Alternatively, use gst_buffer_new_allocate()
 * to create a buffer with preallocated data of a given size.
 *
 * Buffers can contain a list of #GstMemory objects. You can retrieve how many
 * memory objects with gst_buffer_n_memory() and you can get a pointer
 * to memory with gst_buffer_peek_memory()
 *
 * A buffer will usually have timestamps, and a duration, but neither of these
 * are guaranteed (they may be set to #GST_CLOCK_TIME_NONE). Whenever a
 * meaningful value can be given for these, they should be set. The timestamps
 * and duration are measured in nanoseconds (they are #GstClockTime values).
 *
 * A buffer can also have one or both of a start and an end offset. These are
 * media-type specific. For video buffers, the start offset will generally be
 * the frame number. For audio buffers, it will be the number of samples
 * produced so far. For compressed data, it could be the byte offset in a
 * source or destination file. Likewise, the end offset will be the offset of
 * the end of the buffer. These can only be meaningfully interpreted if you
 * know the media type of the buffer (the #GstCaps set on it). Either or both
 * can be set to #GST_BUFFER_OFFSET_NONE.
 *
 * gst_buffer_ref() is used to increase the refcount of a buffer. This must be
 * done when you want to keep a handle to the buffer after pushing it to the
 * next element.
 *
 * To efficiently create a smaller buffer out of an existing one, you can
 * use gst_buffer_copy_region().
 *
 * If a plug-in wants to modify the buffer data or metadata in-place, it should
 * first obtain a buffer that is safe to modify by using
 * gst_buffer_make_writable().  This function is optimized so that a copy will
 * only be made when it is necessary.
 *
 * Several flags of the buffer can be set and unset with the
 * GST_BUFFER_FLAG_SET() and GST_BUFFER_FLAG_UNSET() macros. Use
 * GST_BUFFER_FLAG_IS_SET() to test if a certain #GstBufferFlag is set.
 *
 * Buffers can be efficiently merged into a larger buffer with
 * gst_buffer_span(), which avoids memory copies when the gst_buffer_is_span_fast()
 * function returns TRUE.
 *
 * An element should either unref the buffer or push it out on a src pad
 * using gst_pad_push() (see #GstPad).
 *
 * Buffers are usually freed by unreffing them with gst_buffer_unref(). When
 * the refcount drops to 0, any data pointed to by the buffer is unreffed as
 * well.
 *
 * Last reviewed on November 8, 2011 (0.11.2)
 */
#include "gst_private.h"

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif

#include "gstbuffer.h"
#include "gstbufferpool.h"
#include "gstinfo.h"
#include "gstutils.h"
#include "gstminiobject.h"
#include "gstversion.h"

GType _gst_buffer_type = 0;

static GstMemory *_gst_buffer_arr_span (GstMemory ** mem[], gsize len[],
    guint n, gsize offset, gsize size, gboolean writable);

typedef struct _GstMetaItem GstMetaItem;

struct _GstMetaItem
{
  GstMetaItem *next;
  GstMeta meta;
};
#define ITEM_SIZE(info) ((info)->size + sizeof (GstMetaItem))

#define GST_BUFFER_MEM_MAX         16

#define GST_BUFFER_MEM_LEN(b)      (((GstBufferImpl *)(b))->len)
#define GST_BUFFER_MEM_ARRAY(b)    (((GstBufferImpl *)(b))->mem)
#define GST_BUFFER_MEM_PTR(b,i)    (((GstBufferImpl *)(b))->mem[i])
#define GST_BUFFER_BUFMEM(b)       (((GstBufferImpl *)(b))->bufmem)
#define GST_BUFFER_META(b)         (((GstBufferImpl *)(b))->item)

typedef struct
{
  GstBuffer buffer;

  /* the memory blocks */
  guint len;
  GstMemory *mem[GST_BUFFER_MEM_MAX];

  /* memory of the buffer when allocated from 1 chunk */
  GstMemory *bufmem;

  /* FIXME, make metadata allocation more efficient by using part of the
   * GstBufferImpl */
  GstMetaItem *item;
} GstBufferImpl;

static GstMemory *
_span_memory (GstBuffer * buffer, gsize offset, gsize size, gboolean writable)
{
  GstMemory *span, **mem[1];
  gsize len[1];

  /* not enough room, span buffers */
  mem[0] = GST_BUFFER_MEM_ARRAY (buffer);
  len[0] = GST_BUFFER_MEM_LEN (buffer);

  if (size == -1)
    size = gst_buffer_get_size (buffer);

  span = _gst_buffer_arr_span (mem, len, 1, offset, size, writable);

  return span;
}

static GstMemory *
_get_merged_memory (GstBuffer * buffer, gboolean * merged)
{
  guint len;
  GstMemory *mem;

  len = GST_BUFFER_MEM_LEN (buffer);

  if (G_UNLIKELY (len == 0)) {
    /* no memory */
    mem = NULL;
  } else if (G_LIKELY (len == 1)) {
    /* we can take the first one */
    mem = GST_BUFFER_MEM_PTR (buffer, 0);
    gst_memory_ref (mem);
    *merged = FALSE;
  } else {
    /* we need to span memory */
    mem = _span_memory (buffer, 0, -1, FALSE);
    *merged = TRUE;
  }
  return mem;
}

static void
_replace_all_memory (GstBuffer * buffer, GstMemory * mem)
{
  gsize len, i;

  len = GST_BUFFER_MEM_LEN (buffer);

  if (G_LIKELY (len == 1 && GST_BUFFER_MEM_PTR (buffer, 0) == mem)) {
    gst_memory_unref (mem);
    return;
  }

  GST_LOG ("buffer %p replace with memory %p", buffer, mem);

  /* unref old memory */
  for (i = 0; i < len; i++)
    gst_memory_unref (GST_BUFFER_MEM_PTR (buffer, i));
  /* replace with single memory */
  GST_BUFFER_MEM_PTR (buffer, 0) = mem;
  GST_BUFFER_MEM_LEN (buffer) = 1;
}

static inline void
_memory_add (GstBuffer * buffer, guint idx, GstMemory * mem)
{
  guint i, len = GST_BUFFER_MEM_LEN (buffer);

  if (G_UNLIKELY (len >= GST_BUFFER_MEM_MAX)) {
    /* too many buffer, span them. */
    /* FIXME, there is room for improvement here: We could only try to merge
     * 2 buffers to make some room. If we can't efficiently merge 2 buffers we
     * could try to only merge the two smallest buffers to avoid memcpy, etc. */
    GST_CAT_DEBUG (GST_CAT_PERFORMANCE, "memory array overflow in buffer %p",
        buffer);
    _replace_all_memory (buffer, _span_memory (buffer, 0, -1, FALSE));
    /* we now have 1 single spanned buffer */
    len = 1;
  }

  if (idx == -1)
    idx = len;

  for (i = len; i > idx; i--) {
    /* move buffers to insert, FIXME, we need to insert first and then merge */
    GST_BUFFER_MEM_PTR (buffer, i) = GST_BUFFER_MEM_PTR (buffer, i - 1);
  }
  /* and insert the new buffer */
  GST_BUFFER_MEM_PTR (buffer, idx) = mem;
  GST_BUFFER_MEM_LEN (buffer) = len + 1;
}

GST_DEFINE_MINI_OBJECT_TYPE (GstBuffer, gst_buffer);

void
_priv_gst_buffer_initialize (void)
{
  _gst_buffer_type = gst_buffer_get_type ();
}

/**
 * gst_buffer_copy_into:
 * @dest: a destination #GstBuffer
 * @src: a source #GstBuffer
 * @flags: flags indicating what metadata fields should be copied.
 * @offset: offset to copy from
 * @size: total size to copy. If -1, all data is copied.
 *
 * Copies the information from @src into @dest.
 *
 * @flags indicate which fields will be copied.
 */
void
gst_buffer_copy_into (GstBuffer * dest, GstBuffer * src,
    GstBufferCopyFlags flags, gsize offset, gsize size)
{
  GstMetaItem *walk;
  gsize bufsize;
  gboolean region = FALSE;

  g_return_if_fail (dest != NULL);
  g_return_if_fail (src != NULL);

  /* nothing to copy if the buffers are the same */
  if (G_UNLIKELY (dest == src))
    return;

  g_return_if_fail (gst_buffer_is_writable (dest));

  bufsize = gst_buffer_get_size (src);
  g_return_if_fail (bufsize >= offset);
  if (offset > 0)
    region = TRUE;
  if (size == -1)
    size = bufsize - offset;
  if (size < bufsize)
    region = TRUE;
  g_return_if_fail (bufsize >= offset + size);

  GST_CAT_LOG (GST_CAT_BUFFER, "copy %p to %p, offset %" G_GSIZE_FORMAT
      "-%" G_GSIZE_FORMAT "/%" G_GSIZE_FORMAT, src, dest, offset, size,
      bufsize);

  if (flags & GST_BUFFER_COPY_FLAGS) {
    /* copy flags */
    GST_MINI_OBJECT_FLAGS (dest) = GST_MINI_OBJECT_FLAGS (src);
  }

  if (flags & GST_BUFFER_COPY_TIMESTAMPS) {
    if (offset == 0) {
      GST_BUFFER_PTS (dest) = GST_BUFFER_PTS (src);
      GST_BUFFER_DTS (dest) = GST_BUFFER_DTS (src);
      GST_BUFFER_OFFSET (dest) = GST_BUFFER_OFFSET (src);
      if (size == bufsize) {
        GST_BUFFER_DURATION (dest) = GST_BUFFER_DURATION (src);
        GST_BUFFER_OFFSET_END (dest) = GST_BUFFER_OFFSET_END (src);
      }
    } else {
      GST_BUFFER_PTS (dest) = GST_CLOCK_TIME_NONE;
      GST_BUFFER_DTS (dest) = GST_CLOCK_TIME_NONE;
      GST_BUFFER_DURATION (dest) = GST_CLOCK_TIME_NONE;
      GST_BUFFER_OFFSET (dest) = GST_BUFFER_OFFSET_NONE;
      GST_BUFFER_OFFSET_END (dest) = GST_BUFFER_OFFSET_NONE;
    }
  }

  if (flags & GST_BUFFER_COPY_MEMORY) {
    GstMemory *mem;
    gsize skip, left, len, i, bsize;

    len = GST_BUFFER_MEM_LEN (src);
    left = size;
    skip = offset;

    /* copy and make regions of the memory */
    for (i = 0; i < len && left > 0; i++) {
      mem = GST_BUFFER_MEM_PTR (src, i);
      bsize = gst_memory_get_sizes (mem, NULL, NULL);

      if (bsize <= skip) {
        /* don't copy buffer */
        skip -= bsize;
      } else {
        gsize tocopy;

        tocopy = MIN (bsize - skip, left);
        if (mem->flags & GST_MEMORY_FLAG_NO_SHARE) {
          /* no share, always copy then */
          mem = gst_memory_copy (mem, skip, tocopy);
          skip = 0;
        } else if (tocopy < bsize) {
          /* we need to clip something */
          mem = gst_memory_share (mem, skip, tocopy);
          skip = 0;
        } else {
          mem = gst_memory_ref (mem);
        }
        _memory_add (dest, -1, mem);
        left -= tocopy;
      }
    }
    if (flags & GST_BUFFER_COPY_MERGE) {
      _replace_all_memory (dest, _span_memory (dest, 0, size, FALSE));
    }
  }

  if (flags & GST_BUFFER_COPY_META) {
    for (walk = GST_BUFFER_META (src); walk; walk = walk->next) {
      GstMeta *meta = &walk->meta;
      const GstMetaInfo *info = meta->info;

      if (info->transform_func) {
        GstMetaTransformCopy copy_data;

        copy_data.region = region;
        copy_data.offset = offset;
        copy_data.size = size;

        info->transform_func (dest, meta, src,
            _gst_meta_transform_copy, &copy_data);
      }
    }
  }
}

static GstBuffer *
_gst_buffer_copy (GstBuffer * buffer)
{
  GstBuffer *copy;

  g_return_val_if_fail (buffer != NULL, NULL);

  /* create a fresh new buffer */
  copy = gst_buffer_new ();

  /* we simply copy everything from our parent */
  gst_buffer_copy_into (copy, buffer, GST_BUFFER_COPY_ALL, 0, -1);

  return copy;
}

/* the default dispose function revives the buffer and returns it to the
 * pool when there is a pool */
static gboolean
_gst_buffer_dispose (GstBuffer * buffer)
{
  GstBufferPool *pool;

  /* no pool, do free */
  if ((pool = buffer->pool) == NULL)
    return TRUE;

  /* keep the buffer alive */
  gst_buffer_ref (buffer);
  /* return the buffer to the pool */
  GST_CAT_LOG (GST_CAT_BUFFER, "release %p to pool %p", buffer, pool);
  gst_buffer_pool_release_buffer (pool, buffer);

  return FALSE;
}

static void
_gst_buffer_free (GstBuffer * buffer)
{
  GstMetaItem *walk, *next;
  guint i, len;
  gsize msize;

  g_return_if_fail (buffer != NULL);

  GST_CAT_LOG (GST_CAT_BUFFER, "finalize %p", buffer);

  /* free metadata */
  for (walk = GST_BUFFER_META (buffer); walk; walk = next) {
    GstMeta *meta = &walk->meta;
    const GstMetaInfo *info = meta->info;

    /* call free_func if any */
    if (info->free_func)
      info->free_func (meta, buffer);

    next = walk->next;
    /* and free the slice */
    g_slice_free1 (ITEM_SIZE (info), walk);
  }

  /* get the size, when unreffing the memory, we could also unref the buffer
   * itself */
  msize = GST_MINI_OBJECT_SIZE (buffer);

  /* free our memory */
  len = GST_BUFFER_MEM_LEN (buffer);
  for (i = 0; i < len; i++)
    gst_memory_unref (GST_BUFFER_MEM_PTR (buffer, i));

  /* we set msize to 0 when the buffer is part of the memory block */
  if (msize)
    g_slice_free1 (msize, buffer);
  else
    gst_memory_unref (GST_BUFFER_BUFMEM (buffer));
}

static void
gst_buffer_init (GstBufferImpl * buffer, gsize size)
{
  gst_mini_object_init (GST_MINI_OBJECT_CAST (buffer), _gst_buffer_type, size);

  buffer->buffer.mini_object.copy =
      (GstMiniObjectCopyFunction) _gst_buffer_copy;
  buffer->buffer.mini_object.dispose =
      (GstMiniObjectDisposeFunction) _gst_buffer_dispose;
  buffer->buffer.mini_object.free =
      (GstMiniObjectFreeFunction) _gst_buffer_free;

  GST_BUFFER (buffer)->pool = NULL;
  GST_BUFFER_PTS (buffer) = GST_CLOCK_TIME_NONE;
  GST_BUFFER_DTS (buffer) = GST_CLOCK_TIME_NONE;
  GST_BUFFER_DURATION (buffer) = GST_CLOCK_TIME_NONE;
  GST_BUFFER_OFFSET (buffer) = GST_BUFFER_OFFSET_NONE;
  GST_BUFFER_OFFSET_END (buffer) = GST_BUFFER_OFFSET_NONE;

  GST_BUFFER_MEM_LEN (buffer) = 0;
  GST_BUFFER_META (buffer) = NULL;
}

/**
 * gst_buffer_new:
 *
 * Creates a newly allocated buffer without any data.
 *
 * MT safe.
 *
 * Returns: (transfer full): the new #GstBuffer.
 */
GstBuffer *
gst_buffer_new (void)
{
  GstBufferImpl *newbuf;

  newbuf = g_slice_new (GstBufferImpl);
  GST_CAT_LOG (GST_CAT_BUFFER, "new %p", newbuf);

  gst_buffer_init (newbuf, sizeof (GstBufferImpl));

  return GST_BUFFER_CAST (newbuf);
}

/**
 * gst_buffer_new_allocate:
 * @allocator: (transfer none) (allow-none): the #GstAllocator to use, or NULL to use the
 *     default allocator
 * @size: the size in bytes of the new buffer's data.
 * @params: (transfer none) (allow-none): optional parameters
 *
 * Tries to create a newly allocated buffer with data of the given size and
 * extra parameters from @allocator. If the requested amount of memory can't be
 * allocated, NULL will be returned. The allocated buffer memory is not cleared.
 *
 * When @allocator is NULL, the default memory allocator will be used.
 *
 * Note that when @size == 0, the buffer will not have memory associated with it.
 *
 * MT safe.
 *
 * Returns: (transfer full): a new #GstBuffer, or NULL if the memory couldn't
 *     be allocated.
 */
GstBuffer *
gst_buffer_new_allocate (GstAllocator * allocator, gsize size,
    GstAllocationParams * params)
{
  GstBuffer *newbuf;
  GstMemory *mem;
#if 0
  guint8 *data;
  gsize asize;
#endif

#if 1
  if (size > 0) {
    mem = gst_allocator_alloc (allocator, size, params);
    if (G_UNLIKELY (mem == NULL))
      goto no_memory;
  } else {
    mem = NULL;
  }

  newbuf = gst_buffer_new ();

  if (mem != NULL)
    _memory_add (newbuf, -1, mem);

  GST_CAT_LOG (GST_CAT_BUFFER,
      "new buffer %p of size %" G_GSIZE_FORMAT " from allocator %p", newbuf,
      size, allocator);
#endif

#if 0
  asize = sizeof (GstBufferImpl) + size;
  data = g_slice_alloc (asize);
  if (G_UNLIKELY (data == NULL))
    goto no_memory;

  newbuf = GST_BUFFER_CAST (data);

  gst_buffer_init ((GstBufferImpl *) data, asize);
  if (size > 0) {
    mem = gst_memory_new_wrapped (0, data + sizeof (GstBufferImpl), NULL,
        size, 0, size);
    _memory_add (newbuf, -1, mem);
  }
#endif

#if 0
  /* allocate memory and buffer, it might be interesting to do this but there
   * are many complications. We need to keep the memory mapped to access the
   * buffer fields and the memory for the buffer might be just very slow. We
   * also need to do some more magic to get the alignment right. */
  asize = sizeof (GstBufferImpl) + size;
  mem = gst_allocator_alloc (allocator, asize, align);
  if (G_UNLIKELY (mem == NULL))
    goto no_memory;

  /* map the data part and init the buffer in it, set the buffer size to 0 so
   * that a finalize won't free the buffer */
  data = gst_memory_map (mem, &asize, NULL, GST_MAP_WRITE);
  gst_buffer_init ((GstBufferImpl *) data, 0);
  gst_memory_unmap (mem);

  /* strip off the buffer */
  gst_memory_resize (mem, sizeof (GstBufferImpl), size);

  newbuf = GST_BUFFER_CAST (data);
  GST_BUFFER_BUFMEM (newbuf) = mem;

  if (size > 0)
    _memory_add (newbuf, -1, gst_memory_ref (mem));
#endif

  return newbuf;

  /* ERRORS */
no_memory:
  {
    GST_CAT_WARNING (GST_CAT_BUFFER,
        "failed to allocate %" G_GSIZE_FORMAT " bytes", size);
    return NULL;
  }
}

/**
 * gst_buffer_new_wrapped_full:
 * @flags: #GstMemoryFlags
 * @data: data to wrap
 * @maxsize: allocated size of @data
 * @offset: offset in @data
 * @size: size of valid data
 * @user_data: user_data
 * @notify: called with @user_data when the memory is freed
 *
 * Allocate a new buffer that wraps the given memory. @data must point to
 * @maxsize of memory, the wrapped buffer will have the region from @offset and
 * @size visible.
 *
 * When the buffer is destroyed, @notify will be called with @user_data.
 *
 * The prefix/padding must be filled with 0 if @flags contains
 * #GST_MEMORY_FLAG_ZERO_PREFIXED and #GST_MEMORY_FLAG_ZERO_PADDED respectively.
 *
 * Returns: (transfer full): a new #GstBuffer
 */
GstBuffer *
gst_buffer_new_wrapped_full (GstMemoryFlags flags, gpointer data,
    gsize maxsize, gsize offset, gsize size, gpointer user_data,
    GDestroyNotify notify)
{
  GstBuffer *newbuf;

  newbuf = gst_buffer_new ();
  gst_buffer_append_memory (newbuf,
      gst_memory_new_wrapped (flags, data, maxsize, offset, size,
          user_data, notify));

  return newbuf;
}

/**
 * gst_buffer_new_wrapped:
 * @data: data to wrap
 * @size: allocated size of @data
 *
 * Creates a new buffer that wraps the given @data. The memory will be freed
 * with g_free and will be marked writable.
 *
 * MT safe.
 *
 * Returns: (transfer full): a new #GstBuffer
 */
GstBuffer *
gst_buffer_new_wrapped (gpointer data, gsize size)
{
  return gst_buffer_new_wrapped_full (0, data, size, 0, size, data, g_free);
}

/**
 * gst_buffer_n_memory:
 * @buffer: a #GstBuffer.
 *
 * Get the amount of memory blocks that this buffer has.
 *
 * Returns: (transfer full): the amount of memory block in this buffer.
 */
guint
gst_buffer_n_memory (GstBuffer * buffer)
{
  g_return_val_if_fail (GST_IS_BUFFER (buffer), 0);

  return GST_BUFFER_MEM_LEN (buffer);
}

/**
 * gst_buffer_take_memory:
 * @buffer: a #GstBuffer.
 * @idx: the index to add the memory at, or -1 to append it to the end
 * @mem: (transfer full): a #GstMemory.
 *
 * Add the memory block @mem to @buffer at @idx. This function takes ownership
 * of @mem and thus doesn't increase its refcount.
 */
void
gst_buffer_take_memory (GstBuffer * buffer, gint idx, GstMemory * mem)
{
  g_return_if_fail (GST_IS_BUFFER (buffer));
  g_return_if_fail (gst_buffer_is_writable (buffer));
  g_return_if_fail (mem != NULL);
  g_return_if_fail (idx == -1 ||
      (idx >= 0 && idx <= GST_BUFFER_MEM_LEN (buffer)));

  _memory_add (buffer, idx, mem);
}

static GstMemory *
_get_mapped (GstBuffer * buffer, guint idx, GstMapInfo * info,
    GstMapFlags flags)
{
  GstMemory *mem, *mapped;

  mem = GST_BUFFER_MEM_PTR (buffer, idx);

  mapped = gst_memory_make_mapped (mem, info, flags);
  if (!mapped)
    return NULL;

  if (mapped != mem) {
    GST_BUFFER_MEM_PTR (buffer, idx) = mapped;
    gst_memory_unref (mem);
    mem = mapped;
  }
  return mem;
}

/**
 * gst_buffer_get_memory:
 * @buffer: a #GstBuffer.
 * @idx: an index
 *
 * Get the memory block in @buffer at @idx. If @idx is -1, all memory is merged
 * into one large #GstMemory object that is then returned.
 *
 * Returns: (transfer full): a #GstMemory at @idx. Use gst_memory_unref () after usage.
 */
GstMemory *
gst_buffer_get_memory (GstBuffer * buffer, gint idx)
{
  GstMemory *mem;
  gboolean merged;

  g_return_val_if_fail (GST_IS_BUFFER (buffer), NULL);
  g_return_val_if_fail (idx == -1 ||
      (idx >= 0 && idx <= GST_BUFFER_MEM_LEN (buffer)), NULL);

  if (idx == -1) {
    mem = _get_merged_memory (buffer, &merged);
  } else if ((mem = GST_BUFFER_MEM_PTR (buffer, idx))) {
    gst_memory_ref (mem);
  }
  return mem;
}

/**
 * gst_buffer_replace_memory:
 * @buffer: a #GstBuffer.
 * @idx: an index
 * @mem: (transfer full): a #GstMemory
 *
 * Replaces the memory block in @buffer at @idx with @mem. If @idx is -1, all
 * memory will be removed and replaced with @mem.
 *
 * @buffer should be writable.
 */
void
gst_buffer_replace_memory (GstBuffer * buffer, gint idx, GstMemory * mem)
{
  g_return_if_fail (GST_IS_BUFFER (buffer));
  g_return_if_fail (gst_buffer_is_writable (buffer));
  g_return_if_fail (idx == -1 ||
      (idx >= 0 && idx < GST_BUFFER_MEM_LEN (buffer)));

  if (idx == -1) {
    _replace_all_memory (buffer, mem);
  } else {
    GstMemory *old;

    if ((old = GST_BUFFER_MEM_PTR (buffer, idx)))
      gst_memory_unref (old);
    GST_BUFFER_MEM_PTR (buffer, idx) = mem;
  }
}

/**
 * gst_buffer_remove_memory_range:
 * @buffer: a #GstBuffer.
 * @idx: an index
 * @length: a length
 *
 * Remove @len memory blocks in @buffer starting from @idx.
 *
 * @length can be -1, in which case all memory starting from @idx is removed.
 */
void
gst_buffer_remove_memory_range (GstBuffer * buffer, guint idx, gint length)
{
  guint len, i, end;

  g_return_if_fail (GST_IS_BUFFER (buffer));
  g_return_if_fail (gst_buffer_is_writable (buffer));

  len = GST_BUFFER_MEM_LEN (buffer);
  g_return_if_fail ((length == -1 && idx < len) || length + idx < len);

  if (length == -1)
    length = len - idx;

  end = idx + length;
  for (i = idx; i < end; i++)
    gst_memory_unref (GST_BUFFER_MEM_PTR (buffer, i));

  if (end != len) {
    g_memmove (&GST_BUFFER_MEM_PTR (buffer, idx),
        &GST_BUFFER_MEM_PTR (buffer, end), (len - end) * sizeof (gpointer));
  }
  GST_BUFFER_MEM_LEN (buffer) = len - length;
}

/**
 * gst_buffer_get_sizes:
 * @buffer: a #GstBuffer.
 * @offset: a pointer to the offset
 * @maxsize: a pointer to the maxsize
 *
 * Get the total size of all memory blocks in @buffer.
 *
 * When not %NULL, @offset will contain the offset of the data in the first
 * memory block in @buffer and @maxsize will contain the sum of the size
 * and @offset and the amount of extra padding on the last memory block.
 * @offset and @maxsize can be used to resize the buffer with
 * gst_buffer_resize().
 *
 * Returns: the total size of the memory in @buffer.
 */
gsize
gst_buffer_get_sizes (GstBuffer * buffer, gsize * offset, gsize * maxsize)
{
  guint len;
  gsize size;
  GstMemory *mem;

  g_return_val_if_fail (GST_IS_BUFFER (buffer), 0);

  len = GST_BUFFER_MEM_LEN (buffer);

  if (G_LIKELY (len == 1)) {
    /* common case */
    mem = GST_BUFFER_MEM_PTR (buffer, 0);
    size = gst_memory_get_sizes (mem, offset, maxsize);
  } else {
    guint i;
    gsize extra, offs;

    size = offs = extra = 0;
    for (i = 0; i < len; i++) {
      gsize s, o, ms;

      mem = GST_BUFFER_MEM_PTR (buffer, i);
      s = gst_memory_get_sizes (mem, &o, &ms);

      if (s) {
        if (size == 0)
          /* first size, take accumulated data before as the offset */
          offs = extra + o;
        /* add sizes */
        size += s;
        /* save the amount of data after this block */
        extra = ms - (o + s);
      } else {
        /* empty block, add as extra */
        extra += ms;
      }
    }
    if (offset)
      *offset = offs;
    if (maxsize)
      *maxsize = offs + size + extra;
  }
  return size;
}

/**
 * gst_buffer_resize:
 * @buffer: a #GstBuffer.
 * @offset: the offset adjustement
 * @size: the new size or -1 to just adjust the offset
 *
 * Set the total size of the buffer
 */
void
gst_buffer_resize (GstBuffer * buffer, gssize offset, gssize size)
{
  guint len;
  guint i;
  gsize bsize, bufsize, bufoffs, bufmax;
  GstMemory *mem;

  g_return_if_fail (gst_buffer_is_writable (buffer));
  g_return_if_fail (size >= -1);

  bufsize = gst_buffer_get_sizes (buffer, &bufoffs, &bufmax);

  GST_CAT_LOG (GST_CAT_BUFFER, "trim %p %" G_GSSIZE_FORMAT "-%" G_GSSIZE_FORMAT
      " size:%" G_GSIZE_FORMAT " offs:%" G_GSIZE_FORMAT " max:%"
      G_GSIZE_FORMAT, buffer, offset, size, bufsize, bufoffs, bufmax);

  /* we can't go back further than the current offset or past the end of the
   * buffer */
  g_return_if_fail ((offset < 0 && bufoffs >= -offset) || (offset >= 0
          && bufoffs + offset <= bufmax));
  if (size == -1) {
    g_return_if_fail (bufsize >= offset);
    size = bufsize - offset;
  }
  g_return_if_fail (bufmax >= bufoffs + offset + size);

  /* no change */
  if (offset == 0 && size == bufsize)
    return;

  len = GST_BUFFER_MEM_LEN (buffer);

  /* copy and trim */
  for (i = 0; i < len; i++) {
    gsize left, noffs;

    mem = GST_BUFFER_MEM_PTR (buffer, i);
    bsize = gst_memory_get_sizes (mem, NULL, NULL);

    noffs = 0;
    /* last buffer always gets resized to the remaining size */
    if (i + 1 == len)
      left = size;
    /* shrink buffers before the offset */
    else if ((gssize) bsize <= offset) {
      left = 0;
      noffs = offset - bsize;
      offset = 0;
    }
    /* clip other buffers */
    else
      left = MIN (bsize - offset, size);

    if (offset != 0 || left != bsize) {
      if (gst_memory_is_exclusive (mem)) {
        gst_memory_resize (mem, offset, left);
      } else {
        GstMemory *tmp;

        if (mem->flags & GST_MEMORY_FLAG_NO_SHARE)
          tmp = gst_memory_copy (mem, offset, left);
        else
          tmp = gst_memory_share (mem, offset, left);

        gst_memory_unref (mem);
        mem = tmp;
      }
    }
    offset = noffs;
    size -= left;

    GST_BUFFER_MEM_PTR (buffer, i) = mem;
  }
}

/**
 * gst_buffer_map:
 * @buffer: a #GstBuffer.
 * @info: (out): info about the mapping
 * @flags: flags for the mapping
 *
 * This function fills @info with a pointer to the merged memory in @buffer.
 * @flags describe the desired access of the memory. When @flags is
 * #GST_MAP_WRITE, @buffer should be writable (as returned from
 * gst_buffer_is_writable()).
 *
 * When @buffer is writable but the memory isn't, a writable copy will
 * automatically be created and returned. The readonly copy of the buffer memory
 * will then also be replaced with this writable copy.
 *
 * When the buffer contains multiple memory blocks, the returned pointer will be
 * a concatenation of the memory blocks.
 *
 * The memory in @info should be unmapped with gst_buffer_unmap() after usage.
 *
 * Returns: (transfer full): %TRUE if the map succeeded and @info contains valid
 * data.
 */
gboolean
gst_buffer_map (GstBuffer * buffer, GstMapInfo * info, GstMapFlags flags)
{
  GstMemory *mem, *nmem;
  gboolean write, writable, merged;

  g_return_val_if_fail (GST_IS_BUFFER (buffer), FALSE);
  g_return_val_if_fail (info != NULL, FALSE);

  write = (flags & GST_MAP_WRITE) != 0;
  writable = gst_buffer_is_writable (buffer);

  /* check if we can write when asked for write access */
  if (G_UNLIKELY (write && !writable))
    goto not_writable;

  mem = _get_merged_memory (buffer, &merged);
  if (G_UNLIKELY (mem == NULL))
    goto no_memory;

  /* now try to map */
  nmem = gst_memory_make_mapped (mem, info, flags);
  if (G_UNLIKELY (nmem == NULL))
    goto cannot_map;

  /* if we merged or when the map returned a different memory, we try to replace
   * the memory in the buffer */
  if (G_UNLIKELY (merged || nmem != mem)) {
    /* if the buffer is writable, replace the memory */
    if (writable) {
      _replace_all_memory (buffer, gst_memory_ref (nmem));
    } else {
      if (GST_BUFFER_MEM_LEN (buffer) > 1) {
        GST_CAT_DEBUG (GST_CAT_PERFORMANCE,
            "temporary mapping for memory %p in buffer %p", nmem, buffer);
      }
    }
  }
  return TRUE;

  /* ERROR */
not_writable:
  {
    GST_WARNING_OBJECT (buffer, "write map requested on non-writable buffer");
    g_critical ("write map requested on non-writable buffer");
    return FALSE;
  }
no_memory:
  {
    /* empty buffer, we need to return NULL */
    GST_DEBUG_OBJECT (buffer, "can't get buffer memory");
    info->memory = NULL;
    info->data = NULL;
    info->size = 0;
    info->maxsize = 0;
    return TRUE;
  }
cannot_map:
  {
    GST_DEBUG_OBJECT (buffer, "cannot map memory");
    return FALSE;
  }
}

/**
 * gst_buffer_unmap:
 * @buffer: a #GstBuffer.
 * @info: a #GstMapInfo
 *
 * Release the memory previously mapped with gst_buffer_map().
 */
void
gst_buffer_unmap (GstBuffer * buffer, GstMapInfo * info)
{
  g_return_if_fail (GST_IS_BUFFER (buffer));
  g_return_if_fail (info != NULL);

  /* we need to check for NULL, it is possible that we tried to map a buffer
   * without memory and we should be able to unmap that fine */
  if (G_LIKELY (info->memory)) {
    gst_memory_unmap (info->memory, info);
    gst_memory_unref (info->memory);
  }
}

/**
 * gst_buffer_fill:
 * @buffer: a #GstBuffer.
 * @offset: the offset to fill
 * @src: the source address
 * @size: the size to fill
 *
 * Copy @size bytes from @src to @buffer at @offset.
 *
 * Returns: The amount of bytes copied. This value can be lower than @size
 *    when @buffer did not contain enough data.
 */
gsize
gst_buffer_fill (GstBuffer * buffer, gsize offset, gconstpointer src,
    gsize size)
{
  gsize i, len, left;
  const guint8 *ptr = src;

  g_return_val_if_fail (GST_IS_BUFFER (buffer), 0);
  g_return_val_if_fail (gst_buffer_is_writable (buffer), 0);
  g_return_val_if_fail (src != NULL, 0);

  len = GST_BUFFER_MEM_LEN (buffer);
  left = size;

  for (i = 0; i < len && left > 0; i++) {
    GstMapInfo info;
    gsize tocopy;
    GstMemory *mem;

    mem = _get_mapped (buffer, i, &info, GST_MAP_WRITE);
    if (info.size > offset) {
      /* we have enough */
      tocopy = MIN (info.size - offset, left);
      memcpy ((guint8 *) info.data + offset, ptr, tocopy);
      left -= tocopy;
      ptr += tocopy;
      offset = 0;
    } else {
      /* offset past buffer, skip */
      offset -= info.size;
    }
    gst_memory_unmap (mem, &info);
  }
  return size - left;
}

/**
 * gst_buffer_extract:
 * @buffer: a #GstBuffer.
 * @offset: the offset to extract
 * @dest: the destination address
 * @size: the size to extract
 *
 * Copy @size bytes starting from @offset in @buffer to @dest.
 *
 * Returns: The amount of bytes extracted. This value can be lower than @size
 *    when @buffer did not contain enough data.
 */
gsize
gst_buffer_extract (GstBuffer * buffer, gsize offset, gpointer dest, gsize size)
{
  gsize i, len, left;
  guint8 *ptr = dest;

  g_return_val_if_fail (GST_IS_BUFFER (buffer), 0);
  g_return_val_if_fail (dest != NULL, 0);

  len = GST_BUFFER_MEM_LEN (buffer);
  left = size;

  for (i = 0; i < len && left > 0; i++) {
    GstMapInfo info;
    gsize tocopy;
    GstMemory *mem;

    mem = _get_mapped (buffer, i, &info, GST_MAP_READ);
    if (info.size > offset) {
      /* we have enough */
      tocopy = MIN (info.size - offset, left);
      memcpy (ptr, (guint8 *) info.data + offset, tocopy);
      left -= tocopy;
      ptr += tocopy;
      offset = 0;
    } else {
      /* offset past buffer, skip */
      offset -= info.size;
    }
    gst_memory_unmap (mem, &info);
  }
  return size - left;
}

/**
 * gst_buffer_memcmp:
 * @buffer: a #GstBuffer.
 * @offset: the offset in @buffer
 * @mem: the memory to compare
 * @size: the size to compare
 *
 * Compare @size bytes starting from @offset in @buffer with the memory in @mem.
 *
 * Returns: 0 if the memory is equal.
 */
gint
gst_buffer_memcmp (GstBuffer * buffer, gsize offset, gconstpointer mem,
    gsize size)
{
  gsize i, len;
  const guint8 *ptr = mem;
  gint res = 0;

  g_return_val_if_fail (GST_IS_BUFFER (buffer), 0);
  g_return_val_if_fail (mem != NULL, 0);

  len = GST_BUFFER_MEM_LEN (buffer);

  for (i = 0; i < len && size > 0 && res == 0; i++) {
    GstMapInfo info;
    gsize tocmp;
    GstMemory *mem;

    mem = _get_mapped (buffer, i, &info, GST_MAP_READ);
    if (info.size > offset) {
      /* we have enough */
      tocmp = MIN (info.size - offset, size);
      res = memcmp (ptr, (guint8 *) info.data + offset, tocmp);
      size -= tocmp;
      ptr += tocmp;
      offset = 0;
    } else {
      /* offset past buffer, skip */
      offset -= info.size;
    }
    gst_memory_unmap (mem, &info);
  }
  return res;
}

/**
 * gst_buffer_memset:
 * @buffer: a #GstBuffer.
 * @offset: the offset in @buffer
 * @val: the value to set
 * @size: the size to set
 *
 * Fill @buf with @size bytes with @val starting from @offset.
 *
 * Returns: The amount of bytes filled. This value can be lower than @size
 *    when @buffer did not contain enough data.
 */
gsize
gst_buffer_memset (GstBuffer * buffer, gsize offset, guint8 val, gsize size)
{
  gsize i, len, left;

  g_return_val_if_fail (GST_IS_BUFFER (buffer), 0);
  g_return_val_if_fail (gst_buffer_is_writable (buffer), 0);

  len = GST_BUFFER_MEM_LEN (buffer);
  left = size;

  for (i = 0; i < len && left > 0; i++) {
    GstMapInfo info;
    gsize toset;
    GstMemory *mem;

    mem = _get_mapped (buffer, i, &info, GST_MAP_WRITE);
    if (info.size > offset) {
      /* we have enough */
      toset = MIN (info.size - offset, left);
      memset ((guint8 *) info.data + offset, val, toset);
      left -= toset;
      offset = 0;
    } else {
      /* offset past buffer, skip */
      offset -= info.size;
    }
    gst_memory_unmap (mem, &info);
  }
  return size - left;
}

/**
 * gst_buffer_copy_region:
 * @parent: a #GstBuffer.
 * @flags: the #GstBufferCopyFlags
 * @offset: the offset into parent #GstBuffer at which the new sub-buffer 
 *          begins.
 * @size: the size of the new #GstBuffer sub-buffer, in bytes.
 *
 * Creates a sub-buffer from @parent at @offset and @size.
 * This sub-buffer uses the actual memory space of the parent buffer.
 * This function will copy the offset and timestamp fields when the
 * offset is 0. If not, they will be set to #GST_CLOCK_TIME_NONE and 
 * #GST_BUFFER_OFFSET_NONE.
 * If @offset equals 0 and @size equals the total size of @buffer, the
 * duration and offset end fields are also copied. If not they will be set
 * to #GST_CLOCK_TIME_NONE and #GST_BUFFER_OFFSET_NONE.
 *
 * MT safe.
 *
 * Returns: (transfer full): the new #GstBuffer or NULL if the arguments were
 *     invalid.
 */
GstBuffer *
gst_buffer_copy_region (GstBuffer * buffer, GstBufferCopyFlags flags,
    gsize offset, gsize size)
{
  GstBuffer *copy;

  g_return_val_if_fail (buffer != NULL, NULL);

  /* create the new buffer */
  copy = gst_buffer_new ();

  GST_CAT_LOG (GST_CAT_BUFFER, "new region copy %p of %p %" G_GSIZE_FORMAT
      "-%" G_GSIZE_FORMAT, copy, buffer, offset, size);

  gst_buffer_copy_into (copy, buffer, flags, offset, size);

  return copy;
}

static gboolean
_gst_buffer_arr_is_span_fast (GstMemory ** mem[], gsize len[], guint n,
    gsize * offset, GstMemory ** parent)
{
  GstMemory *mcur, *mprv;
  gboolean have_offset = FALSE;
  guint count, i;

  mcur = mprv = NULL;
  for (count = 0; count < n; count++) {
    gsize offs, clen;
    GstMemory **cmem;

    cmem = mem[count];
    clen = len[count];

    for (i = 0; i < clen; i++) {
      if (mcur)
        mprv = mcur;
      mcur = cmem[i];

      if (mprv && mcur) {
        /* check is memory is contiguous */
        if (!gst_memory_is_span (mprv, mcur, &offs))
          return FALSE;

        if (!have_offset) {
          if (offset)
            *offset = offs;
          if (parent)
            *parent = mprv->parent;

          have_offset = TRUE;
        }
      }
    }
  }
  return have_offset;
}

static GstMemory *
_gst_buffer_arr_span (GstMemory ** mem[], gsize len[], guint n, gsize offset,
    gsize size, gboolean writable)
{
  GstMemory *span, *parent = NULL;
  gsize poffset = 0;

  if (!writable
      && _gst_buffer_arr_is_span_fast (mem, len, n, &poffset, &parent)) {
    if (parent->flags & GST_MEMORY_FLAG_NO_SHARE) {
      GST_CAT_DEBUG (GST_CAT_PERFORMANCE, "copy for span %p", parent);
      span = gst_memory_copy (parent, offset + poffset, size);
    } else {
      span = gst_memory_share (parent, offset + poffset, size);
    }
  } else {
    gsize count, left;
    GstMapInfo dinfo;
    guint8 *ptr;

    span = gst_allocator_alloc (NULL, size, NULL);
    gst_memory_map (span, &dinfo, GST_MAP_WRITE);

    ptr = dinfo.data;
    left = size;

    for (count = 0; count < n; count++) {
      GstMapInfo sinfo;
      gsize i, tocopy, clen;
      GstMemory **cmem;

      cmem = mem[count];
      clen = len[count];

      for (i = 0; i < clen && left > 0; i++) {
        gst_memory_map (cmem[i], &sinfo, GST_MAP_READ);
        tocopy = MIN (sinfo.size, left);
        if (tocopy > offset) {
          GST_CAT_DEBUG (GST_CAT_PERFORMANCE,
              "memcpy for span %p from memory %p", span, cmem[i]);
          memcpy (ptr, (guint8 *) sinfo.data + offset, tocopy - offset);
          left -= tocopy;
          ptr += tocopy;
          offset = 0;
        } else {
          offset -= tocopy;
        }
        gst_memory_unmap (cmem[i], &sinfo);
      }
    }
    gst_memory_unmap (span, &dinfo);
  }
  return span;
}

/**
 * gst_buffer_is_span_fast:
 * @buf1: the first #GstBuffer.
 * @buf2: the second #GstBuffer.
 *
 * Determines whether a gst_buffer_span() can be done without copying
 * the contents, that is, whether the data areas are contiguous sub-buffers of
 * the same buffer.
 *
 * MT safe.
 * Returns: TRUE if the buffers are contiguous,
 * FALSE if a copy would be required.
 */
gboolean
gst_buffer_is_span_fast (GstBuffer * buf1, GstBuffer * buf2)
{
  GstMemory **mem[2];
  gsize len[2];

  g_return_val_if_fail (GST_IS_BUFFER (buf1), FALSE);
  g_return_val_if_fail (GST_IS_BUFFER (buf2), FALSE);
  g_return_val_if_fail (buf1->mini_object.refcount > 0, FALSE);
  g_return_val_if_fail (buf2->mini_object.refcount > 0, FALSE);

  mem[0] = GST_BUFFER_MEM_ARRAY (buf1);
  len[0] = GST_BUFFER_MEM_LEN (buf1);
  mem[1] = GST_BUFFER_MEM_ARRAY (buf2);
  len[1] = GST_BUFFER_MEM_LEN (buf2);

  return _gst_buffer_arr_is_span_fast (mem, len, 2, NULL, NULL);
}

/**
 * gst_buffer_span:
 * @buf1: the first source #GstBuffer to merge.
 * @offset: the offset in the first buffer from where the new
 * buffer should start.
 * @buf2: the second source #GstBuffer to merge.
 * @size: the total size of the new buffer.
 *
 * Creates a new buffer that consists of part of buf1 and buf2.
 * Logically, buf1 and buf2 are concatenated into a single larger
 * buffer, and a new buffer is created at the given offset inside
 * this space, with a given length.
 *
 * If the two source buffers are children of the same larger buffer,
 * and are contiguous, the new buffer will be a child of the shared
 * parent, and thus no copying is necessary. you can use
 * gst_buffer_is_span_fast() to determine if a memcpy will be needed.
 *
 * MT safe.
 *
 * Returns: (transfer full): the new #GstBuffer that spans the two source
 *     buffers, or NULL if the arguments are invalid.
 */
GstBuffer *
gst_buffer_span (GstBuffer * buf1, gsize offset, GstBuffer * buf2, gsize size)
{
  GstBuffer *newbuf;
  GstMemory *span;
  GstMemory **mem[2];
  gsize len[2], len1, len2;

  g_return_val_if_fail (GST_IS_BUFFER (buf1), NULL);
  g_return_val_if_fail (GST_IS_BUFFER (buf2), NULL);
  g_return_val_if_fail (buf1->mini_object.refcount > 0, NULL);
  g_return_val_if_fail (buf2->mini_object.refcount > 0, NULL);
  len1 = gst_buffer_get_size (buf1);
  len2 = gst_buffer_get_size (buf2);
  g_return_val_if_fail (len1 + len2 > offset, NULL);
  if (size == -1)
    size = len1 + len2 - offset;
  else
    g_return_val_if_fail (size <= len1 + len2 - offset, NULL);

  mem[0] = GST_BUFFER_MEM_ARRAY (buf1);
  len[0] = GST_BUFFER_MEM_LEN (buf1);
  mem[1] = GST_BUFFER_MEM_ARRAY (buf2);
  len[1] = GST_BUFFER_MEM_LEN (buf2);

  span = _gst_buffer_arr_span (mem, len, 2, offset, size, FALSE);

  newbuf = gst_buffer_new ();
  _memory_add (newbuf, -1, span);

#if 0
  /* if the offset is 0, the new buffer has the same timestamp as buf1 */
  if (offset == 0) {
    GST_BUFFER_OFFSET (newbuf) = GST_BUFFER_OFFSET (buf1);
    GST_BUFFER_PTS (newbuf) = GST_BUFFER_PTS (buf1);
    GST_BUFFER_DTS (newbuf) = GST_BUFFER_DTS (buf1);

    /* if we completely merged the two buffers (appended), we can
     * calculate the duration too. Also make sure we's not messing with
     * invalid DURATIONS */
    if (buf1->size + buf2->size == len) {
      if (GST_BUFFER_DURATION_IS_VALID (buf1) &&
          GST_BUFFER_DURATION_IS_VALID (buf2)) {
        /* add duration */
        GST_BUFFER_DURATION (newbuf) = GST_BUFFER_DURATION (buf1) +
            GST_BUFFER_DURATION (buf2);
      }
      if (GST_BUFFER_OFFSET_END_IS_VALID (buf2)) {
        /* add offset_end */
        GST_BUFFER_OFFSET_END (newbuf) = GST_BUFFER_OFFSET_END (buf2);
      }
    }
  }
#endif

  return newbuf;
}

/**
 * gst_buffer_get_meta:
 * @buffer: a #GstBuffer
 * @api: the #GType of an API
 *
 * Get the metadata for @api on buffer. When there is no such
 * metadata, NULL is returned.
 *
 * Returns: the metadata for @api on @buffer.
 */
GstMeta *
gst_buffer_get_meta (GstBuffer * buffer, GType api)
{
  GstMetaItem *item;
  GstMeta *result = NULL;

  g_return_val_if_fail (buffer != NULL, NULL);
  g_return_val_if_fail (api != 0, NULL);

  /* find GstMeta of the requested API */
  for (item = GST_BUFFER_META (buffer); item; item = item->next) {
    GstMeta *meta = &item->meta;
    if (meta->info->api == api) {
      result = meta;
      break;
    }
  }
  return result;
}

/**
 * gst_buffer_add_meta:
 * @buffer: a #GstBuffer
 * @info: a #GstMetaInfo
 * @params: params for @info
 *
 * Add metadata for @info to @buffer using the parameters in @params.
 *
 * Returns: (transfer none): the metadata for the api in @info on @buffer.
 */
GstMeta *
gst_buffer_add_meta (GstBuffer * buffer, const GstMetaInfo * info,
    gpointer params)
{
  GstMetaItem *item;
  GstMeta *result = NULL;
  gsize size;

  g_return_val_if_fail (buffer != NULL, NULL);
  g_return_val_if_fail (info != NULL, NULL);

  /* create a new slice */
  size = ITEM_SIZE (info);
  item = g_slice_alloc (size);
  result = &item->meta;
  result->info = info;
  result->flags = GST_META_FLAG_NONE;

  GST_CAT_DEBUG (GST_CAT_BUFFER,
      "alloc metadata %p (%s) of size %" G_GSIZE_FORMAT, result,
      g_type_name (info->type), info->size);

  /* call the init_func when needed */
  if (info->init_func)
    if (!info->init_func (result, params, buffer))
      goto init_failed;

  /* and add to the list of metadata */
  item->next = GST_BUFFER_META (buffer);
  GST_BUFFER_META (buffer) = item;

  return result;

init_failed:
  {
    g_slice_free1 (size, item);
    return NULL;
  }
}

/**
 * gst_buffer_remove_meta:
 * @buffer: a #GstBuffer
 * @meta: a #GstMeta
 *
 * Remove the metadata for @meta on @buffer.
 *
 * Returns: %TRUE if the metadata existed and was removed, %FALSE if no such
 * metadata was on @buffer.
 */
gboolean
gst_buffer_remove_meta (GstBuffer * buffer, GstMeta * meta)
{
  GstMetaItem *walk, *prev;

  g_return_val_if_fail (buffer != NULL, FALSE);
  g_return_val_if_fail (meta != NULL, FALSE);

  /* find the metadata and delete */
  prev = GST_BUFFER_META (buffer);
  for (walk = prev; walk; walk = walk->next) {
    GstMeta *m = &walk->meta;
    if (m == meta) {
      const GstMetaInfo *info = meta->info;

      /* remove from list */
      if (GST_BUFFER_META (buffer) == walk)
        GST_BUFFER_META (buffer) = walk->next;
      else
        prev->next = walk->next;
      /* call free_func if any */
      if (info->free_func)
        info->free_func (m, buffer);

      /* and free the slice */
      g_slice_free1 (ITEM_SIZE (info), walk);
      break;
    }
    prev = walk;
  }
  return walk != NULL;
}

/**
 * gst_buffer_iterate_meta:
 * @buffer: a #GstBuffer
 * @state: an opaque state pointer
 *
 * Retrieve the next #GstMeta after @current. If @state points
 * to %NULL, the first metadata is returned.
 *
 * @state will be updated with an opage state pointer 
 *
 * Returns: The next #GstMeta or %NULL when there are no more items.
 */
GstMeta *
gst_buffer_iterate_meta (GstBuffer * buffer, gpointer * state)
{
  GstMetaItem **meta;

  g_return_val_if_fail (buffer != NULL, NULL);
  g_return_val_if_fail (state != NULL, NULL);

  meta = (GstMetaItem **) state;
  if (*meta == NULL)
    /* state NULL, move to first item */
    *meta = GST_BUFFER_META (buffer);
  else
    /* state !NULL, move to next item in list */
    *meta = (*meta)->next;

  if (*meta)
    return &(*meta)->meta;
  else
    return NULL;
}

/**
 * gst_buffer_foreach_meta:
 * @buffer: a #GstBuffer
 * @func: (scope call): a #GstBufferForeachMetaFunc to call
 * @user_data: (closure): user data passed to @func
 *
 * Call @func with @user_data for each meta in @buffer.
 *
 * @func can modify the passed meta pointer or its contents. The return value
 * of @func define if this function returns or if the remaining metadata items
 * in the buffer should be skipped.
 */
void
gst_buffer_foreach_meta (GstBuffer * buffer, GstBufferForeachMetaFunc func,
    gpointer user_data)
{
  GstMetaItem *walk, *prev, *next;

  g_return_if_fail (buffer != NULL);
  g_return_if_fail (func != NULL);

  /* find the metadata and delete */
  prev = GST_BUFFER_META (buffer);
  for (walk = prev; walk; walk = next) {
    GstMeta *m, *new;
    gboolean res;

    m = new = &walk->meta;
    next = walk->next;

    res = func (buffer, &new, user_data);

    if (new == NULL) {
      const GstMetaInfo *info = m->info;

      GST_CAT_DEBUG (GST_CAT_BUFFER, "remove metadata %p (%s)", m,
          g_type_name (info->type));

      /* remove from list */
      if (GST_BUFFER_META (buffer) == walk)
        GST_BUFFER_META (buffer) = next;
      else
        prev->next = next;

      /* call free_func if any */
      if (info->free_func)
        info->free_func (m, buffer);

      /* and free the slice */
      g_slice_free1 (ITEM_SIZE (info), walk);
    }
    if (!res)
      break;
  }
}
