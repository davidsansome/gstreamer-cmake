cmake_minimum_required(VERSION 2.6)

check_plugin_deps(good pulse PULSE_FOUND)
if (${PLUGIN_pulse_FOUND})
  include_directories(${CMAKE_CURRENT_SOURCE_DIR})
  include_directories(${CMAKE_CURRENT_BINARY_DIR})
  include_directories(${PULSE_INCLUDE_DIRS})

  set(SOURCES
    plugin.c
    pulsemixer.c
    pulsemixerctrl.c
    pulsemixertrack.c
    pulseprobe.c
    pulsesink.c
    pulsesrc.c
    pulseutil.c
  )

  add_library(gstpulse MODULE ${SOURCES})

  target_link_libraries(gstpulse
    gstbase-${GST_MAJORMINOR}
    gstaudio-${GST_MAJORMINOR}
    gstinterfaces-${GST_MAJORMINOR}
    ${GLIB_LIBRARIES}
    ${PULSE_LIBRARIES}
  )

  install_plugin(gstpulse)
endif (${PLUGIN_pulse_FOUND})
