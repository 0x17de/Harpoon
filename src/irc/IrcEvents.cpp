#include "IrcConnection_Impl.hpp"
#include "event/EventIrcJoinChannel.hpp"
#include "event/EventIrcJoined.hpp"
#include "event/EventIrcParted.hpp"

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
}
void IrcConnection_Impl::onNick(irc_session_t* session,
	const char* event,
	const char* origin,
	const char** params,
	unsigned int count)
{
#warning stub onNick
}
void IrcConnection_Impl::onQuit(irc_session_t* session,
	const char* event,
	const char* origin,
	const char** params,
	unsigned int count)
{
#warning stub onQuit
}
void IrcConnection_Impl::onJoin(irc_session_t* session,
	const char* event,
	const char* origin,
	const char** params,
	unsigned int count)
{
	string username(origin);
	string channel(params[0]);
	make_shared<EventIrcJoined>(serverId, origin, channel);
}
void IrcConnection_Impl::onPart(irc_session_t* session,
	const char* event,
	const char* origin,
	const char** params,
	unsigned int count)
{
#warning stub onPart
	//string username(origin);
	//string channel(params[0]);
	//make_shared...
}
void IrcConnection_Impl::onMode(irc_session_t* session,
        const char* event,
        const char* origin,
        const char** params,
        unsigned int count)
{
#warning stub onMode
}
void IrcConnection_Impl::onUmode(irc_session_t* session,
        const char* event,
        const char* origin,
        const char** params,
        unsigned int count)
{
#warning stub onUmode
}
void IrcConnection_Impl::onTopic(irc_session_t* session,
        const char* event,
        const char* origin,
        const char** params,
        unsigned int count)
{
#warning stub onTopic
}
void IrcConnection_Impl::onKick(irc_session_t* session,
        const char* event,
        const char* origin,
        const char** params,
        unsigned int count)
{
#warning stub onKick
}
void IrcConnection_Impl::onChannel(irc_session_t* session,
        const char* event,
        const char* origin,
        const char** params,
        unsigned int count)
{
#warning stub onChannel
}
void IrcConnection_Impl::onPrivmsg(irc_session_t* session,
        const char* event,
        const char* origin,
        const char** params,
        unsigned int count)
{
#warning stub onPrivmsg
	if (count < 2) return;
	string username(origin);
	string self(params[0]);
	string message(params[1]);
}
void IrcConnection_Impl::onNotice(irc_session_t* session,
        const char* event,
        const char* origin,
        const char** params,
        unsigned int count)
{
#warning stub onNotice
}
void IrcConnection_Impl::onChannelNotice(irc_session_t* session,
        const char* event,
        const char* origin,
        const char** params,
        unsigned int count)
{
#warning stub onChannelNotice
}
void IrcConnection_Impl::onInvite(irc_session_t* session,
        const char* event,
        const char* origin,
        const char** params,
        unsigned int count)
{
#warning stub onInvite
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
}

