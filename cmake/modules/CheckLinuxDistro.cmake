find_program(LSB_RELEASE_CMD lsb_release)

if (NOT LSB_RELEASE_CMD)
  message(FATAL_ERROR "lsb_release not found")
endif()


execute_process(COMMAND ${LSB_RELEASE_CMD} -is OUTPUT_VARIABLE LINUX_DISTRO RESULT_VARIABLE result OUTPUT_STRIP_TRAILING_WHITESPACE)

# command failed
if (result)
  message(WARNING "Could not determine Linux distribution")
else()
  message(STATUS "The Linux distribution is ${LINUX_DISTRO}")
endif()
