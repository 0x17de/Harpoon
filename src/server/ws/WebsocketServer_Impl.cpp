#include "WebsocketServer.hpp"
#include "WebsocketServer_Impl.hpp"
#include "event/IUserEvent.hpp"
#include "event/EventQuit.hpp"
#include "event/irc/EventIrcMessage.hpp"
#include "event/irc/EventIrcAction.hpp"
#include "event/irc/EventIrcJoined.hpp"
#include "event/irc/EventIrcKicked.hpp"
#include "event/irc/EventIrcParted.hpp"
#include "event/irc/EventIrcChatListing.hpp"
#include "event/irc/EventIrcQuit.hpp"
#include "event/irc/EventIrcTopic.hpp"
#include "event/irc/EventIrcNickChanged.hpp"
#include "event/irc/EventIrcNoticed.hpp"
#include "event/EventLoginResult.hpp"
#include "event/EventLogout.hpp"
#include "event/EventQueryChats.hpp"
#include <iostream>
#include <sstream>
#include <json/json.h>

#ifdef USE_WEBSOCKET_SERVER_VERBOSE
#include <seasocks/PrintfLogger.h>
#define WEBSOCKET_LOGGER_TYPE seasocks::PrintfLogger
#else
#include <seasocks/IgnoringLogger.h>
#define WEBSOCKET_LOGGER_TYPE seasocks::IgnoringLogger
#endif

using namespace std;


WebsocketServer_Impl::WebsocketServer_Impl(EventQueue* queue, EventQueue* appQueue)
:
	queue{queue},
	appQueue{appQueue},
	server{make_shared<WEBSOCKET_LOGGER_TYPE>()}
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
	} else if (eventType == EventLoginResult::uuid) {
		auto* loginResult = event->as<EventLoginResult>();
		seasocks::WebSocket* socket = (seasocks::WebSocket*)loginResult->getData();
		if (loginResult->getSuccess()) {
			addClient(loginResult->getUserId(), socket);
		} else {
			server.execute([=] {
				socket->close();
			});
		}
	} else if (eventType == EventLogout::uuid) {
		auto logout = event->as<EventLogout>();
		removeClient((seasocks::WebSocket*)logout->getData());
	}
	return true;
}

void WebsocketServer_Impl::addClient(size_t userId, seasocks::WebSocket* socket) {
	cout << "New Client: " << userId << endl;
	auto it = userToClients.find(userId);
	bool found = it != userToClients.end();
	std::list<WebsocketClientData>* dataList;
	if (!found) {
		auto dataListIt = userToClients.emplace(
			piecewise_construct,
			forward_as_tuple(userId),
			forward_as_tuple());
		dataList = &dataListIt.first->second;
	} else {
		dataList = &it->second;
	}

	dataList->emplace_back(userId, socket);
	clients.emplace(socket, (++dataList->rbegin()).base()); // iterator to last element

	appQueue->sendEvent(make_shared<EventQueryChats>(userId, socket));
}

void WebsocketServer_Impl::removeClient(seasocks::WebSocket* socket) {
	auto it = clients.find(socket);
	if (it != clients.end()) {
		auto data = it->second;
		size_t userId = data->userId;
		cout << "Remove Client: " << userId << endl;
		auto clientListIt = userToClients.find(userId);
		if (clientListIt != userToClients.end()) {
			clientListIt->second.erase(data);
			if (clientListIt->second.size() == 0)
				userToClients.erase(userId);
		}
		clients.erase(it);
	}
}

std::string WebsocketServer_Impl::eventToJson(std::shared_ptr<IEvent> event) {
	Json::Value root{Json::objectValue};

	UUID eventType = event->getEventUuid();
	if (eventType == EventIrcMessage::uuid) {
		cout << "eventToJson => IrcMessage" << endl;
		auto message = event->as<EventIrcMessage>();
		root["cmd"] = "chat";
		root["type"] = "irc";
		// jsonpp has no clue of size_t and clients only need to store it
		root["server"] = to_string(message->getServerId());
		root["channel"] = message->getChannel();
		root["nick"] = message->getFrom();
		root["msg"] = message->getMessage();
	} else if (eventType == EventIrcChatListing::uuid) {
		cout << "eventToJson => IrcChatListing" << endl;
		auto listing = event->as<EventIrcChatListing>();
		root["cmd"] = "chatlist";
		root["type"] = "irc";
		Json::Value& serverList = root["servers"];
		for (auto& serverData : listing->getServerList()) {
			Json::Value& server = serverList[to_string(serverData.getServerId())];
			server["name"] = serverData.getServerName();
			Json::Value& channelList = server["channels"];
			for (auto& channelData : serverData.getChannels()) {
				Json::Value& channel = channelList[channelData.getChannelName()] = Json::objectValue;
				Json::Value& users = channel["users"] = Json::objectValue;
				for (auto& user : channelData.getUsers()) {
					users[user.getNick()] = user.getMode();
				}
			}
		}
	} else if (eventType == EventIrcJoined::uuid) {
		cout << "eventToJson => IrcJoined" << endl;
		auto join = event->as<EventIrcJoined>();
		root["cmd"] = "join";
		root["type"] = "irc";
		root["server"] = to_string(join->getServerId());
		root["nick"] = join->getUsername();
		root["channel"] = join->getChannel();
	} else if (eventType == EventIrcParted::uuid) {
		cout << "eventToJson => IrcParted" << endl;
		auto part = event->as<EventIrcParted>();
		root["cmd"] = "part";
		root["type"] = "irc";
		root["server"] = to_string(part->getServerId());
		root["nick"] = part->getUsername();
		root["channel"] = part->getChannel();
	} else if (eventType == EventIrcQuit::uuid) {
		cout << "eventToJson => IrcQuit" << endl;
	        auto quit = event->as<EventIrcQuit>();
		root["cmd"] = "quit";
		root["type"] = "irc";
		root["server"] = to_string(quit->getServerId());
		root["nick"] = quit->getWho();
		root["reason"] = quit->getReason();
	} else if (eventType == EventIrcTopic::uuid) {
		cout << "eventToJson => IrcTopic" << endl;
		auto topic = event->as<EventIrcTopic>();
		root["cmd"] = "topic";
		root["type"] = "irc";
		root["server"] = to_string(topic->getServerId());
		root["nick"] = topic->getUsername();
		root["topic"] = topic->getTopic();
		root["channel"] = topic->getChannel();
	} else if (eventType == EventIrcNickChanged::uuid) {
		cout << "eventToJson => IrcNickChanged" << endl;
		auto nick = event->as<EventIrcNickChanged>();
		root["cmd"] = "nickchange";
		root["type"] = "irc";
		root["server"] = to_string(nick->getServerId());
		root["nick"] = nick->getUsername();
		root["newNick"] = nick->getNewNick();
	} else if (eventType == EventIrcAction::uuid) {
		cout << "eventToJson => IrcAction" << endl;
		auto action = event->as<EventIrcAction>();
		root["cmd"] = "action";
		root["type"] = "irc";
		root["server"] = to_string(action->getServerId());
		root["nick"] = action->getUsername();
		root["msg"] = action->getMessage();
		root["channel"] = action->getChannel();
	} else if (eventType == EventIrcKicked::uuid) {
		cout << "eventToJson => IrcKicked" << endl;
		auto kick = event->as<EventIrcKicked>();
		root["cmd"] = "kick";
		root["type"] = "irc";
		root["server"] = to_string(kick->getServerId());
		root["nick"] = kick->getUsername();
		root["target"] = kick->getTarget();
		root["msg"] = kick->getReason();
		root["channel"] = kick->getChannel();
	} else if (eventType == EventIrcNoticed::uuid) {
		cout << "eventToJson => IrcNoticed" << endl;
		auto notice = event->as<EventIrcNoticed>();
		root["cmd"] = "notice";
		root["type"] = "irc";
		root["server"] = to_string(notice->getServerId());
		root["channel"] = notice->getTarget();
		root["nick"] = notice->getUsername();
		root["msg"] = notice->getMessage();		
	}

	return Json::FastWriter{}.write(root);
}

void WebsocketServer_Impl::sendEventToUser(std::shared_ptr<IEvent> event) {
	auto userEvent = event->as<IClientEvent>();
	if (userEvent == nullptr) return;
	auto it = userToClients.find(userEvent->getUserId());
	if (it != userToClients.end()) {
		std::string json = eventToJson(event);
		list<WebsocketClientData>& clientDataList = it->second;
		auto singleClientEvent = event->as<ISingleClientEvent>();
		if (singleClientEvent) {
			void* data = singleClientEvent->getData();
			for (auto& clientData : clientDataList) {
				if (clientData.socket == data) {
					server.execute([=] {
						clientData.socket->send(reinterpret_cast<const uint8_t*>(json.c_str()), json.size());
					});
				}
			}
		} else {
			for (auto& clientData : clientDataList) {
				server.execute([=] {
					clientData.socket->send(reinterpret_cast<const uint8_t*>(json.c_str()), json.size());
				});
			}
		}
	}
}

