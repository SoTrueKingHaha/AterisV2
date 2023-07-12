#pragma once

#include "igameevents.h"

class CGameEventListener : public IGameEventListener2
{
public:
	virtual void FireGameEvent(CGameEvent* event) = 0;
};

class CGameEventListenerOverrideable
{
public:
	virtual ~CGameEventListenerOverrideable(void) {};

	virtual void FireGameEvent(CGameEvent* pEvent) = 0;
	virtual int GetEventDebugID() { return m_nDebug; }

private:
	int m_nDebug;
};