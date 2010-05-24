cmake_minimum_required(VERSION 2.6)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${LIBOIL_INCLUDE_DIRS})

set(SOURCES
  config_param.c
  convolve_fx.c
  drawmethods.c
  filters.c
  flying_stars_fx.c
  goom_core.c
  goom_tools.c
  graphic.c
  gstgoom.c
  ifs.c
  lines.c
  mathtools.c
  mmx.c
  plugin_info.c
  sound_tester.c
  surf3d.c
  tentacle3d.c
  v3d.c
  xmmx.c
)

add_library(gstgoom MODULE ${SOURCES})

target_link_libraries(gstgoom
  gstbase-${GST_MAJORMINOR}
  ${GLIB_LIBRARIES}
  ${LIBOIL_LIBRARIES}
)

install_plugin(gstgoom)
