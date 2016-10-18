#include "EventIrcServiceInit.hpp"
#include "utils/uuidGen.hpp"


UUID EventIrcServiceInit::uuid = ::uuid.get();
UUID EventIrcServiceInit::getEventUuid() const {
    return this->uuid;
}
