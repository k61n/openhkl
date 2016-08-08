if(POLICY CMP0020)
    cmake_policy(SET CMP0020 NEW)
endif()

# Add NSXQt and NSXQt directories to the include path
include_directories(
    ${CMAKE_SOURCE_DIR}/nsxlib/chemistry/
    ${CMAKE_SOURCE_DIR}/nsxlib/crystal/
    ${CMAKE_SOURCE_DIR}/nsxlib/data/
    ${CMAKE_SOURCE_DIR}/nsxlib/geometry/
    ${CMAKE_SOURCE_DIR}/nsxlib/imaging/
    ${CMAKE_SOURCE_DIR}/nsxlib/instrument/
    ${CMAKE_SOURCE_DIR}/nsxlib/kernel/
    ${CMAKE_SOURCE_DIR}/nsxlib/utils/
  ${CMAKE_SOURCE_DIR}/apps/NSXQt/absorption/
  ${CMAKE_SOURCE_DIR}/apps/NSXQt/GraphicsItems/
  ${CMAKE_SOURCE_DIR}/apps/NSXQt/OpenGL/
  ${CMAKE_SOURCE_DIR}/apps/NSXQt/plot/
  ${CMAKE_SOURCE_DIR}/apps/NSXQt/Tree/
  ${CMAKE_SOURCE_DIR}/apps/NSXQt/views/
  ${CMAKE_SOURCE_DIR}/apps/NSXQt/dialogs/
  ${CMAKE_SOURCE_DIR}/apps/NSXQt/models/
  ${CMAKE_SOURCE_DIR}/apps/NSXQt/chemistry/
  ${CMAKE_SOURCE_DIR}/apps/NSXQt/
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
#  ${CMAKE_SOURCE_DIR}/apps/NSXQt/absorption/*.h
#  ${CMAKE_SOURCE_DIR}/apps/NSXQt/GraphicsItems/*.h
#  ${CMAKE_SOURCE_DIR}/apps/NSXQt/OpenGL/*.h
#  ${CMAKE_SOURCE_DIR}/apps/NSXQt/plot/*.h
#  ${CMAKE_SOURCE_DIR}/apps/NSXQt/Tree/*.h
#  ${CMAKE_SOURCE_DIR}/apps/NSXQt/dialogs/*.h
#  ${CMAKE_SOURCE_DIR}/apps/NSXQt/views/*.h
#  ${CMAKE_SOURCE_DIR}/apps/NSXQt/models/*.h
#  ${CMAKE_SOURCE_DIR}/apps/NSXQt/chemistry/*.h
  ${CMAKE_SOURCE_DIR}/apps/NSXQt/*.h
  )



# Find recursively all the cpp source files of NSXQt
file(GLOB_RECURSE nsxqt_sources
#    ${CMAKE_SOURCE_DIR}/apps/NSXQt/absorption/*.cpp
#    ${CMAKE_SOURCE_DIR}/apps/NSXQt/GraphicsItems/*.cpp
#    ${CMAKE_SOURCE_DIR}/apps/NSXQt/OpenGL/*.cpp
#    ${CMAKE_SOURCE_DIR}/apps/NSXQt/plot/*.cpp
#    ${CMAKE_SOURCE_DIR}/apps/NSXQt/Tree/*.cpp
#    ${CMAKE_SOURCE_DIR}/apps/NSXQt/dialogs/*.cpp
#    ${CMAKE_SOURCE_DIR}/apps/NSXQt/views/*.cpp
#    ${CMAKE_SOURCE_DIR}/apps/NSXQt/models/*.cpp
#    ${CMAKE_SOURCE_DIR}/apps/NSXQt/chemistry/*.cpp
    ${CMAKE_SOURCE_DIR}/apps/NSXQt/*.cpp
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
file(GLOB_RECURSE nsxqt_uis
  ${CMAKE_SOURCE_DIR}/apps/NSXQt/ui/*.ui
  ${CMAKE_SOURCE_DIR}/apps/NSXQt/ui/Absorption/*.ui
  ${CMAKE_SOURCE_DIR}/apps/NSXQt/ui/Chemistry/*.ui
  ${CMAKE_SOURCE_DIR}/apps/NSXQt/ui/Tree/*.ui
  )                                      
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


set(qmake_sources "")
set(qmake_headers "")
set(qmake_uis "")
set(qmake_include_dirs "")

foreach(source_file ${nsxqt_sources})
  set(qmake_sources "${qmake_sources} ${source_file}")
endforeach(source_file)

foreach(source_file ${nsxqt_headers})
  set(qmake_headers "${qmake_headers} ${source_file}")
endforeach(source_file)

foreach(source_file ${nsxqt_uis})
  set(qmake_uis "${qmake_uis} ${source_file}")
endforeach(source_file)

get_property(dirs DIRECTORY PROPERTY INCLUDE_DIRECTORIES)
foreach(dir ${dirs})
  if (NOT ${dir} STREQUAL "/usr/include")
    set(qmake_include_dirs "${qmake_include_dirs} ${dir}")
  endif()
endforeach(dir)

#message("------------------------")
#message("${qmake_sources}")
#message("------------------------")

#message("------------------------")
#message("${nsxqt_sources}")
#message("------------------------")


# generate the qmake project file automatically
configure_file(NSXQt.pro.in ${CMAKE_CURRENT_BINARY_DIR}/NSXQt.pro)



