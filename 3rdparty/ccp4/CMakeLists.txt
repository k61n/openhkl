############################################################################
# CMakeLists.txt file for building mtzlib static library
############################################################################
set(library_name mtz)

set(src_files ccp4_array.c ccp4_parser.c ccp4_unitcell.c cmtzlib.c cvecmat.c library_err.c library_file.c library_utils.c)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_library(
    ${library_name}
    STATIC
    ${src_files}
)
set(${library_name}_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR} PARENT_SCOPE)
set(${library_name}_LIBRARY ${library_name} PARENT_SCOPE)

if(NOT WIN32)
    target_compile_options(${library_name} PRIVATE -Wno-deprecated-declarations)
endif()

target_link_libraries(${library_name})