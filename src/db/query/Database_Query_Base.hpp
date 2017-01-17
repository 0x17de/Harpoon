#ifndef DATABASE_QUERY_H
#define DATABASE_QUERY_H

#include <string>
#include <functional>
#include "utils/Cpp11Utils.hpp"


namespace Query {
    struct Statement;
    using StatementPtr = std::unique_ptr<Statement>;
    using OrderStatement = std::tuple<std::string, std::string>;
    using JoinStatement = std::tuple<std::string, StatementPtr>;

    enum class Op {
        EQ,
        LT,
        NEQ,
        OR
    };

    struct TraverseCallbacks {
        std::function<void()> onUp;
        std::function<void()> onDown;
        std::function<void(const std::string&)> onVar;
        std::function<void(const std::string&)> onConstant;
        std::function<void(Op)> onOp;
    };

    struct Statement {
        inline Statement()
        { }

        virtual inline void traverse(const TraverseCallbacks& cb) {};
    };

    struct Expression : public Statement {
        Op operation;
        StatementPtr left, right;

        inline Expression()
        {};
        inline Expression(StatementPtr&& left, StatementPtr&& right, Op operation)
            : operation{operation}
            , left{std::move(left)}
            , right{std::move(right)}
        { }

        virtual inline void traverse(const TraverseCallbacks& cb) {
            cb.onUp();
            left->traverse(cb);
            cb.onOp(operation);
            right->traverse(cb);
            cb.onDown();
        }
    };
    template<class L, class R>
    using OpResult = typename std::enable_if<
        std::is_base_of<Statement, L>::value
        &&
        std::is_base_of<Statement, R>::value
        , StatementPtr>::type;

    struct Constant : public Statement {
        std::string name;
        inline Constant()
        { }
        inline Constant(const std::string& name)
            : name{name}
        { }

        virtual inline void traverse(const TraverseCallbacks& cb) {
            cb.onConstant(name);
        }
    };
    struct Var : public Statement {
        std::string name;
        Var()
        { }
        Var(const std::string& name)
            : name{name}
        { }

        virtual inline void traverse(const TraverseCallbacks& cb) {
            cb.onVar(name);
        }
    };

    template<class T>
    StatementPtr make_var(T&& t) {
        return cpp11::make_unique<Var>(std::forward<T>(t));
    }
    template<class T>
    StatementPtr make_constant(T&& t) {
        return cpp11::make_unique<Constant>(std::forward<T>(t));
    }


    StatementPtr operator==(StatementPtr&& left, StatementPtr&& right) {
        return cpp11::make_unique<Expression>(std::move(left), std::move(right), Op::EQ);
    }
    StatementPtr operator<(StatementPtr&& left, StatementPtr&& right) {
        return cpp11::make_unique<Expression>(std::move(left), std::move(right), Op::LT);
    }
    StatementPtr operator||(StatementPtr&& left, StatementPtr&& right) {
        return cpp11::make_unique<Expression>(std::move(left), std::move(right), Op::OR);
    }
}

#endif
