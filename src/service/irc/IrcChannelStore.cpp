#include "IrcChannelStore.hpp"
#include <algorithm>

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

IrcChannelStore::IrcChannelStore(const std::string& channelPassword)
    : channelPassword{channelPassword}
{
}

void IrcChannelStore::clear() {
    users.clear();
}

void IrcChannelStore::addUser(const std::string& nick, const std::string& mode) {
    string nickLower = nick;
    transform(nickLower.begin(), nickLower.end(), nickLower.begin(), ::tolower);
    users.emplace(piecewise_construct,
                  forward_as_tuple(nickLower),
                  forward_as_tuple(nick, mode));
}

void IrcChannelStore::removeUser(const std::string& nick) {
    string nickLower = nick;
    transform(nickLower.begin(), nickLower.end(), nickLower.begin(), ::tolower);
    auto it = users.find(nickLower);
    if (it != users.end())
        users.erase(it);
}

void IrcChannelStore::renameUser(const std::string& nick, const std::string& newNick) {
    string nickLower = nick;
    transform(nickLower.begin(), nickLower.end(), nickLower.begin(), ::tolower);

    auto it = users.find(nickLower);
    if (it == users.end()) return;

    string newNickLower = newNick;
    transform(newNickLower.begin(), newNickLower.end(), newNickLower.begin(), ::tolower);

    IrcUserStore& userStore = it->second;
    userStore.setNick(newNick);

    if (nickLower != newNickLower) {
        users.emplace(piecewise_construct,
                      forward_as_tuple(newNickLower),
                      forward_as_tuple(userStore));
        users.erase(it);
    }
}

const std::map<std::string, IrcUserStore>& IrcChannelStore::getUsers() const {
    return users;
}

std::string IrcChannelStore::getChannelPassword() const {
    return channelPassword;
}

