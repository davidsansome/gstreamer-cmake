cmake_minimum_required(VERSION 2.6)

add_definitions(
  -D_GNU_SOURCE
  -DHAVE_CONFIG_H
)

set(GST_VERSION_MICRO 14)
set(GST_VERSION_NANO 0)
set(VERSION ${GST_MAJORMINOR}.${GST_VERSION_MICRO})
set(PACKAGE "gst-plugins-ugly")
set(PACKAGE_NAME "GStreamer Ugly Plugins")
set(PACKAGE_BUGREPORT "http://bugzilla.gnome.org/enter_bug.cgi?product=GStreamer")
set(PACKAGE_STRING "${PACKAGE_NAME} ${VERSION}")
set(PACKAGE_TARNAME ${PACKAGE})
set(PACKAGE_VERSION ${VERSION})
set(GETTEXT_PACKAGE "${PACKAGE}-${GST_MAJORMINOR}")

# Optional dependencies
find_path(A52_INCLUDE_DIRS a52dec/a52.h)
find_library(A52_LIBRARIES a52)
find_path(LAME_INCLUDE_DIRS lame/lame.h)
find_library(LAME_LIBRARIES mp3lame)
find_path(SIDPLAY_INCLUDE_DIRS sidplay/player.h)
find_library(SIDPLAY_LIBRARIES sidplay)

pkg_check_modules(AMRNB opencore-amrnb)
pkg_check_modules(AMRWB opencore-amrwb)
pkg_check_modules(CDIO libcdio>=0.76)
pkg_check_modules(DVDREAD dvdread)
pkg_check_modules(MAD id3tag>=0.15 mad>=0.15)
pkg_check_modules(MPEG2DEC libmpeg2>=0.4.0)
pkg_check_modules(TWOLAME twolame>=0.3.10)
pkg_check_modules(X264 x264)


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

add_subdirectory(ext/a52dec)
add_subdirectory(ext/amrnb)
add_subdirectory(ext/amrwbdec)
add_subdirectory(ext/cdio)
add_subdirectory(ext/dvdread)
add_subdirectory(ext/lame)
add_subdirectory(ext/mad)
add_subdirectory(ext/mpeg2dec)
add_subdirectory(ext/sidplay)
add_subdirectory(ext/twolame)
add_subdirectory(ext/x264)
add_subdirectory(gst/asfdemux)
add_subdirectory(gst/dvdlpcmdec)
add_subdirectory(gst/dvdsub)
add_subdirectory(gst/iec958)
add_subdirectory(gst/mpegaudioparse)
add_subdirectory(gst/mpegstream)
add_subdirectory(gst/realmedia)
add_subdirectory(gst/synaesthesia)
