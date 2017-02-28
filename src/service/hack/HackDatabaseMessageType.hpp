#ifndef IRCDATABASEMESSAGETYPE_H
#define IRCDATABASEMESSAGETYPE_H

enum class IrcDatabaseMessageType : int {
    Message = 0,
    Join = 1,
    Part = 2,
    Warning = 3,
    Info = 4
};

#endif
