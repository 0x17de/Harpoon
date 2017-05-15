#ifndef LOGINDATABASE_INI_H
#define LOGINDATABASE_INI_H

#include "queue/EventQueue.hpp"
#include "queue/EventLoop.hpp"
#include "utils/Ini.hpp"


/// Used to validate users.
/// Stores users as categories having salt and password.
/// To add new users use the --genuser cmdline option together with --save.
class LoginDatabase_Ini : public EventLoop {
    EventQueue* appQueue;
    Ini usersIni;
public:
    explicit LoginDatabase_Ini(EventQueue* appQueue);
    virtual ~LoginDatabase_Ini();

    virtual bool onEvent(std::shared_ptr<IEvent> event) override;
};

#endif
