###############################################################################
# Putting the Linux specific CPack stuff here
# Adapted from Mantid project (http://www.mantidproject.org/)
###############################################################################

set(NSXTOOL_VERSION_MAJOR 1)
set(NSXTOOL_VERSION_MINOR 0)

# Create a package file name for the Linux distributions
string(TOLOWER "${CPACK_PACKAGE_NAME}" CPACK_PACKAGE_NAME)

# define the source generators
set(CPACK_SOURCE_GENERATOR TGZ)

# Common description stuff
set(CPACK_PACKAGE_NAME "nsxtool")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Single crystal data reduction")
set(CPACK_PACKAGE_VENDOR "Institut Laue Langevin")
set(CPACK_PACKAGE_URL http://www.code.fr/scientific-software/nsxtool.git)
set(CPACK_PACKAGE_VERSION_MAJOR "${NSXTOOL_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${NSXTOOL_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION "${NSXTOOL_VERSION_MAJOR}.${NSXTOOL_VERSION_MINOR}")
set(CPACK_SOURCE_IGNORE_FILES "/\\\\.git/;" )
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/License.txt")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Neutron Single Crystal Diffraction Data Reduction")

if (${CMAKE_SYSTEM_NAME} STREQUAL "Linux")

    include(CheckLinuxDistro)

    # define which binary generators to use
    if ("${LINUX_DISTRO}" MATCHES "Ubuntu" OR "${LINUX_DISTRO}" MATCHES "Debian")
        find_program(DPKG_CMD dpkg)
        if (DPKG_CMD)
            # automatically generate dependencies
            set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
            set(CPACK_DEBIAN_PACKAGE_RELEASE 1)
            set(CPACK_DEBIAN_PACKAGE_MAINTAINER Eric Pellegrini <pellegrini@ill.fr>)
            set(CPACK_GENERATOR "DEB")
            execute_process(COMMAND ${DPKG_CMD} --print-architecture OUTPUT_VARIABLE CPACK_DEBIAN_PACKAGE_ARCHITECTURE OUTPUT_STRIP_TRAILING_WHITESPACE)
            set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}_${CPACK_PACKAGE_VERSION}-${CPACK_DEBIAN_PACKAGE_RELEASE}_${CPACK_DEBIAN_PACKAGE_ARCHITECTURE}")

            if (DEFINED NIGHTLY_BUILD)
                set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_FILE_NAME}-nightly")
            endif(DEFINED NIGHTLY_BUILD)

        else(DPKG_CMD)
            message(ERROR "dpkg command not installed")
        endif(DPKG_CMD) 
    endif()

    #RedHatEnterpriseClient RedHatEnterpriseWorkstation
    if ("${LINUX_DISTRO}" MATCHES "RedHatEnterprise" 
     OR "${LINUX_DISTRO}" MATCHES "Fedora" 
     OR "${LINUX_DISTRO}" MATCHES "SUSE LINUX"
     OR "${LINUX_DISTRO}" MATCHES "CentOS")

        find_program(RPMBUILD_CMD rpmbuild)

        if (RPMBUILD_CMD)
            
            set(CPACK_RPM_PACKAGE_LICENSE GPLv3+)
            set(CPACK_RPM_PACKAGE_RELEASE 1)
            set(CPACK_RPM_PACKAGE_GROUP Applications/Science)
        
            set(CPACK_GENERATOR "RPM")
            set(CPACK_RPM_PACKAGE_ARCHITECTURE "${CMAKE_SYSTEM_PROCESSOR}")
            set(CPACK_RPM_PACKAGE_URL "http://www.code.ill.fr/scientific-software/nsxtool.git")
        
            # reset the release name to include the RHEL version if known
            if ("${LINUX_DISTRO}" MATCHES "RedHatEnterprise")
                string(REGEX REPLACE "^([0-9])\\.[0-9]+$" "\\1" TEMP ${UNIX_RELEASE})
                set(CPACK_RPM_PACKAGE_RELEASE "1.el${TEMP}")
            elseif ("${UNIX_DIST}" MATCHES "Fedora")
                set(CPACK_RPM_PACKAGE_RELEASE "1.fc${UNIX_RELEASE}")
            endif ()
            
            # If CPACK_SET_DESTDIR is ON then the Prefix doesn't get put in the spec file
            if (CPACK_SET_DESTDIR)
                message ("Adding \"Prefix:\" line to spec file manually when CPACK_SET_DESTDIR is set")
                set(CPACK_RPM_SPEC_MORE_DEFINE "Prefix: ${CPACK_PACKAGING_INSTALL_PREFIX}")
            endif()
        
            # according to rpm.org: name-version-release.architecture.rpm
            set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-${CPACK_RPM_PACKAGE_RELEASE}.${CPACK_RPM_PACKAGE_ARCHITECTURE}")

        endif()

    endif ()

else(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")

    message(STATUS "Windows installers not available yet")

endif(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")

message(STATUS "CPACK_PACKAGE_FILE_NAME = ${CPACK_PACKAGE_FILE_NAME}")

set(CMAKE_INSTALL_RPATH "/usr/local/lib")

include(CPack)


