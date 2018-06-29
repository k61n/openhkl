if(NSX_PYTHON)
    if(USE_PYTHON3)
        set(MAC_PYTHON_VERSION "py3")
    else()
        set(MAC_PYTHON_VERSION "py2")
    endif()
else()
    message(FATAL_ERROR "NSX_PYTHON option must be set to ON for deploying om MacOS")
endif()

set(CPACK_PACKAGE_FILE_NAME "${CMAKE_PROJECT_NAME}-${NSXTOOL_VERSION}-${MAC_PYTHON_VERSION}-${NSXTOOL_ARCHITECTURE}")

set(CPACK_BINARY_DRAGNDROP ON)

set(CPACK_GENERATOR "DragNDrop")

