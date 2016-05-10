#ifndef IUSEREVENT_H
#define IUSEREVENT_H

#include <cstddef>
#include "IEvent.hpp"


class IUserEvent : public IEvent {
public:
	virtual size_t getUserId() const = 0;
};

#endif
