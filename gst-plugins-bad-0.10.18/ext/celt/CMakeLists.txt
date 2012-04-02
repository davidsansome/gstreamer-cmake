cmake_minimum_required(VERSION 2.6)

check_plugin_deps(bad celt CELT_FOUND)
if(PLUGIN_celt_FOUND)
  include_directories(${CMAKE_CURRENT_SOURCE_DIR})
  include_directories(${CMAKE_CURRENT_BINARY_DIR})
  include_directories(${CELT_INCLUDE_DIRS})

  set(SOURCES
    gstcelt.c
    gstceltdec.c
    gstceltenc.c
  )

  add_library(gstcelt MODULE ${SOURCES})

  target_link_libraries(gstcelt
    gstbase-${GST_MAJORMINOR}
    gsttag-${GST_MAJORMINOR}
    ${GLIB_LIBRARIES}
    ${CELT_LIBRARIES}
  )

  install_plugin(gstcelt)
endif()
