###############################################################################
# Putting all the common CPack stuff in one place
# Adapted from Mantid project (http://www.mantidproject.org/)
###############################################################################

# Common description stuff
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Single crystal data reduction")
set(CPACK_PACKAGE_VENDOR "Institut Laue Langevin")
set(CPACK_PACKAGE_URL http://www.ill.eu/fr/instruments-support/instruments-groups/groups/dif/ )
set(CPACK_DEBIAN_PACKAGE_MAINTAINER Eric Pellegrini <nsxtool-help@nsxtool.ill.eu>)
set(CPACK_PACKAGE_VERSION "0.0.0")
set(CPACK_PACKAGE_VERSION_MAJOR "0")
set(CPACK_PACKAGE_VERSION_MINOR "0")
set(CPACK_PACKAGE_VERSION_PATCH "0")

if(NOT ${CMAKE_SYSTEM_NAME} STREQUAL "Windows") # To avoid breaking Windows vates packaging
    set(CPACK_PACKAGING_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX})
endif()

# RPM information - only used if generating a rpm
set(CPACK_RPM_PACKAGE_LICENSE GPLv3+)
set(CPACK_RPM_PACKAGE_RELEASE 1)
set(CPACK_RPM_PACKAGE_GROUP Applications/Engineering)

# DEB information - only used if generating a deb
set(CPACK_DEBIAN_PACKAGE_RELEASE 1)
