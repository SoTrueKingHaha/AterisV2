#include "Menu.h"

#include "../../utils/draw/draw.h"
#include "WindowMenu.h"
#include <utility>
#include "Input.h"
#include <vector>
#include "../../hooks/hooks.h"
#include "../../hooks/hooklist.h"
#include "config/config.h"

bool LegitInit = false;
bool Init = true;

void do_testing( )
{
	const auto& inetinfo = i::engine_client->GetNetChannelInfo( );
	if ( !inetinfo )
	{
		return;
	}

	if ( inetinfo->IsLoopback( ) )
	{
		const auto& local = entity_cache.get_local( );
		if ( !local || local->deadflag( ) )
		{
			return;
		}

		i::engine_client->ClientCmd_Unrestricted( "sv_cheats 1" );
		for ( int i = 0; i < 4; i++ )
		{
			i::engine_client->ClientCmd_Unrestricted( "bot" );
		}
		i::engine_client->ClientCmd_Unrestricted( "mp_disable_respawn_times 1" );
	}
}

void do_map( std::string map )
{
	std::string command = "map " + map;
	i::engine_client->ClientCmd_Unrestricted( command.c_str( ) );
}

void InitializeMenu( )
{
	window = new CMenuWindow
	{
		"Langram-TF2",
		{
			new CMenuTab { "Aimbot", {
				new CMenuGroupbox { "Global", {
					new CItemCheckbox{ "Enable Aimbot", vars.aimbot.global.global_enabled },
					new CItemKeybind{ "Aimbot Key", vars.aimbot.global.aimbot_key},
					new CItemCheckbox{ "Auto shoot", vars.aimbot.global.aimbot_autoshoot },
					new CItemMultiCombobox{ "Aim Targets", { { "Aim Players", &vars.aimbot.global.aimbot_aimplayers },
															 { "Aim Sentries", &vars.aimbot.global.aimbot_aimbuildings },
															 { "Aim Stickies", &vars.aimbot.global.aimbot_aimstickies }} },

					new CItemMultiCombobox{ "Ignore Flags", { { "Ignore Invincible", &vars.aimbot.global.aimbot_ignore_invincible },
															  { "Ignore Cloaked", &vars.aimbot.global.aimbot_ignore_cloaked },
															  { "Ignore Taunting", &vars.aimbot.global.aimbot_ignore_taunting },
															  { "Ignore Friends", &vars.aimbot.global.aimbot_ignore_friends } } },
					}},

					new CMenuGroupbox { "Hitscan", {
						new CItemCheckbox{ "Enable Aimbot", vars.aimbot.hitscan.hitscan_active },
						new CItemCombobox{ "Aim Method", vars.aimbot.hitscan.hitscan_aim, { "Normal", "Silent" } },
						new CItemSliderInt{ "FOV Amount", vars.aimbot.hitscan.hitscan_fov, 0, 180, 1 },
						new CItemCombobox { "Targetted Hitbox", vars.aimbot.hitscan.hitscan_hitbox, { "Head", "Pelvis" } },
						new CItemCheckbox{ "Use All Hitboxes", vars.aimbot.hitscan.hitscan_multipoint },
						new CItemCheckbox{ "Multipoint Head", vars.aimbot.hitscan.hitscan_multipoint_head },
						new CItemSliderFloat{ "Head Scale", vars.aimbot.hitscan.hitscan_multipoint_head_scale, 0.5f, 0.9f, 0.05f },
						new CItemCheckbox{ "Multipoint Pelvis", vars.aimbot.hitscan.hitscan_multipoint_pelvis },
							new CItemSliderFloat{ "Pelvis Scale", vars.aimbot.hitscan.hitscan_multipoint_pelvis_scale, 0.5f, 0.9f, 0.05f },
					}},

					new CMenuGroupbox { "Projectile", {
						new CItemCheckbox{ "Enable Aimbot", vars.aimbot.projectile.projectile_active },
						new CItemCombobox{ "Aim Method", vars.aimbot.projectile.projectile_aim_method, { "Normal", "Silent" } },	//0 - Normal,	1 - Silent
						new CItemSliderInt{ "FOV Amount", vars.aimbot.projectile.projectile_aim_fov, 0, 180, 1 },
						new CItemCombobox{ "Targetted Hitbox", vars.aimbot.projectile.projectile_aim_position, { "Head", "Body", "Feet", "Auto" } },	//0 - Head,		1 - Body,		2 - Feet,	3 - Auto
						new CItemCheckbox{ "Aim Feet On Ground", vars.aimbot.projectile.projectile_feet_aim_on_ground },
						new CItemMultiCombobox{ "Select Hitboxes", { { "Head", &vars.aimbot.projectile.projectile_head_allowed },
																	 { "Body", &vars.aimbot.projectile.projectile_body_allowed },
																	 { "Feet", &vars.aimbot.projectile.projectile_feet_allowed } } },
						new CItemCheckbox{ "Charge Loose Cannon", vars.aimbot.projectile.projectile_charge_loose_cannon },
						new CItemCheckbox{ "Show Predicted Path", vars.aimbot.projectile.projectile_predicted_path },
						new CItemSliderFloat{ "Predicted Path Time", vars.aimbot.projectile.projectile_path_duration, 1.f, 10.f, 0.1f, "%.1f" },
						new CItemCheckbox{ "Projectile Auto Detonate", vars.aimbot.projectile.projectile_auto_detonate },
						new CItemMultiCombobox{ "Select Projectiles", { { "Stickies", &vars.aimbot.projectile.projectile_auto_detonate_stickies },
																	{ "Flares", &vars.aimbot.projectile.projectile_auto_detonate_flares } } },
						new CItemMultiCombobox{ "Select Targets", { { "Players", &vars.aimbot.projectile.projectile_detonate_players },
																	{ "Sentries", &vars.aimbot.projectile.projectile_detonate_sentries },
																	{ "Bombs", &vars.aimbot.projectile.projectile_detonate_bombs } } },
					}},
			}},
			new CMenuTab{ "ESP", {
				new CMenuGroupbox { "Global", {
					new CItemCheckbox{ "ESP Global", vars.esp.global.esp_global },
				}},
				new CMenuGroupbox { "Buildings", {
					new CItemCombobox{ "Color Scheme", vars.colors.esp.buildings.esp_building_color_scheme, { "Enemy/Team", "Red/Blu" } },
					new CItemCheckbox{ "ESP Buildings", vars.esp.buildings.esp_buildings },
					new CItemCheckbox{ "Render Local Buildings", vars.esp.buildings.esp_buildings_local },
					new CItemCheckbox{ "Render Friends Buildings", vars.esp.buildings.esp_buildings_friends },
					new CItemCheckbox{ "Enemy Only", vars.esp.buildings.esp_buildings_enemy_only },
					new CItemCheckbox{ "Name", vars.esp.buildings.buildings_name },
					new CItemCheckbox{ "Box", vars.esp.buildings.buildings_box },
					new CItemCheckbox{ "Health bar", vars.esp.buildings.buildings_healthbar },
					new CItemMultiCombobox{ "Health bar Flags", { { "Custom Color", &vars.esp.buildings.buildings_custom_healthbar }, { "Dynamic Bar", &vars.esp.buildings.buildings_animated_healthbar },
										  { "Gradient Color", &vars.esp.buildings.buildings_gradient_healthbar }, { "Health bar Lines", &vars.esp.buildings.buildings_healthbar_lines } } },
					new CItemSliderInt{ "Lines Amount", vars.esp.buildings.buildings_healthbar_lines_count, 0, 10 },
					new CItemCheckbox{ "Conditions", vars.esp.buildings.buildings_conds },
					new CItemCheckbox{ "Distance", vars.esp.buildings.buildings_distance },
					new CItemCheckbox{ "Sentry Alert", vars.esp.buildings.sentry_alert },
					new CItemSliderFloat{ "Distance from Center", vars.esp.buildings.buildings_screen_distance, 0.f, 0.25f, 0.01f, "%.2f" },
					new CItemCombobox{ "Shape on Screen", vars.esp.buildings.buildings_screen_shape, { "Circle", "Ellipsis" } },
					new CItemColorPicker{ "Local Buildings", vars.colors.esp.buildings.esp_local_building },
					new CItemColorPicker{ "Red Buildings", vars.colors.esp.buildings.esp_building_red  },
					new CItemColorPicker{ "Blu Buildings", vars.colors.esp.buildings.esp_building_blu  },
					new CItemColorPicker{ "Enemy Buildings", vars.colors.esp.buildings.esp_building_enemy  },
					new CItemColorPicker{ "Teammate Buildings", vars.colors.esp.buildings.esp_building_teammate },
					new CItemColorPicker{ "Health bar Top", vars.colors.esp.buildings.building_healthbar_top },
					new CItemColorPicker{ "Health bar Bottom", vars.colors.esp.buildings.building_healthbar_bottom },
				}},
				new CMenuGroupbox { "Players", {
					new CItemCombobox{ "Color Scheme", vars.colors.esp.players.esp_player_color_scheme, { "Enemy/Team", "Red/Blu" } },
					new CItemCheckbox{ "ESP Players", vars.esp.players.esp_players },
					new CItemCheckbox{ "Render Local Player", vars.esp.players.esp_player_local },
					new CItemCheckbox{ "Render Friends", vars.esp.players.esp_players_friends },
					new CItemCheckbox{ "Enemy Only", vars.esp.players.esp_players_enemy_only },
					new CItemCheckbox{ "Name", vars.esp.players.players_name },
					new CItemCheckbox{ "Box", vars.esp.players.players_box },
					new CItemCheckbox{ "Health bar", vars.esp.players.players_healthbar },
					new CItemMultiCombobox{ "Health bar Flags", { { "Custom Color", &vars.esp.players.players_custom_healthbar }, { "Dynamic Bar", &vars.esp.players.players_animated_healthbar },
										  { "Gradient Color", &vars.esp.players.players_gradient_healthbar }, { "Health bar Lines", &vars.esp.players.players_healthbar_lines } } },
					new CItemSliderInt{ "Lines Amount", vars.esp.players.players_healthbar_lines_count, 0, 10 },
					new CItemCheckbox{ "Conditions", vars.esp.players.players_conds },
					new CItemCheckbox{ "Weapons", vars.esp.players.players_weapons },
					new CItemCheckbox{ "Distance", vars.esp.players.players_distance },
					new CItemCheckbox{ "Player Alert", vars.esp.players.players_alert },
					new CItemSliderFloat{ "Distance from Center", vars.esp.players.players_screen_distance, 0.f, 0.25f, 0.01f, "%.2f" },
					new CItemCombobox{ "Shape on Screen", vars.esp.players.players_screen_shape, { "Circle", "Ellipsis" } },
					new CItemCheckbox{ "High K/D", vars.esp.players.players_high_kd },
					new CItemSliderFloat{ "K/D Amount", vars.esp.players.players_kd, 5.f, 10.f, 0.1f, "%.1f" },
					new CItemCheckbox{ "High Headshot Ratio", vars.esp.players.players_high_headshot },
					new CItemSliderFloat{ "Ratio Amount", vars.esp.players.players_headshot, 0.5f, 1.0f, 0.01f },
					new CItemColorPicker{ "Local Player", vars.colors.esp.players.esp_local_player },
					new CItemColorPicker{ "Red Players", vars.colors.esp.players.esp_player_red  },
					new CItemColorPicker{ "Blu Players", vars.colors.esp.players.esp_player_blu  },
					new CItemColorPicker{ "Enemies", vars.colors.esp.players.esp_player_enemy  },
					new CItemColorPicker{ "Teammates", vars.colors.esp.players.esp_player_teammate },
					new CItemColorPicker{ "Health bar Top", vars.colors.esp.players.player_healthbar_top },
					new CItemColorPicker{ "Health bar Bottom", vars.colors.esp.players.player_healthbar_bottom },
				}},
				new CMenuGroupbox{ "World", {
					new CItemCombobox{ "Color Scheme", vars.colors.esp.world.esp_world_color_scheme, { "Enemy/Team", "Red/Blu" } },
					new CItemCheckbox{ "ESP World", vars.esp.world.esp_world },
					new CItemCheckbox{ "Render Local Entities", vars.esp.world.esp_world_local },
					new CItemCheckbox{ "Render Friends Entities", vars.esp.world.esp_world_friends },
					new CItemCheckbox{ "Enemy Only", vars.esp.world.esp_world_enemy_only },
					new CItemCheckbox{ "Name", vars.esp.world.world_name },
					new CItemCheckbox{ "Box", vars.esp.world.world_box },
					new CItemCheckbox{ "Distance", vars.esp.world.world_distance },
					new CItemColorPicker{ "Local Entities", vars.colors.esp.world.esp_local_world },
					new CItemColorPicker{ "Ammopacks", vars.colors.esp.world.esp_world_ammo },
					new CItemColorPicker{ "Healthpacks", vars.colors.esp.world.esp_world_health },
					new CItemColorPicker{ "Bombs", vars.colors.esp.world.esp_world_bomb },
					new CItemColorPicker{ "Red Entities", vars.colors.esp.world.esp_world_red },
					new CItemColorPicker{ "Blu Entities", vars.colors.esp.world.esp_world_blu },
					new CItemColorPicker{ "Enemy Entities", vars.colors.esp.world.esp_world_enemy },
					new CItemColorPicker{ "Teammate Entities", vars.colors.esp.world.esp_world_teammate },
				} },
			} },
			new CMenuTab{ "Models", {
				new CMenuGroupbox{ "Global", {
					new CItemCheckbox{ "Models Global", vars.models.global.models_global },
					new CItemCombobox{ "Order Scheme", vars.models.global.models_order, { "Models First", "Outlines First" } },
				} },
				new CMenuGroupbox{ "Buildings", {
					new CItemCombobox{ "Color Scheme", vars.colors.models.buildings.models_building_color_scheme, { "Enemy/Team", "Red/Blu" } },
					new CItemCombobox{ "Select Model", vars.models.buildings.buildings_model, { "None", "Flat", "Shaded", "Fresnel" } },
					new CItemCheckbox{ "Buildings Models", vars.models.buildings.models_buildings_enabled },
					new CItemCheckbox{ "Render Local Buildings", vars.models.buildings.models_buildings_local },
					new CItemCheckbox{ "Render Friends Buildings", vars.models.buildings.models_buildings_friends },
					new CItemCheckbox{ "Enemy Only", vars.models.buildings.models_buildings_enemy_only },
					new CItemCheckbox{ "IgnoreZ", vars.models.buildings.buildings_ignorez },
					new CItemCheckbox{ "Models Overlay", vars.models.buildings.buildings_overlay },
					new CItemSliderFloat{ "Overlay Alpha", vars.models.buildings.buildings_overlay_alpha, 0.0f, 1.0f },
					new CItemColorPicker{ "Local Buildings", vars.colors.models.buildings.models_local_building },
					new CItemColorPicker{ "Red Buildings", vars.colors.models.buildings.models_building_red  },
					new CItemColorPicker{ "Blu Buildings", vars.colors.models.buildings.models_building_blu  },
					new CItemColorPicker{ "Enemy Buildings", vars.colors.models.buildings.models_building_enemy  },
					new CItemColorPicker{ "Teammate Buildings", vars.colors.models.buildings.models_building_teammate },
					new CItemColorPicker{ "Fresnel Buildings", vars.colors.models.buildings.models_building_fresnel },
				} },
				new CMenuGroupbox{ "Players", {
					new CItemCombobox{ "Color Scheme", vars.colors.models.players.models_player_color_scheme, { "Enemy/Team", "Red/Blu" } },
					new CItemCombobox{ "Select Model", vars.models.players.players_model, { "None", "Flat", "Shaded", "Fresnel" } },
					new CItemCheckbox{ "Players Models", vars.models.players.models_players_enabled },
					new CItemCheckbox{ "Render Local Models", vars.models.players.models_player_local },
					new CItemCheckbox{ "Render Friends Models", vars.models.players.models_players_friends },
					new CItemCheckbox{ "Enemy Only", vars.models.players.models_players_enemy_only },
					new CItemCheckbox{ "IgnoreZ", vars.models.players.players_ignorez },
					new CItemCheckbox{ "Models Overlay", vars.models.players.players_overlay },
					new CItemSliderFloat{ "Overlay Alpha", vars.models.players.players_overlay_alpha, 0.0f, 1.0f },
					new CItemColorPicker{ "Local Player", vars.colors.models.players.models_local_player },
					new CItemColorPicker{ "Red Players", vars.colors.models.players.models_player_red  },
					new CItemColorPicker{ "Blu Players", vars.colors.models.players.models_player_blu  },
					new CItemColorPicker{ "Enemies", vars.colors.models.players.models_player_enemy  },
					new CItemColorPicker{ "Teammates", vars.colors.models.players.models_player_teammate },
					new CItemColorPicker{ "Fresnel", vars.colors.models.players.models_player_fresnel },
				} },
				new CMenuGroupbox{ "World", {
					new CItemCombobox{ "Color Scheme", vars.colors.models.world.models_world_color_scheme, { "Enemy/Team", "Red/Blu" } },
					new CItemCombobox{ "Select Model", vars.models.world.world_model, { "None", "Flat", "Shaded", "Fresnel" } },
					new CItemCheckbox{ "Models World", vars.models.world.models_world_enabled },
					new CItemCheckbox{ "Render Local Entities", vars.models.world.models_world_local },
					new CItemCheckbox{ "Render Friends Entities", vars.models.world.models_world_friends },
					new CItemCheckbox{ "Enemy Only", vars.models.world.models_world_enemy_only },
					new CItemCheckbox{ "IgnoreZ", vars.models.world.world_ignorez },
					new CItemCheckbox{ "Models Overlay", vars.models.world.world_overlay },
					new CItemSliderFloat{ "Overlay Alpha", vars.models.world.world_overlay_alpha, 0.0f, 1.0f },
					new CItemColorPicker{ "Local Entities", vars.colors.models.world.models_local_world },
					new CItemColorPicker{ "Ammopacks", vars.colors.models.world.models_world_ammo },
					new CItemColorPicker{ "Healthpacks", vars.colors.models.world.models_world_health },
					new CItemColorPicker{ "Bombs", vars.colors.models.world.models_world_bomb },
					new CItemColorPicker{ "Red Entities", vars.colors.models.world.models_world_red },
					new CItemColorPicker{ "Blu Entities", vars.colors.models.world.models_world_blu },
					new CItemColorPicker{ "Enemy Entities", vars.colors.models.world.models_world_enemy },
					new CItemColorPicker{ "Teammate Entities", vars.colors.models.world.models_world_teammate },
					new CItemColorPicker{ "Fresnel Entities", vars.colors.models.world.models_world_fresnel },
				} },
			} },
			new CMenuTab{ "Outlines", {
				new CMenuGroupbox{ "Global", {
					new CItemCheckbox{ "Outlines Global", vars.outlines.global.outlines_global },
					new CItemMultiCombobox{ "Outline Types", { { "Blur Outline", &vars.outlines.global.blur_outline },
															   { "Stencil Outline", & vars.outlines.global.wireframe_outline } } },
															   /*new CItemCheckbox{ "Chams Global", vars.outlines.global.blur_outline },
															   new CItemCheckbox{ "Chams Global", vars.outlines.global.wireframe_outline },*/
					new CItemSliderInt{ "Blur Outline Scale", vars.outlines.global.outline_scale, 0, 10 },
				} },
				new CMenuGroupbox{ "Buildings", {
					new CItemCombobox{ "Color Scheme", vars.colors.outlines.buildings.outlines_building_color_scheme, { "Enemy/Team", "Red/Blu" } },
					new CItemCheckbox{ "Outline Buildings", vars.outlines.buildings.outlines_buildings_enabled },
					new CItemCheckbox{ "Render Local Buildings", vars.outlines.buildings.outlines_buildings_local },
					new CItemCheckbox{ "Render Friends Buildings", vars.outlines.buildings.outlines_buildings_friends },
					new CItemCheckbox{ "Enemy Only", vars.outlines.buildings.outlines_buildings_enemy_only },
						new CItemCheckbox{ "Local Buildings", vars.outlines.buildings.outlines_buildings_local },
						new CItemCheckbox{ "Friend Buildings", vars.outlines.buildings.outlines_buildings_friends },
						new CItemColorPicker{ "Local Buildings", vars.colors.outlines.buildings.outlines_local_building },
						new CItemColorPicker{ "Red Buildings", vars.colors.outlines.buildings.outlines_building_red },
						new CItemColorPicker{ "Blu Buildings", vars.colors.outlines.buildings.outlines_building_blu },
						new CItemColorPicker{ "Enemy Buildings", vars.colors.outlines.buildings.outlines_building_enemy },
						new CItemColorPicker{ "Teammate Buildings", vars.colors.outlines.buildings.outlines_building_teammate },
				} },
				new CMenuGroupbox{ "Players", {
					new CItemCombobox{ "Color Scheme", vars.colors.outlines.players.outlines_player_color_scheme, { "Enemy/Team", "Red/Blu" } },
					new CItemCheckbox{ "Outline Players", vars.outlines.players.outlines_players_enabled },
					new CItemCheckbox{ "Render Local Outlines", vars.outlines.players.outlines_player_local },
					new CItemCheckbox{ "Render Friends Outlines", vars.outlines.players.outlines_players_friends },
					new CItemCheckbox{ "Enemy Only", vars.outlines.players.outlines_players_enemy_only },
					//new CItemCheckbox{ "Chams Global", vars.outlines.players.viewmodel_outline },
					new CItemCheckbox{ "Outline Attachments", vars.outlines.players.players_attachments_outline },
					new CItemColorPicker{ "Local Player", vars.colors.outlines.players.outlines_local_player },
					new CItemColorPicker{ "Red Players", vars.colors.outlines.players.outlines_player_red  },
					new CItemColorPicker{ "Blu Players", vars.colors.outlines.players.outlines_player_blu  },
					new CItemColorPicker{ "Enemies", vars.colors.outlines.players.outlines_player_enemy  },
					new CItemColorPicker{ "Teammates", vars.colors.outlines.players.outlines_player_teammate },
				} },
				new CMenuGroupbox{ "World", {
					new CItemCombobox{ "Color Scheme", vars.colors.outlines.world.outlines_world_color_scheme, { "Enemy/Team", "Red/Blu" } },
					new CItemCheckbox{ "Outline World", vars.outlines.world.outlines_world_enabled },
					new CItemCheckbox{ "Render Local Entities", vars.outlines.world.outlines_world_local },
					new CItemCheckbox{ "Render Friends Entities", vars.outlines.world.outlines_world_friends },
					new CItemCheckbox{ "Enemy Only", vars.outlines.world.outlines_world_enemy_only },
					new CItemColorPicker{ "Local Entities", vars.colors.outlines.world.outlines_local_world },
					new CItemColorPicker{ "Ammopacks", vars.colors.outlines.world.outlines_world_ammo },
					new CItemColorPicker{ "Healthpacks", vars.colors.outlines.world.outlines_world_health },
					new CItemColorPicker{ "Bombs", vars.colors.outlines.world.outlines_world_bomb },
					new CItemColorPicker{ "Red Entities", vars.colors.outlines.world.outlines_world_red },
					new CItemColorPicker{ "Blu Entities", vars.colors.outlines.world.outlines_world_blu },
					new CItemColorPicker{ "Enemy Entities", vars.colors.outlines.world.outlines_world_enemy },
					new CItemColorPicker{ "Teammate Entities", vars.colors.outlines.world.outlines_world_teammate },
				} },
			} },
			new CMenuTab{ "Misc", {
				new CMenuGroupbox { "Movement", {
					new CItemCheckbox{ "Auto Jump", vars.misc.auto_jump },
					new CItemCheckbox{ "Break Jump", vars.misc.break_jump },
					new CItemCheckbox{ "Tomislav Rev", vars.misc.tomislav_rev },
					new CItemCombobox{ "Auto Strafe", vars.misc.auto_strafe, { "None", "Simple", "Directional" } },
					new CItemCheckbox{ "Stop Movement", vars.misc.accurate_movement },
					new CItemCheckbox{ "Auto Scout Jump", vars.misc.auto_scout_jump },
					new CItemKeybind{ "Scout Jump Key", vars.misc.scout_jump_key },
				}},
				new CMenuGroupbox { "View", {
					new CItemCheckbox{ "Remove Scope", vars.visuals.visuals_remove_scope },
					new CItemCheckbox{ "Disable Interp", vars.misc.disable_interp },
					new CItemCheckbox{ "Bypass sv_pure", vars.visuals.bypass_svpure },
					new CItemSliderInt{ "FOV Override",  vars.visuals.visuals_fov, 30, 160 },
					new CItemSliderFloat{ "Aspect Ratio", vars.visuals.aspect_ratio, 0.0f, 3.0f, 0.01f },

					new CItemSpace{ "", false },
					new CItemSpace{ "Thirdperson", true },
					new CItemKeybind{ "Thirdperson Key", vars.visuals.visuals_thirdperson_key },
					new CItemCheckbox{ "Thirdperson Offset", vars.visuals.visuals_thirdperson_offset },
					new CItemSliderInt{ "Offset Forwards", vars.visuals.visuals_thirdperson_offset_forwards, -300, 300, 1 },
					new CItemSliderInt{ "Offset Sideways", vars.visuals.visuals_thirdperson_offset_sideways, -300, 300, 1 },
					new CItemSliderInt{ "Offset Upwards", vars.visuals.visuals_thirdperson_offset_upwards, -300, 300, 1 },

					new CItemSpace{ "", false },
					new CItemSpace{ "Ragdolls", true },
					new CItemCheckbox{ "Enable Ragdolls", vars.visuals.ragdolls_active },
					new CItemCheckbox{ "Enemy Only", vars.visuals.ragdolls_enemy_only },
					new CItemCombobox{ "Model effect", vars.visuals.ragdolls_effect, { "None", "Gold", "Ice" } },
					new CItemCombobox{ "Particle effect", vars.visuals.ragdolls_particle, { "None", "Burning", "Electrocuted" } },
					new CItemMultiCombobox{ "Extra particle effects", { { "Ash", &vars.visuals.ragdolls_ash },
																			{ "Dissolve", &vars.visuals.ragdolls_dissolve } } },
				}},
				new CMenuGroupbox{ "Viewmodel", {
					new CItemCombobox{ "Bullet tracers", vars.visuals.visuals_tracer,
									 { "None", "Machina", "C.A.P.P.E.R", "Short Circuit", "Merasmus Beam 1", "Merasmus Beam 2", "Big Nasty", "Beams" } },
					new CItemCheckbox{ "Custom Beam Color", vars.visuals.custom_beam },
					new CItemSliderFloat{ "Beam Lifetime", vars.visuals.beam_time, 0.5f, 10.f, 0.1f, "%.1f" },
					new CItemSliderFloat{ "Beam Size", vars.visuals.beam_size, 1.0f, 20.f, 0.1f, "%.1f" },
					new CItemColorPicker{ "Beam Custom", vars.colors.misc.beam_custom },
					new CItemColorPicker{ "Beam Red", vars.colors.misc.beam_red },
					new CItemColorPicker{ "Beam Blu", vars.colors.misc.beam_blu },
					new CItemCheckbox{ "Viewmodel Sway", vars.visuals.viewmodel_sway },
					new CItemCheckbox{ "Viewmodel Flip", vars.visuals.viewmodel_flip },
					new CItemCheckbox{ "Minimize Viewmodel", vars.visuals.viewmodel_min },
					new CItemCheckbox{ "Viewmodel Pos", vars.visuals.viewmodel_pos },
					new CItemSliderInt{ "Viewmodel FOV",  vars.visuals.viewmodel_fov, 30, 160 },
					new CItemSliderInt{ "Viewmodel Roll",  vars.visuals.viewmodel_roll, -180, 180 },
					new CItemSliderInt{ "X Coord",  vars.visuals.viewmodel_pos_x, -45, 45 },
					new CItemSliderInt{ "Y Coord",  vars.visuals.viewmodel_pos_y, -45, 45 },
					new CItemSliderInt{ "Z Coord",  vars.visuals.viewmodel_pos_z, -45, 45 },
				}},
				new CMenuGroupbox { "Information", {
					new CItemCheckbox{ "Chat Logs", vars.misc.chatlog.log_active },
					new CItemMultiCombobox{ "Select Logs", { { "Log Connections", &vars.misc.chatlog.log_connects }, { "Log Disconnections", &vars.misc.chatlog.log_disconnects },
															 { "Log Class Changes", &vars.misc.chatlog.log_class_changes }, { "Log Name Changes", &vars.misc.chatlog.log_name_changes }, 
															 { "Log Votes", &vars.misc.chatlog.log_votes } } },
				} },
			} },

			new CMenuTab{ "Misc 2", {
				new CMenuGroupbox { "Tickbase/Network", {
					new CItemMultiCombobox{ "Tick Shifting", { { "Doubletap", &vars.misc.cl_move.doubletap }, { "Warp", &vars.misc.cl_move.warp } } },
					new CItemMultiCombobox{ "Warp Settings", { { "Anti-Warp", &vars.misc.cl_move.antiwarp }, { "Smooth Warp", &vars.misc.cl_move.smooth_warp } } },
					new CItemCheckbox{ "Indicator", vars.misc.cl_move.dt_indicator },
					new CItemKeybind{ "Recharge Key", vars.misc.cl_move.recharge_key },
					new CItemKeybind{ "Warp Key", vars.misc.cl_move.warp_key },
					new CItemSpace{ "", false },
					new CItemSpace{ "Backtrack (Beta)", true },
					new CItemCheckbox{ "Backtrack Active", vars.backtrack.backtrack_active },
					new CItemCheckbox{ "Backtrack Chams", vars.backtrack.chams },
					new CItemCheckbox{ "Show Debug Info", vars.backtrack.debug },
				}},
				new CMenuGroupbox{ "Anti-Aim", {
					new CItemCheckbox{ "Enable Anti-Aim", vars.antiaim.antiaim_active },
					new CItemCheckbox{ "Rage Anti-Aim", vars.antiaim.rage },
					new CItemCheckbox{ "Enable Fake Chams", vars.antiaim.fake_chams },
					new CItemKeybind{ "Toggle Key", vars.antiaim.toggle_key },
					new CItemCheckbox{ "Use flat material", vars.antiaim.flat_material },
					new CItemColorPicker{ "Fake Color", vars.colors.antiaim.local_fake },
					//legit
					new CItemSpace{ "", false },
					new CItemSpace{ "Legit Anti-Aim", true },
					new CItemSliderInt{ "Legit Yaw",  vars.antiaim.legit_yaw, -180, 180 },
					new CItemCombobox{ "Legit Yaw Mode", vars.antiaim.legit_yaw_mode, { "Static", "Edge" } },// 0 - Static,	1 - Edge
					new CItemKeybind{ "Cycle Yaw Key", vars.antiaim.cycle_key },
					//rage
					new CItemSpace{ "", false },
					new CItemSpace{ "Rage Anti-Aim", true },

					new CItemSpace{ "", false },
					new CItemSpace{ "Fake Yaw", true },
					new CItemSliderInt{ "Fake Yaw",  vars.antiaim.fake_yaw, -180, 180 },
					new CItemCombobox{ "Fake Yaw Mode", vars.antiaim.fake_mode, { "Static", "Edge", "Jitter", "Spin", "Rotate" } },		// 0 - Static,	1 - Edge,	2 - Jitter,	3 - Spin
					new CItemSliderInt{ "Rotate Yaw",  vars.antiaim.fake_rotate, 1, 14 },
					new CItemSliderInt{ "Yaw Delta",  vars.antiaim.fake_delta, 0, 180 },
					new CItemSliderInt{ "Yaw Speed",  vars.antiaim.fake_spin, -30, 30 },
					new CItemSliderInt{ "Angle 1",  vars.antiaim.fake_angle1, -180, 180 },
					new CItemSliderInt{ "Angle 2",  vars.antiaim.fake_angle2, -180, 180 },
					new CItemKeybind{ "Cycle Fake Key", vars.antiaim.cycle_fake_key },

					new CItemSpace{ "", false },
					new CItemSpace{ "Real Yaw", true },
					new CItemSliderInt{ "Real Yaw",  vars.antiaim.real_yaw, -180, 180 },		// -180 / 180
					new CItemCombobox{ "Real Yaw Mode", vars.antiaim.real_mode, { "Static", "Edge", "Jitter", "Spin", "Rotate" } },		// 0 - Static,	1 - Edge,	2 - Jitter,	3 - Spin
					new CItemSliderInt{ "Rotate Yaw",  vars.antiaim.real_rotate, 1, 14 },
					new CItemSliderInt{ "Yaw Delta",  vars.antiaim.real_delta, 0, 180 },
					new CItemSliderInt{ "Yaw Speed",  vars.antiaim.real_spin, -30, 30 },
					new CItemSliderInt{ "Angle 1",  vars.antiaim.real_angle1, -180, 180 },
					new CItemSliderInt{ "Angle 2",  vars.antiaim.real_angle2, -180, 180 },
					new CItemKeybind{ "Cycle Real Key", vars.antiaim.cycle_real_key },

					new CItemSpace{ "", false },
					new CItemSpace{ "Pitch", true },
					new CItemCombobox{ "Enable Pitch", vars.antiaim.pitch, { "None", "Automatic", "Manual" } },			// 0 - None,	1 - Automatic,	2 - Manual
					new CItemCheckbox{ "Fake Pitch", vars.antiaim.fake_pitch },
					new CItemCombobox{ "Pitch Mode", vars.antiaim.real_pitch, { "Up", "Down", "Jitter" } },	// 0 - Up,		1 - Down,	2 - Jitter
				}},
				new CMenuGroupbox { "Crits", {
					new CItemCheckbox{ "Enable Crits", vars.misc.crits.crits_active },
					new CItemCheckbox{ "Avoid Random Crits", vars.misc.crits.avoid_random },
					new CItemCheckbox{ "Crit Indicator", vars.misc.crits.crits_indicator },
					new CItemKeybind{ "Crit Key", vars.misc.crits.crit_key },
				}},
				new CMenuGroupbox{ "Fakelag", {

				}},
			} },
			new CMenuTab{ "Settings", {
				new CMenuGroupbox{ "Configs", {
					new CItemButton{ "Save", [ ] ( ) { C::Save( vars.global.selected_config.c_str( ) ); } },
					new CItemButton{ "Load", [ ] ( ) { C::Load( vars.global.selected_config.c_str( ) ); } },
				}},
				new CMenuGroupbox{ "Menu colors", {
					new CItemColorPicker{ "Menu Background 1", vars.colors.menu.menu_background },
					new CItemColorPicker{ "Menu Background 2", vars.colors.menu.menu_background2 },
					new CItemColorPicker{ "Menu Accent", vars.colors.menu.menu_accent },
					new CItemColorPicker{ "Menu Widget", vars.colors.menu.menu_widget },
					new CItemColorPicker{ "Menu Hovered", vars.colors.menu.menu_hovered },
					new CItemColorPicker{ "Menu Selected", vars.colors.menu.menu_selected },
					new CItemColorPicker{ "Menu Groupbox", vars.colors.menu.menu_groupbox },
					new CItemColorPicker{ "Menu Clear", vars.colors.menu.menu_clear },
				}},
				new CMenuGroupbox{ "Cheat Mode", {
					new CItemCheckbox{ "Legit Only", vars.global.legit_only }
				}},
				new CMenuGroupbox{ "Debug", {
					new CItemCheckbox{ "Debug Crits", vars.debug.crit },
					new CItemButton{ "Prepare map", [ ] ( ) { do_map( vars.debug.map ); } },
					new CItemTextbox{ "Input Map", &vars.debug.map, 32, {'!'} },
					new CItemButton{ "Prepare for testing", [ ] ( ) { do_testing( ); } }
				}},
			}},
		}
	};
}

void InitializeLegitMenu( )
{
	window = new CMenuWindow
	{
		"Langram-TF2",
		{
			new CMenuTab { "Legit Aimbot", {
				new CMenuGroupbox { "Targetting", {
					//target algorithm
					//target entities
					//target entity conditions
				}},

				new CMenuGroupbox { "Aiming", {
					//aiming function f(x)
					//aiming function change conditions
				}},

				new CMenuGroupbox { "Anti Legit-Players", {
					//break on spectated
					//break on high kd
					//break on winning round
					//break on top fragging
				}},
			}},

			new CMenuTab { "ESP", {
				new CMenuGroupbox { "Global", {
					new CItemCheckbox{ "ESP Global", vars.esp.global.esp_global },
				}},
				new CMenuGroupbox { "Players", {
					new CItemCheckbox{ "ESP Players", vars.esp.players.esp_players },
					new CItemCheckbox{ "Enemy Only", vars.esp.players.esp_players_enemy_only },
					new CItemCombobox{ "Color Scheme", vars.colors.esp.players.esp_player_color_scheme, { "Enemy/Team", "Red/Blu" } },
					new CItemCheckbox{ "Name", vars.esp.players.players_name },
					new CItemCheckbox{ "Box", vars.esp.players.players_box },
					new CItemCheckbox{ "Health bar", vars.esp.players.players_healthbar },
					new CItemSliderInt{ "Lines", vars.esp.players.players_healthbar_lines_count, 0, 10 },
					new CItemCheckbox{ "Conditions", vars.esp.players.players_conds },
					new CItemCheckbox{ "Weapons", vars.esp.players.players_weapons },
					new CItemCheckbox{ "Distance", vars.esp.players.players_distance },
					//spy warning
					new CItemCheckbox{ "High K/D (Legit)", vars.esp.players.players_high_kd },
					new CItemSliderFloat{ "K/D Amount", vars.esp.players.players_kd, 5.f, 10.f, 0.1f, "%.1f" },
					new CItemCheckbox{ "High Headshot Ratio (Legit)", vars.esp.players.players_high_headshot },
					new CItemSliderFloat{ "Ratio Amount", vars.esp.players.players_headshot, 0.5f, 1.0f, 0.01f },
				}},
				new CMenuGroupbox { "Buildings", {
					new CItemCheckbox{ "ESP Buildings", vars.esp.buildings.esp_buildings },
					new CItemCheckbox{ "Sentry Alert", vars.esp.buildings.sentry_alert },
					new CItemCheckbox{ "Enemy Only", vars.esp.buildings.esp_buildings_enemy_only },
					new CItemCombobox{ "Color Scheme", vars.colors.esp.buildings.esp_building_color_scheme, { "Enemy/Team", "Red/Blu" } },
					new CItemCheckbox{ "Name", vars.esp.buildings.buildings_name },
					new CItemCheckbox{ "Box", vars.esp.buildings.buildings_box },
					new CItemCheckbox{ "Health bar", vars.esp.buildings.buildings_healthbar },
					new CItemSliderInt{ "Lines", vars.esp.buildings.buildings_healthbar_lines_count, 0, 10 },
					new CItemCheckbox{ "Conditions", vars.esp.buildings.buildings_conds },
					new CItemCheckbox{ "Distance", vars.esp.buildings.buildings_distance },
				}},
				new CMenuGroupbox{ "World", {
					new CItemCheckbox{ "ESP World", vars.esp.world.esp_world },
					new CItemCheckbox{ "Name", vars.esp.world.world_name },
					new CItemCheckbox{ "Box", vars.esp.world.world_box },
					new CItemCheckbox{ "Distance", vars.esp.world.world_distance },
					new CItemCheckbox{ "Low Ammo (Legit)", vars.esp.world.low_ammo_esp },
					new CItemCheckbox{ "Low Health (Legit)", vars.esp.world.low_health_esp },
					new CItemCheckbox{ "Intel Alert (Legit)", vars.esp.world.intel_alert },
				}},
			}},

			new CMenuTab{ "Misc", {
				new CMenuGroupbox { "Movement", {
					new CItemCheckbox{ "Stop Movement", vars.misc.accurate_movement },
				}},
				new CMenuGroupbox { "View", {
					new CItemKeybind{ "Thirdperson Key", vars.visuals.visuals_thirdperson_key },
					new CItemCheckbox{ "Thirdperson Offset", vars.visuals.visuals_thirdperson_offset },
					new CItemSliderInt{ "Offset Forwards", vars.visuals.visuals_thirdperson_offset_forwards, -300, 300, 1 },
					new CItemSliderInt{ "Offset Sideways", vars.visuals.visuals_thirdperson_offset_sideways, -300, 300, 1 },
					new CItemSliderInt{ "Offset Upwards", vars.visuals.visuals_thirdperson_offset_upwards, -300, 300, 1 },
					new CItemCheckbox{ "Remove Scope", vars.visuals.visuals_remove_scope },
					new CItemSliderInt{ "FOV Override",  vars.visuals.visuals_fov, 30, 160 },
				}},
			}},

			new CMenuTab{ "Settings", {
				new CMenuGroupbox{ "Configs", {

				}},
				new CMenuGroupbox{ "Menu colors", {

				}},
				new CMenuGroupbox{ "Cheat Mode", {
					new CItemCheckbox{ "Legit Only", vars.global.legit_only }
				}},
			}},
		}
	};
}

void __window_menu::run_menu()
{
	if ( vars.global.legit_only )
	{
		if ( Init )
		{
			hooks::LockCursor::lockcursor_fn( );
			i::surface->SetCursorAlwaysVisible( 0 );
			menu.open = false;

			delete window;

			Init = false;
			InitializeLegitMenu( );
			LegitInit = true;
			return;
		}

		if ( !window )
		{
			InitializeLegitMenu( );
			LegitInit = true;
			return;
		}
	}
	else
	{
		if ( LegitInit )
		{
			hooks::LockCursor::lockcursor_fn( );
			i::surface->SetCursorAlwaysVisible( 0 );
			menu.open = false;

			delete window;

			LegitInit = false;
			InitializeMenu( );
			Init = true;
			return;
		}

		if ( !window )
		{
			InitializeMenu( );
			Init = true;
			return;
		}
	}

	window->update();
	window->render();

	if ( !input.in_focus( ) )
	{
		hooks::LockCursor::lockcursor_fn( );
		i::surface->SetCursorAlwaysVisible( 0 );
		menu.open = false;
	}
}

void __menu::render()
{
	if (GetAsyncKeyState(VK_INSERT) & 0x1)
	{
		i::surface->SetCursorAlwaysVisible(open = !open);
	}

	if (!open)
	{
		return;
	}

	input.update();

	window_menu.run_menu();
	//draw.outlined_string( MENU, 5, 800, { 255, 255, 255, 255 }, ALIGN_DEFAULT, L"%i", input.scroll );
	//draw.outlined_string( MENU, 5, 815, { 255, 255, 255, 255 }, ALIGN_DEFAULT, L"%i", input.hovered );
}

void __menu::reset()
{
	if ( open )
	{
		hooks::LockCursor::lockcursor_fn( );
		i::surface->SetCursorAlwaysVisible( 0 );
		open = false;
	}

	if (ogwndproc)
	{
		SetWindowLongW(gamehwnd, GWL_WNDPROC, reinterpret_cast<LONG>(ogwndproc));
	}
}