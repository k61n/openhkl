# Install script for directory: /home/pellegrini/Downloads/eigen-eigen-67e894c6cd8f/Eigen

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
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/eigen3/Eigen" TYPE FILE FILES
    "/home/pellegrini/Downloads/eigen-eigen-67e894c6cd8f/Eigen/MetisSupport"
    "/home/pellegrini/Downloads/eigen-eigen-67e894c6cd8f/Eigen/StdList"
    "/home/pellegrini/Downloads/eigen-eigen-67e894c6cd8f/Eigen/Geometry"
    "/home/pellegrini/Downloads/eigen-eigen-67e894c6cd8f/Eigen/StdDeque"
    "/home/pellegrini/Downloads/eigen-eigen-67e894c6cd8f/Eigen/OrderingMethods"
    "/home/pellegrini/Downloads/eigen-eigen-67e894c6cd8f/Eigen/CholmodSupport"
    "/home/pellegrini/Downloads/eigen-eigen-67e894c6cd8f/Eigen/Sparse"
    "/home/pellegrini/Downloads/eigen-eigen-67e894c6cd8f/Eigen/QR"
    "/home/pellegrini/Downloads/eigen-eigen-67e894c6cd8f/Eigen/SVD"
    "/home/pellegrini/Downloads/eigen-eigen-67e894c6cd8f/Eigen/SparseQR"
    "/home/pellegrini/Downloads/eigen-eigen-67e894c6cd8f/Eigen/SparseCore"
    "/home/pellegrini/Downloads/eigen-eigen-67e894c6cd8f/Eigen/Cholesky"
    "/home/pellegrini/Downloads/eigen-eigen-67e894c6cd8f/Eigen/SparseCholesky"
    "/home/pellegrini/Downloads/eigen-eigen-67e894c6cd8f/Eigen/SuperLUSupport"
    "/home/pellegrini/Downloads/eigen-eigen-67e894c6cd8f/Eigen/Dense"
    "/home/pellegrini/Downloads/eigen-eigen-67e894c6cd8f/Eigen/UmfPackSupport"
    "/home/pellegrini/Downloads/eigen-eigen-67e894c6cd8f/Eigen/SparseLU"
    "/home/pellegrini/Downloads/eigen-eigen-67e894c6cd8f/Eigen/QtAlignedMalloc"
    "/home/pellegrini/Downloads/eigen-eigen-67e894c6cd8f/Eigen/Jacobi"
    "/home/pellegrini/Downloads/eigen-eigen-67e894c6cd8f/Eigen/StdVector"
    "/home/pellegrini/Downloads/eigen-eigen-67e894c6cd8f/Eigen/Core"
    "/home/pellegrini/Downloads/eigen-eigen-67e894c6cd8f/Eigen/SPQRSupport"
    "/home/pellegrini/Downloads/eigen-eigen-67e894c6cd8f/Eigen/Eigen"
    "/home/pellegrini/Downloads/eigen-eigen-67e894c6cd8f/Eigen/LU"
    "/home/pellegrini/Downloads/eigen-eigen-67e894c6cd8f/Eigen/Householder"
    "/home/pellegrini/Downloads/eigen-eigen-67e894c6cd8f/Eigen/IterativeLinearSolvers"
    "/home/pellegrini/Downloads/eigen-eigen-67e894c6cd8f/Eigen/PardisoSupport"
    "/home/pellegrini/Downloads/eigen-eigen-67e894c6cd8f/Eigen/Eigenvalues"
    "/home/pellegrini/Downloads/eigen-eigen-67e894c6cd8f/Eigen/PaStiXSupport"
    )
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Devel")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/eigen3/Eigen" TYPE DIRECTORY FILES "/home/pellegrini/Downloads/eigen-eigen-67e894c6cd8f/Eigen/src" FILES_MATCHING REGEX "/[^/]*\\.h$")
endif()

