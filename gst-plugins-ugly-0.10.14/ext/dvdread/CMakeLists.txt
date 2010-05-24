cmake_minimum_required(VERSION 2.6)

check_plugin_deps(ugly dvdread DVDREAD_FOUND)
if (${PLUGIN_dvdread_FOUND})
  include_directories(${CMAKE_CURRENT_SOURCE_DIR})
  include_directories(${CMAKE_CURRENT_BINARY_DIR})
  include_directories(${DVDREAD_INCLUDE_DIRS})

  set(SOURCES
    dvdreadsrc.c
  )

  add_library(gstdvdread MODULE ${SOURCES})

  target_link_libraries(gstdvdread
    gstbase-${GST_MAJORMINOR}
    ${GLIB_LIBRARIES}
    ${DVDREAD_LIBRARIES}
  )

  install_plugin(gstdvdread)
endif (${PLUGIN_dvdread_FOUND})
