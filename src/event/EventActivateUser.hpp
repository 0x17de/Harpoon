#ifndef EVENTACTIVATEUSER_H
#define EVENTACTIVATEUSER_H

#include "IEvent.hpp"
#include "IUserEvent.hpp"
#include <string>
#include <map>
#include <list>


struct IrcServerConfiguration {
	size_t serverId;
	std::string serverName;
	std::string serverHostname;
	std::string serverPassword;
	std::list<std::string> nicks;
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
