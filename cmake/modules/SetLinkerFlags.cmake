if(APPLE)
    if(ENABLE_CPACK)
        # this makes sense only if we build a bundle
        set(NSXQT_LINK_FLAGS "-Wl,-rpath,@loader_path/../Frameworks/ -Wl,-rpath,@loader_path/../lib/")
    else()
        set(NSXQT_LINK_FLAGS "-Wl,-rpath,@loader_path/../../nsxlib/")
    endif()
elseif(UNIX)
    set(NSXQT_LINK_FLAGS "-Wl,-rpath,@loader_path/../lib")
endif()

