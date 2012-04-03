cmake_minimum_required(VERSION 2.6)

add_definitions(
  -D_GNU_SOURCE
  -DHAVE_CONFIG_H
)

set(GST_VERSION_MICRO 2)
set(GST_VERSION_NANO 0)
set(VERSION ${GST_MAJORMINOR}.${GST_VERSION_MICRO})
set(PACKAGE "gst-ffmpeg")
set(PACKAGE_NAME "GStreamer FFMPEG Plugins")
set(PACKAGE_BUGREPORT "http://bugzilla.gnome.org/enter_bug.cgi?product=GStreamer")
set(PACKAGE_STRING "${PACKAGE_NAME} ${VERSION}")
set(PACKAGE_TARNAME ${PACKAGE})
set(PACKAGE_VERSION ${VERSION})
set(GETTEXT_PACKAGE "${PACKAGE}-${GST_MAJORMINOR}")

# Optional dependencies
find_library(AALIB_LIBRARIES aa)
find_path(AALIB_INCLUDE_DIRS aalib.h)
find_library(JPEG_LIBRARIES jpeg)
find_path(JPEG_INCLUDE_DIRS jconfig.h)
find_library(VORBIS_LIBRARIES vorbis)
find_library(LAME_LIBRARIES mp3lame)
find_path(LAME_INCLUDE_DIRS lame.h)
find_library(XVID_LIBRARIES xvidcore)
find_path(XVID_INCLUDE_DIRS xvid.h)
find_library(BZ2_LIBRARIES bz2)
find_path(BZ2_INCLUDE_DIRS bzlib.h)

pkg_check_modules(LIBOIL liboil>=0.3.6)
pkg_check_modules(VORBIS vorbis)
pkg_check_modules(VORBISENC vorbisenc)
pkg_check_modules(THEORA theora)
pkg_check_modules(THEORADEC theoradec)
pkg_check_modules(THEORAENC theoraenc)
pkg_check_modules(JACK jack)

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

add_subdirectory(gst-libs/ext/libav)
add_subdirectory(ext/ffmpeg)
add_subdirectory(ext/libpostproc)
add_subdirectory(ext/libswscale)
