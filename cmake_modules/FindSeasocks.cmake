SET(_Seasocks_REQUIRED_VARS Seasocks_INCLUDE_DIR Seasocks_LIBRARY)

FIND_PATH(Seasocks_INCLUDE_DIR WebSocket.h
  PATH_SUFFIXES "" "seasocks"
  PATHS ${Seasocks_PKGCONF_INCLUDE_DIRS}
      /usr/include /sw/include /usr/local/include
      /usr/include/seasocks /sw/include/seasocks
      /usr/local/include/seasocks
)
MARK_AS_ADVANCED(Seasocks_INCLUDE_DIR)
SET(Seasocks_INCLUDE_DIRS ${Seasocks_INCLUDE_DIR})

FIND_LIBRARY(Seasocks_LIBRARY seasocks
  PATH_SUFFIXES "" "seasocks"
  PATHS ${LibSeasocks_PKGCONF_LIBRARY_DIRS}
      /usr/lib /lib /sw/lib /usr/local/lib
)
MARK_AS_ADVANCED(Seasocks_LIBRARY)

GET_FILENAME_COMPONENT(Seasocks_LIBRARY_DIR ${Seasocks_LIBRARY} PATH)
MARK_AS_ADVANCED(Seasocks_LIBRARY_DIR)

IF(Seasocks_INCLUDE_DIR AND Seasocks_LIBRARY)
    SET(Seasocks_FOUND True)
ENDIF()

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Seasocks DEFAULT_MSG ${_Seasocks_REQUIRED_VARS})
