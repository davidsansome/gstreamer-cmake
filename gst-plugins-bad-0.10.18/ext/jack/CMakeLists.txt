cmake_minimum_required(VERSION 2.6)

check_plugin_deps(bad jack JACK_FOUND)
if(PLUGIN_jack_FOUND)
  include_directories(${CMAKE_CURRENT_SOURCE_DIR})
  include_directories(${CMAKE_CURRENT_BINARY_DIR})
  include_directories(${JACK_INCLUDE_DIRS})

  set(SOURCES
    gstjack.c
    gstjackaudioclient.c
    gstjackaudiosink.c
    gstjackaudiosrc.c
  )

  add_library(gstjack MODULE ${SOURCES})

  target_link_libraries(gstjack
    gstbase-${GST_MAJORMINOR}
    gstaudio-${GST_MAJORMINOR}
    ${GLIB_LIBRARIES}
    ${JACK_LIBRARIES}
  )

  install_plugin(gstjack)
endif()
