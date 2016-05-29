#include "IrcChatListing.hpp"
#include "utils/uuidGen.hpp"


UUID IrcChatListing::uuid = ::uuid.get();
UUID IrcChatListing::getListingUuid() const {
	return this->uuid;
}

