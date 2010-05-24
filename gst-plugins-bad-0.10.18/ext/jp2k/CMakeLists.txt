cmake_minimum_required(VERSION 2.6)

check_plugin_deps(bad jp2k JASPER_LIBRARIES JASPER_INCLUDE_DIRS)
if (${PLUGIN_jp2k_FOUND})
  include_directories(${CMAKE_CURRENT_SOURCE_DIR})
  include_directories(${CMAKE_CURRENT_BINARY_DIR})
  include_directories(${JASPER_INCLUDE_DIRS})

  set(SOURCES
    gstjasperdec.c
    gstjasperenc.c
    gstjp2k.c
  )

  add_library(gstjp2k MODULE ${SOURCES})

  target_link_libraries(gstjp2k
    gstbase-${GST_MAJORMINOR}
    gstvideo-${GST_MAJORMINOR}
    ${GLIB_LIBRARIES}
    ${JASPER_LIBRARIES}
  )

  install_plugin(gstjp2k)
endif (${PLUGIN_jp2k_FOUND})
