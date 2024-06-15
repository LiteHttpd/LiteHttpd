#include "EventBase.h"

EventBase::EventBase() {
	/** Init Event Base */
	this->base = event_base_new();
}

EventBase::~EventBase() {
	event_base_free(this->base);
}

event_base* EventBase::getBase() const {
	return this->base;
}

int EventBase::runEventLoop() {
	return event_base_dispatch(this->base);
}

EventBase* EventBase::getInstance() {
	return EventBase::instance ? EventBase::instance
		: (EventBase::instance = new EventBase{});
}

void EventBase::releaseInstance() {
	if (EventBase::instance) {
		delete EventBase::instance;
		EventBase::instance = nullptr;
	}
}

EventBase* EventBase::instance = nullptr;
