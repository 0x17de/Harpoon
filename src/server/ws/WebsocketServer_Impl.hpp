#ifndef WEBSOCKETSERVER_IMPL_H
#define WEBSOCKETSERVER_IMPL_H
#include <memory>
#include <unordered_map>
#include <WebSocket.h>

using namespace seasocks;


class EventQueue;
class WebsocketServer_Impl : public WebSocket::Handler {
	std::unordered_map<WebSocket*, size_t> _connections;
public:
	EventQueue* appQueue;

	virtual void onConnect(WebSocket* connection);
	virtual void onData(WebSocket* connection, const char* data);
	virtual void onDisconnect(WebSocket* connection);

	WebsocketServer_Impl(EventQueue* appQueue);
	bool onEvent(std::shared_ptr<IEvent> event);
};

#endif
