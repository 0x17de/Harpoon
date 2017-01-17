#ifndef DATABASE_DETAILS_SELECT_H
#define DATABASE_DETAILS_SELECT_H

#include <memory>
#include <utility>
#include <exception>

#include "Database_QuerySelect_Store.hpp"


namespace details {
    using namespace Query;

    struct TmpQuerySelect_ORDER {
        std::unique_ptr<QuerySelect_Store> store;

        explicit inline TmpQuerySelect_ORDER(std::unique_ptr<QuerySelect_Store> store)
            : store{std::move(store)}
        { }

        inline operator Query::Select() {
            return std::move(store);
        }

        inline TmpQuerySelect_ORDER& order_by(const std::string& by, const std::string& dir) {
            store->order.emplace_back(by, dir);
            return *this;
        }
    };

    struct TmpQuerySelect_FILTER {
        std::unique_ptr<QuerySelect_Store> store;

        explicit inline TmpQuerySelect_FILTER(std::unique_ptr<QuerySelect_Store> store)
            : store{std::move(store)}
        { }

        inline operator Query::Select() {
            return std::move(store);
        }

        inline TmpQuerySelect_FILTER& where(StatementPtr&& filter) {
            if (store->filter)
                throw std::runtime_error("Query already contains filter");
            store->filter = std::move(filter);
            return *this;
        }

        template<class... T>
        TmpQuerySelect_ORDER order_by(T&&... t) {
            auto temp = TmpQuerySelect_ORDER(std::move(store));
            temp.order_by(std::forward<T>(t)...);
            return temp;
        }
    };

    struct TmpQuerySelect_JOIN {
        std::unique_ptr<QuerySelect_Store> store;

        explicit inline TmpQuerySelect_JOIN(std::unique_ptr<QuerySelect_Store> store)
            : store{std::move(store)}
        { }

        inline operator Query::Select() {
            return std::move(store);
        }

        template<class... T>
        TmpQuerySelect_JOIN& join(T&&... t) {
            store->on.emplace_back(std::forward<T>(t)...);
            return *this;
        }

        template<class... T>
        TmpQuerySelect_FILTER where(T&&... t) {
            auto temp = TmpQuerySelect_FILTER(std::move(store));
            temp.where(std::forward<T>(t)...);
            return temp;
        }

        template<class... T>
        TmpQuerySelect_ORDER order_by(T&&... t) {
            auto temp = TmpQuerySelect_ORDER(std::move(store));
            temp.order_by(std::forward<T>(t)...);
            return temp;
        }
    };

    struct TmpQuerySelect_FROM {
        std::unique_ptr<QuerySelect_Store> store;

        explicit inline TmpQuerySelect_FROM(std::unique_ptr<QuerySelect_Store> store)
            : store{std::move(store)}
        { }

        inline TmpQuerySelect_FROM& from(const std::string& table) {
            if (!store->from.empty())
                throw std::runtime_error("Table for query was already defined");
            store->from = table;
            return *this;
        }

        inline operator Query::Select() {
            return std::move(store);
        }

        template<class...T>
        TmpQuerySelect_JOIN join(T&&... t) {
            auto temp = TmpQuerySelect_JOIN(std::move(store));
            temp.join(std::forward<T>(t)...);
            return temp;
        }

        template<class... T>
        TmpQuerySelect_FILTER where(T&&... t) {
            auto temp = TmpQuerySelect_FILTER(std::move(store));
            temp.where(std::forward<T>(t)...);
            return temp;
        }

        template<class... T>
        TmpQuerySelect_ORDER order_by(T&&... t) {
            auto temp = TmpQuerySelect_ORDER(std::move(store));
            temp.order_by(std::forward<T>(t)...);
            return temp;
        }
    };

    struct TmpQuerySelect_SELECT {
        std::unique_ptr<QuerySelect_Store> store;

        inline TmpQuerySelect_SELECT()
            : store{cpp11::make_unique<QuerySelect_Store>()}
        { }

        inline TmpQuerySelect_SELECT& select(const std::string& str) {
            store->what.emplace_back(str);
            return *this;
        }

        template<class... T>
        TmpQuerySelect_SELECT& select(const std::string& str, T... t) {
            return select(str).select(std::forward<T>(t)...);
        }

        template<class... T>
        TmpQuerySelect_FROM from(T&&... t) {
            auto temp = TmpQuerySelect_FROM(std::move(store));
            temp.from(std::forward<T>(t)...);
            return temp;
        }
    };
}

#endif
