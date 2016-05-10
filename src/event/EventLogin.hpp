#ifndef EVENTLOGIN_H
#define EVENTLOGIN_H

#include <memory>
#include "IEvent.hpp"


class EventQueue;
class EventLogin : public IEvent {
public:
	static UUID uuid;
	virtual UUID getEventUuid() const override;
	//std::shared_ptr<IClient> client;
	EventQueue* target;
	std::string username;
	std::string password;
};

#endif
