cmake_minimum_required(VERSION 2.6)

check_plugin_deps(good gdkpixbuf GDKPIXBUF_FOUND)
if(PLUGIN_gdkpixbuf_FOUND)
  include_directories(${CMAKE_CURRENT_SOURCE_DIR})
  include_directories(${CMAKE_CURRENT_BINARY_DIR})
  include_directories(${GDKPIXBUF_INCLUDE_DIRS})

  set(SOURCES
    gstgdkpixbuf.c
    gstgdkpixbufsink.c
    pixbufscale.c
  )

  add_library(gstgdkpixbuf MODULE ${SOURCES})

  target_link_libraries(gstgdkpixbuf
    gstbase-${GST_MAJORMINOR}
    gstvideo-${GST_MAJORMINOR}
    ${GLIB_LIBRARIES}
    ${GDKPIXBUF_LIBRARIES}
  )

  install_plugin(gstgdkpixbuf)
endif()
