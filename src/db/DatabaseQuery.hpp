#ifndef DATABASEQUERY_H
#define DATABASEQUERY_H

#include <string>
#include <list>


namespace Query {

    class Filter {
    protected:
        Filter() = default;
    };

    class Eq : public Filter {
        std::string key;
        std::string value;
    public:
        Eq(const std::string& key, const std::string& value);
    };

    class Or : public Filter {
        std::list<Filter> rules;
    public:
        template<class ...T>
        Or(T&&...t) : rules{std::forward<T>(t)...} {};
    };

    class And : public Filter {
        std::list<Filter> rules;
    public:
        template<class ...T>
        And(T&&...t) : rules{std::forward<T>(t)...} {};
    };

    class DatabaseQuery {
        std::string table;
        And rule;
    public:
        DatabaseQuery(const std::string& table, And&& rule);
    };

}

#endif
