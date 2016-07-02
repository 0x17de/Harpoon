#ifndef MODULEPROVIDER_H
#define MODULEPROVIDER_H

#include <memory>
#include <map>
#include <functional>

#define PROVIDE_MODULE(category, name, className)                          \
    namespace ProvideModule {                                              \
    static class ProvideModule_##className {                               \
    public:                                                                \
        ProvideModule_##className() {                                      \
            auto&& func = [](EventQueue* appQueue)->shared_ptr<EventLoop>{ \
                return make_shared<className>(appQueue);                   \
        };                                                                 \
        ModuleProvider::getInstance().registerModule(category,             \
                                                     name,                 \
                                                     func);                \
        }                                                                  \
    } provideModule_##className;                                           \
    }

class EventQueue;
class EventLoop;
class ModuleProvider {
    typedef std::shared_ptr<EventLoop> LoopPtr;
    typedef std::map<std::string, std::function<LoopPtr(EventQueue*)>> InitializerMap;
    std::map<std::string, InitializerMap> initializerByCategory;

    ModuleProvider();
public:
    static ModuleProvider& getInstance();
    LoopPtr initializeModule(const std::string& category,
                             const std::string& name,
                             EventQueue* appQueue) const;
    void registerModule(const std::string& category,
                        const std::string& name,
                        std::function<LoopPtr(EventQueue*)> initializer);
};

#endif
