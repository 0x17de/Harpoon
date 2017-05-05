#ifndef IRCDATABASEMESSAGETYPE_H
#define IRCDATABASEMESSAGETYPE_H

/// Type mapping for each possible irc message/action type
enum class IrcDatabaseMessageType : int {
    Message = 0,
    Join = 1,
    Part = 2,
    Quit = 3,
    Kick = 4,
    Notice = 5,
    Action = 6
};

#endif
