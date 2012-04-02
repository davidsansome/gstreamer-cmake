cmake_minimum_required(VERSION 2.6)

check_plugin_deps(bad kate KATE_FOUND TIGER_LIBRARIES TIGER_INCLUDE_DIRS)
if(PLUGIN_kate_FOUND)
  include_directories(${CMAKE_CURRENT_SOURCE_DIR})
  include_directories(${CMAKE_CURRENT_BINARY_DIR})
  include_directories(${KATE_INCLUDE_DIRS})
  include_directories(${TIGER_INCLUDE_DIRS})

  set(SOURCES
    gstkate.c
    gstkatedec.c
    gstkateenc.c
    gstkateparse.c
    gstkatespu.c
    gstkatetag.c
    gstkatetiger.c
    gstkateutil.c
  )

  add_library(gstkate MODULE ${SOURCES})

  target_link_libraries(gstkate
    gstbase-${GST_MAJORMINOR}
    gsttag-${GST_MAJORMINOR}
    ${GLIB_LIBRARIES}
    ${KATE_LIBRARIES}
    ${TIGER_LIBRARIES}
  )

  install_plugin(gstkate)
endif()
