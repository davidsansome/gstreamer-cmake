cmake_minimum_required(VERSION 2.6)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(ALAW_SOURCES
  alaw-decode.c
  alaw-encode.c
  alaw.c
)

set(MULAW_SOURCES
  mulaw-conversion.c
  mulaw-decode.c
  mulaw-encode.c
  mulaw.c
)

add_library(gstalaw MODULE ${ALAW_SOURCES})
add_library(gstmulaw MODULE ${MULAW_SOURCES})

target_link_libraries(gstalaw
  gstbase-${GST_MAJORMINOR}
  gstaudio-${GST_MAJORMINOR}
  ${GLIB_LIBRARIES}
)
target_link_libraries(gstmulaw
  gstbase-${GST_MAJORMINOR}
  gstaudio-${GST_MAJORMINOR}
  ${GLIB_LIBRARIES}
)

install_plugin(gstalaw)
install_plugin(gstmulaw)
