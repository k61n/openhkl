############################################################################
# CMakeLists.txt file for building qcustomplot static library
############################################################################
set(library_name qcustomplot)

set(source_files qcustomplot.h qcustomplot.cpp)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

add_library(
    ${library_name}
    STATIC
    ${source_files} ${mocfiles}
)
set(${library_name}_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR} PARENT_SCOPE)
set(${library_name}_LIBRARY ${library_name} PARENT_SCOPE)

if(NOT WIN32)
    target_compile_options(${library_name} PRIVATE -Wno-deprecated-declarations)
endif()

target_link_libraries(${library_name} ${QT_LIBS})

target_link_libraries(${library_name} ${QT_VER}::Widgets ${QT_VER}::PrintSupport)
