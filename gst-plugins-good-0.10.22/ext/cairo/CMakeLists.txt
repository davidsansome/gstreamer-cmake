cmake_minimum_required(VERSION 2.6)

check_plugin_deps(good cairo CAIRO_FOUND)
if(PLUGIN_cairo_FOUND)
  include_directories(${CMAKE_CURRENT_SOURCE_DIR})
  include_directories(${CMAKE_CURRENT_BINARY_DIR})
  include_directories(${CAIRO_INCLUDE_DIRS})

  set(SOURCES
    gstcairo.c
    gstcairorender.c
    gsttextoverlay.c
    gsttimeoverlay.c
  )

  add_library(gstcairo MODULE ${SOURCES})

  target_link_libraries(gstcairo
    gstbase-${GST_MAJORMINOR}
    ${GLIB_LIBRARIES}
    ${CAIRO_LIBRARIES}
  )

  install_plugin(gstcairo)
endif()
