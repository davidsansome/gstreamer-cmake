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
  set(fulldir include/gstreamer-${GST_MAJORMINOR}/${dir})
  install(FILES ${ARGN} DESTINATION ${fulldir})
  foreach(file ${ARGN})
    configure_file(${file} ${CMAKE_BINARY_DIR}/${fulldir}/${file} COPY_ONLY)
  endforeach(file)
endmacro(install_headers)

macro(add_gen_headers outfiles dir)
  set(fulldir include/gstreamer-${GST_MAJORMINOR}/${dir})
  foreach(file ${ARGN})
    add_custom_command(
      OUTPUT "${CMAKE_BINARY_DIR}/${fulldir}/${file}"
      COMMAND ${CMAKE_COMMAND} -E copy
          "${CMAKE_CURRENT_BINARY_DIR}/${file}"
          "${CMAKE_BINARY_DIR}/${fulldir}/${file}"
      DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/${file}"
    )
    list(APPEND ${outfiles} "${CMAKE_BINARY_DIR}/${fulldir}/${file}")
  endforeach(file)
endmacro(add_gen_headers)

macro(install_gen_headers dir)
  set(fulldir include/gstreamer-${GST_MAJORMINOR}/${dir})
  foreach(file ${ARGN})
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${file}
      DESTINATION include/gstreamer-${GST_MAJORMINOR}/${dir})
  endforeach(file)
endmacro(install_gen_headers)

macro(configure_pkgconfig name)
  configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/${name}.pc.in
    ${CMAKE_CURRENT_BINARY_DIR}/${name}.pc
    @ONLY
  )

  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${name}.pc
    DESTINATION lib/pkgconfig
  )
endmacro(configure_pkgconfig)
