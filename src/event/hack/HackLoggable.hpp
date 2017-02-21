#ifndef HACKLOGGABLE_H
#define HACKLOGGABLE_H


#include <cstddef>
using std::size_t;


class HackLoggable {
    size_t logEntryId;
public:
    HackLoggable();
    size_t getLogEntryId() const;
};

#endif
