cmake_minimum_required(VERSION 2.6)

check_plugin_deps(ugly amrwbdec AMRWB_FOUND)
if (${PLUGIN_amrwbdec_FOUND})
  include_directories(${CMAKE_CURRENT_SOURCE_DIR})
  include_directories(${CMAKE_CURRENT_BINARY_DIR})
  include_directories(${AMRWB_INCLUDE_DIRS})

  set(SOURCES
    amrwb.c
    amrwbdec.c
  )

  add_library(gstamrwbdec MODULE ${SOURCES})

  target_link_libraries(gstamrwbdec
    gstbase-${GST_MAJORMINOR}
    ${GLIB_LIBRARIES}
    ${AMRWB_LIBRARIES}
  )

  install_plugin(gstamrwbdec)
endif (${PLUGIN_amrwbdec_FOUND})
