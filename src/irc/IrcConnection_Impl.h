#ifndef IRCCONNECTION_IMPL_H
#define IRCCONNECTION_IMPL_h

#include <libircclient.h>
#include <list>

class IrcConnection_Impl {
public:
	irc_session_t* ircSession;

	// event callback typedef
	typedef void (ircEventCallback_t)(irc_session_t* session,
		const char* event,
		const char* origin,
		const char** params,
		unsigned int count);

	// various event callbacks for libircclient

	// connectivity
	ircEventCallback_t onConnect;
	ircEventCallback_t onNick;
	ircEventCallback_t onQuit;
	ircEventCallback_t onJoin;
	ircEventCallback_t onPart;

	// modes
	ircEventCallback_t onMode;
	ircEventCallback_t onUmode;

	// channel
	ircEventCallback_t onTopic;
	ircEventCallback_t onKick;
	ircEventCallback_t onChannel;
	ircEventCallback_t onPrivmsg;

	// server
	ircEventCallback_t onNotice;
	ircEventCallback_t onInvite;

	// client2client
	ircEventCallback_t onReq;
	ircEventCallback_t onRep;

	ircEventCallback_t onAction; // me
	ircEventCallback_t onUnknown; // others
};
#warning STUB IrcConnection_Impl

#endif
