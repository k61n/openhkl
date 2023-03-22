set(app OpenHKL)

if(WIN32)
    add_executable(${app} WIN32 main.cpp resources.qrc)
elseif(APPLE)
    set(MACOSX_BUNDLE_ICON_FILE OpenHKL.icns)
    set(APP_ICON_MACOSX ${CMAKE_CURRENT_SOURCE_DIR}/images/icons/${MACOSX_BUNDLE_ICON_FILE})
    set_source_files_properties(${APP_ICON_MACOSX} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
    add_executable(${app} MACOSX_BUNDLE main.cpp ${APP_ICON_MACOSX} resources.qrc)
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
