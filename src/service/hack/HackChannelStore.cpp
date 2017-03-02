#include "HackChannelStore.hpp"
#include <algorithm>

using namespace std;


HackUserStore::HackUserStore(const std::string& nick, const std::string& trip)
    : nick{nick}
    , trip{trip}
{
}

void HackUserStore::setNick(const std::string& lnick) {
    nick = lnick;
}

std::string HackUserStore::getNick() const {
    return nick;
}

void HackUserStore::setTrip(const std::string& ltrip) {
    trip = ltrip;
}

std::string HackUserStore::getTrip() const {
    return trip;
}

std::string HackChannelStore::nickToLower(const std::string& nick) {
    string nickLower = nick;
    transform(nickLower.begin(), nickLower.end(), nickLower.begin(), ::tolower);
    return nickLower;
}

HackChannelStore::HackChannelStore(bool disabled)
    : disabled{disabled}
{
}

void HackChannelStore::clear() {
    users.clear();
}

void HackChannelStore::addUser(const std::string& nick, const std::string& trip) {
    string nickLower = nickToLower(nick);
    users.emplace(piecewise_construct,
                  forward_as_tuple(nickLower),
                  forward_as_tuple(nick, trip));
}

void HackChannelStore::removeUser(const std::string& nick) {
    string nickLower = nickToLower(nick);
    auto it = users.find(nickLower);
    if (it != users.end())
        users.erase(it);
}

void HackChannelStore::renameUser(const std::string& nick, const std::string& newNick) {
    string nickLower = nickToLower(nick);

    auto it = users.find(nickLower);
    if (it == users.end()) return;

    string newNickLower = nickToLower(newNick);

    HackUserStore& userStore = it->second;
    userStore.setNick(newNick);

    if (nickLower != newNickLower) {
        users.emplace(piecewise_construct,
                      forward_as_tuple(newNickLower),
                      forward_as_tuple(userStore));
        users.erase(it);
    }
}

void HackChannelStore::changeUserTrip(const std::string& nick, const std::string& trip) {
    string nickLower = nickToLower(nick);
    auto userIt = users.find(nickLower);
    if (userIt != users.end()) {
        auto& user = userIt->second;
        user.setTrip(trip);
    }
}

const std::map<std::string, HackUserStore>& HackChannelStore::getUsers() const {
    return users;
}

bool HackChannelStore::getDisabled() const {
    return disabled;
}

void HackChannelStore::setDisabled(bool ldisabled) {
    disabled = ldisabled;
}
