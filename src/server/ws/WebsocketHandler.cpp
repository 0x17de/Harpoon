#include "WebsocketServer.hpp"
#include "WebsocketServer_Impl.hpp"
#include "event/EventLogin.hpp"
#include "event/EventLogout.hpp"
#include "event/EventQuerySettings.hpp"
#include "event/irc/EventIrcSendMessage.hpp"
#include "event/irc/EventIrcAddServer.hpp"
#include "event/irc/EventIrcDeleteServer.hpp"
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
                appQueue->sendEvent(make_shared<EventLogin>(connection, username, password));
            }
        }
    } else { // logged in
        WebsocketClientData& clientData = *(it->second);
        Json::Value root;
        Json::Reader reader;
        cout << "Parsing: " << data << endl;
        if (!reader.parse(data, root)) return;

        try {
            string cmd = root.get("cmd", "").asString();
            string type = root.get("type", "").asString();
            if (type == "") {
                if (cmd == "chat") {
                    size_t serverId;
                    stringstream(root.get("server", "").asString()) >> serverId;
                    string channel = root.get("channel", "").asString();
                    string message = root.get("msg", "").asString();
                    appQueue->sendEvent(make_shared<EventIrcSendMessage>(clientData.userId, serverId, channel, message));
                } else if (cmd == "querysettings") {
                    appQueue->sendEvent(make_shared<EventQuerySettings>(clientData.userId, connection));
                }
            } else if (type == "irc") {
                if (cmd == "addserver") {
                    string name = root.get("name", "").asString();
                    appQueue->sendEvent(make_shared<EventIrcAddServer>(clientData.userId, name));
                } else if (cmd == "deleteserver") {
                    size_t serverId;
                    istringstream(root.get("id", "0").asString()) >> serverId;
                    if (serverId != 0)
                        appQueue->sendEvent(make_shared<EventIrcDeleteServer>(clientData.userId, serverId));
                } else if (cmd == "addhost") {
                    /*
                    size_t serverId = root.get("serverId", "").asInt();
                    string host = root.get("host", "").asString();
                    string password = root.get("password", "").asString();
                    int port = root.get("port", -1).asInt();
                    bool ipV6 = root.get("ipv6", true).asBool();
                    bool ssl = root.get("ssl", true).asBool();

                    appQueue->sendEvent(make_shared<EventIrcAddServer>(clientData.userId,
                                                                       name,
                                                                       host,
                                                                       password,
                                                                       port,
                                                                       ipV6,
                                                                       ssl));
                    */
                }
            }
        } catch(std::exception const& error) {
            cout << "Error while reading JSON command:" << endl
                 << error.what() << endl
                 << data << endl;
        }
    }
}

void WebsocketHandler::onDisconnect(seasocks::WebSocket* connection) {
    queue->sendEvent(make_shared<EventLogout>(connection));
}


