#pragma once
#include "../menu/vars.h"
#include "../../utils/entitycache/entitycache.h"
#include "../../utils/utils.h"

class __misc
{
	void auto_jump( C_TFPlayer* local, C_TFWeaponBase* weapon, CUserCmd* cmd );
	void auto_scout_jump( C_TFPlayer* local, C_TFWeaponBase* weapon, CUserCmd* cmd );
	void auto_strafe( C_TFPlayer* local, CUserCmd* cmd );
	void accurate_movement( C_TFPlayer* local, CUserCmd* cmd );
	void fast_stop( C_TFPlayer* local, CUserCmd* cmd );
	void stop_movement( C_TFPlayer* local, CUserCmd* cmd );
public:
	void dormant_esp( StartSoundParams_t& params );
	bool remove_scope( unsigned int panel );

	bool InThirdperson = false;
	void third_person( C_TFPlayer* local, CViewSetup* pView );
	void fov( C_TFPlayer* local, CViewSetup* pView );
	void draw_prediction( );
	void draw_projectile( );
	void viewmodel( );
	void leg_jitter( CUserCmd* pCmd, C_TFPlayer* pLocal );
	inline static int prev_fov;
	inline static float prev_aspect_ratio;

	void run( CUserCmd* cmd );
	void run_late( CUserCmd* cmd );
};
inline __misc misc;