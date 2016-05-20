#include "WebsocketServer.hpp"
#include "WebsocketServer_Impl.hpp"
#include "event/EventLogin.hpp"
#include <sstream>
#include <json/json.h>

using namespace std;


WebsocketHandler::WebsocketHandler(EventQueue* appQueue,
	EventQueue* queue,
	std::unordered_map<seasocks::WebSocket*, size_t>& clients)
:
	appQueue{appQueue},
	queue{queue},
	clients{clients}
{
}

WebsocketHandler::~WebsocketHandler() {
}

void WebsocketHandler::onConnect(seasocks::WebSocket* connection) {
#warning onConnect stub
}

void WebsocketHandler::onData(seasocks::WebSocket* connection, const char* cdata) {
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

void WebsocketHandler::onDisconnect(seasocks::WebSocket* connection) {
#warning onDisconnect stub
}


