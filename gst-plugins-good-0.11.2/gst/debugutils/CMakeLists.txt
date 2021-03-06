cmake_minimum_required(VERSION 2.6)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(DEBUG_SOURCES
  gstdebug.c
  breakmydata.c
  gstnavseek.c
  gstpushfilesrc.c
  gsttaginject.c
  rndbuffersize.c
  progressreport.c
  tests.c
  testplugin.c
  cpureport.c
  gstcapssetter.c
)
set(NAVIGATIONTEST_SOURCES
  gstnavigationtest.c
)

add_library(gstdebug MODULE ${DEBUG_SOURCES})
add_library(gstnavigationtest MODULE ${NAVIGATIONTEST_SOURCES})

target_link_libraries(gstdebug
  gstbase-${GST_MAJORMINOR}
  ${GLIB_LIBRARIES}
)
target_link_libraries(gstnavigationtest
  gstbase-${GST_MAJORMINOR}
  gstvideo-${GST_MAJORMINOR}
  ${GLIB_LIBRARIES}
)

install_plugin(gstdebug)
install_plugin(gstnavigationtest)
