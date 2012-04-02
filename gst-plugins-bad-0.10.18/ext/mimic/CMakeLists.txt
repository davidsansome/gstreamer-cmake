cmake_minimum_required(VERSION 2.6)

check_plugin_deps(bad mimic LIBMIMIC_FOUND)
if(PLUGIN_mimic_FOUND)
  include_directories(${CMAKE_CURRENT_SOURCE_DIR})
  include_directories(${CMAKE_CURRENT_BINARY_DIR})
  include_directories(${LIBMIMIC_INCLUDE_DIRS})

  set(SOURCES
    gstmimdec.c
    gstmimenc.c
    gstmimic.c
  )

  add_library(gstmimic MODULE ${SOURCES})

  target_link_libraries(gstmimic
    gstbase-${GST_MAJORMINOR}
    ${GLIB_LIBRARIES}
    ${LIBMIMIC_LIBRARIES}
  )

  install_plugin(gstmimic)
endif()
