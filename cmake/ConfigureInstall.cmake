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
