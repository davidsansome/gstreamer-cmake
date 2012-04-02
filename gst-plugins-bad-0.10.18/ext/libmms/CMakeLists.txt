cmake_minimum_required(VERSION 2.6)

check_plugin_deps(bad libmms LIBMMS_FOUND)
if(PLUGIN_libmms_FOUND)
  include_directories(${CMAKE_CURRENT_SOURCE_DIR})
  include_directories(${CMAKE_CURRENT_BINARY_DIR})
  include_directories(${LIBMMS_INCLUDE_DIRS})

  set(SOURCES
    gstmms.c
  )

  add_library(gstlibmms MODULE ${SOURCES})

  target_link_libraries(gstlibmms
    gstbase-${GST_MAJORMINOR}
    ${GLIB_LIBRARIES}
    ${LIBMMS_LIBRARIES}
  )

  install_plugin(gstlibmms)
endif()
