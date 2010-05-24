cmake_minimum_required(VERSION 2.6)

check_plugin_deps(good 1394 RAW1394_FOUND LIBIEC61883_FOUND)
if (${PLUGIN_1394_FOUND})
  include_directories(${CMAKE_CURRENT_SOURCE_DIR})
  include_directories(${CMAKE_CURRENT_BINARY_DIR})
  include_directories(${RAW1394_INCLUDE_DIRS})
  include_directories(${LIBIEC61883_INCLUDE_DIRS})

  add_definitions(-DHAVE_LIBIEC61883)

  set(SOURCES
    gst1394.c
    gst1394clock.c
    gst1394probe.c
    gstdv1394src.c
    gsthdv1394src.c
  )

  add_library(gst1394 MODULE ${SOURCES})

  target_link_libraries(gst1394
    gstbase-${GST_MAJORMINOR}
    gstinterfaces-${GST_MAJORMINOR}
    ${GLIB_LIBRARIES}
    ${RAW1394_LIBRARIES}
    ${LIBIEC61883_LIBRARIES}
  )

  install_plugin(gst1394)
endif (${PLUGIN_1394_FOUND})
