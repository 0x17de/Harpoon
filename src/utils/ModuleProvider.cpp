#include "queue/EventLoop.hpp"
#include "ModuleProvider.hpp"
#include <stdexcept>
#include <sstream>
#include <iostream>

using namespace std;


ModuleProvider::ModuleProvider() {
}

ModuleProvider& ModuleProvider::getInstance() {
    static ModuleProvider moduleProvider;
    return moduleProvider;
}

ModuleProvider::LoopPtr ModuleProvider::initializeModule(const std::string& category,
                                                         const std::string& name,
                                                         EventQueue* appQueue) const {
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

    return initializerIt->second(appQueue);
}

void ModuleProvider::registerModule(const std::string& category,
                                    const std::string& name,
                                    std::function<LoopPtr(EventQueue*)> initializer) {
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
