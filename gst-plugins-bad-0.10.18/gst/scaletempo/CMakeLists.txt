cmake_minimum_required(VERSION 2.6)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(SOURCES
  gstscaletempo.c
  gstscaletempoplugin.c
)

add_library(gstscaletempo MODULE ${SOURCES})

target_link_libraries(gstscaletempo
  gstbase-${GST_MAJORMINOR}
  ${GLIB_LIBRARIES}
)

install_plugin(gstscaletempo)
