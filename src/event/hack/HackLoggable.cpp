#include "HackLoggable.hpp"
#include "utils/IdProvider.hpp"

#include <iostream>

using namespace std;


HackLoggable::HackLoggable()
    : logEntryId{IdProvider::getInstance().generateNewId("irc_log")}
{
}

size_t HackLoggable::getLogEntryId() const
{
    return logEntryId;
}
