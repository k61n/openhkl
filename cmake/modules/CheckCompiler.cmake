#---------------------------------------------------------------------------------------------------
#  CheckCompiler.cmake
#---------------------------------------------------------------------------------------------------

 
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  add_definitions( -std=c++11)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  set(MSVC TRUE)
else()
  message(WARNING "C++ compiler not recognized")
endif()

message(STATUS "Using compiler `${CMAKE_CXX_COMPILER_ID}` with flags ${CMAKE_CXX_FLAGS}")
