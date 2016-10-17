#ifndef IRCLOGGABLE_H
#define IRCLOGGABLE_H


#include <cstddef>
using std::size_t;


class IrcLoggable {
    size_t logEntryId;
public:
    IrcLoggable();
    size_t getLogEntryId();
};

#endif
