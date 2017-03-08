#include "queue/EventLoop.hpp"
#include "WebsocketProtocolHandlerProvider.hpp"
#include <stdexcept>
#include <sstream>
#include <iostream>

using namespace std;


WebsocketProtocolHandlerProvider::WebsocketProtocolHandlerProvider() {
}

WebsocketProtocolHandlerProvider& WebsocketProtocolHandlerProvider::getInstance() {
    static WebsocketProtocolHandlerProvider moduleProvider;
    return moduleProvider;
}

WebsocketProtocolHandlerProvider::InitializerResult WebsocketProtocolHandlerProvider::initializeModule(const std::string& category,
                                                                                             const std::string& name) const {
    auto categoryIt = initializerByCategory.find(category);
    if (categoryIt == initializerByCategory.end())
        throw out_of_range(string("No module registered for category ") + category);

    auto& initializerMap = categoryIt->second;
    auto initializerIt = initializerMap.find(name);
    if (initializerIt == initializerMap.end()) {
        stringstream ss;
        for (auto modulePair : initializerMap)
            ss << " " << modulePair.first;
        throw out_of_range(string("No module named ") + name + " found for category " + category + " (Available:" + ss.str() + ")");
    }

    return initializerIt->second();
}

void WebsocketProtocolHandlerProvider::registerModule(const std::string& category,
                                                      const std::string& name,
                                                      InitializerCallback initializer) {
    auto categoryIt = initializerByCategory.find(category);
    InitializerMap* initializerMap;
    if (categoryIt == initializerByCategory.end()) {
        auto resIt = initializerByCategory.emplace(piecewise_construct,
                                                   forward_as_tuple(category),
                                                   forward_as_tuple());
        initializerMap = &resIt.first->second;
    } else {
        initializerMap = &categoryIt->second;
    }
    initializerMap->emplace(name, initializer);
}
