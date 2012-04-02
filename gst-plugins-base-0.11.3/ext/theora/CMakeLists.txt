cmake_minimum_required(VERSION 2.6)

check_plugin_deps(base theora THEORADEC_FOUND THEORAENC_FOUND)
if(PLUGIN_theora_FOUND)
  include_directories(${CMAKE_CURRENT_SOURCE_DIR})
  include_directories(${CMAKE_CURRENT_BINARY_DIR})
  include_directories(${THEORADEC_INCLUDE_DIRS})
  include_directories(${THEORAENC_INCLUDE_DIRS})

  set(SOURCES
    gsttheoraparse.c
    gsttheoraenc.c
    gsttheora.c
    gsttheoradec.c
  )

  add_library(gsttheora MODULE ${SOURCES})

  target_link_libraries(gsttheora
    gstvideo-${GST_MAJORMINOR}
    gsttag-${GST_MAJORMINOR}
    gstbase-${GST_MAJORMINOR}
    ${GLIB_LIBRARIES}
    ${THEORADEC_LIBRARIES}
    ${THEORAENC_LIBRARIES}
  )

  install_plugin(gsttheora)
endif()
