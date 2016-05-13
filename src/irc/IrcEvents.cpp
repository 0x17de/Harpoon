#include "IrcConnection_Impl.hpp"
#include "queue/EventQueue.hpp"
#include "event/EventIrcConnected.hpp"
#include "event/EventIrcModeChanged.hpp"
#include "event/EventIrcUserModeChanged.hpp"
#include "event/EventIrcNickChanged.hpp"
#include "event/EventIrcJoinChannel.hpp"
#include "event/EventIrcJoined.hpp"
#include "event/EventIrcParted.hpp"
#include "event/EventIrcKicked.hpp"
#include "event/EventIrcQuit.hpp"
#include "event/EventIrcTopic.hpp"
#include "event/EventIrcMessage.hpp"
#include "event/EventIrcNoticed.hpp"
#include "event/EventIrcChannelNoticed.hpp"
#include <iostream>

using namespace std;


void IrcConnection_Impl::onConnect(irc_session_t* session,
	const char* event,
	const char* origin,
	const char** params,
	unsigned int count)
{
	lock_guard<mutex> lock(channelLoginDataMutex);
	for (auto joinData : channelLoginData) {
		irc_cmd_join(ircSession, joinData.second.channel.c_str(), joinData.second.password.empty() ? 0 : joinData.second.password.c_str());
	}
	appQueue->sendEvent(make_shared<EventIrcConnected>(userId, configuration.serverId));
}
void IrcConnection_Impl::onNick(irc_session_t* session,
	const char* event,
	const char* origin,
	const char** params,
	unsigned int count)
{
#warning stub onNick
	if (count < 1) return;
	string who(origin);
	string newNick(params[0]);
	cout << "Nickchange<" << who << ">: " << newNick << endl;
	appQueue->sendEvent(make_shared<EventIrcNickChanged>(userId, configuration.serverId, who, newNick));
}
void IrcConnection_Impl::onQuit(irc_session_t* session,
	const char* event,
	const char* origin,
	const char** params,
	unsigned int count)
{
#warning stub onQuit
	string who(origin);
	string reason = count < 1 ? "" : params[0];
	cout << "Q<" << origin << ">: " << reason << endl;
	appQueue->sendEvent(make_shared<EventIrcQuit>(userId, configuration.serverId, who, reason));
}
void IrcConnection_Impl::onJoin(irc_session_t* session,
	const char* event,
	const char* origin,
	const char** params,
	unsigned int count)
{
	if (count < 1) return;
	string who(origin);
	string channel(params[0]);
	appQueue->sendEvent(make_shared<EventIrcJoined>(userId, configuration.serverId, who, channel));
	cout << "JOIN<" << who << ">: " << channel << endl;
}
void IrcConnection_Impl::onPart(irc_session_t* session,
	const char* event,
	const char* origin,
	const char** params,
	unsigned int count)
{
#warning stub onPart
	if (count < 1) return;
	string who(origin);
	string channel(params[0]);
	string reason = count < 2 ? "" : params[1];
	appQueue->sendEvent(make_shared<EventIrcParted>(userId, configuration.serverId, who, channel));
	cout << "PART<" << who << ">: " << channel << ": " << reason << endl;
}
void IrcConnection_Impl::onMode(irc_session_t* session,
        const char* event,
        const char* origin,
        const char** params,
        unsigned int count)
{
#warning stub onMode
	if (count < 2) return;
	string who(origin);
	string channel(params[0]);
	string mode(params[1]);
	string arg = count < 3 ? "" : params[2];
	appQueue->sendEvent(make_shared<EventIrcModeChanged>(userId, configuration.serverId, who, channel, mode, arg));
	cout << "MODE<" << who << ">: " << channel << " " << mode << ": " << arg << endl;
}
void IrcConnection_Impl::onUmode(irc_session_t* session,
        const char* event,
        const char* origin,
        const char** params,
        unsigned int count)
{
#warning stub onUmode
	if (count < 2) return;
	string who(origin);
	string channel(params[0]);
	string mode(params[1]);
	appQueue->sendEvent(make_shared<EventIrcUserModeChanged>(userId, configuration.serverId, who, channel, mode));
	cout << "UMODE<" << who << ">: " << channel << " " << mode << endl;
}
void IrcConnection_Impl::onTopic(irc_session_t* session,
        const char* event,
        const char* origin,
        const char** params,
        unsigned int count)
{
#warning stub onTopic
	if (count < 1) return;
	string who(origin);
	string channel(params[0]);
	string topic = count < 2 ? "" : (params[1]);
	cout << "TOPIC<" << who << ">: " << channel << ": " << topic << endl;
	appQueue->sendEvent(make_shared<EventIrcTopic>(userId, configuration.serverId, who, channel, topic));
}
void IrcConnection_Impl::onKick(irc_session_t* session,
        const char* event,
        const char* origin,
        const char** params,
        unsigned int count)
{
#warning stub onKick
	if (count < 2) return;
	string who(origin);
	string channel(params[0]);
	string target = count < 2 ? "" : params[1];
	string reason = count < 3 ? "" : params[2];
	appQueue->sendEvent(make_shared<EventIrcKicked>(userId, configuration.serverId, who, channel, target, reason));
}
void IrcConnection_Impl::onChannel(irc_session_t* session,
        const char* event,
        const char* origin,
        const char** params,
        unsigned int count)
{
#warning stub onChannel
	if (count < 2) return;
	string who(origin);
	string channel(params[0]);
	string message(params[1]);
	cout << "<" << channel << ":" << who << ">: " << message << endl;
	appQueue->sendEvent(make_shared<EventIrcMessage>(userId, configuration.serverId, who, channel, message));
}
void IrcConnection_Impl::onPrivmsg(irc_session_t* session,
        const char* event,
        const char* origin,
        const char** params,
        unsigned int count)
{
#warning stub onPrivmsg
	if (count < 2) return;
	string who(origin);
	string self(params[0]);
	string message(params[1]);
	cout << "<" << who << "|" << self << ">: " << message << endl;
	appQueue->sendEvent(make_shared<EventIrcMessage>(userId, configuration.serverId, who, who, message));
}
void IrcConnection_Impl::onNotice(irc_session_t* session,
        const char* event,
        const char* origin,
        const char** params,
        unsigned int count)
{
#warning stub onNotice
	if (count < 1) return;
	string who(origin);
	string target(params[0]);
	string message = count < 2 ? "" : params[1];
	appQueue->sendEvent(make_shared<EventIrcNoticed>(userId, configuration.serverId, who, target, message));
	cout << "N<" << who << "|" << target << ">: " << message << endl;
}
void IrcConnection_Impl::onChannelNotice(irc_session_t* session,
        const char* event,
        const char* origin,
        const char** params,
        unsigned int count)
{
#warning stub onChannelNotice
	if (count < 1) return;
	string who(origin);
	string channel(params[0]);
	string message = count < 2 ? "" : params[1];
	appQueue->sendEvent(make_shared<EventIrcChannelNoticed>(userId, configuration.serverId, who, channel, message));
	cout << "CN<" << who << "|" << channel << ">: " << message << endl;
}
void IrcConnection_Impl::onInvite(irc_session_t* session,
        const char* event,
        const char* origin,
        const char** params,
        unsigned int count)
{
#warning stub onInvite
	if (count < 2) return;
	string who(origin);
	string nick(params[0]);
	string channel(params[1]);
	cout << "Invite<" << who << ">: " << channel << ": " << nick << endl;
}
void IrcConnection_Impl::onCtcpReq(irc_session_t* session,
        const char* event,
        const char* origin,
        const char** params,
        unsigned int count)
{
#warning stub onCtcpReq
}
void IrcConnection_Impl::onCtcpRep(irc_session_t* session,
        const char* event,
        const char* origin,
        const char** params,
        unsigned int count)
{
#warning stub onCtcpRep
}
void IrcConnection_Impl::onCtcpAction(irc_session_t* session,
        const char* event,
        const char* origin,
        const char** params,
        unsigned int count)
{
#warning stub onAction
}
void IrcConnection_Impl::onUnknown(irc_session_t* session,
        const char* event,
        const char* origin,
        const char** params,
        unsigned int count)
{
#warning stub onUnknown
	cout << "UnknownEvent" << endl;
}

