# ClangTidy.cmake
#
# description:
#   given a target, generate a new custom target to run clang-tidy
#   on the target's sources
#
# usage:
#   in cmake scripts, for a target 'targ' run the additional command
#     add_tidy_target(targ)
#
#   and from the build directory, run the tidy command as
#        cmake --build . --target targ-tidy
#
# author: Jonathan Fisher
#         j.fisher@fz-juelich.de, jonathan.m.fisher@gmail.com
#
# copyright: 2017 scientific computing group, Forschungszentrum Juelich GmbH
#
# license: see LICENSE
#

find_program(TIDY_COMMAND clang-tidy)
# note: CMAKE_EXPORT_COMPILE_COMMANDS must be ON!

if(TIDY_COMMAND)
    message(STATUS "Found tidy command: ${TIDY_COMMAND}")
else()
    message(FATAL "Could not find tidy command")
endif()

function(add_tidy_target targetname)
    get_property(sources TARGET ${targetname} PROPERTY SOURCES)

    add_custom_target(${targetname}-tidy)
    foreach(source ${sources})
        add_custom_command(
            TARGET ${targetname}-tidy
            COMMAND ${TIDY_COMMAND} ARGS -checks=* ${source} -p=${CMAKE_BINARY_DIR}
            VERBATIM
            )
    endforeach()

    message(STATUS "Added tidy target ${targetname}-tidy")
endfunction()
