#pragma once
#include "../../utils/draw/draw.h"

class __vars
{
public:
	class __global
	{
	public:
		bool legit_only = false;
		std::string selected_config = "config";
	};
	__global global;
	class __aimbot
	{
	public:
		class __global
		{
		public:
			bool global_enabled = true;
			int aimbot_key = VK_XBUTTON1;
			bool aimbot_autoshoot = true;
			bool aimbot_aimplayers = true;
			bool aimbot_aimbuildings = true;
			bool aimbot_aimstickies = true;
			bool aimbot_ignore_invincible = true;
			bool aimbot_ignore_cloaked = true;
			bool aimbot_ignore_friends = true;
			bool aimbot_ignore_taunting = true;
		};
		__global global;

		class __hitscan
		{
		public:
			bool hitscan_active = true;
			int hitscan_aim = 1; //0 - Normal,	1 - Silent
			int hitscan_hitbox = 1; //0 - Head,	1 - Pelvis
			int hitscan_fov = 60;
			int hitscan_tapfire;
			bool hitscan_wait_headshot = false;
			bool hitscan_multipoint = false;
			bool hitscan_multipoint_head = false;
			float hitscan_multipoint_head_scale = 0.8f;
			bool hitscan_multipoint_pelvis = false;
			float hitscan_multipoint_pelvis_scale = 0.8f;
		};
		__hitscan hitscan;

		class __projectile
		{
		public:
			bool projectile_active = true;
			int projectile_aim_method = 1;	//0 - Normal,	1 - Silent
			int projectile_aim_position = 3;	//0 - Head,		1 - Body,		2 - Feet,	3 - Auto
			int projectile_aim_fov = 60;
			bool projectile_respect_fov = true;
			bool projectile_feet_aim_on_ground = true;
			bool projectile_head_allowed = false;
			bool projectile_body_allowed = true;
			bool projectile_feet_allowed = true;
			bool projectile_charge_loose_cannon = false;
			bool projectile_predicted_path = false;
			float projectile_path_duration = 5.f;
			bool projectile_auto_detonate = true;
			bool projectile_auto_detonate_stickies = true;
			bool projectile_auto_detonate_flares = true;
			bool projectile_detonate_players = true;
			bool projectile_detonate_sentries = true;
			bool projectile_detonate_bombs = true;
			/*bool projectile_strafe_prediction_ground = false;
			bool projectile_strafe_prediction_air = true;*/
		};
		__projectile projectile;
	};
	__aimbot aimbot;

	class __backtrack
	{
	public:
		bool backtrack_active = true;
		bool chams = true;
		bool debug = true;
	};
	__backtrack backtrack;

	class __esp
	{
	public:
		class __global
		{
		public:
			bool esp_global = true;
			bool esp_dormant = true;
			bool esp_enemies = false;
		}; 
		__global global;

		class __players
		{
		public:
			bool esp_players = true;
			bool esp_players_enemy_only = true;
			bool esp_players_friends = true;
			bool esp_player_local = true;
			bool players_box = false;
			bool players_name = true;
			bool players_healthbar = true;
			bool players_custom_healthbar = true;
			bool players_animated_healthbar = true;
			bool players_gradient_healthbar = false;
			bool players_healthbar_lines = false;
			int players_healthbar_lines_count = 0;
			bool players_conds = true;
			bool players_skeleton = false;
			bool players_weapons = true;
			bool players_distance = true;
			bool players_high_kd = true;
			float players_kd = 7.f;
			bool players_high_headshot = true;
			float players_headshot = 0.75f;
			bool players_alert = true;
			float players_screen_distance = 0.25f;
			int players_screen_shape = 1; //0 - Circle		1 - Ellipsis
		}; 
		__players players;

		class __buildings
		{
		public:
			bool esp_buildings = true;
			bool esp_buildings_enemy_only = true;
			bool esp_buildings_friends = true;
			bool esp_buildings_local = true;
			bool buildings_box = true;
			bool buildings_name = true;
			bool buildings_healthbar = true;
			bool buildings_custom_healthbar = true;
			bool buildings_animated_healthbar = true;
			bool buildings_gradient_healthbar = false;
			bool buildings_healthbar_lines = false;
			int buildings_healthbar_lines_count = 0;
			bool buildings_conds = true;
			bool buildings_distance = true;
			bool sentry_alert = true;
			float buildings_screen_distance = 0.25f;
			int buildings_screen_shape = 1; //0 - Circle		1 - Ellipsis
		}; 
		__buildings buildings;

		class __world
		{
		public:
			bool esp_world = true;
			bool esp_world_enemy_only = true;
			bool esp_world_friends = true;
			bool esp_world_local = true;
			bool world_box = true;
			bool world_name = true;
			bool world_distance = true;
			///legit features:
			bool low_health_esp = true;
			bool low_ammo_esp = true;
			bool intel_alert = true;
		}; 
		__world world;
	};
	__esp esp;

	class __models
	{
	public:
		class __global
		{
		public:
			bool models_global = true;
			int models_order = 1;
		};
		__global global;

		class __players
		{
		public:
			bool models_players_enabled = true;
			bool models_players_enemy_only = true;
			bool models_players_friends = true;
			bool models_player_local = true;
			int players_model = 0;
			bool players_ignorez = true;
			bool players_overlay = true;
			float players_overlay_alpha = 1.0f;
		};
		__players players;

		class __buildings
		{
		public:
			bool models_buildings_enabled = true;
			bool models_buildings_enemy_only = true;
			bool models_buildings_friends = true;
			bool models_buildings_local = true;
			int buildings_model = 0;
			bool buildings_ignorez = true;
			bool buildings_overlay = true;
			float buildings_overlay_alpha = 1.0f;
		};
		__buildings buildings;

		class __world
		{
		public:
			bool models_world_enabled = true;
			bool models_world_enemy_only = true;
			bool models_world_friends = true;
			bool models_world_local = true;
			int world_model = 0;
			bool world_ignorez = true;
			bool world_overlay = true;
			float world_overlay_alpha = 1.0f;
		};
		__world world;
	};
	__models models;

	class __outlines
	{
	public:
		class __global
		{
		public:
			bool outlines_global = true;
			bool blur_outline = true;
			bool wireframe_outline = true;
			int outline_scale = 2;
		};
		__global global;

		class __players
		{
		public:
			bool outlines_players_enabled = true;
			bool outlines_players_enemy_only = true;
			bool outlines_players_friends = true;
			bool outlines_player_local = true;
			//bool viewmodel_outline = true;
			bool players_attachments_outline = true;
		};
		__players players;

		class __buildings
		{
		public:
			bool outlines_buildings_enabled = true;
			bool outlines_buildings_enemy_only = true;
			bool outlines_buildings_friends = true;
			bool outlines_buildings_local = true;
		};
		__buildings buildings;

		class __world
		{
		public:
			bool outlines_world_enabled = true;
			bool outlines_world_enemy_only = true;
			bool outlines_world_friends = true;
			bool outlines_world_local = true;
		};
		__world world;
	};
	__outlines outlines;

	class __misc
	{
	public:
		bool auto_jump = true;
		bool break_jump = true;
		bool tomislav_rev = true;
		int auto_strafe = 2;
		bool accurate_movement = true;
		bool auto_scout_jump = true;
		int scout_jump_key = 0x0;
		bool disable_interp = true;

		class __chatlog
		{
		public:
			bool log_active = true;
			bool log_connects = true;
			bool log_disconnects = true;
			bool log_class_changes = true;
			bool log_name_changes = true;
			bool log_votes = true;
		};
		__chatlog chatlog;

		class __crits
		{
		public:
			bool crits_active = true;
			bool avoid_random = true;
			int crit_key = 0x52;
			bool crits_indicator = true;
		};
		__crits crits;

		class __cl_move
		{
		public:
			bool doubletap = true;
			bool antiwarp = true;
			bool warp = true;
			bool smooth_warp = true;
			bool dt_indicator = true;
			int recharge_key = 0x0;
			int warp_key = 0x0;
		};
		__cl_move cl_move;
	};
	__misc misc;

	class __antiaim
	{
	public:
		bool antiaim_active = false;
		bool fake_chams = false;
		bool rage = false;
		int toggle_key = 0;
		//legit
		int legit_yaw = 0;
		int legit_yaw_mode = 0;// 0 - Static,	1 - Edge
		int cycle_key = 0;
		//rage
		int fake_yaw = 0;			// -180 / 180
		int fake_mode = 0;		// 0 - Static,	1 - Edge,	2 - Jitter,	3 - Spin
		int fake_rotate = 3;
		int fake_delta = 10;
		int fake_spin = 3;
		int fake_angle1 = 74;
		int fake_angle2 = 104;
		int cycle_fake_key = 0;

		int real_yaw = 0;			// -180 / 180
		int real_mode = 0;		// 0 - Static,	1 - Edge,	2 - Jitter,	3 - Spin
		int real_rotate = 3;
		int real_delta = 10;
		int real_spin = 3;
		int real_angle1 = 74;
		int real_angle2 = 104;
		int cycle_real_key = 0;

		bool flat_material = false;

		int pitch = 0;			// 0 - None,	1 - Automatic,	2 - Manual
		bool fake_pitch = false;
		int real_pitch = 0;		// 0 - Up,		1 - Down,	2 - Jitter
	};
	__antiaim antiaim;

	class __visuals
	{
	public:
		int visuals_thirdperson_key = VK_XBUTTON2;
		bool visuals_thirdperson_offset = false;
		bool visuals_remove_scope = false;
		int visuals_thirdperson_offset_forwards = 150;
		int visuals_thirdperson_offset_sideways = 0;
		int visuals_thirdperson_offset_upwards = 0;
		int visuals_fov = 90;
		int visuals_tracer = 2;
		bool custom_beam = false;
		float beam_time = 1.0f;
		float beam_size = 4.0f;
		float aspect_ratio = 0.f;
		bool bypass_svpure = true;

		bool ragdolls_active = true;
		bool ragdolls_enemy_only = false;
		int ragdolls_particle = 0;
		int ragdolls_effect = 0;
		bool ragdolls_ash = false;
		bool ragdolls_dissolve = false;

		bool viewmodel_sway = false;
		bool viewmodel_pos = false;
		bool viewmodel_flip = false;
		bool viewmodel_min = true;
		int viewmodel_fov = 0;
		int viewmodel_roll = 0;
		int viewmodel_pos_x = 0;
		int viewmodel_pos_y = 0;
		int viewmodel_pos_z = 0;
	};
	__visuals visuals;

	class __testmenu
	{
	public:
		bool testbool = false;
		int testint = 0;
		float testfloat = 0.f;
		int testkey = 0x0;
		int testcombo = 0;
		color testcolor = { 255, 255, 255, 255 };
		bool testbox1 = false;
		bool testbox2 = false;
		bool testbox3 = false;
		bool testbox4 = false;
		std::string teststring = { };
	};
	__testmenu testmenu;

	class __colors
	{
	public:
		class __esp
		{
		public:
			class __players
			{
			public:
				int esp_player_color_scheme = 0; //0 - Enemy/Team,	1 - Red/Blu
				color esp_local_player = { 130, 200, 40, 180 };
				color esp_player_default = { 200, 200, 200, 180 };

				color esp_player_red = { 255, 107, 107, 180 };
				color esp_player_blu = { 72, 219, 251, 180 };

				color esp_player_enemy = { 240, 180, 60, 180 };
				color esp_player_teammate = { 120, 240, 120, 180 };

				color player_dormant = { 170, 170, 170, 180 };
				color player_cloaked = { 150, 150, 150, 180 };
				color player_friend = { 0, 200, 255, 180 };
				color player_suspicious = { 255, 150, 0, 180 };
				color player_cheater = { 255, 55, 0, 180 };
				color player_conds_overheal = { 150, 255, 150, 180 };
				color player_conds_cloaked = { 200, 200, 200, 180 };
				color player_conds_uber = { 255, 110, 255, 180 };
				color player_conds_crit = { 255, 150, 255, 180 };
				color player_conds_highkd = { 255, 255, 0, 180 };

				color player_healthbar_top = { 128, 200, 0, 255 };
				color player_healthbar_bottom = { 240, 80, 60, 255 };
			};
			__players players;

			class __buildings
			{
			public:
				int esp_building_color_scheme = 0; //0 - Enemy/Team,	1 - Red/Blu
				color esp_local_building = { 130, 200, 40, 180 };
				color esp_building_default = { 200, 200, 200, 180 };

				color esp_building_red = { 255, 107, 107, 180 };
				color esp_building_blu = { 72, 219, 251, 180 };

				color esp_building_enemy = { 240, 180, 60, 180 };
				color esp_building_teammate = { 120, 240, 120, 180 };

				color building_healthbar_top = { 128, 200, 0, 255 };
				color building_healthbar_bottom = { 240, 80, 60, 255 };
			};
			__buildings buildings;

			class __world
			{
			public:
				int esp_world_color_scheme = 0; //0 - Enemy/Team,	1 - Red/Blu
				color esp_local_world = { 130, 200, 40, 180 };
				color esp_world_default = { 200, 200, 200, 180 };

				color esp_world_ammo = { 255, 255, 255, 180 };
				color esp_world_health = { 55, 200, 55, 180 };
				color esp_world_bomb = { 255, 75, 75, 255 };

				color esp_world_red = { 255, 107, 107, 180 };
				color esp_world_blu = { 72, 219, 251, 180 };

				color esp_world_enemy = { 240, 180, 60, 180 };
				color esp_world_teammate = { 120, 240, 120, 180 };
			};
			__world world;
		};
		__esp esp;

		class __models
		{
		public:
			class __players
			{
			public:
				int models_player_color_scheme = 0; //0 - Enemy/Team,	1 - Red/Blu
				color models_local_player = { 130, 200, 40, 180 };
				color models_player_default = { 200, 200, 200, 180 };
				color models_player_fresnel = { 0, 0, 0, 180 };
				color models_player_cloaked = { 150, 150, 150, 180 };

				color models_player_red = { 255, 107, 107, 180 };
				color models_player_blu = { 72, 219, 251, 180 };

				color models_player_enemy = { 240, 180, 60, 180 };
				color models_player_teammate = { 120, 240, 120, 180 };
			};
			__players players;

			class __buildings
			{
			public:
				int models_building_color_scheme = 0; //0 - Enemy/Team,	1 - Red/Blu
				color models_local_building = { 130, 200, 40, 180 };
				color models_building_default = { 200, 200, 200, 180 };
				color models_building_fresnel = { 0, 0, 0, 180 };

				color models_building_red = { 255, 107, 107, 180 };
				color models_building_blu = { 72, 219, 251, 180 };

				color models_building_enemy = { 240, 180, 60, 180 };
				color models_building_teammate = { 120, 240, 120, 180 };
			};
			__buildings buildings;

			class __world
			{
			public:
				int models_world_color_scheme = 0; //0 - Enemy/Team,	1 - Red/Blu
				color models_local_world = { 130, 200, 40, 180 };
				color models_world_default = { 200, 200, 200, 180 };
				color models_world_fresnel = { 0, 0, 0, 180 };

				color models_world_ammo = { 255, 255, 255, 180 };
				color models_world_health = { 55, 200, 55, 180 };
				color models_world_bomb = { 255, 75, 75, 255 };

				color models_world_red = { 255, 107, 107, 180 };
				color models_world_blu = { 72, 219, 251, 180 };

				color models_world_enemy = { 240, 180, 60, 180 };
				color models_world_teammate = { 120, 240, 120, 180 };
			};
			__world world;
		};
		__models models;

		class __outlines
		{
		public:
			class __players
			{
			public:
				int outlines_player_color_scheme = 0; //0 - Enemy/Team,	1 - Red/Blu
				color outlines_local_player = { 130, 200, 40, 180 };
				color outlines_player_default = { 200, 200, 200, 180 };
				color outlines_player_cloaked = { 150, 150, 150, 180 };

				color outlines_player_red = { 255, 107, 107, 180 };
				color outlines_player_blu = { 72, 219, 251, 180 };

				color outlines_player_enemy = { 240, 180, 60, 180 };
				color outlines_player_teammate = { 120, 240, 120, 180 };
			};
			__players players;

			class __buildings
			{
			public:
				int outlines_building_color_scheme = 0; //0 - Enemy/Team,	1 - Red/Blu
				color outlines_local_building = { 130, 200, 40, 180 };
				color outlines_building_default = { 200, 200, 200, 180 };

				color outlines_building_red = { 255, 107, 107, 180 };
				color outlines_building_blu = { 72, 219, 251, 180 };

				color outlines_building_enemy = { 240, 180, 60, 180 };
				color outlines_building_teammate = { 120, 240, 120, 180 };
			};
			__buildings buildings;

			class __world
			{
			public:
				int outlines_world_color_scheme = 0; //0 - Enemy/Team,	1 - Red/Blu
				color outlines_local_world = { 130, 200, 40, 180 };
				color outlines_world_default = { 200, 200, 200, 180 };

				color outlines_world_ammo = { 255, 255, 255, 180 };
				color outlines_world_health = { 55, 200, 55, 180 };
				color outlines_world_bomb = { 255, 75, 75, 255 };

				color outlines_world_red = { 255, 107, 107, 180 };
				color outlines_world_blu = { 72, 219, 251, 180 };

				color outlines_world_enemy = { 240, 180, 60, 180 };
				color outlines_world_teammate = { 120, 240, 120, 180 };
			};
			__world world;
		};
		__outlines outlines;

		class __antiaim
		{
		public:
			color local_fake = { 130, 200, 40, 128 };
		};
		__antiaim antiaim;

		class __misc
		{
		public:
			color beam_red = { 255, 0, 0, 255 };
			color beam_blu = { 50, 150, 255, 255 };
			color beam_custom = { 130, 200, 40, 255 };
		};
		__misc misc;

		class __testmenu
		{
		public:
			color menu_background = { 40, 40, 40, 255 };
			color menu_background2 = { 28, 28, 28, 255 };
			color menu_groupbox = { 40, 40, 40, 255 };
			color menu_widget = { 60, 60, 60, 255 };
			color menu_hovered = { 45, 45, 45, 255 };
			color menu_accent = { 50, 180, 255, 255 };
			color menu_selected = { 40, 120, 170, 255 };
			color menu_clear = { 255, 255, 255, 255 };
		};
		__testmenu menu;
		//color menu_accent = { 153, 119, 238, 255 };
		//color menu_selected = { 153, 119, 238, 100 };
	};
	__colors colors;

	class __debug
	{
	public:
		bool crit = false;
		std::string map = "tr_walkway_rc3a";
	};
	__debug debug;

	void init( );
};
inline __vars vars;