#pragma once

#include "icollideable.h"


#include "../../utils/netvars/netvars.h"
#include "../../utils/signatures/signatures.h"
#include "UtlFlags.h"
#include "utlvector.h"



typedef CHandle<C_BaseEntity> EHANDLE;

enum thinkmethods_t
{
	THINK_FIRE_ALL_FUNCTIONS,
	THINK_FIRE_BASE_ONLY,
	THINK_FIRE_ALL_BUT_BASE
};

enum ETFGrenadeProjectileType
{
	TYPE_PIPE,
	TYPE_STICKY
};

class CCollisionProperty : public ICollideable
{
public:
	__inline void SetCollisionBounds( const vector& mins, const vector& maxs )
	{
		static auto FN = reinterpret_cast< void( __thiscall* )( CCollisionProperty*, const vector&, const vector& ) >( ( DWORD )memory->find_signature( "client.dll", "55 8B EC 83 EC 28 53 8B 5D 08 56 8B 75 0C 57 8B 03" ) );
		FN( this, mins, maxs );
	}

	__inline void CalcNearestPoint( const vector& vecWorldPt, vector* pVecNearestWorldPt )
	{
		static auto FN = reinterpret_cast< void( __thiscall* )( CCollisionProperty*, const vector&, vector* ) >( ( DWORD )memory->find_signature( "client.dll", "55 8B EC 83 EC 28 53 8B 5D 08 56 8B 75 0C 57 8B 03" ) );
		FN( this, vecWorldPt, pVecNearestWorldPt );
	}
};

class C_BaseEntity : public IClientEntity
{	
public:
	NETVAR(m_flAnimTime, int, "CBaseEntity", "m_flAnimTime");
	NETVAR(m_flSimulationTime, float, "CBaseEntity", "m_flSimulationTime");
	NETVAR(m_ubInterpolationFrame, int, "CBaseEntity", "m_ubInterpolationFrame");
	NETVAR(m_vecOrigin, vector, "CBaseEntity", "m_vecOrigin");
	NETVAR(m_angRotation, vector, "CBaseEntity", "m_angRotation");
	NETVAR(m_nModelIndex, int, "CBaseEntity", "m_nModelIndex");
	NETVAR(m_fEffects, int, "CBaseEntity", "m_fEffects");
	NETVAR(m_nRenderMode, int, "CBaseEntity", "m_nRenderMode");
	NETVAR(m_nRenderFX, int, "CBaseEntity", "m_nRenderFX");
	NETVAR(m_clrRender, int, "CBaseEntity", "m_clrRender");
	NETVAR(m_iTeamNum, int, "CBaseEntity", "m_iTeamNum");
	NETVAR(m_CollisionGroup, int, "CBaseEntity", "m_CollisionGroup");
	NETVAR(m_flElasticity, float, "CBaseEntity", "m_flElasticity");
	NETVAR(m_flShadowCastDistance, float, "CBaseEntity", "m_flShadowCastDistance");
	NETVAR(m_hOwnerEntity, EHANDLE, "CBaseEntity", "m_hOwnerEntity");
	NETVAR(m_hEffectEntity, EHANDLE, "CBaseEntity", "m_hEffectEntity");
	NETVAR(moveparent, int, "CBaseEntity", "moveparent");
	NETVAR(m_iParentAttachment, int, "CBaseEntity", "m_iParentAttachment");
	NETVAR(m_Collision, void*, "CBaseEntity", "m_Collision");
	NETVAR(m_vecMinsPreScaled, vector, "CBaseEntity", "m_vecMinsPreScaled");
	NETVAR(m_vecMaxsPreScaled, vector, "CBaseEntity", "m_vecMaxsPreScaled");
	NETVAR(m_vecMins, vector, "CBaseEntity", "m_vecMins");
	NETVAR(m_vecMaxs, vector, "CBaseEntity", "m_vecMaxs");
	NETVAR(m_nSolidType, int, "CBaseEntity", "m_nSolidType");
	NETVAR(m_usSolidFlags, int, "CBaseEntity", "m_usSolidFlags");
	NETVAR(m_nSurroundType, int, "CBaseEntity", "m_nSurroundType");
	NETVAR(m_triggerBloat, int, "CBaseEntity", "m_triggerBloat");
	NETVAR(m_bUniformTriggerBloat, bool, "CBaseEntity", "m_bUniformTriggerBloat");
	NETVAR(m_vecSpecifiedSurroundingMinsPreScaled, vector, "CBaseEntity", "m_vecSpecifiedSurroundingMinsPreScaled");
	NETVAR(m_vecSpecifiedSurroundingMaxsPreScaled, vector, "CBaseEntity", "m_vecSpecifiedSurroundingMaxsPreScaled");
	NETVAR(m_vecSpecifiedSurroundingMins, vector, "CBaseEntity", "m_vecSpecifiedSurroundingMins");
	NETVAR(m_vecSpecifiedSurroundingMaxs, vector, "CBaseEntity", "m_vecSpecifiedSurroundingMaxs");
	NETVAR(m_iTextureFrameIndex, int, "CBaseEntity", "m_iTextureFrameIndex");
	NETVAR(m_PredictableID, int, "CBaseEntity", "m_PredictableID");
	NETVAR(m_bIsPlayerSimulated, bool, "CBaseEntity", "m_bIsPlayerSimulated");
	NETVAR(m_bSimulatedEveryTick, bool, "CBaseEntity", "m_bSimulatedEveryTick");
	NETVAR(m_bAnimatedEveryTick, bool, "CBaseEntity", "m_bAnimatedEveryTick");
	NETVAR(m_bAlternateSorting, bool, "CBaseEntity", "m_bAlternateSorting");
	NETVAR(m_nModelIndexOverrides, void*, "CBaseEntity", "m_nModelIndexOverrides");
	NETVAR_OFFSET_PURE(m_pMoveParent, CHandle<C_BaseEntity>, 0x17C);
	NETVAR_OFFSET_PURE(m_pMoveChild, CHandle<C_BaseEntity>, 0x1B0);
	NETVAR_OFFSET_PURE(m_pMoveNextPeer, CHandle<C_BaseEntity>, 0x1B4);
	NETVAR_OFFSET_PURE(m_pMovePrevPeer, CHandle<C_BaseEntity>, 0x1B8);
	inline CCollisionProperty* m_CollisionProperty( )
	{
		return reinterpret_cast< CCollisionProperty* >( reinterpret_cast<DWORD>( this ) + 0x1C8 );
	}

	inline MoveType_t GetMoveType( )
	{
		return *reinterpret_cast< MoveType_t* >( reinterpret_cast< DWORD >( this ) + 0x1A4 );
	}

	NETVAR_OFFSET_PURE( m_flGravity, float, 0x64 );
	NETVAR_OFFSET_PURE(m_nPipebombType, int, 0x8FC);
	NETVAR_OFFSET_PURE(m_flCreationTime, float, 0x900);
	NETVAR_OFFSET_PURE(m_bPipebombPulsed, bool, 0x908);

	NETVAR_OFFSET_FROM(movetype, int, "CBaseEntity", "movetype", 0x1A4);

	//NETVAR_OFFSET_PURE(m_pAttributes, IHasAttributes*, 84);

	__forceinline bool IsBomb( )
	{
		switch ( GetClassID( ) )
		{
			case ETFClassID::CTFPumpkinBomb:
			case ETFClassID::CTFGenericBomb:
			{
				return true;
				break;
			}
			default: break;
		}

		return false;
	}

	__forceinline bool IsProjectile( )
	{
		switch ( GetClassID( ) )
		{
			case ETFClassID::CTFProjectile_Rocket:
			case ETFClassID::CTFGrenadePipebombProjectile:
			case ETFClassID::CTFProjectile_Jar:
			case ETFClassID::CTFProjectile_JarGas:
			case ETFClassID::CTFProjectile_JarMilk:
			case ETFClassID::CTFProjectile_Arrow:
			case ETFClassID::CTFProjectile_SentryRocket:
			case ETFClassID::CTFProjectile_Flare:
			case ETFClassID::CTFProjectile_Cleaver:
			case ETFClassID::CTFProjectile_HealingBolt:
			case ETFClassID::CTFProjectile_EnergyRing:
			case ETFClassID::CTFProjectile_EnergyBall:
			case ETFClassID::CTFProjectile_ThrowableBreadMonster:
			case ETFClassID::CTFStunBall:
			case ETFClassID::CTFBall_Ornament:
			{
				return true;
				break;
			}
			default: break;
		}

		return false;
	}
	
	__forceinline bool IsInValidTeam()
	{
		return this->m_iTeamNum() == TEAM_RED || this->m_iTeamNum() == TEAM_BLU;
	}

	__forceinline bool IsPlayer()
	{
		return static_cast<ETFClassID>(GetClientClass()->m_ClassID) == ETFClassID::CTFPlayer;
	}

	__forceinline void Think( )
	{
		static auto FN = reinterpret_cast< void( __thiscall* )( void* ) >( memory->get_vfunc( this, 174 ) );
		FN( this );
	}

	__forceinline float m_flOldSimulationTime( )
	{
		static int offset = netvars.find_netvar( "CBaseEntity", "m_flSimulationTime" );
		return *reinterpret_cast< float* >( reinterpret_cast< DWORD >( this ) + offset + sizeof( int ) );
	}

	__forceinline int GetAmmoCount( int iAmmoIndex )
	{
		static auto FN = reinterpret_cast< int( __thiscall* )( C_BaseEntity*, int ) >( S::GetAmmoCount.address );
		return FN( this, iAmmoIndex );
	}

	int SaveData( const char* const context, const int slot, const int type );
	int RestoreData( const char* const context, const int slot, const int type );

	bool IsWearable();
	bool OnSteamFriendsList();
	bool ShouldCollide(int collisionGroup, int contentsMask);
	void UpdateVisibility();
	void SetAbsAngles(const QAngle& vAngles);
	void SetAbsOrigin(const vector& vOrigin);
	vector EstimateAbsVelocity();
	vector GetHitboxPosition(const int nHitbox);
	vector GetWorldSpaceCenter();
	ETFClassID GetClassID();
	SolidType_t GetSolid();
	C_BaseEntity* FirstMoveChild();
	C_BaseEntity* NextMovePeer();
	C_BaseEntity* GetOwnerEntity();
};

namespace G
{
	float AttribHookValue(float flValue, const char* pszAttribHook, C_BaseEntity* pEntity, CUtlVector<C_BaseEntity*>* pItemList = NULL, bool bIsGlobalConstString = false);
}

#define CALL_ATTRIB_HOOK( vartype, retval, hookName, who, itemlist) \
	retval = G::AttribHookValue( retval, #hookName, who, itemlist, true);

#define CALL_ATTRIB_HOOK_INT( retval, hookName )	CALL_ATTRIB_HOOK( int, retval, hookName, this, NULL )
#define CALL_ATTRIB_HOOK_FLOAT( retval, hookName )	CALL_ATTRIB_HOOK( float, retval, hookName, this, NULL )
#define CALL_ATTRIB_HOOK_STRING( retval, hookName )	CALL_ATTRIB_HOOK( CAttribute_String, retval, hookName, this, NULL )
#define CALL_ATTRIB_HOOK_INT_ON_OTHER( other, retval, hookName )	CALL_ATTRIB_HOOK( int, retval, hookName, other, NULL )
#define CALL_ATTRIB_HOOK_FLOAT_ON_OTHER( other, retval, hookName )	CALL_ATTRIB_HOOK( float, retval, hookName, other, NULL )
#define CALL_ATTRIB_HOOK_STRING_ON_OTHER( other, retval, hookName )	CALL_ATTRIB_HOOK( CAttribute_String, retval, hookName, other, NULL )
#define CALL_ATTRIB_HOOK_INT_ON_OTHER_WITH_ITEMS( other, retval, items_array, hookName )	CALL_ATTRIB_HOOK( int, retval, hookName, other, items_array )
#define CALL_ATTRIB_HOOK_FLOAT_ON_OTHER_WITH_ITEMS( other, retval, items_array, hookName )	CALL_ATTRIB_HOOK( float, retval, hookName, other, items_array )
#define CALL_ATTRIB_HOOK_STRING_ON_OTHER_WITH_ITEMS( other, retval, items_array, hookName )	CALL_ATTRIB_HOOK( CAttribute_String, retval, hookName, other, items_array )