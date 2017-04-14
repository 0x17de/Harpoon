#ifndef CPP11UTILS_H
#define CPP11UTILS_H

#include <memory>


namespace cpp11 {
    /// Template for make_unique which is introduced with C++14
    template<class R, class... T>
    std::unique_ptr<R> make_unique(T&&... t) {
        return std::unique_ptr<R>(new R(std::forward<T>(t)...));
    }
}

#endif
