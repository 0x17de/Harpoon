cmake_minimum_required(VERSION 3.3)
project(iirc)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake_modules")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11")

set(SOURCE_FILES src/app/Application.cpp src/app/Application.hpp
	src/app/ApplicationGuard.cpp src/app/ApplicationGuard.hpp
	src/queue/EventLoop.cpp src/queue/EventLoop.hpp
	src/db/ChatDatabaseDummy.cpp src/db/ChatDatabaseDummy.hpp
	src/event/IEvent.hpp src/event/IUserEvent.hpp
	src/ini/IniReader.cpp src/ini/IniReader.hpp
	src/irc/IrcConnection.cpp src/irc/IrcConnection.hpp
	src/irc/IrcConnection_Impl.hpp src/irc/IrcEvents.cpp
	src/main.cpp src/queue/EventQueue.cpp src/queue/EventQueue.hpp
	src/queue/EventQueue_Impl.hpp src/tcp/TcpClient.cpp
	src/tcp/TcpClient.hpp src/tcp/TcpServer.cpp src/tcp/TcpServer.hpp
	src/user/User.cpp src/user/User.hpp src/utils/uuid.hpp
	src/user/UserManager.cpp src/user/UserManager.hpp
	src/utils/Base64.cpp src/utils/Base64.hpp
	src/utils/Crypto.cpp src/utils/Crypto.hpp
	src/event/EventInit.cpp src/event/EventInit.hpp
	src/event/EventActivateUser.cpp src/event/EventActivateUser.hpp
	src/event/EventLogin.cpp src/event/EventLogin.hpp
	src/event/EventLoginResult.cpp src/event/EventLoginResult.hpp
	src/event/EventIrcParted.cpp src/event/EventIrcParted.hpp
	src/event/EventIrcPartChannel.cpp src/event/EventIrcPartChannel.hpp
	src/event/EventIrcJoined.cpp src/event/EventIrcJoined.hpp
	src/event/EventIrcJoinChannel.cpp src/event/EventIrcJoinChannel.hpp
	src/event/EventIrcMessage.cpp src/event/EventIrcMessage.hpp
	src/event/EventQuit.cpp src/event/EventQuit.hpp
	)


find_package(Protobuf REQUIRED)
include_directories(${PROTOBUF_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_BINARY_DIR})
#PROTOBUF_GENERATE_CPP(PROTO_SRCS PROTO_HDRS
#		src/commands/common.proto
#		src/commands/server.proto
#		src/commands/client.proto)


if(${USE_WEBSOCKET_SERVER})
	message("Websocket server included")
	set(SOURCE_FILES ${SOURCE_FILES}
		src/server/ws/WebsocketServer.cpp
		src/server/ws/WebsocketServer.hpp
		src/server/ws/WebsocketServer_Impl.hpp
	)
	add_definitions(-DUSE_WEBSOCKET_SERVER)
	find_package(WebSocketPP REQUIRED)
endif()


add_executable(iirc ${SOURCE_FILES} ${PROTO_SRCS} ${PROTO_HDRS})


target_link_libraries(iirc ${PROTOBUF_LIBRARIES})

if(${USE_WEBSOCKET_SERVER})
  if(${WEBSOCKETPP_FOUND})
    include_directories(iirc ${WEBSOCKETPP_INCLUDE_DIR})
  else()
    message(WARNING "WEBSOCKETPP NOT FOUND")
    set(MISSING_LIB 1)
  endif()
endif()

#find_package(Boost REQUIRED COMPONENTS system)
#include_directories(${Boost_INCLUDE_DIR})
#target_link_libraries(iirc ${Boost_LIBRARIES})

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
