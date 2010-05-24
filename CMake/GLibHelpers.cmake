cmake_minimum_required(VERSION 2.6)
if(POLICY CMP0011)
  cmake_policy(SET CMP0011 NEW)
endif(POLICY CMP0011)

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

macro(add_glib_enumtypes_t outfiles name htemplate ctemplate)
  add_custom_command(
    OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${name}.h"
    COMMAND ${GLIB_MKENUMS}
            --template "${htemplate}"
            ${ARGN} > "${CMAKE_CURRENT_BINARY_DIR}/${name}.h"
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    DEPENDS ${ARGN} "${htemplate}"
  )
  add_custom_command(
    OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${name}.c"
    COMMAND ${GLIB_MKENUMS}
            --template "${ctemplate}"
            ${ARGN} > "${CMAKE_CURRENT_BINARY_DIR}/${name}.c"
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    DEPENDS ${ARGN} ${ctemplate}
            "${CMAKE_CURRENT_BINARY_DIR}/${name}.h"
  )
  list(APPEND ${outfiles} "${CMAKE_CURRENT_BINARY_DIR}/${name}.c")
endmacro(add_glib_enumtypes_t)

macro(add_glib_enumtypes outfiles name includeguard)
  set(htemplate "${CMAKE_CURRENT_BINARY_DIR}/${name}.h.template")
  set(ctemplate "${CMAKE_CURRENT_BINARY_DIR}/${name}.c.template")

  # Write the .h template
  file(WRITE ${htemplate} "/*** BEGIN file-header ***/
#ifndef __${includeguard}_ENUM_TYPES_H__
#define __${includeguard}_ENUM_TYPES_H__

#include <glib-object.h>

G_BEGIN_DECLS

/*** END file-header ***/
/*** BEGIN file-production ***/

/* enumerations from \"@filename@\" */

/*** END file-production ***/
/*** BEGIN value-header ***/
GType @enum_name@_get_type (void);
#define GST_TYPE_@ENUMSHORT@ (@enum_name@_get_type())

/*** END value-header ***/
/*** BEGIN file-tail ***/
G_END_DECLS

#endif /* __${includeguard}_ENUM_TYPES_H__ */
/*** END file-tail ***/
")

  # Write the .c template
  file(WRITE ${ctemplate} "/*** BEGIN file-header ***/
#include \"${name}.h\"\n")
  foreach(header ${ARGN})
    file(APPEND ${ctemplate} "#include \"${header}\"\n")
  endforeach(header)
  file(APPEND ${ctemplate} "
/*** END file-header ***/
/*** BEGIN file-production ***/

/* enumerations from \"@filename@\" */
/*** END file-production ***/
/*** BEGIN value-header ***/
GType
@enum_name@_get_type (void)
{
  static volatile gsize g_define_type_id__volatile = 0;
  if (g_once_init_enter (&g_define_type_id__volatile)) {
    static const G@Type@Value values[] = {
/*** END value-header ***/
/*** BEGIN value-production ***/
{ @VALUENAME@, \"@VALUENAME@\", \"@valuenick@\" },
/*** END value-production ***/
/*** BEGIN value-tail ***/
{ 0, NULL, NULL }
    };
    GType g_define_type_id = g_@type@_register_static (\"@EnumName@\", values);
    g_once_init_leave (&g_define_type_id__volatile, g_define_type_id);
  }
  return g_define_type_id__volatile;
}

/*** END value-tail ***/
")

  add_glib_enumtypes_t(${outfiles} ${name} ${htemplate} ${ctemplate} ${ARGN})
endmacro(add_glib_enumtypes)