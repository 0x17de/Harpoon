#ifndef WEBSOCKETHANDLER_H
#define WEBSOCKETHANDLER_H

#include <list>
#include <unordered_map>
#include <seasocks/WebSocket.h>
#include <seasocks/Server.h>
#include "WebsocketProtocolHandler.hpp"


class EventQueue;


struct WebsocketClientData {
public:
    WebsocketClientData(size_t userId, seasocks::WebSocket* socket);
    size_t userId;
    seasocks::WebSocket* socket;
};

class WebsocketHandler : public seasocks::WebSocket::Handler {
    EventQueue* appQueue;
    EventQueue* queue;
    std::map<std::string, std::unique_ptr<WebsocketProtocolHandler>> protocolHandlers;
    const std::unordered_map<seasocks::WebSocket*, std::list<WebsocketClientData>::iterator>& clients;
public:

    WebsocketHandler(EventQueue* appQueue,
                     EventQueue* queue,
                     const std::unordered_map<seasocks::WebSocket*, std::list<WebsocketClientData>::iterator>& clients);
    virtual ~WebsocketHandler();
    virtual void onConnect(seasocks::WebSocket* connection);
    virtual void onData(seasocks::WebSocket* connection, const char* data);
    virtual void onDisconnect(seasocks::WebSocket* connection);
};

#endif
