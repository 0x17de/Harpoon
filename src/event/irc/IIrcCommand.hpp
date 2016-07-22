#ifndef IIRCCOMMAND_H
#define IIRCCOMMAND_H

#include "../IUserEvent.hpp"


class IIrcCommand : public IUserEvent {
public:
    virtual size_t getServerId() const = 0;
};

#endif
