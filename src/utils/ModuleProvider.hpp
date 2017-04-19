#ifndef MODULEPROVIDER_H
#define MODULEPROVIDER_H

#include <memory>
#include <map>
#include <sstream>
#include <string>
#include <functional>

/// Registers a basic module for the specified 'target' - e.g. EventLoop
#define PROVIDE_MODULE(target, category, name, className)               \
    namespace { ProvideModule<target, className> provider(category, name); }
/// PROVIDE_MODULE: Simplified for EventLoop
#define PROVIDE_EVENTLOOP_MODULE(category, name, className)             \
    namespace { ProvideModule<EventLoop, className> provider(category, name); }
/// PROVIDE_MODULE: Simplified for WebsocketProtocolHandler
#define PROVIDE_WEBSOCKETPROTOCOL_MODULE(category, name, className)             \
    namespace { ProvideModule<WebsocketProtocolHandler, className> provider(category, name); }


class WebsocketProtocolHandler;
class EventQueue;
class EventLoop;


/// Template for all module types
/// The init function can be different for each module
template<class Target>
struct ModuleInitializer { };

/// Init template specialized for the EventLoop type
template<>
struct ModuleInitializer<EventLoop> {
    using return_type = std::shared_ptr<EventLoop>;
    using callback_type = return_type(*)(EventQueue*);

    template<class ModuleClass>
    static std::shared_ptr<EventLoop> init(EventQueue* appQueue) {
        return std::make_shared<ModuleClass>(appQueue);
    }
};

/// Init template specialized for the WebsocketProtocolHandler type
template<>
struct ModuleInitializer<WebsocketProtocolHandler> {
    using return_type = std::shared_ptr<WebsocketProtocolHandler>;
    using callback_type = return_type(*)();

    template<class ModuleClass>
    static std::shared_ptr<WebsocketProtocolHandler> init() {
        return std::make_shared<ModuleClass>();
    }
};


/// Modules register at the module provider.
/// Others can use the module provider to initialize instances
/// of the corresponding type. See ModuleInitializer.
/// Macros are used to register at the ModuleProvider.
template<class Target>
class ModuleProvider {
    using callback_type = typename ModuleInitializer<Target>::callback_type;
    using initializer_map_type = std::map<std::string, callback_type>;
    std::map<std::string, initializer_map_type> initializerByCategory;

    ModuleProvider() = default;
public:
    /// Get the static instance of a module provider
    static ModuleProvider& getInstance() {
        static ModuleProvider moduleProvider;
        return moduleProvider;
    }

    /// Returns the function for initialization of some module
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

    /// Registers a module with its callback
    ///
    /// \param category Multiple modules can provide the same functionality. Example: Database
    /// \param name The module's name
    /// \param initializer Callback to initialize some instance of the module
    void registerModule(const std::string& category,
                        const std::string& name,
                        const typename ModuleInitializer<Target>::callback_type& initializer) {
        initializer_map_type& initializerMap = initializerByCategory[category];
        initializerMap.emplace(name, initializer);
    }

    /// Helper function to iterate through all existing modules
    template<class Fn>
    void forEachModule(const std::string& category, Fn f) {
        auto it = initializerByCategory.find(category);
        if (it == initializerByCategory.end()) return;
        initializer_map_type& map = *it;
        for (auto& p : map) f(p.first, p.second);
    }
};

/// Helper template class to globally register modules on application start
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
