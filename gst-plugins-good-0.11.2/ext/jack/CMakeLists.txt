cmake_minimum_required(VERSION 2.6)

check_plugin_deps(good jack JACK_LIBRARIES JACK_INCLUDE_DIRS)
if(PLUGIN_jack_FOUND)
  include_directories(${CMAKE_CURRENT_SOURCE_DIR})
  include_directories(${CMAKE_CURRENT_BINARY_DIR})
  include_directories(${JACK_INCLUDE_DIRS})

  set(SOURCES
    gstjackutil.c
    gstjackaudiosrc.c
    gstjackaudioclient.c
    gstjack.c
    gstjackaudiosink.c
  )

  add_library(gstjack MODULE ${SOURCES})

  target_link_libraries(gstjack
    ${GLIB_LIBRARIES}
    ${JACK_LIBRARIES}
  )

  install_plugin(gstjack)
endif()
