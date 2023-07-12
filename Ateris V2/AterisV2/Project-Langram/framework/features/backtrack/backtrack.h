#pragma once

#include "../../sdk/globals/globalinfo.h"
#include "../menu/vars.h"
#include "../../utils/utils.h"
#include "../../utils/convars/convars.h"
#include "../../utils/entitycache/entitycache.h"

struct backtrack_data
{
	float simtime;
	int lag_ticks;
	vector pos;
	vector headpos;
	vector bodypos;
	float fov_to;
	matrix3x4_t bones[ 128 ];
};

struct backtrack_target
{
	C_TFPlayer* player = nullptr;
	float fov_to = 0.f;
};

class __backtrack
{
	bool is_valid( float simtime );
	bool push_target( C_TFPlayer* local, C_TFWeaponBase* weapon, backtrack_target& out );
	bool push_record( C_TFPlayer* local, C_TFWeaponBase* weapon, backtrack_target target, backtrack_data& out );
public:

	std::unordered_map< C_TFPlayer*, std::deque< backtrack_data > > data;
	std::vector<backtrack_target> targets;
	vector record_angle;
	bool is_backtracking;
	bool drawing_chams;
	backtrack_target best_target{ };
	backtrack_data best_data{ };
	void reset( );
	void fill( );
	backtrack_data get_last( C_TFPlayer* player );
	void run( C_TFPlayer* local, C_TFWeaponBase* weapon, CUserCmd* cmd );
};

inline __backtrack backtrack;