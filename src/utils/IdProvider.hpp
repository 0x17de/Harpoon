#ifndef IDPROVIDER_H
#define IDPROVIDER_H

#include "utils/Ini.hpp"

#include <mutex>


/// Used for generating unique ids for messages
/// All last ids are stored inside '${PWD}/config/ids.ini'
class IdProvider {
    Ini idIni;
    Ini::Entries& idMap;
    bool modified;
    std::mutex idMutex;
    IdProvider();
public:
    /// Returns a singleton instance for generating ids
    static IdProvider& getInstance();
    /// Returns a new unique id for the key
    size_t generateNewId(const std::string& key);
    /// Returns the last generated id
    size_t getLastId(const std::string& key);
    /// Sets the last generated id, known from a previous session
    void setLowestId(const std::string& key, size_t value);
    /// Saves all ids to a file
    void save();
};

#endif
