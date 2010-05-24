cmake_minimum_required(VERSION 2.6)

check_plugin_deps(bad dts DTS_FOUND)
if (${PLUGIN_dts_FOUND})
  include_directories(${CMAKE_CURRENT_SOURCE_DIR})
  include_directories(${CMAKE_CURRENT_BINARY_DIR})
  include_directories(${DTS_INCLUDE_DIRS})
  include_directories(${LIBOIL_INCLUDE_DIRS})

  set(SOURCES
    gstdtsdec.c
  )

  add_library(gstdts MODULE ${SOURCES})

  target_link_libraries(gstdts
    gstbase-${GST_MAJORMINOR}
    gstaudio-${GST_MAJORMINOR}
    ${GLIB_LIBRARIES}
    ${DTS_LIBRARIES}
    ${LIBOIL_LIBRARIES}
  )

  install_plugin(gstdts)
endif (${PLUGIN_dts_FOUND})
