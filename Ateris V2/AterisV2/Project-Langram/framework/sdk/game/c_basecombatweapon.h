#pragma once

#include "C_BaseAnimating.h"

struct FireBulletsInfo_t
{
	FireBulletsInfo_t()
	{
		m_iShots = 1;
		m_vecSpread = vector();
		m_flDistance = 8192;
		m_iTracerFreq = 4;
		m_flDamage = 0;
		m_iPlayerDamage = 0;
		m_pAttacker = NULL;
		m_nFlags = 0;
		m_pAdditionalIgnoreEnt = NULL;
		m_flDamageForceScale = 1.0f;
		m_bPrimaryAttack = true;
		m_bUseServerRandomSeed = false;
	}

	FireBulletsInfo_t(int nShots, const vector& vecSrc, const vector& vecDir, const vector& vecSpread, float flDistance, int nAmmoType, bool bPrimaryAttack = true)
	{
		m_iShots = nShots;
		m_vecSrc = vecSrc;
		m_vecDirShooting = vecDir;
		m_vecSpread = vecSpread;
		m_flDistance = flDistance;
		m_iAmmoType = nAmmoType;
		m_iTracerFreq = 4;
		m_flDamage = 0;
		m_iPlayerDamage = 0;
		m_pAttacker = NULL;
		m_nFlags = 0;
		m_pAdditionalIgnoreEnt = NULL;
		m_flDamageForceScale = 1.0f;
		m_bPrimaryAttack = bPrimaryAttack;
		m_bUseServerRandomSeed = false;
	}

	int m_iShots = 0;
	vector m_vecSrc = { };
	vector m_vecDirShooting = { };
	vector m_vecSpread = { };
	float m_flDistance = 0.f;
	int m_iAmmoType = 0;
	int m_iTracerFreq = 0;
	float m_flDamage = 0.f;
	int m_iPlayerDamage = 0;
	int m_nFlags = 0;
	float m_flDamageForceScale = 0.f;
	C_BaseEntity* m_pAttacker = nullptr;
	C_BaseEntity* m_pAdditionalIgnoreEnt = nullptr;
	bool m_bPrimaryAttack = false;
	bool m_bUseServerRandomSeed = false;
};

class C_BaseProjectile : public C_BaseAnimating
{
public:
	NETVAR(m_hOriginalLauncher, int, "CBaseProjectile", "m_hOriginalLauncher");
};

class C_BaseGrenade : public C_BaseProjectile
{
public:
	NETVAR(m_flDamage, float, "CBaseGrenade", "m_flDamage");
	NETVAR(m_DmgRadius, float, "CBaseGrenade", "m_DmgRadius");
	NETVAR(m_bIsLive, bool, "CBaseGrenade", "m_bIsLive");
	NETVAR(m_hThrower, int, "CBaseGrenade", "m_hThrower");
	NETVAR(m_vecVelocity, vector, "CBaseGrenade", "m_vecVelocity");
	NETVAR(m_fFlags, int, "CBaseGrenade", "m_fFlags");
};

class C_TFWeaponBaseGrenadeProj : public C_BaseGrenade
{
public:
	NETVAR(m_vInitialVelocity, vector, "CTFWeaponBaseGrenadeProj", "m_vInitialVelocity");
	NETVAR(m_bCritical, bool, "CTFWeaponBaseGrenadeProj", "m_bCritical");
	NETVAR(m_iDeflected, int, "CTFWeaponBaseGrenadeProj", "m_iDeflected");
	NETVAR(m_vecOrigin, vector, "CTFWeaponBaseGrenadeProj", "m_vecOrigin");
	NETVAR(m_angRotation, vector, "CTFWeaponBaseGrenadeProj", "m_angRotation");
	NETVAR(m_hDeflectOwner, int, "CTFWeaponBaseGrenadeProj", "m_hDeflectOwner");
};

class C_TFGrenadePipebombProjectile : public C_TFWeaponBaseGrenadeProj
{
public:
	NETVAR(m_bTouched, bool, "CTFGrenadePipebombProjectile", "m_bTouched");
	NETVAR(m_iType, int, "CTFGrenadePipebombProjectile", "m_iType");
	NETVAR(m_hLauncher, EHANDLE, "CTFGrenadePipebombProjectile", "m_hLauncher");
	NETVAR(m_bDefensiveBomb, bool, "CTFGrenadePipebombProjectile", "m_bDefensiveBomb");
};

class C_BaseCombatWeapon : public C_BaseAnimating
{
public:
	NETVAR(m_iClip1, int, "CBaseCombatWeapon", "m_iClip1");
	NETVAR(m_iClip2, int, "CBaseCombatWeapon", "m_iClip2");
	NETVAR(m_iPrimaryAmmoType, int, "CBaseCombatWeapon", "m_iPrimaryAmmoType");
	NETVAR(m_iSecondaryAmmoType, int, "CBaseCombatWeapon", "m_iSecondaryAmmoType");
	NETVAR(m_nViewModelIndex, int, "CBaseCombatWeapon", "m_nViewModelIndex");
	NETVAR(m_bFlipViewModel, bool, "CBaseCombatWeapon", "m_bFlipViewModel");
	NETVAR(m_flNextPrimaryAttack, float, "CBaseCombatWeapon", "m_flNextPrimaryAttack");
	NETVAR(m_flNextSecondaryAttack, float, "CBaseCombatWeapon", "m_flNextSecondaryAttack");
	NETVAR(m_nNextThinkTick, int, "CBaseCombatWeapon", "m_nNextThinkTick");
	NETVAR(m_flTimeWeaponIdle, float, "CBaseCombatWeapon", "m_flTimeWeaponIdle");
	NETVAR(m_iViewModelIndex, int, "CBaseCombatWeapon", "m_iViewModelIndex");
	NETVAR(m_iWorldModelIndex, int, "CBaseCombatWeapon", "m_iWorldModelIndex");
	NETVAR(m_iState, int, "CBaseCombatWeapon", "m_iState");
	NETVAR_OFFSET_FROM(m_bInReload, bool, "CBaseCombatWeapon", "m_flTimeWeaponIdle", 4);

	const char* GetName();
	bool GetAttachment(int number, vector& origin);
};