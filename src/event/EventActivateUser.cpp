#include "EventActivateUser.hpp"
#include "utils/uuidGen.hpp"


UUID EventActivateUser::uuid = ::uuid.get();
UUID EventActivateUser::getEventUuid() const {
	return this->uuid;
}

EventActivateUser::EventActivateUser(size_t userId, const std::map<size_t, IrcServerConfiguration>& loginData)
:
	userId{userId},
	loginData{loginData}
{
}

size_t EventActivateUser::getUserId() const {
	return userId;
}

const std::map<size_t, IrcServerConfiguration> EventActivateUser::getLoginConfiguration() const {

}
