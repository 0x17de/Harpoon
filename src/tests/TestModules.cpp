#include <gtest/gtest.h>
#include "queue/EventLoop.hpp"
#include "db/IrcDatabase_Ini.hpp"
#include "db/IrcDatabase_Dummy.hpp"
#include "db/LoginDatabase_Dummy.hpp"
#include "db/handler/Postgres.hpp"
#include "db/LoginDatabase_Ini.hpp"
#include "event/EventInit.hpp"
#include "service/irc/IrcConnection.hpp"
#include "service/irc/IrcBacklogService.hpp"
#include "service/irc/IrcService.hpp"
#include "service/irc/IrcServerConfiguration.hpp"
#include "server/ws/WebsocketServer.hpp"
#include "user/UserManager.hpp"


class SimpleLoop : public EventLoop {
public:
    virtual ~SimpleLoop() {}
    virtual bool onEvent(std::shared_ptr<IEvent> event) override {
        return true;
    }
};

TEST(Modules, ModulesInitDeinit) {
    /*
    bool testRunning;
    std::shared_ptr<SimpleLoop> base;

    std::thread watcher([&]{
            while (testRunning) {
                if (base) {
                    base->getEventQueue()->sendEvent(std::make_shared<EventInit>());
                }
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }
        });
    
    base = std::make_shared<SimpleLoop>();
    EventQueue* q = base->getEventQueue();

    // Check each object for proper initialization/deinitialization
    //{IrcDatabase_Ini ircdatabase_ini(q);}
    //{IrcDatabase_Dummy ircdatabase_dummy(q);}
    //{LoginDatabase_Dummy logindatabase_dummy(q);}
    //{IrcConnection ircconnection(q, 0, IrcServerConfiguration{0, "server"});}
    //{IrcBacklogService ircbacklogservice(q);}
    //{IrcService ircservice(0, q);}
    //{WebsocketServer websocketserver(q);}
    //{UserManager usermanager(q);}

    testRunning = false;
    watcher.join();
    */
}
