#ifndef WEBSOCKETPROTOCOLHANDLER_H
#define WEBSOCKETPROTOCOLHANDLER_H

#include <memory>


class WebsocketProtocolHandler {
    virtual void onEvent(std::shared_ptr<IEvent> event) = 0;
};

#endif
