#pragma once
#include "GameEventListener.h"
#include "C_BaseCombatCharacter.h"
#include "c_usercmd.h"
#include "../../utils/signatures/signatures.h"

class CSteamID;
class IRagdoll;
class CViewSetup;
class CHintSystem;
class IGameEvent;
class C_BaseViewModel;

enum PLAYER_ANIM;

struct surfacedata_t;

class C_CommandContext
{
public:
	bool			needsprocessing;

	CUserCmd		cmd;
	int				command_number;
};

class C_PredictionError
{
public:
	float	time;
	vector	error;
};

#define CHASE_CAM_DISTANCE_MIN	16.0f
#define CHASE_CAM_DISTANCE_MAX	96.0f
#define WALL_OFFSET				6.0f

class CPlayerState
{
public:
	// This virtual method is necessary to generate a vtable in all cases
	// (DECLARE_PREDICTABLE will generate a vtable also)!
	virtual ~CPlayerState() {}

	// true if the player is dead
	bool        deadflag;
	// Viewing angle (player only)
	vector		v_angle;
};

enum EObserverModes : int
{
	OBS_MODE_NONE = 0,		// not in spectator mode
	OBS_MODE_DEATHCAM,		// special mode for death cam animation
	OBS_MODE_FREEZECAM,		// zooms to a target, and freeze-frames on them
	OBS_MODE_FIXED,			// view from a fixed camera position
	OBS_MODE_FIRSTPERSON,	// follow a player in first person view
	OBS_MODE_THIRDPERSON,	// follow a player in third person view
	OBS_MODE_ROAMING,		// free roaming
};

class CBoneCacheHandler
{
public:
	float			m_TimeValid;
	int				m_BoneMask;
	unsigned int	m_Size;
	unsigned short	m_CachedBoneCount;
	unsigned short	m_MatrixOffset;
	unsigned short	m_CachedToStudioOffset;
	unsigned short	m_BoneOutOffset;
	matrix3x4_t* BoneArray()
	{
		return (matrix3x4_t*)((char*)(this + 1) + m_MatrixOffset);
	}
	short* CachedToStudio()
	{
		return (short*)((char*)(this + 1) + m_CachedToStudioOffset);
	}
	void UpdateBones(matrix3x4_t* pBoneToWorld, int numbones, float curtime)
	{
		const matrix3x4_t* pBones = BoneArray();
		const short* pCachedToStudio = CachedToStudio();

		for (int i = 0; i < m_CachedBoneCount; i++)
		{
			const int index = pCachedToStudio[i];
			memcpy(pBoneToWorld[index].matrix3x4, pBones[i].matrix3x4, sizeof(matrix3x4_t));
		}
		m_TimeValid = curtime;
	}
};

class C_BasePlayer : public C_BaseCombatCharacter, public CGameEventListener
{
public:
	NETVAR(m_Local, void*, "CBasePlayer", "m_Local");
	NETVAR(m_chAreaBits, void*, "CBasePlayer", "m_chAreaBits");
	NETVAR(m_chAreaPortalBits, void*, "CBasePlayer", "m_chAreaPortalBits");
	NETVAR(m_iHideHUD, int, "CBasePlayer", "m_iHideHUD");
	NETVAR(m_flFOVRate, float, "CBasePlayer", "m_flFOVRate");
	NETVAR(m_bDucked, bool, "CBasePlayer", "m_bDucked");
	NETVAR(m_bDucking, bool, "CBasePlayer", "m_bDucking");
	NETVAR(m_bInDuckJump, bool, "CBasePlayer", "m_bInDuckJump");
	NETVAR(m_flDucktime, float, "CBasePlayer", "m_flDucktime");
	NETVAR(m_flDuckJumpTime, float, "CBasePlayer", "m_flDuckJumpTime");
	NETVAR(m_flJumpTime, float, "CBasePlayer", "m_flJumpTime");
	NETVAR(m_flFallVelocity, float, "CBasePlayer", "m_flFallVelocity");
	NETVAR(m_vecPunchAngle, vector, "CBasePlayer", "m_vecPunchAngle");
	NETVAR(m_vecPunchAngleVel, vector, "CBasePlayer", "m_vecPunchAngleVel");
	NETVAR(m_bDrawViewmodel, bool, "CBasePlayer", "m_bDrawViewmodel");
	NETVAR(m_bWearingSuit, bool, "CBasePlayer", "m_bWearingSuit");
	NETVAR(m_bPoisoned, bool, "CBasePlayer", "m_bPoisoned");
	NETVAR(m_flStepSize, float, "CBasePlayer", "m_flStepSize");
	NETVAR(m_bAllowAutoMovement, bool, "CBasePlayer", "m_bAllowAutoMovement");
	NETVAR(m_vecViewOffset, vector, "CBasePlayer", "m_vecViewOffset[0]");
	NETVAR(m_flFriction, float, "CBasePlayer", "m_flFriction");
	NETVAR(m_iAmmo, void*, "CBasePlayer", "m_iAmmo");
	NETVAR(m_fOnTarget, int, "CBasePlayer", "m_fOnTarget");
	NETVAR(m_nTickBase, int, "CBasePlayer", "m_nTickBase");
	NETVAR(m_nNextThinkTick, int, "CBasePlayer", "m_nNextThinkTick");
	NETVAR(m_hLastWeapon, EHANDLE, "CBasePlayer", "m_hLastWeapon");
	NETVAR(m_hGroundEntity, EHANDLE, "CBasePlayer", "m_hGroundEntity");
	NETVAR(m_vecVelocity, vector, "CBasePlayer", "m_vecVelocity[0]");
	NETVAR(m_vecBaseVelocity, vector, "CBasePlayer", "m_vecBaseVelocity");
	NETVAR(m_hConstraintEntity, int, "CBasePlayer", "m_hConstraintEntity");
	NETVAR(m_vecConstraintCenter, vector, "CBasePlayer", "m_vecConstraintCenter");
	NETVAR(m_flConstraintRadius, float, "CBasePlayer", "m_flConstraintRadius");
	NETVAR(m_flConstraintWidth, float, "CBasePlayer", "m_flConstraintWidth");
	NETVAR(m_flConstraintSpeedFactor, float, "CBasePlayer", "m_flConstraintSpeedFactor");
	NETVAR(m_flDeathTime, float, "CBasePlayer", "m_flDeathTime");
	NETVAR(m_nWaterLevel, int, "CBasePlayer", "m_nWaterLevel");
	NETVAR(m_flLaggedMovementValue, float, "CBasePlayer", "m_flLaggedMovementValue");
	NETVAR(m_AttributeList, void*, "CBasePlayer", "m_AttributeList");
	NETVAR(m_iAttributeDefinitionIndex, int, "CBasePlayer", "m_iAttributeDefinitionIndex");
	NETVAR(m_flValue, float, "CBasePlayer", "m_flValue");
	NETVAR(m_nRefundableCurrency, int, "CBasePlayer", "m_nRefundableCurrency");
	NETVAR(pl, CPlayerState, "CBasePlayer", "pl");
	NETVAR(deadflag, bool, "CBasePlayer", "deadflag");
	NETVAR(m_iFOV, int, "CBasePlayer", "m_iFOV");
	NETVAR(m_iFOVStart, int, "CBasePlayer", "m_iFOVStart");
	NETVAR(m_flFOVTime, float, "CBasePlayer", "m_flFOVTime");
	NETVAR(m_iDefaultFOV, int, "CBasePlayer", "m_iDefaultFOV");
	NETVAR(m_hZoomOwner, EHANDLE, "CBasePlayer", "m_hZoomOwner");
	NETVAR(m_hVehicle, EHANDLE, "CBasePlayer", "m_hVehicle");
	NETVAR(m_hUseEntity, EHANDLE, "CBasePlayer", "m_hUseEntity");
	NETVAR(m_iHealth, int, "CBasePlayer", "m_iHealth");
	NETVAR(m_lifeState, unsigned char, "CBasePlayer", "m_lifeState");
	NETVAR(m_iBonusProgress, int, "CBasePlayer", "m_iBonusProgress");
	NETVAR(m_iBonusChallenge, int, "CBasePlayer", "m_iBonusChallenge");
	NETVAR(m_flMaxspeed, float, "CBasePlayer", "m_flMaxspeed");
	NETVAR(m_fFlags, CUtlFlags<int>, "CBasePlayer", "m_fFlags");
	NETVAR(m_iObserverMode, int, "CBasePlayer", "m_iObserverMode");
	NETVAR(m_hObserverTarget, EHANDLE, "CBasePlayer", "m_hObserverTarget");
	NETVAR(m_hViewModel, EHANDLE, "CBasePlayer", "m_hViewModel[0]");
	NETVAR(m_szLastPlaceName, const char*, "CBasePlayer", "m_szLastPlaceName");
	NETVAR_OFFSET_FROM(m_pCurrentCommand, CUserCmd*, "CBasePlayer", "m_hConstraintEntity", -0x04);
	NETVAR_OFFSET_FROM( m_nImpulse, int, "CBasePlayer", "m_vecViewOffset[0]", sizeof( vector ) + sizeof( float ) + sizeof( int ) );

	inline void SetCurrentCmd( CUserCmd* pCmd )
	{
		static int offset = netvars.find_netvar( "CBasePlayer", "m_hConstraintEntity" );
		*reinterpret_cast< CUserCmd** >( reinterpret_cast< DWORD >( this ) + offset - 0x4 ) = pCmd;
	}

	inline void SetImpulse( int impulse )
	{
		static int offset = netvars.find_netvar( "CBasePlayer", "m_vecViewOffset[0]" );
		*reinterpret_cast< int* >( reinterpret_cast< DWORD >( this ) + offset + sizeof( vector ) + sizeof( float ) + sizeof( int ) ) = impulse;
	}

	inline void SetNextThinkTick( int tick )
	{
		static int offset = netvars.find_netvar( "CBasePlayer", "m_nNextThinkTick" );
		*reinterpret_cast< int* >( reinterpret_cast< DWORD >( this ) + offset ) = tick;
	}

	inline void SetTickBase( int tickbase )
	{
		static int offset = netvars.find_netvar( "CBasePlayer", "m_nTickBase" );
		*reinterpret_cast< int* >( reinterpret_cast< DWORD >( this ) + offset ) = tickbase;
	}

	static void SetPredictionPlayer( C_BasePlayer* player )
	{
		auto m_pPredictionPlayer = reinterpret_cast< C_BasePlayer* >( S::C_BaseEntity_PredictionPlayer.address );
		m_pPredictionPlayer = player;
	}

	inline void UpdateButtonState( int nUserCmdButtonMask )
	{
		static auto FN = reinterpret_cast< void( __thiscall* )( C_BasePlayer*, int ) >( S::C_BaseEntity_UpdateButtonState.address );
		FN( this, nUserCmdButtonMask );
	}

	inline void SelectItem( const char* ptr, int iSubType )
	{
		static auto FN = reinterpret_cast< void( __thiscall* )( C_BasePlayer*, const char*, int ) >( memory->get_vfunc( this, 336 ) );
		FN( this, ptr, iSubType );
	}

	inline bool PhysicsRunThink( const thinkmethods_t thinkMethod = THINK_FIRE_ALL_FUNCTIONS )
	{
		static auto FN = reinterpret_cast< bool( __thiscall* )( void*, thinkmethods_t ) >( S::C_BasePlayer_PhysicsRunThink.address );
		return FN( this, thinkMethod );
	}

	inline void PreThink( )
	{
		static auto FN = reinterpret_cast< void( __thiscall* )( void* ) >( memory->get_vfunc( this, 325 ) );
		FN( this );
	}

	inline void PostThink( )
	{
		static auto FN = reinterpret_cast< void( __thiscall* )( void* ) >( memory->get_vfunc( this, 326 ) );
		FN( this );
	}

	inline bool UsingStandardWeaponsInVehicle( )
	{
		static auto FN = reinterpret_cast< bool( __thiscall* )( void* ) >( S::C_BasePlayer_UsingStandardWeaponsInVehicle.address );
		return FN( this );
	}

	int GetMaxHealth();
	CBoneCacheHandler* GetBoneCache();
	bool IsAlive();
};