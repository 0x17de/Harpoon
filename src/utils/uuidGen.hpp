#ifndef UUIDGEN_H
#define UUIDGEN_H

#include "uuid.hpp"


static struct {
    inline constexpr UUID get() const {
        return (UUID)this;
    }
} uuid;

#endif
