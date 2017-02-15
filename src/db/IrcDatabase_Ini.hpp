#ifndef IRCDATABASE_INI_H
#define IRCDATABASE_INI_H

#include "queue/EventQueue.hpp"
#include "queue/EventLoop.hpp"
#include "GenericIniDatabase.hpp"


class IrcDatabase_Ini : public EventLoop, public GenericIniDatabase {
    EventQueue* appQueue_;
public:
    explicit IrcDatabase_Ini(EventQueue* appQueue);
    virtual ~IrcDatabase_Ini();

    virtual bool onEvent(std::shared_ptr<IEvent> event) override;
};

#endif
