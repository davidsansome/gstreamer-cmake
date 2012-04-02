cmake_minimum_required(VERSION 2.6)

check_plugin_deps(base vorbis VORBIS_FOUND VORBISENC_FOUND)
if(PLUGIN_vorbis_FOUND)
  include_directories(${CMAKE_CURRENT_SOURCE_DIR})
  include_directories(${CMAKE_CURRENT_BINARY_DIR})
  include_directories(${VORBIS_INCLUDE_DIRS})
  include_directories(${VORBISENC_INCLUDE_DIRS})

  set(SOURCES
    gstvorbis.c
    gstvorbiscommon.c
    gstvorbisdec.c
    gstvorbisdeclib.c
    gstvorbisenc.c
    gstvorbisparse.c
    gstvorbistag.c
  )

  add_library(gstvorbis MODULE ${SOURCES})

  target_link_libraries(gstvorbis
    gstaudio-${GST_MAJORMINOR}
    gsttag-${GST_MAJORMINOR}
    ${GLIB_LIBRARIES}
    ${VORBIS_LIBRARIES}
    ${VORBISENC_LIBRARIES}
  )

  install_plugin(gstvorbis)
endif()
