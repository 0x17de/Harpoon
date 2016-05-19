#ifndef EVENTLOGINRESULT_H
#define EVENTLOGINRESULT_H

#include <memory>
#include "IEvent.hpp"


class EventLoginResult : public IEvent {
	bool success;
	size_t userId;
	void* data;
public:
	EventLoginResult(bool success, size_t userId, void* data);

	static UUID uuid;
	virtual UUID getEventUuid() const override;

	bool getSuccess() const;
	size_t getUserId() const;
	void* getData() const;
};

#endif
