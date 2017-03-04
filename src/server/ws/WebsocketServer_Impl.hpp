#ifndef WEBSOCKETSERVER_IMPL_H
#define WEBSOCKETSERVER_IMPL_H

#include <memory>
#include <unordered_map>
#include <thread>
#include <string>
#include <seasocks/WebSocket.h>
#include <seasocks/Server.h>
#include "WebsocketHandler.hpp"

class EventQueue;


class WebsocketServer_Impl {
    EventQueue* queue;
    EventQueue* appQueue;
    // websocket server
    std::shared_ptr<WebsocketHandler> websocketHandler;
    std::unordered_map<size_t, std::list<WebsocketClientData>> userToClients;
    std::unordered_map<seasocks::WebSocket*, std::list<WebsocketClientData>::iterator> clients;
    seasocks::Server server;
    std::thread serverThread;

    std::string eventToJson(std::shared_ptr<IEvent> event);
public:
    WebsocketServer_Impl(EventQueue* queue, EventQueue* appQueue);
    ~WebsocketServer_Impl();

    void addClient(size_t userId, seasocks::WebSocket* socket);
    void removeClient(seasocks::WebSocket* socket);

    bool onEvent(std::shared_ptr<IEvent> event);
};

#endif
