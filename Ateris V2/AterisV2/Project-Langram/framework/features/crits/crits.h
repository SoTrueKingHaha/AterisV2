#pragma once
#include "../../sdk/globals/globalinfo.h"
#include "../menu/vars.h"
#include "../../utils/convars/convars.h"
#include "../../utils/utils.h"

class __crits
{
private:
	bool AreRandomCritsEnabled( );
	void fill( C_TFPlayer* local, C_TFWeaponBase* weapon, const CUserCmd* pCmd, int loops = 10 );
	int FindCritCmd( CUserCmd* cmd, C_TFWeaponBase* weapon, bool bCrit );
	void get_damage_til_unban( );
	void update_damage( C_TFPlayer* local, C_TFWeaponBase* weapon, C_TFPlayerResource* resource );
	bool get_total_crits( C_TFPlayer* local, C_TFWeaponBase* weapon ) const;
	void fix_heavy_rev_bug( CUserCmd* pCmd );
	void compute_can_crit( int cmd_num );
	u32 decrypt_or_encrypt_seed( C_TFPlayer* local, C_TFWeaponBase* weapon, const u32 seed );
	bool is_pure_crit_command( const i32 command_number );
	int LastGoodCritTick( const CUserCmd* pCmd );
	int LastGoodSkipTick( const CUserCmd* pCmd );
	std::vector<int> force_cmds{};
	std::vector<int> skip_cmds{};

public:
	bool weapon_can_crit( C_TFWeaponBase* weapon );
	bool IsAttacking( const CUserCmd* pCmd, C_TFWeaponBase* pWeapon );
	void run( C_TFPlayer* local, C_TFWeaponBase* weapon, CUserCmd* pCmd );
	void reset( );
	bool IsEnabled( );
	bool CalcIsAttackCriticalHandler( C_TFPlayer* local, C_TFWeaponBase* weapon );
	void can_fire_critical_shot_handler( float crit_chance );
	bool racist = false;
	bool can_crit = false;
	bool last_hit_registered_melee = false;
	bool crit_banned = false;
	int total_round_melee_damage = 0;
	int crit_damage_til_unban = 0;
	int boostracist = 0;
	int Crematorium1488 = 0;
	int Yid = 0;
	std::array<unsigned int, 3> damage_correct = { 0, 0, 0 };
	std::array<int, 3> bucket = { 300, 300, 300 };
	float crit_chance_correct = 0.f;
	float crit_mult = 0.f;
	float cost = 0.f;
	int crit_token_bucket = 0;
	std::array<unsigned int, 3> crits = { 0, 0, 0 };
	std::array<unsigned int, 3> potential_crits = { 0, 0, 0 };
	int32_t wish_random_seed = 0;
};

inline __crits crithack;