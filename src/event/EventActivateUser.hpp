#ifndef EVENTACTIVATEUSER_H
#define EVENTACTIVATEUSER_H

#include "IEvent.hpp"
#include "IUserEvent.hpp"
#include "EventIrcJoinChannel.hpp"
#include <string>
#include <map>
#include <list>


struct IrcServerConfiguration {
	IrcServerConfiguration(size_t serverId, const std::string& serverName,
		const std::string& hostname, int port, const std::string& password,
		const std::list<std::string>& nicks, 
		const std::list<IrcChannelLoginData>& channels,
		bool ipv6, bool ssl);
	size_t serverId;
	std::string serverName;
	std::string hostname;
	int port;
	std::string password;
	std::list<std::string> nicks;
	std::list<IrcChannelLoginData> channels;
	bool ipv6;
	bool ssl;
};

class EventActivateUser : public IEvent {
	size_t userId;
	std::map<size_t, IrcServerConfiguration> loginData;
public:
	static UUID uuid;
	virtual UUID getEventUuid() const override;

	EventActivateUser(size_t userId, const std::map<size_t, IrcServerConfiguration>& loginData);

	size_t getUserId() const;
	const std::map<size_t, IrcServerConfiguration>& getLoginConfiguration() const;
};

#endif
