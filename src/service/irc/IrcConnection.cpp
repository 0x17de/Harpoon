#include "IrcConnection.hpp"
#include "IrcConnection_Impl.hpp"
#include "event/EventQuit.hpp"

using namespace std;


IrcConnection::IrcConnection(EventQueue* appQueue,
                             size_t userId, const
                             IrcServerConfiguration& configuration)
    : EventLoop({
          EventQuit::uuid
      }, {
          EventGuard<IUserEvent>
      })
    , impl{make_shared<IrcConnection_Impl>(appQueue, getEventQueue(), userId, configuration)}
{
}

IrcConnection::~IrcConnection() {
}

bool IrcConnection::onEvent(std::shared_ptr<IEvent> event) {
    return impl->onEvent(event);
}

std::string IrcConnection::getActiveNick() const {
    return impl->nick;
}

size_t IrcConnection::getServerId() const {
    return impl->configuration.getServerId();
}

void IrcConnection::addHost(const std::string& hostName,
                            int port,
                            const std::string& password,
                            bool ipV6,
                            bool ssl) {
    impl->configuration.addHostConfiguration(hostName,
                                port,
                                password,
                                ipV6,
                                ssl);
}

void IrcConnection::removeHost(const std::string& host, int port) {
    impl->configuration.removeHost(host, port);
}


const IrcServerConfiguration& IrcConnection::getServerConfiguration() const {
    return impl->configuration;
}

std::string IrcConnection::getServerName() const {
    return impl->configuration.getServerName();
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
