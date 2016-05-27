#ifndef EVENTCHATLISTING_H
#define EVENTCHATLISTING_H

#include "IEvent.hpp"
#include <list>
#include <memory>


class ChatListing {
public:
	virtual UUID getListingUuid() = 0;
	template <class T>
	T* as() { return dynamic_cast<T*>(this); }
};

class EventChatListing : public IEvent {
	size_t userId;
	std::list<std::shared_ptr<ChatListing>> chatListings;
public:
	static UUID uuid;
	virtual UUID getEventUuid() const override;

	EventChatListing(size_t userId, const std::list<std::shared_ptr<ChatListing>>& chatListings);

	size_t getUserId() const;
	const std::list<std::shared_ptr<ChatListing>>& getChatListings() const;
};

#endif
