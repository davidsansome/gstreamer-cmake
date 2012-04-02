cmake_minimum_required(VERSION 2.6)

check_plugin_deps(bad timidity TIMIDITY_FOUND)
if(PLUGIN_timidity_FOUND)
  include_directories(${CMAKE_CURRENT_SOURCE_DIR})
  include_directories(${CMAKE_CURRENT_BINARY_DIR})
  include_directories(${TIMIDITY_INCLUDE_DIRS})

  set(SOURCES
    gsttimidity.c
  )

  add_library(gsttimidity MODULE ${SOURCES})

  target_link_libraries(gsttimidity
    gstbase-${GST_MAJORMINOR}
    ${GLIB_LIBRARIES}
    ${TIMIDITY_LIBRARIES}
  )

  install_plugin(gsttimidity)
endif()


check_plugin_deps(bad wildmidi WILDMIDI_LIBRARIES WILDMIDI_INCLUDE_DIRS)
if(PLUGIN_wildmidi_FOUND)
  include_directories(${CMAKE_CURRENT_SOURCE_DIR})
  include_directories(${CMAKE_CURRENT_BINARY_DIR})
  include_directories(${WILDMIDI_INCLUDE_DIRS})

  set(SOURCES
    gstwildmidi.c
  )

  add_library(gstwildmidi MODULE ${SOURCES})

  target_link_libraries(gstwildmidi
    gstbase-${GST_MAJORMINOR}
    ${GLIB_LIBRARIES}
    ${WILDMIDI_LIBRARIES}
  )

  install_plugin(gstwildmidi)
endif()
