cmake_minimum_required(VERSION 2.6)

set(prefix ${CMAKE_INSTALL_PREFIX})
set(exec_prefix ${CMAKE_INSTALL_PREFIX})
set(libdir ${CMAKE_INSTALL_PREFIX}/lib)
set(includedir ${CMAKE_INSTALL_PREFIX}/include)

configure_pkgconfig(gstreamer-base)
configure_pkgconfig(gstreamer-check)
configure_pkgconfig(gstreamer-controller)
configure_pkgconfig(gstreamer-net)
configure_pkgconfig(gstreamer)
