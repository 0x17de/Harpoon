#ifndef DATABASE_DETAILS_CREATE_H
#define DATABASE_DETAILS_CREATE_H

#include <memory>
#include <utility>
#include <exception>

#include "Database_QueryCreate_Store.hpp"


namespace details {
    using namespace Query;

    struct TmpQueryCreate_CREATE {
        std::unique_ptr<QueryCreate_Store> store;

        explicit inline TmpQueryCreate_CREATE(const std::string& name)
            : store{cpp11::make_unique<QueryCreate_Store>()}
        {
            store->name = name;
        }

        inline operator Query::Create() {
            return std::move(store);
        }

        inline TmpQueryCreate_CREATE& field(const std::string& str, FieldType type) {
            store->fields.emplace_back(str, type);
            return *this;
        }
    };
}

#endif
