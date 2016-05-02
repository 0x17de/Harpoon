SET(_LibIrcClient_REQUIRED_VARS LibIrcClient_INCLUDE_DIR LibIrcClient_LIBRARY)

FIND_PATH(LibIrcClient_INCLUDE_DIR libircclient.h
  PATH_SUFFIXES "" "libircclient"
  PATHS ${LibIrcClient_PKGCONF_INCLUDE_DIRS}
      /usr/include /sw/include /usr/local/include
      /usr/include/libircclient /sw/include/libircclient
      /usr/local/include/libircclient
)
MARK_AS_ADVANCED(LibIrcClient_INCLUDE_DIR)
SET(LibIrcClient_INCLUDE_DIRS ${LibIrcClient_INCLUDE_DIR})

FIND_LIBRARY(LibIrcClient_LIBRARY ircclient
  PATH_SUFFIXES "" "libircclient"
  PATHS ${LibIRCClient_PKGCONF_LIBRARY_DIRS}
      /usr/lib /lib /sw/lib /usr/local/lib
)
MARK_AS_ADVANCED(LibIrcClient_LIBRARY)

GET_FILENAME_COMPONENT(LibIrcClient_LIBRARY_DIR ${LibIrcClient_LIBRARY} PATH)
MARK_AS_ADVANCED(LibIrcClient_LIBRARY_DIR)

IF(LibIrcClient_INCLUDE_DIR AND LibIrcClient_LIBRARY)
    SET(LibIrcClient_FOUND True)
ENDIF()

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(LibIrcClient DEFAULT_MSG ${_LibIrcClient_REQUIRED_VARS})