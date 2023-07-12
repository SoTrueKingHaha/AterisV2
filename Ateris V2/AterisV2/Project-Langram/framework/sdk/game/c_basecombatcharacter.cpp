#include "C_BaseCombatCharacter.h"

#include "../Interfaces/Interfaces.h"

inline bool C_BaseCombatCharacter::CanAttack()
{
	return (i::global_vars->curtime >= m_flNextAttack());
}

int C_BaseCombatCharacter::WeaponCount() const
{
	return MAX_WEAPONS;
}

C_BaseCombatWeapon* C_BaseCombatCharacter::GetWeapon(const int i)
{
	assert((i >= 0) && (i < MAX_WEAPONS));
	return m_hMyWeapons()[i].Get();
}

std::array<CHandle<C_BaseCombatWeapon>, MAX_WEAPONS>& C_BaseCombatCharacter::m_hMyWeapons()
{
	static const int nOffset = netvars.find_netvar("CBaseCombatCharacter", "m_hMyWeapons");
	return *reinterpret_cast<std::array<CHandle<C_BaseCombatWeapon>, MAX_WEAPONS>*>(reinterpret_cast<DWORD>(this) + nOffset);
}
