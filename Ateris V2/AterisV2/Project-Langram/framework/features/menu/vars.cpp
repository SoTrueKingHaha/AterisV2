#include "vars.h"
#include "config/config.h"
#pragma warning( disable: 4003 )

void __vars::init( )
{
	CFGVAR( vars.aimbot.global, global_enabled );
	CFGVAR( vars.aimbot.global, aimbot_key );
	CFGVAR( vars.aimbot.global, aimbot_autoshoot );
	CFGVAR( vars.aimbot.global, aimbot_aimplayers );
	CFGVAR( vars.aimbot.global, aimbot_aimbuildings );
	CFGVAR( vars.aimbot.global, aimbot_ignore_invincible );
	CFGVAR( vars.aimbot.global, aimbot_ignore_cloaked );
	CFGVAR( vars.aimbot.global, aimbot_ignore_friends );
	CFGVAR( vars.aimbot.global, aimbot_ignore_taunting );

	CFGVAR( vars.aimbot.hitscan, hitscan_active );
	CFGVAR( vars.aimbot.hitscan, hitscan_aim ); //0 - Normal,	1 - Silent
	CFGVAR( vars.aimbot.hitscan, hitscan_hitbox ); //0 - Head,	1 - Pelvis
	CFGVAR( vars.aimbot.hitscan, hitscan_fov );
	CFGVAR( vars.aimbot.hitscan, hitscan_tapfire );
	CFGVAR( vars.aimbot.hitscan, hitscan_wait_headshot );
	CFGVAR( vars.aimbot.hitscan, hitscan_multipoint );
	CFGVAR( vars.aimbot.hitscan, hitscan_multipoint_head );
	CFGVAR( vars.aimbot.hitscan, hitscan_multipoint_head_scale );
	CFGVAR( vars.aimbot.hitscan, hitscan_multipoint_pelvis );
	CFGVAR( vars.aimbot.hitscan, hitscan_multipoint_pelvis_scale );

	CFGVAR( vars.aimbot.projectile, projectile_active );
	CFGVAR( vars.aimbot.projectile, projectile_aim_method );
	CFGVAR( vars.aimbot.projectile, projectile_aim_position );
	CFGVAR( vars.aimbot.projectile, projectile_aim_fov );
	CFGVAR( vars.aimbot.projectile, projectile_respect_fov );
	CFGVAR( vars.aimbot.projectile, projectile_feet_aim_on_ground );
	CFGVAR( vars.aimbot.projectile, projectile_head_allowed );
	CFGVAR( vars.aimbot.projectile, projectile_body_allowed );
	CFGVAR( vars.aimbot.projectile, projectile_feet_allowed );
	CFGVAR( vars.aimbot.projectile, projectile_charge_loose_cannon );
	CFGVAR( vars.aimbot.projectile, projectile_predicted_path );
	CFGVAR( vars.aimbot.projectile, projectile_path_duration );
	CFGVAR( vars.aimbot.projectile, projectile_auto_detonate );
	CFGVAR( vars.aimbot.projectile, projectile_auto_detonate_stickies );
	CFGVAR( vars.aimbot.projectile, projectile_auto_detonate_flares );
	CFGVAR( vars.aimbot.projectile, projectile_detonate_players );
	CFGVAR( vars.aimbot.projectile, projectile_detonate_sentries );
	CFGVAR( vars.aimbot.projectile, projectile_detonate_bombs );


	CFGVAR( vars.backtrack, backtrack_active );
	CFGVAR( vars.backtrack, chams );
	CFGVAR( vars.backtrack, debug );


	CFGVAR( vars.esp.global, esp_global );
	CFGVAR( vars.esp.global, esp_dormant );
	CFGVAR( vars.esp.global, esp_enemies );


	CFGVAR( vars.esp.players, esp_players );
	CFGVAR( vars.esp.players, esp_players_friends );
	CFGVAR( vars.esp.players, esp_player_local );
	CFGVAR( vars.esp.players, esp_players_enemy_only );
	CFGVAR( vars.esp.players, players_box );
	CFGVAR( vars.esp.players, players_name );
	CFGVAR( vars.esp.players, players_healthbar );
	CFGVAR( vars.esp.players, players_custom_healthbar );
	CFGVAR( vars.esp.players, players_animated_healthbar );
	CFGVAR( vars.esp.players, players_gradient_healthbar );
	CFGVAR( vars.esp.players, players_healthbar_lines_count );
	CFGVAR( vars.esp.players, players_conds );
	CFGVAR( vars.esp.players, players_skeleton );
	CFGVAR( vars.esp.players, players_weapons );
	CFGVAR( vars.esp.players, players_distance );
	CFGVAR( vars.esp.players, players_alert );
	CFGVAR( vars.esp.players, players_screen_distance );
	CFGVAR( vars.esp.players, players_screen_shape );
	///legit features:
	CFGVAR( vars.esp.players, players_kd );
	CFGVAR( vars.esp.players, players_high_kd );
	CFGVAR( vars.esp.players, players_headshot );
	CFGVAR( vars.esp.players, players_high_headshot );


	CFGVAR( vars.esp.buildings, esp_buildings );
	CFGVAR( vars.esp.buildings, esp_buildings_friends );
	CFGVAR( vars.esp.buildings, esp_buildings_local );
	CFGVAR( vars.esp.buildings, esp_buildings_enemy_only );
	CFGVAR( vars.esp.buildings, buildings_box );
	CFGVAR( vars.esp.buildings, buildings_name );
	CFGVAR( vars.esp.buildings, buildings_healthbar );
	CFGVAR( vars.esp.buildings, buildings_custom_healthbar );
	CFGVAR( vars.esp.buildings, buildings_animated_healthbar );
	CFGVAR( vars.esp.buildings, buildings_gradient_healthbar );
	CFGVAR( vars.esp.buildings, buildings_healthbar_lines_count );
	CFGVAR( vars.esp.buildings, buildings_conds );
	CFGVAR( vars.esp.buildings, buildings_distance );
	CFGVAR( vars.esp.buildings, sentry_alert );
	CFGVAR( vars.esp.buildings, buildings_screen_distance );
	CFGVAR( vars.esp.buildings, buildings_screen_shape );


	CFGVAR( vars.esp.world, esp_world );
	CFGVAR( vars.esp.world, esp_world_friends );
	CFGVAR( vars.esp.world, esp_world_local );
	CFGVAR( vars.esp.world, esp_world_enemy_only );
	CFGVAR( vars.esp.world, world_box );
	CFGVAR( vars.esp.world, world_name );
	CFGVAR( vars.esp.world, world_distance );
	///legit features:
	CFGVAR( vars.esp.world, low_health_esp );
	CFGVAR( vars.esp.world, low_ammo_esp );
	CFGVAR( vars.esp.world, intel_alert );


	CFGVAR( vars.models.global, models_global );
	CFGVAR( vars.models.global, models_order );

	CFGVAR( vars.models.players, models_players_enabled );
	CFGVAR( vars.models.players, models_players_friends );
	CFGVAR( vars.models.players, models_player_local );
	CFGVAR( vars.models.players, models_players_enemy_only );
	CFGVAR( vars.models.players, players_model );
	CFGVAR( vars.models.players, players_ignorez );
	CFGVAR( vars.models.players, players_overlay );
	CFGVAR( vars.models.players, players_overlay_alpha );

	CFGVAR( vars.models.buildings, models_buildings_enabled );
	CFGVAR( vars.models.buildings, models_buildings_friends );
	CFGVAR( vars.models.buildings, models_buildings_local );
	CFGVAR( vars.models.buildings, models_buildings_enemy_only );
	CFGVAR( vars.models.buildings, buildings_model );
	CFGVAR( vars.models.buildings, buildings_ignorez );
	CFGVAR( vars.models.buildings, buildings_overlay );
	CFGVAR( vars.models.buildings, buildings_overlay_alpha );

	CFGVAR( vars.models.world, models_world_enabled );
	CFGVAR( vars.models.world, models_world_friends );
	CFGVAR( vars.models.world, models_world_local );
	CFGVAR( vars.models.world, models_world_enemy_only );
	CFGVAR( vars.models.world, world_model );
	CFGVAR( vars.models.world, world_ignorez );
	CFGVAR( vars.models.world, world_overlay );
	CFGVAR( vars.models.world, world_overlay_alpha );


	CFGVAR( vars.outlines.global, outlines_global );
	CFGVAR( vars.outlines.global, blur_outline );
	CFGVAR( vars.outlines.global, wireframe_outline );
	CFGVAR( vars.outlines.global, outline_scale );

	CFGVAR( vars.outlines.players, outlines_players_enabled );
	CFGVAR( vars.outlines.players, outlines_players_friends );
	CFGVAR( vars.outlines.players, outlines_player_local );
	CFGVAR( vars.outlines.players, outlines_players_enemy_only );
	CFGVAR( vars.outlines.players, players_attachments_outline );

	CFGVAR( vars.outlines.buildings, outlines_buildings_enabled );
	CFGVAR( vars.outlines.buildings, outlines_buildings_friends );
	CFGVAR( vars.outlines.buildings, outlines_buildings_local );
	CFGVAR( vars.outlines.buildings, outlines_buildings_enemy_only );

	CFGVAR( vars.outlines.world, outlines_world_enabled );
	CFGVAR( vars.outlines.world, outlines_world_friends );
	CFGVAR( vars.outlines.world, outlines_world_local );
	CFGVAR( vars.outlines.world, outlines_world_enemy_only );


	CFGVAR( vars.misc, auto_jump );
	CFGVAR( vars.misc, break_jump );
	CFGVAR( vars.misc, tomislav_rev );
	CFGVAR( vars.misc, auto_strafe );
	CFGVAR( vars.misc, accurate_movement );
	CFGVAR( vars.misc, auto_scout_jump );
	CFGVAR( vars.misc, scout_jump_key );
	CFGVAR( vars.misc, disable_interp );


	CFGVAR( vars.misc.chatlog, log_active );
	CFGVAR( vars.misc.chatlog, log_connects );
	CFGVAR( vars.misc.chatlog, log_disconnects );
	CFGVAR( vars.misc.chatlog, log_class_changes );
	CFGVAR( vars.misc.chatlog, log_name_changes );
	CFGVAR( vars.misc.chatlog, log_votes );


	CFGVAR( vars.misc.crits, crits_active );
	CFGVAR( vars.misc.crits, avoid_random );
	CFGVAR( vars.misc.crits, crit_key );
	CFGVAR( vars.misc.crits, crits_indicator );


	CFGVAR( vars.misc.cl_move, doubletap );
	CFGVAR( vars.misc.cl_move, antiwarp );
	CFGVAR( vars.misc.cl_move, warp );
	CFGVAR( vars.misc.cl_move, smooth_warp );
	CFGVAR( vars.misc.cl_move, dt_indicator );
	CFGVAR( vars.misc.cl_move, recharge_key );
	CFGVAR( vars.misc.cl_move, warp_key );


	CFGVAR( vars.antiaim, antiaim_active );
	CFGVAR( vars.antiaim, rage );
	CFGVAR( vars.antiaim, fake_chams );
	CFGVAR( vars.antiaim, toggle_key );
	//legit
	CFGVAR( vars.antiaim, legit_yaw );
	CFGVAR( vars.antiaim, legit_yaw_mode );// 0 - Static,	1 - Edge
	CFGVAR( vars.antiaim, cycle_key );
	//rage
	CFGVAR( vars.antiaim, fake_yaw );			// -180 / 180
	CFGVAR( vars.antiaim, fake_mode );		// 0 - Static,	1 - Edge,	2 - Jitter,	3 - Spin
	CFGVAR( vars.antiaim, fake_delta );
	CFGVAR( vars.antiaim, fake_spin );
	CFGVAR( vars.antiaim, cycle_fake_key );

	CFGVAR( vars.antiaim, real_yaw );			// -180 / 180
	CFGVAR( vars.antiaim, real_mode );		// 0 - Static,	1 - Edge,	2 - Jitter,	3 - Spin
	CFGVAR( vars.antiaim, real_rotate );
	CFGVAR( vars.antiaim, real_delta );
	CFGVAR( vars.antiaim, real_spin );
	CFGVAR( vars.antiaim, real_angle1 );
	CFGVAR( vars.antiaim, real_angle2 );
	CFGVAR( vars.antiaim, cycle_real_key );

	CFGVAR( vars.antiaim, flat_material );

	CFGVAR( vars.antiaim, pitch );			// 0 - None,	1 - Automatic,	2 - Manual
	CFGVAR( vars.antiaim, fake_pitch );
	CFGVAR( vars.antiaim, real_pitch );		// 0 - Up,		1 - Down,	2 - Jitter


	CFGVAR( vars.visuals, visuals_thirdperson_key, VK_XBUTTON2 );
	CFGVAR( vars.visuals, visuals_thirdperson_offset );
	CFGVAR( vars.visuals, visuals_remove_scope );
	CFGVAR( vars.visuals, visuals_thirdperson_offset_forwards );
	CFGVAR( vars.visuals, visuals_thirdperson_offset_sideways );
	CFGVAR( vars.visuals, visuals_thirdperson_offset_upwards );
	CFGVAR( vars.visuals, visuals_fov );
	CFGVAR( vars.visuals, visuals_tracer );
	CFGVAR( vars.visuals, custom_beam );
	CFGVAR( vars.visuals, beam_time );
	CFGVAR( vars.visuals, beam_size );
	CFGVAR( vars.visuals, aspect_ratio );
	CFGVAR( vars.visuals, bypass_svpure );

	CFGVAR( vars.visuals, ragdolls_active );
	CFGVAR( vars.visuals, ragdolls_enemy_only );
	CFGVAR( vars.visuals, ragdolls_particle );
	CFGVAR( vars.visuals, ragdolls_effect );
	CFGVAR( vars.visuals, ragdolls_ash );
	CFGVAR( vars.visuals, ragdolls_dissolve );


	CFGVAR( vars.visuals, viewmodel_sway );
	CFGVAR( vars.visuals, viewmodel_pos );
	CFGVAR( vars.visuals, viewmodel_flip );
	CFGVAR( vars.visuals, viewmodel_min );
	CFGVAR( vars.visuals, viewmodel_fov );
	CFGVAR( vars.visuals, viewmodel_roll );
	CFGVAR( vars.visuals, viewmodel_pos_x );
	CFGVAR( vars.visuals, viewmodel_pos_y );
	CFGVAR( vars.visuals, viewmodel_pos_z );


	CFGVAR( vars.colors.esp.players, esp_player_color_scheme ); //0 - Enemy/Team,	1 - Red/Blu
	CFGVAR( vars.colors.esp.players, esp_local_player );

	CFGVAR( vars.colors.esp.players, esp_player_red );
	CFGVAR( vars.colors.esp.players, esp_player_blu );

	CFGVAR( vars.colors.esp.players, esp_player_enemy );
	CFGVAR( vars.colors.esp.players, esp_player_teammate );

	CFGVAR( vars.colors.esp.players, player_healthbar_top );
	CFGVAR( vars.colors.esp.players, player_healthbar_bottom );

	CFGVAR( vars.colors.esp.buildings, esp_building_color_scheme ); //0 - Enemy/Team,	1 - Red/Blu
	CFGVAR( vars.colors.esp.buildings, esp_local_building );
	CFGVAR( vars.colors.esp.buildings, esp_building_default );

	CFGVAR( vars.colors.esp.buildings, esp_building_red );
	CFGVAR( vars.colors.esp.buildings, esp_building_blu );

	CFGVAR( vars.colors.esp.buildings, esp_building_enemy );
	CFGVAR( vars.colors.esp.buildings, esp_building_teammate );

	CFGVAR( vars.colors.esp.buildings, building_healthbar_top );
	CFGVAR( vars.colors.esp.buildings, building_healthbar_bottom );

	CFGVAR( vars.colors.esp.world, esp_world_color_scheme ); //0 - Enemy/Team,	1 - Red/Blu
	CFGVAR( vars.colors.esp.world, esp_world_ammo );
	CFGVAR( vars.colors.esp.world, esp_world_health );
	CFGVAR( vars.colors.esp.world, esp_world_bomb );

	CFGVAR( vars.colors.esp.world, esp_world_red );
	CFGVAR( vars.colors.esp.world, esp_world_blu );

	CFGVAR( vars.colors.esp.world, esp_world_enemy );
	CFGVAR( vars.colors.esp.world, esp_world_teammate );


	CFGVAR( vars.colors.models.players, models_player_color_scheme ); //0 - Enemy/Team,	1 - Red/Blu
	CFGVAR( vars.colors.models.players, models_local_player );
	CFGVAR( vars.colors.models.players, models_player_fresnel );

	CFGVAR( vars.colors.models.players, models_player_red );
	CFGVAR( vars.colors.models.players, models_player_blu );

	CFGVAR( vars.colors.models.players, models_player_enemy );
	CFGVAR( vars.colors.models.players, models_player_teammate );


	CFGVAR( vars.colors.models.buildings, models_building_color_scheme ); //0 - Enemy/Team,	1 - Red/Blu
	CFGVAR( vars.colors.models.buildings, models_local_building );
	CFGVAR( vars.colors.models.buildings, models_building_default );
	CFGVAR( vars.colors.models.buildings, models_building_fresnel );

	CFGVAR( vars.colors.models.buildings, models_building_red );
	CFGVAR( vars.colors.models.buildings, models_building_blu );

	CFGVAR( vars.colors.models.buildings, models_building_enemy );
	CFGVAR( vars.colors.models.buildings, models_building_teammate );

	CFGVAR( vars.colors.models.world, models_world_color_scheme ); //0 - Enemy/Team,	1 - Red/Blu
	CFGVAR( vars.colors.models.world, models_world_ammo );
	CFGVAR( vars.colors.models.world, models_world_health );
	CFGVAR( vars.colors.models.world, models_world_bomb );
	CFGVAR( vars.colors.models.world, models_world_fresnel );

	CFGVAR( vars.colors.models.world, models_world_red );
	CFGVAR( vars.colors.models.world, models_world_blu );

	CFGVAR( vars.colors.models.world, models_world_enemy );
	CFGVAR( vars.colors.models.world, models_world_teammate );


	CFGVAR( vars.colors.outlines.players, outlines_player_color_scheme ); //0 - Enemy/Team,	1 - Red/Blu
	CFGVAR( vars.colors.outlines.players, outlines_local_player );

	CFGVAR( vars.colors.outlines.players, outlines_player_red );
	CFGVAR( vars.colors.outlines.players, outlines_player_blu );

	CFGVAR( vars.colors.outlines.players, outlines_player_enemy );
	CFGVAR( vars.colors.outlines.players, outlines_player_teammate );


	CFGVAR( vars.colors.outlines.buildings, outlines_building_color_scheme ); //0 - Enemy/Team,	1 - Red/Blu
	CFGVAR( vars.colors.outlines.buildings, outlines_local_building);
	CFGVAR( vars.colors.outlines.buildings, outlines_building_default );

	CFGVAR( vars.colors.outlines.buildings, outlines_building_red );
	CFGVAR( vars.colors.outlines.buildings, outlines_building_blu );

	CFGVAR( vars.colors.outlines.buildings, outlines_building_enemy );
	CFGVAR( vars.colors.outlines.buildings, outlines_building_teammate );

	CFGVAR( vars.colors.outlines.world, outlines_world_color_scheme ); //0 - Enemy/Team,	1 - Red/Blu
	CFGVAR( vars.colors.outlines.world, outlines_world_ammo );
	CFGVAR( vars.colors.outlines.world, outlines_world_health );
	CFGVAR( vars.colors.outlines.world, outlines_world_bomb );

	CFGVAR( vars.colors.outlines.world, outlines_world_red );
	CFGVAR( vars.colors.outlines.world, outlines_world_blu );

	CFGVAR( vars.colors.outlines.world, outlines_world_enemy );
	CFGVAR( vars.colors.outlines.world, outlines_world_teammate );


	CFGVAR( vars.colors.antiaim, local_fake );


	CFGVAR( vars.colors.misc, beam_red );
	CFGVAR( vars.colors.misc, beam_blu );
	CFGVAR( vars.colors.misc, beam_custom );

	CFGVAR( vars.colors.menu, menu_background );
	CFGVAR( vars.colors.menu, menu_background2 );
	CFGVAR( vars.colors.menu, menu_groupbox );
	CFGVAR( vars.colors.menu, menu_widget );
	CFGVAR( vars.colors.menu, menu_hovered );
	CFGVAR( vars.colors.menu, menu_accent );
	CFGVAR( vars.colors.menu, menu_selected );
	CFGVAR( vars.colors.menu, menu_clear );
};