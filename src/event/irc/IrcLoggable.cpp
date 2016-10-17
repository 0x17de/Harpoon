#include "IrcLoggable.hpp"
#include "utils/IdProvider.hpp"


IrcLoggable::IrcLoggable()
    : logEntryId{IdProvider::getInstance().generateNewId("irc_log")}
{
}
