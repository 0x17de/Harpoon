#include "WebsocketServer.hpp"
#include "WebsocketServer_Impl.hpp"
#include "event/IUserEvent.hpp"
#include "event/EventQuit.hpp"
#include "event/EventLogin.hpp"
#include <iostream>
#include <sstream>
#include <json/json.h>

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

WebsocketServer_Impl::WebsocketServer_Impl(EventQueue* queue, EventQueue* appQueue)
:
	queue{queue},
	appQueue{appQueue}
{

}

void WebsocketServer_Impl::onConnect(WebSocket* connection) {
#warning onConnect stub
}
void WebsocketServer_Impl::onData(WebSocket* connection, const char* cdata) {
#warning onData stub
	
	auto it = clients.find(connection);
	size_t userId;
	if (it == clients.end()) {
		clients.emplace(connection, 0);
		userId = 0;
	} else {
		userId = it->second;
	}

	string data{cdata};
	// until login verified use plaintext protocol
	if (userId == 0) {
		if (data.size() > 512) return; // ignore large data during login
		istringstream is(data);
		string line;
		while (getline(is, line)) {
			istringstream lis(line);
			string cmd;
			getline(lis, cmd, ' ');
			if (cmd == "LOGIN") {
				string username;
				getline(lis, username, ' ');
				string password;
				getline(lis, password, ' ');
				appQueue->sendEvent(make_shared<EventLogin>(queue, connection, username, password));
			}
		}
	} else {
		Json::Value root;
		Json::Reader reader;
		if (!reader.parse(data, root)) return;

		string cmd = root.get("cmd", "" ).asString();
#warning json parse stub
	}
}
void WebsocketServer_Impl::onDisconnect(WebSocket* connection) {
#warning onDisconnect stub
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

