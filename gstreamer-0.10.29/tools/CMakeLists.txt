cmake_minimum_required(VERSION 2.6)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

function(add_versioned_executable name source)
  add_executable(gst-${name}-${GST_MAJORMINOR} ${source})
  target_link_libraries(gst-${name}-${GST_MAJORMINOR}
    gstreamer-${GST_MAJORMINOR}
    ${GLIB_LIBRARIES}
    ${GOBJECT_LIBRARIES}
    ${GMODULE_LIBRARIES}
  )

  add_executable(gst-${name} gst-run.c)
  target_link_libraries(gst-${name}
    gstreamer-${GST_MAJORMINOR}
    ${GLIB_LIBRARIES}
    ${GOBJECT_LIBRARIES}
    ${GMODULE_LIBRARIES}
  )

  install(TARGETS gst-${name}-${GST_MAJORMINOR} gst-${name}
    RUNTIME DESTINATION bin
  )
endfunction(add_versioned_executable)


if (NOT ${GST_DISABLE_LOADSAVE})
  add_versioned_executable(xmllaunch gst-launch.c)
endif (NOT ${GST_DISABLE_LOADSAVE})

add_versioned_executable(launch gst-launch.c)
add_versioned_executable(inspect gst-inspect.c)
add_versioned_executable(typefind gst-typefind.c)
add_versioned_executable(xmlinspect gst-xmlinspect.c)

