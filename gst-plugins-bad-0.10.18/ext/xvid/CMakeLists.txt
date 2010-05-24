cmake_minimum_required(VERSION 2.6)

check_plugin_deps(bad xvid XVID_LIBRARIES XVID_INCLUDE_DIRS)
if (${PLUGIN_xvid_FOUND})
  include_directories(${CMAKE_CURRENT_SOURCE_DIR})
  include_directories(${CMAKE_CURRENT_BINARY_DIR})
  include_directories(${XVID_INCLUDE_DIRS})

  set(SOURCES
    gstxvid.c
    gstxviddec.c
    gstxvidenc.c
  )

  add_library(gstxvid MODULE ${SOURCES})

  target_link_libraries(gstxvid
    gstbase-${GST_MAJORMINOR}
    ${GLIB_LIBRARIES}
    ${XVID_LIBRARIES}
  )

  install_plugin(gstxvid)
endif (${PLUGIN_xvid_FOUND})
