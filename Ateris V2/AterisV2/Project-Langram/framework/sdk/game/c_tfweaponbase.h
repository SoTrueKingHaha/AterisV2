#pragma once

#include "icollideable.h"
#include "C_TFPlayer.h"
#include "C_BaseCombatWeapon.h"
#include "GameEventListener.h"

typedef enum
{
	EMPTY,
	SINGLE,
	SINGLE_NPC,
	WPN_DOUBLE, // Can't be "DOUBLE" because windows.h uses it.
	DOUBLE_NPC,
	BURST,
	RELOAD,
	RELOAD_NPC,
	MELEE_MISS,
	MELEE_HIT,
	MELEE_HIT_WORLD,
	SPECIAL1,
	SPECIAL2,
	SPECIAL3,
	TAUNT,
	DEPLOY,

	// Add new shoot sound types here

	NUM_SHOOT_SOUND_TYPES,
} WeaponSound_t;

#define MAX_SHOOT_SOUNDS			16
#define MAX_WEAPON_STRING			80
#define MAX_WEAPON_PREFIX			16
#define MAX_WEAPON_AMMO_NAME		32
#define WEAPON_PRINTNAME_MISSING	"!!! Missing printname on weapon"

struct CHudTexture;

class FileWeaponInfo_t
{
public:

	FileWeaponInfo_t();

	// Each game can override this to get whatever values it wants from the script.
	virtual void Parse(KeyValues* pKeyValuesData, const char* szWeaponName);


public:
	bool					bParsedScript;
	bool					bLoadedHudElements;

	// SHARED
	char					szClassName[MAX_WEAPON_STRING];
	char					szPrintName[MAX_WEAPON_STRING];			// Name for showing in HUD, etc.

	char					szViewModel[MAX_WEAPON_STRING];			// View model of this weapon
	char					szWorldModel[MAX_WEAPON_STRING];		// Model of this weapon seen carried by the player
	char					szAnimationPrefix[MAX_WEAPON_PREFIX];	// Prefix of the animations that should be used by the player carrying this weapon
	int						iSlot;									// inventory slot.
	int						iPosition;								// position in the inventory slot.
	int						iMaxClip1;								// max primary clip size (-1 if no clip)
	int						iMaxClip2;								// max secondary clip size (-1 if no clip)
	int						iDefaultClip1;							// amount of primary ammo in the gun when it's created
	int						iDefaultClip2;							// amount of secondary ammo in the gun when it's created
	int						iWeight;								// this value used to determine this weapon's importance in autoselection.
	int						iRumbleEffect;							// Which rumble effect to use when fired? (xbox)
	bool					bAutoSwitchTo;							// whether this weapon should be considered for autoswitching to
	bool					bAutoSwitchFrom;						// whether this weapon can be autoswitched away from when picking up another weapon or ammo
	int						iFlags;									// miscellaneous weapon flags
	char					szAmmo1[MAX_WEAPON_AMMO_NAME];			// "primary" ammo type
	char					szAmmo2[MAX_WEAPON_AMMO_NAME];			// "secondary" ammo type

	// Sound blocks
	char					aShootSounds[NUM_SHOOT_SOUND_TYPES][MAX_WEAPON_STRING];

	int						iAmmoType;
	int						iAmmo2Type;
	bool					m_bMeleeWeapon;		// Melee weapons can always "fire" regardless of ammo.

	// This tells if the weapon was built right-handed (defaults to true).
	// This helps cl_righthand make the decision about whether to flip the model or not.
	bool					m_bBuiltRightHanded;
	bool					m_bAllowFlipping;	// False to disallow flipping the model, regardless of whether
												// it is built left or right handed.

// CLIENT DLL
	// Sprite data, read from the data file
	int						iSpriteCount;
	CHudTexture* iconActive;
	CHudTexture* iconInactive;
	CHudTexture* iconAmmo;
	CHudTexture* iconAmmo2;
	CHudTexture* iconCrosshair;
	CHudTexture* iconAutoaim;
	CHudTexture* iconZoomedCrosshair;
	CHudTexture* iconZoomedAutoaim;
	CHudTexture* iconSmall;

	// TF2 specific
	bool					bShowUsageHint;							// if true, then when you receive the weapon, show a hint about it

// SERVER DLL

};

struct WeaponData_t
{
	int		m_nDamage;
	int		m_nBulletsPerShot;
	float	m_flRange;
	float	m_flSpread;
	float	m_flPunchAngle;
	float	m_flTimeFireDelay;				// Time to delay between firing
	float	m_flTimeIdle;					// Time to idle after firing
	float	m_flTimeIdleEmpty;				// Time to idle after firing last bullet in clip
	float	m_flTimeReloadStart;			// Time to start into a reload (ie. shotgun)
	float	m_flTimeReload;					// Time to reload
	bool	m_bDrawCrosshair;				// Should the weapon draw a crosshair
	int		m_iProjectile;					// The type of projectile this mode fires
	int		m_iAmmoPerShot;					// How much ammo each shot consumes
	float	m_flProjectileSpeed;			// Start speed for projectiles (nail, etc.); NOTE: union with something non-projectile
	float	m_flSmackDelay;					// how long after swing should damage happen for melee weapons
	bool	m_bUseRapidFireCrits;

	void Init(void)
	{
		m_nDamage = 0;
		m_nBulletsPerShot = 0;
		m_flRange = 0.0f;
		m_flSpread = 0.0f;
		m_flPunchAngle = 0.0f;
		m_flTimeFireDelay = 0.0f;
		m_flTimeIdle = 0.0f;
		m_flTimeIdleEmpty = 0.0f;
		m_flTimeReloadStart = 0.0f;
		m_flTimeReload = 0.0f;
		m_iProjectile = 0;
		m_iAmmoPerShot = 0;
		m_flProjectileSpeed = 0.0f;
		m_flSmackDelay = 0.0f;
		m_bUseRapidFireCrits = false;
	};
};

class CTFWeaponInfo : public FileWeaponInfo_t
{
public:

	CTFWeaponInfo();
	~CTFWeaponInfo();

	virtual void Parse(::KeyValues* pKeyValuesData, const char* szWeaponName);

	WeaponData_t const& GetWeaponData(int iWeapon) const { return m_WeaponData[iWeapon]; }

public:

	WeaponData_t	m_WeaponData[2];

	int		m_iWeaponType;

	// Grenade.
	bool	m_bGrenade;
	float	m_flDamageRadius;
	float	m_flPrimerTime;
	bool	m_bLowerWeapon;
	bool	m_bSuppressGrenTimer;

	// Skins
	bool	m_bHasTeamSkins_Viewmodel;
	bool	m_bHasTeamSkins_Worldmodel;

	// Muzzle flash
	char	m_szMuzzleFlashModel[128];
	float	m_flMuzzleFlashModelDuration;
	char	m_szMuzzleFlashParticleEffect[128];

	// Tracer
	char	m_szTracerEffect[128];

	// Eject Brass
	bool	m_bDoInstantEjectBrass;
	char	m_szBrassModel[128];

	// Explosion Effect
	char	m_szExplosionSound[128];
	char	m_szExplosionEffect[128];
	char	m_szExplosionPlayerEffect[128];
	char	m_szExplosionWaterEffect[128];

	bool	m_bDontDrop;
};

struct TF2HudTexture_t
{
	void* pDestructor;
	char			_shortname[64];
	char			_spritename[64];
	unsigned char	_RenderUsingFont;
	unsigned char	_Precached;
	unsigned char	_CharacterInFont;
	unsigned char	pad;
	unsigned long	_hFont;
	int				_textureId;
	float			_textcoords[4];
	int				_rect[4];
};

class IHasOwner
{
public:
	virtual C_BaseEntity* GetOwnerViaInterface(void) = 0;
};


class C_TFWeaponBase : public C_BaseCombatWeapon, public IHasOwner, public CGameEventListener
{
public:
	NETVAR(m_bLowered, bool, "CTFWeaponBase", "m_bLowered");
	NETVAR(m_iReloadMode, int, "CTFWeaponBase", "m_iReloadMode");
	NETVAR(m_bResetParity, bool, "CTFWeaponBase", "m_bResetParity");
	NETVAR(m_bReloadedThroughAnimEvent, bool, "CTFWeaponBase", "m_bReloadedThroughAnimEvent");
	NETVAR(m_bDisguiseWeapon, bool, "CTFWeaponBase", "m_bDisguiseWeapon");
	NETVAR(m_flReloadPriorNextFire, float, "CTFWeaponBase", "m_flReloadPriorNextFire");
	NETVAR(m_flLastFireTime, float, "CTFWeaponBase", "m_flLastFireTime");
	NETVAR(m_flEffectBarRegenTime, float, "CTFWeaponBase", "m_flEffectBarRegenTime");
	NETVAR(m_flEnergy, float, "CTFWeaponBase", "m_flEnergy");
	NETVAR(m_hExtraWearable, int, "CTFWeaponBase", "m_hExtraWearable");
	NETVAR(m_hExtraWearableViewModel, int, "CTFWeaponBase", "m_hExtraWearableViewModel");
	NETVAR(m_bBeingRepurposedForTaunt, bool, "CTFWeaponBase", "m_bBeingRepurposedForTaunt");
	NETVAR(m_nKillComboClass, int, "CTFWeaponBase", "m_nKillComboClass");
	NETVAR(m_nKillComboCount, int, "CTFWeaponBase", "m_nKillComboCount");
	NETVAR(m_flInspectAnimEndTime, float, "CTFWeaponBase", "m_flInspectAnimEndTime");
	NETVAR(m_nInspectStage, int, "CTFWeaponBase", "m_nInspectStage");
	NETVAR(m_iConsecutiveShots, int, "CTFWeaponBase", "m_iConsecutiveShots");
	NETVAR(m_iItemDefinitionIndex, int, "CEconEntity", "m_iItemDefinitionIndex");
	NETVAR_OFFSET_FROM(m_flSmackTime, float, "CTFWeaponBase", "m_nInspectStage", 0x1C);
	//NETVAR_OFFSET_FROM( m_flCritTime, float, "CTFWeaponBase", "m_flLastCritCheckTime", -0x4 );
	NETVAR_OFFSET_FROM(m_pWeaponInfo, CTFWeaponInfo*, "CTFWeaponBase", "m_flReloadPriorNextFire", 4);

	NETVAR(m_flLastCritCheckTime, float, "CTFWeaponBase", "m_flLastCritCheckTime");
	NETVAR(m_flObservedCritChance, float, "CTFWeaponBase", "m_flObservedCritChance");
	inline void SetObservedCritChance( float crit_chance )
	{
		static auto offset = netvars.find_netvar( "CTFWeaponBase", "m_flObservedCritChance" );
		*reinterpret_cast< float* >( reinterpret_cast< DWORD >( this ) + offset ) = crit_chance;
	}
	NETVAR_OFFSET_PURE(m_iWeaponMode, int, 716);
	inline void SetWeaponMode( int mode )
	{
		*reinterpret_cast< int* >( reinterpret_cast< DWORD >( this ) + 716 ) = mode;
	}
	NETVAR_OFFSET_PURE(m_iCurrentSeed, int, 727);
	NETVAR_OFFSET_PURE(m_flCritTokenBucket, float, 2644);
	NETVAR_OFFSET_PURE(m_nCritChecks, int, 2648);
	NETVAR_OFFSET_PURE(m_nCritSeedRequests, int, 2652);
	NETVAR_OFFSET_PURE(m_flCritTime, float, 2896);
	NETVAR_OFFSET_PURE(m_flLastRapidFireCritCheckTime, float, 2912);

	NETVAR_OFFSET_PURE(m_bFlipViewModel, bool, 2776);

	int GetSlot();
	bool InReload();
	const char* GetName();
	int GetWeaponID();
	int GetDamageType();
	WeaponData_t GetWeaponData(int m_iWeaponMode = 0);
	CTFWeaponInfo* GetWeaponInfo();
	float GetWeaponSpread();
	float GetSwingRange(C_TFPlayer* entity);
	bool DoSwingTrace(trace_t* trace);
	bool CalcIsAttackCriticalHelperNoCrits();
	bool AreRandomCritsEnabled();
	bool CalcIsAttackCritical();
	bool CalcIsAttackCriticalHelper();
	bool CalcIsAttackCriticalHelperMelee();
	bool CanFireRandomCriticalShot(const float flCritChance);
	bool CanShoot();
	bool CanSecondaryAttack();
	void GetProjectileFireSetup(C_TFPlayer* pPlayer, vector vecOffset, vector* vecSrc, QAngle* angForward, bool bHitTeammates, float flEndDist);
	EWeaponType GetWeaponType();
	bool CanFireCriticalShot(const bool bHeadshot);
	bool CanWeaponHeadShot();
	bool WillCrit( );
	bool WithdrawFromCritbucket( float damage );

	bool IsAttacking(CUserCmd* pCmd);

	int GetReserveAmmo( );
	int GetMaxAmmo( );
	int GetBulletsAmount( );
	bool IsStreamingWeapon( );

	inline bool IsAmmoCritical( )
	{
		return GetReserveAmmo( ) < ( GetMaxAmmo( ) * 0.4f );
	}
};

class C_TFPipebombLauncher : public C_TFWeaponBase
{
public:
	NETVAR(m_iPipebombCount, int, "CTFPipebombLauncher", "m_iPipebombCount");
	NETVAR(m_flChargeBeginTime, float, "CTFPipebombLauncher", "m_flChargeBeginTime");
};

// Class ID: 352
class CWeaponMedigun : public C_TFWeaponBase
{
public:
	NETVAR(m_hHealingTarget, EHANDLE, "CWeaponMedigun", "m_hHealingTarget"); //3144
	NETVAR(m_bHealing, bool, "CWeaponMedigun", "m_bHealing"); //3153
	NETVAR(m_bAttacking, bool, "CWeaponMedigun", "m_bAttacking"); //3154
	NETVAR(m_bChargeRelease, bool, "CWeaponMedigun", "m_bChargeRelease"); //3177
	NETVAR(m_bHolstered, bool, "CWeaponMedigun", "m_bHolstered"); //3176
	NETVAR(m_nChargeResistType, int, "CWeaponMedigun", "m_nChargeResistType"); //3184
	NETVAR(m_hLastHealingTarget, EHANDLE, "CWeaponMedigun", "m_hLastHealingTarget"); //3148
	NETVAR(m_flChargeLevel, float, "CWeaponMedigun", "m_flChargeLevel"); //3180
};

class CTFMinigun : public C_TFWeaponBase
{
public:
	NETVAR( m_iWeaponState, int, "CTFMinigun", "m_iWeaponState" );
	NETVAR( m_bCritShot, bool, "CTFMinigun", "m_bCritShot" );
};

class CTFKnife : public C_TFWeaponBase
{
public:
	NETVAR( m_bReadyToBackstab, bool, "CTFKnife", "m_bReadyToBackstab" );
	NETVAR( m_bKnifeExists, bool, "CTFKnife", "m_bKnifeExists" );
	NETVAR( m_flKnifeRegenerateDuration, float, "CTFKnife", "m_flKnifeRegenerateDuration" );
	NETVAR( m_flKnifeMeltTimestamp, float, "CTFKnife", "m_flKnifeMeltTimestamp" );
};

class CTFPipebombLauncher : public C_TFWeaponBase
{
public:
	NETVAR( m_iPipebombCount, int, "CTFPipebombLauncher", "m_iPipebombCount" );
	NETVAR( m_flChargeBeginTime, float, "CTFPipebombLauncher", "m_flChargeBeginTime" );
};

class CTFGrenadeLauncher : public C_TFWeaponBase
{
public:
	NETVAR( m_flDetonateTime, float, "CTFGrenadeLauncher", "m_flDetonateTime" )
};

class CTFSniperRifle : public C_TFWeaponBase
{
public:
	NETVAR( m_flChargedDamage, float, "CTFSniperRifle", "m_flChargedDamage" );
};