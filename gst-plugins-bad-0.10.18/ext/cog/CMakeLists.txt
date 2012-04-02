cmake_minimum_required(VERSION 2.6)

check_plugin_deps(bad cog COG_FOUND)
if(PLUGIN_cog_FOUND)
  include_directories(${CMAKE_CURRENT_SOURCE_DIR})
  include_directories(${CMAKE_CURRENT_SOURCE_DIR}/..)
  include_directories(${CMAKE_CURRENT_BINARY_DIR})
  include_directories(${COG_INCLUDE_DIRS})

  add_definitions(-DCOG_ENABLE_UNSTABLE_API)

  set(SOURCES
    cogframe.c
    cogtables.c
    cogvirtframe.c
    generate_tables.c
    gstcms.c
    gstcog.c
    gstcogcolorspace.c
    gstcogdownsample.c
    gstcogfilter.c
    gstcogmse.c
    gstcogscale.c
    gstcogutils.c
    gstcolorconvert.c
    gstlogoinsert.c
  )

  add_library(gstcog MODULE ${SOURCES})

  target_link_libraries(gstcog
    gstbase-${GST_MAJORMINOR}
    gstvideo-${GST_MAJORMINOR}
    ${GLIB_LIBRARIES}
    ${COG_LIBRARIES}
  )

  install_plugin(gstcog)
endif()
