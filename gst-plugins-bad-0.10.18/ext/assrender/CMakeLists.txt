cmake_minimum_required(VERSION 2.6)

check_plugin_deps(bad assrender LIBASS_FOUND)
if(PLUGIN_assrender_FOUND)
  include_directories(${CMAKE_CURRENT_SOURCE_DIR})
  include_directories(${CMAKE_CURRENT_BINARY_DIR})
  include_directories(${LIBASS_INCLUDE_DIRS})

  set(SOURCES
    gstassrender.c
  )

  add_library(gstassrender MODULE ${SOURCES})

  target_link_libraries(gstassrender
    gstbase-${GST_MAJORMINOR}
    gstvideo-${GST_MAJORMINOR}
    ${GLIB_LIBRARIES}
    ${LIBASS_LIBRARIES}
  )

  install_plugin(gstassrender)
endif()
