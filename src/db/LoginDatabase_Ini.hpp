#ifndef LOGINDATABASE_INI_H
#define LOGINDATABASE_INI_H

#include "queue/EventQueue.hpp"
#include "queue/EventLoop.hpp"
#include "utils/Ini.hpp"


class LoginDatabase_Ini : public EventLoop {
	EventQueue* appQueue;
	Ini usersIni;
public:
	LoginDatabase_Ini(EventQueue* appQueue);
	virtual bool onEvent(std::shared_ptr<IEvent> event) override;
};

#endif
