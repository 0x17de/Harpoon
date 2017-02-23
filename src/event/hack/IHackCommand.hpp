#ifndef IHACKCOMMAND_H
#define IHACKCOMMAND_H

#include "../IUserEvent.hpp"


class IHackCommand : public IUserEvent {
public:
    virtual size_t getServerId() const = 0;
};

#endif
