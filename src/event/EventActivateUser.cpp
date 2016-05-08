#include "EventActivateUser.hpp"
#include "utils/uuidGen.hpp"


UUID EventActivateUser::uuid = ::uuid.get();
UUID EventActivateUser::getEventUuid() const {
	return this->uuid;
}

IrcServerConfiguration::IrcServerConfiguration(size_t serverId, const std::string& serverName,
	const std::string& hostname, int port, const std::string& password,
	const std::list<std::string>& nicks,
	const std::list<IrcChannelLoginData>& channels,
	bool ipv6, bool ssl)
:
	serverId{serverId},
	serverName{serverName},
	hostname{hostname},
	port{port},
	password{password},
	nicks{nicks},
	channels{channels},
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
