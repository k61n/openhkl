###########################################################################
# Determine the version of OS X that we are running
###########################################################################

# Set the system name (and remove the space)
execute_process(
      COMMAND /usr/bin/sw_vers -productVersion
      OUTPUT_VARIABLE OSX_VERSION
      RESULT_VARIABLE OSX_VERSION_STATUS
  )
# Strip off any /CR or /LF
string(STRIP ${OSX_VERSION} OSX_VERSION)

if (OSX_VERSION VERSION_LESS 10.9)
  message (FATAL_ERROR "The minimum supported version of Mac OS X is 10.9 (Mavericks).")
endif()

if (OSX_VERSION VERSION_GREATER 10.9 OR OSX_VERSION VERSION_EQUAL 10.9)
  set ( OSX_CODENAME "Mavericks")
endif()

if (OSX_VERSION VERSION_GREATER 10.10 OR OSX_VERSION VERSION_EQUAL 10.10)
  set ( OSX_CODENAME "Yosemite")
endif()

if (OSX_VERSION VERSION_GREATER 10.11 OR OSX_VERSION VERSION_EQUAL 10.11)
  set ( OSX_CODENAME "El Capitan")
endif()

# Export variables globally
set(OSX_VERSION ${OSX_VERSION} CACHE INTERNAL "")
set(OSX_CODENAME ${OSX_CODENAME} CACHE INTERNAL "")

message (STATUS "Operating System: Mac OS X ${OSX_VERSION} (${OSX_CODENAME})")

# Enable the use of the -isystem flag to mark headers in Third_Party as system headers
set(CMAKE_INCLUDE_SYSTEM_FLAG_CXX "-isystem ")

###########################################################################
# Mac-specific installation setup
###########################################################################

set(CMAKE_INSTALL_PREFIX "")
set(CPACK_PACKAGE_EXECUTABLES NSXQt)
set(INBUNDLE NSXQt.app/)

set(BIN_DIR NSXQt.app/Contents/MacOS )
set(LIB_DIR NSXQt.app/Contents/MacOS )
set(PLUGINS_DIR NSXQt.app/plugins )

set(CMAKE_MACOSX_RPATH 1)

install(FILES ${CMAKE_SOURCE_DIR}/apps/NSXQt/resources/nsxtool.icns DESTINATION NSXQt.app/Contents/Resources/)

install(FILES ${CMAKE_SOURCE_DIR}/cmake/nsxtool_Info.plist DESTINATION NSXQt.app/Contents/ RENAME Info.plist)

set(CPACK_DMG_BACKGROUND_IMAGE ${CMAKE_SOURCE_DIR}/apps/NSXQt/resources/crystalIcon.png)
set(MACOSX_BUNDLE_ICON_FILE nsxtool.icns)

string (REPLACE " " "" CPACK_SYSTEM_NAME ${OSX_CODENAME})

set(CPACK_GENERATOR DragNDrop)
