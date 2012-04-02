cmake_minimum_required(VERSION 2.6)

check_plugin_deps(bad divx
  DIVX_ENCORE_LIBRARIES
  DIVX_ENCORE_INCLUDE_DIRS
  DIVX_DECORE_LIBRARIES
  DIVX_DECORE_INCLUDE_DIRS
)
if(PLUGIN_divx_FOUND)
  include_directories(${CMAKE_CURRENT_SOURCE_DIR})
  include_directories(${CMAKE_CURRENT_BINARY_DIR})
  include_directories(${DIVX_ENCORE_INCLUDE_DIRS})
  include_directories(${DIVX_DECORE_INCLUDE_DIRS})

  set(ENC_SOURCES
    gstdivxenc.c
  )
  set(DEC_SOURCES
    gstdivxdec.c
  )

  add_library(gstdivxenc MODULE ${ENC_SOURCES})
  add_library(gstdivxdec MODULE ${DEC_SOURCES})

  target_link_libraries(gstdivxenc
    gstbase-${GST_MAJORMINOR}
    ${GLIB_LIBRARIES}
    ${DIVX_ENCORE_LIBRARIES}
  )
  target_link_libraries(gstdivxdec
    gstbase-${GST_MAJORMINOR}
    ${GLIB_LIBRARIES}
    ${DIVX_DECORE_LIBRARIES}
  )

  install_plugin(gstdivxenc)
  install_plugin(gstdivxdec)
endif()
