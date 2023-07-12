#pragma once
#include "../game/ibaseclientdll.h"
#include "../game/c_usercmd.h"
#include "../game/c_tfplayer.h"
#include <unordered_map>

struct dormant_data
{
	vector location;
	float last_update = 0.f;
};

struct vel_fix_record
{
	vector origin;
	int flags;
	float simtime;
};

struct global_info_t
{
	bool scout_jumping = false;
	bool attacking = false;
	bool can_attack = false;
	bool should_stop = false;
	bool shifting = false;
	bool recharging = false;
	bool hitscan_running = false;
	bool hitscan_silent = false;
	bool silent_time = false;
	bool projectile_silent = false; //flamethrower
	bool projectile_should_fire = false;
	bool antiaim_active = false;
	bool drawing_props = false;
	bool drawing_prop = false;
	int shifted = 0;
	int ticks = 0;
	int aimbot_fov = 0;
	float lerp_time = 0.f;
	vector real_angles = { };
	vector punch_angles = { };
	vector predicted_pos = {};
	vector aim_pos = {};
	vector viewangles = {};
	vector real_viewangles = {};
	vector fake_viewangles = {};
	vector projectile_viewangles = { };
	CUserCmd* current_user_cmd = nullptr;
	CUserCmd* shift_user_cmd = nullptr;
	CUserCmd* last_user_cmd = nullptr;
	std::string prop_name = {};
	std::pair<bool, bool> antiaim = {};	//	fakeset, realset
	std::unordered_map<int, dormant_data> dormant_players; // <index, data>
	std::vector<vector> prediction_lines;
	std::vector<vector> projectile_lines;
	std::vector<vector> pred_lines_backup;
	std::unordered_map<int, std::vector<vector>> player_prediction_lines;
	std::unordered_map<C_TFPlayer*, vel_fix_record> vel_fix_records;
};
inline global_info_t global_info;