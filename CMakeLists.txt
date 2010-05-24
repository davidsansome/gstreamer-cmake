cmake_minimum_required(VERSION 2.6)

set(GST_LICENCE "LGPL")
set(GST_VERSION_MAJOR 0)
set(GST_VERSION_MINOR 10)
set(GST_MAJORMINOR ${GST_VERSION_MAJOR}.${GST_VERSION_MINOR})
set(GST_PACKAGE_NAME "GStreamer source release" CACHE STRING "Package name")
set(GST_PACKAGE_ORIGIN "Unknown package origin" CACHE STRING "Package origin")
set(GST_DISABLE_GST_DEBUG 0 CACHE BOOL "Disable debugging")
set(GST_DISABLE_LOADSAVE 0 CACHE BOOL "Disable loadsave")
set(GST_DISABLE_PARSE 0 CACHE BOOL "Disable parse")
set(GST_DISABLE_TRACE 0 CACHE BOOL "Disable trace")
set(GST_DISABLE_ALLOC 0 CACHE BOOL "Disable alloc")
set(GST_DISABLE_REGISTRY 0 CACHE BOOL "Disable registry")
set(GST_DISABLE_XML 0 CACHE BOOL "Disable xml")
set(GST_DISABLE_PLUGIN 0 CACHE BOOL "Disable plugins")

set(DEFAULT_AUDIOSINK "autoaudiosink" CACHE STRING "Default audio sink")
set(DEFAULT_AUDIOSRC "alsasrc" CACHE STRING "Default audio source")
set(DEFAULT_VIDEOSINK "autovideosink" CACHE STRING "Default video sink")
set(DEFAULT_VIDEOSRC "v4l2src" CACHE STRING "Default video source")
set(DEFAULT_VISUALISER "goom" CACHE STRING "Default visualiser")

# Dependencies
include(FindPkgConfig)
pkg_check_modules(GLIB REQUIRED glib-2.0)
pkg_check_modules(GOBJECT REQUIRED gobject-2.0)
pkg_check_modules(GMODULE REQUIRED gmodule-2.0)
pkg_check_modules(LIBOIL REQUIRED liboil-0.3)
pkg_check_modules(LIBXML libxml-2.0)

find_package(BISON)
find_package(FLEX)

# Helpers
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMake")
include(GLibHelpers)
include(GstMacros)

if(${CMAKE_BUILD_TYPE} MATCHES "Release")
  add_definitions(-DNDEBUG)
  add_definitions(-DQT_NO_DEBUG_OUTPUT)
endif(${CMAKE_BUILD_TYPE} MATCHES "Release")

set(PLUGINS_FOUND "" CACHE INTERNAL foo)
set(PLUGINS_NOT_FOUND "" CACHE INTERNAL foo)

add_subdirectory(gstreamer-0.10.29)
add_subdirectory(gst-plugins-base-0.10.29)
add_subdirectory(gst-plugins-good-0.10.22)
add_subdirectory(gst-plugins-ugly-0.10.14)

show_plugin_summary()
