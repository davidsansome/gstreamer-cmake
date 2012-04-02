cmake_minimum_required(VERSION 2.6)

check_plugin_deps(bad resindvd DVDNAV_FOUND)
if(PLUGIN_resindvd_FOUND)
  include_directories(${CMAKE_CURRENT_SOURCE_DIR})
  include_directories(${CMAKE_CURRENT_BINARY_DIR})
  include_directories(${DVDNAV_INCLUDE_DIRS})

  set(SOURCES
    gstmpegdemux.c
    gstmpegdesc.c
    gstpesfilter.c
    plugin.c
    resindvdbin.c
    resindvdsrc.c
    rsnaudiomunge.c
    rsndec.c
    rsnparsetter.c
    rsnstreamselector.c
    rsnwrappedbuffer.c
  )

  add_library(gstresindvd MODULE ${SOURCES})

  target_link_libraries(gstresindvd
    gstbase-${GST_MAJORMINOR}
    gstvideo-${GST_MAJORMINOR}
    gstinterfaces-${GST_MAJORMINOR}
    gstpbutils-${GST_MAJORMINOR}
    ${GLIB_LIBRARIES}
    ${DVDNAV_LIBRARIES}
  )

  install_plugin(gstresindvd)
endif()
