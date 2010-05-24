cmake_minimum_required(VERSION 2.6)

check_plugin_deps(wavpack WAVPACK_FOUND)
if (${PLUGIN_wavpack_FOUND})
  include_directories(${CMAKE_CURRENT_SOURCE_DIR})
  include_directories(${CMAKE_CURRENT_BINARY_DIR})
  include_directories(${WAVPACK_INCLUDE_DIRS})

  set(SOURCES
    gstwavpack.c
    gstwavpackcommon.c
    gstwavpackdec.c
    gstwavpackenc.c
    gstwavpackparse.c
    gstwavpackstreamreader.c
  )

  add_library(gstwavpack MODULE ${SOURCES})

  target_link_libraries(gstwavpack
    gstbase-${GST_MAJORMINOR}
    gstaudio-${GST_MAJORMINOR}
    ${GLIB_LIBRARIES}
    ${WAVPACK_LIBRARIES}
  )

  install_plugin(gstwavpack)
endif (${PLUGIN_wavpack_FOUND})
