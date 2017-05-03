#ifndef EVENTQUERYTYPE_H
#define EVENTQUERYTYPE_H

enum class EventQueryType
{
    Chats, ///< requests a listing of all active servers and chats
    Settings ///< returns configuration data like server+channel settings
};

#endif
