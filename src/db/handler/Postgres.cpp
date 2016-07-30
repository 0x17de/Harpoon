#include "Postgres.hpp"
#include "utils/ModuleProvider.hpp"
#include "event/EventQuit.hpp"
#include "event/EventInit.hpp"

using namespace std;


namespace Database {

    PROVIDE_MODULE("database", "postgres", Postgres);


    struct Postgres_Impl {
        EventQueue* appQueue;
        Postgres_Impl(EventQueue* appQueue) : appQueue{appQueue} {};
        bool onEvent(std::shared_ptr<IEvent> event);
        friend Postgres;
    };

    Postgres::Postgres(EventQueue* appQueue)
        : EventLoop{}
        , impl{make_shared<Postgres_Impl>(appQueue)}
    {
    }

    bool Postgres::onEvent(std::shared_ptr<IEvent> event) {
        return impl->onEvent(event);
    }

    bool Postgres_Impl::onEvent(std::shared_ptr<IEvent> event) {
        UUID eventType = event->getEventUuid();
        if (eventType == EventQuit::uuid) {
            return false;
        } else if (eventType == EventInit::uuid) {

        }
        return true;
    }

}
