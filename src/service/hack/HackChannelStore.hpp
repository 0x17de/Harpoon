#ifndef HACKCHANNELSTORE_H
#define HACKCHANNELSTORE_H

#include <map>
#include <string>


class HackUserStore {
    std::string nick;
    std::string trip;
public:
    HackUserStore(const std::string& nick, const std::string& trip);
    void setNick(const std::string& nick);
    void setTrip(const std::string& trip);
    std::string getNick() const;
    std::string getTrip() const;
};

class HackChannelStore {
    std::string channelTopic;
    std::map<std::string, HackUserStore> users;
    bool disabled;

    std::string nickToLower(const std::string& nick);
public:
    HackChannelStore(bool disabled);

    void clear();
    void addUser(const std::string& nick, const std::string& trip);
    void removeUser(const std::string& nick);
    void renameUser(const std::string& nick, const std::string& newNick);
    void changeUserTrip(const std::string& nick, const std::string& trip);
    const std::map<std::string, HackUserStore>& getUsers() const;
    bool getDisabled() const;
    void setDisabled(bool ldisabled);
};

#endif
