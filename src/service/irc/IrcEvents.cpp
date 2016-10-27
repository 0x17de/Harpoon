#include "IrcConnection_Impl.hpp"
#include "queue/EventQueue.hpp"
#include "event/irc/EventIrcConnected.hpp"
#include "event/irc/EventIrcModeChanged.hpp"
#include "event/irc/EventIrcUserModeChanged.hpp"
#include "event/irc/EventIrcNickChanged.hpp"
#include "event/irc/EventIrcJoined.hpp"
#include "event/irc/EventIrcParted.hpp"
#include "event/irc/EventIrcKicked.hpp"
#include "event/irc/EventIrcQuit.hpp"
#include "event/irc/EventIrcTopic.hpp"
#include "event/irc/EventIrcAction.hpp"
#include "event/irc/EventIrcMessage.hpp"
#include "event/irc/EventIrcInvited.hpp"
#include "event/irc/EventIrcNumeric.hpp"
#include <iostream>

using namespace std;


void IrcConnection_Impl::onConnect(irc_session_t* session,
                                   const char* event,
                                   const char* origin,
                                   const std::vector<std::string>& params,
                                   std::shared_ptr<IEvent>& resultEvent)
{
    lock_guard<mutex> lock(channelLoginDataMutex);
    for (auto& joinDataPair : channelStores) {
        string channelName = joinDataPair.first;
        string channelPassword = joinDataPair.second.getChannelPassword();
        if (!joinDataPair.second.getDisabled()) {
            irc_cmd_join(ircSession,
                         channelName.c_str(),
                         channelPassword.empty()
                         ? 0
                         : channelPassword.c_str());
        }
    }
    resultEvent = make_shared<EventIrcConnected>(userId, configuration.getServerId());
}

void IrcConnection_Impl::onNick(irc_session_t* session,
                                const char* event,
                                const char* origin,
                                const std::vector<std::string>& params,
                                std::shared_ptr<IEvent>& resultEvent)
{
    if (params.size() < 1) return;
    string who(origin);
    string newNick(params.at(0));
    cout << "Nickchange<" << who << ">: " << newNick << endl;
    resultEvent = make_shared<EventIrcNickChanged>(userId, configuration.getServerId(), who, newNick);
}

void IrcConnection_Impl::onQuit(irc_session_t* session,
                                const char* event,
                                const char* origin,
                                const std::vector<std::string>& params,
                                std::shared_ptr<IEvent>& resultEvent)
{
    string who(origin);
    string reason = params.size() < 1 ? "" : params.at(0);
    cout << "Q<" << origin << ">: " << reason << endl;
    resultEvent = make_shared<EventIrcQuit>(userId, configuration.getServerId(), who, reason);
}

void IrcConnection_Impl::onJoin(irc_session_t* session,
                                const char* event,
                                const char* origin,
                                const std::vector<std::string>& params,
                                std::shared_ptr<IEvent>& resultEvent)
{
    if (params.size() < 1) return;
    string who(origin);
    string channel(params.at(0));
    resultEvent = make_shared<EventIrcJoined>(userId, configuration.getServerId(), who, channel);
    cout << "JOIN<" << who << ">: " << channel << endl;
}

void IrcConnection_Impl::onPart(irc_session_t* session,
                                const char* event,
                                const char* origin,
                                const std::vector<std::string>& params,
                                std::shared_ptr<IEvent>& resultEvent)
{
    if (params.size() < 1) return;
    string who(origin);
    string channel(params.at(0));
    string reason = params.size() < 2 ? "" : params.at(1);
    resultEvent = make_shared<EventIrcParted>(userId, configuration.getServerId(), who, channel);
    cout << "PART<" << who << ">: " << channel << ": " << reason << endl;
}

void IrcConnection_Impl::onMode(irc_session_t* session,
                                const char* event,
                                const char* origin,
                                const std::vector<std::string>& params,
                                std::shared_ptr<IEvent>& resultEvent)
{
    if (params.size() < 2) return;
    string who(origin);
    string channel(params.at(0));
    string mode(params.at(1));
    string arg = params.size() < 3 ? "" : params.at(2);
    resultEvent = make_shared<EventIrcModeChanged>(userId, configuration.getServerId(), who, channel, mode, arg);
    cout << "MODE<" << who << ">: " << channel << " " << mode << ": " << arg << endl;
}

void IrcConnection_Impl::onUmode(irc_session_t* session,
                                 const char* event,
                                 const char* origin,
                                 const std::vector<std::string>& params,
                                 std::shared_ptr<IEvent>& resultEvent)
{
    if (params.size() < 2) return;
    string who(origin);
    string channel(params.at(0));
    string mode(params.at(1));
    resultEvent = make_shared<EventIrcUserModeChanged>(userId, configuration.getServerId(), who, channel, mode);
    cout << "UMODE<" << who << ">: " << channel << " " << mode << endl;
}

void IrcConnection_Impl::onTopic(irc_session_t* session,
                                 const char* event,
                                 const char* origin,
                                 const std::vector<std::string>& params,
                                 std::shared_ptr<IEvent>& resultEvent)
{
    if (params.size() < 1) return;
    string who(origin);
    string channel(params.at(0));
    string topic = params.size() < 2 ? "" : (params.at(1));
    cout << "TOPIC<" << who << ">: " << channel << ": " << topic << endl;
    resultEvent = make_shared<EventIrcTopic>(userId, configuration.getServerId(), who, channel, topic);
}

void IrcConnection_Impl::onKick(irc_session_t* session,
                                const char* event,
                                const char* origin,
                                const std::vector<std::string>& params,
                                std::shared_ptr<IEvent>& resultEvent)
{
    if (params.size() < 2) return;
    string who(origin);
    string channel(params.at(0));
    string target = params.size() < 2 ? "" : params.at(1);
    string reason = params.size() < 3 ? "" : params.at(2);
    resultEvent = make_shared<EventIrcKicked>(userId, configuration.getServerId(), who, channel, target, reason);
}

void IrcConnection_Impl::onChannel(irc_session_t* session,
                                   const char* event,
                                   const char* origin,
                                   const std::vector<std::string>& params,
                                   std::shared_ptr<IEvent>& resultEvent)
{
    if (params.size() < 2) return;
    string who(origin);
    string channel(params.at(0));
    string message(params.at(1));
    cout << "<" << channel << ":" << who << ">: " << message << endl;
    resultEvent = make_shared<EventIrcMessage>(userId, configuration.getServerId(), who, channel, message, MessageType::Message);
}

void IrcConnection_Impl::onPrivmsg(irc_session_t* session,
                                   const char* event,
                                   const char* origin,
                                   const std::vector<std::string>& params,
                                   std::shared_ptr<IEvent>& resultEvent)
{
    if (params.size() < 2) return;
    string who(origin);
    string self(params.at(0));
    string message(params.at(1));
    cout << "<" << who << "|" << self << ">: " << message << endl;
    resultEvent = make_shared<EventIrcMessage>(userId, configuration.getServerId(), who, who, message, MessageType::Message);
}

void IrcConnection_Impl::onNotice(irc_session_t* session,
                                  const char* event,
                                  const char* origin,
                                  const std::vector<std::string>& params,
                                  std::shared_ptr<IEvent>& resultEvent)
{
    if (params.size() < 1) return;
    string who(origin);
    string target(params.at(0));
    string message = params.size() < 2 ? "" : params.at(1);
    resultEvent = make_shared<EventIrcMessage>(userId, configuration.getServerId(), who, target, message, MessageType::Notice);
    cout << "N<" << who << "|" << target << ">: " << message << endl;
}

void IrcConnection_Impl::onChannelNotice(irc_session_t* session,
                                         const char* event,
                                         const char* origin,
                                         const std::vector<std::string>& params,
                                         std::shared_ptr<IEvent>& resultEvent)
{
    if (params.size() < 1) return;
    string who(origin);
    string channel(params.at(0));
    string message = params.size() < 2 ? "" : params.at(1);
    resultEvent = make_shared<EventIrcMessage>(userId, configuration.getServerId(), who, channel, message, MessageType::Notice);
    cout << "CN<" << who << "|" << channel << ">: " << message << endl;
}

void IrcConnection_Impl::onInvite(irc_session_t* session,
                                  const char* event,
                                  const char* origin,
                                  const std::vector<std::string>& params,
                                  std::shared_ptr<IEvent>& resultEvent)
{
    if (params.size() < 1) return;
    string who(origin);
    string target(params.at(0));
    string channel = params.size() < 2 ? "" : params.at(1);
    resultEvent = make_shared<EventIrcInvited>(userId, configuration.getServerId(), who, target, channel);
    cout << "Invite<" << who << ">: " << channel << ": " << target << endl;
}

void IrcConnection_Impl::onCtcpReq(irc_session_t* session,
                                   const char* event,
                                   const char* origin,
                                   const std::vector<std::string>& params,
                                   std::shared_ptr<IEvent>& resultEvent)
{
#pragma message "stub onCtcpReq"
}

void IrcConnection_Impl::onCtcpRep(irc_session_t* session,
                                   const char* event,
                                   const char* origin,
                                   const std::vector<std::string>& params,
                                   std::shared_ptr<IEvent>& resultEvent)
{
#pragma message "stub onCtcpRep"
}

void IrcConnection_Impl::onCtcpAction(irc_session_t* session,
                                      const char* event,
                                      const char* origin,
                                      const std::vector<std::string>& params,
                                      std::shared_ptr<IEvent>& resultEvent)
{
    if (params.size() < 1) return;
    string who(origin);
    string target(params.at(0));
    string message = params.size() < 2 ? "" : params.at(1);
    resultEvent = make_shared<EventIrcAction>(userId, configuration.getServerId(), who, target, message);
    cout << "Action<" << who << ">: " << target << ": " << message << endl;
}

void IrcConnection_Impl::onNumeric(irc_session_t* session,
                                   unsigned int event,
                                   const char* origin,
                                   const std::vector<std::string>& parameters,
                                   std::shared_ptr<IEvent>& resultEvent)
{
    string who = origin == 0 ? "" : origin;
    resultEvent = make_shared<EventIrcNumeric>(userId, configuration.getServerId(), event, who, parameters);
    cout << "Numeric<" << who << ">: " << event;
    for (string s : parameters)
        cout << " | " << s;
    cout << endl;
}

void IrcConnection_Impl::onUnknown(irc_session_t* session,
                                   const char* event,
                                   const char* origin,
                                   const std::vector<std::string>& params,
                                   std::shared_ptr<IEvent>& resultEvent)
{
    cout << "UnknownEvent" << endl;
}

