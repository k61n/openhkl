find_program(LSB_RELEASE_CMD lsb_release)

if (NOT LSB_RELEASE_CMD)
  message(FATAL_ERROR "lsb_release not found")
endif()

execute_process(COMMAND ${LSB_RELEASE_CMD} -is OUTPUT_VARIABLE LINUX_DISTRO RESULT_VARIABLE result OUTPUT_STRIP_TRAILING_WHITESPACE)

if ("${LINUX_DISTRO}" MATCHES "RedHatEnterprise" 
 OR "${LINUX_DISTRO}" MATCHES "Fedora" 
 OR "${LINUX_DISTRO}" MATCHES "Scientific"
 OR "${LINUX_DISTRO}" MATCHES "SUSE LINUX"
 OR "${LINUX_DISTRO}" MATCHES "openSUSE project"
 OR "${LINUX_DISTRO}" MATCHES "CentOS")
    set(DEBIAN_LIKE false)
else()
    set(DEBIAN_LIKE true)
endif()

# command failed
if (result)
  message(WARNING "Could not determine Linux distribution")
else()
  message(STATUS "The Linux distribution is ${LINUX_DISTRO}")
endif()
