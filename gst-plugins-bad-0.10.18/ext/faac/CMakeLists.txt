cmake_minimum_required(VERSION 2.6)

check_plugin_deps(bad faac FAAC_LIBRARIES FAAC_INCLUDE_DIRS)
if(PLUGIN_faac_FOUND)
  include_directories(${CMAKE_CURRENT_SOURCE_DIR})
  include_directories(${CMAKE_CURRENT_BINARY_DIR})
  include_directories(${FAAC_INCLUDE_DIRS})

  set(SOURCES
    gstfaac.c
  )

  add_library(gstfaac MODULE ${SOURCES})

  target_link_libraries(gstfaac
    gstbase-${GST_MAJORMINOR}
    gstaudio-${GST_MAJORMINOR}
    ${GLIB_LIBRARIES}
    ${FAAC_LIBRARIES}
  )

  install_plugin(gstfaac)
endif()
