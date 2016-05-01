#ifndef EVENTLOGIN_H
#define EVENTLOGIN_H

#include <memory>
#include "IEvent.h"


class EventLogin : public IEvent {
public:
	static UUID uuid;
	virtual UUID getEventUuid() const override;
	//std::shared_ptr<IClient> client;
	std::string username;
	std::string password;
};

#endif
