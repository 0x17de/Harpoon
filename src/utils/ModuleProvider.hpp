#ifndef MODULEPROVIDER_H
#define MODULEPROVIDER_H

#include <memory>
#include <map>
#include <functional>

#define PROVIDE_MODULE(category, name, className)                       \
    static class ProvideModule_##className {                            \
    public:                                                             \
    ProvideModule_##className() {                                       \
            auto&& func = [](EventQueue* appQueue)->std::shared_ptr<EventLoop>{ \
                return std::make_shared<className>(appQueue);           \
            };                                                          \
            ModuleProvider::getInstance().registerModule(category,      \
                                                         name,          \
                                                         func);         \
        }                                                               \
    } provideModule_##className


class EventQueue;
class EventLoop;
class ModuleProvider {
    using LoopPtr = std::shared_ptr<EventLoop>;
    using LoopInitializer = std::function<LoopPtr(EventQueue*)>;
    using InitializerMap = std::map<std::string, LoopInitializer>;
    using InitializerCallback = std::function<LoopPtr(EventQueue*)>;
    std::map<std::string, InitializerMap> initializerByCategory;

    ModuleProvider();
public:
    static ModuleProvider& getInstance();
    LoopPtr initializeModule(const std::string& category,
                             const std::string& name,
                             EventQueue* appQueue) const;
    void registerModule(const std::string& category,
                        const std::string& name,
                        InitializerCallback initializer);

    template<class Fn>
    void forEachModule(const std::string& category, Fn f) {
        auto it = initializerByCategory.find(category);
        if (it == initializerByCategory.end()) return;
        InitializerMap& map = *it;
        for (auto& p : map) f(p.first, p.second);
    }
};

#endif
