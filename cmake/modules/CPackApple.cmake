if(NSX_PYTHON)
    if(NSX_PYTHON3)
        set(MAC_PYTHON_VERSION "python3")
    else()
        set(MAC_PYTHON_VERSION "python2")
    endif()
else()
    message(FATAL_ERROR "NSX_PYTHON option must be set to ON for deploying om MacOS")
endif()

set(BUNDLE BUNDLE)

set(EXECUTABLE_OPTIONS MACOSX_BUNDLE)

set(MACOSX_BUNDLE_IDENTIFIER "org.nsxtoolproject")

set(MACOSX_BUNDLE_ICON_FILE nsxtool.icns)

set(MACOSX_BUNDLE_NAME nsxtool)

set(NSXTOOL_BUNDLE_DIR ${CMAKE_BINARY_DIR}/nsxtool.app/Contents)
file(MAKE_DIRECTORY ${NSXTOOL_BUNDLE_DIR})

set(NSXQT_INSTALL_DIR ${NSXTOOL_BUNDLE_DIR})

set_source_files_properties(${CMAKE_SOURCE_DIR}/apps/nsxqt/resources/nsxtool.icns PROPERTIES MACOSX_PACKAGE_LOCATION Resources)

configure_file("${CMAKE_SOURCE_DIR}/cmake/modules/MacOSXBundleInfo.plist.in" "${NSXTOOL_BUNDLE_DIR}/Info.plist" @ONLY)

set(CPACK_PACKAGE_FILE_NAME "${CMAKE_PROJECT_NAME}-${NSXTOOL_VERSION}-${MAC_PYTHON_VERSION}-${NSXTOOL_ARCHITECTURE}")

set(CPACK_BINARY_DRAGNDROP ON)

set(CPACK_GENERATOR "DragNDrop")

