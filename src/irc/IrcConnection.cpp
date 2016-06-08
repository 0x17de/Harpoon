#include "IrcConnection.hpp"
#include "IrcConnection_Impl.hpp"
#include "event/EventQuit.hpp"
#include "event/IActivateServiceEvent.hpp"
#include "event/irc/EventIrcJoinChannel.hpp"
#include "event/irc/EventIrcPartChannel.hpp"
#include "event/irc/EventIrcSendMessage.hpp"
#include "event/irc/EventIrcMessage.hpp"
#include "event/irc/EventIrcNumeric.hpp"
#include <algorithm>
#include <iostream>
#include <sstream>
#include <map>
#include <thread>
#include <chrono>
#include <libirc_rfcnumeric.h>

using namespace std;


static map<irc_session_t*, IrcConnection_Impl*> activeIrcConnections;

template <void (IrcConnection_Impl::*F)(irc_session_t*, const char*, const char*, const vector<string>&, std::shared_ptr<IEvent>&)>
void onIrcEvent(irc_session_t* session,
	const char* event,
	const char* origin,
	const char** inparameters,
	unsigned int count)
{
	vector<string> parameters;
	for (unsigned int i = 0; i < count; ++i)
		parameters.push_back(string(inparameters[i]));

	IrcConnection_Impl* cxn = activeIrcConnections.at(session);
	shared_ptr<IEvent> resultEvent;
	(cxn->*F)(session, event, origin, parameters, resultEvent);
	if (resultEvent) {
		cxn->queue->sendEvent(resultEvent);
		cxn->appQueue->sendEvent(resultEvent);
	}
}
template <void (IrcConnection_Impl::*F)(irc_session_t*, unsigned int, const char*, const vector<string>&, std::shared_ptr<IEvent>&)>
void onIrcNumeric(irc_session_t* session,
	unsigned int eventCode,
	const char* origin,
	const char** inparameters,
	unsigned int count)
{
	vector<string> parameters;
	for (unsigned int i = 0; i < count; ++i)
		parameters.push_back(string(inparameters[i]));

	IrcConnection_Impl* cxn = activeIrcConnections.at(session);
	shared_ptr<IEvent> resultEvent;
	(cxn->*F)(session, eventCode, origin, parameters, resultEvent);
	if (resultEvent) {
		cxn->queue->sendEvent(resultEvent);
		cxn->appQueue->sendEvent(resultEvent);
	}
}

bool IrcConnection_Impl::findUnusedNick(std::string& nick) {
	auto& nicks = this->configuration.nicks;
	auto nickIt = find_if(nicks.begin(), nicks.end(), [this](std::string& foundNick) {
		return inUseNicks.find(foundNick) == inUseNicks.end(); // not used
	});
	bool success = nickIt != nicks.end();
	if (success) nick = *nickIt; // assign nick
	return success;
}

IrcConnection_Impl::IrcConnection_Impl(EventQueue* appQueue, EventQueue* queue, size_t userId, const IrcServerConfiguration& configuration)
:
	appQueue{appQueue},
	queue{queue},
	userId{userId},
	configuration{configuration},
	running{true}
{
	ircLoop = thread([this]{ 
		irc_callbacks_t callbacks = {0};
		callbacks.event_connect = &onIrcEvent<&IrcConnection_Impl::onConnect>;
		callbacks.event_nick = &onIrcEvent<&IrcConnection_Impl::onNick>;
		callbacks.event_quit = &onIrcEvent<&IrcConnection_Impl::onQuit>;
		callbacks.event_join = &onIrcEvent<&IrcConnection_Impl::onJoin>;
		callbacks.event_part = &onIrcEvent<&IrcConnection_Impl::onPart>;
		callbacks.event_mode = &onIrcEvent<&IrcConnection_Impl::onMode>;
		callbacks.event_umode = &onIrcEvent<&IrcConnection_Impl::onUmode>;
		callbacks.event_topic = &onIrcEvent<&IrcConnection_Impl::onTopic>;
		callbacks.event_kick = &onIrcEvent<&IrcConnection_Impl::onKick>;
		callbacks.event_channel = &onIrcEvent<&IrcConnection_Impl::onChannel>;
		callbacks.event_privmsg = &onIrcEvent<&IrcConnection_Impl::onPrivmsg>;
		callbacks.event_notice = &onIrcEvent<&IrcConnection_Impl::onNotice>;
		callbacks.event_channel_notice = &onIrcEvent<&IrcConnection_Impl::onNotice>;
		callbacks.event_invite = &onIrcEvent<&IrcConnection_Impl::onInvite>;
		//callbacks.event_ctcp_req = &onIrcEvent<&IrcConnection_Impl::onCtcpReq>;
		//callbacks.event_ctcp_rep = &onIrcEvent<&IrcConnection_Impl::onCtcpRep>;
		callbacks.event_ctcp_action = &onIrcEvent<&IrcConnection_Impl::onCtcpAction>;
		callbacks.event_unknown = &onIrcEvent<&IrcConnection_Impl::onUnknown>;

		callbacks.event_numeric = &onIrcNumeric<&IrcConnection_Impl::onNumeric>;

		// callbacks.irc_event_dcc_chat_t event_dcc_chat_req = &onIrcEvent<&IrcConnection_Impl::onReq>
		// callbacks.irc_event_dcc_send_t event_dcc_send_req = &onIrcEvent<&IrcConnection_Impl::onReq>

		ircSession = irc_create_session(&callbacks);
		// remember session
		activeIrcConnections.emplace(ircSession, this);

		while (ircSession != 0 && running) {
			{
				lock_guard<mutex> lock(channelLoginDataMutex);
				// copy channels to login
				for (auto& channel : this->configuration.channels)
					channelLoginData.emplace(channel.getChannelName(), channel);

				if (!findUnusedNick(nick)) break; // give up
			}

			// connect to server
			irc_connect(ircSession,
				this->configuration.hostname.c_str(),
				this->configuration.port,
				this->configuration.password.empty() ? 0 : this->configuration.password.c_str(),
				nick.c_str(),
				0 /* username */,
				0 /* realname */);
			cout << "[IC] Connect" << endl;

			// run irc event loop
			irc_run(ircSession);
			cout << "[IC] Loop finished" << endl;

			irc_disconnect(ircSession);

			if (running) {
				// reconnect
				this_thread::sleep_for(chrono::seconds(1));
			}
		}
	});
}

IrcConnection_Impl::~IrcConnection_Impl() {
	if (ircSession != 0) {
		irc_cmd_quit(ircSession, 0);
		ircLoop.join();
		activeIrcConnections.erase(ircSession);
		irc_destroy_session(ircSession);
	}
}

IrcConnection::IrcConnection(EventQueue* appQueue, size_t userId, const IrcServerConfiguration& configuration)
:
	impl{new IrcConnection_Impl(appQueue, getEventQueue(), userId, configuration)},
	EventLoop({
		EventQuit::uuid,
		EventIrcJoinChannel::uuid,
		EventIrcSendMessage::uuid
	})
{
}

IrcConnection::~IrcConnection() {
}

bool IrcConnection::onEvent(std::shared_ptr<IEvent> event) {
	return impl->onEvent(event);
}

bool IrcConnection_Impl::onEvent(std::shared_ptr<IEvent> event) {
	UUID type = event->getEventUuid();
	if (type == EventQuit::uuid) {
		running = false;
		cout << "[IC] received QUIT" << endl;
		return false;
	} else if (type == EventIrcJoinChannel::uuid) {
		lock_guard<mutex> lock(channelLoginDataMutex);
		auto joinCommand = event->as<EventIrcJoinChannel>();
		for (auto& entry : joinCommand->getLoginData()) {
			auto it = channelLoginData.find(entry.getChannelName());
			if (it != channelLoginData.end())
				continue;
			IrcChannelLoginData& loginData = it->second;
			irc_cmd_join(ircSession,
				loginData.getChannelName().c_str(),
				loginData.getChannelPassword().c_str());
			channelLoginData.emplace(entry.getChannelName(), entry);
		}
	} else if (type == EventIrcNumeric::uuid) {
		auto num = event->as<EventIrcNumeric>();
		unsigned int code = num->getEventCode();
       		if (code == LIBIRC_RFC_ERR_NICKNAMEINUSE) {
			lock_guard<mutex> lock(channelLoginDataMutex);
			cout << "Nick in use: " << nick << endl;
			inUseNicks.emplace(nick);
			if (findUnusedNick(nick))
				irc_cmd_nick(ircSession, nick.c_str());
		} else if (code == LIBIRC_RFC_RPL_NAMREPLY) {
			lock_guard<mutex> lock(channelLoginDataMutex);
			auto num = event->as<EventIrcNumeric>();
			auto& parameters = num->getParameters();

			string channelName = parameters.at(1);
			if (channelName.size() > 0) {
				char channelMode = parameters.at(0).at(0);
				if (channelMode == '='
				 || channelMode == '*'
				 || channelMode == '@') {
#pragma warning channel mode stub
				}
	
				auto it = channelUsers.find(channelName);
				IrcChannelStore* channelStore;
				// find userdata
				if (it == channelUsers.end()) {
					auto insertResult = channelUsers.emplace(piecewise_construct,
						forward_as_tuple(channelName),
						forward_as_tuple());
					channelStore = &insertResult.first->second;
				} else {
					channelStore = &it->second;
				}
				// assign users from parameter
				channelStore->clear();
				istringstream users(parameters.at(2));
				string user;
				while (getline(users, user, ' ')) {
					if (user.size() == 0) continue;
					char userMode = user.at(0);
					string mode = "";
					if (userMode == '@'
					 || userMode == '+') {
#pragma warning user mode stub
						mode = "";
						user = user.substr(1);
					}
					channelStore->addUser(user, "");
				}
			}
		} else if (code == LIBIRC_RFC_RPL_ENDOFNAMES) {
			lock_guard<mutex> lock(channelLoginDataMutex);
			auto num = event->as<EventIrcNumeric>();
			auto& parameters = num->getParameters();
			string channelName = parameters.at(0);
			auto it = channelUsers.find(channelName);
			if (it != channelUsers.end()) {
				auto& channelStore = it->second;
				auto& users = channelStore.getUsers();
				for (auto userStorePair : users) {
					auto& userStore = userStorePair.second;
#pragma warning submit userlist stub
				}
			}
		}
	} else if (type == EventIrcPartChannel::uuid) {
		lock_guard<mutex> lock(channelLoginDataMutex);
		auto partCommand = event->as<EventIrcPartChannel>();
		for (string channel : partCommand->getChannels()) {
			auto it = channelLoginData.find(channel);
			if (it == channelLoginData.end())
				continue;
			irc_cmd_part(ircSession, it->second.getChannelName().c_str());
			channelLoginData.erase(it);
		}
	} else if (type == EventIrcSendMessage::uuid) {
		auto message = event->as<EventIrcSendMessage>();
		if (!irc_cmd_msg(ircSession, message->getChannel().c_str(), message->getMessage().c_str())) {
			appQueue->sendEvent(make_shared<EventIrcMessage>(message->getUserId(), message->getServerId(), "", message->getChannel(), message->getMessage()));
		}
	}
	return true;
}

size_t IrcConnection::getServerId() const {
	return impl->configuration.serverId;
}

std::string IrcConnection::getServerName() const {
	return impl->configuration.serverName;
}

std::mutex& IrcConnection::getChannelLoginDataMutex() const {
	return impl->channelLoginDataMutex;
}

const std::map<std::string, IrcChannelLoginData>& IrcConnection::getChannelLoginData() const {
	return impl->channelLoginData;
}


