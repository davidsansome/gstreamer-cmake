cmake_minimum_required(VERSION 2.6)

check_plugin_deps(ffmpeg ffmpeg)
if (${PLUGIN_ffmpeg_FOUND})
  include_directories(${CMAKE_CURRENT_SOURCE_DIR})
  include_directories(${CMAKE_CURRENT_BINARY_DIR})
  include_directories(${ALSA_INCLUDE_DIRS})
  include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../gst-libs/ext/ffmpeg)
  include_directories(BEFORE ${CMAKE_CURRENT_BINARY_DIR}/../..)

  add_definitions(-DHAVE_CONFIG_H)

  set(SOURCES
    gstffmpeg.c
    gstffmpegaudioresample.c
    gstffmpegcfg.c
    gstffmpegcodecmap.c
    gstffmpegdec.c
    gstffmpegdeinterlace.c
    gstffmpegdemux.c
    gstffmpegenc.c
    gstffmpegmux.c
    gstffmpegprotocol.c
    gstffmpegutils.c
  )

  add_library(ffmpeg MODULE ${SOURCES})

  target_link_libraries(ffmpeg
    libavcodec
    libavformat
    libavutil
    gstaudio-${GST_MAJORMINOR}
    gstbase-${GST_MAJORMINOR}
    ${GLIB_LIBRARIES}
  )

  install_plugin(ffmpeg)
endif (${PLUGIN_ffmpeg_FOUND})
