#pragma once

#include <array>

#include "C_BaseFlex.h"

class C_BaseCombatWeapon;
class C_WeaponCombatShield;

#define BCC_DEFAULT_LOOK_TOWARDS_TOLERANCE 0.9f

class C_BaseCombatCharacter : public C_BaseFlex
{
public:
	enum LineOfSightCheckType
	{
		IGNORE_NOTHING,
		IGNORE_ACTORS
	};

public:
	int	WeaponCount() const;
	C_BaseCombatWeapon* GetWeapon(const int i);

	NETVAR(m_flNextAttack, float, "CBaseCombatCharacter", "m_flNextAttack");
	NETVAR(m_hActiveWeapon, CHandle<C_BaseCombatWeapon>, "CBaseCombatCharacter", "m_hActiveWeapon");
	//NETVAR_OFFSET_PURE(m_hActiveWeapon, CHandle<C_BaseCombatWeapon>, 3512);
	NETVAR(m_bGlowEnabled, bool, "CBaseCombatCharacter", "m_bGlowEnabled");

	std::array<CHandle<C_BaseCombatWeapon>, MAX_WEAPONS>& m_hMyWeapons();
	bool CanAttack();

};


