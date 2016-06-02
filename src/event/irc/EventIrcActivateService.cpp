#include "EventIrcActivateService.hpp"
#include "utils/uuidGen.hpp"
#include "EventIrcJoinChannel.hpp"
#include "irc/IrcService.hpp"

using namespace std;


UUID EventIrcActivateService::uuid = ::uuid.get();
UUID EventIrcActivateService::getEventUuid() const {
	return this->uuid;
}

IrcServerConfiguration::IrcServerConfiguration(size_t serverId, const std::string& serverName,
	const std::string& hostname, int port, const std::string& password,
	bool ipv6, bool ssl)
:
	serverId{serverId},
	serverName{serverName},
	hostname{hostname},
	port{port},
	password{password},
	ipv6{ipv6},
	ssl{ssl}
{
}

EventIrcActivateService::EventIrcActivateService(size_t userId)
:
	userId{userId}
{
}

std::shared_ptr<EventLoop> EventIrcActivateService::instantiateService(size_t userId, EventQueue* appQueue) const
{
	return make_shared<IrcService>(userId, appQueue);
}

size_t EventIrcActivateService::getUserId() const {
	return userId;
}

void IrcServerConfiguration::addNick(const std::string& nick) {
	nicks.emplace_back(nick);
}

const std::list<std::string>& IrcServerConfiguration::getNicks() const {
	return nicks;
}

void IrcServerConfiguration::addChannelLoginData(size_t channelId, const std::string& channelName, const std::string& channelPassword) {
	channels.emplace_back(channelId, channelName, channelPassword);
}

const std::list<IrcChannelLoginData>& IrcServerConfiguration::getChannelLoginData() const {
	return channels;
}

IrcServerConfiguration& EventIrcActivateService::addLoginConfiguration(size_t serverId,
		const std::string& serverName,
		const std::string& hostname,
		int port,
		const std::string& password,
		bool ipv6,
		bool ssl) {
	auto it = loginData.emplace(piecewise_construct,
		forward_as_tuple(serverId),
		forward_as_tuple(serverId,
			serverName,
			hostname,
			port,
			password,
			ipv6,
			ssl));
	return it.first->second;
}

const std::map<size_t, IrcServerConfiguration>& EventIrcActivateService::getLoginConfiguration() const {
	return loginData;
}
