#ifndef IRCCONNECTION_IMPL_H
#define IRCCONNECTION_IMPL_h

#include "event/irc/EventIrcActivateService.hpp"
#include <libircclient.h>
#include <list>
#include <vector>
#include <map>
#include <set>
#include <string>
#include <mutex>
#include <memory>
#include <thread>


class IEvent;
class EventQueue;
class IrcChannelLoginData;
class IrcConnection_Impl {
public:
	IrcConnection_Impl(EventQueue* appQueue, EventQueue* queue, size_t userId, const IrcServerConfiguration& configuration);
	~IrcConnection_Impl();

	bool running;
	irc_session_t* ircSession;
	EventQueue* appQueue;
	EventQueue* queue;
	size_t userId;
	IrcServerConfiguration configuration;

	std::thread ircLoop;

	std::mutex channelLoginDataMutex;
	std::map<std::string, IrcChannelLoginData> channelLoginData;
	std::string nick;
	std::set<std::string> inUseNicks;

	bool onEvent(std::shared_ptr<IEvent> event);
	bool findUnusedNick(std::string& nick);

	// event callback typedef
	typedef void (ircEventCallback_t)(irc_session_t* session,
		const char* event,
		const char* origin,
		const std::vector<std::string>& parameters,
		std::shared_ptr<IEvent>& resultEvent);
	typedef void (ircEventCodeCallback_t)(irc_session_t* session,
		unsigned int event,
		const char* origin,
		const std::vector<std::string>& parameters,
		std::shared_ptr<IEvent>& resultEvent);
	typedef void (ircEventDccChat_t)(irc_session_t* session,
		const char* nick,
		const char* addr,
		irc_dcc_t dccid);
	typedef void (ircEventDccSend_t)(irc_session_t* session,
		const char* nick,
		const char* addr,
		const char* filename,
		unsigned long size,
		irc_dcc_t dccid);
	typedef void (ircDccCallback_t)(irc_session_t* session,
		irc_dcc_t id,
		int status,
		void* ctx,
		const char* data,
		unsigned int length);

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
	ircEventCallback_t onChannelNotice;
	ircEventCallback_t onInvite;

	// client2client
	ircEventCallback_t onCtcpReq;
	ircEventCallback_t onCtcpRep;
	ircEventCallback_t onCtcpAction;

	// numeric response
	ircEventCodeCallback_t onNumeric;

	// others
	ircEventCallback_t onUnknown;
};

#endif
