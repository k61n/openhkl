if(POLICY CMP0020)
    cmake_policy(SET CMP0020 NEW)
endif()

# Add NSXQt and NSXQt directories to the include path
include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${CMAKE_CURRENT_SOURCE_DIR}/models
  ${CMAKE_CURRENT_SOURCE_DIR}/views
  ${CMAKE_CURRENT_BINARY_DIR}
  )

# Disable console on Windows
if(WIN32)
    set(GUI_TYPE WIN32)
endif(WIN32)

# Find a few Qt submodules
find_package(Qt5Core REQUIRED)
find_package(Qt5PrintSupport REQUIRED)
find_package(Qt5OpenGL REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Gui REQUIRED)

#Find OpenGL 
find_package(OpenGL REQUIRED)

# Find recursively all the header files of NSXQt
file(GLOB_RECURSE nsxqt_headers
  ${CMAKE_SOURCE_DIR}/apps/NSXQt/include/*.h
  ${CMAKE_SOURCE_DIR}/apps/NSXQt/views/*.h
  ${CMAKE_SOURCE_DIR}/apps/NSXQt/models/*.h
  ${CMAKE_SOURCE_DIR}/apps/NSXQt/chemistry/*.h
  )



# Find recursively all the cpp source files of NSXQt
file(GLOB_RECURSE nsxqt_sources
  ${CMAKE_SOURCE_DIR}/apps/NSXQt/src/*.cpp
  ${CMAKE_SOURCE_DIR}/apps/NSXQt/views/*.cpp
  ${CMAKE_SOURCE_DIR}/apps/NSXQt/models/*.cpp
  ${CMAKE_SOURCE_DIR}/apps/NSXQt/chemistry/*.cpp
  )                   

if (${CMAKE_VERSION} VERSION_LESS 2.8.11)

	# Add the include directories for the Qt 5 Widgets module to the compile lines.
	# There might be some redundancies.
	include_directories(${Qt5Core_INCLUDE_DIRS})    
	include_directories(${Qt5PrintSupport_INCLUDE_DIRS})    
	include_directories(${Qt5OpenGL_INCLUDE_DIRS})    
	include_directories(${Qt5Widgets_INCLUDE_DIRS})    
	include_directories(${Qt5Gui_INCLUDE_DIRS})    
	
	# Use the compile definitions defined in the Qt 5 Widgets module
	# There might be some redundancies. 
	add_definitions(${Qt5Core_DEFINITIONS})
	add_definitions(${Qt5PrintSupport_DEFINITIONS})
	add_definitions(${Qt5OpenGL_DEFINITIONS})
	add_definitions(${Qt5Widgets_DEFINITIONS})
	add_definitions(${Qt5Gui_DEFINITIONS})
	
	# Add compiler flags for building executables (i.e. -fPIE)
	# There might be some redundancies. 
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5Core_EXECUTABLE_COMPILE_FLAGS}")        
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5PrintSupport_EXECUTABLE_COMPILE_FLAGS}")        
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5OpenGL_EXECUTABLE_COMPILE_FLAGS}")        
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}")        
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5Gui_EXECUTABLE_COMPILE_FLAGS}")        
    
    # Generate the Qt moc files
    # All the sources files that are not Qt Q_OBJECT will trigger a warning and won't be compiled as moc objects. 
    qt5_wrap_cpp(mocs ${nsxqt_headers})
    
    # Append the moc cpp files to the NSXQt sources
    list(APPEND nsxqt_sources ${mocs})
                                
else ()

    # Find includes in corresponding build directories
    set(CMAKE_INCLUDE_CURRENT_DIR ON)

    # Handle automatically moc files
    set(CMAKE_AUTOMOC ON)
    # Handle automatically ui files
    set(CMAKE_AUTOUIC OFF)
    # Specify that the app will use OpenGL
    set(QT_USE_QTOPENGL TRUE)
    # Handle automatically resources files
    set(CMAKE_AUTORCC ON)
        
endif()

# Find recursively all the Qt ui files of NSXQt
file(GLOB_RECURSE nsxqt_uis ${CMAKE_SOURCE_DIR}/apps/NSXQt/ui/*.ui)                   
# Build the headers files for all Qt ui files
qt5_wrap_ui(ui_headers ${nsxqt_uis})

# Find recursively all the Qt resources files of NSXQt
file(GLOB_RECURSE nsxqt_rcs ${CMAKE_SOURCE_DIR}/apps/NSXQt/*.qrc)                   
# "Compile" the resources files
qt5_add_resources(resources ${nsxqt_rcs})

# For info use 'objdump -x nsxqt | grep RPATH' to the installed executable (not the built one) 
# to check that the RPATH is correctly set

# Use, i.e. don't skip the full RPATH for the build tree
set(CMAKE_SKIP_BUILD_RPATH  FALSE)
# When building, don't use the install RPATH already (but later on when installing)
set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE) 
# Set the install RPATH
set(CMAKE_INSTALL_RPATH ${CMAKE_INSTALL_PREFIX}/lib)

# Build the nsxqt executable
add_executable(nsxqt ${GUI_TYPE} ${nsxqt_sources} ${nsxqt_headers} ${ui_headers} ${resources})        

# Link the nsxqt executable with the Qt5 libraries in use.
target_link_libraries(nsxqt Qt5::Core Qt5::PrintSupport Qt5::Gui Qt5::Widgets Qt5::OpenGL ${OPENGL_LIBRARIES} NSXTool ${HDF5_LIBRARIES})        

# Install section, installing libraries 
install(TARGETS nsxqt DESTINATION bin)

