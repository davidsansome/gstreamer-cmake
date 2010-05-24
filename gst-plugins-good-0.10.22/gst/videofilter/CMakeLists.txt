cmake_minimum_required(VERSION 2.6)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(GAMMA_SOURCES
  gstgamma.c
)
set(BALANCE_SOURCES
  gstvideobalance.c
)
set(FLIP_SOURCES
  gstvideoflip.c
)

add_library(gstgamma MODULE ${GAMMA_SOURCES})
add_library(gstvideobalance MODULE ${BALANCE_SOURCES})
add_library(gstvideoflip MODULE ${FLIP_SOURCES})

target_link_libraries(gstgamma
  gstbase-${GST_MAJORMINOR}
  gstvideo-${GST_MAJORMINOR}
  ${GLIB_LIBRARIES}
)
target_link_libraries(gstvideobalance
  gstbase-${GST_MAJORMINOR}
  gstvideo-${GST_MAJORMINOR}
  gstinterfaces-${GST_MAJORMINOR}
  ${GLIB_LIBRARIES}
)
target_link_libraries(gstvideoflip
  gstbase-${GST_MAJORMINOR}
  gstvideo-${GST_MAJORMINOR}
  ${GLIB_LIBRARIES}
)

install_plugin(gstgamma)
install_plugin(gstvideobalance)
install_plugin(gstvideoflip)
