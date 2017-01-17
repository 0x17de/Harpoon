#ifndef DATABASE_DETAILS_INSERT_H
#define DATABASE_DETAILS_INSERT_H

#include <memory>
#include <utility>
#include <exception>

#include "Database_QueryInsert_Store.hpp"


namespace details {
    using namespace Query;

    struct TmpQueryInsert_DATA {
        std::unique_ptr<QueryInsert_Store> store;

        explicit inline TmpQueryInsert_DATA(std::unique_ptr<QueryInsert_Store> store)
            : store{std::move(store)}
        { }

        inline operator Query::Insert() {
            return std::move(store);
        }

        TmpQueryInsert_DATA& data(std::vector<std::string>::iterator start,
                                  std::vector<std::string>::iterator end)
        {
            store->data.reserve(store->data.size()+std::distance(start, end));
            store->data.insert(store->data.end(), start, end);
            return *this;
        }
    };

    struct TmpQueryInsert_JOIN {
        std::unique_ptr<QueryInsert_Store> store;

        explicit inline TmpQueryInsert_JOIN(std::unique_ptr<QueryInsert_Store> store)
            : store{std::move(store)}
        { }

        template<class... T>
        TmpQueryInsert_JOIN& join(T&&... t) {
            store->on.emplace_back(std::forward<T>(t)...);
            return *this;
        }

        template<class... T>
        TmpQueryInsert_DATA data(T&&... t) {
            auto temp = TmpQueryInsert_DATA(std::move(store));
            temp.data(std::forward<T>(t)...);
            return temp;
        }
    };

    struct TmpQueryInsert_FORMAT {
        std::unique_ptr<QueryInsert_Store> store;

        explicit inline TmpQueryInsert_FORMAT(std::unique_ptr<QueryInsert_Store> store)
            : store{std::move(store)}
        { }

        inline TmpQueryInsert_FORMAT& format(const std::string& column) {
            store->format.emplace_back(column);
            return *this;
        }

        template<class... T>
        inline TmpQueryInsert_FORMAT& format(const std::string& column, T&&... t) {
            store->format.emplace_back(column);
            format(std::forward<T>(t)...);
            return *this;
        }

        template<class... T>
        TmpQueryInsert_JOIN join(T&&... t) {
            auto temp = TmpQueryInsert_JOIN(std::move(store));
            temp.join(std::forward<T>(t)...);
            return temp;
        }

        template<class... T>
        TmpQueryInsert_DATA data(T&&... t) {
            auto temp = TmpQueryInsert_DATA(std::move(store));
            temp.data(std::forward<T>(t)...);
            return temp;
        }
    };

    struct TmpQueryInsert_INTO {
        std::unique_ptr<QueryInsert_Store> store;

        explicit inline TmpQueryInsert_INTO(std::unique_ptr<QueryInsert_Store> store)
            : store{std::move(store)}
        { }

        inline TmpQueryInsert_FORMAT into(const std::string& table) {
            if (!store->into.empty())
                throw std::runtime_error("Table for query was already defined");
            store->into = table;
            auto temp = TmpQueryInsert_FORMAT(std::move(store));
            return temp;
        }
    };

    struct TmpQueryInsert_INSERT {
        std::unique_ptr<QueryInsert_Store> store;

        inline TmpQueryInsert_INSERT()
            : store{cpp11::make_unique<QueryInsert_Store>()}
        { }

        template<class... T>
        TmpQueryInsert_FORMAT into(T&&... t) {
            auto temp = TmpQueryInsert_INTO(std::move(store));
            return temp.into(std::forward<T>(t)...);
        }
    };
}

#endif
