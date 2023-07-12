#include "CBaseHandle.h"

#include "../Interfaces/Interfaces.h"

IHandleEntity* CBaseHandle::Get() const
{
	return reinterpret_cast<IHandleEntity*>(i::entity_list->GetClientEntityFromHandle(*this));
}

