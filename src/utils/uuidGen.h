#ifndef UUIDGEN_H
#define UUIDGEN_H

#include "uuid.h"


static struct {
  inline constexpr UUID get() const {
    return (UUID)this;
  }
} uuid;

#endif
