#ifndef IDPROVIDER_H
#define IDPROVIDER_H

#include "utils/Ini.hpp"

#include <mutex>


class IdProvider {
    Ini idIni;
    Ini::Entries& idMap;
    bool modified;
    std::mutex idMutex;
    IdProvider();
public:
    static IdProvider& getInstance();
    size_t generateNewId(const std::string& key);
    size_t getLastId(const std::string& key);
    void save();
};

#endif
