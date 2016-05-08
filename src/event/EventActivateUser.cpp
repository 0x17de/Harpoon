#include "EventActivateUser.hpp"
#include "utils/uuidGen.hpp"


UUID EventActivateUser::uuid = ::uuid.get();
UUID EventActivateUser::getEventUuid() const {
	return this->uuid;
}

IrcServerConfiguration::IrcServerConfiguration(size_t serverId, const std::string& serverName,
	const std::string& serverHostname, const std::string& serverPassword,
	std::list<std::string> nicks, bool ipv6, bool ssl)
:
	serverId{serverId},
	serverName{serverName},
	serverHostname{serverPassword},
	nicks{nicks},
	ipv6{ipv6},
	ssl{ssl}
{
}

EventActivateUser::EventActivateUser(size_t userId, const std::map<size_t, IrcServerConfiguration>& loginData)
:
	userId{userId},
	loginData{loginData}
{
}

size_t EventActivateUser::getUserId() const {
	return userId;
}

const std::map<size_t, IrcServerConfiguration>& EventActivateUser::getLoginConfiguration() const {
	return loginData;
}
