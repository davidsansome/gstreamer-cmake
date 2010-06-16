cmake_minimum_required(VERSION 2.6)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(BEFORE ${CMAKE_CURRENT_SOURCE_DIR}/..)

set(CMAKE_C_FLAGS "-std=c99")
add_definitions(-DHAVE_AV_CONFIG_H)

set(SOURCES
  adler32.c
  aes.c
  avstring.c
  base64.c
  crc.c
  des.c
  fifo.c
  integer.c
  intfloat_readwrite.c
  lfg.c
  lls.c
  log.c
  lzo.c
  mathematics.c
  md5.c
  mem.c
  pca.c
  pixdesc.c
  random_seed.c
  rational.c
  rc4.c
  sha.c
  softfloat.c
  tree.c
  utils.c
)

add_library(libavutil SHARED ${SOURCES})
