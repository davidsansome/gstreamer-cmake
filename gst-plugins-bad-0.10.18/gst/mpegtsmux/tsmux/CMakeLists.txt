cmake_minimum_required(VERSION 2.6)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(SOURCES
  tsmux.c
  tsmuxstream.c
)

add_library(tsmux SHARED ${SOURCES})

target_link_libraries(tsmux
  ${GLIB_LIBRARIES}
)

install(TARGETS tsmux
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
)
