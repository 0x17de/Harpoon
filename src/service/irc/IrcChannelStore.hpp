#ifndef IRCCHANNELSTORE_H
#define IRCCHANNELSTORE_H

#include <map>
#include <string>

/// User specific settings for a channel
class IrcUserStore {
    std::string nick;
    std::string mode;
public:
    IrcUserStore(const std::string& nick, const std::string& mode);
    void setNick(const std::string& nick);
    void setMode(const std::string& mode);
    std::string getNick() const;
    std::string getMode() const;
    void addMode(char mode);
    void removeMode(char mode);
};

/// Channel settings used inside the IrcConnection
class IrcChannelStore {
    std::string channelPassword;
    std::string channelTopic;
    std::map<std::string, IrcUserStore> users;
    bool disabled;

    /// convert a nick to lowercase
    std::string nickToLower(const std::string& nick);
public:
    IrcChannelStore(const std::string& channelPassword,
                    bool disabled);

    void clear();
    void addUser(const std::string& nick, const std::string& mode);
    void removeUser(const std::string& nick);
    void renameUser(const std::string& nick, const std::string& newNick);
    void changeUserMode(const std::string& nick, char mode, bool add);
    const std::map<std::string, IrcUserStore>& getUsers() const;
    std::string getChannelPassword() const;
    bool getDisabled() const;
    void setDisabled(bool ldisabled);
    std::string getTopic() const;
    void setTopic(const std::string& topic);
};

#endif
