cmake_minimum_required(VERSION 2.6)

check_plugin_deps(good gconfelements LIBGCONF_FOUND)
if (${PLUGIN_gconfelements_FOUND})
  include_directories(${CMAKE_CURRENT_SOURCE_DIR})
  include_directories(${CMAKE_CURRENT_BINARY_DIR})
  include_directories(${LIBGCONF_INCLUDE_DIRS})

  add_definitions(
    "-DGST_GCONF_DIR=\"/system/gstreamer/${GST_MAJORMINOR}\""
  )

  set(SOURCES
    gstgconf.c
    gstgconfaudiosink.c
    gstgconfaudiosrc.c
    gstgconfelements.c
    gstgconfvideosink.c
    gstgconfvideosrc.c
    gstswitchsink.c
  )

  add_library(gstgconfelements MODULE ${SOURCES})

  target_link_libraries(gstgconfelements
    gstbase-${GST_MAJORMINOR}
    ${GLIB_LIBRARIES}
    ${LIBGCONF_LIBRARIES}
  )

  install_plugin(gstgconfelements)
endif (${PLUGIN_gconfelements_FOUND})
