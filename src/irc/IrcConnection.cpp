#include "IrcConnection.h"
#include "IrcConnection_Impl.h"
#include "event/EventQuit.h"
#include "event/EventIrcJoinChannel.h"
#include "event/EventIrcPartChannel.h"
#include <map>

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

IrcConnection::IrcConnection(EventQueue* appQueue, size_t serverId)
:
	impl{new IrcConnection_Impl()},
	EventLoop({
		EventQuit::uuid,
		EventIrcJoinChannel::uuid
	}),
	appQueue{appQueue},
	serverId{serverId}
{
	irc_callbacks_t callbacks;
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

	// callbacks.irc_eventcode_callback_t event_numeric = &onIrcEvent<&IrcConnection_Impl::onNumeric>
	// callbacks.irc_event_dcc_chat_t event_dcc_chat_req = &onIrcEvent<&IrcConnection_Impl::onReq>
	// callbacks.irc_event_dcc_send_t event_dcc_send_req = &onIrcEvent<&IrcConnection_Impl::onReq>

	impl->ircSession = irc_create_session(&callbacks);
	if (impl->ircSession != 0) {
		activeIrcConnections.emplace(impl->ircSession, impl.get());
	}
}

IrcConnection::~IrcConnection() {
	if (impl->ircSession != 0) {
		activeIrcConnections.erase(impl->ircSession);
		irc_destroy_session(impl->ircSession);
	}
}

bool IrcConnection::onEvent(std::shared_ptr<IEvent> event) {
	return impl->onEvent(event);
}

bool IrcConnection_Impl::onEvent(std::shared_ptr<IEvent> event) {
	UUID type = event->getEventUuid();
	if (type == EventQuit::uuid) {
		return false;
	} else if (type == EventIrcJoinChannel::uuid) {
		lock_guard<mutex> lock(channelLoginDataMutex);
		EventIrcJoinChannel* joinCommand = dynamic_cast<EventIrcJoinChannel*>(event.get());
		for (auto& entry : joinCommand->getLoginData()) {
			auto it = channelLoginData.find(entry.channelId);
			if (it != channelLoginData.end())
				continue;
			irc_cmd_join(ircSession, it->second.channel.c_str(), it->second.password.c_str());
			channelLoginData.emplace(entry.channelId, entry);
		}
	} else if (type == EventIrcPartChannel::uuid) {
		lock_guard<mutex> lock(channelLoginDataMutex);
		EventIrcPartChannel* partCommand = dynamic_cast<EventIrcPartChannel*>(event.get());
		for (size_t channelId : partCommand->getChannelIds()) {
			auto it = channelLoginData.find(channelId);
			if (it == channelLoginData.end())
				continue;
			irc_cmd_part(ircSession, it->second.channel.c_str());
			channelLoginData.erase(it);
		}
	}
	return true;
}

