#include <gtest/gtest.h>
#include <memory>
#include <sstream>
#include <mutex>
#include <iostream>
#include <condition_variable>
#include <soci/soci.h>

using namespace std;

#include "queue/EventLoop.hpp"
#include "event/EventInit.hpp"
#include "event/EventDatabaseQuery.hpp"
#include "event/EventDatabaseResult.hpp"
#include "db/handler/Postgres.hpp"
#include "db/query/Database_Query.hpp"
#include "utils/Ini.hpp"


struct DatabaseHelper {
    shared_ptr<soci::session> session;
    void login() {
        Ini settings("config/postgres.ini");
        ASSERT_EQ(false, settings.isNew());

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

        ASSERT_NO_THROW(([&]{
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
        ASSERT_EQ(false, exists(table));
    }
    void compareColumns(const std::string& table,
                        const std::map<std::string, std::string>& mapping) {
        std::string columnName, dataType;
        soci::statement s = (session->prepare << "SELECT column_name, data_type FROM information_schema.columns WHERE table_name = :table_name", soci::into(columnName), soci::into(dataType), soci::use(table));
        s.execute();
        size_t column_count = 0;
        while (s.fetch()) {
            auto it = mapping.find(columnName);
            ASSERT_EQ(false, it == mapping.end());
            ASSERT_EQ(it->second, dataType);
            ++column_count;
        }
        ASSERT_EQ(mapping.size(), column_count);
    }
};

struct PostgresChecker : public EventLoop, public DatabaseHelper {
    PostgresChecker() {
        login();
    }

    virtual ~PostgresChecker() {
    }

    void testConnect() {
        tryDrop("test_postgres");
        ASSERT_NO_THROW(([this]{
            session->once << "SELECT * FROM information_schema.tables";
        }));
        ASSERT_EQ(false, exists("test_postgres"));
        session->once << "CREATE TABLE test_postgres (id serial, key text)";
        ASSERT_EQ(true, exists("test_postgres"));

        compareColumns("test_postgres", {
            {"id", "integer"}, // serial converts to integer
            {"key", "text"}
        });

        // cleanup
        session->once << "DROP TABLE test_postgres";
        ASSERT_EQ(false, exists("test_postgres"));
    }

    void testWrite() {
        tryDrop("test_postgres_write");
        session->once << "CREATE TABLE test_postgres_write (id serial, key text)";
        ASSERT_EQ(true, exists("test_postgres_write"));

        size_t count;
        size_t id;
        string key;

        // expect no inserted element yet
        count = 0;
        soci::statement stCheck1 = (session->prepare << "SELECT id, key FROM test_postgres_write", soci::into(id), soci::into(key));
        stCheck1.execute();
        while (stCheck1.fetch())
            ++count;
        ASSERT_EQ(true, count == 0);

        // classic insert
        vector<string> textToInsert = {"test0", "test1"};
        session->once << "INSERT INTO test_postgres_write (id, key) VALUES (1, :text1), (2, :text2)", soci::use(textToInsert[0]), soci::use(textToInsert[1]);

        // check for inserted elements
        count = 0;
        soci::statement stCheck2 = (session->prepare << "SELECT id, key FROM test_postgres_write", soci::into(id), soci::into(key));
        stCheck2.execute();
        while (stCheck2.fetch()) {
            if (id == 1)
                ASSERT_EQ(textToInsert[0], key);
            else if (id == 2)
                ASSERT_EQ(textToInsert[1], key);
            else
                ADD_FAILURE(); // Unreachable
            ++count;
        }
        ASSERT_EQ(true, count == 2);

        // cleanup
        session->once << "DROP TABLE test_postgres_write";
        ASSERT_EQ(false, exists("test_postgres_write"));
    }

    void testWeirdWrite() {
        tryDrop("test_postgres_weirdwrite");
        session->once << "CREATE TABLE test_postgres_weirdwrite (id serial, key text)";
        ASSERT_EQ(true, exists("test_postgres_weirdwrite"));

        vector<string> textToInsert = {"test0", "test1"};

        // weird insert
        {
            auto once(session->once);
            // temporary variable q is required. Query is executed once it is destructed
            auto q = once << "INSERT INTO test_postgres_weirdwrite (id, key) VALUES ";

            // basic statement
            for (size_t i = 0; i < textToInsert.size(); ++i) {
                if (i != 0) q << ", "; // comma to separate inserts
                q << "(" << (i+1) << ", " << ":key" << i << ")";
            }
            // use values from vector
            for (size_t i = 0; i < textToInsert.size(); ++i)
                q, soci::use(textToInsert[i]);
        }

        // check for inserted elements
        size_t count = 0;
        size_t id;
        string key;
        soci::statement st = (session->prepare << "SELECT id, key FROM test_postgres_weirdwrite", soci::into(id), soci::into(key));
        st.execute();
        while (st.fetch()) {
            if (id == 1)
                ASSERT_EQ(textToInsert[0], key);
            else if (id == 2)
                ASSERT_EQ(textToInsert[1], key);
            else
                ADD_FAILURE(); // Unreachable
            ++count;
        }
        ASSERT_EQ(true, count == 2);

        // cleanup
        session->once << "DROP TABLE test_postgres_weirdwrite";
        ASSERT_EQ(false, exists("test_postgres_weirdwrite"));
    }

    virtual bool onEvent(std::shared_ptr<IEvent> event) override {
        return false;
    }
};

struct PostgresHandlerChecker : public EventLoop, public DatabaseHelper {
    Database::Postgres handler;
    std::condition_variable setupWaitCondition;
    std::list<std::shared_ptr<IEvent>> results;

    PostgresHandlerChecker()
        : handler{getEventQueue()}
    {
        login();
        handler.getEventQueue()->sendEvent(make_shared<EventInit>());
    }

    bool waitForEvent() {
        std::mutex setupWaitMutex;
        std::unique_lock<mutex> setupWait(setupWaitMutex);
        if (std::cv_status::timeout == setupWaitCondition.wait_for(setupWait, std::chrono::seconds(3)))
            return false;
        return true;
    }

    void testHandler() {
        using namespace Query;

        tryDrop("test_postgreshandler");

        // create table
        {
            Create stmt = create("test_postgreshandler")
                .field("id", FieldType::Id)
                .field("key", FieldType::Text);
            auto eventSetup = make_shared<EventDatabaseQuery>(getEventQueue(), make_shared<EventInit>(), std::move(stmt));

            handler.getEventQueue()->sendEvent(eventSetup);
        }

        // wait till table is created
        ASSERT_EQ(true, waitForEvent());

        // check result
        ASSERT_EQ(true, results.size() == 1);
        ASSERT_EQ(true, results.back()->as<EventDatabaseResult>()->getSuccess());
        results.clear();

        // check table columns
        ASSERT_EQ(true, exists("test_postgreshandler"));
        compareColumns("test_postgreshandler", {
            {"id", "integer"}, // serial converts to integer
            {"key", "text"}
        });

        // insert test elements
        {
            Insert stmt = insert()
                .into("test_postgreshandler")
                .format("id", "key")
                .data(vector<string>{"1", "test0", "2", "test1"});
            auto eventInsert = make_shared<EventDatabaseQuery>(getEventQueue(), make_shared<EventInit>(), std::move(stmt));

            handler.getEventQueue()->sendEvent(eventInsert);
        }

        ASSERT_EQ(true, waitForEvent());

        // check result
        ASSERT_EQ(true, results.size() == 1);
        ASSERT_EQ(true, results.back()->as<EventDatabaseResult>()->getSuccess());
        results.clear();

        {
            // check for inserted elements
            size_t count = 0;
            size_t id;
            string key;
            soci::statement st = (session->prepare << "SELECT id, key FROM test_postgreshandler", soci::into(id), soci::into(key));
            st.execute();
            while (st.fetch()) {
                if (id == 1)
                    ASSERT_EQ("test0", key);
                else if (id == 2)
                    ASSERT_EQ("test1", key);
                else
                    ADD_FAILURE(); // Unreachable
                ++count;
            }
            ASSERT_EQ(true, count == 2);
        }

        {
            Select stmt = select("id", "key")
                .from("test_postgreshandler")
                .where(make_var("id") == make_constant("1"));
            auto eventFetch = make_shared<EventDatabaseQuery>(getEventQueue(), make_shared<EventInit>(), std::move(stmt));

            handler.getEventQueue()->sendEvent(eventFetch);
        }

        ASSERT_EQ(true, waitForEvent());

        // check result
        {
            ASSERT_EQ(true, results.size() == 1);
            auto result = results.back()->as<EventDatabaseResult>();
            ASSERT_EQ(true, result != nullptr);
            ASSERT_EQ(true, result->getSuccess());

            auto& resultList = result->getResults();
            ASSERT_EQ(true, resultList.size() == 2);
            auto it = resultList.begin();
            ASSERT_EQ("1", *it++);
            ASSERT_EQ("test0", *it);

            // cleanup
            results.clear();
        }


        // test limit
        {
            Select stmt = select("id")
                .from("test_postgreshandler")
                .limit(2);
            auto eventFetch = make_shared<EventDatabaseQuery>(getEventQueue(), make_shared<EventInit>(), std::move(stmt));

            handler.getEventQueue()->sendEvent(eventFetch);
        }

        ASSERT_EQ(true, waitForEvent());

        // check result
        {
            ASSERT_EQ(true, results.size() == 1);
            auto dbResult = results.back()->as<EventDatabaseResult>();
            ASSERT_EQ(true, dbResult->getSuccess());
            ASSERT_EQ(2uL, dbResult->getResults().size());
            results.clear();
        }


        // test order
        {
            Select stmt = select("id")
                .from("test_postgreshandler")
                .order_by("id", "DESC")
                .limit(1);
            auto eventFetch = make_shared<EventDatabaseQuery>(getEventQueue(), make_shared<EventInit>(), std::move(stmt));

            handler.getEventQueue()->sendEvent(eventFetch);
        }

        ASSERT_EQ(true, waitForEvent());

        // check result
        {
            ASSERT_EQ(true, results.size() == 1);
            auto dbResult = results.back()->as<EventDatabaseResult>();
            ASSERT_EQ(true, dbResult->getSuccess());
            ASSERT_EQ(1uL, dbResult->getResults().size());
            ASSERT_EQ("2", dbResult->getResults().front());
            results.clear();
        }


        // test compare lower
        {
            Select stmt = select("id", "key")
                .from("test_postgreshandler")
                .where(make_var("id") < make_constant("2"));
            auto eventFetch = make_shared<EventDatabaseQuery>(getEventQueue(), make_shared<EventInit>(), std::move(stmt));

            handler.getEventQueue()->sendEvent(eventFetch);
        }

        ASSERT_EQ(true, waitForEvent());

        // check result
        {
            ASSERT_EQ(true, results.size() == 1);
            auto dbResult = results.back()->as<EventDatabaseResult>();
            ASSERT_EQ(true, dbResult->getSuccess());
            ASSERT_EQ(2uL, dbResult->getResults().size());
            ASSERT_EQ("1", dbResult->getResults().front());
            ASSERT_EQ("test0", dbResult->getResults().back());
            results.clear();
        }


        // test compare greater
        {
            Select stmt = select("id")
                .from("test_postgreshandler")
                .where(make_var("id") > make_constant("1"));
            auto eventFetch = make_shared<EventDatabaseQuery>(getEventQueue(), make_shared<EventInit>(), std::move(stmt));

            handler.getEventQueue()->sendEvent(eventFetch);
        }

        ASSERT_EQ(true, waitForEvent());

        // check result
        {
            ASSERT_EQ(true, results.size() == 1);
            auto dbResult = results.back()->as<EventDatabaseResult>();
            ASSERT_EQ(true, dbResult->getSuccess());
            ASSERT_EQ(1uL, dbResult->getResults().size());
            ASSERT_EQ("2", dbResult->getResults().back());
            results.clear();
        }


        // test update element
        {
            Update stmt1 = update("test_postgreshandler")
                .format("key")
                .data(std::vector<std::string>{"replaced"})
                .where(make_var("id") == make_constant("1"));
            auto eventUpdate = make_shared<EventDatabaseQuery>(getEventQueue(), make_shared<EventInit>(), std::move(stmt1));

            handler.getEventQueue()->sendEvent(eventUpdate);

            ASSERT_EQ(true, waitForEvent());
            results.clear();

            Select stmt2 = select("key")
                .from("test_postgreshandler")
                .where(make_var("id") == make_constant("1"));
            auto eventFetch = make_shared<EventDatabaseQuery>(getEventQueue(), make_shared<EventInit>(), std::move(stmt2));

            handler.getEventQueue()->sendEvent(eventFetch);
        }

        ASSERT_EQ(true, waitForEvent());

        // check result
        {
            ASSERT_EQ(true, results.size() == 1);
            auto dbResult = results.back()->as<EventDatabaseResult>();
            ASSERT_EQ(true, dbResult->getSuccess());
            ASSERT_EQ(1uL, dbResult->getResults().size());
            ASSERT_EQ("replaced", dbResult->getResults().back());
            results.clear();
        }


        // delete test elements
        {
            Delete stmt = erase()
                .from("test_postgreshandler")
                .where(make_var("id") == make_constant("2"));
            auto eventDelete = make_shared<EventDatabaseQuery>(getEventQueue(), make_shared<EventInit>(), std::move(stmt));

            handler.getEventQueue()->sendEvent(eventDelete);
        }

        ASSERT_EQ(true, waitForEvent());

        // check result
        ASSERT_EQ(true, results.size() == 1);
        ASSERT_EQ(true, results.back()->as<EventDatabaseResult>()->getSuccess());
        results.clear();

        {
            // check for inserted elements
            size_t count = 0;
            size_t id;
            string key;
            soci::statement st = (session->prepare << "SELECT id, key FROM test_postgreshandler", soci::into(id), soci::into(key));
            st.execute();
            while (st.fetch()) {
                if (id == 1)
                    ASSERT_EQ("replaced", key);
                else
                    ADD_FAILURE(); // Unreachable
                ++count;
            }
            ASSERT_EQ(true, count == 1);
        }

        // cleanup
        session->once << "DROP TABLE test_postgreshandler";
        ASSERT_EQ(false, exists("test_postgreshandler"));
    }

    void testJoin() {
        using namespace Query;

        tryDrop("test_postgresjoin");
        tryDrop("test_postgresjoin_name");

        // create table
        {
            Create stmt1 = create("test_postgresjoin")
                .field("id", FieldType::Id)
                .field("key", FieldType::Text)
                .field("name_ref", FieldType::Integer);
            Create stmt2 = create("test_postgresjoin_name")
                .field("name_id", FieldType::Id)
                .field("name", FieldType::Text);

            auto eventSetup = make_shared<EventDatabaseQuery>(getEventQueue(),
                                                              make_shared<EventInit>(),
                                                              std::move(stmt1),
                                                              std::move(stmt2));

            handler.getEventQueue()->sendEvent(eventSetup);
        }

        // wait till table is created
        ASSERT_EQ(true, waitForEvent());

        // check result
        ASSERT_EQ(true, results.size() == 1);
        ASSERT_EQ(true, results.back()->as<EventDatabaseResult>()->getSuccess());
        results.clear();

        // check table columns
        ASSERT_EQ(true, exists("test_postgresjoin"));
        ASSERT_EQ(true, exists("test_postgresjoin_name"));
        compareColumns("test_postgresjoin", {
            {"id", "integer"}, // serial converts to integer
            {"key", "text"},
            {"name_ref", "integer"}
        });
        compareColumns("test_postgresjoin_name", {
            {"name_id", "integer"}, // serial converts to integer
            {"name", "text"}
        });

        // insert test elements
        {
            Insert stmt = insert()
                .into("test_postgresjoin")
                .format("id", "key")
                .join("test_postgresjoin_name", "name", "testname")
                .data(std::vector<std::string>{"1", "test0"});
            auto eventInsert = make_shared<EventDatabaseQuery>(getEventQueue(), make_shared<EventInit>(), std::move(stmt));

            handler.getEventQueue()->sendEvent(eventInsert);
        }

        ASSERT_EQ(true, waitForEvent());

        // check result
        ASSERT_EQ(true, results.size() == 1);
        ASSERT_EQ(true, results.back()->as<EventDatabaseResult>()->getSuccess());
        results.clear();

        {
            // check for inserted elements
            size_t count = 0;
            size_t id;
            string name;
            soci::indicator id_ind, name_ind;
            soci::statement st = (session->prepare << "SELECT name_id, name FROM test_postgresjoin_name", soci::into(id, id_ind), soci::into(name, name_ind));
            st.execute();
            while (st.fetch()) {
                ASSERT_EQ(true, id_ind == soci::i_ok);
                ASSERT_EQ(true, name_ind == soci::i_ok);
                if (id == 1) {
                    ASSERT_EQ(true, id == 1);
                    ASSERT_EQ("testname", name);
                } else
                    ADD_FAILURE(); // Unreachable
                ++count;
            }
            ASSERT_EQ(true, count == 1);
        }

        {
            // check for inserted elements
            size_t count = 0;
            size_t id;
            string key;
            size_t name_ref;
            soci::indicator id_ind, key_ind, name_ref_ind;
            soci::statement st = (session->prepare << "SELECT id, key, name_ref FROM test_postgresjoin WHERE id = '1'", soci::into(id, id_ind), soci::into(key, key_ind), soci::into(name_ref, name_ref_ind));
            st.execute();
            while (st.fetch()) {
                ASSERT_EQ(true, id_ind == soci::i_ok);
                ASSERT_EQ(true, key_ind == soci::i_ok);
                ASSERT_EQ(true, name_ref_ind == soci::i_ok);
                if (id == 1) {
                    ASSERT_EQ("test0", key);
                    ASSERT_EQ(true, name_ref != 0);
                } else
                    ADD_FAILURE(); // Unreachable
                ++count;
            }
            ASSERT_EQ(true, count == 1);
        }

        {
            // check for inserted elements
            size_t count = 0;
            size_t id;
            string key;
            string name;
            soci::indicator id_ind, key_ind, name_ind;
            soci::statement st = (session->prepare << "SELECT id, key, name FROM test_postgresjoin LEFT JOIN test_postgresjoin_name ON name_ref = name_id WHERE id = '1'", soci::into(id, id_ind), soci::into(key, key_ind), soci::into(name, name_ind));
            st.execute();
            while (st.fetch()) {
                ASSERT_EQ(true, id_ind == soci::i_ok);
                ASSERT_EQ(true, key_ind == soci::i_ok);
                ASSERT_EQ(true, name_ind == soci::i_ok);
                if (id == 1) {
                    ASSERT_EQ("test0", key);
                    ASSERT_EQ("testname", name);
                } else
                    ADD_FAILURE(); // Unreachable
                ++count;
            }
            ASSERT_EQ(true, count == 1);
        }

        {
            Select stmt = select("id", "key", "name")
                .from("test_postgresjoin")
                .join("test_postgresjoin_name", "name")
                .where(make_var("id") == make_constant("1"));
            auto eventFetch = make_shared<EventDatabaseQuery>(getEventQueue(), make_shared<EventInit>(), std::move(stmt));

            handler.getEventQueue()->sendEvent(eventFetch);
        }

        ASSERT_EQ(true, waitForEvent());

        // check result
        {
            ASSERT_EQ(true, results.size() == 1);
            auto dbResult = results.back()->as<EventDatabaseResult>();
            ASSERT_EQ(true, dbResult->getSuccess());
            ASSERT_EQ(3uL, dbResult->getResults().size());

            auto it = dbResult->getResults().begin();
            ASSERT_EQ("1", *it++);
            ASSERT_EQ("test0", *it++);
            ASSERT_EQ("testname", *it++);
            results.clear();
        }

        // cleanup
        session->once << "DROP TABLE test_postgresjoin";
        session->once << "DROP TABLE test_postgresjoin_name";
        ASSERT_EQ(false, exists("test_postgresjoin"));
        ASSERT_EQ(false, exists("test_postgresjoin_name"));
    }

    void testTypes() {
        using namespace Query;

        tryDrop("test_postgrestypes");

        // create table
        {
            Create stmt = create("test_postgrestypes")
                .field("id", FieldType::Id)
                .field("key", FieldType::Text)
                .field("time", FieldType::Time)
                .field("number", FieldType::Integer);
            auto eventSetup = make_shared<EventDatabaseQuery>(getEventQueue(), make_shared<EventInit>(), std::move(stmt));

            handler.getEventQueue()->sendEvent(eventSetup);
        }

        ASSERT_EQ(true, waitForEvent());

        session->once << "DROP TABLE test_postgrestypes";
        ASSERT_EQ(false, exists("test_postgrestypes"));
    }

    virtual bool onEvent(std::shared_ptr<IEvent> event) override {
        UUID eventUuid = event->getEventUuid();
        if (eventUuid == EventDatabaseResult::uuid) {
            results.push_back(event);
            setupWaitCondition.notify_one();
        }
        return true;
    }
};

TEST(Postgres, PostgresConnect) {
    PostgresChecker checker;
    checker.testConnect();
}

TEST(Postgres, PostgresWrite) {
    PostgresChecker checker;
    checker.testWrite();
}

TEST(Postgres, PostgresWeirdWrite) {
    PostgresChecker checker;
    checker.testWeirdWrite();
}

TEST(Postgres, PostgresHandler) {
    PostgresHandlerChecker checker;
    checker.testHandler();
}

TEST(Postgres, PostgresHandlerJoin) {
    PostgresHandlerChecker checker;
    checker.testJoin();
}

TEST(Postgres, PostgresHandlerTypes) {
    PostgresHandlerChecker checker;
    checker.testTypes();
}
