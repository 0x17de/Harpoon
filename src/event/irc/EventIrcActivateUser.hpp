#ifndef EVENTIRCACTIVATEUSER_H
#define EVENTIRCACTIVATEUSER_H

#include "../IActivateUserEvent.hpp"
#include <string>
#include <map>
#include <list>

class IrcChannelLoginData;


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

class EventIrcActivateUser : public IActivateUserEvent {
	size_t userId;
	std::map<size_t, IrcServerConfiguration> loginData;
public:
	static UUID uuid;
	virtual UUID getEventUuid() const override;

	EventIrcActivateUser(size_t userId, const std::map<size_t, IrcServerConfiguration>& loginData);
	virtual std::shared_ptr<EventLoop> instantiateService(size_t userId, EventQueue* appQueue) const override;

	size_t getUserId() const;
	const std::map<size_t, IrcServerConfiguration>& getLoginConfiguration() const;
};

#endif
