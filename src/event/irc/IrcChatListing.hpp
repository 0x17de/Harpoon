#ifndef IRCCHATLISTING_H
#define IRCCHATLISTING_H

#include "../EventChatListing.hpp"


class IrcChatListing : public ChatListing {
public:
	static UUID uuid;
	virtual UUID getListingUuid() const;
};

#endif
