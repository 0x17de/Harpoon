#ifndef IRCCONNECTION_IMPL_H
#define IRCCONNECTION_IMPL_h

#include <libircclient.h>
#include <list>

class IrcConnection_Impl {
public:
	irc_session_t* ircSession;

	typedef void (ircEventCallback_t)(irc_session_t* session,
		const char* event,
		const char* origin,
		const char** params,
		unsigned int count);

	ircEventCallback_t onConnect;
	ircEventCallback_t onNick;
	ircEventCallback_t onQuit;
	ircEventCallback_t onJoin;
	ircEventCallback_t onPart;
};
#warning STUB IrcConnection_Impl

#endif
