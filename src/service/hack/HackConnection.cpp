#include "HackConnection.hpp"
#include "HackConnection_Impl.hpp"
#include "event/EventQuit.hpp"

using namespace std;


HackConnection::HackConnection(EventQueue* appQueue,
                               size_t userId, const
                               HackServerConfiguration& configuration)
    : EventLoop({
          EventQuit::uuid
      }, {
          EventGuard<IUserEvent>
      })
    , impl{make_shared<HackConnection_Impl>(appQueue, getEventQueue(), userId, configuration)}
{
}

HackConnection::~HackConnection() {
}

bool HackConnection::onEvent(std::shared_ptr<IEvent> event) {
    return impl->onEvent(event);
}

std::string HackConnection::getActiveNick() const {
    return impl->nick;
}

size_t HackConnection::getServerId() const {
    return impl->configuration.getServerId();
}

void HackConnection::addHost(const std::string& hostName,
                             const std::string& websocketUri,
                             int port,
                             bool ipV6,
                             bool ssl) {
    impl->configuration.addHostConfiguration(hostName,
                                             websocketUri,
                                             port,
                                             ipV6,
                                             ssl);
}

void HackConnection::removeHost(const std::string& host, int port) {
    impl->configuration.removeHost(host, port);
}


const HackServerConfiguration& HackConnection::getServerConfiguration() const {
    return impl->configuration;
}

std::string HackConnection::getServerName() const {
    return impl->configuration.getServerName();
}

std::mutex& HackConnection::getChannelLoginDataMutex() const {
    return impl->channelLoginDataMutex;
}

const std::map<std::string, HackChannelStore>& HackConnection::getChannelStore() const {
    return impl->channelStores;
}

const HackChannelStore* HackConnection::getChannelStore(const std::string& channelName) const {
    auto it = impl->channelStores.find(channelName);
    return it == impl->channelStores.end() ? 0 : &it->second;
}
