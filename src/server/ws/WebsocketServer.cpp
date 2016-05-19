#include "WebsocketServer.hpp"
#include "WebsocketServer_Impl.hpp"
#include "event/IUserEvent.hpp"
#include "event/EventQuit.hpp"
#include <iostream>
#include <json/json.h>

using namespace std;


WebsocketServer::WebsocketServer(EventQueue* appQueue)
:
	impl{new WebsocketServer_Impl(appQueue)}
{
}

WebsocketServer::~WebsocketServer() {
}

bool WebsocketServer::onEvent(std::shared_ptr<IEvent> event) {
	return impl->onEvent(event);
}

WebsocketServer_Impl::WebsocketServer_Impl(EventQueue* appQueue)
:
	appQueue{appQueue}
{

}

void WebsocketServer_Impl::onConnect(WebSocket* connection) {
#warning onConnect stub
}
void WebsocketServer_Impl::onData(WebSocket* connection, const char* data) {
#warning onData stub
}
void WebsocketServer_Impl::onDisconnect(WebSocket* connection) {
#warning onDisconnect stub
}

bool WebsocketServer_Impl::onEvent(std::shared_ptr<IEvent> event) {
	UUID eventType = event->getEventUuid();
	if (eventType == EventQuit::uuid) {
		return false;
	}
	return true;
}

void WebsocketServer::sendEventToUser(std::shared_ptr<IEvent> event) {
	auto userEvent = event->as<IUserEvent>();
	if (userEvent != nullptr) {
		cout << "NonNULL event" << endl;
		//auto it = users.find(userEvent->getUserId());
		//if (it != users.end()) {
			//EventQueue* queue = it->second.getEventQueue();
			//if (queue->canProcessEvent(event->getEventUuid()))
			//	queue->sendEvent(event);
		//}
	}
}

