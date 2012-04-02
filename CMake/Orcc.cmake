cmake_minimum_required(VERSION 2.6)
if(POLICY CMP0011)
  cmake_policy(SET CMP0011 NEW)
endif(POLICY CMP0011)

find_program(ORCC_EXECUTABLE orcc)

macro(add_orc outfiles name)
  set(_orc "${CMAKE_CURRENT_SOURCE_DIR}/${name}.orc")
  set(_h "${CMAKE_CURRENT_BINARY_DIR}/${name}.h")
  set(_c "${CMAKE_CURRENT_BINARY_DIR}/tmp-orc-${name}.c")

  add_custom_command(
    OUTPUT "${_c}"
    COMMAND ${ORCC_EXECUTABLE}
            --compat 0.4.11
            --implementation
            --include glib.h
            -o "${_c}"
            "${_orc}"
    DEPENDS "${_orc}"
  )
  add_custom_command(
    OUTPUT "${_h}"
    COMMAND ${ORCC_EXECUTABLE}
            --compat 0.4.11
            --header
            --include glib.h
            -o "${_h}"
            "${_orc}"
    DEPENDS "${_c}"
  )
  list(APPEND ${outfiles} "${_c}" "${_h}")
endmacro()