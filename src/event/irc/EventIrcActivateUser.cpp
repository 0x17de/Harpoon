#include "EventIrcActivateUser.hpp"
#include "utils/uuidGen.hpp"
#include "EventIrcJoinChannel.hpp"
#include "irc/IrcService.hpp"

using namespace std;


UUID EventIrcActivateUser::uuid = ::uuid.get();
UUID EventIrcActivateUser::getEventUuid() const {
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

EventIrcActivateUser::EventIrcActivateUser(size_t userId, const std::map<size_t, IrcServerConfiguration>& loginData)
:
	userId{userId},
	loginData{loginData}
{
}

std::shared_ptr<EventLoop> EventIrcActivateUser::instantiateService(size_t userId, EventQueue* appQueue) const
{
	return make_shared<IrcService>(userId, appQueue);
}

size_t EventIrcActivateUser::getUserId() const {
	return userId;
}

const std::map<size_t, IrcServerConfiguration>& EventIrcActivateUser::getLoginConfiguration() const {
	return loginData;
}
