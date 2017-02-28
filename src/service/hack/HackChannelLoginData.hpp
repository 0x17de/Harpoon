#ifndef HACKCHANNELLOGINDATA_H
#define HACKCHANNELLOGINDATA_H

#include <string>


class HackChannelLoginData {
    size_t channelId;
    std::string channelName;
    bool disabled;
public:
    HackChannelLoginData(size_t channelId,
                        const std::string& channel,
                        bool disabled);

    size_t getChannelId() const;
    std::string getChannelName() const;
    bool getDisabled() const;
    void setDisabled(bool ldisabled);
};

#endif
