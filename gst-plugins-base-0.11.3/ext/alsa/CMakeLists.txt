cmake_minimum_required(VERSION 2.6)

check_plugin_deps(base alsa ALSA_FOUND)
if(PLUGIN_alsa_FOUND)
  include_directories(${CMAKE_CURRENT_SOURCE_DIR})
  include_directories(${CMAKE_CURRENT_BINARY_DIR})
  include_directories(${ALSA_INCLUDE_DIRS})

  set(SOURCES
    gstalsamixertrack.c
    gstalsasink.c
    gstalsa.c
    gstalsamixer.c
    gstalsaplugin.c
    gstalsamixeroptions.c
    gstalsadeviceprobe.c
    gstalsamixerelement.c
    gstalsasrc.c
  )

  add_library(gstalsa MODULE ${SOURCES})

  target_link_libraries(gstalsa
    gstinterfaces-${GST_MAJORMINOR}
    gstaudio-${GST_MAJORMINOR}
    gstbase-${GST_MAJORMINOR}
    ${GLIB_LIBRARIES}
    ${ALSA_LIBRARIES}
  )

  install_plugin(gstalsa)
endif()
