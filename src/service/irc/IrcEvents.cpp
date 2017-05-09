#include "IrcConnection.hpp"
#include "queue/EventQueue.hpp"
#include "event/irc/EventIrcConnected.hpp"
#include "event/irc/EventIrcModeChanged.hpp"
#include "event/irc/EventIrcUserModeChanged.hpp"
#include "event/irc/EventIrcNickChanged.hpp"
#include "event/irc/EventIrcUserStatusChanged.hpp"
#include "event/irc/EventIrcQuit.hpp"
#include "event/irc/EventIrcTopic.hpp"
#include "event/irc/EventIrcAction.hpp"
#include "event/irc/EventIrcMessage.hpp"
#include "event/irc/EventIrcInvited.hpp"
#include "event/irc/EventIrcNumeric.hpp"
#include <iostream>

using namespace std;


void IrcConnection::onConnect(irc_session_t* session,
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

void IrcConnection::onNick(irc_session_t* session,
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

void IrcConnection::onQuit(irc_session_t* session,
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

void IrcConnection::onJoin(irc_session_t* session,
                                const char* event,
                                const char* origin,
                                const std::vector<std::string>& params,
                                std::shared_ptr<IEvent>& resultEvent)
{
    if (params.size() < 1) return;
    string who(origin);
    string channel(params.at(0));
    resultEvent = make_shared<EventIrcUserStatusChanged>(userId,
                                                         configuration.getServerId(),
                                                         who,
                                                         channel,
                                                         EventIrcUserStatusChanged::Status::Joined);
    cout << "JOIN<" << who << ">: " << channel << endl;
}

void IrcConnection::onPart(irc_session_t* session,
                                const char* event,
                                const char* origin,
                                const std::vector<std::string>& params,
                                std::shared_ptr<IEvent>& resultEvent)
{
    if (params.size() < 1) return;
    string who(origin);
    string channel(params.at(0));
    string reason = params.size() < 2 ? "" : params.at(1);
    resultEvent = make_shared<EventIrcUserStatusChanged>(userId,
                                              configuration.getServerId(),
                                              who,
                                              channel,
                                              EventIrcUserStatusChanged::Status::Joined);
    cout << "PART<" << who << ">: " << channel << ": " << reason << endl;
}

void IrcConnection::onMode(irc_session_t* session,
                                const char* event,
                                const char* origin,
                                const std::vector<std::string>& params,
                                std::shared_ptr<IEvent>& resultEvent)
{
    if (params.size() < 2) return;
    string who(origin);
    string channel(params.at(0));
    string mode(params.at(1));
    auto argIt = params.begin()+2;
    resultEvent = make_shared<EventIrcModeChanged>(userId, configuration.getServerId(), who, channel, mode, argIt, params.end());
    cout << "MODE<" << who << ">: " << userId << " " << channel << " " << mode << ":";
    for (auto it = argIt; it != params.end(); ++it)
        cout << " " << *it;
    cout << endl;
}

void IrcConnection::onUmode(irc_session_t* session,
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

void IrcConnection::onTopic(irc_session_t* session,
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

void IrcConnection::onKick(irc_session_t* session,
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
    resultEvent = make_shared<EventIrcUserStatusChanged>(userId,
                                                         configuration.getServerId(),
                                                         who,
                                                         channel,
                                                         EventIrcUserStatusChanged::Status::Kicked,
                                                         target,
                                                         reason);
}

void IrcConnection::onChannel(irc_session_t* session,
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
    resultEvent = make_shared<EventIrcMessage>(userId, configuration.getServerId(), who, channel, message, IrcMessageType::Message);
}

void IrcConnection::onPrivmsg(irc_session_t* session,
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
    resultEvent = make_shared<EventIrcMessage>(userId, configuration.getServerId(), who, who, message, IrcMessageType::Message);
}

void IrcConnection::onNotice(irc_session_t* session,
                                  const char* event,
                                  const char* origin,
                                  const std::vector<std::string>& params,
                                  std::shared_ptr<IEvent>& resultEvent)
{
    if (params.size() < 1) return;
    string who(origin);
    string target(params.at(0));
    string message = params.size() < 2 ? "" : params.at(1);
    resultEvent = make_shared<EventIrcMessage>(userId, configuration.getServerId(), who, target, message, IrcMessageType::Notice);
    cout << "N<" << who << "|" << target << ">: " << message << endl;
}

void IrcConnection::onChannelNotice(irc_session_t* session,
                                         const char* event,
                                         const char* origin,
                                         const std::vector<std::string>& params,
                                         std::shared_ptr<IEvent>& resultEvent)
{
    if (params.size() < 1) return;
    string who(origin);
    string channel(params.at(0));
    string message = params.size() < 2 ? "" : params.at(1);
    resultEvent = make_shared<EventIrcMessage>(userId, configuration.getServerId(), who, channel, message, IrcMessageType::Notice);
    cout << "CN<" << who << "|" << channel << ">: " << message << endl;
}

void IrcConnection::onInvite(irc_session_t* session,
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

void IrcConnection::onCtcpReq(irc_session_t* session,
                                   const char* event,
                                   const char* origin,
                                   const std::vector<std::string>& params,
                                   std::shared_ptr<IEvent>& resultEvent)
{
#pragma message "stub onCtcpReq"
}

void IrcConnection::onCtcpRep(irc_session_t* session,
                                   const char* event,
                                   const char* origin,
                                   const std::vector<std::string>& params,
                                   std::shared_ptr<IEvent>& resultEvent)
{
#pragma message "stub onCtcpRep"
}

void IrcConnection::onCtcpAction(irc_session_t* session,
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

void IrcConnection::onNumeric(irc_session_t* session,
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

void IrcConnection::onUnknown(irc_session_t* session,
                                   const char* event,
                                   const char* origin,
                                   const std::vector<std::string>& params,
                                   std::shared_ptr<IEvent>& resultEvent)
{
    cout << "UnknownEvent" << endl;
}

