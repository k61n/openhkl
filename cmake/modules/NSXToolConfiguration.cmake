if(APPLE)
    set(NSX_LIB_TYPE SHARED)
elseif(WIN32)
    set(NSX_LIB_TYPE STATIC)
else()
    set(NSX_LIB_TYPE SHARED)
endif()

if (NSX_BUILD_STATIC)
    set(NSX_LIB_TYPE STATIC)
endif()

set(NSX_LIBRARY nsx)
set(NSXQT_EXECUTABLE nsxqt)

