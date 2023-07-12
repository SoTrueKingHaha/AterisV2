#include "c_basecombatweapon.h"

const char* C_BaseCombatWeapon::GetName()
{
	static auto FN = reinterpret_cast<const char* (__thiscall*)(void*)>( memory->get_vfunc(this, 329));
	return FN(this);
}

bool C_BaseCombatWeapon::GetAttachment(int number, vector& origin)
{
	static auto FN = reinterpret_cast<bool(__thiscall*)(void*, int, vector&)>( memory->get_vfunc(this, 71));
	return FN(this, number, origin);
}
