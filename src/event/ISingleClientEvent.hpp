#ifndef ISINGLECLIENTEVENT_H
#define ISINGLECLIENTEVENT_H

#include <cstddef>
#include "IClientEvent.hpp"


class ISingleClientEvent : public IClientEvent {
public:
	virtual void* getData() const = 0;
};

#endif
