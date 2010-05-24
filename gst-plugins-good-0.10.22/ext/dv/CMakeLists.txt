cmake_minimum_required(VERSION 2.6)

check_plugin_deps(good dv LIBDV_FOUND)
if (${PLUGIN_dv_FOUND})
  include_directories(${CMAKE_CURRENT_SOURCE_DIR})
  include_directories(${CMAKE_CURRENT_BINARY_DIR})
  include_directories(${LIBDV_INCLUDE_DIRS})

  set(SOURCES
    gstdv.c
    gstdvdec.c
    gstdvdemux.c
    gstsmptetimecode.c
    smpte_test.c
  )

  add_library(gstdv MODULE ${SOURCES})

  target_link_libraries(gstdv
    gstbase-${GST_MAJORMINOR}
    gstaudio-${GST_MAJORMINOR}
    ${GLIB_LIBRARIES}
    ${LIBDV_LIBRARIES}
  )

  install_plugin(gstdv)
endif (${PLUGIN_dv_FOUND})
