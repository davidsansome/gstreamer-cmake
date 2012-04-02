cmake_minimum_required(VERSION 2.6)

check_plugin_deps(bad neonhttpsrc NEON_FOUND)
if(PLUGIN_neonhttpsrc_FOUND)
  include_directories(${CMAKE_CURRENT_SOURCE_DIR})
  include_directories(${CMAKE_CURRENT_BINARY_DIR})
  include_directories(${NEON_INCLUDE_DIRS})

  set(SOURCES
    gstneonhttpsrc.c
  )

  add_library(gstneonhttpsrc MODULE ${SOURCES})

  target_link_libraries(gstneonhttpsrc
    gstbase-${GST_MAJORMINOR}
    ${GLIB_LIBRARIES}
    ${NEON_LIBRARIES}
  )

  install_plugin(gstneonhttpsrc)
endif()
