cmake_minimum_required(VERSION 2.6)

add_definitions(
  -D_GNU_SOURCE
  -DHAVE_CONFIG_H
)

set(GST_VERSION_MICRO 29)
set(GST_VERSION_NANO 0)
set(VERSION ${GST_MAJORMINOR}.${GST_VERSION_MICRO})
set(PACKAGE "gst-plugins-base")
set(PACKAGE_NAME "GStreamer Base Plugins")
set(PACKAGE_BUGREPORT "http://bugzilla.gnome.org/enter_bug.cgi?product=GStreamer")
set(PACKAGE_STRING "${PACKAGE_NAME} ${VERSION}")
set(PACKAGE_TARNAME ${PACKAGE})
set(PACKAGE_VERSION ${VERSION})
set(GETTEXT_PACKAGE "${PACKAGE}-${GST_MAJORMINOR}")


include_directories(${CMAKE_BINARY_DIR}/include/gstreamer-${GST_MAJORMINOR})
include_directories(${CMAKE_SOURCE_DIR}/gstreamer-${VERSION}/gst)
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

#add_subdirectory(gst/adder)
add_subdirectory(gst-libs/gst/app)
add_subdirectory(gst-libs/gst/audio)
add_subdirectory(gst-libs/gst/cdda)
add_subdirectory(gst-libs/gst/fft)
add_subdirectory(gst-libs/gst/floatcast)
add_subdirectory(gst-libs/gst/interfaces)
add_subdirectory(gst-libs/gst/netbuffer)
add_subdirectory(gst-libs/gst/pbutils)
add_subdirectory(gst-libs/gst/riff)
add_subdirectory(gst-libs/gst/rtp)
add_subdirectory(gst-libs/gst/rtsp)
add_subdirectory(gst-libs/gst/sdp)
add_subdirectory(gst-libs/gst/tag)
add_subdirectory(gst-libs/gst/video)

