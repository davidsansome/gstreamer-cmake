cmake_minimum_required(VERSION 2.6)

check_plugin_deps(bad cdaudio LIBCDAUDIO_FOUND)
if(PLUGIN_cdaudio_FOUND)
  include_directories(${CMAKE_CURRENT_SOURCE_DIR})
  include_directories(${CMAKE_CURRENT_BINARY_DIR})
  include_directories(${LIBCDAUDIO_INCLUDE_DIRS})

  set(SOURCES
    gstcdaudio.c
  )

  add_library(gstcdaudio MODULE ${SOURCES})

  target_link_libraries(gstcdaudio
    gstbase-${GST_MAJORMINOR}
    ${GLIB_LIBRARIES}
    ${LIBCDAUDIO_LIBRARIES}
  )

  install_plugin(gstcdaudio)
endif()
