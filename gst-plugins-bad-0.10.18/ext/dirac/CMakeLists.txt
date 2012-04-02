cmake_minimum_required(VERSION 2.6)

check_plugin_deps(bad dirac DIRAC_FOUND)
if(PLUGIN_dirac_FOUND)
  include_directories(${CMAKE_CURRENT_SOURCE_DIR})
  include_directories(${CMAKE_CURRENT_BINARY_DIR})
  include_directories(${DIRAC_INCLUDE_DIRS})

  add_definitions(-DGST_USE_UNSTABLE_API)

  set(SOURCES
    gstdirac.cc
    gstdiracenc.cc
  )

  add_library(gstdirac MODULE ${SOURCES})

  target_link_libraries(gstdirac
    gstbase-${GST_MAJORMINOR}
    gstbasevideo-${GST_MAJORMINOR}
    ${GLIB_LIBRARIES}
    ${DIRAC_LIBRARIES}
  )

  install_plugin(gstdirac)
endif()
