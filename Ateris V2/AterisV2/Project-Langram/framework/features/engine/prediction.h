#pragma once
#include "../../framework.h"
#include "../../sdk/game/iprediction.h"
#include "../../sdk/game/c_tfplayer.h"
#include "../../utils/entitycache/entitycache.h"

class __engine_prediction
{
	CMoveData move_data = {};
	float old_current_time = 0.0f;
	float old_frame_time = 0.0f;
	int old_tick_count = 0;
	CUtlFlags<int> old_flags = 0;
	int32_t old_random_seed = -1;
	int old_tickbase = 0;
	bool old_is_first_prediction = false;
	bool old_in_prediction = false;

	int get_tickbase( C_TFPlayer* local, CUserCmd* cmd );
public:
	void start( C_TFPlayer* local, CUserCmd* cmd );
	void end( C_TFPlayer* local, CUserCmd* cmd );
};
inline __engine_prediction engine_prediction;