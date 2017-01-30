#ifndef DATABASE_DETAILS_UPDATE_H
#define DATABASE_DETAILS_UPDATE_H

#include <memory>
#include <utility>
#include <exception>

#include "Database_QueryUpdate_Store.hpp"


namespace details {
    using namespace Query;

    struct TmpQueryUpdate_DATA {
        std::unique_ptr<QueryUpdate_Store> store;

        explicit inline TmpQueryUpdate_DATA(std::unique_ptr<QueryUpdate_Store> store)
            : store{std::move(store)}
        { }

        inline operator Query::Update() {
            return std::move(store);
        }

        TmpQueryUpdate_DATA& data(std::vector<std::string>&& data)
        {
            if (store->data.size() == 0) {
                store->data.swap(data);
            } else {
                store->data.reserve(store->data.size()+std::distance(data.cbegin(), data.cend()));
                store->data.insert(store->data.end(), data.cbegin(), data.cend());
            }
            return *this;
        }

        TmpQueryUpdate_DATA& data(std::vector<std::string>::iterator start,
                                  std::vector<std::string>::iterator end)
        {
            store->data.reserve(store->data.size()+std::distance(start, end));
            store->data.insert(store->data.end(), start, end);
            return *this;
        }
    };

    struct TmpQueryUpdate_FORMAT {
        std::unique_ptr<QueryUpdate_Store> store;

        explicit inline TmpQueryUpdate_FORMAT(std::unique_ptr<QueryUpdate_Store> store)
            : store{std::move(store)}
        { }

        inline TmpQueryUpdate_FORMAT& format(const std::string& column) {
            store->format.emplace_back(column);
            return *this;
        }

        template<class... T>
        inline TmpQueryUpdate_FORMAT& format(const std::string& column, T&&... t) {
            store->format.emplace_back(column);
            format(std::forward<T>(t)...);
            return *this;
        }

        template<class... T>
        TmpQueryUpdate_DATA data(T&&... t) {
            auto temp = TmpQueryUpdate_DATA(std::move(store));
            temp.data(std::forward<T>(t)...);
            return temp;
        }
    };

    struct TmpQueryUpdate_INTO {
        std::unique_ptr<QueryUpdate_Store> store;

        explicit inline TmpQueryUpdate_INTO(std::unique_ptr<QueryUpdate_Store> store)
            : store{std::move(store)}
        { }

        inline TmpQueryUpdate_FORMAT into(const std::string& table) {
            if (!store->table.empty())
                throw std::runtime_error("Table for query was already defined");
            store->table = table;
            auto temp = TmpQueryUpdate_FORMAT(std::move(store));
            return temp;
        }
    };

    struct TmpQueryUpdate_UPDATE {
        std::unique_ptr<QueryUpdate_Store> store;

        inline TmpQueryUpdate_UPDATE()
            : store{cpp11::make_unique<QueryUpdate_Store>()}
        { }

        template<class... T>
        TmpQueryUpdate_FORMAT into(T&&... t) {
            auto temp = TmpQueryUpdate_INTO(std::move(store));
            return temp.into(std::forward<T>(t)...);
        }
    };
}

#endif
