#include "IrcConnection.hpp"
#include "IrcConnection_Impl.hpp"
#include "event/EventQuit.hpp"

using namespace std;


IrcConnection::IrcConnection(EventQueue* appQueue, size_t userId, const IrcServerConfiguration& configuration)
:
	impl{make_shared<IrcConnection_Impl>(appQueue, getEventQueue(), userId, configuration)},
	EventLoop({
		EventQuit::uuid
	}, {
		EventGuard<IUserEvent>
	})
{
}

IrcConnection::~IrcConnection() {
}

bool IrcConnection::onEvent(std::shared_ptr<IEvent> event) {
	return impl->onEvent(event);
}

size_t IrcConnection::getServerId() const {
	return impl->configuration.serverId;
}

std::string IrcConnection::getServerName() const {
	return impl->configuration.serverName;
}

std::mutex& IrcConnection::getChannelLoginDataMutex() const {
	return impl->channelLoginDataMutex;
}

const std::map<std::string, IrcChannelStore>& IrcConnection::getChannelStore() const {
	return impl->channelStores;
}

const IrcChannelStore* IrcConnection::getChannelStore(const std::string& channelName) const {
	auto it = impl->channelStores.find(channelName);
	return it == impl->channelStores.end() ? 0 : &it->second;
}
