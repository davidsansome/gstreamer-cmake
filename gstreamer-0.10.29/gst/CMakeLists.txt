cmake_minimum_required(VERSION 2.6)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${GLIB_INCLUDE_DIRS})
include_directories(${LIBXML_INCLUDE_DIRS})

# Optional sources
if (NOT ${GST_DISABLE_LOADSAVE})
  set(GST_LOADSAVE_SRC gstxml.c)
endif (NOT ${GST_DISABLE_LOADSAVE})

if (NOT ${GST_DISABLE_REGISTRY})
  set(GST_REGISTRY_SRC gstregistrybinary.c)
endif (NOT ${GST_DISABLE_REGISTRY})

if (NOT ${GST_DISABLE_TRACE})
  set(GST_TRACE_SRC gsttrace.c)
endif (NOT ${GST_DISABLE_TRACE})

# TODO: parse

set(SOURCES
  gst.c
  gstobject.c
  gstbin.c
  gstbuffer.c
  gstbufferlist.c
  gstbus.c
  gstcaps.c
  gstchildproxy.c
  gstclock.c
  gstdebugutils.c
  gstelement.c
  gstelementfactory.c
  gsterror.c
  gstevent.c
  gstfilter.c
  gstformat.c
  gstghostpad.c
  gstindex.c
  gstindexfactory.c
  gstinfo.c
  gstinterface.c
  gstiterator.c
  gstmessage.c
  gstminiobject.c
  gstpad.c
  gstpadtemplate.c 
  gstparamspecs.c
  gstpipeline.c
  gstplugin.c
  gstpluginfeature.c
  gstpluginloader.c
  gstpoll.c
  gstpreset.c
  gstquark.c
  gstquery.c
  gstregistry.c
  gstregistrychunks.c
  gstsegment.c
  gststructure.c
  gstsystemclock.c
  gsttaglist.c
  gsttagsetter.c
  gsttask.c
  gsttaskpool.c
  ${GST_TRACE_SRC}
  gsttypefind.c
  gsttypefindfactory.c
  gsturi.c
  gstutils.c
  gstvalue.c
  gstparse.c
  ${GST_REGISTRY_SRC}
  ${GST_LOADSAVE_SRC}
)

set(HEADERS
  gst.h
  glib-compat.h
  gstobject.h
  gstbin.h
  gstbuffer.h
  gstbufferlist.h
  gstbus.h
  gstcaps.h
  gstchildproxy.h
  gstclock.h
  gstcompat.h
  gstdebugutils.h
  gstelement.h
  gstelementfactory.h
  gsterror.h
  gstevent.h
  gstfilter.h
  gstformat.h
  gstghostpad.h
  gstindex.h
  gstindexfactory.h
  gstinfo.h
  gstinterface.h
  gstiterator.h
  gstmacros.h
  gstmessage.h
  gstminiobject.h
  gstpad.h
  gstpadtemplate.h
  gstparamspecs.h
  gstpipeline.h
  gstplugin.h
  gstpluginfeature.h
  gstpoll.h
  gstpreset.h
  gstquery.h
  gstsegment.h
  gststructure.h
  gstsystemclock.h
  gsttaglist.h
  gsttagsetter.h
  gsttask.h
  gsttaskpool.h
  gsttrace.h
  gsttypefind.h
  gsttypefindfactory.h
  gsturi.h
  gstutils.h
  gstvalue.h
  gstregistry.h
  gstparse.h
  gstxml.h
)

add_glib_marshal(SOURCES gstmarshal gst_marshal gst_private.h)
add_glib_enumtypes(SOURCES gstenumtypes ${HEADERS})

add_library(gstreamer-${GST_MAJORMINOR} SHARED ${SOURCES})

target_link_libraries(gstreamer-${GST_MAJORMINOR}
  ${GLIB_LIBRARIES}
  ${LIBXML_LIBRARIES}
)