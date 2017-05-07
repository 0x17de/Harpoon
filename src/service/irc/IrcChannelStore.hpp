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

    /// remove all users from the channel
    void clear();
    /// add a single user to the channel
    void addUser(const std::string& nick, const std::string& mode);
    /// remove a single user from the channel
    void removeUser(const std::string& nick);
    /// rename a user
    ///
    /// \param nick Old nickname
    /// \param newNick New nickname
    void renameUser(const std::string& nick, const std::string& newNick);
    /// Change the irc user mode of a user
    ///
    /// \param nick Current nickname
    /// \param mode New mode char
    /// \param add True to add or false to remove a mode
    void changeUserMode(const std::string& nick, char mode, bool add);
    /// Returns the map of users to stored data
    const std::map<std::string, IrcUserStore>& getUsers() const;
    /// Returns the channel's password that is currently used
    std::string getChannelPassword() const;
    /// True if the channel is disabled
    bool getDisabled() const;
    /// Disable or enable a channel
    void setDisabled(bool ldisabled);
    /// Returns the topic string
    std::string getTopic() const;
    /// Sets the topic string
    void setTopic(const std::string& topic);
};

#endif
