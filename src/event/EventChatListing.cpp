#include "EventChatListing.hpp"
#include "utils/uuidGen.hpp"


UUID EventChatListing::uuid = ::uuid.get();
UUID EventChatListing::getEventUuid() const {
	return this->uuid;
}

EventChatListing::EventChatListing(size_t userId, const std::list<std::shared_ptr<ChatListing>>& chatListings)
:
	userId{userId},
	chatListings{chatListings}
{
}

size_t EventChatListing::getUserId() const {
	return userId;
}


const std::list<std::shared_ptr<ChatListing>>& EventChatListing::getChatListings() const {
	return chatListings;
}

