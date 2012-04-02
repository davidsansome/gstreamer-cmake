cmake_minimum_required(VERSION 2.6)

check_plugin_deps(ugly amrnb AMRNB_FOUND)
if(PLUGIN_amrnb_FOUND)
  include_directories(${CMAKE_CURRENT_SOURCE_DIR})
  include_directories(${CMAKE_CURRENT_BINARY_DIR})
  include_directories(${AMRNB_INCLUDE_DIRS})

  set(SOURCES
    amrnb.c
    amrnbdec.c
    amrnbenc.c
  )

  add_library(gstamrnb MODULE ${SOURCES})

  target_link_libraries(gstamrnb
    gstbase-${GST_MAJORMINOR}
    ${GLIB_LIBRARIES}
    ${AMRNB_LIBRARIES}
  )

  install_plugin(gstamrnb)
endif()
