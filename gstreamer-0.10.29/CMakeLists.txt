cmake_minimum_required(VERSION 2.6)

add_definitions(
  -D_GNU_SOURCE
  -DGST_DISABLE_DEPRECATED
  -DHAVE_CONFIG_H
)

set(GST_VERSION_MICRO 29)
set(GST_VERSION_NANO 0)
set(VERSION ${GST_MAJORMINOR}.${GST_VERSION_MICRO})
set(PACKAGE "gstreamer")
set(PACKAGE_NAME "GStreamer")
set(PACKAGE_BUGREPORT "http://bugzilla.gnome.org/enter_bug.cgi?product=GStreamer")
set(PACKAGE_STRING "${PACKAGE_NAME} ${VERSION}")
set(PACKAGE_TARNAME ${PACKAGE})
set(PACKAGE_VERSION ${VERSION})
set(GETTEXT_PACKAGE "${PACKAGE}-${GST_MAJORMINOR}")


include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/gst)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libs)
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR}/gst)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/libs)

include_directories(${GLIB_INCLUDE_DIRS})
include_directories(${GMODULE_INCLUDE_DIRS})
include_directories(${LIBXML_INCLUDE_DIRS})

configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/gst/gstconfig.h.in
  ${CMAKE_CURRENT_BINARY_DIR}/gst/gstconfig.h
)
configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/gst/gstversion.h.in
  ${CMAKE_CURRENT_BINARY_DIR}/gst/gstversion.h
)
configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/config.h.in
  ${CMAKE_CURRENT_BINARY_DIR}/config.h
)

add_subdirectory(gst)
add_subdirectory(libs/gst/base)
add_subdirectory(libs/gst/check)
add_subdirectory(libs/gst/controller)
add_subdirectory(libs/gst/dataprotocol)
add_subdirectory(libs/gst/helpers)
add_subdirectory(libs/gst/net)
add_subdirectory(plugins/elements)
add_subdirectory(plugins/indexers)
add_subdirectory(tools)
