cmake_minimum_required(VERSION 2.6)

check_plugin_deps(bad alsaspdif ALSA_FOUND)
if (${PLUGIN_alsaspdif_FOUND})
  include_directories(${CMAKE_CURRENT_SOURCE_DIR})
  include_directories(${CMAKE_CURRENT_BINARY_DIR})
  include_directories(${ALSA_INCLUDE_DIRS})

  set(SOURCES
    alsaspdifsink.c
  )

  add_library(gstalsaspdif MODULE ${SOURCES})

  target_link_libraries(gstalsaspdif
    gstbase-${GST_MAJORMINOR}
    gstaudio-${GST_MAJORMINOR}
    ${GLIB_LIBRARIES}
    ${ALSA_LIBRARIES}
  )

  install_plugin(gstalsaspdif)
endif (${PLUGIN_alsaspdif_FOUND})
