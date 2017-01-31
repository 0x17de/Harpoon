cmake_minimum_required(VERSION 3.0)
project(Harpoon)
include(ExternalProject)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake_modules")


# READ VERSION FROM SOURCE

file(STRINGS src/version.hpp VERSION)
string(REPLACE " " ";" VERSION "${VERSION}")
list(LENGTH VERSION VERSION_LEN)
list(GET VERSION 4 CPACK_PACKAGE_VERSION_MAJOR)
list(GET VERSION 10 CPACK_PACKAGE_VERSION_MINOR)
list(GET VERSION 16 CPACK_PACKAGE_VERSION_PATCH)
set(CPACK_PACKAGE_VERSION "${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}")
message("Harpoon Version ${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}")


# CONFIGURATION

macro(process_flag level flag default description)
    # crete cache entry
    set(${flag} "${default}" CACHE BOOL "${description}")
    # display flag information
    set(_description "${description} (${flag})")
    if(${flag})
        set(_description "[x] ${_description}")
    else()
        set(_description "[ ] ${_description}")
    endif()
    # indentation
    foreach(i RANGE ${level})
        set(_description "  ${_description}")
    endforeach()
    # print final message
    message(${_description})
endmacro()


# REPORTING

process_flag(0 USE_WEBSOCKET_SERVER 1 "Using websocket server")
  process_flag(1 USE_WEBSOCKET_SERVER_VERBOSE 0 "Verbose websocket messages")
  process_flag(1 Seasocks_GIT 0 "Get seasocks from git")

process_flag(0 USE_POSTGRES_DATABASE 1 "Using postgres")
  process_flag(1 DATABASE_VERBOSE_QUERY 0 "Verbose SQL printing")

process_flag(0 BUILD_TEST 0 "Build tests")


# SOURCES

set(SOURCE_FILES src/version.hpp
    src/app/Application.cpp src/app/Application.hpp
    src/app/ApplicationGuard.cpp src/app/ApplicationGuard.hpp
    src/queue/EventLoop.cpp src/queue/EventLoop.hpp
    src/db/query/Database_Query.hpp src/db/query/Database_Query_Base.hpp src/db/query/Database_QueryBase.cpp
    src/db/query/Database_Details_Create.hpp src/db/query/Database_QueryCreate_Store.hpp
    src/db/query/Database_Details_Insert.hpp src/db/query/Database_QueryInsert_Store.hpp
    src/db/query/Database_Details_Update.hpp src/db/query/Database_QueryUpdate_Store.hpp
    src/db/query/Database_Details_Select.hpp src/db/query/Database_QuerySelect_Store.cpp src/db/query/Database_QuerySelect_Store.hpp
    src/db/query/Database_Details_Delete.hpp src/db/query/Database_QueryDelete_Store.cpp src/db/query/Database_QueryDelete_Store.hpp
    src/db/LoginDatabase_Dummy.cpp src/db/LoginDatabase_Dummy.hpp
    src/db/LoginDatabase_Ini.cpp src/db/LoginDatabase_Ini.hpp
    src/db/IrcDatabase_Dummy.cpp src/db/IrcDatabase_Dummy.hpp
    src/db/IrcDatabase_Ini.cpp src/db/IrcDatabase_Ini.hpp
    src/event/IEvent.hpp src/event/IEvent.cpp src/event/IClientEvent.hpp src/event/irc/IIrcCommand.hpp
    src/service/irc/IrcBacklogService.cpp src/service/irc/IrcBacklogService.hpp
    src/service/irc/IrcChannelLoginData.cpp src/service/irc/IrcChannelLoginData.hpp
    src/service/irc/IrcServerConfiguration.cpp src/service/irc/IrcServerConfiguration.hpp
    src/service/irc/IrcServerHostConfiguration.cpp src/service/irc/IrcServerHostConfiguration.hpp
    src/service/irc/IrcConnection.cpp src/service/irc/IrcConnection_Impl.cpp
    src/service/irc/IrcConnection.hpp
    src/service/irc/IrcChannelStore.cpp src/service/irc/IrcChannelStore.hpp
    src/service/irc/IrcConnection_Impl.hpp src/service/irc/IrcEvents.cpp
    src/queue/EventQueue.cpp src/queue/EventQueue.hpp
    src/queue/EventQueue_Impl.hpp src/utils/uuid.hpp
    src/utils/Ini.cpp src/utils/Ini.hpp
    src/service/irc/IrcService.cpp src/service/irc/IrcService.hpp
    src/user/UserManager.cpp src/user/UserManager.hpp
    src/utils/ModuleProvider.cpp src/utils/ModuleProvider.hpp
    src/utils/Filesystem.cpp src/utils/Filesystem.hpp
    src/utils/Base64.cpp src/utils/Base64.hpp
    src/utils/Crypto.cpp src/utils/Crypto.hpp
    src/utils/Password.cpp src/utils/Password.hpp
    src/utils/IdProvider.cpp src/utils/IdProvider.hpp
    src/event/EventInit.cpp src/event/EventInit.hpp
    src/event/irc/EventIrcServiceInit.cpp src/event/irc/EventIrcServiceInit.hpp
    src/event/EventDatabaseQuery.cpp src/event/EventDatabaseQuery.hpp
    src/event/EventDatabaseResult.cpp src/event/EventDatabaseResult.hpp
    src/event/IActivateServiceEvent.hpp
    src/event/irc/EventIrcActivateService.cpp src/event/irc/EventIrcActivateService.hpp
    src/event/EventLogin.cpp src/event/EventLogin.hpp
    src/event/EventLogout.cpp src/event/EventLogout.hpp
    src/event/EventLoginResult.cpp src/event/EventLoginResult.hpp
    src/event/EventQuery.cpp src/event/EventQuery.hpp
    src/event/irc/IrcLoggable.cpp src/event/irc/IrcLoggable.hpp
    src/event/irc/EventIrcChatListing.cpp src/event/irc/EventIrcChatListing.hpp
    src/event/irc/EventIrcSettingsListing.cpp src/event/irc/EventIrcSettingsListing.hpp
    src/event/irc/EventIrcAction.cpp src/event/irc/EventIrcAction.hpp
    src/event/irc/EventIrcSendMessage.cpp src/event/irc/EventIrcSendMessage.hpp
    src/event/irc/EventIrcSendAction.cpp src/event/irc/EventIrcSendAction.hpp
    src/event/irc/EventIrcConnected.cpp src/event/irc/EventIrcConnected.hpp
    src/event/irc/EventIrcTopic.cpp src/event/irc/EventIrcTopic.hpp
    src/event/irc/EventIrcInvited.cpp src/event/irc/EventIrcInvited.hpp
    src/event/irc/EventIrcModeChanged.cpp src/event/irc/EventIrcModeChanged.hpp
    src/event/irc/EventIrcUserModeChanged.cpp src/event/irc/EventIrcUserModeChanged.hpp
    src/event/irc/EventIrcQuit.cpp src/event/irc/EventIrcQuit.hpp
    src/event/irc/EventIrcParted.cpp src/event/irc/EventIrcParted.hpp
    src/event/irc/EventIrcChangeNick.cpp src/event/irc/EventIrcChangeNick.hpp
    src/event/irc/EventIrcRequestBacklog.cpp src/event/irc/EventIrcRequestBacklog.hpp
    src/event/irc/EventIrcNickChanged.cpp src/event/irc/EventIrcNickChanged.hpp
    src/event/irc/EventIrcPartChannel.cpp src/event/irc/EventIrcPartChannel.hpp
    src/event/irc/EventIrcKicked.cpp src/event/irc/EventIrcKicked.hpp
    src/event/irc/EventIrcJoined.cpp src/event/irc/EventIrcJoined.hpp
    src/event/irc/EventIrcJoinChannel.cpp src/event/irc/EventIrcJoinChannel.hpp
    src/event/irc/EventIrcMessage.cpp src/event/irc/EventIrcMessage.hpp
    src/event/irc/EventIrcNumeric.cpp src/event/irc/EventIrcNumeric.hpp
    src/event/irc/EventIrcAddServer.cpp src/event/irc/EventIrcAddServer.hpp
    src/event/irc/EventIrcAddHost.cpp src/event/irc/EventIrcAddHost.hpp
    src/event/irc/EventIrcDeleteServer.cpp src/event/irc/EventIrcDeleteServer.hpp
    src/event/irc/EventIrcDeleteChannel.cpp src/event/irc/EventIrcDeleteChannel.hpp
    src/event/irc/EventIrcDeleteHost.cpp src/event/irc/EventIrcDeleteHost.hpp
    src/event/irc/EventIrcServerAdded.cpp src/event/irc/EventIrcServerAdded.hpp
    src/event/irc/EventIrcHostAdded.cpp src/event/irc/EventIrcHostAdded.hpp
    src/event/irc/EventIrcServerDeleted.cpp src/event/irc/EventIrcServerDeleted.hpp
    src/event/irc/EventIrcHostDeleted.cpp src/event/irc/EventIrcHostDeleted.hpp
    src/event/irc/EventIrcModifyNick.cpp src/event/irc/EventIrcModifyNick.hpp
    src/event/irc/EventIrcNickModified.cpp src/event/irc/EventIrcNickModified.hpp
    src/event/irc/EventIrcUserlistReceived.cpp src/event/irc/EventIrcUserlistReceived.hpp
    src/event/irc/EventIrcReconnectServer.cpp src/event/irc/EventIrcReconnectServer.hpp
    src/event/irc/IrcChannelListing.cpp src/event/irc/IrcChannelListing.hpp
    src/event/irc/IrcChannelUser.cpp src/event/irc/IrcChannelUser.hpp
    src/event/irc/IrcServerListing.cpp src/event/irc/IrcServerListing.hpp
    src/event/EventQuit.cpp src/event/EventQuit.hpp)

set(TEST_SOURCE_FILES
    src/test.cpp src/test.hpp
    src/tests/TestModules.cpp)


# LIBRARIES

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
set(LINK_LIBRARIES "Threads::Threads")
set(INCLUDE_DIRECTORIES "")



if(USE_POSTGRES_DATABASE)
    list(APPEND SOURCE_FILES
        src/db/handler/Postgres.hpp
        src/db/handler/Postgres.cpp)
    add_definitions(-DUSE_POSTGRES_DATABASE)
endif()
if(DATABASE_VERBOSE_QUERY)
    add_definitions(-DDATABASE_VERBOSE_QUERY)
endif()



if(USE_POSTGRES_DATABASE)
    find_package(Soci REQUIRED)
    if(SOCI_FOUND)
        list(APPEND INCLUDE_DIRECTORIES ${SOCI_INCLUDE_DIRS})
        list(APPEND LINK_LIBRARIES ${SOCI_LIBRARY} ${SOCI_postgresql_PLUGIN})
    else()
        message(FATAL_ERROR "SOCI NOT FOUND")
        set(MISSING_LIB 1)
    endif()

    list(APPEND TEST_SOURCE_FILES
        src/tests/TestPostgres.cpp)
endif()



if(USE_WEBSOCKET_SERVER)
    if(USE_WEBSOCKET_SERVER_VERBOSE)
        add_definitions(-DUSE_WEBSOCKET_SERVER_VERBOSE)
    endif()
    list(APPEND SOURCE_FILES
        src/server/ws/WebsocketServer.cpp
        src/server/ws/WebsocketServer_Impl.cpp
        src/server/ws/WebsocketHandler.cpp
        src/server/ws/WebsocketServer.hpp
        src/server/ws/WebsocketServer_Impl.hpp)
    add_definitions(-DUSE_WEBSOCKET_SERVER)
    if(NOT Seasocks_GIT)
        find_package(Seasocks REQUIRED)
        if(NOT Seasocks_FOUND)
            message(FATAL_ERROR "SEASOCKS NOT FOUND")
            set(MISSING_LIB 1)
        endif()
    endif()
    find_package(JsonCpp REQUIRED)
endif()



if(USE_WEBSOCKET_SERVER)
  if(Seasocks_GIT)
    ExternalProject_Add(Seasocks
      GIT_REPOSITORY "https://github.com/mattgodbolt/seasocks"
      GIT_TAG "master"
      UPDATE_COMMAND ""
      INSTALL_COMMAND "")
  else()
    if(Seasocks_FOUND)
      list(APPEND INCLUDE_DIRECTORIES ${Seasocks_INCLUDE_DIR})
      list(APPEND LINK_LIBRARIES ${Seasocks_LIBRARY})
    else()
      message(FATAL_ERROR "SEASOCKS NOT FOUND")
      set(MISSING_LIB 1)
    endif()
  endif()
  if(JsonCpp_FOUND)
    list(APPEND INCLUDE_DIRECTORIES ${JsonCpp_INCLUDE_DIR})
    list(APPEND LINK_LIBRARIES ${JsonCpp_LIBRARY})
  else()
    message(FATAL_ERROR "JSONCPP NOT FOUND")
    set(MISSING_LIB 1)
  endif()
endif()



find_package(OpenSSL REQUIRED)
if(OPENSSL_FOUND)
  list(APPEND INCLUDE_DIRECTORIES ${OPENSSL_INCLUDE_DIR})
  list(APPEND LINK_LIBRARIES ${OPENSSL_LIBRARIES})
else()
  message(FATAL_ERROR "OPENSSL NOT FOUND")
  set(MISSING_LIB 1)
endif()



find_package(LibIrcClient REQUIRED)
if(LibIrcClient_FOUND)
  list(APPEND INCLUDE_DIRECTORIES ${LibIrcClient_INCLUDE_DIRS})
  list(APPEND LINK_LIBRARIES ${LibIrcClient_LIBRARY})
else()
  message(FATAL_ERROR "LibIrcClient NOT FOUND")
  set(MISSING_LIB 1)
endif()


# EXECUTABLES

add_library(HarpoonCore SHARED ${SOURCE_FILES})
add_executable(Harpoon src/main.cpp)
target_link_libraries(Harpoon HarpoonCore)
if(BUILD_TEST)
    add_executable(HarpoonTest
                   ${TEST_SOURCE_FILES})
    target_link_libraries(HarpoonTest HarpoonCore)
endif()


## Default

target_link_libraries(HarpoonCore ${LINK_LIBRARIES})
target_include_directories(HarpoonCore PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src ${INCLUDE_DIRECTORIES})
set_target_properties(HarpoonCore PROPERTIES COMPILE_FLAGS "-std=c++11 -pedantic -Wall")
set_target_properties(Harpoon PROPERTIES COMPILE_FLAGS "-std=c++11 -pedantic -Wall")


## Test

if(BUILD_TEST)
  target_link_libraries(HarpoonTest ${LINK_LIBRARIES})
  target_include_directories(HarpoonTest PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src ${INCLUDE_DIRECTORIES})
  set_target_properties(HarpoonTest PROPERTIES COMPILE_FLAGS "-std=c++11 -pedantic -Wall")
endif()


# SETUP

INSTALL(TARGETS Harpoon HarpoonCore
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib/static)
include(InstallRequiredSystemLibraries)

SET(CPACK_PACKAGE_VERSION_MAJOR "0")
SET(CPACK_PACKAGE_VERSION_MINOR "2")
SET(CPACK_PACKAGE_VERSION_PATCH "0")
SET(CPACK_COMPONENTS_ALL_IN_ONE_PACKAGE 1)
SET(CPACK_MONOLITHIC_INSTALL 1)
SET(CPACK_GENERATOR "DEB;TGZ")
SET(CPACK_DEBIAN_PACKAGE_MAINTAINER "Manuel Herrmann")
SET(CPACK_PACKAGE_EXECUTABLES "Harpoon" "Harpoon")

INCLUDE(CPack)
