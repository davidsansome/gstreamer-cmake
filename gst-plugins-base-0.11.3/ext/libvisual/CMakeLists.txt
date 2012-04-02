cmake_minimum_required(VERSION 2.6)

check_plugin_deps(base libvisual LIBVISUAL_FOUND)
if(PLUGIN_visual_FOUND)
  include_directories(${CMAKE_CURRENT_SOURCE_DIR})
  include_directories(${CMAKE_CURRENT_BINARY_DIR})
  include_directories(${LIBVISUAL_INCLUDE_DIRS})

  set(SOURCES
    visual.c
  )

  add_library(gstlibvisual MODULE ${SOURCES})

  target_link_libraries(gstlibvisual
    gstaudio-${GST_MAJORMINOR}
    gstbase-${GST_MAJORMINOR}
    ${GLIB_LIBRARIES}
    ${LIBVISUAL_LIBRARIES}
  )

  install_plugin(gstlibvisual)
endif()
