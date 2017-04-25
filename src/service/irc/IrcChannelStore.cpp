#include "IrcChannelStore.hpp"
#include <algorithm>
#include <iterator>

using namespace std;


IrcUserStore::IrcUserStore(const std::string& nick, const std::string& mode)
    : nick{nick}
    , mode{mode}
{
}

void IrcUserStore::setNick(const std::string& lnick) {
    nick = lnick;
}
void IrcUserStore::setMode(const std::string& lmode) {
    mode = lmode;
}

std::string IrcUserStore::getNick() const {
    return nick;
}

std::string IrcUserStore::getMode() const {
    return mode;
}

void IrcUserStore::addMode(char modeChar) {
    if (mode.find(modeChar) == string::npos)
        mode.push_back(modeChar);
}

void IrcUserStore::removeMode(char modeChar) {
    mode.erase(std::remove(mode.begin(), mode.end(), modeChar), mode.end());
}

std::string IrcChannelStore::nickToLower(const std::string& nick) {
    string nickLower;
    transform(nickLower.begin(), nickLower.end(), std::back_inserter(nickLower), ::tolower);
    return nickLower;
}

IrcChannelStore::IrcChannelStore(const std::string& channelPassword,
                                 bool disabled)
    : channelPassword{channelPassword}
    , disabled{disabled}
{
}

void IrcChannelStore::clear() {
    users.clear();
}

void IrcChannelStore::addUser(const std::string& nick, const std::string& mode) {
    string nickLower = nickToLower(nick);
    users.emplace(piecewise_construct,
                  forward_as_tuple(nickLower),
                  forward_as_tuple(nick, mode));
}

void IrcChannelStore::removeUser(const std::string& nick) {
    string nickLower = nickToLower(nick);
    auto it = users.find(nickLower);
    if (it != users.end())
        users.erase(it);
}

void IrcChannelStore::renameUser(const std::string& nick, const std::string& newNick) {
    string nickLower = nickToLower(nick);

    auto it = users.find(nickLower);
    if (it == users.end()) return;

    string newNickLower = nickToLower(newNick);

    IrcUserStore& userStore = it->second;
    userStore.setNick(newNick);

    if (nickLower != newNickLower) {
        users.emplace(piecewise_construct,
                      forward_as_tuple(newNickLower),
                      forward_as_tuple(userStore));
        users.erase(it);
    }
}

void IrcChannelStore::changeUserMode(const std::string& nick, char mode, bool add) {
    string nickLower = nickToLower(nick);
    auto userIt = users.find(nickLower);
    if (userIt != users.end()) {
        auto& user = userIt->second;

        if (add) {
            user.addMode(mode);
        } else {
            user.removeMode(mode);
        }
    }
}

const std::map<std::string, IrcUserStore>& IrcChannelStore::getUsers() const {
    return users;
}

std::string IrcChannelStore::getChannelPassword() const {
    return channelPassword;
}

bool IrcChannelStore::getDisabled() const {
    return disabled;
}

void IrcChannelStore::setDisabled(bool ldisabled) {
    disabled = ldisabled;
}

std::string IrcChannelStore::getTopic() const {
    return channelTopic;
}

void IrcChannelStore::setTopic(const std::string& topic) {
    channelTopic = topic;
}
