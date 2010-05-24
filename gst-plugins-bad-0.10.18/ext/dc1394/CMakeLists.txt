cmake_minimum_required(VERSION 2.6)

check_plugin_deps(bad dc1394 LIBDC1394_FOUND)
if (${PLUGIN_dc1394_FOUND})
  include_directories(${CMAKE_CURRENT_SOURCE_DIR})
  include_directories(${CMAKE_CURRENT_BINARY_DIR})
  include_directories(${LIBDC1394_INCLUDE_DIRS})

  set(SOURCES
    gstdc1394.c
  )

  add_library(gstdc1394 MODULE ${SOURCES})

  target_link_libraries(gstdc1394
    gstbase-${GST_MAJORMINOR}
    ${GLIB_LIBRARIES}
    ${LIBDC1394_LIBRARIES}
  )

  install_plugin(gstdc1394)
endif (${PLUGIN_dc1394_FOUND})
