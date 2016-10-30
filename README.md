# Harpoon
Connect to various services through this bouncer. Initially meant for IRC

This program is still work in progress. It will require a server.

**You can ask questions and make requests on [freenode.net channel #harpoon](https://webchat.freenode.net/?channels=harpoon).**

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
   - You can additionally rely on the [bitlbee](https://www.bitlbee.org/) project, which is using the IRC protocol
   - ...

## Setup
### Dependencies
 - Services
   - IRC
     - LibIrcClient (>=1.6-r2)
   - Backlog (optional, via USE_*_DATABASE flags)
     - SOCI (>=3.2.2)
     - PostgreSQL (optional via USE_POSTGRES_DATABASE)
 - Client Protocols
   - WebChat (optional via USE_WEBSOCKET_SERVER)
     - Seasocks (>=1.2.4)
     - JsonCpp (>=0.10.2-r1)

### Generate Makefiles
For building the source CMake is required. Run the following in the project's build folder:
```
cmake .. -DCMAKE_BUILD_TYPE=Debug -DUSE_WEBSOCKET_SERVER=1 -DUSE_WEBSOCKET_SERVER_VERBOSE=0 -DUSE_POSTGRES_DATABASE=1
```
Run ```make``` afterwards.

### Run the binary
To start the service run ```build/iirc``` from the project root. If you enabled
webclient, you can check it on `localhost:8080`.
