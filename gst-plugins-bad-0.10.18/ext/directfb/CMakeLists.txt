cmake_minimum_required(VERSION 2.6)

check_plugin_deps(bad directfb DIRECTFB_FOUND)
if (${PLUGIN_directfb_FOUND})
  include_directories(${CMAKE_CURRENT_SOURCE_DIR})
  include_directories(${CMAKE_CURRENT_BINARY_DIR})
  include_directories(${DIRECTFB_INCLUDE_DIRS})

  set(SOURCES
    dfb-example.c
    dfbvideosink.c
  )

  add_library(gstdirectfb MODULE ${SOURCES})

  target_link_libraries(gstdirectfb
    gstbase-${GST_MAJORMINOR}
    gstvideo-${GST_MAJORMINOR}
    gstinterfaces-${GST_MAJORMINOR}
    ${GLIB_LIBRARIES}
    ${DIRECTFB_LIBRARIES}
  )

  install_plugin(gstdirectfb)
endif (${PLUGIN_directfb_FOUND})
