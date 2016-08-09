#ifndef DATABASEPOSTGRES_H
#define DATABASEPOSTGRES_H

#include <memory>
#include "queue/EventLoop.hpp"


class EventQueue;
namespace Database {

    struct Postgres_Impl;
    class Postgres : public EventLoop {
        std::shared_ptr<Postgres_Impl> impl;
    public:
        Postgres(EventQueue* appQueue);
        virtual ~Postgres();

        virtual bool onEvent(std::shared_ptr<IEvent> event) override;
    };

};


#endif
