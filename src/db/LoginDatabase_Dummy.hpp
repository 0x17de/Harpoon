#ifndef LOGINDATABASE_DUMMY_H
#define LOGINDATABASE_DUMMY_H

#include "queue/EventQueue.hpp"
#include "queue/EventLoop.hpp"


class LoginDatabase_Dummy : public EventLoop {
    EventQueue* appQueue;
public:
    LoginDatabase_Dummy(EventQueue* appQueue);
    virtual bool onEvent(std::shared_ptr<IEvent> event) override;
};

#endif
