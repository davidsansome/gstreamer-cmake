cmake_minimum_required(VERSION 2.6)

check_plugin_deps(base pango PANGO_FOUND PANGOCAIRO_FOUND)
if(PLUGIN_pango_FOUND)
  include_directories(${CMAKE_CURRENT_SOURCE_DIR})
  include_directories(${CMAKE_CURRENT_BINARY_DIR})
  include_directories(${PANGO_INCLUDE_DIRS})
  include_directories(${PANGOCAIRO_INCLUDE_DIRS})

  set(SOURCES
    gstclockoverlay.c
    gsttimeoverlay.c
    gsttextoverlay.c
    gstbasetextoverlay.c
    gsttextrender.c
  )

  add_library(gstpango MODULE ${SOURCES})

  target_link_libraries(gstpango
    gstvideo-${GST_MAJORMINOR}
    gstbase-${GST_MAJORMINOR}
    ${GLIB_LIBRARIES}
    ${PANGO_LIBRARIES}
    ${PANGOCAIRO_LIBRARIES}
  )

  install_plugin(gstpango)
endif()
