#include "WebsocketServer.hpp"
#include "WebsocketServer_Impl.hpp"
#include "event/EventLogin.hpp"
#include "event/EventLogout.hpp"
#include "event/irc/EventIrcSendMessage.hpp"
#include <sstream>
#include <json/json.h>

using namespace std;


WebsocketHandler::WebsocketHandler(EventQueue* appQueue,
                                   EventQueue* queue,
                                   const std::unordered_map<seasocks::WebSocket*, std::list<WebsocketClientData>::iterator>& clients)
    : appQueue{appQueue}
    , queue{queue}
    , clients{clients}
{
}

WebsocketHandler::~WebsocketHandler() {
}

void WebsocketHandler::onConnect(seasocks::WebSocket* connection) {
#warning onConnect stub
}

void WebsocketHandler::onData(seasocks::WebSocket* connection, const char* cdata) {
    string data{cdata};

    auto it = clients.find(connection);
    // until login verified use plaintext protocol
    if (it == clients.end()) {
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
    } else { // logged in
        WebsocketClientData& clientData = *(it->second);
        Json::Value root;
        Json::Reader reader;
        cout << "Parsing: " << data << endl;
        if (!reader.parse(data, root)) return;

        string cmd = root.get("cmd", "" ).asString();
        if (cmd == "chat") {
            size_t serverId;
            stringstream(root.get("server", "").asString()) >> serverId;
            string channel = root.get("channel", "").asString();
            string message = root.get("msg", "").asString();
            appQueue->sendEvent(make_shared<EventIrcSendMessage>(clientData.userId, serverId, channel, message));
        }
#warning onData commands stub
#warning json parse stub
    }
}

void WebsocketHandler::onDisconnect(seasocks::WebSocket* connection) {
    queue->sendEvent(make_shared<EventLogout>(connection));
}


