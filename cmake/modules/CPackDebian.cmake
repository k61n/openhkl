set(CPACK_GENERATOR "DEB")

set(CPACK_DEBIAN_PACKAGE_NAME "${CPACK_PACKAGE_NAME}")

# parameters to build a debian package
set(CPACK_DEBIAN_PACKAGE_MAINTAINER Eric Pellegrini <pellegrini@ill.fr>)

# Architecture: (mandatory)
if (NOT CPACK_DEBIAN_PACKAGE_ARCHITECTURE)
    # There is no such thing as i686 architecture on debian, you should use i386 instead
    # $ dpkg --print-architecture
    find_program(DPKG_CMD dpkg)
    if(DPKG_CMD)
      execute_process(COMMAND "${DPKG_CMD}" --print-architecture OUTPUT_VARIABLE CPACK_DEBIAN_PACKAGE_ARCHITECTURE OUTPUT_STRIP_TRAILING_WHITESPACE)
    else()
        message(STATUS "Can not find dpkg in your path, default to i386.")
        set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE i386)
    endif()
endif()

set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
set(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")
set(CPACK_DEBIAN_PACKAGE_SECTION "devel")
set(CPACK_STRIP_FILES "TRUE")
set(CPACK_DEBIAN_PACKAGE_DESCRIPTION "${CPACK_PACKAGE_DESCRIPTION}")
set(CPACK_DEBIAN_PACKAGE_VERSION ${NSXTool_VERSION_PATCH})
set(CPACK_PACKAGE_FILE_NAME "${CPACK_DEBIAN_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}-${CPACK_DEBIAN_PACKAGE_VERSION}_${CPACK_DEBIAN_PACKAGE_ARCHITECTURE}")
set(CPACK_DEBIAN_PACKAGE_CONFLICTS "${CPACK_DEBIAN_PACKAGE_NAME}(<=${NSXTOOL_VERSION})")

# write copywrite file
file(GENERATE OUTPUT "${CMAKE_BINARY_DIR}/copyright" INPUT "${CMAKE_SOURCE_DIR}/LICENSE.txt")

install(FILES "${CMAKE_BINARY_DIR}/copyright" DESTINATION "share/${NSXTOOL_VERSION}")

# write changelog file
execute_process(COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_SOURCE_DIR}/CHANGELOG" "${CMAKE_BINARY_DIR}/changelog")

execute_process(COMMAND gzip -f -9 "${CMAKE_BINARY_DIR}/changelog")
install(FILES "${CMAKE_BINARY_DIR}/changelog.gz" DESTINATION "share/${NSXTOOL_VERSION}")

