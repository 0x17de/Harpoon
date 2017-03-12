#ifndef MODULEPROVIDER_H
#define MODULEPROVIDER_H

#include <memory>
#include <map>
#include <sstream>
#include <string>
#include <functional>

#define PROVIDE_MODULE(target, category, name, className)               \
    namespace { ProvideModule<target, className> provider(category, name); }
#define PROVIDE_EVENTLOOP_MODULE(category, name, className)             \
    namespace { ProvideModule<EventLoop, className> provider(category, name); }
#define PROVIDE_WEBSOCKETPROTOCOL_MODULE(category, name, className)             \
    namespace { ProvideModule<WebsocketProtocolHandler, className> provider(category, name); }


class WebsocketProtocolHandler;
class EventQueue;
class EventLoop;


template<class Target>
struct ModuleInitializer { };

template<>
struct ModuleInitializer<EventLoop> {
    using return_type = std::shared_ptr<EventLoop>;
    using callback_type = return_type(*)(EventQueue*);

    template<class ModuleClass>
    static std::shared_ptr<EventLoop> init(EventQueue* appQueue) {
        return std::make_shared<ModuleClass>(appQueue);
    }
};

template<>
struct ModuleInitializer<WebsocketProtocolHandler> {
    using return_type = std::shared_ptr<WebsocketProtocolHandler>;
    using callback_type = return_type(*)();

    template<class ModuleClass>
    static std::shared_ptr<WebsocketProtocolHandler> init() {
        return std::make_shared<ModuleClass>();
    }
};



template<class Target>
class ModuleProvider {
    using initializer_result = typename ModuleInitializer<Target>::return_type;
    using initializer_map_type = std::map<std::string, typename ModuleInitializer<Target>::callback_type>;
    std::map<std::string, initializer_map_type> initializerByCategory;

    ModuleProvider() = default;
public:
    static ModuleProvider& getInstance() {
        static ModuleProvider moduleProvider;
        return moduleProvider;
    }

    typename ModuleInitializer<Target>::callback_type getInitializerFunction(const std::string& category,
                                        const std::string& name) const {
        auto categoryIt = initializerByCategory.find(category);
        if (categoryIt == initializerByCategory.end())
            throw std::out_of_range("No module registered for category " + category);

        auto& initializerMap = categoryIt->second;
        auto initializerIt = initializerMap.find(name);
        if (initializerIt == initializerMap.end()) {
            std::stringstream ss;
            for (auto modulePair : initializerMap)
                ss << " " << modulePair.first;
            throw std::out_of_range("No module named " + name + " found for category " + category + " (Available:" + ss.str() + ")");
        }

        return initializerIt->second;
    }

    void registerModule(const std::string& category,
                        const std::string& name,
                        typename ModuleInitializer<Target>::callback_type initializer) {
        auto categoryIt = initializerByCategory.find(category);
        initializer_map_type* initializerMap;
        if (categoryIt == initializerByCategory.end()) {
            auto resIt = initializerByCategory.emplace(std::piecewise_construct,
                                                       std::forward_as_tuple(category),
                                                       std::forward_as_tuple());
            initializerMap = &resIt.first->second;
        } else {
            initializerMap = &categoryIt->second;
        }
        initializerMap->emplace(name, initializer);
    }

    template<class Fn>
    void forEachModule(const std::string& category, Fn f) {
        auto it = initializerByCategory.find(category);
        if (it == initializerByCategory.end()) return;
        initializer_map_type& map = *it;
        for (auto& p : map) f(p.first, p.second);
    }
};

template<class Target, class ModuleClass>
class ProvideModule {
public:
    ProvideModule(const std::string& category,
                  const std::string& name) {
        ModuleProvider<Target>::getInstance().registerModule(category,
                                                             name,
                                                             &ModuleInitializer<Target>::template init<ModuleClass>);
    }
};

#endif
