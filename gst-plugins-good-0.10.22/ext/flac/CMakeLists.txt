cmake_minimum_required(VERSION 2.6)

check_plugin_deps(flac FLAC_FOUND)
if (${PLUGIN_flac_FOUND})
  include_directories(${CMAKE_CURRENT_SOURCE_DIR})
  include_directories(${CMAKE_CURRENT_BINARY_DIR})
  include_directories(${FLAC_INCLUDE_DIRS})

  set(SOURCES
    gstflac.c
    gstflacdec.c
    gstflacenc.c
    gstflactag.c
  )

  add_library(gstflac MODULE ${SOURCES})

  target_link_libraries(gstflac
    gstbase-${GST_MAJORMINOR}
    gstaudio-${GST_MAJORMINOR}
    gsttag-${GST_MAJORMINOR}
    ${GLIB_LIBRARIES}
    ${FLAC_LIBRARIES}
  )

  install_plugin(gstflac)
endif (${PLUGIN_flac_FOUND})
