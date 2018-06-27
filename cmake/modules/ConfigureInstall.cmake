if (APPLE AND ENABLE_CPACK)

    set(NSXTOOL_TARGET BUNDLE)

    set(EXECUTABLE_OPTIONS MACOSX_BUNDLE)

    set(MACOSX_BUNDLE_INFO_STRING "NSXTool: an application for reducing single-crystal diffraction data")

    set(MACOSX_BUNDLE_SHORT_VERSION_STRING "NSXTool version ${NSXTOOL_VERSION}")

    set(MACOSX_BUNDLE_LONG_VERSION_STRING "NSXTool version ${NSXTOOL_VERSION}")

    set(MACOSX_BUNDLE_GUI_IDENTIFIER "org.nsxtoolproject.NSXTool")

    set(MACOSX_BUNDLE_IDENTIFIER "org.nsxtoolproject")

    set(MACOSX_BUNDLE_ICON_FILE nsxtool.icns)

    set(MACOSX_BUNDLE_NAME nsxtool)

    set(NSXTOOL_BUNDLE_DIR ${CMAKE_BINARY_DIR}/nsxtool.app)

    set_source_files_properties(${CMAKE_SOURCE_DIR}/apps/nsxqt/resources/nsxtool.icns PROPERTIES MACOSX_PACKAGE_LOCATION Resources)

    configure_file("${CMAKE_SOURCE_DIR}/cmake/modules/MacOSXBundleInfo.plist.in" "${NSXTOOL_BUNDLE_DIR}/Info.plist" @ONLY)

elseif(WIN32)
    set(EXECUTABLE_OPTIONS WIN32)
endif()
