cmake_minimum_required(VERSION 2.6)

check_plugin_deps(ugly x264 X264_FOUND)
if(PLUGIN_x264_FOUND)
  include_directories(${CMAKE_CURRENT_SOURCE_DIR})
  include_directories(${CMAKE_CURRENT_BINARY_DIR})
  include_directories(${X264_INCLUDE_DIRS})

  set(SOURCES
    gstx264enc.c
  )

  add_library(gstx264 MODULE ${SOURCES})

  target_link_libraries(gstx264
    gstbase-${GST_MAJORMINOR}
    gstvideo-${GST_MAJORMINOR}
    ${GLIB_LIBRARIES}
    ${X264_LIBRARIES}
  )

  install_plugin(gstx264)
endif()
