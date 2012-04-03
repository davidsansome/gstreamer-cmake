cmake_minimum_required(VERSION 2.6)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/parse)
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR}/parse)
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/parse)

add_definitions(-DG_LOG_DOMAIN=g_log_domain_gstreamer)

# Optional sources
if (NOT ${GST_DISABLE_REGISTRY})
  set(GST_REGISTRY_SRC gstregistrybinary.c)
endif()

if (NOT ${GST_DISABLE_TRACE})
  set(GST_TRACE_SRC gsttrace.c)
endif()

BISON_TARGET(gstparse-parser
  parse/grammar.y
  ${CMAKE_CURRENT_BINARY_DIR}/grammar.tab.c
  COMPILE_FLAGS -ppriv_gst_parse_yy
)
FLEX_TARGET(gstparse-lexer
  parse/parse.l
  ${CMAKE_CURRENT_BINARY_DIR}/lex._gst_parse_yy.c
  COMPILE_FLAGS -Ppriv_gst_parse_yy
)
ADD_FLEX_BISON_DEPENDENCY(gstparse-lexer gstparse-parser)

set(SOURCES
  ${BISON_gstparse-parser_OUTPUTS}
  ${FLEX_gstparse-lexer_OUTPUTS}
  ${GST_LOADSAVE_SRC}
  ${GST_REGISTRY_SRC}
  ${GST_TRACE_SRC}
  gst.c
  gstatomicqueue.c
  gstbin.c
  gstbuffer.c
  gstbufferlist.c
  gstbufferpool.c
  gstbus.c
  gstcaps.c
  gstchildproxy.c
  gstclock.c
  gstcontrolbinding.c
  gstcontrolsource.c
  gstdatetime.c
  gstdebugutils.c
  gstelement.c
  gstelementfactory.c
  gsterror.c
  gstevent.c
  gstformat.c
  gstghostpad.c
  gstinfo.c
  gstiterator.c
  gstmemory.c
  gstmessage.c
  gstmeta.c
  gstminiobject.c
  gstobject.c
  gstpad.c
  gstpadtemplate.c 
  gstparamspecs.c
  gstparse.c
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
  gstsample.c
  gstsegment.c
  gststructure.c
  gstsystemclock.c
  gsttaglist.c
  gsttagsetter.c
  gsttask.c
  gsttaskpool.c
  gsttypefind.c
  gsttypefindfactory.c
  gsturi.c
  gstutils.c
  gstvalue.c
)

set(HEADERS
  glib-compat.h
  gst.h
  gstatomicqueue.h
  gstbin.h
  gstbuffer.h
  gstbufferlist.h
  gstbufferpool.h
  gstbus.h
  gstcaps.h
  gstchildproxy.h
  gstclock.h
  gstcompat.h
  gstcontrolbinding.h
  gstcontrolsource.h
  gstdatetime.h
  gstdebugutils.h
  gstelement.h
  gstelementfactory.h
  gstelementmetadata.h
  gsterror.h
  gstevent.h
  gstformat.h
  gstghostpad.h
  gstinfo.h
  gstiterator.h
  gstmacros.h
  gstmemory.h
  gstmessage.h
  gstmeta.h
  gstminiobject.h
  gstobject.h
  gstpad.h
  gstpadtemplate.h
  gstparamspecs.h
  gstparse.h
  gstpipeline.h
  gstplugin.h
  gstpluginfeature.h
  gstpoll.h
  gstpreset.h
  gstquery.h
  gstregistry.h
  gstsample.h
  gstsegment.h
  gststructure.h
  gstsystemclock.h
  gsttaglist.h
  gsttagsetter.h
  gsttask.h
  gsttaskpool.h
  gsttypefind.h
  gsttypefindfactory.h
  gsturi.h
  gstutils.h
  gstvalue.h
  math-compat.h
  gst_private.h
)

set(GEN_HEADERS
  gstenumtypes.h
  gstversion.h
  gstconfig.h
)

add_glib_enumtypes_t(SOURCES gstenumtypes
  gstenumtypes.h.template
  gstenumtypes.c.template
  ${HEADERS})
add_gen_headers(SOURCES gst ${GEN_HEADERS})

add_library(gstreamer-${GST_MAJORMINOR} SHARED ${SOURCES})

target_link_libraries(gstreamer-${GST_MAJORMINOR}
  ${GLIB_LIBRARIES}
  ${GMODULE_LIBRARIES}
  ${GOBJECT_LIBRARIES}
  ${GTHREAD_LIBRARIES}
  ${LIBXML_LIBRARIES}
  m
)

install_lib(gstreamer)
install_headers(gst ${HEADERS})
install_gen_headers(gst ${GEN_HEADERS})
