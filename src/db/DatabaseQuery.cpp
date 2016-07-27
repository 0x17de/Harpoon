#include "DatabaseQuery.hpp"


namespace Query {

    Eq::Eq(const std::string& key, const std::string& value)
        : key{key}
        , value{value}
    {
    }

    Join::Join(const std::string& table, Eq&& comparison)
        : table{table}
        , comparison{comparison}
    {
    }

}
