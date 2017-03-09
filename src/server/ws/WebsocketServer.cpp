#include "WebsocketServer.hpp"
#include <sstream>
#include <map>
#include <json/json.h>
#include "event/IUserEvent.hpp"
#include "event/EventQuit.hpp"
#include "event/irc/EventIrcMessage.hpp"
#include "event/irc/EventIrcAction.hpp"
#include "event/irc/EventIrcJoined.hpp"
#include "event/irc/EventIrcKicked.hpp"
#include "event/irc/EventIrcParted.hpp"
#include "event/irc/EventIrcChatListing.hpp"
#include "event/irc/EventIrcSettingsListing.hpp"
#include "event/irc/EventIrcQuit.hpp"
#include "event/irc/EventIrcTopic.hpp"
#include "event/irc/EventIrcNickChanged.hpp"
#include "event/irc/EventIrcServerAdded.hpp"
#include "event/irc/EventIrcServerDeleted.hpp"
#include "event/irc/EventIrcHostAdded.hpp"
#include "event/irc/EventIrcHostDeleted.hpp"
#include "event/irc/EventIrcUserlistReceived.hpp"
#include "event/irc/EventIrcNickModified.hpp"
#include "event/irc/EventIrcModeChanged.hpp"
#include "event/irc/EventIrcBacklogResponse.hpp"
#include "event/EventLoginResult.hpp"
#include "event/EventLogout.hpp"
#include "event/EventQuery.hpp"
#include "event/EventQueryType.hpp"
#include "service/irc/IrcDatabaseMessageType.hpp"
#include "utils/ModuleProvider.hpp"


#ifdef USE_WEBSOCKET_SERVER_VERBOSE
#include <seasocks/PrintfLogger.h>
#define WEBSOCKET_LOGGER_TYPE seasocks::PrintfLogger
#else
#include <seasocks/IgnoringLogger.h>
#define WEBSOCKET_LOGGER_TYPE seasocks::IgnoringLogger
#endif

using namespace std;


const std::map<IrcDatabaseMessageType, std::string> databaseMessageTypeToName {
    { IrcDatabaseMessageType::Message, "msg" },
    { IrcDatabaseMessageType::Join, "join" },
    { IrcDatabaseMessageType::Part, "part" },
    { IrcDatabaseMessageType::Quit, "quit" },
    { IrcDatabaseMessageType::Kick, "kick" },
    { IrcDatabaseMessageType::Notice, "notice" },
    { IrcDatabaseMessageType::Action, "action" }
};


WebsocketClientData::WebsocketClientData(size_t userId, seasocks::WebSocket* socket)
    : userId{userId}
    , socket{socket}
{
}


PROVIDE_MODULE("server", "websocket", WebsocketServer)



WebsocketServer::WebsocketServer(EventQueue* appQueue)
    : appQueue{appQueue}
    , server{make_shared<WEBSOCKET_LOGGER_TYPE>()}
{
    server.addWebSocketHandler("/ws", make_shared<WebsocketHandler>(appQueue, getEventQueue(), clients));
    serverThread = thread([this]{
        server.serve("public", 8080);
    });
}

WebsocketServer::~WebsocketServer() {
    server.terminate();
    serverThread.join();
}

bool WebsocketServer::onEvent(std::shared_ptr<IEvent> event) {
    UUID eventType = event->getEventUuid();

    if (eventType == EventQuit::uuid) {
        return false;
    } else if (eventType == EventLoginResult::uuid) {
        auto* loginResult = event->as<EventLoginResult>();
        auto socket = (seasocks::WebSocket*)loginResult->getData();
        if (loginResult->getSuccess()) {
            addClient(loginResult->getUserId(), socket);
        } else {
            server.execute([socket] {
                socket->close();
            });
        }
    }

    auto userEvent = event->as<IClientEvent>();
    if (userEvent) {
        auto it = userToClients.find(userEvent->getUserId());
        if (it != userToClients.end()) {
            std::string json = eventToJson(event);
            if (json.size() > 0) {
                list<WebsocketClientData>& clientDataList = it->second;
                auto singleClientEvent = event->as<ISingleClientEvent>();
                if (singleClientEvent) {
                    void* data = singleClientEvent->getData();
                    for (auto& clientData : clientDataList) {
                        if (clientData.socket == data) {
                            auto socket = clientData.socket;
                            server.execute([socket, json] {
                                    socket->send(reinterpret_cast<const uint8_t*>(json.c_str()), json.size());
                                });
                        }
                    }
                } else {
                    for (auto& clientData : clientDataList) {
                        auto socket = clientData.socket;
                        server.execute([socket, json] {
                                socket->send(reinterpret_cast<const uint8_t*>(json.c_str()), json.size());
                            });
                    }
                }
            }
        }
    }

    if (eventType == EventLogout::uuid) {
        auto logout = event->as<EventLogout>();
        removeClient((seasocks::WebSocket*)logout->getData());
    }
    return true;
}

void WebsocketServer::addClient(size_t userId, seasocks::WebSocket* socket) {
    auto it = userToClients.find(userId);
    bool found = it != userToClients.end();
    std::list<WebsocketClientData>* dataList;
    if (!found) {
        auto dataListIt = userToClients.emplace(piecewise_construct,
                                                forward_as_tuple(userId),
                                                forward_as_tuple());
        dataList = &dataListIt.first->second;
    } else {
        dataList = &it->second;
    }

    dataList->emplace_back(userId, socket);
    clients.emplace(socket, (++dataList->rbegin()).base()); // iterator to last element

    appQueue->sendEvent(make_shared<EventQuery>(userId, socket, EventQueryType::Chats));
}

void WebsocketServer::removeClient(seasocks::WebSocket* socket) {
    auto it = clients.find(socket);
    if (it != clients.end()) {
        auto data = it->second;
        size_t userId = data->userId;
        auto clientListIt = userToClients.find(userId);
        if (clientListIt != userToClients.end()) {
            clientListIt->second.erase(data);
            if (clientListIt->second.size() == 0)
                userToClients.erase(userId);
        }
        clients.erase(it);
    }
}

std::string WebsocketServer::eventToJson(std::shared_ptr<IEvent> event) {
    Json::Value root{Json::objectValue};

    auto loggable = event->as<IrcLoggable>();
    auto id = loggable == nullptr ? 0 : loggable->getLogEntryId();

    UUID eventType = event->getEventUuid();
    switch(eventType) {
    case EventIrcMessage::uuid: {
        auto message = event->as<EventIrcMessage>();
        root["cmd"] = "chat";
        root["protocol"] = "irc";
        root["id"] = to_string(id);
        // jsonpp has no clue of size_t and clients only need to store it
        root["server"] = to_string(message->getServerId());
        root["channel"] = message->getChannel();
        root["nick"] = message->getFrom();
        root["msg"] = message->getMessage();
        root["type"] = static_cast<int>(message->getType());
        break;
    }
    case EventLoginResult::uuid: {
        auto result = event->as<EventLoginResult>();
        root["cmd"] = "login";
        root["success"] = result->getSuccess();
        break;
    }
    case EventIrcUserlistReceived::uuid: {
        auto userlist = event->as<EventIrcUserlistReceived>();
        root["cmd"] = "userlist";
        root["protocol"] = "irc";
        root["server"] = to_string(userlist->getServerId());
        root["channel"] = userlist->getChannel();
        auto& users = root["users"] = Json::objectValue;
        for (auto& user : userlist->getUsers())
            users[user.nick] = user.mode;
        break;
    }
    case EventIrcServerAdded::uuid: {
        auto added = event->as<EventIrcServerAdded>();
        root["cmd"] = "serveradded";
        root["protocol"] = "irc";
        root["name"] = added->getServerName();
        root["server"] = to_string(added->getServerId());
        break;
    }
    case EventIrcServerDeleted::uuid: {
        auto added = event->as<EventIrcServerDeleted>();
        root["cmd"] = "serverremoved";
        root["protocol"] = "irc";
        root["server"] = to_string(added->getServerId());
        break;
    }
    case EventIrcHostAdded::uuid: {
        auto added = event->as<EventIrcHostAdded>();
        root["cmd"] = "hostadded";
        root["protocol"] = "irc";
        root["server"] = to_string(added->getServerId());
        root["host"] = added->getHost();
        root["hasPassword"] = added->getPassword().size() > 0;
        root["port"] = added->getPort();
        root["ipv6"] = added->getIpV6();
        root["ssl"] = added->getSsl();
        break;
    }
    case EventIrcHostDeleted::uuid: {
        auto deleted = event->as<EventIrcHostDeleted>();
        root["cmd"] = "hostdeleted";
        root["protocol"] = "irc";
        root["server"] = to_string(deleted->getServerId());
        root["host"] = deleted->getHost();
        root["port"] = deleted->getPort();
        break;
    }
    case EventIrcNickModified::uuid: {
        auto modified = event->as<EventIrcNickModified>();
        root["cmd"] = "nickmodified";
        root["protocol"] = "irc";
        root["server"] = to_string(modified->getServerId());
        root["oldnick"] = modified->getOldNick();
        root["newnick"] = modified->getNewNick();
        break;
    }
    case EventIrcSettingsListing::uuid: {
        auto settings = event->as<EventIrcSettingsListing>();
        root["cmd"] = "settings";
        root["protocol"] = "irc";
        Json::Value& data = root["data"] = Json::objectValue;
        Json::Value& servers = data["servers"] = Json::objectValue;
        for (auto& serverConfiguration : settings->getServerList()) {
            Json::Value& server = servers[to_string(serverConfiguration.getServerId())];
            server["name"] = serverConfiguration.getServerName();
            Json::Value& hosts = server["hosts"] = Json::objectValue;
            for (auto& hostConfiguration : serverConfiguration.getHostConfigurations()) {
                stringstream hostKey;
                hostKey << hostConfiguration.getHostName() << ":" << hostConfiguration.getPort();
                Json::Value& host = hosts[hostKey.str()] = Json::objectValue;
                host["hasPassword"] = !hostConfiguration.getPassword().empty();
                host["ipv6"] = hostConfiguration.getIpV6();
                host["ssl"] = hostConfiguration.getSsl();
            }
            Json::Value& nicks = server["nicks"] = Json::arrayValue;
            for (auto& nick : serverConfiguration.getNicks()) {
                nicks.append(nick);
            }
            Json::Value& channels = server["channels"] = Json::arrayValue;
            for (auto& channelConfiguration : serverConfiguration.getChannelLoginData()) {
                Json::Value& channel = channels[channelConfiguration.getChannelName()];
                channel["hasPassword"] = !channelConfiguration.getChannelPassword().empty();
            }
        }
        break;
    }
    case EventIrcChatListing::uuid: {
        auto listing = event->as<EventIrcChatListing>();
        root["cmd"] = "chatlist";
        root["protocol"] = "irc";
        root["firstId"] = to_string(listing->getFirstId());
        Json::Value& serverList = root["servers"] = Json::objectValue;
        for (auto& serverData : listing->getServerList()) {
            Json::Value& server = serverList[to_string(serverData.getServerId())];
            server["nick"] = serverData.getActiveNick();
            server["name"] = serverData.getServerName();
            Json::Value& channelList = server["channels"] = Json::objectValue;
            for (auto& channelData : serverData.getChannels()) {
                Json::Value& channel = channelList[channelData.getChannelName()] = Json::objectValue;
                if (channelData.getDisabled())
                    channel["disabled"] = true;
                channel["topic"] = channelData.getChannelTopic();
                Json::Value& users = channel["users"] = Json::objectValue;
                for (auto& user : channelData.getUsers()) {
                    users[user.getNick()] = user.getMode();
                }
            }
        }
        break;
    }
    case EventIrcJoined::uuid: {
        auto join = event->as<EventIrcJoined>();
        root["cmd"] = "join";
        root["protocol"] = "irc";
        root["id"] = to_string(id);
        root["server"] = to_string(join->getServerId());
        root["nick"] = join->getUsername();
        root["channel"] = join->getChannel();
        break;
    }
    case EventIrcParted::uuid: {
        auto part = event->as<EventIrcParted>();
        root["cmd"] = "part";
        root["protocol"] = "irc";
        root["id"] = to_string(id);
        root["server"] = to_string(part->getServerId());
        root["nick"] = part->getUsername();
        root["channel"] = part->getChannel();
        break;
    }
    case EventIrcQuit::uuid: {
        auto quit = event->as<EventIrcQuit>();
        root["cmd"] = "quit";
        root["protocol"] = "irc";
        root["id"] = to_string(id);
        root["server"] = to_string(quit->getServerId());
        root["nick"] = quit->getWho();
        root["reason"] = quit->getReason();
        break;
    }
    case EventIrcTopic::uuid: {
        auto topic = event->as<EventIrcTopic>();
        root["cmd"] = "topic";
        root["protocol"] = "irc";
        root["id"] = to_string(id);
        root["server"] = to_string(topic->getServerId());
        root["nick"] = topic->getUsername();
        root["topic"] = topic->getTopic();
        root["channel"] = topic->getChannel();
        break;
    }
    case EventIrcNickChanged::uuid: {
        auto nick = event->as<EventIrcNickChanged>();
        root["cmd"] = "nickchange";
        root["protocol"] = "irc";
        root["id"] = to_string(id);
        root["server"] = to_string(nick->getServerId());
        root["nick"] = nick->getUsername();
        root["newNick"] = nick->getNewNick();
        break;
    }
    case EventIrcAction::uuid: {
        auto action = event->as<EventIrcAction>();
        root["cmd"] = "action";
        root["protocol"] = "irc";
        root["id"] = to_string(id);
        root["server"] = to_string(action->getServerId());
        root["nick"] = action->getUsername();
        root["msg"] = action->getMessage();
        root["channel"] = action->getChannel();
        break;
    }
    case EventIrcModeChanged::uuid: {
        auto mode = event->as<EventIrcModeChanged>();
        root["cmd"] = "mode";
        root["protocol"] = "irc";
        root["id"] = to_string(id);
        root["server"] = to_string(mode->getServerId());
        root["channel"] = mode->getChannel();
        root["nick"] = mode->getUsername();
        root["mode"] = mode->getMode();
        auto& args = root["args"] = Json::arrayValue;
        for (auto& s : mode->getArgs())
            args.append(s);
        break;
    }
    case EventIrcKicked::uuid: {
        auto kick = event->as<EventIrcKicked>();
        root["cmd"] = "kick";
        root["protocol"] = "irc";
        root["id"] = to_string(id);
        root["server"] = to_string(kick->getServerId());
        root["nick"] = kick->getUsername();
        root["target"] = kick->getTarget();
        root["msg"] = kick->getReason();
        root["channel"] = kick->getChannel();
        break;
    }
    case EventIrcBacklogResponse::uuid: {
        auto response = event->as<EventIrcBacklogResponse>();

        root["cmd"] = "backlogresponse";
        root["protocol"] = "irc";
        root["server"] = to_string(response->getServerId());
        root["channel"] = response->getChannel();

        auto& data = response->getData();
        auto& lines = root["lines"] = Json::arrayValue;
        lines.resize(data.size());
        int i = 0;
        for (auto& entry : data) {
            auto& line = lines[i] = Json::objectValue;
            line["id"] = to_string(entry.messageId);
            line["time"] = static_cast<double>(entry.time);
            line["msg"] = entry.message;
            line["sender"] = entry.sender;

            auto it = databaseMessageTypeToName.find(entry.type);
            if (it == databaseMessageTypeToName.end()) {
                line["type"] = "unknown";
            } else {
                line["type"] = it->second;
            }

            ++i;
        }

        break;
    }
    default:
        return "";
    } // switch(eventType)
    root["time"] = (Json::UInt64)chrono::duration_cast<chrono::milliseconds>(event->getTimestamp().time_since_epoch()).count();
    return Json::FastWriter{}.write(root);
}
