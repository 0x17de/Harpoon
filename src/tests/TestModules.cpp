#include <gtest/gtest.h>
#include "queue/EventLoop.hpp"
#include "db/IrcDatabase_Ini.hpp"
#include "db/IrcDatabase_Dummy.hpp"
#include "db/LoginDatabase_Dummy.hpp"
#include "db/handler/Postgres.hpp"
#include "db/LoginDatabase_Ini.hpp"
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
    {
        SimpleLoop simpleLoop;
    }
    SimpleLoop base;
    EventQueue* q = base.getEventQueue();

    // Check each object for proper initialization/deinitialization
    {IrcDatabase_Ini ircdatabase_ini(q);}
    {IrcDatabase_Dummy ircdatabase_dummy(q);}
    {LoginDatabase_Dummy logindatabase_dummy(q);}
    {IrcConnection ircconnection(q, 0, IrcServerConfiguration{0, "server"});}
    {IrcBacklogService ircbacklogservice(q);}
    {IrcService ircservice(0, q);}
    {WebsocketServer websocketserver(q);}
    {UserManager usermanager(q);}
    */
}
