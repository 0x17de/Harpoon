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
	std::map<std::string, IrcUserStore> users;
public:
	void clear();
	void addUser(std::string nick, std::string mode);
	std::map<std::string, IrcUserStore>& getUsers();
};

#endif
