cmake_minimum_required(VERSION 2.6)

check_plugin_deps(base cdparanoia CDPARANOIA_LIBRARIES CDPARANOIA_INCLUDE_DIRS)
if(PLUGIN_cdparanoia_FOUND)
  include_directories(${CMAKE_CURRENT_SOURCE_DIR})
  include_directories(${CMAKE_CURRENT_BINARY_DIR})
  include_directories(${CDPARANOIA_INCLUDE_DIRS})

  set(SOURCES
    gstcdparanoiasrc.c
  )

  add_library(gstcdparanoia MODULE ${SOURCES})

  target_link_libraries(gstcdparanoia
    gstaudio-${GST_MAJORMINOR}
    gstbase-${GST_MAJORMINOR}
    ${GLIB_LIBRARIES}
    ${CDPARANOIA_LIBRARIES}
  )

  install_plugin(gstcdparanoia)
endif()
