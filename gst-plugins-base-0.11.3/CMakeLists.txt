cmake_minimum_required(VERSION 2.6)

add_definitions(
  -D_GNU_SOURCE
  -DHAVE_CONFIG_H
)

set(GST_VERSION_MICRO 3)
set(GST_VERSION_NANO 0)
set(VERSION ${GST_MAJORMINOR}.${GST_VERSION_MICRO})
set(PACKAGE "gst-plugins-base")
set(PACKAGE_NAME "GStreamer Base Plugins")
set(PACKAGE_BUGREPORT "http://bugzilla.gnome.org/enter_bug.cgi?product=GStreamer")
set(PACKAGE_STRING "${PACKAGE_NAME} ${VERSION}")
set(PACKAGE_TARNAME ${PACKAGE})
set(PACKAGE_VERSION ${VERSION})
set(GETTEXT_PACKAGE "${PACKAGE}-${GST_MAJORMINOR}")

# Optional dependencies
find_library(CDPARANOIA_LIBRARIES cdda_paranoia)
find_path(CDPARANOIA_INCLUDE_DIRS cdda_paranoia.h)

pkg_check_modules(ALSA alsa>=0.9.1)
pkg_check_modules(LIBVISUAL libvisual-0.4>=0.4.0)
pkg_check_modules(OGG ogg>=1.0)
pkg_check_modules(ORC orc-0.4>=0.4.11)
pkg_check_modules(PANGO pango>=1.22.0)
pkg_check_modules(PANGOCAIRO pangocairo>=1.22.0)
pkg_check_modules(THEORADEC theoradec>=1.1)
pkg_check_modules(THEORAENC theoraenc>=1.1)
pkg_check_modules(VORBIS vorbis>=1.0)
pkg_check_modules(VORBISENC vorbisenc>=1.0)


include_directories(${CMAKE_BINARY_DIR}/include/gstreamer-${GST_MAJORMINOR})
include_directories(${CMAKE_SOURCE_DIR}/gstreamer-${VERSION}/gst)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/gst-libs)
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR}/gst-libs)

include_directories(${GLIB_INCLUDE_DIRS})
include_directories(${GMODULE_INCLUDE_DIRS})
include_directories(${LIBXML_INCLUDE_DIRS})
include_directories(${ORC_INCLUDE_DIRS})

configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/config.h.in
  ${CMAKE_CURRENT_BINARY_DIR}/config.h
)

configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/_stdint.h.in
  ${CMAKE_CURRENT_BINARY_DIR}/_stdint.h
)

add_subdirectory(ext/alsa)
add_subdirectory(ext/cdparanoia)
add_subdirectory(ext/libvisual)
add_subdirectory(ext/ogg)
add_subdirectory(ext/pango)
add_subdirectory(ext/theora)
add_subdirectory(ext/vorbis)
add_subdirectory(gst/adder)
add_subdirectory(gst/app)
add_subdirectory(gst/audioconvert)
add_subdirectory(gst/audiorate)
add_subdirectory(gst/audioresample)
add_subdirectory(gst/audiotestsrc)
add_subdirectory(gst/encoding)
add_subdirectory(gst/gdp)
add_subdirectory(gst/playback)
add_subdirectory(gst/subparse)
add_subdirectory(gst/tcp)
add_subdirectory(gst/typefind)
add_subdirectory(gst/videoconvert)
add_subdirectory(gst/videorate)
add_subdirectory(gst/videoscale)
add_subdirectory(gst/videotestsrc)
add_subdirectory(gst/volume)
add_subdirectory(gst-libs/gst/app)
add_subdirectory(gst-libs/gst/audio)
add_subdirectory(gst-libs/gst/fft)
add_subdirectory(gst-libs/gst/interfaces)
add_subdirectory(gst-libs/gst/pbutils)
add_subdirectory(gst-libs/gst/riff)
add_subdirectory(gst-libs/gst/rtp)
add_subdirectory(gst-libs/gst/rtsp)
add_subdirectory(gst-libs/gst/sdp)
add_subdirectory(gst-libs/gst/tag)
add_subdirectory(gst-libs/gst/video)
add_subdirectory(pkgconfig)
