#ifndef EVENTLOGINRESULT_H
#define EVENTLOGINRESULT_H

#include <memory>
#include "IEvent.hpp"


class EventLoginResult : public IEvent {
public:
	static UUID uuid;
	virtual UUID getEventUuid() const override;
	//std::shared_ptr<IClient> client;
	bool success;
	size_t userId;
};

#endif
