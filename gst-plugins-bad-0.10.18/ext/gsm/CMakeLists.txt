cmake_minimum_required(VERSION 2.6)

check_plugin_deps(bad gsm GSM_LIBRARIES GSM_INCLUDE_DIRS)
if(PLUGIN_gsm_FOUND)
  include_directories(${CMAKE_CURRENT_SOURCE_DIR})
  include_directories(${CMAKE_CURRENT_BINARY_DIR})
  include_directories(${GSM_INCLUDE_DIRS})

  set(SOURCES
    gstgsm.c
    gstgsmdec.c
    gstgsmenc.c
  )

  add_library(gstgsm MODULE ${SOURCES})

  target_link_libraries(gstgsm
    gstbase-${GST_MAJORMINOR}
    gstaudio-${GST_MAJORMINOR}
    ${GLIB_LIBRARIES}
    ${GSM_LIBRARIES}
  )

  install_plugin(gstgsm)
endif()
