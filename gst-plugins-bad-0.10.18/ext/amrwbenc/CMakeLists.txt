cmake_minimum_required(VERSION 2.6)

check_plugin_deps(bad amrwbenc AMRWBENC_LIBRARIES AMRWBENC_INCLUDE_DIRS)
if (${PLUGIN_amrwbenc_FOUND})
  include_directories(${CMAKE_CURRENT_SOURCE_DIR})
  include_directories(${CMAKE_CURRENT_BINARY_DIR})
  include_directories(${AMRWBENC_INCLUDE_DIRS})

  set(SOURCES
    gstamrwb.c
    gstamrwbenc.c
  )

  add_library(gstamrwbenc MODULE ${SOURCES})

  target_link_libraries(gstamrwbenc
    gstbase-${GST_MAJORMINOR}
    ${GLIB_LIBRARIES}
    ${AMRWBENC_LIBRARIES}
  )

  install_plugin(gstamrwbenc)
endif (${PLUGIN_amrwbenc_FOUND})
