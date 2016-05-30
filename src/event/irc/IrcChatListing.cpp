#include "IrcChatListing.hpp"
#include "utils/uuidGen.hpp"


UUID IrcChatListing::uuid = ::uuid.get();
UUID IrcChatListing::getListingUuid() const {
	return this->uuid;
}

IrcChannelListing::IrcChannelListing(const std::string& name)
:
	name{name}
{
}

IrcServerListing::IrcServerListing(size_t id, const std::string& name)
:
	id{id},
	name{name}
{
}
