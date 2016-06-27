#ifndef WEBSOCKETSERVER_IMPL_H
#define WEBSOCKETSERVER_IMPL_H

#include <memory>
#include <unordered_map>
#include <thread>
#include <seasocks/WebSocket.h>
#include <seasocks/Server.h>

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
    void sendEventToUser(std::shared_ptr<IEvent> event);
};

#endif
