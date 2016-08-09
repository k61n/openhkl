# Propose two build configurations
##set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "Compilation configuration" FORCE)
#if(DEFINED CMAKE_BUILD_TYPE)
#    set_property( CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug;Release" )
#endif()


#if(NOT CMAKE_BUILD_TYPE)
#  set(CMAKE_BUILD_TYPE "Release")
#endif()



# Set a default build type if none was specified
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to 'Release' as none was specified.")
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
  # Set the possible values of build type for cmake-gui
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release"
    "MinSizeRel" "RelWithDebInfo")
endif()
