cmake_minimum_required(VERSION 2.6)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_definitions(-DNSF_PLAYER)

set(SOURCES
  fds_snd.c
  fmopl.c
  gstnsf.c
  log.c
  mmc5_snd.c
  nes6502.c
  nes_apu.c
  nsf.c
  vrc7_snd.c
  vrcvisnd.c
)

add_library(gstnsf MODULE ${SOURCES})

target_link_libraries(gstnsf
  gstbase-${GST_MAJORMINOR}
  ${GLIB_LIBRARIES}
)

install_plugin(gstnsf)
