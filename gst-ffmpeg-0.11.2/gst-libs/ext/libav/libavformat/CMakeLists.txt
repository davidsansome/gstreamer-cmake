cmake_minimum_required(VERSION 2.6)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(BEFORE ${CMAKE_CURRENT_SOURCE_DIR}/..)

set(CMAKE_C_FLAGS "-std=c99")
add_definitions(-DHAVE_AV_CONFIG_H)

set(SOURCES
  4xm.c
  a64.c
  aacdec.c
  ac3dec.c
  adtsenc.c
  adxdec.c
  aea.c
  aiffdec.c
  aiffenc.c
  allformats.c
  amr.c
  anm.c
  apc.c
  ape.c
  apetag.c
  applehttp.c
  asf.c
  asfcrypt.c
  asfdec.c
  asfenc.c
  assdec.c
  assenc.c
  au.c
  audiointerleave.c
  avc.c
  avidec.c
  avienc.c
  avio.c
  aviobuf.c
  avlanguage.c
  avs.c
  bethsoftvid.c
  bfi.c
  bink.c
  bmv.c
  c93.c
  caf.c
  cafdec.c
  cavsvideodec.c
  cdg.c
  crcenc.c
  cutils.c
  daud.c
  dfa.c
  diracdec.c
  dnxhddec.c
  dsicin.c
  dtsdec.c
  dv.c
  dvenc.c
  dxa.c
  eacdata.c
  electronicarts.c
  ffmdec.c
  ffmenc.c
  ffmetadec.c
  ffmetaenc.c
  filmstripdec.c
  filmstripenc.c
  flacdec.c
  flacenc.c
  flacenc_header.c
  flic.c
  flvdec.c
  flvenc.c
  framecrcenc.c
  gif.c
  gsmdec.c
  gxf.c
  gxfenc.c
  h261dec.c
  h263dec.c
  h264dec.c
  id3v1.c
  id3v2.c
  id3v2enc.c
  idcin.c
  idroqdec.c
  idroqenc.c
  iff.c
  img2.c
  ingenientdec.c
  ipmovie.c
  isom.c
  iss.c
  iv8.c
  ivfdec.c
  ivfenc.c
  jvdec.c
  latmenc.c
  lmlm4.c
  lxfdec.c
  m4vdec.c
  matroska.c
  matroskadec.c
  matroskaenc.c
  md5enc.c
  metadata.c
  mm.c
  mmf.c
  mov.c
  mov_chan.c
  movenc.c
  movenchint.c
  mp3dec.c
  mp3enc.c
  mpc.c
  mpc8.c
  mpeg.c
  mpegenc.c
  mpegts.c
  mpegtsenc.c
  mpegvideodec.c
  mpjpeg.c
  msnwc_tcp.c
  mtv.c
  mvi.c
  mxf.c
  mxfdec.c
  mxfenc.c
  mxg.c
  ncdec.c
  nsvdec.c
  nullenc.c
  nut.c
  nutdec.c
  nutenc.c
  nuv.c
  oggdec.c
  oggenc.c
  oggparsecelt.c
  oggparsedirac.c
  oggparseflac.c
  oggparseogm.c
  oggparseskeleton.c
  oggparsespeex.c
  oggparsetheora.c
  oggparsevorbis.c
  oma.c
  omadec.c
  omaenc.c
  options.c
  os_support.c
  pcm.c
  pcmdec.c
  pcmenc.c
  pmpdec.c
  psxstr.c
  pva.c
  qcp.c
  r3d.c
  rawdec.c
  rawenc.c
  rawvideodec.c
  riff.c
  rl2.c
  rm.c
  rmdec.c
  rmenc.c
  rpl.c
  rso.c
  rsodec.c
  rsoenc.c
  rtp.c
  rtpenc.c
  rtpenc_aac.c
  rtpenc_amr.c
  rtpenc_chain.c
  rtpenc_h263.c
  rtpenc_h264.c
  rtpenc_latm.c
  rtpenc_mpv.c
  rtpenc_vp8.c
  rtpenc_xiph.c
  sauce.c
  sdp.c
  seek.c
  segafilm.c
  segment.c
  sierravmd.c
  siff.c
  smacker.c
  smjpeg.c
  smjpegdec.c
  smjpegenc.c
  sol.c
  soxdec.c
  soxenc.c
  spdif.c
  spdifdec.c
  spdifenc.c
  srtdec.c
  swfdec.c
  swfenc.c
  thp.c
  tiertexseq.c
  tmv.c
  tta.c
  tty.c
  txd.c
  utils.c
  vc1test.c
  vc1testenc.c
  voc.c
  vocdec.c
  vocenc.c
  vorbiscomment.c
  vqf.c
  wav.c
  wc3movie.c
  westwood.c
  wtv.c
  wv.c
  xa.c
  xmv.c
  xwma.c
  yop.c
  yuv4mpeg.c
)

if(LINUX)
  list(APPEND SOURCES
  )
endif()

if(WINDOWS)
  list(APPEND SOURCES avisynth.c)
endif()
add_library(avformat SHARED ${SOURCES})
target_link_libraries(avformat
  avcodec
  avutil
  swscale
  ${BZ2_LIBRARIES}
)

install(TARGETS avformat
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
)