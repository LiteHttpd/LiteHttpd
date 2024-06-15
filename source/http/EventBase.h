#pragma once

#include <event2/event.h>

class EventBase final {
public:
	EventBase();
	~EventBase();

	event_base* getBase() const;
	int runEventLoop();

private:
	event_base* base = nullptr;

public:
	static EventBase* getInstance();
	static void releaseInstance();

private:
	static EventBase* instance;
};
