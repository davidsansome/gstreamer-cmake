cmake_minimum_required(VERSION 2.6)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(SOURCES
  gst-plugin-scanner.c
)

add_executable(gst-plugin-scanner ${SOURCES})

target_link_libraries(gst-plugin-scanner
  gstreamer-${GST_MAJORMINOR}
  ${GLIB_LIBRARIES}
  ${GOBJECT_LIBRARIES}
  ${GMODULE_LIBRARIES}
)

install(TARGETS gst-plugin-scanner
  RUNTIME DESTINATION lib/gstreamer${GST_MAJORMINOR}
)
