cmake_minimum_required(VERSION 2.6)

check_plugin_deps(base ogg OGG_FOUND)
if(PLUGIN_ogg_FOUND)
  include_directories(${CMAKE_CURRENT_SOURCE_DIR})
  include_directories(${CMAKE_CURRENT_BINARY_DIR})
  include_directories(${OGG_INCLUDE_DIRS})

  set(SOURCES
    gstoggmux.c
    gstoggstream.c
    gstoggparse.c
    gstoggaviparse.c
    gstogg.c
    vorbis_parse.c
    gstoggdemux.c
    dirac_parse.c
    gstogmparse.c
  )

  add_library(gstogg MODULE ${SOURCES})

  target_link_libraries(gstogg
    gstbase-${GST_MAJORMINOR}
    gsttag-${GST_MAJORMINOR}
    gstriff-${GST_MAJORMINOR}
    ${GLIB_LIBRARIES}
    ${OGG_LIBRARIES}
  )

  install_plugin(gstogg)
endif()
