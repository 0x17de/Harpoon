#ifndef WEBSOCKETHANDLER_H
#define WEBSOCKETHANDLER_H

#include <list>
#include <unordered_map>
#include <seasocks/WebSocket.h>
#include <seasocks/Server.h>
#include "WebsocketProtocolHandler.hpp"


class EventQueue;

/// Session storage for websocket connections
struct WebsocketClientData {
public:
    /// Constructor for the session storage
    WebsocketClientData(size_t userId, seasocks::WebSocket* socket);
    /// The user id for the session
    size_t userId;
    /// Socket for communication with the client
    seasocks::WebSocket* socket;
};

/// Handles all websocket related commands, sent by the connected clients
class WebsocketHandler : public seasocks::WebSocket::Handler {
    /// Core event loop queue
    EventQueue* appQueue;
    /// Internal queue
    EventQueue* queue;
    /// Not used yet. Will be an abstraction for various protocols (irc/hack/...) for handling commands
    std::map<std::string, std::unique_ptr<WebsocketProtocolHandler>> protocolHandlers;
    /// Client list
    const std::unordered_map<seasocks::WebSocket*, std::list<WebsocketClientData>::iterator>& clients;

public:
    /// Constructs the general websocket handler
    WebsocketHandler(EventQueue* appQueue,
                     EventQueue* queue,
                     const std::unordered_map<seasocks::WebSocket*, std::list<WebsocketClientData>::iterator>& clients);
    virtual ~WebsocketHandler();

private:
    /// Seasocks callback on established connections
    virtual void onConnect(seasocks::WebSocket* connection);
    /// Seasocks callback once data was received
    virtual void onData(seasocks::WebSocket* connection, const char* data);
    /// Seasocks callback on disconnects
    virtual void onDisconnect(seasocks::WebSocket* connection);
};

#endif
