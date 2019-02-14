if(POLICY CMP0020)
  cmake_policy(SET CMP0020 NEW)
endif()

if(POLICY CMP0042)
    cmake_policy(SET CMP0042 NEW)
endif()

if(POLICY CMP0071)
  cmake_policy(SET CMP0071 NEW)
endif()

get_subdirectories(NSXQT_SUBDIRS ${CMAKE_CURRENT_SOURCE_DIR})

# Find recursively all the header files of NSXQt
file(GLOB_RECURSE NSXQT_HEADERS ${CMAKE_SOURCE_DIR}/apps/nsxqt/*.h)

# Find recursively all the cpp source files of NSXQt
file(GLOB_RECURSE NSXQT_SOURCES ${CMAKE_SOURCE_DIR}/apps/nsxqt/*.cpp)

# Find recursively all the Qt ui files of NSXQt
file(GLOB_RECURSE NSXQT_UIS ${CMAKE_SOURCE_DIR}/apps/nsxqt/*.ui)

# Build the headers files for all Qt ui files
qt5_wrap_ui(NSXQT_UI_HEADERS ${NSXQT_UIS})

# Find recursively all the Qt resources files of NSXQt
file(GLOB_RECURSE NSXQT_RCS *.qrc)

get_directory_property(PYNSX_INCLUDE_DIRECTORIES INCLUDE_DIRECTORIES)
list(REMOVE_DUPLICATES PYNSX_INCLUDE_DIRECTORIES)

get_subdirectories(NSXQT_SUBDIRS ${CMAKE_CURRENT_SOURCE_DIR})

if(APPLE AND ENABLE_CPACK)
    set_source_files_properties(${NSXQT_BUNDLE_RESOURCES} PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
    configure_file("${CMAKE_SOURCE_DIR}/cmake/modules/MacOSXBundleInfo.plist.in" "${CMAKE_CURRENT_BINARY_DIR}/Info.plist" @ONLY)
endif()

# Build the nsxqt executable
add_executable(
    ${NSXQT_EXECUTABLE}
    ${EXECUTABLE_OPTIONS}
    ${NSXQT_SOURCES}
    ${NSXQT_RCS}
    ${NSXQT_RESOURCES_CPP}
    ${NSXQT_HEADERS}
    ${NSXQT_UI_HEADERS}
    ${NSXQT_BUNDLE_RESOURCES})

target_include_directories(
    ${NSXQT_EXECUTABLE}
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/3rdparty/QCustomPlot
    ${NSXQT_SUBDIRS}
    ${CMAKE_BINARY_DIR}/core/include)

target_link_libraries(
    ${NSXQT_EXECUTABLE}
    ${QT_LIBRARIES}
    QCustomPlot
    ${OPENGL_LIBRARIES}
    ${NSX_LIBRARY})

if(ENABLE_CPACK)
    if(APPLE)
        set_target_properties(${NSXQT_EXECUTABLE} PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_BINARY_DIR}/Info.plist)
        set_target_properties(${NSXQT_EXECUTABLE} PROPERTIES INSTALL_RPATH "@loader_path/../Frameworks;@loader_path/../lib")

        configure_file(${CMAKE_SOURCE_DIR}/cmake/modules/FixAppleBundle.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/FixAppleBundle.cmake @ONLY)
        install(SCRIPT ${CMAKE_CURRENT_BINARY_DIR}/FixAppleBundle.cmake COMPONENT Runtime)
    elseif(LINUX)
        set_target_properties(${NSXQT_EXECUTABLE} PROPERTIES INSTALL_RPATH "\$ORIGIN/../lib")
    endif()
endif()


# Install section, installing libraries
install(TARGETS ${NSXQT_EXECUTABLE} ${NSXTOOL_TARGET} DESTINATION ${NSXQT_INSTALL_DIR} COMPONENT APPLICATIONS)

string(REPLACE ";" "\\\n" NSXQT_SOURCES "${NSXQT_SOURCES}")
string(REPLACE ";" "\\\n" NSXQT_HEADERS "${NSXQT_HEADERS}")
string(REPLACE ";" "\\\n" NSXQT_UIS "${NSXQT_UIS}")

get_property(NSXQT_QMAKE_INCLUDE_DIRECTORIES DIRECTORY PROPERTY INCLUDE_DIRECTORIES)
string(REPLACE ";" "\\\n" NSXQT_QMAKE_INCLUDE_DIRECTORIES "${NSXQT_QMAKE_INCLUDE_DIRECTORIES}")

# generate the qmake project file automatically
configure_file(nsxqt.pro.in ${CMAKE_CURRENT_BINARY_DIR}/nsxqt.pro)

# tidy
if(NSX_TIDY)
  add_tidy_target(nsxqt)
endif()
