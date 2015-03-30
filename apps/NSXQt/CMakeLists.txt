include_directories(${CMAKE_SOURCE_DIR}/apps/NSXQt/include)

# Disable console on Windows
if(WIN32)
    set(GUI_TYPE WIN32)
endif(WIN32)

# Find the QtWidgets library
find_package(Qt5Core REQUIRED)
find_package(Qt5PrintSupport REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Gui REQUIRED)

# Add the include directories for the Qt 5 Widgets module to the compile lines.
include_directories(${Qt5Core_INCLUDE_DIRS})    
include_directories(${Qt5PrintSupport_INCLUDE_DIRS})    
include_directories(${Qt5Widgets_INCLUDE_DIRS})    
include_directories(${Qt5Gui_INCLUDE_DIRS})    

# Use the compile definitions defined in the Qt 5 Widgets module
add_definitions(${Qt5Core_DEFINITIONS})
add_definitions(${Qt5PrintSupport_DEFINITIONS})
add_definitions(${Qt5Widgets_DEFINITIONS})
add_definitions(${Qt5Gui_DEFINITIONS})

# Add compiler flags for building executables (-fPIE)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5Core_EXECUTABLE_COMPILE_FLAGS}")        
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5PrintSupport_EXECUTABLE_COMPILE_FLAGS}")        
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}")        
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5Gui_EXECUTABLE_COMPILE_FLAGS}")        

if (${CMAKE_VERSION} VERSION_LESS 2.8.11)

    if (${CMAKE_VERSION} VERSION_LESS 2.8.9)

        include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})    
    
        file(GLOB_RECURSE nsxqt_sources ${CMAKE_SOURCE_DIR}/apps/NSXQt/src/*.cpp)                   

        file(GLOB_RECURSE nsxqt_headers ${CMAKE_SOURCE_DIR}/apps/NSXQt/include/*.h)                   

        file(GLOB_RECURSE nsxqt_uis ${CMAKE_SOURCE_DIR}/apps/NSXQt/ui/*.ui)                   

        file(GLOB_RECURSE nsxqt_rcs ${CMAKE_SOURCE_DIR}/apps/NSXQt/*.qrc)                   
        
        qt5_wrap_ui(wrapped_ui_headers ${nsxqt_uis})

        qt5_wrap_cpp(wrapped_mocs ${nsxqt_headers})
        
        qt5_add_resources(added_resources ${nsxqt_rcs})
                
        add_executable(nsxqt ${GUI_TYPE} ${nsxqt_sources} ${nsxqt_headers} ${wrapped_ui_headers} ${wrapped_mocs} ${added_resources} )        
        
        # Link the nsxqt executable with the Qt5 libraries in use.
        target_link_libraries(nsxqt Qt5::Core Qt5::PrintSupport Qt5::Gui Qt5::Widgets NSXTool)        
        
    else()
    
    endif()
    
else ()

    # Find includes in corresponding build directories
    set(CMAKE_INCLUDE_CURRENT_DIR ON)

    # Instruct CMake to run moc automatically when needed.
    set(CMAKE_AUTOMOC ON)
    set(CMAKE_AUTOUIC ON)
    
    qt5_wrap_ui(nsxqt_ui mainwindow.ui) 
    
    # Tell CMake to create the nsxqt executable
    add_executable(nsxqt WIN32 ${CMAKE_SOURCE_DIR}/apps/NSXQt/src/Main.cpp ${CMAKE_SOURCE_DIR}/apps/NSXQt/src/MainWindow.cpp ${nsxqt_ui})
    # Use the Widgets module from Qt 5.
    target_link_libraries(nsxqt NSXTool Qt5::Widgets)
    
endif()

