#ifndef ISINGLECLIENTEVENT_H
#define ISINGLECLIENTEVENT_H

#include <cstddef>
#include "IUserEvent.hpp"


class ISingleClientEvent : public IUserEvent {
public:
	virtual void* getData() const = 0;
};

#endif
