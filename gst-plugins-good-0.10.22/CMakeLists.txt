cmake_minimum_required(VERSION 2.6)

add_definitions(
  -D_GNU_SOURCE
  -DHAVE_CONFIG_H
)

set(GST_VERSION_MICRO 22)
set(GST_VERSION_NANO 0)
set(VERSION ${GST_MAJORMINOR}.${GST_VERSION_MICRO})
set(PACKAGE "gst-plugins-good")
set(PACKAGE_NAME "GStreamer Good Plugins")
set(PACKAGE_BUGREPORT "http://bugzilla.gnome.org/enter_bug.cgi?product=GStreamer")
set(PACKAGE_STRING "${PACKAGE_NAME} ${VERSION}")
set(PACKAGE_TARNAME ${PACKAGE})
set(PACKAGE_VERSION ${VERSION})
set(GETTEXT_PACKAGE "${PACKAGE}-${GST_MAJORMINOR}")

include_directories(${CMAKE_BINARY_DIR}/include/gstreamer-${GST_MAJORMINOR})
include_directories(${CMAKE_SOURCE_DIR}/gstreamer-0.10.29/gst)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/gst-libs)
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR}/gst-libs)

include_directories(${GLIB_INCLUDE_DIRS})
include_directories(${GMODULE_INCLUDE_DIRS})
include_directories(${LIBXML_INCLUDE_DIRS})

configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/config.h.in
  ${CMAKE_CURRENT_BINARY_DIR}/config.h
)

configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/_stdint.h.in
  ${CMAKE_CURRENT_BINARY_DIR}/_stdint.h
)

add_subdirectory(gst/alpha)
add_subdirectory(gst/apetag)
add_subdirectory(gst/audiofx)
add_subdirectory(gst/auparse)
add_subdirectory(gst/autodetect)
add_subdirectory(gst/avi)
add_subdirectory(gst/cutter)
add_subdirectory(gst/debugutils)
add_subdirectory(gst/deinterlace)
add_subdirectory(gst/effectv)
add_subdirectory(gst/equalizer)
add_subdirectory(gst/flv)
add_subdirectory(gst/flx)
add_subdirectory(gst/goom)
add_subdirectory(gst/goom2k1)
add_subdirectory(gst/icydemux)
add_subdirectory(gst/id3demux)
add_subdirectory(gst/interleave)
add_subdirectory(gst/law)
add_subdirectory(gst/level)
add_subdirectory(gst/matroska)
add_subdirectory(gst/monoscope)
add_subdirectory(gst/multifile)
add_subdirectory(gst/multipart)
add_subdirectory(gst/qtdemux)
add_subdirectory(gst/replaygain)
add_subdirectory(gst/rtp)
add_subdirectory(gst/rtpmanager)
add_subdirectory(gst/rtsp)
add_subdirectory(gst/shapewipe)
add_subdirectory(gst/smpte)
add_subdirectory(gst/spectrum)
add_subdirectory(gst/udp)
add_subdirectory(gst/videobox)
add_subdirectory(gst/videocrop)
add_subdirectory(gst/videofilter)
add_subdirectory(gst/videomixer)
add_subdirectory(gst/wavenc)
add_subdirectory(gst/wavparse)
add_subdirectory(gst/y4m)
