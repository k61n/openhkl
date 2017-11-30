find_program(LSB_RELEASE_CMD lsb_release)

if (LSB_RELEASE_CMD)
    execute_process(COMMAND ${LSB_RELEASE_CMD} -is OUTPUT_VARIABLE LINUX_DISTRO OUTPUT_STRIP_TRAILING_WHITESPACE)
else (LSB_RELEASE_CMD)
    message(ERROR "lsb_release must be installed to define linux distribution type")
endif(LSB_RELEASE_CMD)
