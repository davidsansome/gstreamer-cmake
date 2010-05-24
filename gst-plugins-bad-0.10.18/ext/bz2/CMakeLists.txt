cmake_minimum_required(VERSION 2.6)

check_plugin_deps(bad bz2 BZ2_LIBRARIES BZ2_INCLUDE_DIRS)
if (${PLUGIN_bz2_FOUND})
  include_directories(${CMAKE_CURRENT_SOURCE_DIR})
  include_directories(${CMAKE_CURRENT_BINARY_DIR})
  include_directories(${BZ2_INCLUDE_DIRS})

  set(SOURCES
    gstbz2.c
    gstbz2dec.c
    gstbz2enc.c
  )

  add_library(gstbz2 MODULE ${SOURCES})

  target_link_libraries(gstbz2
    gstbase-${GST_MAJORMINOR}
    ${GLIB_LIBRARIES}
    ${BZ2_LIBRARIES}
  )

  install_plugin(gstbz2)
endif (${PLUGIN_bz2_FOUND})
