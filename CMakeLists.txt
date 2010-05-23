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

include(FindPkgConfig)
pkg_check_modules(GLIB REQUIRED glib-2.0)
pkg_check_modules(GOBJECT REQUIRED gobject-2.0)
pkg_check_modules(GMODULE REQUIRED gmodule-2.0)
pkg_check_modules(LIBXML libxml-2.0)

find_package(BISON)
find_package(FLEX)

find_program(GLIB_MKENUMS glib-mkenums)
find_program(GLIB_GENMARSHAL glib-genmarshal)

macro(add_glib_marshal outfiles name prefix otherinclude)
  add_custom_command(
    OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${name}.h"
    COMMAND ${GLIB_GENMARSHAL} --header "--prefix=${prefix}"
            "${CMAKE_CURRENT_SOURCE_DIR}/${name}.list"
            > "${CMAKE_CURRENT_BINARY_DIR}/${name}.h"
    DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/${name}.list"
  )
  add_custom_command(
    OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${name}.c"
    COMMAND echo "\\#include \\\"${otherinclude}\\\"" > "${CMAKE_CURRENT_BINARY_DIR}/${name}.c"
    COMMAND echo "\\#include \\\"glib-object.h\\\"" >> "${CMAKE_CURRENT_BINARY_DIR}/${name}.c"
    COMMAND echo "\\#include \\\"${name}.h\\\"" >> "${CMAKE_CURRENT_BINARY_DIR}/${name}.c"
    COMMAND ${GLIB_GENMARSHAL} --body "--prefix=${prefix}"
            "${CMAKE_CURRENT_SOURCE_DIR}/${name}.list"
            >> "${CMAKE_CURRENT_BINARY_DIR}/${name}.c"
    DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/${name}.list"
            "${CMAKE_CURRENT_BINARY_DIR}/${name}.h"
  )
  list(APPEND ${outfiles} "${CMAKE_CURRENT_BINARY_DIR}/${name}.c")
endmacro(add_glib_marshal)

macro(add_glib_enumtypes outfiles name)
  add_custom_command(
    OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${name}.h"
    COMMAND ${GLIB_MKENUMS}
            --template "${CMAKE_CURRENT_SOURCE_DIR}/${name}.h.template"
            ${ARGN} > "${CMAKE_CURRENT_BINARY_DIR}/${name}.h"
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    DEPENDS ${ARGN} "${CMAKE_CURRENT_SOURCE_DIR}/${name}.h.template"
  )
  add_custom_command(
    OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${name}.c"
    COMMAND ${GLIB_MKENUMS}
            --template "${CMAKE_CURRENT_SOURCE_DIR}/${name}.c.template"
            ${ARGN} > "${CMAKE_CURRENT_BINARY_DIR}/${name}.c"
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    DEPENDS ${ARGN} "${CMAKE_CURRENT_SOURCE_DIR}/${name}.c.template"
            "${CMAKE_CURRENT_BINARY_DIR}/${name}.h"
  )
  list(APPEND ${outfiles} "${CMAKE_CURRENT_BINARY_DIR}/${name}.c")
endmacro(add_glib_enumtypes)

macro(install_plugin target)
  install(TARGETS ${target}
    LIBRARY DESTINATION lib/gstreamer-${GST_MAJORMINOR}
    RUNTIME DESTINATION lib/gstreamer-${GST_MAJORMINOR}
  )
endmacro(install_plugin)

macro(install_lib target)
  install(TARGETS ${target}-${GST_MAJORMINOR}
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
  )
endmacro(install_lib)

macro(install_headers dir)
  install(FILES ${ARGN} DESTINATION include/gstreamer-${GST_MAJORMINOR}/${dir})
endmacro(install_headers)


if(${CMAKE_BUILD_TYPE} MATCHES "Release")
  add_definitions(-DNDEBUG)
  add_definitions(-DQT_NO_DEBUG_OUTPUT)
endif(${CMAKE_BUILD_TYPE} MATCHES "Release")


add_subdirectory(gstreamer-0.10.29)
#add_subdirectory(gst-plugins-base-0.10.29)
