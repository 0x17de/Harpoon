#ifndef IEVENT_H
#define IEVENT_H

#include "utils/uuid.hpp"
#include <chrono>


class IEvent {
    std::chrono::time_point<std::chrono::system_clock> timestamp;
protected:
    IEvent();
public:
    virtual UUID getEventUuid() const = 0;
    decltype(timestamp) getTimestamp() const;
    template <class T>
    T* as() { return dynamic_cast<T*>(this); }
};

#endif
