cmake_minimum_required(VERSION 2.6)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${LIBOIL_INCLUDE_DIRS})

set(SOURCES
  gstvideotestsrc.c
  videotestsrc.c
)

add_orc(SOURCES gstvideotestsrcorc)
add_library(gstvideotestsrc MODULE ${SOURCES})

target_link_libraries(gstvideotestsrc
  gstbase-${GST_MAJORMINOR}
  gstvideo-${GST_MAJORMINOR}
  ${GLIB_LIBRARIES}
  ${ORC_LIBRARIES}
)

install_plugin(gstvideotestsrc)
