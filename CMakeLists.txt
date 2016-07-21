cmake_minimum_required(VERSION 3.3)
project(iirc)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake_modules")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11")

set(SOURCE_FILES src/app/Application.cpp src/app/Application.hpp
    src/app/ApplicationGuard.cpp src/app/ApplicationGuard.hpp
    src/queue/EventLoop.cpp src/queue/EventLoop.hpp
    src/db/LoginDatabase_Dummy.cpp src/db/LoginDatabase_Dummy.hpp
    src/db/LoginDatabase_Ini.cpp src/db/LoginDatabase_Ini.hpp
    src/db/IrcDatabase_Dummy.cpp src/db/IrcDatabase_Dummy.hpp
    src/db/IrcDatabase_Ini.cpp src/db/IrcDatabase_Ini.hpp
    src/event/IEvent.hpp src/event/IClientEvent.hpp
    src/service/irc/IrcChannelLoginData.cpp src/service/irc/IrcChannelLoginData.hpp
    src/service/irc/IrcServerConfiguration.cpp src/service/irc/IrcServerConfiguration.hpp
    src/service/irc/IrcServerHostConfiguration.cpp src/service/irc/IrcServerHostConfiguration.hpp
    src/service/irc/IrcConnection.cpp src/service/irc/IrcConnection_Impl.cpp
    src/service/irc/IrcConnection.hpp
    src/service/irc/IrcChannelStore.cpp src/service/irc/IrcChannelStore.hpp
    src/service/irc/IrcConnection_Impl.hpp src/service/irc/IrcEvents.cpp
    src/main.cpp src/queue/EventQueue.cpp src/queue/EventQueue.hpp
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
    src/event/IActivateServiceEvent.hpp
    src/event/irc/EventIrcActivateService.cpp src/event/irc/EventIrcActivateService.hpp
    src/event/EventLogin.cpp src/event/EventLogin.hpp
    src/event/EventLogout.cpp src/event/EventLogout.hpp
    src/event/EventLoginResult.cpp src/event/EventLoginResult.hpp
    src/event/EventQueryChats.cpp src/event/EventQueryChats.hpp
    src/event/EventQuerySettings.cpp src/event/EventQuerySettings.hpp
    src/event/irc/EventIrcChatListing.cpp src/event/irc/EventIrcChatListing.hpp
    src/event/irc/EventIrcSettingsListing.cpp src/event/irc/EventIrcSettingsListing.hpp
    src/event/irc/EventIrcAction.cpp src/event/irc/EventIrcAction.hpp
    src/event/irc/EventIrcSendMessage.cpp src/event/irc/EventIrcSendMessage.hpp
    src/event/irc/EventIrcConnected.cpp src/event/irc/EventIrcConnected.hpp
    src/event/irc/EventIrcTopic.cpp src/event/irc/EventIrcTopic.hpp
    src/event/irc/EventIrcNoticed.cpp src/event/irc/EventIrcNoticed.hpp
    src/event/irc/EventIrcInvited.cpp src/event/irc/EventIrcInvited.hpp
    src/event/irc/EventIrcModeChanged.cpp src/event/irc/EventIrcModeChanged.hpp
    src/event/irc/EventIrcUserModeChanged.cpp src/event/irc/EventIrcUserModeChanged.hpp
    src/event/irc/EventIrcQuit.cpp src/event/irc/EventIrcQuit.hpp
    src/event/irc/EventIrcParted.cpp src/event/irc/EventIrcParted.hpp
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
    src/event/irc/EventIrcDeleteNick.cpp src/event/irc/EventIrcDeleteNick.hpp
    src/event/irc/EventIrcReconnectServer.cpp src/event/irc/EventIrcReconnectServer.hpp
    src/event/EventQuit.cpp src/event/EventQuit.hpp
    )


#find_package(Protobuf REQUIRED)
#include_directories(${PROTOBUF_INCLUDE_DIRS})
#include_directories(${CMAKE_CURRENT_BINARY_DIR})
#PROTOBUF_GENERATE_CPP(PROTO_SRCS PROTO_HDRS
#       src/commands/common.proto
#       src/commands/server.proto
#       src/commands/client.proto)


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
        src/server/ws/WebsocketServer_Impl.hpp
    )
    add_definitions(-DUSE_WEBSOCKET_SERVER)
    find_package(Seasocks REQUIRED)
    find_package(JsonCpp REQUIRED)
endif()


add_executable(iirc ${SOURCE_FILES})
    #${PROTO_SRCS} ${PROTO_HDRS})


#target_link_libraries(iirc ${PROTOBUF_LIBRARIES})

if(${USE_WEBSOCKET_SERVER})
  if(${Seasocks_FOUND})
    include_directories(iirc ${Seasocks_INCLUDE_DIR})
    target_link_libraries(iirc ${Seasocks_LIBRARY})
  else()
    message(WARNING "SEASOCKS NOT FOUND")
    set(MISSING_LIB 1)
  endif()
  if(${JsonCpp_FOUND})
    include_directories(iirc ${JsonCpp_INCLUDE_DIR})
    target_link_libraries(iirc ${JsonCpp_LIBRARY})
  else()
    message(WARNING "JSONCPP NOT FOUND")
    set(MISSING_LIB 1)
  endif()
endif()

find_package(OpenSSL REQUIRED)
if(${OPENSSL_FOUND})
  include_directories(${OPENSSL_INCLUDE_DIR})
  target_link_libraries(iirc ${OPENSSL_LIBRARIES})
else()
  message(WARNING "OPENSSL NOT FOUND")
  set(MISSING_LIB 1)
endif()

find_package(Soci REQUIRED)
if(${SOCI_FOUND})
  include_directories(${SOCI_INCLUDE_DIRS})
  target_link_libraries(iirc ${SOCI_LIBRARY} ${SOCI_postgresql_PLUGIN})
else()
  message(WARNING "SOCI NOT FOUND")
  set(MISSING_LIB 1)
endif()

find_package(LibIrcClient REQUIRED)
if(${LibIrcClient_FOUND})
  include_directories(${LibIrcClient_INCLUDE_DIRS})
  target_link_libraries(iirc ${LibIrcClient_LIBRARY})
else()
  message(WARNING "LibIrcClient NOT FOUND")
  set(MISSING_LIB 1)
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)
