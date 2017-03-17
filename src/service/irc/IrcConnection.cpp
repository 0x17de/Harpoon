#include "IrcConnection.hpp"
#include "IrcChannelLoginData.hpp"
#include "event/EventQuit.hpp"
#include "event/IActivateServiceEvent.hpp"
#include "event/irc/EventIrcJoinChannel.hpp"
#include "event/irc/EventIrcJoined.hpp"
#include "event/irc/EventIrcParted.hpp"
#include "event/irc/EventIrcKicked.hpp"
#include "event/irc/EventIrcQuit.hpp"
#include "event/irc/EventIrcNickChanged.hpp"
#include "event/irc/EventIrcPartChannel.hpp"
#include "event/irc/EventIrcDeleteChannel.hpp"
#include "event/irc/EventIrcSendMessage.hpp"
#include "event/irc/EventIrcSendAction.hpp"
#include "event/irc/EventIrcMessage.hpp"
#include "event/irc/EventIrcAction.hpp"
#include "event/irc/EventIrcTopic.hpp"
#include "event/irc/EventIrcNumeric.hpp"
#include "event/irc/EventIrcModifyNick.hpp"
#include "event/irc/EventIrcNickModified.hpp"
#include "event/irc/EventIrcChangeNick.hpp"
#include "event/irc/EventIrcUserlistReceived.hpp"
#include "event/irc/EventIrcModeChanged.hpp"
#include <algorithm>
#include <iostream>
#include <sstream>
#include <map>
#include <thread>
#include <chrono>
#include <libirc_rfcnumeric.h>

using namespace std;


static map<irc_session_t*, IrcConnection*> activeIrcConnections;

template <void (IrcConnection::*F)(irc_session_t*, const char*, const char*, const vector<string>&, std::shared_ptr<IEvent>&)>
void IrcConnection::onIrcEvent(irc_session_t* session,
                const char* event,
                const char* origin,
                const char** inparameters,
                unsigned int count)
{
    vector<string> parameters;
    for (unsigned int i = 0; i < count; ++i)
        parameters.push_back(string(inparameters[i]));

    IrcConnection* cxn = activeIrcConnections.at(session);
    shared_ptr<IEvent> resultEvent;
    (cxn->*F)(session, event, origin, parameters, resultEvent);
    if (resultEvent) {
        cxn->getEventQueue()->sendEvent(resultEvent);
        cxn->appQueue->sendEvent(resultEvent);
    }
}
template <void (IrcConnection::*F)(irc_session_t*, unsigned int, const char*, const vector<string>&, std::shared_ptr<IEvent>&)>
void IrcConnection::onIrcNumeric(irc_session_t* session,
                  unsigned int eventCode,
                  const char* origin,
                  const char** inparameters,
                  unsigned int count)
{
    vector<string> parameters;
    for (unsigned int i = 0; i < count; ++i)
        parameters.push_back(string(inparameters[i]));

    IrcConnection* cxn = activeIrcConnections.at(session);
    shared_ptr<IEvent> resultEvent;
    (cxn->*F)(session, eventCode, origin, parameters, resultEvent);
    if (resultEvent) {
        cxn->getEventQueue()->sendEvent(resultEvent);
        cxn->appQueue->sendEvent(resultEvent);
    }
}

bool IrcConnection::findUnusedNick(std::string& nick) {
    auto& nicks = this->configuration.getNicks();
    auto nickIt = find_if(nicks.begin(), nicks.end(), [this](const std::string& foundNick) {
        return inUseNicks.find(foundNick) == inUseNicks.end(); // not used
    });
    bool success = nickIt != nicks.end();
    if (success) nick = *nickIt; // assign nick
    return success;
}

IrcConnection::IrcConnection(EventQueue* appQueue,
                             size_t userId,
                             const IrcServerConfiguration& configuration)
    : EventLoop({
          EventQuit::uuid
      }, {
          EventGuard<IUserEvent>
      })
    , appQueue{appQueue}
    , userId{userId}
    , configuration{configuration}
    , running{true}
    , ircSession{0}
{
    ircLoop = thread([this]{
        irc_callbacks_t callbacks = {0};
        callbacks.event_connect = &onIrcEvent<&IrcConnection::onConnect>;
        callbacks.event_nick = &onIrcEvent<&IrcConnection::onNick>;
        callbacks.event_quit = &onIrcEvent<&IrcConnection::onQuit>;
        callbacks.event_join = &onIrcEvent<&IrcConnection::onJoin>;
        callbacks.event_part = &onIrcEvent<&IrcConnection::onPart>;
        callbacks.event_mode = &onIrcEvent<&IrcConnection::onMode>;
        callbacks.event_umode = &onIrcEvent<&IrcConnection::onUmode>;
        callbacks.event_topic = &onIrcEvent<&IrcConnection::onTopic>;
        callbacks.event_kick = &onIrcEvent<&IrcConnection::onKick>;
        callbacks.event_channel = &onIrcEvent<&IrcConnection::onChannel>;
        callbacks.event_privmsg = &onIrcEvent<&IrcConnection::onPrivmsg>;
        callbacks.event_notice = &onIrcEvent<&IrcConnection::onNotice>;
        callbacks.event_channel_notice = &onIrcEvent<&IrcConnection::onNotice>;
        callbacks.event_invite = &onIrcEvent<&IrcConnection::onInvite>;
        //callbacks.event_ctcp_req = &onIrcEvent<&IrcConnection::onCtcpReq>;
        //callbacks.event_ctcp_rep = &onIrcEvent<&IrcConnection::onCtcpRep>;
        callbacks.event_ctcp_action = &onIrcEvent<&IrcConnection::onCtcpAction>;
        callbacks.event_unknown = &onIrcEvent<&IrcConnection::onUnknown>;

        callbacks.event_numeric = &onIrcNumeric<&IrcConnection::onNumeric>;

        // callbacks.irc_event_dcc_chat_t event_dcc_chat_req = &onIrcEvent<&IrcConnection::onReq>
        // callbacks.irc_event_dcc_send_t event_dcc_send_req = &onIrcEvent<&IrcConnection::onReq>

        ircSession = irc_create_session(&callbacks);
        // remember session
        activeIrcConnections.emplace(ircSession, this);

        size_t hostIndex = 0; // on error choose next server
        while (ircSession != 0 && running) {
            IrcServerHostConfiguration hostConfiguration;
            {
                lock_guard<mutex> lock(channelLoginDataMutex);
                // copy channels to login
                channelStores.clear();
                for (auto& channel : this->configuration.getChannelLoginData()) {
                    string channelLower = channel.getChannelName();
                    transform(channelLower.begin(), channelLower.end(), channelLower.begin(), ::tolower);
                    channelStores.emplace(piecewise_construct,
                                          forward_as_tuple(channelLower),
                                          forward_as_tuple(channel.getChannelPassword(), channel.getDisabled()));
                }

                auto& hostConfigurations = this->configuration.getHostConfigurations();
                if (hostConfigurations.empty()) break; // give up
                hostIndex %= hostConfigurations.size();
                hostConfiguration = *(next(hostConfigurations.begin(), hostIndex));

                if (!findUnusedNick(nick)) break; // give up
            }

            stringstream host;
            if (hostConfiguration.getSsl())
                host << "#";
            host << hostConfiguration.getHostName();
            // connect to server
            if ((hostConfiguration.getIpV6()
                 ? irc_connect6
                 : irc_connect)(ircSession,
                                host.str().c_str(),
                                hostConfiguration.getPort(),
                                hostConfiguration.getPassword().empty()
                                ? 0
                                : hostConfiguration.getPassword().c_str(),
                                nick.c_str(),
                                0 /* username */,
                                0 /* realname */)) {
                hostIndex += 1;
            }
            cout << "[IC] Connection initiated: " << host.str() << ":" << hostConfiguration.getPort() << endl;

            // run irc event loop
            if (irc_run(ircSession))
                hostIndex += 1;
            cout << "[IC] Loop finished" << endl;

            irc_disconnect(ircSession);

            if (running) {
                // reconnect
                this_thread::sleep_for(chrono::seconds(1));
            }
        }
#pragma message "IrcConnection: send disconnected event to appqueue"
    });
}

IrcConnection::~IrcConnection() {
    running = false;
    if (ircSession != 0)
        irc_cmd_quit(ircSession, 0);
    if (ircLoop.joinable()) ircLoop.join();
    activeIrcConnections.erase(ircSession);
    if (ircSession != 0)
        irc_destroy_session(ircSession);
}

const std::map<char, char> IrcConnection::prefixToMode {
    {'~', 'q'}, // Owner
    {'&', 'a'}, // Admin
    {'@', 'o'}, // Operator
    {'%', 'h'}, // HalfOperator
    {'+', 'v'} // Voiced
};

std::string IrcConnection::getPureNick(const std::string& nick) {
    size_t exclamationPosition = nick.find('!');
    if (exclamationPosition == string::npos)
        return nick;
    return nick.substr(0, exclamationPosition);
}

bool IrcConnection::onEvent(std::shared_ptr<IEvent> event) {
    UUID type = event->getEventUuid();
    if (type == EventQuit::uuid) {
        running = false;
        cout << "[IC] received QUIT" << endl;
        return false;
    } else if (type == EventIrcChangeNick::uuid) {
        lock_guard<mutex> lock(channelLoginDataMutex);
        auto nick = event->as<EventIrcChangeNick>();
        irc_cmd_nick(ircSession, nick->getNick().c_str());
    } else if (type == EventIrcJoinChannel::uuid) {
        lock_guard<mutex> lock(channelLoginDataMutex);
        auto joinCommand = event->as<EventIrcJoinChannel>();
        for (auto& entry : joinCommand->getLoginData()) {
            string channelName = entry.name;

            size_t lastPound = channelName.rfind('#');
            if (channelName.size() == 0 || lastPound == channelName.size())
                continue; // invalid channel name

            string channelLower = channelName;
            transform(channelLower.begin(), channelLower.end(), channelLower.begin(), ::tolower);

            auto it = channelStores.find(channelLower);
            if (it != channelStores.end()) {
                if (it->second.getDisabled()) {
                    // TODO: check if password specified
                    irc_cmd_join(ircSession,
                                 channelLower.c_str(),
                                 it->second.getChannelPassword().c_str());
                    it->second.setDisabled(false);
                }
                continue;
            }
            string channelPassword = entry.password;
            irc_cmd_join(ircSession,
                         channelLower.c_str(),
                         channelPassword.c_str());
            channelStores.emplace(piecewise_construct,
                                  forward_as_tuple(channelLower),
                                  forward_as_tuple(channelPassword, false));
            appQueue->sendEvent(make_shared<EventIrcJoined>(joinCommand->getUserId(),
                                                            joinCommand->getServerId(),
                                                            "",
                                                            channelLower));
        }
    } else if (type == EventIrcJoined::uuid) {
        auto join = event->as<EventIrcJoined>();
        string userName = join->getUsername();
        if (userName.size() > 0)  {
            string channelName = join->getChannel();

            lock_guard<mutex> lock(channelLoginDataMutex);
            auto it = channelStores.find(channelName);
            if (it != channelStores.end()) {
                IrcChannelStore& channelStore = it->second;
                channelStore.addUser(getPureNick(userName), "");
            }
        }
    } else if (type == EventIrcParted::uuid) {
        auto part = event->as<EventIrcParted>();
        string channelName = part->getChannel();

        lock_guard<mutex> lock(channelLoginDataMutex);
        auto it = channelStores.find(channelName);
        if (it != channelStores.end()) {
            IrcChannelStore& channelStore = it->second;
            channelStore.removeUser(getPureNick(part->getUsername()));
        }
    } else if (type == EventIrcTopic::uuid) {
        auto topic = event->as<EventIrcTopic>();
        string topicText = topic->getTopic();
        string channelName = topic->getChannel();

        lock_guard<mutex> lock(channelLoginDataMutex);
        auto it = channelStores.find(channelName);
        if (it != channelStores.end()) {
            IrcChannelStore& channelStore = it->second;
            channelStore.setTopic(topicText);
        }
    } else if (type == EventIrcKicked::uuid) {
        auto kick = event->as<EventIrcKicked>();
        string channelName = kick->getChannel();

        lock_guard<mutex> lock(channelLoginDataMutex);
        auto it = channelStores.find(channelName);
        if (it != channelStores.end()) {
            IrcChannelStore& channelStore = it->second;
            channelStore.removeUser(getPureNick(kick->getTarget()));
        }
    } else if (type == EventIrcQuit::uuid) {
        auto part = event->as<EventIrcQuit>();
        string nick = getPureNick(part->getWho());

        lock_guard<mutex> lock(channelLoginDataMutex);
        for (auto& channelPair : channelStores) {
            auto& channelStore = channelPair.second;
            channelStore.removeUser(nick);
        }
    } else if (type == EventIrcModifyNick::uuid) {
        lock_guard<mutex> lock(channelLoginDataMutex);
        auto modify = event->as<EventIrcModifyNick>();
        configuration.modifyNick(modify->getOldNick(), modify->getNewNick());
        appQueue->sendEvent(make_shared<EventIrcNickModified>(modify->getUserId(), modify->getServerId(), modify->getOldNick(), modify->getNewNick()));
    } else if (type == EventIrcNickChanged::uuid) {
        auto nickChange = event->as<EventIrcNickChanged>();

        lock_guard<mutex> lock(channelLoginDataMutex);
        for (auto& channelStorePair : channelStores) {
            IrcChannelStore& channelStore = channelStorePair.second;
            channelStore.renameUser(getPureNick(nickChange->getUsername()), nickChange->getNewNick());
        }
        if (nick == getPureNick(nickChange->getUsername()))
            nick = nickChange->getNewNick();
    } else if (type == EventIrcNumeric::uuid) {
        auto num = event->as<EventIrcNumeric>();
        unsigned int code = num->getEventCode();
        if (code == LIBIRC_RFC_ERR_NICKNAMEINUSE) {
            lock_guard<mutex> lock(channelLoginDataMutex);
            cout << "Nick in use: " << nick << endl;
            inUseNicks.emplace(nick);
            if (findUnusedNick(nick))
                irc_cmd_nick(ircSession, nick.c_str());
        } else if (code == LIBIRC_RFC_RPL_TOPIC) {
            lock_guard<mutex> lock(channelLoginDataMutex);
            auto num = event->as<EventIrcNumeric>();
            auto& parameters = num->getParameters();

            auto it = channelStores.find(parameters.at(1));
            if (it != channelStores.end())
                it->second.setTopic(parameters.at(2));
        } else if (code == LIBIRC_RFC_RPL_NAMREPLY) {
            lock_guard<mutex> lock(channelLoginDataMutex);
            auto num = event->as<EventIrcNumeric>();
            auto& parameters = num->getParameters();

            string channelName = parameters.at(2);

            if (channelName.size() > 0) {
                char channelMode = parameters.at(0).at(1);
                if (channelMode == '='
                    || channelMode == '*'
                    || channelMode == '@') {
#pragma message "channel mode stub"
                }

                auto it = channelStores.find(channelName);
                IrcChannelStore* channelStore;
                // find userdata
                if (it == channelStores.end()) {
                    auto insertResult = channelStores.emplace(piecewise_construct,
                                                              forward_as_tuple(channelName),
                                                              forward_as_tuple("", false));
                    channelStore = &insertResult.first->second;
                } else {
                    channelStore = &it->second;
                }
                // assign users from parameter
                channelStore->clear();
                auto userlist = make_shared<EventIrcUserlistReceived>(num->getUserId(), num->getServerId(), channelName);
                istringstream users(parameters.at(3));
                string username;
                while (getline(users, username, ' ')) {
                    if (username.size() == 0) continue;

                    std::string mode; // optional, first letter of username
                    auto modeIt = prefixToMode.find(username.at(0));
                    if (modeIt != prefixToMode.end()) {
                        mode = modeIt->second;
                        username = username.substr(1);
                    }
                    channelStore->addUser(username, mode);
                    userlist->addUser(username, mode);
                }
                appQueue->sendEvent(userlist);
            }
        } else if (code == LIBIRC_RFC_RPL_ENDOFNAMES) {
            // userlist is sent in parts
        }
    } else if (type == EventIrcModeChanged::uuid) {
        auto mode = event->as<EventIrcModeChanged>();

        auto channelIt = channelStores.find(mode->getChannel());
        if (channelIt != channelStores.end()) {
            auto& channel = channelIt->second;
            auto modeString = mode->getMode();
            auto& args = mode->getArgs();
            bool add = true;
            size_t argIndex = 0;

            for (char modeChar : modeString) {
                if (modeChar == '+') {
                    add = true;
                } else if (modeChar == '-') {
                    add = false;
                } else {
                    auto& targetNick = args.at(argIndex);
                    channel.changeUserMode(targetNick, modeChar, add);
                    argIndex += 1;
                }
            }
        }
    } else if (type == EventIrcPartChannel::uuid) {
        lock_guard<mutex> lock(channelLoginDataMutex);
        auto partCommand = event->as<EventIrcPartChannel>();
        for (string channelName : partCommand->getChannels()) {
            string channelLower = channelName;
            transform(channelLower.begin(), channelLower.end(), channelLower.begin(), ::tolower);
            auto it = channelStores.find(channelLower);
            if (it == channelStores.end())
                continue;
            irc_cmd_part(ircSession, channelLower.c_str());
            it->second.setDisabled(true);
        }
    } else if (type == EventIrcDeleteChannel::uuid) {
        lock_guard<mutex> lock(channelLoginDataMutex);
        auto deleteCommang = event->as<EventIrcDeleteChannel>();
        string channelName = deleteCommang->getChannelName();
        string channelLower = channelName;
        transform(channelLower.begin(), channelLower.end(), channelLower.begin(), ::tolower);
        auto it = channelStores.find(channelLower);
        if (it != channelStores.end()) { // channel was found
            irc_cmd_part(ircSession, channelLower.c_str());
            channelStores.erase(it);
        }
    } else if (type == EventIrcSendMessage::uuid) {
        auto message = event->as<EventIrcSendMessage>();
        if (!irc_cmd_msg(ircSession, message->getChannel().c_str(), message->getMessage().c_str())) {
            appQueue->sendEvent(make_shared<EventIrcMessage>(message->getUserId(), message->getServerId(), nick, message->getChannel(), message->getMessage(), message->getType()));
        }
    } else if (type == EventIrcSendAction::uuid) {
        auto aaction = event->as<EventIrcSendAction>();
        if (!irc_cmd_me(ircSession, aaction->getChannel().c_str(), aaction->getMessage().c_str())) {
            appQueue->sendEvent(make_shared<EventIrcAction>(aaction->getUserId(), aaction->getServerId(), nick, aaction->getChannel(), aaction->getMessage()));
        }
    }
    return true;
}

std::string IrcConnection::getActiveNick() const {
    return nick;
}

size_t IrcConnection::getServerId() const {
    return configuration.getServerId();
}

void IrcConnection::addHost(const std::string& hostName,
                            int port,
                            const std::string& password,
                            bool ipV6,
                            bool ssl) {
    configuration.addHostConfiguration(hostName,
                                       port,
                                       password,
                                       ipV6,
                                       ssl);
}

void IrcConnection::removeHost(const std::string& host, int port) {
    configuration.removeHost(host, port);
}


const IrcServerConfiguration& IrcConnection::getServerConfiguration() const {
    return configuration;
}

std::string IrcConnection::getServerName() const {
    return configuration.getServerName();
}

std::mutex& IrcConnection::getChannelLoginDataMutex() {
    return channelLoginDataMutex;
}

const std::map<std::string, IrcChannelStore>& IrcConnection::getChannelStore() const {
    return channelStores;
}

const IrcChannelStore* IrcConnection::getChannelStore(const std::string& channelName) const {
    auto it = channelStores.find(channelName);
    return it == channelStores.end() ? 0 : &it->second;
}
