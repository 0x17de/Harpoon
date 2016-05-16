#include "IrcConnection.hpp"
#include "IrcConnection_Impl.hpp"
#include "event/EventQuit.hpp"
#include "event/EventActivateUser.hpp"
#include "event/EventIrcJoinChannel.hpp"
#include "event/EventIrcPartChannel.hpp"
#include <iostream>
#include <map>
#include <thread>
#include <chrono>

using namespace std;


static map<irc_session_t*, IrcConnection_Impl*> activeIrcConnections;

template <void (IrcConnection_Impl::*F)(irc_session_t*, const char*, const char*, const char**, unsigned int)>
void onIrcEvent(irc_session_t* session,
	const char* event,
	const char* origin,
	const char** params,
	unsigned int count)
{
	IrcConnection_Impl* cxn = activeIrcConnections.at(session);
	(cxn->*F)(session, event, origin, params, count);
}
template <void (IrcConnection_Impl::*F)(irc_session_t*, unsigned int, const char*, const vector<string>&)>
void onIrcNumeric(irc_session_t* session,
	unsigned int eventCode,
	const char* origin,
	const char** params,
	unsigned int count)
{
	vector<string> parameters(count);
	for (unsigned int i = 0; i < count; ++i)
		parameters.push_back(string(params[i]));
	IrcConnection_Impl* cxn = activeIrcConnections.at(session);
	(cxn->*F)(session, eventCode, origin, parameters);
}

IrcConnection_Impl::IrcConnection_Impl(EventQueue* appQueue, size_t userId, const IrcServerConfiguration& configuration)
:
	appQueue{appQueue},
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
		callbacks.event_ctcp_req = &onIrcEvent<&IrcConnection_Impl::onCtcpReq>;
		callbacks.event_ctcp_rep = &onIrcEvent<&IrcConnection_Impl::onCtcpRep>;
		callbacks.event_ctcp_action = &onIrcEvent<&IrcConnection_Impl::onCtcpAction>;
		callbacks.event_unknown = &onIrcEvent<&IrcConnection_Impl::onUnknown>;

		callbacks.event_numeric = &onIrcNumeric<&IrcConnection_Impl::onNumeric>;

		// callbacks.irc_event_dcc_chat_t event_dcc_chat_req = &onIrcEvent<&IrcConnection_Impl::onReq>
		// callbacks.irc_event_dcc_send_t event_dcc_send_req = &onIrcEvent<&IrcConnection_Impl::onReq>

		ircSession = irc_create_session(&callbacks);
		// remember session
		activeIrcConnections.emplace(ircSession, this);
		cout << "[IC] Session: " << ircSession << endl;

		while (ircSession != 0 && running) {

			// copy channels to login
			for (auto& channel : this->configuration.channels)
				channelLoginData.emplace(channel.channel, channel);

			// connect to server
			irc_connect(ircSession,
				this->configuration.hostname.c_str(),
				this->configuration.port,
				this->configuration.password.empty() ? 0 : this->configuration.password.c_str(),
				this->configuration.nicks.front().c_str(),
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
	impl{new IrcConnection_Impl(appQueue, userId, configuration)},
	EventLoop({
		EventQuit::uuid,
		EventIrcJoinChannel::uuid
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
			auto it = channelLoginData.find(entry.channel);
			if (it != channelLoginData.end())
				continue;
			irc_cmd_join(ircSession, it->second.channel.c_str(), it->second.password.c_str());
			channelLoginData.emplace(entry.channel, entry);
		}
	} else if (type == EventIrcPartChannel::uuid) {
		lock_guard<mutex> lock(channelLoginDataMutex);
		auto partCommand = event->as<EventIrcPartChannel>();
		for (string channel : partCommand->getChannels()) {
			auto it = channelLoginData.find(channel);
			if (it == channelLoginData.end())
				continue;
			irc_cmd_part(ircSession, it->second.channel.c_str());
			channelLoginData.erase(it);
		}
	}
	return true;
}

