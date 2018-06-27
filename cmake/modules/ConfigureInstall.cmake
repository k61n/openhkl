if (APPLE)
    set(NSX_LIB_TYPE SHARED)

    if(ENABLE_CPACK)
        set(EXECUTABLE_OPTIONS MACOSX_BUNDLE)

        set(NSXTOOL_TARGET BUNDLE)
        set(NSXTOOL_BUNDLE_DIR nsxtool.app)

        set(NSX_INSTALL_DIR "${NSXTOOL_BUNDLE_DIR}/Contents/")
        set(NSXQT_INSTALL_DIR ".")

        set(MACOSX_BUNDLE_INFO_STRING "NSXTool: an application for reducing single-crystal diffraction data")
        set(MACOSX_BUNDLE_SHORT_VERSION_STRING "NSXTool version ${NSXTOOL_VERSION}")
        set(MACOSX_BUNDLE_LONG_VERSION_STRING "NSXTool version ${NSXTOOL_VERSION}")
        set(MACOSX_BUNDLE_GUI_IDENTIFIER "org.nsxtoolproject.NSXTool")
        set(MACOSX_BUNDLE_IDENTIFIER "org.nsxtoolproject")
        set(MACOSX_BUNDLE_ICON_FILE nsxtool.icns)
        set(MACOSX_BUNDLE_NAME nsxtool)

        set_source_files_properties(${CMAKE_SOURCE_DIR}/apps/nsxqt/resources/nsxtool.icns PROPERTIES MACOSX_PACKAGE_LOCATION Resources)

        configure_file("${CMAKE_SOURCE_DIR}/cmake/modules/MacOSXBundleInfo.plist.in" "${NSXTOOL_BUNDLE_DIR}/Info.plist" @ONLY)
    else()
        set(NSX_INSTALL_DIR "")
        set(NSXQT_INSTALL_DIR bin)
    endif()

elseif(WIN32)

    set(EXECUTABLE_OPTIONS WIN32)

    set(NSX_LIB_TYPE STATIC)

    set(NSX_INSTALL_DIR "")
    set(NSXQT_INSTALL_DIR bin)

elseif(UNIX)

    set(NSX_LIB_TYPE SHARED)

    set(NSX_INSTALL_DIR "")
    set(NSXQT_INSTALL_DIR bin)

endif()

if (NSX_BUILD_STATIC)
    set(NSX_LIB_TYPE STATIC)
endif()

