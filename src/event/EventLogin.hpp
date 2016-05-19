#ifndef EVENTLOGIN_H
#define EVENTLOGIN_H

#include <memory>
#include "IEvent.hpp"


class EventQueue;
class EventLogin : public IEvent {
	EventQueue* target;
	void* data;
	std::string username;
	std::string password;
public:
	EventLogin(EventQueue* target, void* data, std::string username, std::string password);

	static UUID uuid;
	virtual UUID getEventUuid() const override;

	EventQueue* getTarget() const;
	void* getData() const;
	std::string getUsername() const;
	std::string getPassword() const;
};

#endif
