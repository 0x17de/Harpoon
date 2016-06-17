#ifndef IRCCHANNELSTORE_H
#define IRCCHANNELSTORE_H

#include <map>
#include <string>


class IrcUserStore {
	std::string nick;
	std::string mode;
public:
	IrcUserStore(std::string nick, std::string mode);
	void setNick(std::string nick);
	void setMode(std::string mode);
	std::string getNick() const;
	std::string getMode() const;
};

class IrcChannelStore {
	std::string channelPassword;
	std::map<std::string, IrcUserStore> users;
public:
	IrcChannelStore(const std::string& channelPassword);

	void clear();
	void addUser(const std::string& nick, const std::string& mode);
	void removeUser(const std::string& nick);
	void renameUser(const std::string& nick, const std::string& newNick);
	const std::map<std::string, IrcUserStore>& getUsers() const;
	std::string getChannelPassword() const;
};

#endif
