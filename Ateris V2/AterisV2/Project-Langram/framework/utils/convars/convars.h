#pragma once
#include "../../sdk/game/convar.h"
#include "../entitycache/entitycache.h"

class __convars
{
public:
	ConVar* cl_interp = nullptr;
	ConVar* cl_sidespeed = nullptr;
	ConVar* cl_interp_ratio = nullptr;
	ConVar* cl_updaterate = nullptr;
	ConVar* cl_cmdrate = nullptr;
	ConVar* tf_crit_cap = nullptr;
	ConVar* tf_crit_bottom = nullptr;
	ConVar* tf_crit_default = nullptr;
	ConVar* sv_gravity = nullptr;
	ConVar* cl_flipviewmodels = nullptr;
	ConVar* sv_maxunlag = nullptr;
	ConVar* tf_enable_glows_after_respawn = nullptr;
	ConVar* glow_outline_effect_enable = nullptr;
	ConVar* r_drawspecificstaticprop = nullptr;
	ConVar* sv_namechange_cooldown_seconds = nullptr;
	ConVar* afkTimer = nullptr;
	ConVar* sv_maxusrcmdprocessticks = nullptr;
	ConVar* sv_maxusrcmdprocessticks_holdaim = nullptr;
	ConVar* tf_weapon_criticals_nopred = nullptr;
	ConVar* cl_wpn_sway_interp = nullptr;
	ConVar* viewmodel_fov = nullptr;
	ConVar* r_aspectratio = nullptr;
	ConVar* tf_use_min_viewmodels = nullptr;

	void init( );
	ConVar* find_var( const char* cvarname );
	ConCommand* find_command( const char* commandname );

private:
	std::unordered_map<FNV1A_t, ConVar*> cvarMap;
	std::unordered_map<FNV1A_t, ConCommand*> commandMap;
};

inline __convars convars;