# Extra code to deploy DLL dependences to Windows.

include(CPackIFW)

# Transform .lib, as set by find_package commands, to .dll, then install
set(external_libs
    ${YAML_CPP_LIBRARIES}
    ${Blosc_LIBRARIES}
    ${TIFF_LIBRARIES}
    ${GSL_LIBRARIES}
    ${QHULL_LIBRARY1}
    #${QHULL_LIBRARY2
    )

foreach(ext ${external_libs})
    string(REGEX REPLACE "lib$" "dll" dll ${ext})
    string(REGEX REPLACE "lib" "bin" dll2 ${dll})
    if (EXISTS ${dll2})
    message(STATUS "for lib ${ext} install ${dll2}")
    install(FILES ${dll2} DESTINATION bin)
    endif()
endforeach()

set(thirdparty_libs
    "3rdparty/qcustomplot/qcustomplot.lib"
	)

foreach(third ${thirdparty_libs})
    set(path "${CMAKE_BINARY_DIR}/${third}")
    message(STATUS "for 3rd party lib ${third} install ${path}")
    install(FILES ${path} DESTINATION bin)
endforeach()

string(REGEX REPLACE "lib$" "dll" HDF5_DLL ${HDF5_LIB_PATH})
string(REGEX REPLACE "/lib/lib" "/bin/" HDF5_DLL_PATH ${HDF5_DLL})
message(STATUS "for lib ${HDF5_LIB_PATH} install ${HDF5_DLL_PATH}")
install(FILES ${HDF5_DLL_PATH} DESTINATION bin)

set(QtComponents Core Gui Widgets PrintSupport)
find_package(Qt5 REQUIRED COMPONENTS ${QtComponents})
foreach(comp ${QtComponents})
    set(QtComp Qt5::${comp})
    get_property(dll TARGET ${QtComp} PROPERTY IMPORTED_LOCATION_RELEASE)
    message(STATUS "for Qt lib ${QtComp} install ${dll}")
    install(FILES ${dll} DESTINATION bin)
endforeach()

set(QTDIR "C:/Qt/5.15.2/msvc2019_64")
install(FILES ${QTDIR}/plugins/platforms/qwindows.dll DESTINATION bin/platforms)
install(FILES ${QTDIR}/plugins/iconengines/qsvgicon.dll DESTINATION bin/iconengines)

set(CMAKE_INSTALL_SYSTEM_RUNTIME_COMPONENT System)
include(InstallRequiredSystemLibraries)
