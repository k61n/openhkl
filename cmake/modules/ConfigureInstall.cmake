if (APPLE)
    set(NSX_LIB_TYPE SHARED)

    if(ENABLE_CPACK)
        set(EXECUTABLE_OPTIONS MACOSX_BUNDLE)

        set(NSXTOOL_TARGET BUNDLE)
        set(NSXTOOL_BUNDLE_DIR nsxqt.app)

        set(NSX_INSTALL_DIR "${NSXTOOL_BUNDLE_DIR}/Contents/")
        set(NSXQT_INSTALL_DIR ".")

        set(NSXQT_BUNDLE_RESOURCES ${CMAKE_SOURCE_DIR}/cmake/modules/qt.conf
                                   ${CMAKE_SOURCE_DIR}/apps/nsxqt/resources/nsxtool.icns)

        set(MACOSX_BUNDLE_INFO_STRING "NSXTool: an application for reducing single-crystal diffraction data")
        set(MACOSX_BUNDLE_SHORT_VERSION_STRING "NSXTool version ${NSXTOOL_VERSION}")
        set(MACOSX_BUNDLE_LONG_VERSION_STRING "NSXTool version ${NSXTOOL_VERSION}")
        set(MACOSX_BUNDLE_GUI_IDENTIFIER "org.nsxtoolproject.NSXTool")
        set(MACOSX_BUNDLE_IDENTIFIER "org.nsxtoolproject")
        set(MACOSX_BUNDLE_ICON_FILE nsxtool.icns)
        set(MACOSX_BUNDLE_NAME nsxqt)
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

