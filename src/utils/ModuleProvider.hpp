#ifndef MODULEPROVIDER_H
#define MODULEPROVIDER_H

#include <memory>
#include <map>
#include <functional>

#define PROVIDE_MODULE(category, name, className)                   \
    namespace { ProvideModule<className> provider(category, name); }


class EventQueue;
class EventLoop;
class ModuleProvider {
    using InitializerResult = std::shared_ptr<EventLoop>;
    using InitializerCallback = std::function<InitializerResult(EventQueue*)>;
    using InitializerMap = std::map<std::string, InitializerCallback>;
    std::map<std::string, InitializerMap> initializerByCategory;

    ModuleProvider();
public:
    static ModuleProvider& getInstance();
    InitializerResult initializeModule(const std::string& category,
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

template<class Target>
class ProvideModule {
public:
    ProvideModule(const std::string& category,
                  const std::string& name) {
        auto&& func = [](EventQueue* appQueue)->std::shared_ptr<EventLoop>{
            return std::make_shared<Target>(appQueue);
        };
        ModuleProvider::getInstance().registerModule(category,
                                                     name,
                                                     func);
    }
};

#endif
