cmake_minimum_required(VERSION 2.6)

check_plugin_deps(jpeg JPEG_LIBRARIES JPEG_INCLUDE_DIRS)
if (${PLUGIN_jpeg_FOUND})
  include_directories(${CMAKE_CURRENT_SOURCE_DIR})
  include_directories(${CMAKE_CURRENT_BINARY_DIR})
  include_directories(${JPEG_INCLUDE_DIRS})

  set(SOURCES
    gstjpeg.c
    gstjpegdec.c
    gstjpegenc.c
    gstsmokedec.c
    gstsmokeenc.c
    smokecodec.c
  )

  add_library(gstjpeg MODULE ${SOURCES})

  target_link_libraries(gstjpeg
    gstbase-${GST_MAJORMINOR}
    ${GLIB_LIBRARIES}
    ${JPEG_LIBRARIES}
  )

  install_plugin(gstjpeg)
endif (${PLUGIN_jpeg_FOUND})
