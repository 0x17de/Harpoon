#include "WebsocketServer.hpp"
#include "WebsocketServer_Impl.hpp"
#include "event/IUserEvent.hpp"
#include "event/EventQuit.hpp"
#include "event/EventLogin.hpp"
#include <iostream>
#include <sstream>
#include <json/json.h>
#include <seasocks/IgnoringLogger.h>

using namespace std;


WebsocketServer::WebsocketServer(EventQueue* appQueue)
:
	impl{new WebsocketServer_Impl(getEventQueue(), appQueue)}
{
}

WebsocketServer::~WebsocketServer() {
}

bool WebsocketServer::onEvent(std::shared_ptr<IEvent> event) {
	return impl->onEvent(event);
}

// IMPL

WebsocketServer_Impl::WebsocketServer_Impl(EventQueue* queue, EventQueue* appQueue)
:
	queue{queue},
	appQueue{appQueue},
	server{make_shared<seasocks::IgnoringLogger>()}
{
	server.addWebSocketHandler("/ws", make_shared<WebsocketHandler>(appQueue, queue, clients));
	serverThread = thread([this]{
		server.serve("public", 8080);
	});
}

WebsocketServer_Impl::~WebsocketServer_Impl() {
	server.terminate();
	serverThread.join();
}

bool WebsocketServer_Impl::onEvent(std::shared_ptr<IEvent> event) {
	UUID eventType = event->getEventUuid();
	if (eventType == EventQuit::uuid) {
		return false;
	}
#warning handle EventLoginResult
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

