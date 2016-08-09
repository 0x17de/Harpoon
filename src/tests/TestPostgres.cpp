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


struct DatabaseHelper {
    shared_ptr<soci::session> session;
    void login() {
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

        ASSERT_NOTHROW(([&]{
            session = make_shared<soci::session>(login.str());
        }));
    }
    bool  exists(const std::string& table) {
        session->once << "SELECT * FROM information_schema.tables WHERE table_name = :table_name LIMIT 1", soci::use(table);
        return session->got_data();
    }
    void tryDrop(const std::string& table) {
        if (exists(table))
            session->once << "DROP TABLE " << table;
    }
    void compareColumns(const std::string& table,
                        const std::map<std::string, std::string>& mapping) {
        std::string columnName, dataType;
        soci::statement s = (session->prepare << "SELECT column_name, data_type FROM information_schema.columns WHERE table_name = 'test_postgres'", soci::into(columnName), soci::into(dataType));
        s.execute();
        size_t column_count = 0;
        while (s.fetch()) {
            auto it = mapping.find(columnName);
            ASSERT_EQUAL(false, it == mapping.end());
            ASSERT_EQUAL(it->second, dataType);
            ++column_count;
        }
        ASSERT_EQUAL(mapping.size(), column_count);
    }
};

struct PostgresChecker : public EventLoop, public DatabaseHelper {
    PostgresChecker() {
        login();
    }

    virtual ~PostgresChecker() {
    }

    void test1() {
        tryDrop("test_postgres");
        ASSERT_NOTHROW(([this]{
            session->once << "SELECT * FROM information_schema.tables";
        }));
        ASSERT_EQUAL(false, exists("test_postgres"));
        session->once << "CREATE TABLE test_postgres (id serial, key text)";
        ASSERT_EQUAL(true, exists("test_postgres"));

        compareColumns("test_postgres", {
            {"id", "integer"}, // serial converts to integer
            {"key", "text"}
        });

        session->once << "DROP TABLE test_postgres";
        ASSERT_EQUAL(false, exists("test_postgres"));
    }

    virtual bool onEvent(std::shared_ptr<IEvent> event) override {
        return false;
    }
};

TEST(Connect,
     ([]{
         PostgresChecker checker;
         checker.test1();
     }));

