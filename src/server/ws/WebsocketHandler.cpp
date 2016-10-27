#include "WebsocketServer.hpp"
#include "WebsocketServer_Impl.hpp"
#include "event/EventLogin.hpp"
#include "event/EventLogout.hpp"
#include "event/EventQuerySettings.hpp"
#include "event/irc/EventIrcMessageType.hpp"
#include "event/irc/EventIrcSendMessage.hpp"
#include "event/irc/EventIrcSendAction.hpp"
#include "event/irc/EventIrcAddServer.hpp"
#include "event/irc/EventIrcDeleteServer.hpp"
#include "event/irc/EventIrcAddHost.hpp"
#include "event/irc/EventIrcDeleteHost.hpp"
#include "event/irc/EventIrcModifyNick.hpp"
#include "event/irc/EventIrcReconnectServer.hpp"
#include "event/irc/EventIrcJoinChannel.hpp"
#include "event/irc/EventIrcPartChannel.hpp"
#include "event/irc/EventIrcChangeNick.hpp"
#include "event/irc/EventIrcRequestBacklog.hpp"
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
#pragma message "onConnect stub"
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
            string protocol = root.get("protocol", "").asString();
            if (protocol == "") {
                if (cmd == "querysettings") {
                    appQueue->sendEvent(make_shared<EventQuerySettings>(clientData.userId, connection));
                }
            } else if (protocol == "irc") {
                if (cmd == "chat") {
                    size_t serverId;
                    istringstream(root.get("server", "0").asString()) >> serverId;
                    string channel = root.get("channel", "").asString();
                    string message = root.get("msg", "").asString();
                    appQueue->sendEvent(make_shared<EventIrcSendMessage>(clientData.userId, serverId, channel, message, MessageType::Message));
                } else if (cmd == "requestbacklog") {
                    size_t serverId, fromId;
                    istringstream(root.get("server", "0").asString()) >> serverId;
                    string channel = root.get("channel", "").asString();
                    istringstream(root.get("channel", "").asString()) >> fromId;
                    int count = root.get("channel", "").asInt();
                    appQueue->sendEvent(make_shared<EventIrcRequestBacklog>(clientData.userId, serverId, channel, fromId, count));
                } else if (cmd == "action") {
                    size_t serverId;
                    istringstream(root.get("server", "0").asString()) >> serverId;
                    string channel = root.get("channel", "").asString();
                    string message = root.get("msg", "").asString();
                    appQueue->sendEvent(make_shared<EventIrcSendAction>(clientData.userId, serverId, channel, message));
                } else if (cmd == "join") {
                    size_t serverId;
                    istringstream(root.get("server", "0").asString()) >> serverId;
                    string channel = root.get("channel", "").asString();
                    string password = root.get("password", "").asString();
                    auto join = make_shared<EventIrcJoinChannel>(clientData.userId, serverId);
                    join->addLoginData(channel, password);
                    appQueue->sendEvent(join);
                } else if (cmd == "part") {
                    size_t serverId;
                    istringstream(root.get("server", "0").asString()) >> serverId;
                    string channel = root.get("channel", "").asString();
                    auto part = make_shared<EventIrcPartChannel>(clientData.userId, serverId);
                    part->addChannel(channel);
                    appQueue->sendEvent(part);
                } else if (cmd == "nick") {
                    size_t serverId;
                    istringstream(root.get("server", "0").asString()) >> serverId;
                    string nick = root.get("nick", "").asString();
                    appQueue->sendEvent(make_shared<EventIrcChangeNick>(clientData.userId, serverId, nick));
                } else if (cmd == "addserver") {
                    string name = root.get("name", "").asString();
                    appQueue->sendEvent(make_shared<EventIrcAddServer>(clientData.userId, name));
                } else if (cmd == "deleteserver") {
                    size_t serverId;
                    istringstream(root.get("id", "0").asString()) >> serverId;
                    if (serverId != 0)
                        appQueue->sendEvent(make_shared<EventIrcDeleteServer>(clientData.userId, serverId));
                } else if (cmd == "addhost") {
                    size_t serverId;
                    istringstream(root.get("server", "0").asString()) >> serverId;
                    string host = root.get("host", "").asString();
                    string password = root.get("password", "").asString();
                    int port = root.get("port", -1).asInt();
                    bool ipV6 = root.get("ipv6", true).asBool();
                    bool ssl = root.get("ssl", true).asBool();

                    appQueue->sendEvent(make_shared<EventIrcAddHost>(clientData.userId,
                                                                     serverId,
                                                                     host,
                                                                     port,
                                                                     password,
                                                                     ipV6,
                                                                     ssl));
                } else if (cmd == "modifynick") {
                    size_t serverId;
                    istringstream(root.get("server", "0").asString()) >> serverId;
                    string oldNick = root.get("oldnick", "").asString();
                    string newNick = root.get("newnick", "").asString();

                    appQueue->sendEvent(make_shared<EventIrcModifyNick>(clientData.userId,
                                                                        serverId,
                                                                        oldNick,
                                                                        newNick));
                } else if (cmd == "reconnect") {
                    size_t serverId;
                    istringstream(root.get("server", "0").asString()) >> serverId;

                    appQueue->sendEvent(make_shared<EventIrcReconnectServer>(clientData.userId,
                                                                             serverId));
                } else if (cmd == "deletehost") {
                    size_t serverId;
                    istringstream(root.get("server", "0").asString()) >> serverId;
                    string host = root.get("host", "").asString();
                    int port = root.get("port", -1).asInt();

                    appQueue->sendEvent(make_shared<EventIrcDeleteHost>(clientData.userId,
                                                                        serverId,
                                                                        host,
                                                                        port));
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
