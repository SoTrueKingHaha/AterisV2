#include "imovehelper.h"

#include "../../utils/memory/memory.h"

void IMoveHelper::SetHost(C_BaseEntity* host)
{
	static auto FN = reinterpret_cast<void(__thiscall *)(void*, C_BaseEntity*)>( memory->get_vfunc(this, 0));
	FN(this, host);
}