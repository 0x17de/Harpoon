SET(_JsonCpp_REQUIRED_VARS JsonCpp_INCLUDE_DIR JsonCpp_LIBRARY)

FIND_PATH(JsonCpp_INCLUDE_DIR json/json.h
  PATH_SUFFIXES "" "jsoncpp"
  PATHS ${JsonCpp_PKGCONF_INCLUDE_DIRS}
      /usr/include /sw/include /usr/local/include
      /usr/include/jsoncpp /sw/include/jsoncpp
      /usr/local/include/jsoncpp
)
MARK_AS_ADVANCED(JsonCpp_INCLUDE_DIR)
SET(JsonCpp_INCLUDE_DIRS ${JsonCpp_INCLUDE_DIR})

FIND_LIBRARY(JsonCpp_LIBRARY jsoncpp
  PATH_SUFFIXES "" "jsoncpp"
  PATHS ${LibJsonCpp_PKGCONF_LIBRARY_DIRS}
      /usr/lib /lib /sw/lib /usr/local/lib
)
MARK_AS_ADVANCED(JsonCpp_LIBRARY)

GET_FILENAME_COMPONENT(JsonCpp_LIBRARY_DIR ${JsonCpp_LIBRARY} PATH)
MARK_AS_ADVANCED(JsonCpp_LIBRARY_DIR)

IF(JsonCpp_INCLUDE_DIR AND JsonCpp_LIBRARY)
    SET(JsonCpp_FOUND True)
ENDIF()

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(JsonCpp DEFAULT_MSG ${_JsonCpp_REQUIRED_VARS})
