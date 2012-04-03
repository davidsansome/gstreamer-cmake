cmake_minimum_required(VERSION 2.6)

# Things you can change
set(GST_LICENSE "LGPL")
set(GST_VERSION_MAJOR 0)
set(GST_VERSION_MINOR 11)
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
add_definitions(-DGST_USE_UNSTABLE_API)

set(DEFAULT_AUDIOSINK "autoaudiosink" CACHE STRING "Default audio sink")
set(DEFAULT_AUDIOSRC "alsasrc" CACHE STRING "Default audio source")
set(DEFAULT_VIDEOSINK "autovideosink" CACHE STRING "Default video sink")
set(DEFAULT_VIDEOSRC "v4l2src" CACHE STRING "Default video source")
set(DEFAULT_VISUALISER "goom" CACHE STRING "Default visualiser")

# Helpers
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMake")
include(GLibHelpers)
include(GstMacros)
include(Orcc)

# Dependencies that are required by all packages
include(FindPkgConfig)
pkg_check_modules(GLIB REQUIRED glib-2.0>=2.31.14)
pkg_check_modules(GOBJECT REQUIRED gobject-2.0)
pkg_check_modules(GMODULE REQUIRED gmodule-2.0)
pkg_check_modules(GTHREAD REQUIRED gthread-2.0)
pkg_check_modules(LIBOIL REQUIRED liboil-0.3)
pkg_check_modules(LIBXML libxml-2.0)
pkg_check_modules(CHECK check)
pkg_check_modules(SPEEX speex>=1.2)

find_library(FAAC_LIBRARIES faac)
find_path(FAAC_INCLUDE_DIRS faac.h)
find_library(FAAD_LIBRARIES faad)
find_path(FAAD_INCLUDE_DIRS faad.h)

link_directories(${GLIB_LIBRARY_DIRS})

find_package(BISON)
find_package(FLEX)

# Release mode
if(${CMAKE_BUILD_TYPE} MATCHES "Release")
  add_definitions(-DNDEBUG)
endif()

# Useful variables for autotools compat.
if (${CMAKE_SYSTEM_PROCESSOR} EQUAL "x86_64")
  set(HAVE_CPU_X86_64 1)
else (${CMAKE_SYSTEM_PROCESSOR} EQUAL "x86_64")
  set(HAVE_CPU_I386 1)
endif()

# These lists are appended to by GstMacros
set(PLUGINS_FOUND "" CACHE INTERNAL foo)
set(PLUGINS_NOT_FOUND "" CACHE INTERNAL foo)

add_subdirectory(gstreamer-0.11.3)
add_subdirectory(gst-plugins-base-0.11.3)
add_subdirectory(gst-plugins-good-0.11.2)
#add_subdirectory(gst-plugins-ugly-0.10.14)
#add_subdirectory(gst-plugins-bad-0.10.18)
#add_subdirectory(gst-ffmpeg-0.10.10)

show_plugin_summary()
