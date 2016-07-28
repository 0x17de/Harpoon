#ifndef DATABASEQUERY_H
#define DATABASEQUERY_H

#include <string>
#include <list>
#include <tuple>
#include <type_traits>


namespace Query {

    // BASE TYPES
    class Filter {
    protected:
        Filter() = default;
    };

    class FilterLogic : public Filter {
    protected:
        FilterLogic() = default;
    };

    class JoinLogic {
    protected:
        JoinLogic() = default;
    };

    // FILTER
    class Eq : public Filter {
        std::string key;
        std::string value;
    public:
        Eq(const std::string& key, const std::string& value);
    };

    template<class ...T>
    class Or : public FilterLogic {
        std::tuple<T...> rules;
    public:
        Or(T&&...t) : rules{std::forward<T>(t)...} {};
    };

    template<class ...T>
    class And : public FilterLogic {
        std::tuple<T...> rules;
    public:
        And(T&&...t) : rules{std::forward<T>(t)...} {};
    };

    class Any : public FilterLogic {
    };

    // JOIN
    class Join : public JoinLogic {
        std::string table;
        Eq comparison;
    public:
        Join(const std::string& table, Eq&& comparison);
    };

    class NoJoin : public JoinLogic {
    };

    // QUERY
    template<class R = Any, class J = NoJoin>
    class DatabaseQuery {
        std::string table;
        std::list<std::string> columns;
        R rule;
        J join;
    public:
        static_assert(std::is_base_of<FilterLogic, R>::value, "T must be descendant of FilterLogic");
        static_assert(std::is_base_of<JoinLogic, J>::value, "J must be descendant of JoinLogic");
        static_assert(!std::is_same<FilterLogic, R>::value, "T must be descendant of FilterLogic");
        static_assert(!std::is_same<JoinLogic, J>::value, "J must be descendant of JoinLogic");

        DatabaseQuery(const std::string& table,
                      std::list<std::string>&& columns,
                      R&& rule = R{},
                      std::list<J>&& join = {})
            : columns{columns}
            , rule{std::forward(rule)}
            , join{std::forward(join)}
        {
        };
    };

}

#endif
