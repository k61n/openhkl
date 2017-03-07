
include_directories(
  ${CMAKE_BINARY_DIR}/include/
  ../externals/c-blosc/blosc
)

# note: *.c needed for blosc files in data directory
file(GLOB_RECURSE sources *.cpp *.c)
file(GLOB_RECURSE headers *.h)

# Creating the library
# Under Windows and Miscrosoft Visual Studio the library will be static to avoid
# the DLL_EXPORT mess
if(MSVC OR NSX_BUILD_STATIC)
  add_library(nsx STATIC ${sources} ${headers})
else()
  add_library(nsx SHARED ${sources} ${headers})
endif()

foreach(path ${CMAKE_PREFIX_PATH})
  message("Path: " ${path})
endforeach(path)

if(MSVC)
  target_link_libraries(nsx
    ${HDF5_LIBRARIES}
    blosc_static
    ${Boost_LIBRARIES}
    ${PYTHON_LIBRARIES}
    ${FFTW_LIBRARIES}
    ${TIFF_LIBRARY}
    ${GSL_LIBRARIES}
    )
else()
  target_link_libraries(nsx
    ${HDF5_LIBRARIES}
    blosc_shared
    ${Boost_LIBRARIES}
    ${PYTHON_LIBRARIES}
    ${FFTW_LIBRARIES}
    ${TIFF_LIBRARY}
    ${GSL_LIBRARIES}
    )
endif()

# Install section, installing libraries
install(TARGETS nsx DESTINATION lib)
set(INCLUDE_DIR "${CMAKE_INSTALL_PREFIX}/include/nsxlib/")
file(GLOB_RECURSE all_headers *.h)
install(CODE "MESSAGE(\"Installing headers...\")")
install(FILES ${headers} DESTINATION ${INCLUDE_DIR})

message("FROM NSXLIB: GSL_LIBRARIES is ${GSL_LIBRARIES}")

if(NSX_TIDY)
  add_tidy_target(nsx)
endif()
