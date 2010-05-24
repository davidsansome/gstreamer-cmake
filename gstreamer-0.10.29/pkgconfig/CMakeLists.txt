cmake_minimum_required(VERSION 2.6)

set(prefix ${CMAKE_INSTALL_PREFIX})
set(exec_prefix ${CMAKE_INSTALL_PREFIX})
set(libdir ${CMAKE_INSTALL_PREFIX}/lib)
set(includedir ${CMAKE_INSTALL_PREFIX}/include)

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

configure_pkgconfig(gstreamer-base)
configure_pkgconfig(gstreamer-check)
configure_pkgconfig(gstreamer-controller)
configure_pkgconfig(gstreamer-dataprotocol)
configure_pkgconfig(gstreamer-net)
configure_pkgconfig(gstreamer)
