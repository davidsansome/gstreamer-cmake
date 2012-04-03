cmake_minimum_required(VERSION 2.6)

add_definitions(-DPIC)

add_subdirectory(libavcodec)
add_subdirectory(libavdevice)
add_subdirectory(libavfilter)
add_subdirectory(libavformat)
add_subdirectory(libavutil)
add_subdirectory(libpostproc)
add_subdirectory(libswscale)
