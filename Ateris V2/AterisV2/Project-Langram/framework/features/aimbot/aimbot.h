#pragma once
#include "../../sdk/globals/globalinfo.h"
#include "../menu/vars.h"
#include "../../utils/utils.h"
#include "../../utils/convars/convars.h"
#include "../../utils/entitycache/entitycache.h"
#include "movesim.h"
#undef max

enum struct ETargetType {
	UNKNOWN, PLAYER, BUILDING, STICKY
};

enum struct ESortMethod {
	UNKNOWN, FOV, DISTANCE
};

struct Target_t
{
	C_BaseEntity* m_pEntity = nullptr;
	ETargetType m_TargetType = ETargetType::UNKNOWN;
	vector m_vPos = {};
	vector m_vAngleTo = {};
	float m_flFOVTo = std::numeric_limits<float>::max( );
};

class __aimbot
{
	std::vector<Target_t> m_vecTargets = {};

	bool should_run( C_TFPlayer* local, C_TFWeaponBase* weapon );
	bool get_targets( C_TFPlayer* local, C_TFWeaponBase* weapon );
	bool push_target( C_TFPlayer* local, C_TFWeaponBase* weapon, Target_t& out );
	void sort_targets( );

	//hitscan
	bool hitscan_should_fire( C_TFPlayer* pLocal, C_TFWeaponBase* pWeapon, CUserCmd* pCmd, const Target_t& Target );
	bool hitscan_verify_target( C_TFPlayer* pLocal, C_TFWeaponBase* pWeapon, Target_t& Target );
	void hitscan_run( C_TFPlayer* pLocal, C_TFWeaponBase* pWeapon, CUserCmd* pCmd );
	void hitscan_aim( CUserCmd* cmd, vector& angle );
	bool hitscan_use_all_player_hitboxes( C_TFPlayer* local, Target_t& target );
	bool hitscan_multipoint_buildings( C_TFPlayer* local, Target_t& target );
	bool hitscan_multipoint_head( C_TFPlayer* local, Target_t& target );
	bool hitscan_multipoint_pelvis( C_TFPlayer* local, Target_t& target );

	//projectile
	struct ProjectileInfo_t
	{
		float m_flVelocity = 0.0f;
		float m_flGravity = 0.0f;
		float m_flMaxTime = 2.0f;
	};

	struct Predictor_t
	{
		C_BaseEntity* m_pEntity = nullptr;
		vector m_vPosition = {};
		vector m_vVelocity = {};
		vector m_vAcceleration = {};
	};

	struct Solution_t
	{
		float m_flPitch = 0.0f;
		float m_flYaw = 0.0f;
		float m_flTime = 0.0f;
	};

	bool GetProjectileInfo( C_TFWeaponBase* pWeapon, ProjectileInfo_t& out );
	bool CalcProjAngle( const vector& vLocalPos, const vector& vTargetPos, const ProjectileInfo_t& ProjInfo, Solution_t& out );
	bool SolveProjectile( C_TFPlayer* pLocal, C_TFWeaponBase* pWeapon, CUserCmd* pCmd, Predictor_t& Predictor, const ProjectileInfo_t& ProjInfo, Solution_t& out );

	vector GetAimPos( C_TFPlayer* pLocal, C_BaseEntity* pEntity, const vector& targetPredPos );
	vector GetAimPosBuilding( C_TFPlayer* pLocal, C_BaseEntity* pEntity );
	bool WillProjectileHit( C_TFPlayer* pLocal, C_TFWeaponBase* pWeapon, CUserCmd* pCmd, const vector& vPredictedPos, Solution_t& out, const ProjectileInfo_t& projInfo, const Predictor_t& predictor );
	ESortMethod GetSortMethod( );
	bool GetTargets( C_TFPlayer* pLocal, C_TFWeaponBase* pWeapon );
	bool VerifyTarget( C_TFPlayer* pLocal, C_TFWeaponBase* pWeapon, CUserCmd* pCmd, Target_t& Target );
	bool GetTarget( C_TFPlayer* pLocal, C_TFWeaponBase* pWeapon, CUserCmd* pCmd, Target_t& Out );
	void Aim( CUserCmd* pCmd, C_TFWeaponBase* pWeapon, vector& vAngle );
	bool ShouldFire( CUserCmd* pCmd, Target_t target );
	void projectile_run( C_TFPlayer* pLocal, C_TFWeaponBase* pWeapon, CUserCmd* pCmd, bool* send_packet );

	bool m_bIsFlameThrower;
	bool m_bIsBoosted;
	int aim_method;

	bool should_detonate( C_TFPlayer* pLocal, C_TFWeaponBase* pWeapon, C_BaseEntity* pExplosive, float radius, CUserCmd* pCmd );
	//melee

public:
	EWeaponType weapon_type = { };
	bool should_backtrack = false;
	bool IsAttacking( const CUserCmd* pCmd, C_TFWeaponBase* pWeapon );
	bool is_key_held( );
	void run( CUserCmd* pCmd, bool* send_packet );
	void detonator_run( C_TFPlayer* pLocal, C_TFWeaponBase* pWeapon, CUserCmd* pCmd );
	void aimbot_psilent( CUserCmd* cmd, const vector& oldview, const float side, const float forward, bool* send_packet );
};
inline __aimbot aimbot;