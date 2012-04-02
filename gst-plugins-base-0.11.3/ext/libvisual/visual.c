/* GStreamer
 * Copyright (C) 2004 Benjamin Otte <otte@gnome.org>
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
#include "config.h"
#endif

#include <gst/gst.h>
#include <gst/base/gstadapter.h>
#include <gst/video/video.h>
#include <gst/video/gstvideopool.h>
#include <gst/audio/audio.h>
#include <libvisual/libvisual.h>

#define GST_TYPE_VISUAL (gst_visual_get_type())
#define GST_IS_VISUAL(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj),GST_TYPE_VISUAL))
#define GST_VISUAL(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj),GST_TYPE_VISUAL,GstVisual))
#define GST_IS_VISUAL_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass),GST_TYPE_VISUAL))
#define GST_VISUAL_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass),GST_TYPE_VISUAL,GstVisualClass))
#define GST_VISUAL_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), GST_TYPE_VISUAL, GstVisualClass))

typedef struct _GstVisual GstVisual;
typedef struct _GstVisualClass GstVisualClass;

GST_DEBUG_CATEGORY_STATIC (libvisual_debug);
#define GST_CAT_DEFAULT (libvisual_debug)

/* amounf of samples before we can feed libvisual */
#define VISUAL_SAMPLES  512

#define DEFAULT_WIDTH   320
#define DEFAULT_HEIGHT  240
#define DEFAULT_FPS_N   25
#define DEFAULT_FPS_D   1

struct _GstVisual
{
  GstElement element;

  /* pads */
  GstPad *sinkpad;
  GstPad *srcpad;
  GstSegment segment;

  /* libvisual stuff */
  VisAudio *audio;
  VisVideo *video;
  VisActor *actor;

  /* audio/video state */
  GstAudioInfo info;

  /* framerate numerator & denominator */
  gint fps_n;
  gint fps_d;
  gint width;
  gint height;
  GstClockTime duration;
  guint outsize;
  GstBufferPool *pool;

  /* samples per frame based on caps */
  guint spf;

  /* state stuff */
  GstAdapter *adapter;
  guint count;

  /* QoS stuff *//* with LOCK */
  gdouble proportion;
  GstClockTime earliest_time;
};

struct _GstVisualClass
{
  GstElementClass parent_class;

  VisPluginRef *plugin;
};

GType gst_visual_get_type (void);


static GstStaticPadTemplate src_template = GST_STATIC_PAD_TEMPLATE ("src",
    GST_PAD_SRC,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS (GST_VIDEO_CAPS_MAKE (" { "
#if G_BYTE_ORDER == G_BIG_ENDIAN
            "\"xRGB\", " "\"RGB\", "
#else
            "\"BGRx\", " "\"BGR\", "
#endif
            "\"RGB16\" } "))
    );

static GstStaticPadTemplate sink_template = GST_STATIC_PAD_TEMPLATE ("sink",
    GST_PAD_SINK,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS ("audio/x-raw, "
        "format = (string) " GST_AUDIO_NE (S16) ", "
        "layout = (string) interleaved, " "channels = (int) { 1, 2 }, "
#if defined(VISUAL_API_VERSION) && VISUAL_API_VERSION >= 4000 && VISUAL_API_VERSION < 5000
        "rate = (int) { 8000, 11250, 22500, 32000, 44100, 48000, 96000 }"
#else
        "rate = (int) [ 1000, MAX ]"
#endif
    )
    );


static void gst_visual_class_init (gpointer g_class, gpointer class_data);
static void gst_visual_init (GstVisual * visual);
static void gst_visual_finalize (GObject * object);

static GstStateChangeReturn gst_visual_change_state (GstElement * element,
    GstStateChange transition);
static GstFlowReturn gst_visual_chain (GstPad * pad, GstObject * parent,
    GstBuffer * buffer);
static gboolean gst_visual_sink_event (GstPad * pad, GstObject * parent,
    GstEvent * event);
static gboolean gst_visual_src_event (GstPad * pad, GstObject * parent,
    GstEvent * event);

static gboolean gst_visual_src_query (GstPad * pad, GstObject * parent,
    GstQuery * query);

static gboolean gst_visual_sink_setcaps (GstPad * pad, GstCaps * caps);
static GstCaps *gst_visual_getcaps (GstPad * pad, GstCaps * filter);
static void libvisual_log_handler (const char *message, const char *funcname,
    void *priv);

static GstElementClass *parent_class = NULL;

GType
gst_visual_get_type (void)
{
  static GType type = 0;

  if (G_UNLIKELY (type == 0)) {
    static const GTypeInfo info = {
      sizeof (GstVisualClass),
      NULL,
      NULL,
      gst_visual_class_init,
      NULL,
      NULL,
      sizeof (GstVisual),
      0,
      (GInstanceInitFunc) gst_visual_init,
    };

    type = g_type_register_static (GST_TYPE_ELEMENT, "GstVisual", &info, 0);
  }
  return type;
}

static void
libvisual_log_handler (const char *message, const char *funcname, void *priv)
{
  GST_CAT_LEVEL_LOG (libvisual_debug, (GstDebugLevel) (priv), NULL, "%s - %s",
      funcname, message);
}

static void
gst_visual_class_init (gpointer g_class, gpointer class_data)
{
  GstVisualClass *klass = GST_VISUAL_CLASS (g_class);
  GstElementClass *element = GST_ELEMENT_CLASS (g_class);
  GObjectClass *object = G_OBJECT_CLASS (g_class);

  klass->plugin = class_data;

  element->change_state = gst_visual_change_state;

  if (class_data == NULL) {
    parent_class = g_type_class_peek_parent (g_class);
  } else {
    char *longname = g_strdup_printf ("libvisual %s plugin v.%s",
        klass->plugin->info->name, klass->plugin->info->version);

    /* FIXME: improve to only register what plugin supports? */
    gst_element_class_add_pad_template (element,
        gst_static_pad_template_get (&src_template));
    gst_element_class_add_pad_template (element,
        gst_static_pad_template_get (&sink_template));

    gst_element_class_set_details_simple (element,
        longname, "Visualization",
        klass->plugin->info->about, "Benjamin Otte <otte@gnome.org>");

    g_free (longname);
  }

  object->finalize = gst_visual_finalize;
}

static void
gst_visual_init (GstVisual * visual)
{
  /* create the sink and src pads */
  visual->sinkpad = gst_pad_new_from_static_template (&sink_template, "sink");
  gst_pad_set_chain_function (visual->sinkpad, gst_visual_chain);
  gst_pad_set_event_function (visual->sinkpad, gst_visual_sink_event);
  gst_element_add_pad (GST_ELEMENT (visual), visual->sinkpad);

  visual->srcpad = gst_pad_new_from_static_template (&src_template, "src");
  gst_pad_set_event_function (visual->srcpad, gst_visual_src_event);
  gst_pad_set_query_function (visual->srcpad, gst_visual_src_query);
  gst_element_add_pad (GST_ELEMENT (visual), visual->srcpad);

  visual->adapter = gst_adapter_new ();
}

static void
gst_visual_clear_actors (GstVisual * visual)
{
  if (visual->actor) {
    visual_object_unref (VISUAL_OBJECT (visual->actor));
    visual->actor = NULL;
  }
  if (visual->video) {
    visual_object_unref (VISUAL_OBJECT (visual->video));
    visual->video = NULL;
  }
  if (visual->audio) {
    visual_object_unref (VISUAL_OBJECT (visual->audio));
    visual->audio = NULL;
  }
}

static void
gst_visual_finalize (GObject * object)
{
  GstVisual *visual = GST_VISUAL (object);

  g_object_unref (visual->adapter);
  if (visual->pool)
    gst_object_unref (visual->pool);
  gst_visual_clear_actors (visual);

  GST_CALL_PARENT (G_OBJECT_CLASS, finalize, (object));
}

static void
gst_visual_reset (GstVisual * visual)
{
  gst_adapter_clear (visual->adapter);
  gst_segment_init (&visual->segment, GST_FORMAT_UNDEFINED);

  GST_OBJECT_LOCK (visual);
  visual->proportion = 1.0;
  visual->earliest_time = -1;
  GST_OBJECT_UNLOCK (visual);
}

static GstCaps *
gst_visual_getcaps (GstPad * pad, GstCaps * filter)
{
  GstCaps *ret;
  GstVisual *visual = GST_VISUAL (GST_PAD_PARENT (pad));
  int depths;

  if (!visual->actor) {
    ret = gst_pad_get_pad_template_caps (visual->srcpad);
    goto beach;
  }

  ret = gst_caps_new_empty ();
  depths = visual_actor_get_supported_depth (visual->actor);
  if (depths < 0) {
    /* FIXME: set an error */
    goto beach;
  }
  if (depths == VISUAL_VIDEO_DEPTH_GL) {
    /* We can't handle GL only plugins */
    goto beach;
  }

  GST_DEBUG_OBJECT (visual, "libvisual plugin supports depths %u (0x%04x)",
      depths, depths);
  /* if (depths & VISUAL_VIDEO_DEPTH_32BIT) Always supports 32bit output */
#if G_BYTE_ORDER == G_BIG_ENDIAN
  gst_caps_append (ret, gst_caps_from_string (GST_VIDEO_CAPS_MAKE ("xRGB")));
#else
  gst_caps_append (ret, gst_caps_from_string (GST_VIDEO_CAPS_MAKE ("BGRx")));
#endif

  if (depths & VISUAL_VIDEO_DEPTH_24BIT) {
#if G_BYTE_ORDER == G_BIG_ENDIAN
    gst_caps_append (ret, gst_caps_from_string (GST_VIDEO_CAPS_MAKE ("RGB")));
#else
    gst_caps_append (ret, gst_caps_from_string (GST_VIDEO_CAPS_MAKE ("BGR")));
#endif
  }
  if (depths & VISUAL_VIDEO_DEPTH_16BIT) {
    gst_caps_append (ret, gst_caps_from_string (GST_VIDEO_CAPS_MAKE ("RGB16")));
  }

beach:

  if (filter) {
    GstCaps *intersection;

    intersection =
        gst_caps_intersect_full (filter, ret, GST_CAPS_INTERSECT_FIRST);
    gst_caps_unref (ret);
    ret = intersection;
  }

  GST_DEBUG_OBJECT (visual, "returning caps %" GST_PTR_FORMAT, ret);

  return ret;
}

static gboolean
gst_visual_src_setcaps (GstVisual * visual, GstCaps * caps)
{
  gboolean res;
  GstStructure *structure;
  gint depth, pitch, rate;
  const gchar *fmt;

  structure = gst_caps_get_structure (caps, 0);

  GST_DEBUG_OBJECT (visual, "src pad got caps %" GST_PTR_FORMAT, caps);

  if (!gst_structure_get_int (structure, "width", &visual->width))
    goto error;
  if (!gst_structure_get_int (structure, "height", &visual->height))
    goto error;
  if (!(fmt = gst_structure_get_string (structure, "format")))
    goto error;
  if (!gst_structure_get_fraction (structure, "framerate", &visual->fps_n,
          &visual->fps_d))
    goto error;

  if (!strcmp (fmt, "BGR") || !strcmp (fmt, "RGB"))
    depth = 24;
  else if (!strcmp (fmt, "BGRx") || !strcmp (fmt, "xRGB"))
    depth = 32;
  else
    depth = 16;

  visual_video_set_depth (visual->video,
      visual_video_depth_enum_from_value (depth));
  visual_video_set_dimension (visual->video, visual->width, visual->height);
  pitch = GST_ROUND_UP_4 (visual->width * visual->video->bpp);
  visual_video_set_pitch (visual->video, pitch);
  visual_actor_video_negotiate (visual->actor, 0, FALSE, FALSE);

  rate = GST_AUDIO_INFO_RATE (&visual->info);

  /* precalc some values */
  visual->outsize = visual->video->height * pitch;
  visual->spf = gst_util_uint64_scale_int (rate, visual->fps_d, visual->fps_n);
  visual->duration =
      gst_util_uint64_scale_int (GST_SECOND, visual->fps_d, visual->fps_n);

  res = gst_pad_push_event (visual->srcpad, gst_event_new_caps (caps));

  return res;

  /* ERRORS */
error:
  {
    GST_DEBUG_OBJECT (visual, "error parsing caps");
    return FALSE;
  }
}

static gboolean
gst_visual_sink_setcaps (GstPad * pad, GstCaps * caps)
{
  GstVisual *visual = GST_VISUAL (GST_PAD_PARENT (pad));
  GstAudioInfo info;
  gint rate;

  if (!gst_audio_info_from_caps (&info, caps))
    goto invalid_caps;

  visual->info = info;

  rate = GST_AUDIO_INFO_RATE (&info);

  /* this is how many samples we need to fill one frame at the requested
   * framerate. */
  if (visual->fps_n != 0) {
    visual->spf =
        gst_util_uint64_scale_int (rate, visual->fps_d, visual->fps_n);
  }

  return TRUE;

  /* ERRORS */
invalid_caps:
  {
    GST_ERROR_OBJECT (visual, "invalid caps received");
    return FALSE;
  }
}

static gboolean
gst_vis_src_negotiate (GstVisual * visual)
{
  GstCaps *othercaps, *target;
  GstStructure *structure;
  GstCaps *caps;
  GstQuery *query;
  GstBufferPool *pool = NULL;
  GstStructure *config;
  guint size, min, max;

  caps = gst_pad_query_caps (visual->srcpad, NULL);

  /* see what the peer can do */
  othercaps = gst_pad_peer_query_caps (visual->srcpad, caps);
  if (othercaps) {
    target = othercaps;
    gst_caps_unref (caps);

    if (gst_caps_is_empty (target))
      goto no_format;

    target = gst_caps_truncate (target);
  } else {
    /* need a copy, we'll be modifying it when fixating */
    target = gst_caps_ref (caps);
  }
  GST_DEBUG_OBJECT (visual, "before fixate caps %" GST_PTR_FORMAT, target);

  target = gst_caps_make_writable (target);
  /* fixate in case something is not fixed. This does nothing if the value is
   * already fixed. For video we always try to fixate to something like
   * 320x240x25 by convention. */
  structure = gst_caps_get_structure (target, 0);
  gst_structure_fixate_field_nearest_int (structure, "width", DEFAULT_WIDTH);
  gst_structure_fixate_field_nearest_int (structure, "height", DEFAULT_HEIGHT);
  gst_structure_fixate_field_nearest_fraction (structure, "framerate",
      DEFAULT_FPS_N, DEFAULT_FPS_D);
  target = gst_caps_fixate (target);

  GST_DEBUG_OBJECT (visual, "after fixate caps %" GST_PTR_FORMAT, target);

  gst_visual_src_setcaps (visual, target);

  /* try to get a bufferpool now */
  /* find a pool for the negotiated caps now */
  query = gst_query_new_allocation (target, TRUE);

  if (!gst_pad_peer_query (visual->srcpad, query)) {
    /* not a problem, we deal with the defaults of the query */
    GST_DEBUG_OBJECT (visual, "allocation query failed");
  }

  if (gst_query_get_n_allocation_pools (query) > 0) {
    gst_query_parse_nth_allocation_pool (query, 0, &pool, &size, &min, &max);

    size = MAX (size, visual->outsize);
  } else {
    pool = NULL;
    size = visual->outsize;
    min = max = 0;
  }

  if (pool == NULL) {
    /* no pool, just parameters, we can make our own */
    GST_DEBUG_OBJECT (visual, "no pool, making new pool");
    pool = gst_video_buffer_pool_new ();
  }

  /* and configure */
  config = gst_buffer_pool_get_config (pool);
  gst_buffer_pool_config_set_params (config, target, size, min, max);
  gst_buffer_pool_set_config (pool, config);

  if (visual->pool)
    gst_object_unref (visual->pool);
  visual->pool = pool;

  /* and activate */
  gst_buffer_pool_set_active (pool, TRUE);

  gst_caps_unref (target);

  return TRUE;

  /* ERRORS */
no_format:
  {
    GST_ELEMENT_ERROR (visual, STREAM, FORMAT, (NULL),
        ("could not negotiate output format"));
    gst_caps_unref (target);
    return FALSE;
  }
}

static gboolean
gst_visual_sink_event (GstPad * pad, GstObject * parent, GstEvent * event)
{
  GstVisual *visual;
  gboolean res;

  visual = GST_VISUAL (parent);

  switch (GST_EVENT_TYPE (event)) {
    case GST_EVENT_FLUSH_START:
      res = gst_pad_push_event (visual->srcpad, event);
      break;
    case GST_EVENT_FLUSH_STOP:
      /* reset QoS and adapter. */
      gst_visual_reset (visual);
      res = gst_pad_push_event (visual->srcpad, event);
      break;
    case GST_EVENT_CAPS:
    {
      GstCaps *caps;

      gst_event_parse_caps (event, &caps);
      res = gst_visual_sink_setcaps (pad, caps);
      gst_event_unref (event);
      break;
    }
    case GST_EVENT_SEGMENT:
    {
      /* the newsegment values are used to clip the input samples
       * and to convert the incomming timestamps to running time so
       * we can do QoS */
      gst_event_copy_segment (event, &visual->segment);

      /* and forward */
      res = gst_pad_push_event (visual->srcpad, event);
      break;
    }
    default:
      res = gst_pad_event_default (pad, parent, event);
      break;
  }

  return res;
}

static gboolean
gst_visual_src_event (GstPad * pad, GstObject * parent, GstEvent * event)
{
  GstVisual *visual;
  gboolean res;

  visual = GST_VISUAL (parent);

  switch (GST_EVENT_TYPE (event)) {
    case GST_EVENT_QOS:
    {
      gdouble proportion;
      GstClockTimeDiff diff;
      GstClockTime timestamp;

      gst_event_parse_qos (event, NULL, &proportion, &diff, &timestamp);

      /* save stuff for the _chain function */
      GST_OBJECT_LOCK (visual);
      visual->proportion = proportion;
      if (diff >= 0)
        /* we're late, this is a good estimate for next displayable
         * frame (see part-qos.txt) */
        visual->earliest_time = timestamp + 2 * diff + visual->duration;
      else
        visual->earliest_time = timestamp + diff;

      GST_OBJECT_UNLOCK (visual);

      res = gst_pad_push_event (visual->sinkpad, event);
      break;
    }
    case GST_EVENT_RECONFIGURE:
      /* dont't forward */
      gst_event_unref (event);
      res = TRUE;
      break;
    default:
      res = gst_pad_event_default (pad, parent, event);
      break;
  }

  return res;
}

static gboolean
gst_visual_src_query (GstPad * pad, GstObject * parent, GstQuery * query)
{
  gboolean res;
  GstVisual *visual;

  visual = GST_VISUAL (parent);

  switch (GST_QUERY_TYPE (query)) {
    case GST_QUERY_LATENCY:
    {
      /* We need to send the query upstream and add the returned latency to our
       * own */
      GstClockTime min_latency, max_latency;
      gboolean us_live;
      GstClockTime our_latency;
      guint max_samples;

      if ((res = gst_pad_peer_query (visual->sinkpad, query))) {
        gst_query_parse_latency (query, &us_live, &min_latency, &max_latency);

        GST_DEBUG_OBJECT (visual, "Peer latency: min %"
            GST_TIME_FORMAT " max %" GST_TIME_FORMAT,
            GST_TIME_ARGS (min_latency), GST_TIME_ARGS (max_latency));

        /* the max samples we must buffer */
        max_samples = MAX (VISUAL_SAMPLES, visual->spf);
        our_latency =
            gst_util_uint64_scale_int (max_samples, GST_SECOND,
            GST_AUDIO_INFO_RATE (&visual->info));

        GST_DEBUG_OBJECT (visual, "Our latency: %" GST_TIME_FORMAT,
            GST_TIME_ARGS (our_latency));

        /* we add some latency but only if we need to buffer more than what
         * upstream gives us */
        min_latency += our_latency;
        if (max_latency != -1)
          max_latency += our_latency;

        GST_DEBUG_OBJECT (visual, "Calculated total latency : min %"
            GST_TIME_FORMAT " max %" GST_TIME_FORMAT,
            GST_TIME_ARGS (min_latency), GST_TIME_ARGS (max_latency));

        gst_query_set_latency (query, TRUE, min_latency, max_latency);
      }
      break;
    }
    case GST_QUERY_CAPS:
    {
      GstCaps *filter, *caps;

      gst_query_parse_caps (query, &filter);
      caps = gst_visual_getcaps (pad, filter);
      gst_query_set_caps_result (query, caps);
      gst_caps_unref (caps);
      res = TRUE;
    }
    default:
      res = gst_pad_query_default (pad, parent, query);
      break;
  }

  return res;
}

/* Make sure we are negotiated */
static GstFlowReturn
ensure_negotiated (GstVisual * visual)
{
  gboolean reconfigure;

  reconfigure = gst_pad_check_reconfigure (visual->srcpad);

  /* we don't know an output format yet, pick one */
  if (reconfigure || !gst_pad_has_current_caps (visual->srcpad)) {
    if (!gst_vis_src_negotiate (visual))
      return GST_FLOW_NOT_NEGOTIATED;
  }
  return GST_FLOW_OK;
}

static GstFlowReturn
gst_visual_chain (GstPad * pad, GstObject * parent, GstBuffer * buffer)
{
  GstBuffer *outbuf = NULL;
  guint i;
  GstVisual *visual = GST_VISUAL (parent);
  GstFlowReturn ret = GST_FLOW_OK;
  guint avail;
  gint bpf, rate, channels;

  GST_DEBUG_OBJECT (visual, "chain function called");

  /* Make sure have an output format */
  ret = ensure_negotiated (visual);
  if (ret != GST_FLOW_OK) {
    gst_buffer_unref (buffer);
    goto beach;
  }

  /* resync on DISCONT */
  if (GST_BUFFER_FLAG_IS_SET (buffer, GST_BUFFER_FLAG_DISCONT)) {
    gst_adapter_clear (visual->adapter);
  }

  rate = GST_AUDIO_INFO_RATE (&visual->info);
  bpf = GST_AUDIO_INFO_BPF (&visual->info);
  channels = GST_AUDIO_INFO_CHANNELS (&visual->info);

  GST_DEBUG_OBJECT (visual,
      "Input buffer has %" G_GSIZE_FORMAT " samples, time=%" G_GUINT64_FORMAT,
      gst_buffer_get_size (buffer) / bpf, GST_BUFFER_TIMESTAMP (buffer));

  gst_adapter_push (visual->adapter, buffer);

  while (TRUE) {
    gboolean need_skip;
    const guint16 *data;
    guint64 dist, timestamp;
    GstMapInfo outmap;

    GST_DEBUG_OBJECT (visual, "processing buffer");

    avail = gst_adapter_available (visual->adapter);
    GST_DEBUG_OBJECT (visual, "avail now %u", avail);

    /* we need at least VISUAL_SAMPLES samples */
    if (avail < VISUAL_SAMPLES * bpf)
      break;

    /* we need at least enough samples to make one frame */
    if (avail < visual->spf * bpf)
      break;

    /* get timestamp of the current adapter byte */
    timestamp = gst_adapter_prev_timestamp (visual->adapter, &dist);
    if (GST_CLOCK_TIME_IS_VALID (timestamp)) {
      /* convert bytes to time */
      dist /= bpf;
      timestamp += gst_util_uint64_scale_int (dist, GST_SECOND, rate);
    }

    if (timestamp != -1) {
      gint64 qostime;

      /* QoS is done on running time */
      qostime = gst_segment_to_running_time (&visual->segment, GST_FORMAT_TIME,
          timestamp);
      qostime += visual->duration;

      GST_OBJECT_LOCK (visual);
      /* check for QoS, don't compute buffers that are known to be late */
      need_skip = visual->earliest_time != -1 &&
          qostime <= visual->earliest_time;
      GST_OBJECT_UNLOCK (visual);

      if (need_skip) {
        GST_WARNING_OBJECT (visual,
            "QoS: skip ts: %" GST_TIME_FORMAT ", earliest: %" GST_TIME_FORMAT,
            GST_TIME_ARGS (qostime), GST_TIME_ARGS (visual->earliest_time));
        goto skip;
      }
    }

    /* Read VISUAL_SAMPLES samples per channel */
    data =
        (const guint16 *) gst_adapter_map (visual->adapter,
        VISUAL_SAMPLES * bpf);

#if defined(VISUAL_API_VERSION) && VISUAL_API_VERSION >= 4000 && VISUAL_API_VERSION < 5000
    {
      VisBuffer *lbuf, *rbuf;
      guint16 ldata[VISUAL_SAMPLES], rdata[VISUAL_SAMPLES];
      VisAudioSampleRateType vrate;

      lbuf = visual_buffer_new_with_buffer (ldata, sizeof (ldata), NULL);
      rbuf = visual_buffer_new_with_buffer (rdata, sizeof (rdata), NULL);

      if (channels == 2) {
        for (i = 0; i < VISUAL_SAMPLES; i++) {
          ldata[i] = *data++;
          rdata[i] = *data++;
        }
      } else {
        for (i = 0; i < VISUAL_SAMPLES; i++) {
          ldata[i] = *data;
          rdata[i] = *data++;
        }
      }

      switch (rate) {
        case 8000:
          vrate = VISUAL_AUDIO_SAMPLE_RATE_8000;
          break;
        case 11250:
          vrate = VISUAL_AUDIO_SAMPLE_RATE_11250;
          break;
        case 22500:
          vrate = VISUAL_AUDIO_SAMPLE_RATE_22500;
          break;
        case 32000:
          vrate = VISUAL_AUDIO_SAMPLE_RATE_32000;
          break;
        case 44100:
          vrate = VISUAL_AUDIO_SAMPLE_RATE_44100;
          break;
        case 48000:
          vrate = VISUAL_AUDIO_SAMPLE_RATE_48000;
          break;
        case 96000:
          vrate = VISUAL_AUDIO_SAMPLE_RATE_96000;
          break;
        default:
          visual_object_unref (VISUAL_OBJECT (lbuf));
          visual_object_unref (VISUAL_OBJECT (rbuf));
          GST_ERROR_OBJECT (visual, "unsupported rate %d", rate);
          ret = GST_FLOW_ERROR;
          goto beach;
          break;
      }

      visual_audio_samplepool_input_channel (visual->audio->samplepool,
          lbuf,
          vrate, VISUAL_AUDIO_SAMPLE_FORMAT_S16,
          (char *) VISUAL_AUDIO_CHANNEL_LEFT);
      visual_audio_samplepool_input_channel (visual->audio->samplepool, rbuf,
          vrate, VISUAL_AUDIO_SAMPLE_FORMAT_S16,
          (char *) VISUAL_AUDIO_CHANNEL_RIGHT);

      visual_object_unref (VISUAL_OBJECT (lbuf));
      visual_object_unref (VISUAL_OBJECT (rbuf));

    }
#else
    if (visual->channels == 2) {
      for (i = 0; i < VISUAL_SAMPLES; i++) {
        visual->audio->plugpcm[0][i] = *data++;
        visual->audio->plugpcm[1][i] = *data++;
      }
    } else {
      for (i = 0; i < VISUAL_SAMPLES; i++) {
        visual->audio->plugpcm[0][i] = *data;
        visual->audio->plugpcm[1][i] = *data++;
      }
    }
#endif

    /* alloc a buffer if we don't have one yet, this happens
     * when we pushed a buffer in this while loop before */
    if (outbuf == NULL) {
      GST_DEBUG_OBJECT (visual, "allocating output buffer");
      ret = gst_buffer_pool_acquire_buffer (visual->pool, &outbuf, NULL);
      if (ret != GST_FLOW_OK) {
        gst_adapter_unmap (visual->adapter);
        goto beach;
      }
    }
    gst_buffer_map (outbuf, &outmap, GST_MAP_WRITE);
    visual_video_set_buffer (visual->video, outmap.data);
    visual_audio_analyze (visual->audio);
    visual_actor_run (visual->actor, visual->audio);
    visual_video_set_buffer (visual->video, NULL);
    gst_buffer_unmap (outbuf, &outmap);
    GST_DEBUG_OBJECT (visual, "rendered one frame");

    gst_adapter_unmap (visual->adapter);

    GST_BUFFER_TIMESTAMP (outbuf) = timestamp;
    GST_BUFFER_DURATION (outbuf) = visual->duration;

    ret = gst_pad_push (visual->srcpad, outbuf);
    outbuf = NULL;

  skip:
    GST_DEBUG_OBJECT (visual, "finished frame, flushing %u samples from input",
        visual->spf);

    /* Flush out the number of samples per frame */
    gst_adapter_flush (visual->adapter, visual->spf * bpf);

    /* quit the loop if something was wrong */
    if (ret != GST_FLOW_OK)
      break;
  }

beach:

  if (outbuf != NULL)
    gst_buffer_unref (outbuf);

  return ret;
}

static GstStateChangeReturn
gst_visual_change_state (GstElement * element, GstStateChange transition)
{
  GstVisual *visual = GST_VISUAL (element);
  GstStateChangeReturn ret;

  switch (transition) {
    case GST_STATE_CHANGE_NULL_TO_READY:
      visual->actor =
          visual_actor_new (GST_VISUAL_GET_CLASS (visual)->plugin->
          info->plugname);
      visual->video = visual_video_new ();
      visual->audio = visual_audio_new ();
      /* can't have a play without actors */
      if (!visual->actor || !visual->video)
        goto no_actors;

      if (visual_actor_realize (visual->actor) != 0)
        goto no_realize;

      visual_actor_set_video (visual->actor, visual->video);
      break;
    case GST_STATE_CHANGE_READY_TO_PAUSED:
      gst_visual_reset (visual);
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
      if (visual->pool) {
        gst_buffer_pool_set_active (visual->pool, FALSE);
        gst_object_unref (visual->pool);
        visual->pool = NULL;
      }
      break;
    case GST_STATE_CHANGE_READY_TO_NULL:
      gst_visual_clear_actors (visual);
      break;
    default:
      break;
  }

  return ret;

  /* ERRORS */
no_actors:
  {
    GST_ELEMENT_ERROR (visual, LIBRARY, INIT, (NULL),
        ("could not create actors"));
    gst_visual_clear_actors (visual);
    return GST_STATE_CHANGE_FAILURE;
  }
no_realize:
  {
    GST_ELEMENT_ERROR (visual, LIBRARY, INIT, (NULL),
        ("could not realize actor"));
    gst_visual_clear_actors (visual);
    return GST_STATE_CHANGE_FAILURE;
  }
}

static void
make_valid_name (char *name)
{
  /*
   * Replace invalid chars with _ in the type name
   */
  static const gchar extra_chars[] = "-_+";
  gchar *p = name;

  for (; *p; p++) {
    int valid = ((p[0] >= 'A' && p[0] <= 'Z') ||
        (p[0] >= 'a' && p[0] <= 'z') ||
        (p[0] >= '0' && p[0] <= '9') || strchr (extra_chars, p[0]));
    if (!valid)
      *p = '_';
  }
}

static gboolean
gst_visual_actor_plugin_is_gl (VisObject * plugin, const gchar * name)
{
  gboolean is_gl;
  gint depth;

#if !defined(VISUAL_API_VERSION)

  depth = VISUAL_PLUGIN_ACTOR (plugin)->depth;
  is_gl = (depth == VISUAL_VIDEO_DEPTH_GL);

#elif VISUAL_API_VERSION >= 4000 && VISUAL_API_VERSION < 5000

  depth = VISUAL_ACTOR_PLUGIN (plugin)->vidoptions.depth;
  /* FIXME: how to figure this out correctly in 0.4? */
  is_gl = (depth & VISUAL_VIDEO_DEPTH_GL) == VISUAL_VIDEO_DEPTH_GL;

#else
# error what libvisual version is this?
#endif

  if (!is_gl) {
    GST_DEBUG ("plugin %s is not a GL plugin (%d), registering", name, depth);
  } else {
    GST_DEBUG ("plugin %s is a GL plugin (%d), ignoring", name, depth);
  }

  return is_gl;
}

static gboolean
plugin_init (GstPlugin * plugin)
{
  guint i, count;
  VisList *list;

  GST_DEBUG_CATEGORY_INIT (libvisual_debug, "libvisual", 0,
      "libvisual audio visualisations");

#ifdef LIBVISUAL_PLUGINSBASEDIR
  gst_plugin_add_dependency_simple (plugin, "HOME/.libvisual/actor",
      LIBVISUAL_PLUGINSBASEDIR "/actor", NULL, GST_PLUGIN_DEPENDENCY_FLAG_NONE);
#endif

  visual_log_set_verboseness (VISUAL_LOG_VERBOSENESS_LOW);
  visual_log_set_info_handler (libvisual_log_handler, (void *) GST_LEVEL_INFO);
  visual_log_set_warning_handler (libvisual_log_handler,
      (void *) GST_LEVEL_WARNING);
  visual_log_set_critical_handler (libvisual_log_handler,
      (void *) GST_LEVEL_ERROR);
  visual_log_set_error_handler (libvisual_log_handler,
      (void *) GST_LEVEL_ERROR);

  if (!visual_is_initialized ())
    if (visual_init (NULL, NULL) != 0)
      return FALSE;

  list = visual_actor_get_list ();

#if !defined(VISUAL_API_VERSION)
  count = visual_list_count (list);
#elif VISUAL_API_VERSION >= 4000 && VISUAL_API_VERSION < 5000
  count = visual_collection_size (VISUAL_COLLECTION (list));
#endif

  for (i = 0; i < count; i++) {
    VisPluginRef *ref = visual_list_get (list, i);
    VisPluginData *visplugin = NULL;
    gboolean skip = FALSE;
    GType type;
    gchar *name;
    GTypeInfo info = {
      sizeof (GstVisualClass),
      NULL,
      NULL,
      gst_visual_class_init,
      NULL,
      ref,
      sizeof (GstVisual),
      0,
      NULL
    };

    visplugin = visual_plugin_load (ref);

    if (ref->info->plugname == NULL)
      continue;

    /* Blacklist some plugins */
    if (strcmp (ref->info->plugname, "gstreamer") == 0 ||
        strcmp (ref->info->plugname, "gdkpixbuf") == 0) {
      skip = TRUE;
    } else {
      /* Ignore plugins that only support GL output for now */
      skip = gst_visual_actor_plugin_is_gl (visplugin->info->plugin,
          visplugin->info->plugname);
    }

    visual_plugin_unload (visplugin);

    if (!skip) {
      name = g_strdup_printf ("GstVisual%s", ref->info->plugname);
      make_valid_name (name);
      type = g_type_register_static (GST_TYPE_VISUAL, name, &info, 0);
      g_free (name);

      name = g_strdup_printf ("libvisual_%s", ref->info->plugname);
      make_valid_name (name);
      if (!gst_element_register (plugin, name, GST_RANK_NONE, type)) {
        g_free (name);
        return FALSE;
      }
      g_free (name);
    }
  }

  return TRUE;
}

GST_PLUGIN_DEFINE (GST_VERSION_MAJOR,
    GST_VERSION_MINOR,
    "libvisual",
    "libvisual visualization plugins",
    plugin_init, VERSION, "LGPL", GST_PACKAGE_NAME, GST_PACKAGE_ORIGIN)
