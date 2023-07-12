#include "gameeventmanager.h"

#include "../../utils/memory/memory.h"

IGameEvent* CGameEventManager::CreateNewEvent(const char* szName, bool bForce)
{
	return reinterpret_cast<IGameEvent* (__thiscall*)(void*, const char*, bool)>(memory->get_vfunc(this, 6))(this, szName, bForce);
}
