#pragma once

#include "GameEventListener.h"

class CGameEventManager
{
public:
	virtual				~CGameEventManager() {}
	virtual int			LoadEventsFromFile(const char* szFileName) = 0;
	virtual void		Reset() = 0;
	virtual bool		AddListener( IGameEventListener2* pListener, const char* szName, bool bServerSide) = 0;
	virtual bool		FindListener( IGameEventListener2* pListener, const char* szName) = 0;
	virtual void		RemoveListener( IGameEventListener2* pListener) = 0;
	virtual void		AddListenerGlobal( IGameEventListener2* pListener, bool bServerSide) = 0;
	// virtual IGameEvent* CreateNewEvent(const char* szName, bool bForce = false, int* unknown = nullptr) = 0;
	IGameEvent* CreateNewEvent(const char* szName, bool bForce = false);
	virtual bool		FireEvent(IGameEvent* pEvent, bool bDontBroadcast = false) = 0;
	virtual bool		FireEventClientSide( IGameEvent* pEvent) = 0;
	virtual IGameEvent* DuplicateEvent( IGameEvent* pEvent) = 0;
	virtual void		FreeEvent( IGameEvent* pEvent) = 0;
	virtual bool		SerializeEvent( IGameEvent* pEvent, CSVCMsg_GameEvent* pEventMsg) = 0;
	virtual IGameEvent* UnserializeEvent(const CSVCMsg_GameEvent& eventMsg) = 0;
	virtual void* GetEventDataTypes( IGameEvent* pEvent) = 0;
};

#define GAMEEVENTSMANAGER_ENGINE_INTERFACE "GAMEEVENTSMANAGER002"