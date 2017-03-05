#ifdef USE_WEBSOCKET_SERVER
#ifndef WEBSOCKETSERVER_H
#define WEBSOCKETSERVER_H

#include <memory>
#include <unordered_map>
#include <thread>
#include <string>
#include <seasocks/WebSocket.h>
#include <seasocks/Server.h>
#include "queue/EventLoop.hpp"
#include "WebsocketHandler.hpp"


class WebsocketServer : public EventLoop {
    EventQueue* appQueue;

    std::shared_ptr<WebsocketHandler> websocketHandler;
    std::unordered_map<size_t, std::list<WebsocketClientData>> userToClients;
    std::unordered_map<seasocks::WebSocket*, std::list<WebsocketClientData>::iterator> clients;
    seasocks::Server server;
    std::thread serverThread;

    std::string eventToJson(std::shared_ptr<IEvent> event);
public:
    WebsocketServer(EventQueue* appQueue);
    ~WebsocketServer();

    void addClient(size_t userId, seasocks::WebSocket* socket);
    void removeClient(seasocks::WebSocket* socket);

    virtual bool onEvent(std::shared_ptr<IEvent> event) override;
};

#endif
#endif
