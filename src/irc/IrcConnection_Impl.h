#ifndef IRCCONNECTION_IMPL_H
#define IRCCONNECTION_IMPL_h

#include <libircclient.h>
#include <list>


class IrcConnection_Impl {
public:
	irc_session_t* ircSession;

	void onConnect(irc_session_t* session,
		const char* event,
		const char* origin,
		const char** params,
		unsigned int count);
};
#warning STUB IrcConnection_Impl

#endif
