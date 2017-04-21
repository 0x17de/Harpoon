#ifndef IRCCONNECTION_H
#define IRCCONNECTION_H

#include "event/irc/EventIrcActivateService.hpp"
#include "IrcConnection.hpp"
#include "IrcChannelStore.hpp"
#include "IrcServerConfiguration.hpp"
#include "IrcServerHostConfiguration.hpp"
#include "queue/EventLoop.hpp"
#include <libircclient.h>
#include <list>
#include <vector>
#include <map>
#include <set>
#include <string>
#include <mutex>
#include <memory>
#include <thread>


/// Each client has one irc connection per host
/// Irc connections are created by the IrcService class on activate user
/// or on EventIrcServerAdded messages.
class IrcConnection : public EventLoop {
    EventQueue* appQueue;

    static const std::map<char, char> prefixToMode;

    size_t userId;
    IrcServerConfiguration configuration;
    bool running;
    irc_session_t* ircSession;

    std::thread ircLoop;

    std::mutex channelLoginDataMutex;

    std::string nick;
    std::set<std::string> inUseNicks;
    std::map<std::string, IrcChannelStore> channelStores;

public:
    IrcConnection(EventQueue* appQueue, size_t userId, const IrcServerConfiguration& configuration);
    virtual ~IrcConnection();
    virtual bool onEvent(std::shared_ptr<IEvent> event) override;
    bool findUnusedNick(std::string& nick);

    std::string getActiveNick() const;
    size_t getServerId() const;
    std::string getServerName() const;
    std::mutex& getChannelLoginDataMutex();
    void addHost(const std::string& hostName,
                 int port,
                 const std::string& password,
                 bool ipV6,
                 bool ssl);
    void removeHost(const std::string& host, int port);
    const IrcServerConfiguration& getServerConfiguration() const;
    const std::map<std::string, IrcChannelStore>& getChannelStore() const;
    const IrcChannelStore* getChannelStore(const std::string& channelName) const;

private:
    static std::string getPureNick(const std::string& nick);

    template <void (IrcConnection::*F)(irc_session_t*, const char*, const char*, const std::vector<std::string>&, std::shared_ptr<IEvent>&)>
    static void onIrcEvent(irc_session_t* session,
                           const char* event,
                           const char* origin,
                           const char** inparameters,
                           unsigned int count);
    template <void (IrcConnection::*F)(irc_session_t*, unsigned int, const char*, const std::vector<std::string>&, std::shared_ptr<IEvent>&)>
    static void onIrcNumeric(irc_session_t* session,
                             unsigned int eventCode,
                             const char* origin,
                             const char** inparameters,
                             unsigned int count);

    // event callback aliases
    using ircEventCallback_t = void(irc_session_t* session,
                                    const char* event,
                                    const char* origin,
                                    const std::vector<std::string>& parameters,
                                    std::shared_ptr<IEvent>& resultEvent);
    using ircEventCodeCallback_t = void(irc_session_t* session,
                                        unsigned int event,
                                        const char* origin,
                                        const std::vector<std::string>& parameters,
                                        std::shared_ptr<IEvent>& resultEvent);
    using ircEventDccChat_t = void(irc_session_t* session,
                                   const char* nick,
                                   const char* addr,
                                   irc_dcc_t dccid);
    using ircEventDccSend_t = void(irc_session_t* session,
                                   const char* nick,
                                   const char* addr,
                                   const char* filename,
                                   unsigned long size,
                                   irc_dcc_t dccid);
    using ircDccCallback_t = void(irc_session_t* session,
                                  irc_dcc_t id,
                                  int status,
                                  void* ctx,
                                  const char* data,
                                  unsigned int length);

    // various event callbacks for libircclient

    // connectivity
    ircEventCallback_t onConnect;
    ircEventCallback_t onNick;
    ircEventCallback_t onQuit;
    ircEventCallback_t onJoin;
    ircEventCallback_t onPart;

    // modes
    ircEventCallback_t onMode;
    ircEventCallback_t onUmode;

    // channel
    ircEventCallback_t onTopic;
    ircEventCallback_t onKick;
    ircEventCallback_t onChannel;
    ircEventCallback_t onPrivmsg;

    // server
    ircEventCallback_t onNotice;
    ircEventCallback_t onChannelNotice;
    ircEventCallback_t onInvite;

    // client2client
    ircEventCallback_t onCtcpReq;
    ircEventCallback_t onCtcpRep;
    ircEventCallback_t onCtcpAction;

    // numeric response
    ircEventCodeCallback_t onNumeric;

    // others
    ircEventCallback_t onUnknown;
};

#endif
