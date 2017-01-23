#ifndef DATABASE_DETAILS_DELETE_H
#define DATABASE_DETAILS_DELETE_H

#include <memory>
#include <utility>
#include <exception>

#include "Database_QueryDelete_Store.hpp"


namespace details {
    using namespace Query;

    struct TmpQueryDelete_LIMIT {
        std::unique_ptr<QueryDelete_Store> store;

        explicit inline TmpQueryDelete_LIMIT(std::unique_ptr<QueryDelete_Store> store)
            : store{std::move(store)}
        { }

        inline operator Query::Delete() {
            return std::move(store);
        }

        inline TmpQueryDelete_LIMIT& limit(size_t count) {
            store->limit = count;
            return *this;
        }
    };

    struct TmpQueryDelete_FILTER {
        std::unique_ptr<QueryDelete_Store> store;

        explicit inline TmpQueryDelete_FILTER(std::unique_ptr<QueryDelete_Store> store)
            : store{std::move(store)}
        { }

        inline operator Query::Delete() {
            return std::move(store);
        }

        inline TmpQueryDelete_FILTER& where(StatementPtr&& filter) {
            if (store->filter)
                throw std::runtime_error("Query already contains filter");
            store->filter = std::move(filter);
            return *this;
        }

        template<class... T>
        TmpQueryDelete_LIMIT limit(T&&... t) {
            auto temp = TmpQueryDelete_LIMIT(std::move(store));
            temp.limit(std::forward<T>(t)...);
            return temp;
        }
    };

    struct TmpQueryDelete_JOIN {
        std::unique_ptr<QueryDelete_Store> store;

        explicit inline TmpQueryDelete_JOIN(std::unique_ptr<QueryDelete_Store> store)
            : store{std::move(store)}
        { }

        inline operator Query::Delete() {
            return std::move(store);
        }

        template<class... T>
        TmpQueryDelete_JOIN& join(T&&... t) {
            store->on.emplace_back(std::forward<T>(t)...);
            return *this;
        }

        template<class... T>
        TmpQueryDelete_FILTER where(T&&... t) {
            auto temp = TmpQueryDelete_FILTER(std::move(store));
            temp.where(std::forward<T>(t)...);
            return temp;
        }

        template<class... T>
        TmpQueryDelete_LIMIT limit(T&&... t) {
            auto temp = TmpQueryDelete_LIMIT(std::move(store));
            temp.limit(std::forward<T>(t)...);
            return temp;
        }
    };

    struct TmpQueryDelete_FROM {
        std::unique_ptr<QueryDelete_Store> store;

        explicit inline TmpQueryDelete_FROM(std::unique_ptr<QueryDelete_Store> store)
            : store{std::move(store)}
        { }

        inline TmpQueryDelete_FROM& from(const std::string& table) {
            if (!store->from.empty())
                throw std::runtime_error("Table for query was already defined");
            store->from = table;
            return *this;
        }

        inline operator Query::Delete() {
            return std::move(store);
        }

        template<class...T>
        TmpQueryDelete_JOIN join(T&&... t) {
            auto temp = TmpQueryDelete_JOIN(std::move(store));
            temp.join(std::forward<T>(t)...);
            return temp;
        }

        template<class... T>
        TmpQueryDelete_FILTER where(T&&... t) {
            auto temp = TmpQueryDelete_FILTER(std::move(store));
            temp.where(std::forward<T>(t)...);
            return temp;
        }

        template<class... T>
        TmpQueryDelete_LIMIT limit(T&&... t) {
            auto temp = TmpQueryDelete_LIMIT(std::move(store));
            temp.limit(std::forward<T>(t)...);
            return temp;
        }
    };

    struct TmpQueryDelete_DELETE {
        std::unique_ptr<QueryDelete_Store> store;

        inline TmpQueryDelete_DELETE()
            : store{cpp11::make_unique<QueryDelete_Store>()}
        { }

        template<class... T>
        TmpQueryDelete_FROM from(T&&... t) {
            auto temp = TmpQueryDelete_FROM(std::move(store));
            temp.from(std::forward<T>(t)...);
            return temp;
        }
    };
}

#endif
