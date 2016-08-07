#include "test.hpp"
#include <memory>

using namespace std;

#define private public
#include "queue/EventLoop.hpp"
#include "db/handler/Postgres.hpp"
#include "event/EventQuit.hpp"


struct PostgresChecker : public EventLoop {
    virtual bool onEvent(std::shared_ptr<IEvent> event) override {
        return false;
    }
};

TEST(Connect,
     ([]{
         PostgresChecker checker;
         checker.getEventQueue()->sendEvent(make_shared<EventQuit>());
         checker.join();
     }));


