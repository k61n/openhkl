# Install script for directory: /home/pellegrini/Downloads/eigen-eigen-67e894c6cd8f/unsupported/Eigen

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Devel")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/eigen3/unsupported/Eigen" TYPE FILE FILES
    "/home/pellegrini/Downloads/eigen-eigen-67e894c6cd8f/unsupported/Eigen/AdolcForward"
    "/home/pellegrini/Downloads/eigen-eigen-67e894c6cd8f/unsupported/Eigen/AlignedVector3"
    "/home/pellegrini/Downloads/eigen-eigen-67e894c6cd8f/unsupported/Eigen/ArpackSupport"
    "/home/pellegrini/Downloads/eigen-eigen-67e894c6cd8f/unsupported/Eigen/AutoDiff"
    "/home/pellegrini/Downloads/eigen-eigen-67e894c6cd8f/unsupported/Eigen/BVH"
    "/home/pellegrini/Downloads/eigen-eigen-67e894c6cd8f/unsupported/Eigen/EulerAngles"
    "/home/pellegrini/Downloads/eigen-eigen-67e894c6cd8f/unsupported/Eigen/FFT"
    "/home/pellegrini/Downloads/eigen-eigen-67e894c6cd8f/unsupported/Eigen/IterativeSolvers"
    "/home/pellegrini/Downloads/eigen-eigen-67e894c6cd8f/unsupported/Eigen/KroneckerProduct"
    "/home/pellegrini/Downloads/eigen-eigen-67e894c6cd8f/unsupported/Eigen/LevenbergMarquardt"
    "/home/pellegrini/Downloads/eigen-eigen-67e894c6cd8f/unsupported/Eigen/MatrixFunctions"
    "/home/pellegrini/Downloads/eigen-eigen-67e894c6cd8f/unsupported/Eigen/MoreVectorization"
    "/home/pellegrini/Downloads/eigen-eigen-67e894c6cd8f/unsupported/Eigen/MPRealSupport"
    "/home/pellegrini/Downloads/eigen-eigen-67e894c6cd8f/unsupported/Eigen/NonLinearOptimization"
    "/home/pellegrini/Downloads/eigen-eigen-67e894c6cd8f/unsupported/Eigen/NumericalDiff"
    "/home/pellegrini/Downloads/eigen-eigen-67e894c6cd8f/unsupported/Eigen/OpenGLSupport"
    "/home/pellegrini/Downloads/eigen-eigen-67e894c6cd8f/unsupported/Eigen/Polynomials"
    "/home/pellegrini/Downloads/eigen-eigen-67e894c6cd8f/unsupported/Eigen/Skyline"
    "/home/pellegrini/Downloads/eigen-eigen-67e894c6cd8f/unsupported/Eigen/SparseExtra"
    "/home/pellegrini/Downloads/eigen-eigen-67e894c6cd8f/unsupported/Eigen/SpecialFunctions"
    "/home/pellegrini/Downloads/eigen-eigen-67e894c6cd8f/unsupported/Eigen/Splines"
    )
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Devel")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/eigen3/unsupported/Eigen" TYPE DIRECTORY FILES "/home/pellegrini/Downloads/eigen-eigen-67e894c6cd8f/unsupported/Eigen/src" FILES_MATCHING REGEX "/[^/]*\\.h$")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("/home/pellegrini/Downloads/eigen-eigen-67e894c6cd8f/build/unsupported/Eigen/CXX11/cmake_install.cmake")

endif()

