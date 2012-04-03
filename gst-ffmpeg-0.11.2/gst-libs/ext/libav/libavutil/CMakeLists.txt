cmake_minimum_required(VERSION 2.6)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(BEFORE ${CMAKE_CURRENT_SOURCE_DIR}/..)

set(CMAKE_C_FLAGS "-std=c99")
add_definitions(-DHAVE_AV_CONFIG_H)

set(SOURCES
  adler32.c
  aes.c
  audioconvert.c
  avstring.c
  base64.c
  cpu.c
  crc.c
  des.c
  dict.c
  error.c
  eval.c
  fifo.c
  file.c
  imgutils.c
  intfloat_readwrite.c
  inverse.c
  lfg.c
  lls.c
  log.c
  lzo.c
  mathematics.c
  md5.c
  mem.c
  opt.c
  parseutils.c
  pixdesc.c
  random_seed.c
  rational.c
  rc4.c
  samplefmt.c
  sha.c
  tree.c
  utils.c
)

add_library(avutil SHARED ${SOURCES})

install(TARGETS avutil
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
)