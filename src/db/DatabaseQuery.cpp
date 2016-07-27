#include "DatabaseQuery.hpp"


namespace Query {

    Eq::Eq(const std::string& key, const std::string& value)
        : key{key}
        , value{value}
    {
    }

    DatabaseQuery::DatabaseQuery(const std::string& table, And&& rule)
        : table{table}
        , rule{rule}
    {
    }

}

