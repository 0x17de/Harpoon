#ifndef DATABASE_QUERYCREATE_STORE_H
#define DATABASE_QUERYCREATE_STORE_H

#include <list>
#include <string>
#include <memory>


namespace Query {
    enum class FieldType {
        Id,
        Time,
        Integer,
        Text,
        Bool
    };

    struct QueryCreate_Field {
        std::string name;
        FieldType type;
    };

    struct QueryCreate_Store {
        std::string name;
        std::list<QueryCreate_Field> fields;
    };

    using Create = std::unique_ptr<QueryCreate_Store>;
}

#endif
