cmake_minimum_required(VERSION 2.6)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(ALPHA_SOURCES
  gstalpha.c
)
set(ALPHACOLOR_SOURCES
  gstalphacolor.c
)

add_library(gstalpha MODULE ${ALPHA_SOURCES})
add_library(gstalphacolor MODULE ${ALPHACOLOR_SOURCES})

target_link_libraries(gstalpha
  gstbase-${GST_MAJORMINOR}
  gstvideo-${GST_MAJORMINOR}
  gstcontroller-${GST_MAJORMINOR}
  ${GLIB_LIBRARIES}
)
target_link_libraries(gstalphacolor
  gstbase-${GST_MAJORMINOR}
  gstvideo-${GST_MAJORMINOR}
  gstcontroller-${GST_MAJORMINOR}
  ${GLIB_LIBRARIES}
)

install_plugin(gstalpha)
install_plugin(gstalphacolor)
