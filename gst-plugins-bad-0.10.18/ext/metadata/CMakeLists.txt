cmake_minimum_required(VERSION 2.6)

check_plugin_deps(bad metadata EXIF_FOUND IPTC_FOUND XMP_FOUND)
if (${PLUGIN_metadata_FOUND})
  include_directories(${CMAKE_CURRENT_SOURCE_DIR})
  include_directories(${CMAKE_CURRENT_BINARY_DIR})
  include_directories(${EXIF_INCLUDE_DIRS})
  include_directories(${IPTC_INCLUDE_DIRS})
  include_directories(${XMP_INCLUDE_DIRS})

  set(SOURCES
    gstbasemetadata.c
    gstmetadata.c
    gstmetadatademux.c
    gstmetadatamux.c
    metadata.c
    metadataexif.c
    metadataiptc.c
    metadatamuxjpeg.c
    metadatamuxpng.c
    metadataparsejpeg.c
    metadataparsepng.c
    metadataparseutil.c
    metadatatags.c
    metadatatypes.c
    metadataxmp.c
  )

  add_library(gstmetadata MODULE ${SOURCES})

  target_link_libraries(gstmetadata
    gstbase-${GST_MAJORMINOR}
    gsttag-${GST_MAJORMINOR}
    ${GLIB_LIBRARIES}
    ${EXIF_LIBRARIES}
    ${IPTC_LIBRARIES}
    ${XMP_LIBRARIES}
  )

  install_plugin(gstmetadata)
endif (${PLUGIN_metadata_FOUND})
