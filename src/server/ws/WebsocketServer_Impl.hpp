#ifndef WEBSOCKETSERVER_IMPL_H
#define WEBSOCKETSERVER_IMPL_H

#include <memory>
#include <unordered_map>
#include <thread>
#include <seasocks/WebSocket.h>
#include <seasocks/Server.h>

class EventQueue;


class WebsocketHandler : public seasocks::WebSocket::Handler {
	EventQueue* appQueue;
	EventQueue* queue;
	std::unordered_map<seasocks::WebSocket*, size_t>& clients;
public:

	WebsocketHandler(EventQueue* appQueue,
		EventQueue* queue,
		std::unordered_map<seasocks::WebSocket*, size_t>& clients);
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
	std::unordered_map<seasocks::WebSocket*, size_t> clients;
	seasocks::Server server;
	std::thread serverThread;
public:
	WebsocketServer_Impl(EventQueue* queue, EventQueue* appQueue);
	~WebsocketServer_Impl();

	bool onEvent(std::shared_ptr<IEvent> event);
};

#endif
