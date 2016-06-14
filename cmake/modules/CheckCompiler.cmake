#---------------------------------------------------------------------------------------------------
#  CheckCompiler.cmake
#---------------------------------------------------------------------------------------------------

 
if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR
    CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  set(MSVC TRUE)
else()
  message(WARNING "C++ compiler not recognized")
endif()

message(STATUS "Using compiler `${CMAKE_CXX_COMPILER_ID}` with flags ${CMAKE_CXX_FLAGS}")
