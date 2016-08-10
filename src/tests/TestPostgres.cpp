#include "test.hpp"
#include <memory>
#include <sstream>
#include <soci/soci.h>

using namespace std;

#include "queue/EventLoop.hpp"
#include "db/handler/Postgres.hpp"
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

    void testConnect() {
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

        // cleanup
        session->once << "DROP TABLE test_postgres";
        ASSERT_EQUAL(false, exists("test_postgres"));
    }

    void testWrite() {
        tryDrop("test_postgres_write");
        session->once << "CREATE TABLE test_postgres_write (id serial, key text)";
        ASSERT_EQUAL(true, exists("test_postgres_write"));

        size_t count;
        size_t id;
        string key;

        // expect no inserted element yet
        count = 0;
        soci::statement stCheck1 = (session->prepare << "SELECT id, key FROM test_postgres_write", soci::into(id), soci::into(key));
        stCheck1.execute();
        while (stCheck1.fetch())
            ++count;
        ASSERT_EQUAL(0, count);

        // classic insert
        vector<string> textToInsert = {"test0", "test1"};
        session->once << "INSERT INTO test_postgres_write (id, key) VALUES (1, :text1), (2, :text2)", soci::use(textToInsert[0]), soci::use(textToInsert[1]);

        // check for inserted elements
        count = 0;
        soci::statement stCheck2 = (session->prepare << "SELECT id, key FROM test_postgres_write", soci::into(id), soci::into(key));
        stCheck2.execute();
        while (stCheck2.fetch()) {
            if (id == 1)
                ASSERT_EQUAL(textToInsert[0], key);
            else if (id == 2)
                ASSERT_EQUAL(textToInsert[1], key);
            else
                ASSERT_INVALID("Unreachable");
            ++count;
        }
        ASSERT_EQUAL(2, count);

        // cleanup
        session->once << "DROP TABLE test_postgres_write";
        ASSERT_EQUAL(false, exists("test_postgres_write"));
    }

    void testWeirdWrite() {
        tryDrop("test_postgres_weirdwrite");
        session->once << "CREATE TABLE test_postgres_weirdwrite (id serial, key text)";
        ASSERT_EQUAL(true, exists("test_postgres_weirdwrite"));

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
                ASSERT_EQUAL(textToInsert[0], key);
            else if (id == 2)
                ASSERT_EQUAL(textToInsert[1], key);
            else
                ASSERT_INVALID("Unreachable");
            ++count;
        }
        ASSERT_EQUAL(2, count);

        // cleanup
        session->once << "DROP TABLE test_postgres_weirdwrite";
        ASSERT_EQUAL(false, exists("test_postgres_weirdwrite"));
    }

    virtual bool onEvent(std::shared_ptr<IEvent> event) override {
        return false;
    }
};

TEST(PostgresConnect,
     ([]{
         PostgresChecker checker;
         checker.testConnect();
     }));

TEST(PostgresWrite,
     ([]{
         PostgresChecker checker;
         checker.testWrite();
     }));

TEST(PostgresWeirdWrite,
     ([]{
         PostgresChecker checker;
         checker.testWeirdWrite();
     }));

