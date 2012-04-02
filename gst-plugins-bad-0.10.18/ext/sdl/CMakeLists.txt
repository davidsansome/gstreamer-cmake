cmake_minimum_required(VERSION 2.6)

check_plugin_deps(bad sdl SDL_FOUND)
if(PLUGIN_sdl_FOUND)
  include_directories(${CMAKE_CURRENT_SOURCE_DIR})
  include_directories(${CMAKE_CURRENT_BINARY_DIR})
  include_directories(${SDL_INCLUDE_DIRS})

  set(SOURCES
    gstsdl.c
    sdlaudiosink.c
    sdlvideosink.c
  )

  add_library(gstsdl MODULE ${SOURCES})

  target_link_libraries(gstsdl
    gstbase-${GST_MAJORMINOR}
    gstvideo-${GST_MAJORMINOR}
    gstaudio-${GST_MAJORMINOR}
    gstinterfaces-${GST_MAJORMINOR}
    ${GLIB_LIBRARIES}
    ${SDL_LIBRARIES}
  )

  install_plugin(gstsdl)
endif()
