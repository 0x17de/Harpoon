#include "IrcLoggable.hpp"
#include "utils/IdProvider.hpp"

#include <iostream>

using namespace std;


IrcLoggable::IrcLoggable()
    : logEntryId{IdProvider::getInstance().generateNewId("irc_log")}
{
}

size_t IrcLoggable::getLogEntryId()
{
    return logEntryId;
}
