#ifndef IRCCONNECTION_IMPL_H
#define IRCCONNECTION_IMPL_h

#include "event/irc/EventIrcActivateService.hpp"
#include "IrcConnection.hpp"
#include "IrcChannelStore.hpp"
#include "IrcServerConfiguration.hpp"
#include "IrcServerHostConfiguration.hpp"
#include <libircclient.h>
#include <list>
#include <vector>
#include <map>
#include <set>
#include <string>
#include <mutex>
#include <memory>
#include <thread>


class IEvent;
class EventQueue;
class IrcChannelLoginData;
class IrcConnection_Impl {
    static std::string getPureNick(const std::string& nick);
public:
    IrcConnection_Impl(EventQueue* appQueue, EventQueue* queue, size_t userId, const IrcServerConfiguration& configuration);
    ~IrcConnection_Impl();

    EventQueue* appQueue;
    EventQueue* queue;
    size_t userId;
    IrcServerConfiguration configuration;
    bool running;
    irc_session_t* ircSession;

    std::thread ircLoop;

    std::mutex channelLoginDataMutex;

    std::string nick;
    std::set<std::string> inUseNicks;
    std::map<std::string, IrcChannelStore> channelStores;

    bool onEvent(std::shared_ptr<IEvent> event);
    bool findUnusedNick(std::string& nick);

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
