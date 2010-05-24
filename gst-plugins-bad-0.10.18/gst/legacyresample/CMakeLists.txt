cmake_minimum_required(VERSION 2.6)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${LIBOIL_INCLUDE_DIRS})

set(SOURCES
  buffer.c
  functable.c
  gstlegacyresample.c
  resample.c
  resample_chunk.c
  resample_functable.c
  resample_ref.c
)

add_library(gstlegacyresample MODULE ${SOURCES})

target_link_libraries(gstlegacyresample
  gstbase-${GST_MAJORMINOR}
  ${GLIB_LIBRARIES}
  ${LIBOIL_LIBRARIES}
)

install_plugin(gstlegacyresample)
