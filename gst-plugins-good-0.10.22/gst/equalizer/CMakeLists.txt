cmake_minimum_required(VERSION 2.6)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(SOURCES
  gstiirequalizer.c
  gstiirequalizer10bands.c
  gstiirequalizer3bands.c
  gstiirequalizernbands.c
)

add_library(gstequalizer MODULE ${SOURCES})

target_link_libraries(gstequalizer
  gstbase-${GST_MAJORMINOR}
  gstaudio-${GST_MAJORMINOR}
  gstcontroller-${GST_MAJORMINOR}
  ${GLIB_LIBRARIES}
)

install_plugin(gstequalizer)
