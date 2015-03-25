include_directories(${CMAKE_SOURCE_DIR}/apps/NSXQt/include)

file(GLOB_RECURSE nsxqt_sources apps/NSXQt/*.cpp)                   

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

# Find the QtWidgets library
find_package(Qt5Widgets)

# Tell CMake to create the nsxqt executable
add_executable(nsxqt WIN32 ${CMAKE_SOURCE_DIR}/apps/NSXQt/src/Main.cpp)

# Use the Widgets module from Qt 5.
target_link_libraries(nsxqt NSXTool Qt5::Widgets)