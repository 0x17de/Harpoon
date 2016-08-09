#include "test.hpp"
#include <memory>
#include <iostream>
#include <sstream>
#include <soci/soci.h>

using namespace std;

#include "queue/EventLoop.hpp"
#include "db/handler/Postgres.hpp"
#include "event/EventQuit.hpp"
#include "utils/Ini.hpp"


struct PostgresChecker : public EventLoop {
    std::shared_ptr<soci::session> session;

    PostgresChecker() {
        Ini settings("config/postgres.ini");
        ASSERT_EQUAL(false, settings.isNew());

        string host,
            port,
            username,
            password,
            database;
        auto& auth = settings.expectCategory("auth");
        settings.getEntry(auth, "host", host);
        settings.getEntry(auth, "port", port);
        settings.getEntry(auth, "username", username);
        settings.getEntry(auth, "password", password);
        settings.getEntry(auth, "database", database);

        // login into the database
        stringstream login;
        login << "postgresql://";
        if (host.size() > 0)
            login << "host=" << host << " ";
        if (port.size() > 0)
            login << "port=" << port << " ";
        login << "dbname=" << database << " "
              << "user=" << username << " "
              << "password=" << password;

        cout << login.str() << endl;;
        ASSERT_NOTHROW(([&]{
            session = make_shared<soci::session>(login.str());
        }));
    }

    virtual ~PostgresChecker() {
    }

    virtual bool onEvent(std::shared_ptr<IEvent> event) override {
        return false;
    }
};

TEST(Connect,
     ([]{
         PostgresChecker checker;
     }));

