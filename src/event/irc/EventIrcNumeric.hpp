#ifndef EVENTIRCNUMERIC_H
#define EVENTIRCNUMERIC_H

#include "../IClientEvent.hpp"
#include <string>
#include <vector>


class EventIrcNumeric : public IClientEvent {
    size_t userId;
    size_t serverId;
    unsigned int eventCode;
    std::string from;
    std::vector<std::string> parameters;
public:
    static constexpr UUID uuid = 32;
    virtual UUID getEventUuid() const override;

    EventIrcNumeric(size_t userId, size_t serverId,
                    unsigned int eventCode,
                    const std::string& from,
                    const std::vector<std::string>& parameters);
    virtual size_t getUserId() const override;
    size_t getServerId() const;
    unsigned int getEventCode() const;
    std::string getFrom() const;
    const std::vector<std::string>& getParameters() const;
};

#endif
