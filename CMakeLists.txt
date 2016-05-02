cmake_minimum_required(VERSION 3.3)
project(iirc)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake_modules")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11")

set(SOURCE_FILES src/app/Application.cpp src/app/Application.h
	src/app/ApplicationGuard.cpp src/app/ApplicationGuard.h
	src/queue/EventLoop.cpp src/queue/EventLoop.h
	src/db/ChatDatabaseDummy.cpp src/db/ChatDatabaseDummy.h
	src/event/IEvent.h src/ini/IniReader.cpp src/ini/IniReader.h
	src/irc/IrcConnection.cpp src/irc/IrcConnection.h
	src/irc/IrcConnection_Impl.h
	src/main.cpp src/queue/EventQueue.cpp src/queue/EventQueue.h
	src/queue/EventQueue_Impl.h src/tcp/TcpClient.cpp
	src/tcp/TcpClient.h src/tcp/TcpServer.cpp src/tcp/TcpServer.h
	src/user/User.cpp src/user/User.h src/utils/uuid.h
	src/user/UserManager.cpp src/user/UserManager.h
	src/event/EventLogin.cpp src/event/EventLogin.h
	src/event/EventLoginResult.cpp src/event/EventLoginResult.h
	src/event/EventIrcJoinChannel.cpp src/event/EventIrcJoinChannel.h
	src/event/EventQuit.cpp src/event/EventQuit.h
	)


find_package(Protobuf REQUIRED)
include_directories(${PROTOBUF_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_BINARY_DIR})
#PROTOBUF_GENERATE_CPP(PROTO_SRCS PROTO_HDRS
#		src/commands/common.proto
#		src/commands/server.proto
#		src/commands/client.proto)


add_executable(iirc ${SOURCE_FILES} ${PROTO_SRCS} ${PROTO_HDRS})


target_link_libraries(iirc ${PROTOBUF_LIBRARIES})


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
