cmake_minimum_required(VERSION 2.6)

check_plugin_deps(bad musicbrainz MUSICBRAINZ_FOUND)
if(PLUGIN_musicbrainz_FOUND)
  include_directories(${CMAKE_CURRENT_SOURCE_DIR})
  include_directories(${CMAKE_CURRENT_BINARY_DIR})
  include_directories(${MUSICBRAINZ_INCLUDE_DIRS})

  set(SOURCES
    gsttrm.c
  )

  add_library(gstmusicbrainz MODULE ${SOURCES})

  target_link_libraries(gstmusicbrainz
    gstbase-${GST_MAJORMINOR}
    gsttag-${GST_MAJORMINOR}
    ${GLIB_LIBRARIES}
    ${MUSICBRAINZ_LIBRARIES}
  )

  install_plugin(gstmusicbrainz)
endif()
