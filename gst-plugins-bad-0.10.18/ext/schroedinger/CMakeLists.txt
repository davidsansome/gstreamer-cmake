cmake_minimum_required(VERSION 2.6)

check_plugin_deps(bad schroedinger SCHRO_FOUND)
if (${PLUGIN_schroedinger_FOUND})
  include_directories(${CMAKE_CURRENT_SOURCE_DIR})
  include_directories(${CMAKE_CURRENT_BINARY_DIR})
  include_directories(${SCHRO_INCLUDE_DIRS})

  add_definitions(-DGST_USE_UNSTABLE_API)

  set(SOURCES
    gstschro.c
    gstschrodec.c
    gstschroenc.c
    gstschroparse.c
    gstschroutils.c
  )

  add_library(gstschroedinger MODULE ${SOURCES})

  target_link_libraries(gstschroedinger
    gstbase-${GST_MAJORMINOR}
    gstbasevideo-${GST_MAJORMINOR}
    gstvideo-${GST_MAJORMINOR}
    ${GLIB_LIBRARIES}
    ${SCHRO_LIBRARIES}
  )

  install_plugin(gstschroedinger)
endif (${PLUGIN_schroedinger_FOUND})
