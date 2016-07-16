# Harpoon
Connect to various services through this bouncer. Initially meant for IRC

This program is still work in progress. It will require a server.

![Screenshot](https://www.0x17.de/proj/harpoonClient4.png)

## Features
 - Multiple users can use this service at once (e.g. family)
 - Multiple clients at the same time possible for the same user
 - Stays online when you aren't
 - Read the backlog
 - IRC support
 - Modular design (disable modules you don't like at compiletime)
 - Optional WebClient (built in)
 - Mobile client for highlights and queries. Safe battery through push notifications
 - Easy to extend
 - Serverside/Clientside scripting support in the future - e.g. automate your coffee machine at work ;)
 - Other chats integrated in the future (and can also be removed during compile time)
   - Hangouts
   - Steam
   - Jabber
   - ...

## Setup
### Dependencies
 - Core
   - Protobuf
   - OpenSSL
   - LibIrcClient
 - WebChat (optional via USE_WEBSOCKET_SERVER)
   - Seasocks
   - JsonCpp

### Generate Makefiles
For building the source CMake is required. Run the following in the project's build folder:
```
cmake .. -DCMAKE_BUILD_TYPE=Debug -DUSE_WEBSOCKET_SERVER=1 -DUSE_WEBSOCKET_SERVER_VERBOSE=1
```
Run ```make``` afterwards.

### Run the binary
To start the service run ```build/iirc``` from the project root.
