cmake_minimum_required(VERSION 2.6)

check_plugin_deps(bad lv2 LV2_FOUND)
if(PLUGIN_lv2_FOUND)
  include_directories(${CMAKE_CURRENT_SOURCE_DIR})
  include_directories(${CMAKE_CURRENT_BINARY_DIR})
  include_directories(${LV2_INCLUDE_DIRS})

  set(SOURCES
    gstlv2.c
  )

  add_library(gstlv2 MODULE ${SOURCES})

  target_link_libraries(gstlv2
    gstbase-${GST_MAJORMINOR}
    gstsignalprocessor-${GST_MAJORMINOR}
    ${GLIB_LIBRARIES}
    ${LV2_LIBRARIES}
  )

  install_plugin(gstlv2)
endif()
