set(app OpenHKL)

if(WIN32)
    add_executable(${app} WIN32 main.cpp resources.qrc)
elseif(APPLE)
    add_executable(${app} MACOSX_BUNDLE main.cpp resources.qrc)
else()
    add_executable(${app} main.cpp resources.qrc)
endif()

target_include_directories(${app}
    PRIVATE
    ${CMAKE_BINARY_DIR} # for manifest.h
    ${CMAKE_SOURCE_DIR}
    ${QT_INCLUDES}
    )

target_link_libraries(${app} PRIVATE ${QT_LIBS} openhklgui)

if(WIN32)
#    include(PackIFW)
endif()
