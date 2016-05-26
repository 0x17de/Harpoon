#include "IrcConnection_Impl.hpp"
#include "queue/EventQueue.hpp"
#include "event/irc/EventIrcConnected.hpp"
#include "event/irc/EventIrcModeChanged.hpp"
#include "event/irc/EventIrcUserModeChanged.hpp"
#include "event/irc/EventIrcNickChanged.hpp"
#include "event/irc/EventIrcJoinChannel.hpp"
#include "event/irc/EventIrcJoined.hpp"
#include "event/irc/EventIrcParted.hpp"
#include "event/irc/EventIrcKicked.hpp"
#include "event/irc/EventIrcQuit.hpp"
#include "event/irc/EventIrcTopic.hpp"
#include "event/irc/EventIrcMessage.hpp"
#include "event/irc/EventIrcNoticed.hpp"
#include "event/irc/EventIrcChannelNoticed.hpp"
#include "event/irc/EventIrcInvited.hpp"
#include "event/irc/EventIrcNumeric.hpp"
#include <iostream>

using namespace std;


void IrcConnection_Impl::onConnect(irc_session_t* session,
	const char* event,
	const char* origin,
	const std::vector<std::string>& params)
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
	const std::vector<std::string>& params)
{
	if (params.size() < 1) return;
	string who(origin);
	string newNick(params.at(0));
	cout << "Nickchange<" << who << ">: " << newNick << endl;
	appQueue->sendEvent(make_shared<EventIrcNickChanged>(userId, configuration.serverId, who, newNick));
}
void IrcConnection_Impl::onQuit(irc_session_t* session,
	const char* event,
	const char* origin,
	const std::vector<std::string>& params)
{
	string who(origin);
	string reason = params.size() < 1 ? "" : params.at(0);
	cout << "Q<" << origin << ">: " << reason << endl;
	appQueue->sendEvent(make_shared<EventIrcQuit>(userId, configuration.serverId, who, reason));
}
void IrcConnection_Impl::onJoin(irc_session_t* session,
	const char* event,
	const char* origin,
	const std::vector<std::string>& params)
{
	if (params.size() < 1) return;
	string who(origin);
	string channel(params.at(0));
	appQueue->sendEvent(make_shared<EventIrcJoined>(userId, configuration.serverId, who, channel));
	cout << "JOIN<" << who << ">: " << channel << endl;
}
void IrcConnection_Impl::onPart(irc_session_t* session,
	const char* event,
	const char* origin,
	const std::vector<std::string>& params)
{
	if (params.size() < 1) return;
	string who(origin);
	string channel(params.at(0));
	string reason = params.size() < 2 ? "" : params.at(1);
	appQueue->sendEvent(make_shared<EventIrcParted>(userId, configuration.serverId, who, channel));
	cout << "PART<" << who << ">: " << channel << ": " << reason << endl;
}
void IrcConnection_Impl::onMode(irc_session_t* session,
        const char* event,
        const char* origin,
        const std::vector<std::string>& params)
{
	if (params.size() < 2) return;
	string who(origin);
	string channel(params.at(0));
	string mode(params.at(1));
	string arg = params.size() < 3 ? "" : params.at(2);
	appQueue->sendEvent(make_shared<EventIrcModeChanged>(userId, configuration.serverId, who, channel, mode, arg));
	cout << "MODE<" << who << ">: " << channel << " " << mode << ": " << arg << endl;
}
void IrcConnection_Impl::onUmode(irc_session_t* session,
        const char* event,
        const char* origin,
        const std::vector<std::string>& params)
{
	if (params.size() < 2) return;
	string who(origin);
	string channel(params.at(0));
	string mode(params.at(1));
	appQueue->sendEvent(make_shared<EventIrcUserModeChanged>(userId, configuration.serverId, who, channel, mode));
	cout << "UMODE<" << who << ">: " << channel << " " << mode << endl;
}
void IrcConnection_Impl::onTopic(irc_session_t* session,
        const char* event,
        const char* origin,
        const std::vector<std::string>& params)
{
	if (params.size() < 1) return;
	string who(origin);
	string channel(params.at(0));
	string topic = params.size() < 2 ? "" : (params.at(1));
	cout << "TOPIC<" << who << ">: " << channel << ": " << topic << endl;
	appQueue->sendEvent(make_shared<EventIrcTopic>(userId, configuration.serverId, who, channel, topic));
}
void IrcConnection_Impl::onKick(irc_session_t* session,
        const char* event,
        const char* origin,
        const std::vector<std::string>& params)
{
	if (params.size() < 2) return;
	string who(origin);
	string channel(params.at(0));
	string target = params.size() < 2 ? "" : params.at(1);
	string reason = params.size() < 3 ? "" : params.at(2);
	appQueue->sendEvent(make_shared<EventIrcKicked>(userId, configuration.serverId, who, channel, target, reason));
}
void IrcConnection_Impl::onChannel(irc_session_t* session,
        const char* event,
        const char* origin,
        const std::vector<std::string>& params)
{
	if (params.size() < 2) return;
	string who(origin);
	string channel(params.at(0));
	string message(params.at(1));
	cout << "<" << channel << ":" << who << ">: " << message << endl;
	appQueue->sendEvent(make_shared<EventIrcMessage>(userId, configuration.serverId, who, channel, message));
}
void IrcConnection_Impl::onPrivmsg(irc_session_t* session,
        const char* event,
        const char* origin,
        const std::vector<std::string>& params)
{
	if (params.size() < 2) return;
	string who(origin);
	string self(params.at(0));
	string message(params.at(1));
	cout << "<" << who << "|" << self << ">: " << message << endl;
	appQueue->sendEvent(make_shared<EventIrcMessage>(userId, configuration.serverId, who, who, message));
}
void IrcConnection_Impl::onNotice(irc_session_t* session,
        const char* event,
        const char* origin,
        const std::vector<std::string>& params)
{
	if (params.size() < 1) return;
	string who(origin);
	string target(params.at(0));
	string message = params.size() < 2 ? "" : params.at(1);
	appQueue->sendEvent(make_shared<EventIrcNoticed>(userId, configuration.serverId, who, target, message));
	cout << "N<" << who << "|" << target << ">: " << message << endl;
}
void IrcConnection_Impl::onChannelNotice(irc_session_t* session,
        const char* event,
        const char* origin,
        const std::vector<std::string>& params)
{
	if (params.size() < 1) return;
	string who(origin);
	string channel(params.at(0));
	string message = params.size() < 2 ? "" : params.at(1);
	appQueue->sendEvent(make_shared<EventIrcChannelNoticed>(userId, configuration.serverId, who, channel, message));
	cout << "CN<" << who << "|" << channel << ">: " << message << endl;
}
void IrcConnection_Impl::onInvite(irc_session_t* session,
        const char* event,
        const char* origin,
        const std::vector<std::string>& params)
{
	if (params.size() < 1) return;
	string who(origin);
	string target(params.at(0));
	string channel = params.size() < 2 ? "" : params.at(1);
	appQueue->sendEvent(make_shared<EventIrcInvited>(userId, configuration.serverId, who, target, channel));
	cout << "Invite<" << who << ">: " << channel << ": " << target << endl;
}
void IrcConnection_Impl::onCtcpReq(irc_session_t* session,
        const char* event,
        const char* origin,
        const std::vector<std::string>& params)
{
#warning stub onCtcpReq
}
void IrcConnection_Impl::onCtcpRep(irc_session_t* session,
        const char* event,
        const char* origin,
        const std::vector<std::string>& params)
{
#warning stub onCtcpRep
}
void IrcConnection_Impl::onCtcpAction(irc_session_t* session,
        const char* event,
        const char* origin,
        const std::vector<std::string>& params)
{
#warning stub onAction
}
void IrcConnection_Impl::onNumeric(irc_session_t* session,
	unsigned int event,
	const char* origin,
	const std::vector<std::string>& parameters)
{
	string who = origin == 0 ? "" : origin;
	appQueue->sendEvent(make_shared<EventIrcNumeric>(userId, configuration.serverId, event, who, parameters));
	cout << "Numeric<" << who << ">: " << event;
	for (string s : parameters)
		cout << " | " << s;
	cout << endl;
}
void IrcConnection_Impl::onUnknown(irc_session_t* session,
        const char* event,
        const char* origin,
        const std::vector<std::string>& params)
{
	cout << "UnknownEvent" << endl;
}

