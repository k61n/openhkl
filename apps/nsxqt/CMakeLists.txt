if(POLICY CMP0020)
  cmake_policy(SET CMP0020 NEW)
endif()

if(POLICY CMP0042)
    cmake_policy(SET CMP0042 NEW)
endif()

if(POLICY CMP0071)
  cmake_policy(SET CMP0071 NEW)
endif()

include(SetupQt)

get_subdirectories(NSXQT_SUBDIRS ${CMAKE_CURRENT_SOURCE_DIR})

# add project root directory to include path
include_directories(${CMAKE_CURRENT_SOURCE_DIR}
                    ${NSXQT_SUBDIRS}
                    ${CMAKE_BINARY_DIR}/nsxlib/include)

# Build the nsxqt executable
add_executable(${NSXQT_EXECUTABLE} ${EXECUTABLE_OPTIONS} ${NSXQT_SOURCES} ${NSXQT_RCS} ${NSXQT_RESOURCES_CPP} ${NSXQT_HEADERS} ${NSXQT_UI_HEADERS})

# Link the nsxqt executable with the Qt5 libraries in use.
target_link_libraries(${NSXQT_EXECUTABLE} ${QT_LIBRARIES} ${OPENGL_LIBRARIES} ${NSX_LIBRARY} ${HDF5_LIBRARIES})

set_target_properties(${NSXQT_EXECUTABLE} PROPERTIES LINK_FLAGS ${NSXQT_LINK_FLAGS})

if(APPLE AND ENABLE_CPACK)
    set_target_properties(${NSXQT_EXECUTABLE} PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${NSXTOOL_BUNDLE_DIR}/Info.plist)

endif()


# Install section, installing libraries
install(TARGETS ${NSXQT_EXECUTABLE} ${NSXTOOL_TARGET} DESTINATION ${NSXQT_INSTALL_DIR} COMPONENT Applications)

string(REPLACE ";" "\\\n" NSXQT_SOURCES "${NSXQT_SOURCES}") 
string(REPLACE ";" "\\\n" NSXQT_HEADERS "${NSXQT_HEADERS}")
string(REPLACE ";" "\\\n" NSXQT_UIS "${NSXQT_UIS}")

get_property(NSXQT_QMAKE_INCLUDE_DIRECTORIES DIRECTORY PROPERTY INCLUDE_DIRECTORIES)
string(REPLACE ";" "\\\n" NSXQT_QMAKE_INCLUDE_DIRECTORIES "${NSXQT_QMAKE_INCLUDE_DIRECTORIES}")

# generate the qmake project file automatically
configure_file(NSXQt.pro.in ${CMAKE_CURRENT_BINARY_DIR}/NSXQt.pro)

# tidy
if(NSX_TIDY)
  add_tidy_target(nsxqt)
endif()
