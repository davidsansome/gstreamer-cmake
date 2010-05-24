cmake_minimum_required(VERSION 2.6)

check_plugin_deps(bad musepack MUSEPACK_LIBRARIES MUSEPACK_INCLUDE_DIRS)
if (${PLUGIN_musepack_FOUND})
  include_directories(${CMAKE_CURRENT_SOURCE_DIR})
  include_directories(${CMAKE_CURRENT_BINARY_DIR})
  include_directories(${MUSEPACK_INCLUDE_DIRS})

  set(SOURCES
    gstmusepackdec.c
    gstmusepackreader.c
  )

  add_library(gstmusepack MODULE ${SOURCES})

  target_link_libraries(gstmusepack
    gstbase-${GST_MAJORMINOR}
    ${GLIB_LIBRARIES}
    ${MUSEPACK_LIBRARIES}
  )

  install_plugin(gstmusepack)
endif (${PLUGIN_musepack_FOUND})
