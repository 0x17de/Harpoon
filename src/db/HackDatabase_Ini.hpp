#ifndef HACKDATABASE_INI_H
#define HACKDATABASE_INI_H

#include "queue/EventQueue.hpp"
#include "queue/EventLoop.hpp"


class HackDatabase_Ini : public EventLoop {
    EventQueue* appQueue;
public:
    explicit HackDatabase_Ini(EventQueue* appQueue);
    virtual ~HackDatabase_Ini();

    virtual bool onEvent(std::shared_ptr<IEvent> event) override;
};

#endif
