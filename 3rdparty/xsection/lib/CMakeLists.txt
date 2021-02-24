set(lib XSection)

file(GLOB sources *.cpp)

set(installable_includes
    ChemicalFormulaParser.h IsotopeDatabaseManager.h Material.h Variant.h ISingleton.h)

add_library(${lib} ${sources})

target_include_directories(${lib} PRIVATE ${CMAKE_SOURCE_DIR} ${Boost_INCLUDE_DIRS})

target_link_libraries(${lib} PRIVATE ${YAMLCPP_LIBRARIES})

set_target_properties(
    ${lib} PROPERTIES
    OUTPUT_NAME ${lib}
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR})

install(
    TARGETS ${lib} LIBRARY
    DESTINATION ${destination}/lib
    COMPONENT Libraries)
install(
    FILES ${installable_includes}
    DESTINATION ${destination}/include/xsection
    COMPONENT Headers)
