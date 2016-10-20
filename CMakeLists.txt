cmake_minimum_required(VERSION 3.3)
project(iirc)
include(ExternalProject)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake_modules")



set(SOURCE_FILES src/app/Application.cpp src/app/Application.hpp
    src/app/ApplicationGuard.cpp src/app/ApplicationGuard.hpp
    src/queue/EventLoop.cpp src/queue/EventLoop.hpp
    src/db/DatabaseQuery.cpp src/db/DatabaseQuery.hpp
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
    src/event/EventQueryChats.cpp src/event/EventQueryChats.hpp
    src/event/EventQuerySettings.cpp src/event/EventQuerySettings.hpp
    src/event/irc/IrcLoggable.cpp src/event/irc/IrcLoggable.hpp
    src/event/irc/EventIrcChatListing.cpp src/event/irc/EventIrcChatListing.hpp
    src/event/irc/EventIrcSettingsListing.cpp src/event/irc/EventIrcSettingsListing.hpp
    src/event/irc/EventIrcAction.cpp src/event/irc/EventIrcAction.hpp
    src/event/irc/EventIrcSendMessage.cpp src/event/irc/EventIrcSendMessage.hpp
    src/event/irc/EventIrcSendAction.cpp src/event/irc/EventIrcSendAction.hpp
    src/event/irc/EventIrcConnected.cpp src/event/irc/EventIrcConnected.hpp
    src/event/irc/EventIrcTopic.cpp src/event/irc/EventIrcTopic.hpp
    src/event/irc/EventIrcNoticed.cpp src/event/irc/EventIrcNoticed.hpp
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
    src/event/irc/EventIrcDeleteHost.cpp src/event/irc/EventIrcDeleteHost.hpp
    src/event/irc/EventIrcServerAdded.cpp src/event/irc/EventIrcServerAdded.hpp
    src/event/irc/EventIrcHostAdded.cpp src/event/irc/EventIrcHostAdded.hpp
    src/event/irc/EventIrcServerDeleted.cpp src/event/irc/EventIrcServerDeleted.hpp
    src/event/irc/EventIrcHostDeleted.cpp src/event/irc/EventIrcHostDeleted.hpp
    src/event/irc/EventIrcModifyNick.cpp src/event/irc/EventIrcModifyNick.hpp
    src/event/irc/EventIrcNickModified.cpp src/event/irc/EventIrcNickModified.hpp
    src/event/irc/EventIrcUserlistReceived.cpp src/event/irc/EventIrcUserlistReceived.hpp
    src/event/irc/EventIrcReconnectServer.cpp src/event/irc/EventIrcReconnectServer.hpp
    src/event/EventQuit.cpp src/event/EventQuit.hpp)

set(LINK_LIBRARIES "")
set(INCLUDE_DIRECTORIES "")



if(${USE_POSTGRES_DATABASE})
    message("Postgres included")
    set(SOURCE_FILES ${SOURCE_FILES}
        src/db/handler/Postgres.hpp
        src/db/handler/Postgres.cpp
        )
    add_definitions(-DUSE_POSTGRES_DATABASE)
endif()



if(${USE_WEBSOCKET_SERVER})
    message("Websocket server included")
    if(${USE_WEBSOCKET_SERVER_VERBOSE})
        message("Websocket server verbose")
        add_definitions(-DUSE_WEBSOCKET_SERVER_VERBOSE)
    endif()
    set(SOURCE_FILES ${SOURCE_FILES}
        src/server/ws/WebsocketServer.cpp
        src/server/ws/WebsocketServer_Impl.cpp
        src/server/ws/WebsocketHandler.cpp
        src/server/ws/WebsocketServer.hpp
        src/server/ws/WebsocketServer_Impl.hpp)
    add_definitions(-DUSE_WEBSOCKET_SERVER)
    if(${Seasocks_GIT})
        message("Using Seasocks from GIT")
    else()
        message("Using local Seasocks")
        find_package(Seasocks REQUIRED)
    endif()
    find_package(JsonCpp REQUIRED)
endif()



add_executable(iirc src/main.cpp ${SOURCE_FILES})
if(${BUILD_TEST})
  set(TEST_SOURCE_FILES
      src/tests/TestPostgres.cpp
      src/tests/TestModules.cpp)
  add_executable(iircTest
                 src/test.cpp src/test.hpp
                 ${SOURCE_FILES}
                 ${TEST_SOURCE_FILES})
endif()



if(${USE_WEBSOCKET_SERVER})
  if(${Seasocks_GIT})
    ExternalProject_Add(Seasocks
      GIT_REPOSITORY "https://github.com/mattgodbolt/seasocks"
      GIT_TAG "master"
      UPDATE_COMMAND ""
      INSTALL_COMMAND "")
  else()
    if(${Seasocks_FOUND})
      list(APPEND INCLUDE_DIRECTORIES ${Seasocks_INCLUDE_DIR})
      list(APPEND LINK_LIBRARIES ${Seasocks_LIBRARY})
    else()
      message(WARNING "SEASOCKS NOT FOUND")
      set(MISSING_LIB 1)
    endif()
  endif()
  if(${JsonCpp_FOUND})
    list(APPEND INCLUDE_DIRECTORIES ${JsonCpp_INCLUDE_DIR})
    list(APPEND LINK_LIBRARIES ${JsonCpp_LIBRARY})
  else()
    message(WARNING "JSONCPP NOT FOUND")
    set(MISSING_LIB 1)
  endif()
endif()



find_package(OpenSSL REQUIRED)
if(${OPENSSL_FOUND})
  list(APPEND INCLUDE_DIRECTORIES ${OPENSSL_INCLUDE_DIR})
  list(APPEND LINK_LIBRARIES ${OPENSSL_LIBRARIES})
else()
  message(WARNING "OPENSSL NOT FOUND")
  set(MISSING_LIB 1)
endif()



find_package(Soci REQUIRED)
if(${SOCI_FOUND})
  list(APPEND INCLUDE_DIRECTORIES ${SOCI_INCLUDE_DIRS})
  list(APPEND LINK_LIBRARIES ${SOCI_LIBRARY} ${SOCI_postgresql_PLUGIN})
else()
  message(WARNING "SOCI NOT FOUND")
  set(MISSING_LIB 1)
endif()



find_package(LibIrcClient REQUIRED)
if(${LibIrcClient_FOUND})
  list(APPEND INCLUDE_DIRECTORIES ${LibIrcClient_INCLUDE_DIRS})
  list(APPEND LINK_LIBRARIES ${LibIrcClient_LIBRARY})
else()
  message(WARNING "LibIrcClient NOT FOUND")
  set(MISSING_LIB 1)
endif()



target_link_libraries(iirc ${LINK_LIBRARIES})
target_include_directories(iirc PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src ${INCLUDE_DIRECTORIES})
set_target_properties(iirc PROPERTIES COMPILE_FLAGS "-std=c++11 -pedantic -Wall")

if(${BUILD_TEST})
  target_link_libraries(iircTest ${LINK_LIBRARIES})
  target_include_directories(iircTest PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src ${INCLUDE_DIRECTORIES})
  set_target_properties(iircTest PROPERTIES COMPILE_FLAGS "-std=c++11 -pedantic -Wall")
endif()
