cmake_minimum_required(VERSION 2.6)

check_plugin_deps(ffmpeg postproc)
if(PLUGIN_postproc_FOUND)
  include_directories(${CMAKE_CURRENT_SOURCE_DIR})
  include_directories(${CMAKE_CURRENT_BINARY_DIR})
  include_directories(${ALSA_INCLUDE_DIRS})
  include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../gst-libs/ext/ffmpeg)
  include_directories(BEFORE ${CMAKE_CURRENT_BINARY_DIR}/../..)

  include_directories(${LIBOIL_INCLUDE_DIRS})

  add_definitions(-DHAVE_CONFIG_H)

  set(SOURCES
    gstpostproc.c
  )

  add_library(postproc MODULE ${SOURCES})

  target_link_libraries(postproc
    libavutil
    libpostproc
    ${LIBOIL_LIBRARIES}
    ${GLIB_LIBRARIES}
    ${GOBJECT_LIBRARIES}
    gstbase-${GST_MAJORMINOR}
    gstvideo-${GST_MAJORMINOR}
  )

  install_plugin(postproc)
endif()
