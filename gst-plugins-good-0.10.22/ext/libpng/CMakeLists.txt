cmake_minimum_required(VERSION 2.6)

check_plugin_deps(good png PNG_FOUND)
if(PLUGIN_png_FOUND)
  include_directories(${CMAKE_CURRENT_SOURCE_DIR})
  include_directories(${CMAKE_CURRENT_BINARY_DIR})
  include_directories(${PNG_INCLUDE_DIRS})

  set(SOURCES
    gstpng.c
    gstpngdec.c
    gstpngenc.c
  )

  add_library(gstpng MODULE ${SOURCES})

  target_link_libraries(gstpng
    gstbase-${GST_MAJORMINOR}
    ${GLIB_LIBRARIES}
    ${PNG_LIBRARIES}
  )

  install_plugin(gstpng)
endif()
