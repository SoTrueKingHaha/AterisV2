#include "C_TFWeaponBase.h"

#include "../interfaces/interfaces.h"
#include "../../utils/signatures/signatures.h"

int C_TFWeaponBase::GetSlot()
{
	static auto FN = reinterpret_cast<int(__thiscall*)(C_TFWeaponBase*)>(memory->get_vfunc(this, 330));
	return FN(this);
}

bool C_TFWeaponBase::InReload()
{
	return (m_bInReload() || m_iReloadMode() != 0);
}

const char* C_TFWeaponBase::GetName()
{
	static auto FN = reinterpret_cast<const char* (__thiscall*)(C_TFWeaponBase*)>(memory->get_vfunc(this, 333));
	return FN(this);
}

int C_TFWeaponBase::GetWeaponID()
{
	return memory->get_vfunc<int(__thiscall*)(C_TFWeaponBase*)>(this, 381)(this);
}

int C_TFWeaponBase::GetDamageType()
{
	return memory->get_vfunc<int(__thiscall*)(C_TFWeaponBase*)>(this, 340)(this);
}

WeaponData_t C_TFWeaponBase::GetWeaponData(int m_iWeaponMode)
{
	static auto FN = reinterpret_cast<CTFWeaponInfo * (__cdecl*)(int)>(S::C_TFWeaponBase_GetWeaponData.address);
	return FN(GetWeaponID())->m_WeaponData[m_iWeaponMode];
}

CTFWeaponInfo* C_TFWeaponBase::GetWeaponInfo()
{
	static auto FN = reinterpret_cast<CTFWeaponInfo * (__cdecl*)(int)>(S::C_TFWeaponBase_GetWeaponInfo.address);
	if (!this)
	{
		return nullptr;
	}
	return FN(GetWeaponID());
}

//WeaponData_t C_TFWeaponBase::GetWeaponData()
//{
//	static auto GetTFWeaponDataFn = reinterpret_cast<CTFWeaponInfo * (__cdecl*)(int)>(S::GetWeaponData.Get());
//	return GetTFWeaponDataFn(GetWeaponID())->m_WeaponData[0];
//}
//
//CTFWeaponInfo* C_TFWeaponBase::GetWeaponInfo()
//{
//	static auto GetTFWeaponDataFn = reinterpret_cast<CTFWeaponInfo * (__cdecl*)(int)>(S::GetWeaponData.Get());
//	return GetTFWeaponDataFn(GetWeaponID());
//}
//
float C_TFWeaponBase::GetWeaponSpread( )
{
	static auto GetWeaponSpreadFn = reinterpret_cast< float( __thiscall* )( decltype( this ) ) >( S::GetWeaponSpread.address );
	return GetWeaponSpreadFn( this );
}

float C_TFWeaponBase::GetSwingRange(C_TFPlayer* entity)
{
	return memory->get_vfunc<float(__thiscall*)(C_TFPlayer*)>(this, 455)(entity);
}

bool C_TFWeaponBase::DoSwingTrace(trace_t* trace)
{
	return memory->get_vfunc<bool(__thiscall*)(trace_t*)>(this, 457)(trace);
}

// bool (__thiscall*)(void*)

bool C_TFWeaponBase::CalcIsAttackCriticalHelperNoCrits()
{
	return memory->get_vfunc<bool(__thiscall*)(void*)>(this, 463)(this);
}

bool C_TFWeaponBase::AreRandomCritsEnabled()
{
	return memory->get_vfunc<bool(__thiscall*)(void*)>(this, 470)(this);
}

bool C_TFWeaponBase::CalcIsAttackCritical()
{
	static auto FN = reinterpret_cast<bool(__thiscall*)(void*)>(S::C_TFWeaponBase_CalcIsAttackCritical.address);
	return FN(this);
}

bool C_TFWeaponBase::CalcIsAttackCriticalHelper()
{
	static auto FN = reinterpret_cast<bool(__thiscall*)(void*)>(S::C_TFweaponBase_CalcIsAttackCriticalHelper.address);
	return FN(this);
}

bool C_TFWeaponBase::CalcIsAttackCriticalHelperMelee()
{
	static auto FN = reinterpret_cast<bool(__thiscall*)(void*)>(S::C_TFWeaponBaseMelee_CalcIsAttackCriticalHelper.address);
	return FN(this);
}

bool C_TFWeaponBase::WithdrawFromCritbucket( float damage )
{
	static auto FN = reinterpret_cast< bool( __thiscall* )( void*, float ) >( S::IsAllowedToWithdrawFromCritBucket.address );
	return FN( this, damage );
}

bool C_TFWeaponBase::CanFireRandomCriticalShot(const float flCritChance)
{
	return memory->get_vfunc<bool(__thiscall*)(decltype(this), float)>(this, 423)(this, flCritChance);
}

bool C_TFWeaponBase::CanShoot()
{
	if (const auto& pLocal = GET_LOCAL_PLAYER()) {
		const auto nCond = pLocal->m_nPlayerCond();
		const auto nCond2 = pLocal->m_nPlayerCondEx2();
		if (pLocal->deadflag() || nCond & TFCond_Taunting || nCond && TFCond_Bonked || nCond2 & TFCondEx2_HalloweenGhostMode)
		{
			return false;
		}

		if (pLocal->m_iClass() == CLASS_SPY)
		{
			const auto flCurtime = i::global_vars->curtime;
			{ // Dead ringer
				static float flTimer = 0.0f;

				if (pLocal->m_bFeignDeathReady())
				{
					flTimer = 0.0f;
					return false;
				}

				else
				{
					if (!flTimer)
					{
						flTimer = flCurtime;
					}

					if (flTimer > flCurtime)
					{
						flTimer = 0.0f;
					}

					if ((flCurtime - flTimer) < 0.4f)
					{
						return false;
					}
				}
			}

			{ // Normal invisibility
				static float flTimer = 0.0f;

				if (pLocal->IsCloaked())
				{
					flTimer = 0.0f;
					return false;
				}

				else
				{
					if (!flTimer)
					{
						flTimer = flCurtime;
					}

					if (flTimer > flCurtime)
					{
						flTimer = 0.0f;
					}

					if ((flCurtime - flTimer) < 2.0f)
					{
						return false;
					}
				}
			}
		}

		return ( m_flNextPrimaryAttack( ) < ( float )( pLocal->m_nTickBase( ) ) * i::global_vars->interval_per_tick );
	}
	else
	{
		return false;
	}
}

bool C_TFWeaponBase::CanSecondaryAttack()
{
	if (const auto& pLocal = GET_LOCAL_PLAYER())
	{
		const auto nCond = pLocal->m_nPlayerCond();
		const auto nCond2 = pLocal->m_nPlayerCondEx2();
		if (pLocal->deadflag() || nCond & TFCond_Taunting || nCond && TFCond_Bonked || nCond2 & TFCondEx2_HalloweenGhostMode)
		{
			return false;
		}

		return (m_flNextSecondaryAttack() < (float)(pLocal->m_nTickBase()) * i::global_vars->interval_per_tick);
	}
	else
	{
		return false;
	}
}

void C_TFWeaponBase::GetProjectileFireSetup(C_TFPlayer* pPlayer, vector vecOffset, vector* vecSrc, QAngle* angForward, bool bHitTeammates, float flEndDist)
{
	static auto FN = reinterpret_cast<void(__thiscall*)(void*, C_TFPlayer*, vector, vector*, QAngle*, bool, float)>(S::C_TFWeaponBase_GetProjectileFireSetup.address);
	return FN(this, pPlayer, vecOffset, vecSrc, angForward, bHitTeammates, flEndDist);
}

EWeaponType C_TFWeaponBase::GetWeaponType()
{
	if (!this)
	{
		return EWeaponType::UNKNOWN;
	}

	if (GetSlot() == EWeaponSlots::SLOT_MELEE)
	{
		return EWeaponType::MELEE;
	}

	switch (GetWeaponID())
	{
		case TF_WEAPON_ROCKETLAUNCHER:
		case TF_WEAPON_GRENADELAUNCHER:
		case TF_WEAPON_FLAREGUN:
		case TF_WEAPON_COMPOUND_BOW:
		case TF_WEAPON_ROCKETLAUNCHER_DIRECTHIT:
		case TF_WEAPON_CROSSBOW:
		case TF_WEAPON_PARTICLE_CANNON:
		case TF_WEAPON_DRG_POMSON:
		case TF_WEAPON_FLAREGUN_REVENGE:
		case TF_WEAPON_CANNON:
		case TF_WEAPON_SYRINGEGUN_MEDIC:
		case TF_WEAPON_SHOTGUN_BUILDING_RESCUE:
		case TF_WEAPON_FLAMETHROWER:
		{
			return EWeaponType::PROJECTILE;
		}

		case TF_WEAPON_PIPEBOMBLAUNCHER:
		case 109: //dragon's fury
		{
			//broken Idk
			return EWeaponType::UNKNOWN;
		}

		default:
		{
			int nDamageType = GetDamageType();

			if (nDamageType & DMG_BULLET || nDamageType && DMG_BUCKSHOT)
				return EWeaponType::HITSCAN;

			break;
		}
	}

	return EWeaponType::UNKNOWN;
}

bool C_TFWeaponBase::CanFireCriticalShot(const bool bHeadshot)
{
	bool bReturn = false;

	if (const auto& pOwner = m_hOwnerEntity().Get()->As<C_TFPlayer>())
	{
		const int nOldFOV = pOwner->m_iFOV();
		pOwner->m_iFOV() = 70;

		static auto FN = memory->get_vfunc<bool(__thiscall*)(decltype(this), bool, C_BaseEntity*)>(this, 425); 

		bReturn = FN(this, bHeadshot, nullptr);

		pOwner->m_iFOV() = nOldFOV;
	}

	return bReturn;
}

bool C_TFWeaponBase::CanWeaponHeadShot()
{
	return ((GetDamageType() & DMG_USE_HITLOCATIONS) && CanFireCriticalShot(true)); //credits to bertti
}

bool C_TFWeaponBase::IsAttacking(CUserCmd* pCmd)
{
	return false;
	/*
	const int nWeaponSlot = GetSlot();
	const int nWeaponID = GetWeaponID();
	if (nWeaponSlot == SLOT_MELEE)
	{
		if (nWeaponID == TF_WEAPON_KNIFE)
		{
			return ((pCmd->buttons & IN_ATTACK) && H::WeaponCanAttack);
		}
		else
		{
			return fabs(m_flSmackTime() - i::GlobalVarsBase->curtime) < i::GlobalVarsBase->interval_per_tick * 2.0f;
		}
	}
	else
	{
		if (H::CurrentItemDefinitionIndex == Soldier_m_TheBeggarsBazooka)
		{
			static bool s_bLoadingBazooka = false;

			if (m_iClip1() > 0)
			{
				s_bLoadingBazooka = true;
			}

			if (!(pCmd->buttons & IN_ATTACK) && s_bLoadingBazooka)
			{
				s_bLoadingBazooka = false;
				return true;
			}
		}
		else
		{
			switch (nWeaponID)
			{
				case TF_WEAPON_COMPOUND_BOW:
				case TF_WEAPON_PIPEBOMBLAUNCHER:
				case TF_WEAPON_STICKY_BALL_LAUNCHER:
				case TF_WEAPON_GRENADE_STICKY_BALL:
				case TF_WEAPON_CANNON:
				{
					static bool s_bChargingBallistic = false;

					if (this->As<C_TFPipebombLauncher>()->m_flChargeBeginTime() > 0.0f)
					{
						s_bChargingBallistic = true;
					}

					if (!(pCmd->buttons & IN_ATTACK) && s_bChargingBallistic)
					{
						s_bChargingBallistic = false;
						return true;
					}

					break;
				}

				case TF_WEAPON_JAR:
				case TF_WEAPON_JAR_MILK:
				case TF_WEAPON_JAR_GAS:
				case TF_WEAPON_GRENADE_JAR_GAS:
				case TF_WEAPON_CLEAVER:
				{
					static float s_flLastThrowTime = 0.0f;

					if ((pCmd->buttons & IN_ATTACK) && H::WeaponCanAttack && !s_flLastThrowTime)
					{
						s_flLastThrowTime = i::GlobalVarsBase->curtime + 0.16f;
					}

					if (s_flLastThrowTime && i::GlobalVarsBase->curtime >= s_flLastThrowTime)
					{
						s_flLastThrowTime = 0.0f;
						return true;
					}

					break;
				}
				default:
				{
					if ((pCmd->buttons & IN_ATTACK) && H::WeaponCanAttack)
					{
						return true;
					}

					break;
				}
			}
		}
	}

	return false;
	*/
}

int C_TFWeaponBase::GetBulletsAmount( )
{
	auto v7 = *( DWORD* )( this + 2864 );
	auto v8 = *( DWORD* )( this + 2852 ) << 6;
	int bulletspershot = *( DWORD* )( v8 + v7 + 1788 );
	if ( bulletspershot >= 1 )
	{
		CALL_ATTRIB_HOOK_FLOAT( bulletspershot, "mult_bullets_per_shot" );
	}
	else
	{
		bulletspershot = 1;
	}
	return bulletspershot;
}

bool C_TFWeaponBase::IsStreamingWeapon( )
{
	switch ( GetWeaponID( ) )
	{
		case TF_WEAPON_MINIGUN:
		case TF_WEAPON_FLAMETHROWER:
		case TF_WEAPON_PISTOL:
		case TF_WEAPON_PISTOL_SCOUT:
		case TF_WEAPON_HANDGUN_SCOUT_SECONDARY:
		case TF_WEAPON_SMG:
		case TF_WEAPON_SYRINGEGUN_MEDIC:
		case TF_WEAPON_HANDGUN_SCOUT_PRIMARY:
		{
			return true;
			break;
		}
		default: break;
	}

	return false;
}

int C_TFWeaponBase::GetReserveAmmo( )
{
	HMODULE hModule = GetModuleHandleA( "client.dll" );
	if ( hModule )
	{
		DWORD address = ( DWORD )hModule + 0xC8B5FC;
		DWORD offset1 = 0xD0;
		DWORD offset2 = 0x58;
		DWORD offset3 = 0x84;
		DWORD offset4 = 0x19C;

		return *reinterpret_cast< int* >( *reinterpret_cast< DWORD* >( *reinterpret_cast< DWORD* >( *reinterpret_cast< DWORD* >( *reinterpret_cast< DWORD* >( address ) + offset1 ) + offset2 ) + offset3 ) + offset4 );
	}

	return 0;
}

int C_TFWeaponBase::GetMaxAmmo( )
{
	if ( const auto& local = GET_LOCAL_PLAYER( ) )
	{
		switch ( local->m_iClass( ) )
		{
			case CLASS_SCOUT:
			{
				switch ( GetSlot( ) )
				{
					case SLOT_PRIMARY:
					{
						return 32;
						break;
					}

					case SLOT_SECONDARY:
					{
						return 36;
						break;
					}
					default: break;
				}
				break;
			}

			case CLASS_SOLDIER:
			{
				switch ( GetSlot( ) )
				{
					case SLOT_PRIMARY:
					{
						return 20;
						break;
					}

					case SLOT_SECONDARY:
					{
						return 32;
						break;
					}
					default: break;
				}
				break;
			}

			case CLASS_PYRO:
			{
				switch ( GetSlot( ) )
				{
					case SLOT_PRIMARY:
					{
						switch ( GetWeaponID( ) )
						{
							case TF_WEAPON_FLAME_BALL :
							{
								return 40;
								break;
							}

							case TF_WEAPON_FLAMETHROWER:
							{
								return 200;
								break;
							}
							default: break;
						}
						break;
					}

					case SLOT_SECONDARY:
					{
						switch ( GetWeaponID( ) )
						{
							case TF_WEAPON_SHOTGUN_PYRO:
							{
								return 32;
								break;
							}

							case TF_WEAPON_FLAREGUN:
							case TF_WEAPON_FLAREGUN_REVENGE:
							{
								return 16;
								break;
							}
						}
						break;
					}
					default: break;
				}
				break;
			}

			case CLASS_DEMOMAN:
			{
				switch ( GetWeaponID( ) )
				{
					case TF_WEAPON_GRENADELAUNCHER:
					{
						return 16;
						break;
					}
					case TF_WEAPON_PIPEBOMBLAUNCHER:
					{
						switch ( m_iItemDefinitionIndex( ) )
						{
							case Demoman_s_TheScottishResistance:
							{
								return 36;
								break;
							}
							default:
							{
								return 24;
							}
						}
					}
					default: break;
				}
				break;
			}

			case CLASS_HEAVY:
			{
				switch ( GetSlot( ) )
				{
					case SLOT_PRIMARY:
					{
						return 200;
						break;
					}

					case SLOT_SECONDARY:
					{
						switch ( GetWeaponID( ) )
						{
							case TF_WEAPON_SHOTGUN_HWG:
							{
								return 32;
								break;
							}

							case TF_WEAPON_LUNCHBOX:
							{
								return 1;
								break;
							}
						}
					}
					default: break;
				}
				break;
			}

			case CLASS_ENGINEER:
			{
				switch ( GetSlot( ) )
				{
					case SLOT_PRIMARY:
					{
						switch( GetWeaponID( ) )
						{
							case TF_WEAPON_SHOTGUN_BUILDING_RESCUE:
							{
								return 16;
								break;
							}
							case TF_WEAPON_SHOTGUN_PRIMARY:
							{
								return 32;
								break;
							}
							default: break;
						}
						return 32;
						break;
					}

					case SLOT_SECONDARY:
					{
						return 200;
						break;
					}
					default: break;
				}
				break;
			}

			case CLASS_MEDIC:
			{
				switch ( GetSlot( ) )
				{
					case SLOT_PRIMARY:
					{
						switch ( GetWeaponID( ) )
						{
							case TF_WEAPON_CROSSBOW:
							{
								return 38;
								break;
							}
							case TF_WEAPON_SYRINGEGUN_MEDIC:
							{
								return 150;
							}
							default: break;
						}
						break;
					}
					default: break;
				}

				break;
			}

			case CLASS_SNIPER:
			{
				switch ( GetSlot( ) )
				{
					case SLOT_PRIMARY:
					{
						switch ( GetWeaponID( ) )
						{
							case TF_WEAPON_COMPOUND_BOW:
							{
								return 12;
								break;
							}
							case TF_WEAPON_SNIPERRIFLE:
							{
								return 25;
								break;
							}
							default: break;
						}
						break;
					}

					case SLOT_SECONDARY:
					{
						switch ( GetWeaponID( ) )
						{
							case TF_WEAPON_SMG:
							{
								return 75;
								break;
							}
							default: break;
						}
						break;
					}
					default: break;
				}

				break;
			}

			case CLASS_SPY:
			{
				switch ( GetSlot( ) )
				{
					case SLOT_PRIMARY:
					{
						return 24;
						break;
					}

					default: break;
				}

				break;
			}

			default: break;
		}
	}

	return 0;
}

bool C_TFWeaponBase::WillCrit( )
{
	return this->GetSlot( ) == SLOT_MELEE ? this->CalcIsAttackCriticalHelperMelee( ) : this->CalcIsAttackCriticalHelper( );
}