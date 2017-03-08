#ifndef MODULEPROVIDER_H
#define MODULEPROVIDER_H

#include <memory>
#include <map>
#include <functional>

#define PROVIDE_WEBSOCKET_PROTOCOL_MODULE(category, name, className)    \
    static class ProvideModule_##className {                            \
    public:                                                             \
        ProvideModule_##className() {                                       \
            auto&& func = [](EventQueue* appQueue)->std::shared_ptr<WebsocketProtocolHandler>{ \
                return std::make_shared<className>(appQueue);           \
            };                                                          \
            WebsocketProtocolHandlerProvider::getInstance().registerModule(category, \
                                                                           name, \
                                                                           func); \
        }                                                               \
    } provideModule_##className


class EventQueue;
class WebsocketProtocolHandler;
class WebsocketProtocolHandlerProvider {
    using InitializerResult = std::shared_ptr<WebsocketProtocolHandler>;
    using InitializerCallback = std::function<InitializerResult()>;
    using InitializerMap = std::map<std::string, InitializerCallback>;
    std::map<std::string, InitializerMap> initializerByCategory;

    WebsocketProtocolHandlerProvider();
public:
    static WebsocketProtocolHandlerProvider& getInstance();
    InitializerResult initializeModule(const std::string& category,
                             const std::string& name) const;
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
