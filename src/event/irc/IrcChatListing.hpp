#ifndef IRCCHATLISTING_H
#define IRCCHATLISTING_H

#include "../EventChatListing.hpp"


struct IrcChannelListing {
	IrcChannelListing(const std::string& name);

	std::string name;
};

struct IrcServerListing {
	IrcServerListing(size_t id, const std::string& name);

	size_t id;
	std::string name;
	std::list<IrcChannelListing> channels;
};

class IrcChatListing : public ChatListing {
public:
	static UUID uuid;
	virtual UUID getListingUuid() const;
};

#endif
