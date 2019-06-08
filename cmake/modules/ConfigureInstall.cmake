if(APPLE)
    set(NSX_LIB_TYPE SHARED)

    set(NSX_INSTALL_DIR "")
    set(NSXQT_INSTALL_DIR bin)

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

if(NSX_BUILD_STATIC)
    set(NSX_LIB_TYPE STATIC)
endif()

if(ENABLE_CPACK)

    if(APPLE)

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

    endif()

else()

    # Use, i.e. don't skip the full RPATH for the build tree
    set(CMAKE_SKIP_BUILD_RPATH  FALSE)
    # When building, don't use the install RPATH already (but later on when installing)
    set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
    # Disable the RPATH stripping during installation
    # See here for more explanations: https://stackoverflow.com/questions/32469953/why-is-cmake-designed-so-that-it-removes-runtime-path-when-installing
    set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

    # The RPATH to be used when installing, but only if it's not a system directory
    # See here for details https://gitlab.kitware.com/cmake/community/wikis/doc/cmake/RPATH-handling
    list(FIND CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES "${CMAKE_INSTALL_PREFIX}/lib" isSystemDir)
    IF("${isSystemDir}" STREQUAL "-1")
        set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
    endif("${isSystemDir}" STREQUAL "-1")

endif()
