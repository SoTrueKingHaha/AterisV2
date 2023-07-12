#include "hooklist.h"
#include "../features/outlines/outlines.h"
#include "../features/indicators/indicators.h"
#include "../features/crits/crits.h"
#include "../features/antiaim/antiaim.h"
#include "../features/aimbot/proj_sim.h"
#include "../features/backtrack/backtrack.h"

MAKE_HOOK( Paint, memory->get_vfunc( i::engine_vgui, 14 ), void, __fastcall,
		   void *ECX, void *EDX, CUtlFlags<int> mode )
{
	CALL_ORIGINAL( ECX, EDX, mode );
	const auto& local = entity_cache.get_local( );
	const auto& weapon = entity_cache.get_weapon( );

	if ( !mode.IsFlagSet( PAINT_UIPANELS ) )
	{
		return;
	}

	if ( !draw.screen_width || !draw.screen_height )
		draw.update_screen_size( );

	draw.update_w2s_matrix( );

	input.hovered = false;
	i::surface->StartDrawing( );
	{
		if ( !i::engine_vgui->IsGameUIVisible( ) && i::engine_client->IsInGame( ) )
		{
			esp.render( );
			//debug_info( );
		}

		indicators.render( );
		menu.render( );

		/*if ( local && local->InCond( TF_COND_ZOOMED ) && ( vars.visuals.visuals_remove_scope || vars.visuals.visuals_remove_zoom ) )
		{
			draw.line( draw.screen_wcenter, 0, draw.screen_wcenter, draw.screen_height, { 0, 0, 0, 255 } );
			draw.line( 0, draw.screen_hcenter, draw.screen_width, draw.screen_hcenter, { 0, 0, 0, 255 } );
		}*/

		draw.outlined_string( ESP, 5, 55, { 255, 255, 255, 255 }, ALIGN_DEFAULT, "Project Langram" );
		std::wstring mode = vars.global.legit_only ? L"Legit Mode" : L"Rage Mode";
		color modecol = vars.global.legit_only ? color{ 25, 255, 25, 255 } : color{ 255, 25, 25, 255 };
		draw.outlined_string( ESP, 5, 67, modecol, ALIGN_DEFAULT, mode.c_str( ) );
	}
	i::surface->FinishDrawing( );
}

MAKE_HOOK( OnScreenSizeChanged, memory->get_vfunc( i::surface, 111 ), void, __fastcall,
		   void *ECX, void *EDX, int oldwidth, int oldheight )
{
	CALL_ORIGINAL( ECX, EDX, oldwidth, oldheight );
	draw.reload_fonts( );
	draw.update_screen_size( );
}

MAKE_HOOK( LockCursor, memory->get_vfunc( i::surface, 62 ), void, __fastcall,
		   void *ECX, void *EDX )
{
	cursor_backup.ECX = ECX;
	cursor_backup.EDX = EDX;

	if ( menu.open )
	{
		return i::surface->UnlockCursor( );
	}

	CALL_ORIGINAL( ECX, EDX );
}

void hooks::LockCursor::lockcursor_fn( )
{
	func.original<FN>( )( cursor_backup.ECX, cursor_backup.EDX );
}

void hooks::WndProc::init( )
{
	while ( !gamehwnd )
	{
		gamehwnd = FindWindowW( L"Valve001", nullptr );
		Sleep( 100 );
	}

	ogwndproc = reinterpret_cast< WNDPROC >( SetWindowLongW( gamehwnd, GWL_WNDPROC, reinterpret_cast< LONG >( detour ) ) );
}

LRESULT CALLBACK hooks::WndProc::detour( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if ( menu.open )
	{
		if ( uMsg == WM_CHAR )
		{
			input.key_pressed = static_cast< char >( wParam );
		}
		if ( uMsg == WM_MOUSEWHEEL )
		{
			if ( input.hovered )
			{
				input.scroll = GET_WHEEL_DELTA_WPARAM( wParam );
			}
		}
		if ( input.prevent_inputs )
		{
			i::input_system->ResetInputState( );
			return 0;
		}
		if ( i::engine_client->Con_IsVisible( ) || i::engine_vgui->IsGameUIVisible( ) )
		{
			if ( uMsg != WM_MOUSEWHEEL )
				return 1;
		}
	}

	return CallWindowProcW( ogwndproc, hwnd, uMsg, wParam, lParam );
}

MAKE_HOOK( FrameStageNotify, memory->get_vfunc( i::client, 35 ), void, __fastcall,
		   void *ECX, void *EDX, ClientFrameStage_t curStage )
{
	const auto& local = entity_cache.get_local( );
	if ( !vars.global.legit_only )
	{
		switch ( curStage )
		{
			case ClientFrameStage_t::FRAME_RENDER_START:
			{
				if ( local && local != nullptr && !local->IsDormant( ) )
				{
					if ( !local->deadflag( ) )
					{
						global_info.punch_angles = local->m_vecPunchAngle( );
						local->m_vecPunchAngle( ).zero( );
					}
				}
				break;
			}
			default: break;
		}
	}

	CALL_ORIGINAL( ECX, EDX, curStage );

	switch ( curStage )
	{
		default: break;

		case ClientFrameStage_t::FRAME_NET_UPDATE_START:
		{
			entity_cache.reset( );
			break;
		}

		case ClientFrameStage_t::FRAME_NET_UPDATE_END:
		{
			entity_cache.fill( );
			backtrack.fill( );

			for ( int i = 0; i < i::engine_client->GetMaxClients( ); i++ )
			{
				if ( const auto& player = i::entity_list->GetClientEntity( i )->As<C_TFPlayer>( ) )
				{
					const vel_fix_record record = { player->m_vecOrigin( ), player->m_fFlags( ).GetFlags( ), player->m_flSimulationTime( ) };
					global_info.vel_fix_records[ player ] = record;
				}
			}

			break;
		}


		if ( !vars.global.legit_only )
		{
			case ClientFrameStage_t::FRAME_RENDER_START:
			{
				if ( strstr( i::engine_client->GetLevelName( ), "tr_walkway" ) )
				{
					for ( const auto& pEntity : entity_cache.players[ TEAMSALL ] )
					{
						const auto& player = pEntity->As<C_TFPlayer>( );

						if ( player->m_iTeamNum( ) != 3 || player == local )
							continue;

						const auto& anim_state = player->m_PlayerAnimState( );

						if ( anim_state )
						{
							if ( anim_state->m_PoseParameterData.m_iMoveX )
							{
								anim_state->m_PoseParameterData.m_iMoveX = 0;
							}
							if ( anim_state->m_PoseParameterData.m_iMoveY )
							{
								anim_state->m_PoseParameterData.m_iMoveY = 0;
							}
						}
					}
				}

				break;
			}
		}
	}
}

MAKE_HOOK( CL_Move, S::CL_Move.address, void, __cdecl, float accumulated_extra_samples, bool final_tick )
{
	if ( ( !vars.misc.cl_move.warp && !vars.misc.cl_move.doubletap ) || vars.global.legit_only || !i::engine_client->IsConnected( ) )
	{
		return CALL_ORIGINAL( accumulated_extra_samples, final_tick );
	}

	if ( utils.is_window_in_focus( ) && vars.misc.cl_move.recharge_key && ( GetAsyncKeyState( vars.misc.cl_move.recharge_key ) & 0x8001 ) )
	{
		global_info.recharging = true;
	}

	if ( global_info.shifted < 22 && global_info.recharging )
	{
		global_info.shifted++;
		return;
	}
	else
	{
		global_info.recharging = false;
	}

	CALL_ORIGINAL( accumulated_extra_samples, final_tick );

	if ( vars.misc.cl_move.warp )
	{
		if ( utils.is_window_in_focus( ) && vars.misc.cl_move.warp_key && ( GetAsyncKeyState( vars.misc.cl_move.warp_key ) & 0x8001 ) )
		{
			if ( vars.misc.cl_move.smooth_warp )
			{
				if ( global_info.shifted )
				{
					CALL_ORIGINAL( accumulated_extra_samples, global_info.shifted == 1 );
					i::cvar->ConsoleColorPrintf( { 0, 255, 255, 255 }, "|Smooth Warp| CL_Move( %i - %.3f, %d )\n", global_info.shifted, global_info.shifted * i::global_vars->interval_per_tick, global_info.shifted == 1 );
					global_info.shifted--;
				}
			}
			else
			{
				while ( global_info.shifted )
				{
					CALL_ORIGINAL( accumulated_extra_samples, global_info.shifted == 1 );
					i::cvar->ConsoleColorPrintf( { 0, 255, 255, 255 }, "|Instant Warp| CL_Move( %i - %.3f, %d )\n", global_info.shifted, global_info.shifted * i::global_vars->interval_per_tick, global_info.shifted == 1 );
					global_info.shifted--;
				}
			}
		}
	}

	if ( vars.misc.cl_move.doubletap )
	{
		if ( utils.is_window_in_focus( ) && global_info.shifting )
		{
			while ( global_info.shifted > global_info.ticks )
			{
				//accumulated_extra_samples = 0.0f;
				CALL_ORIGINAL( accumulated_extra_samples, global_info.shifted == global_info.ticks + 1 );
				//i::cvar->ConsoleColorPrintf( { 0, 255, 255, 255 }, "[Doubletap] CL_Move( %i - %.8f, %d )\n", global_info.shifted, accumulated_extra_samples, global_info.shifted == global_info.ticks + 1 );
				global_info.shifted--;
				i::engine_client->FireEvents( );
			}

			global_info.shifting = false;
		}
	}
}

MAKE_HOOK( CreateMove, memory->get_vfunc( i::client_mode, 21 ), bool, __fastcall,
		   void *ECX, void *EDX, float input_sample_time, CUserCmd *cmd )
{
	const auto &local = entity_cache.get_local( );
	const auto &weapon = entity_cache.get_weapon( );

	if ( !cmd || !cmd->command_number || !local || !weapon || local->deadflag( ) )
		return CALL_ORIGINAL( ECX, EDX, input_sample_time, cmd );

	if ( CALL_ORIGINAL( ECX, EDX, input_sample_time, cmd ) )
		i::prediction->SetLocalViewAngles( cmd->viewangles );

	uintptr_t _bp;
	__asm mov _bp, ebp;
	auto send_packet = reinterpret_cast< bool * >( ***reinterpret_cast< uintptr_t *** >( _bp ) - 0x1 );

	vector oldview = cmd->viewangles;
	float oldside = cmd->sidemove;
	float oldforward = cmd->forwardmove;

	global_info.current_user_cmd = cmd;
	global_info.real_angles = cmd->viewangles;
	global_info.can_attack = weapon->CanShoot( );
	global_info.attacking = utils.is_attacking( cmd, local, weapon );
	aimbot.weapon_type = utils.get_weapon_type( weapon );

	static int nOldClass = local->m_iClass( );
	if ( local->m_iClass( ) != nOldClass )
	{
		crithack.potential_crits = { 0, 0, 0 };
		crithack.crits = { 0, 0, 0 };
		crithack.damage_correct = { 0, 0, 0 };
		crithack.bucket = { 300, 300, 300 };
		nOldClass = local->m_iClass( );
	}

	if ( local->IsClass( CLASS_SCOUT ) )
	{
		global_info.ticks = 2;
	}
	else
	{
		global_info.ticks = 1;
	}

	misc.run( cmd );

	engine_prediction.start( local, cmd );
	{
		//createmove shit
		if ( !vars.global.legit_only )
		{
			aimbot.run( cmd, send_packet );
			backtrack.run( local, weapon, cmd );
			aimbot.detonator_run( local, weapon, cmd );
			if ( aimbot.weapon_type == EWeaponType::PROJECTILE )
			{
				aimbot.aimbot_psilent( cmd, oldview, oldside, oldforward, send_packet );
			}

			if ( vars.antiaim.antiaim_active )
			{
				if ( cmd->command_number % 2 == 0 )
				{
					antiaim.run_real( local, weapon, cmd, send_packet );
					global_info.real_viewangles = cmd->viewangles;
					*send_packet = false;
				}
				else
				{
					antiaim.run_fake( local, weapon, cmd, send_packet );
					global_info.fake_viewangles = cmd->viewangles;
					*send_packet = true;
				}
				//g_GlobalInfo.m_vFakeViewAngles = fakeangle;
				//g_GlobalInfo.m_vRealViewAngles = realangle;
			}
			/*if ( cmd->buttons & IN_ATTACK && !cmd->hasbeenpredicted )
			{
				projectile_simulation::run_simulation( local, weapon, cmd->viewangles );
			}*/

			/*for ( const auto& player : entity_cache.players[ ENEMY ] )
			{
				CMoveData data;
				vector origin;
				global_info.player_prediction_lines[ player->entindex( ) ].clear( );
				if ( move_sim.Initialize( player ) )
				{
					for ( int i = 0; i < TIME_TO_TICKS( 1.0f ); i++ )
					{
						move_sim.RunTick( data, origin );
						global_info.player_prediction_lines[ player->entindex( ) ].push_back( origin );
					}
					move_sim.Restore( );
				}
			}*/
		}
		else
		{
			//legit aimbot
		}
	}
	engine_prediction.end( local, cmd );

	crithack.run( local, weapon, cmd );

	if ( !vars.global.legit_only )
	{
		misc.run_late( cmd );

		static int choked = 0;
		if ( global_info.shifting )
		{
			global_info.shift_user_cmd = cmd;
			update_attack( );
			*send_packet = global_info.shifted == global_info.ticks + 1;
			//input_sample_time = 0.0f;
			//i::cvar->ConsoleColorPrintf( { 0, 255, 255, 255 }, "[Doubletap] CreateMove( %i - %.8f, %d )\n", cmd->command_number, input_sample_time, *send_packet );
		}

		{
			static int nChoked = 0;
			const int nAmount = 22;

			if ( !*send_packet )
				nChoked++;

			else nChoked = 0;

			if ( nChoked > nAmount )
				*send_packet = true;
		}

		if ( global_info.silent_time
			 || global_info.hitscan_running
			 || global_info.scout_jumping
			 || global_info.antiaim_active
			 || ( global_info.shifting ) )
		{
			return false;
		}
	}

	global_info.last_user_cmd = cmd;
	return CALL_ORIGINAL( ECX, EDX, input_sample_time, cmd );
}

MAKE_HOOK( OverrideView, memory->get_vfunc( i::client_mode, 16 ), void, __fastcall,
		   void* ECX, void* EDX, CViewSetup* view )
{
	if ( !view )
	{
		return;
	}

	const auto& local = entity_cache.get_local( );

	if ( !local || local->deadflag( ) )
	{
		return;
	}

	misc.fov( local, view );
	misc.third_person( local, view );
}

MAKE_HOOK( CL_SendMove, S::CL_SendMove.address, void, __cdecl,
		   void* ECX, void* EDX )
{
	if ( !vars.global.legit_only )
	{
		if ( const auto& pLocal = entity_cache.get_local( ) )
		{
			if ( pLocal->m_lifeState( ) != LIFE_ALIVE )
			{
				return CALL_ORIGINAL( ECX, EDX );
			}
		}
		byte data[ 4000 ];

		const int nextcommandnr = i::client_state->lastoutgoingcommand + i::client_state->chokedcommands + 1;

		CLC_Move moveMsg;
		moveMsg.m_DataOut.StartWriting( data, sizeof( data ) );
		moveMsg.m_nNewCommands = std::clamp( 1 + i::client_state->chokedcommands, 0, MAX_NEW_COMMANDS );
		const int extraCommands = i::client_state->chokedcommands + 1 - moveMsg.m_nNewCommands;
		const int backupCommands = std::max( 2, extraCommands );
		moveMsg.m_nBackupCommands = std::clamp( backupCommands, 0, MAX_BACKUP_COMMANDS );

		const int numcmds = moveMsg.m_nNewCommands + moveMsg.m_nBackupCommands;

		int from = -1;
		bool bOK = true;
		for ( int to = nextcommandnr - numcmds + 1; to <= nextcommandnr; to++ )
		{
			const bool isnewcmd = to >= nextcommandnr - moveMsg.m_nNewCommands + 1;
			bOK = bOK && i::client->WriteUsercmdDeltaToBuffer( &moveMsg.m_DataOut, from, to, isnewcmd );
			from = to;
		}

		if ( bOK )
		{
			if ( extraCommands )
			{
				i::client_state->m_NetChannel->m_nChokedPackets -= extraCommands;
			}
			static auto SendNetMsg = memory->get_vfunc<bool( __thiscall* )( PVOID, INetMessage * msg, bool, bool )>( i::client_state->m_NetChannel, 37 );
			SendNetMsg( i::client_state->m_NetChannel, &moveMsg, false, false );
		}
	}
	else
	{
		return CALL_ORIGINAL( ECX, EDX );
	}
}

MAKE_HOOK( SendNetMsg, S::CNetChan_SendNetMsg.address, bool, __fastcall,
		   CNetChannel* CNetChan, void* EDX, INetMessage& msg, bool bForceReliable, bool bVoice )
{
	if ( !vars.global.legit_only )
	{
		switch ( msg.GetType( ) )
		{
			case clc_VoiceData:
			{
				bVoice = true;
				break;
			}

			case clc_FileCRCCheck:
			{
				if ( vars.visuals.bypass_svpure )
				{
					return false;
				}
				break;
			}

			case clc_Move:
			{
				const int iAllowedNewCommands = 24 - ( global_info.shifted );
				const auto& moveMsg = reinterpret_cast< CLC_Move& >( msg );
				if ( moveMsg.m_nNewCommands > iAllowedNewCommands )
				{
					global_info.shifted -= moveMsg.m_nNewCommands - iAllowedNewCommands;
				}
				break;
			}
		}
	}

	return CALL_ORIGINAL( CNetChan, EDX, msg, bForceReliable, bVoice );
}

void ClearEffects( C_BaseEntity* pEntity )
{
	*reinterpret_cast< bool* >( reinterpret_cast< DWORD >( pEntity ) + 0xC91 ) = false; // Gib
	*reinterpret_cast< bool* >( reinterpret_cast< DWORD >( pEntity ) + 0xC92 ) = false; // Burning
	*reinterpret_cast< bool* >( reinterpret_cast< DWORD >( pEntity ) + 0xC93 ) = false; // Electrocuted
	*reinterpret_cast< bool* >( reinterpret_cast< DWORD >( pEntity ) + 0xC99 ) = false; // Become ash
	*reinterpret_cast< bool* >( reinterpret_cast< DWORD >( pEntity ) + 0xCA0 ) = false; // Gold
	*reinterpret_cast< bool* >( reinterpret_cast< DWORD >( pEntity ) + 0xCA1 ) = false; // Ice
}

MAKE_HOOK( CreateTFRagdoll, S::CreateTFRagdoll.address, void, __fastcall,
		   void* ECX, void* EDX )
{
	if ( vars.visuals.ragdolls_active && !vars.global.legit_only )
	{
		i::engine_client->ClientCmd_Unrestricted(
			"cl_ragdoll_fade_time 15; cl_ragdoll_forcefade 0; cl_ragdoll_physics_enable 1; g_ragdoll_fadespeed 600; g_ragdoll_lvfadespeed 100; ragdoll_sleepaftertime 5.0f" );

		if ( const auto& pEntity = static_cast< C_BaseEntity* >( ECX ) )
		{
			if ( vars.visuals.ragdolls_enemy_only )
			{
				if ( const auto& pLocal = entity_cache.get_local( ) )
				{
					if ( *reinterpret_cast< int* >( reinterpret_cast< DWORD >( pEntity ) + 0xCBC ) == pLocal->m_iTeamNum( ) )
					{
						//Team offset
						return;
					}
				}
			}

			ClearEffects( pEntity );

			*reinterpret_cast< bool* >( reinterpret_cast< DWORD >( pEntity ) + 0xC92 ) = vars.visuals.ragdolls_particle == 1;
			*reinterpret_cast< bool* >( reinterpret_cast< DWORD >( pEntity ) + 0xC93 ) = vars.visuals.ragdolls_particle == 2;
			*reinterpret_cast< bool* >( reinterpret_cast< DWORD >( pEntity ) + 0xC99 ) = vars.visuals.ragdolls_ash;
			*reinterpret_cast< bool* >( reinterpret_cast< DWORD >( pEntity ) + 0xC95 ) = vars.visuals.ragdolls_dissolve;
			*reinterpret_cast< bool* >( reinterpret_cast< DWORD >( pEntity ) + 0xCA0 ) = vars.visuals.ragdolls_effect == 1;
			*reinterpret_cast< bool* >( reinterpret_cast< DWORD >( pEntity ) + 0xCA1 ) = vars.visuals.ragdolls_effect == 2;
		}
	}

	CALL_ORIGINAL( ECX, EDX );
}

MAKE_HOOK( FireBullets, S::C_BaseEntity_FireBullets.address, void, __fastcall,
		   void* ECX, void* EDX, C_TFWeaponBase* weapon, const FireBulletsInfo_t& info, bool do_effects, int damage_type, int custom_damage_type )
{
	if ( !weapon )
	{
		return CALL_ORIGINAL( ECX, EDX, weapon, info, do_effects, damage_type, custom_damage_type );
	}

	const auto& local = entity_cache.get_local( );
	if ( !local )
	{
		return CALL_ORIGINAL( ECX, EDX, weapon, info, do_effects, damage_type, custom_damage_type );
	}

	const vector vStart = info.m_vecSrc;
	const vector vEnd = vStart + info.m_vecDirShooting * info.m_flDistance;

	CGameTrace trace = {};
	CTraceFilterHitscan filter = {};
	filter.pSkip = local;

	/* if ur shooting thru stuff, change MASK_SHOT to MASK_SOLID - fatherless */
	utils.trace( vStart, vEnd, ( MASK_SHOT /* | CONTENTS_GRATE | MASK_VISIBLE*/ ), &filter, &trace );

	const int iAttachment = weapon->LookupAttachment( "muzzle" );
	const int team = local->m_iTeamNum( );
	weapon->GetAttachment( iAttachment, trace.m_vStartPos );

	if ( !local->IsInValidTeam( ) )
	{
		return;
	}

	/*local->IsCritBoosted( ) 
		? ( utils.particle_tracer( team == TEAM_RED ? "bullet_tracer_raygun_red_crit" : "bullet_tracer_raygun_blue_crit", trace.m_vStartPos, trace.m_vEndPos, local->entindex( ), iAttachment, true ) )
		: ( utils.particle_tracer( team == TEAM_RED ? "bullet_tracer_raygun_red" : "bullet_tracer_raygun_blue", trace.m_vStartPos, trace.m_vEndPos, local->entindex( ), iAttachment, true ) );*/

	if ( local )
	{
		switch ( vars.visuals.visuals_tracer )
		{
			//Machina
			case 1:
			{
				utils.particle_tracer( team == 2 ? "dxhr_sniper_rail_red" : "dxhr_sniper_rail_blue", trace.m_vStartPos, trace.m_vEndPos, local->entindex( ), iAttachment, true );
				break;
			}

			//C.A.P.P.E.R
			case 2:
			{
				local->IsCritBoosted( )
					? ( utils.particle_tracer( team == 2 ? "bullet_tracer_raygun_red_crit" : "bullet_tracer_raygun_blue_crit",
						trace.m_vStartPos, trace.m_vEndPos, local->entindex( ), iAttachment, true ) )
					: ( utils.particle_tracer( team == 2 ? "bullet_tracer_raygun_red" : "bullet_tracer_raygun_blue", trace.m_vStartPos,
						trace.m_vEndPos, local->entindex( ), iAttachment, true ) );
				break;
			}

			//Short circuit
			case 3:
			{
				utils.particle_tracer( team == 2 ? "dxhr_lightningball_hit_zap_red" : "dxhr_lightningball_hit_zap_blue", trace.m_vStartPos, trace.m_vEndPos, local->entindex( ), iAttachment, true );
				break;
			}

			//Merasmus ZAP
			case 4:
			{
				utils.particle_tracer( "merasmus_zap", trace.m_vStartPos, trace.m_vEndPos, local->entindex( ), iAttachment, true );
				break;
			}

			//Merasmus ZAP Beam 2
			case 5:
			{
				utils.particle_tracer( "merasmus_zap_beam02", trace.m_vStartPos, trace.m_vEndPos, local->entindex( ), iAttachment, true );
				break;
			}

			case 6:
			{
				local->IsCritBoosted( )
					? ( utils.particle_tracer(
					team == 2 ? "bullet_bignasty_tracer01_red_crit" : "bullet_bignasty_tracer01_blue_crit",
					trace.m_vStartPos, trace.m_vEndPos, local->entindex( ), iAttachment, true ) )
					: ( utils.particle_tracer( team == 2 ? "bullet_bignasty_tracer01_blue" : "bullet_bignasty_tracer01_red",
						trace.m_vStartPos, trace.m_vEndPos, local->entindex( ), iAttachment, true ) );
				break;
			}
			case 7:
			{
				i::beams->DrawBeam( trace.m_vStartPos, trace.m_vEndPos,
									vars.visuals.custom_beam ? vars.colors.misc.beam_custom : ( local->m_iTeamNum( ) == TEAM_RED ? vars.colors.misc.beam_red : vars.colors.misc.beam_blu ),
									vars.visuals.beam_time, vars.visuals.beam_size );
				break;
			}

			default: break;
		}
	}
}

MAKE_HOOK( ViewmodelPos, S::ViewmodelPos.address, void, __fastcall,
		   void* ecx, void* edx, C_TFPlayer* owner, vector& eye_pos, vector& eye_angles )
{
	if ( vars.visuals.viewmodel_pos )
	{
		vector AxisX{}, AxisY{}, AxisZ{};

		math.angle_vectors( eye_angles, &AxisY, &AxisX, &AxisZ );

		vector eyepos = eye_pos + ( AxisX * vars.visuals.viewmodel_pos_x ) + ( AxisY * vars.visuals.viewmodel_pos_y ) + ( AxisZ * vars.visuals.viewmodel_pos_z );
		eye_angles.z += vars.visuals.viewmodel_roll;

		CALL_ORIGINAL( ecx, edx, owner, eyepos, eye_angles );
	}
	else
	{
		CALL_ORIGINAL( ecx, edx, owner, eye_pos, eye_angles );
	}
}

MAKE_HOOK( StartDynamicSound, S::S_StartDynamicSound.address, int, __cdecl,
		   StartSoundParams_t& params )
{
	misc.dormant_esp( params );
	return CALL_ORIGINAL( params );
}

MAKE_HOOK( ProcessDSP, S::ProcessDSP.address, void, __cdecl,
		   unsigned int idsp, int* pbfront, int* pbrear, int* pbcenter, int sampleCount )
{
	if ( vars.global.legit_only )
	{
		return CALL_ORIGINAL( idsp, pbfront, pbrear, pbcenter, sampleCount );
	}
}

MAKE_HOOK( PostProcessing, S::DoEnginePostProcessing.address, void, __cdecl,
		   int x, int y, int w, int h, bool bFlashlightIsOn, bool bPostVGui )
{
	if ( vars.global.legit_only )
	{
		return CALL_ORIGINAL( x, y, w, h, bFlashlightIsOn, bPostVGui );
	}
}

MAKE_HOOK( ScoreboardKV, S::KeyValues_SetInt.address, void, __fastcall,
		   void* ECX, void* EDX, const char* keyName, int value )
{
	CALL_ORIGINAL( ECX, EDX, keyName, value );

	if ( reinterpret_cast< DWORD >( _ReturnAddress( ) ) == S::Scoreboard_ReturnAddressDesired.address && std::string_view( keyName ).find( "nemesis" ) != std::string_view::npos )
		*reinterpret_cast< uintptr_t* >( _AddressOfReturnAddress( ) ) = S::Scoreboard_ReturnAddressJump.address;
}

MAKE_HOOK( IsPlayerDominated, S::IsPlayerDominated.address, bool, __fastcall,
		   void* ECX, void* EDX, int index )
{
	bool result = CALL_ORIGINAL( ECX, EDX, index );

	if ( !result )
	{
		if ( reinterpret_cast< DWORD >( _ReturnAddress( ) ) == S::IsPlayerDominated_ReturnAddressDesired.address )
			*reinterpret_cast< uintptr_t* >( _AddressOfReturnAddress( ) ) = S::Scoreboard_ReturnAddressJump.address;
	}

	return result;
}

MAKE_HOOK( NotificationQueue_Add, S::NotificationQueue_Add.address, int, __cdecl,
		   CEconNotification* notification )
{
	if ( reinterpret_cast< DWORD >( _ReturnAddress( ) ) == S::CTFPlayer_OnHasNewItems_NotificationQueue_Add_Call.address )
	{
		notification->Accept( );
		notification->Trigger( );
		notification->UpdateTick( );
		notification->MarkForDeletion( );
		return 0;
	}

	return CALL_ORIGINAL( notification );
}

MAKE_HOOK( PaintTraverse, memory->get_vfunc( i::vgui_panel, 41 ), void, __fastcall, 
		   void* ECX, void* EDX, unsigned int vgui_panel, bool force_repaint, bool allow_force )
{
	if ( misc.remove_scope( vgui_panel ) )
	{
		return;
	}

	CALL_ORIGINAL( ECX, EDX, vgui_panel, force_repaint, allow_force );
}

MAKE_HOOK( InCond, S::CTFPlayerShared_InCond.address, bool, __fastcall,
		   void* ECX, void* EDX, ETFCond id )
{
	const auto dwRetAddr = reinterpret_cast< DWORD >( _ReturnAddress( ) );

	if ( id == TF_COND_ZOOMED )
	{
		if ( dwRetAddr == S::PlayerShouldDraw_InCond.address || dwRetAddr == S::WearableShouldDraw_InCond.address 
			 || ( vars.visuals.visuals_remove_scope && dwRetAddr == S::HudScopeShouldDraw_InCond.address ) )
		{
			return false;
		}
	}

	auto GetOuter = [ &ECX ] ( ) -> C_BaseEntity*
	{
		static const auto dwShared = netvars.find_netvar( "CTFPlayer", "m_Shared" );
		static const auto dwBombHeadStage = netvars.find_netvar(
			"CTFPlayer", "m_nHalloweenBombHeadStage" );
		static const auto dwOff = ( dwBombHeadStage - dwShared ) + 0x4;
		return *reinterpret_cast< C_BaseEntity** >( reinterpret_cast< DWORD >( ECX ) + dwOff );
	};

	return CALL_ORIGINAL( ECX, EDX, id );
}

MAKE_HOOK( ShouldDrawViewmodel, memory->get_vfunc( i::client_mode, 24 ), bool, __fastcall,
		   void* ECX, void* EDX )
{
	if ( const auto& local = entity_cache.get_local( ); local )
	{
		if ( !local->deadflag( ) && local->IsClass( CLASS_SNIPER ) && local->InCond( TF_COND_ZOOMED ) && vars.visuals.visuals_remove_scope && !vars.global.legit_only && !i::input->CAM_IsThirdPerson( ) )
		{
			return true;
		}
	}

	return CALL_ORIGINAL( ECX, EDX );
}

MAKE_HOOK( ClientState_InterpAmount, S::ClientState_GetClientInterpAmount.address, float, __fastcall,
		   void* ECX, void* EDX )
{
	const float retValue = CALL_ORIGINAL( ECX, EDX );
	global_info.lerp_time = retValue;
	return ( vars.misc.disable_interp /* || global_info.recharging*/ && !vars.global.legit_only ) ? 0.f : retValue;
}

MAKE_HOOK( GetClientInterpAmount, S::GetClientInterpAmount.address, float, __cdecl )
{
	return ( vars.misc.disable_interp /* || global_info.recharging*/ && !vars.global.legit_only ) ? 0.f : CALL_ORIGINAL( );
}

MAKE_HOOK( C_BaseEntity_Interpolate, S::C_BaseEntity_Interpolate.address, bool, __fastcall,
		   void* ECX, void* EDX, float current_time )
{
	return ( vars.misc.disable_interp /* || global_info.recharging*/ && !vars.global.legit_only ) ? true : CALL_ORIGINAL( ECX, EDX, current_time );
}

MAKE_HOOK( C_BaseAnimating_Interpolate, S::C_BaseAnimating_Interpolate.address, bool, __fastcall,
		   void* ECX, void* EDX, float current_time )
{
	return ( vars.misc.disable_interp /* || global_info.recharging*/ && !vars.global.legit_only ) ? true : CALL_ORIGINAL( ECX, EDX, current_time );
}

MAKE_HOOK( CInterpolatedVarArrayBase_Interpolate, S::CInterpolatedVarArrayBase_Interpolate.address, int, __fastcall,
		   void* ECX, void* EDX, float currentTime, float interpolation_amount )
{
	if ( vars.misc.disable_interp /* || global_info.recharging*/ && !vars.global.legit_only ) { return 1; }
	return CALL_ORIGINAL( ECX, EDX, currentTime, interpolation_amount );
}

MAKE_HOOK( EngineClient_IsPlayingTimeDemo, memory->get_vfunc( i::engine_client, 78 ), bool, __fastcall,
		   void* ECX, void* EDX )
{
	if ( vars.misc.disable_interp && !vars.global.legit_only )
	{
		if ( reinterpret_cast< DWORD >( _ReturnAddress( ) ) == ( S::InterpolateServerEntities.address + 0xB8 ) )
		{
			return true;
		}
	}

	return CALL_ORIGINAL( ECX, EDX );
}

/*int CalculateTick( int simTicks, C_BasePlayer* player )
{
	const int clockcorrect = TIME_TO_TICKS( 0.06f ); //sv_clockcorrection_msecs 60 ~4 ticks

	const int nIdealFinalTick = i::global_vars->tickcount + TIME_TO_TICKS( i::engine_client ->GetNetChannelInfo( )->GetLatency( FLOW_OUTGOING ) ) + clockcorrect;

	const int estimatedFinal = player->m_nTickBase( ) + simTicks;

	const int fast = nIdealFinalTick + clockcorrect;
	const int slow = nIdealFinalTick - clockcorrect;

	if ( estimatedFinal > fast || estimatedFinal < slow ) {
		return nIdealFinalTick - simTicks;
	}

	// this is useless
	return estimatedFinal;
}

MAKE_HOOK( RunCommand, memory->get_vfunc( i::prediction, 17 ), void, __fastcall,
		   void* ECX, void* EDX, C_BasePlayer* player, CUserCmd* cmd, IMoveHelper* move_helper )
{
	const auto& pLocal = entity_cache.get_local( );

	if ( player && !player->deadflag( ) && pLocal && !pLocal->deadflag( ) && pLocal == player && cmd && cmd->command_number )
	{
		const int backupTick = player->m_nTickBase( );
		const float curtimeBackup = i::global_vars->curtime;

		if ( vars.misc.cl_move.doubletap && global_info.should_shift )
		{
			if ( cmd->command_number == global_info.last_user_cmd->command_number )
			{
				player->SetTickBase( CalculateTick( i::client_state->chokedcommands + global_info.shifted + 1,
									  player ) );
				i::global_vars->curtime = TICKS_TO_TIME( player->m_nTickBase( ) );
			}
		}

		CALL_ORIGINAL( ECX, EDX, player, cmd, move_helper );

		if ( vars.misc.cl_move.doubletap && global_info.should_shift )
		{
			if ( cmd->command_number == global_info.last_user_cmd->command_number )
			{
				player->SetTickBase( backupTick );
				i::global_vars ->curtime = curtimeBackup;
			}
		}
	}
	else
	{
		CALL_ORIGINAL( ECX, EDX, player, cmd, move_helper );
	}
}*/

MAKE_HOOK( GetMaxItemCount, S::GetMaxItemCount.address, int, __fastcall,
		   void* ECX, void* EDX )
{
	return 3000;
}

std::unordered_map<void*, std::pair<int, float>> pAnimatingInfo;
MAKE_HOOK( FrameAdvance, S::FrameAdvance.address, float, __fastcall,
		   void* ECX, void* EDX, float interval )
{
	if ( const auto player = static_cast< C_TFPlayer* >( ECX ) )
	{
		if ( player->IsPlayer( ) )
		{
			if ( player->m_flSimulationTime( ) == player->m_flOldSimulationTime( ) || pAnimatingInfo[ ECX ].first == i::global_vars->tickcount )
			{
				pAnimatingInfo[ ECX ].second += interval;
				return 0.0f;
			}
		}
	}

	interval = pAnimatingInfo[ ECX ].second; pAnimatingInfo[ ECX ].second = 0.0f; pAnimatingInfo[ ECX ].first = i::global_vars->tickcount;
	return CALL_ORIGINAL( ECX, EDX, interval );
}

MAKE_HOOK( LevelInitPre, memory->get_vfunc( i::client, 5 ), void, __fastcall,
		   void* ECX, void* EDX, char const* map_name )
{
	float rate = 1.f / i::global_vars->interval_per_tick;

	convars.cl_updaterate->SetValue( rate );
	convars.cl_cmdrate->SetValue( rate );

	CALL_ORIGINAL( ECX, EDX, map_name );
}

MAKE_HOOK( LevelShutdown, memory->get_vfunc( i::client, 7 ), void, __fastcall,
		   void* ECX, void* EDX )
{
	CALL_ORIGINAL( ECX, EDX );
	entity_cache.reset( );
}

MAKE_HOOK( CheckForSequenceChange, S::CheckForSequenceChange.address, void, __fastcall,
		   void* ECX, void* EDX, CStudioHdr* hdr, int nCurSequence, bool bForceNewSequence, bool bInterpolate )
{
	return CALL_ORIGINAL( ECX, EDX, hdr, nCurSequence, bForceNewSequence, vars.global.legit_only ? bInterpolate : false );
}

void AdjustPlayerTimeBase( C_BasePlayer* local, int simulation_ticks )
{
	if ( simulation_ticks < 0 )
		return;

	if ( i::global_vars->maxClients == 1 )
	{
		// set TickBase so that player simulation tick matches gpGlobals->tickcount after
		// all commands have been executed
		local->SetTickBase( i::global_vars->tickcount - simulation_ticks + i::global_vars->simTicksThisFrame );
		i::cvar->ConsoleColorPrintf( { 0, 255, 255, 255 }, "|RunCommand - Singleplayer| Adjusted Local TickCount to %i.\n", i::global_vars->tickcount - simulation_ticks + i::global_vars->simTicksThisFrame );
	}
	else // multiplayer
	{
		float flCorrectionSeconds = std::clamp( convars.find_var( "sv_clockcorrection_msecs" )->GetFloat( ) / 1000.0f, 0.0f, 1.0f );
		int nCorrectionTicks = TIME_TO_TICKS( flCorrectionSeconds );

		// Set the target tick flCorrectionSeconds (rounded to ticks) ahead in the future. this way the client can
		//  alternate around this target tick without getting smaller than gpGlobals->tickcount.
		// After running the commands simulation time should be equal or after current gpGlobals->tickcount, 
		//  otherwise the simulation time drops out of the client side interpolated var history window.

		//int	nIdealFinalTick = i::global_vars->tickcount + i::client_state->chokedcommands + 1 + TIME_TO_TICKS( i::engine_client->GetNetChannelInfo( )->GetLatency( FLOW_OUTGOING ) ) + nCorrectionTicks;
		int	nIdealFinalTick = i::client_state->m_ClockDriftMgr.server_tick + 1 + TIME_TO_TICKS( i::client_state->m_NetChannel->GetLatency( FLOW_OUTGOING ) ) + nCorrectionTicks;

		int nEstimatedFinalTick = local->m_nTickBase( ) + simulation_ticks;

		// If client gets ahead of this, we'll need to correct
		int	 too_fast_limit = nIdealFinalTick + nCorrectionTicks;
		// If client falls behind this, we'll also need to correct
		int	 too_slow_limit = nIdealFinalTick - nCorrectionTicks;

		// See if we are too fast
		if ( nEstimatedFinalTick > too_fast_limit ||
			 nEstimatedFinalTick < too_slow_limit )
		{
			int nCorrectedTick = nIdealFinalTick - simulation_ticks;

			i::cvar->ConsoleColorPrintf( { 0, 255, 255, 255 }, "|RunCommand - Multiplayer| Adjusted Local TickCount from %i to %i.\n", local->m_nTickBase( ), nCorrectedTick );
			local->SetTickBase( nCorrectedTick );
		}
	}
}

/*MAKE_HOOK( RunSimulation, S::RunSimulation.address, void, __fastcall,
		   void* ECX, void* EDX, int current_command, float curtime, CUserCmd* cmd, C_BasePlayer* local )
{
	/*const int iArrivalTick = TIME_TO_TICKS( local->m_nTickBase( ) * i::global_vars->interval_per_tick );
	const int iCmdMaxTickDelta = ( 1.f / i::global_vars->interval_per_tick ) * 2.5f;
	const int iMinDelta = std::max( 0, iArrivalTick - iCmdMaxTickDelta );
	const int iMaxDelta = iArrivalTick + iCmdMaxTickDelta;

	const bool bValid =
		( cmd->tick_count >= iMinDelta && cmd->tick_count < iMaxDelta ) &&
		( cmd->viewangles.is_valid( ) && IsEntityQAngleReasonable( cmd->viewangles ) ) &&
		( IsFinite( cmd->forwardmove ) && IsEntityCoordinateReasonable( cmd->forwardmove ) ) &&
		( IsFinite( cmd->sidemove ) && IsEntityCoordinateReasonable( cmd->sidemove ) ) &&
		( IsFinite( cmd->upmove ) && IsEntityCoordinateReasonable( cmd->upmove ) );

	if ( !bValid )
	{
		//keep our clock in sync
		local->m_nTickBase( )++;
		cmd->MakeInert( );

		//save off the current results,
		//since this command will get 'discarded'
		return;
	}

	if ( global_info.shifted < 22 && !global_info.recharging )
	{
		AdjustPlayerTimeBase( local, global_info.shifted );
		i::global_vars->curtime = TICKS_TO_TIME( local->m_nTickBase( ) );
	}

	CALL_ORIGINAL( ECX, EDX, current_command, curtime, cmd, local );
}

MAKE_HOOK( RunCommand, memory->get_vfunc( i::prediction, 17 ), void, __fastcall,
		   void* ECX, void* EDX, C_TFPlayer* player, CUserCmd* cmd, IMoveHelper* move_helper )
{
	//CALL_ORIGINAL( ECX, EDX, player, cmd, move_helper );

	if ( move_helper && !i::move_helper )
		i::move_helper = move_helper;

	const auto& local = entity_cache.get_local( );
	if ( local && !local->deadflag( ) && player && !player->deadflag( ) && local == player && global_info.shift_user_cmd )
	{
		if ( global_info.shift_user_cmd->command_number == cmd->command_number )
		{
			local->m_nTickBase( ) -= global_info.shifted;
			i::global_vars->curtime = TICKS_TO_TIME( local->m_nTickBase( ) );
		}

		if ( global_info.shift_user_cmd->command_number + 1 == cmd->command_number )
		{
			local->m_nTickBase( ) += global_info.shifted;
			i::global_vars->curtime = TICKS_TO_TIME( local->m_nTickBase( ) );
		}
		
		CALL_ORIGINAL( ECX, EDX, player, cmd, move_helper );
	}
	else
	{
		CALL_ORIGINAL( ECX, EDX, player, cmd, move_helper );
	}
}*/

MAKE_HOOK( DoPostScreenSpaceEffects, memory->get_vfunc( i::client_mode, 39 ), bool, __fastcall,
		   void* ECX, void* EDX, CViewSetup* setup )
{
	if ( vars.models.global.models_order )
	{
		outlines.Render( );
		models.Render( );
	}
	else
	{
		models.Render( );
		outlines.Render( );
	}
	misc.draw_prediction( );
	misc.draw_projectile( );

	return CALL_ORIGINAL( ECX, EDX, setup );
}

MAKE_HOOK( DrawModelExecute, memory->get_vfunc( i::model_render, 19 ), void, __fastcall,
		   void* ECX, void* EDX, const DrawModelState_t& state, ModelRenderInfo_t& info, const matrix3x4_t* bone_to_world )
{
	C_BaseEntity* entity = GET_ENT_I( info.entity_index )->As<C_BaseEntity>( );
	const auto& local = entity_cache.get_local( );

	if ( vars.backtrack.backtrack_active && vars.backtrack.chams && aimbot.weapon_type == EWeaponType::HITSCAN )
	{
		for ( const auto& target : backtrack.targets )
		{
			const auto& player = target.player;
			if ( !player || player->deadflag( ) || player->IsDormant( ) || player->InCond( TF_COND_HALLOWEEN_GHOST_MODE ) )
			{
				continue;
			}

			if ( local && entity && entity == player && backtrack.is_backtracking && !outlines.m_bRendering && !models.m_bRendering )
			{
				const auto data = backtrack.get_last( player );
				const auto& pRenderContext = i::material_system->GetRenderContext( );

				if ( data.lag_ticks >= 12 )
				{
					continue;
				}

				if ( !pRenderContext )
				{
					return;
				}

				i::model_render->ForcedMaterialOverride( models.m_pMatShaded );
				i::render_view->SetColorModulation( 1.0f, 1.0f, 1.0f );

				if ( vars.models.players.players_ignorez )
				{
					pRenderContext->DepthRange( 0.0f, 0.2f );
				}
				i::render_view->SetBlend( 1.0f );
				CALL_ORIGINAL( ECX, EDX, state, info, data.bones );

				i::model_render->ForcedMaterialOverride( nullptr );
				i::render_view->SetColorModulation( 1.0f, 1.0f, 1.0f );

				if ( vars.models.players.players_ignorez )
				{
					pRenderContext->DepthRange( 0.0f, 1.0f );
				}
				i::render_view->SetBlend( 1.0f );
			}
		}
	}

	if ( local && entity && entity == local && entity != entity_cache.get_weapon( ) && !outlines.m_bRendering && !models.m_bRendering && vars.antiaim.antiaim_active && vars.antiaim.fake_chams
		 && ( global_info.real_viewangles.y - global_info.fake_viewangles.y > 1.f || global_info.real_viewangles.y - global_info.fake_viewangles.y < -1.f )
		 && antiaim.toggle && vars.models.global.models_global )
	{
		//rotate the matrix
		vector BonePos;
		vector OutPos;
		matrix3x4_t fake_matrix[ MAXSTUDIOBONES ];
		matrix3x4_t bone_matrix[ MAXSTUDIOBONES ];
		if ( local->SetupBones( bone_matrix, MAXSTUDIOBONES, BONE_USED_BY_ANYTHING, i::global_vars->curtime ) )
		{
			for ( int i = 0; i < MAXSTUDIOBONES; i++ )
			{
				math.angle_matrix( vector( 0, global_info.fake_viewangles.y - global_info.real_viewangles.y, 0 ), fake_matrix[ i ] );
				math.MatrixMultiply( fake_matrix[ i ], bone_matrix[ i ], fake_matrix[ i ] );
				BonePos = vector( bone_matrix[ i ][ 0 ][ 3 ], bone_matrix[ i ][ 1 ][ 3 ], bone_matrix[ i ][ 2 ][ 3 ] ) - info.origin;
				math.VectorRotate( BonePos, vector( 0, global_info.fake_viewangles.y - global_info.real_viewangles.y, 0 ), OutPos );
				OutPos += info.origin;
				fake_matrix[ i ][ 0 ][ 3 ] = OutPos.x;
				fake_matrix[ i ][ 1 ][ 3 ] = OutPos.y;
				fake_matrix[ i ][ 2 ][ 3 ] = OutPos.z;
			}
		}
		const auto& pRenderContext = i::material_system->GetRenderContext( );

		if ( !pRenderContext )
		{
			return;
		}

		if ( vars.antiaim.flat_material )
		{
			i::model_render->ForcedMaterialOverride( models.m_pMatFlat );
		}

		//pRenderContext->DepthRange( 0.0f, 0.2f );

		i::render_view->SetColorModulation
		(
			utils.clr2flt( vars.colors.antiaim.local_fake.r ),
			utils.clr2flt( vars.colors.antiaim.local_fake.g ),
			utils.clr2flt( vars.colors.antiaim.local_fake.b )
		);

		i::render_view->SetBlend( utils.clr2flt( vars.colors.antiaim.local_fake.a ) );

		CALL_ORIGINAL( ECX, EDX, state, info, fake_matrix );

		pRenderContext->DepthRange( 0.0f, 1.0f );
		i::model_render->ForcedMaterialOverride( nullptr );
		i::render_view->SetColorModulation( 1.0f, 1.0f, 1.0f );
		i::render_view->SetBlend( 1.0f );
	}

	if ( ( models.HasDrawn( entity ) || outlines.HasDrawn( entity ) ) && !outlines.m_bDrawingGlow ) { return; }
	CALL_ORIGINAL( ECX, EDX, state, info, bone_to_world );
}

MAKE_HOOK( ForcedMaterialOverride, memory->get_vfunc( i::model_render, 1 ), void, __fastcall,
		   void* ECX, void* EDX, IMaterial* mat, OverrideType_t type )
{
	if ( ( outlines.m_bRendering && !outlines.IsGlowMaterial( mat ) ) || ( models.m_bRendering && !models.IsChamsMaterial( mat ) ) ) { return; }

	CALL_ORIGINAL( ECX, EDX, mat, type );
}

MAKE_HOOK( CalcIsAttackCritical, S::C_TFWeaponBase_CalcIsAttackCritical.address, void, __fastcall,
		   void* ECX, void* EDX )
{
	const auto& local = entity_cache.get_local( );
	const auto& weapon = entity_cache.get_weapon( );

	if ( !local || !weapon )
	{
		return CALL_ORIGINAL( ECX, EDX );
	}

	if ( !crithack.CalcIsAttackCriticalHandler( local, weapon ) )
	{
		return;
	}

	const auto nPreviousWeaponMode = weapon->m_iWeaponMode( );
	weapon->SetWeaponMode( 0 );

	CALL_ORIGINAL( ECX, EDX );

	weapon->SetWeaponMode( nPreviousWeaponMode );
}

/*MAKE_HOOK( RandomSeed, GetProcAddress( GetModuleHandleW( L"vstdlib.dll" ), "RandomSeed" ), void, __cdecl, int seed )
{
	if ( crithack.racist )
	{
		return;
	}
}*/

MAKE_HOOK( AddToCritBucket, S::AddToCritBucket.address, void, __fastcall,
		   void* ECX, void* EDX, float amount )
{
	if ( crithack.racist )
	{
		return;
	}

	CALL_ORIGINAL( ECX, EDX, amount );
}

MAKE_HOOK( WithdrawFromCritBucket, S::IsAllowedToWithdrawFromCritBucket.address, bool, __fastcall,
		   void* ECX, void* EDX, float damage )
{
	const auto& weapon = entity_cache.get_weapon( );
	if ( !weapon )
	{
		return CALL_ORIGINAL( ECX, EDX, damage );
	}

	i::cvar->ConsoleColorPrintf( { 0, 255, 255, 255 }, "Called WithdrawFromCritBucket.\n" );
	if ( crithack.damage_correct[ weapon->GetSlot( ) ] == 0 )
	{
		crithack.damage_correct[ weapon->GetSlot( ) ] = static_cast< int >( damage );
	}

	return CALL_ORIGINAL( ECX, EDX, damage );
}

MAKE_HOOK( GetUserCmd, memory->get_vfunc( i::input, 8 ), CUserCmd*, __fastcall,
		   void* ecx, void* edx, int sequence_number )
{
	return &i::input->GetCmds( )[ sequence_number % 90 ];
}

MAKE_HOOK( CanFireRandomCriticalShot, S::CanFireRandomCriticalShot.address, bool, __fastcall,
		   void* ECX, void* EDX, float crit_chance )
{
	const auto& weapon = entity_cache.get_weapon( );
	if ( !weapon )
	{
		return CALL_ORIGINAL( ECX, EDX, crit_chance );
	}

	if ( weapon )
	{
		crithack.can_fire_critical_shot_handler( crit_chance );
	}

	return CALL_ORIGINAL( ECX, EDX, crit_chance );
}

MAKE_NETVAR_HOOK( CTFPlayerResource, m_iDamage )
{
	if ( !data || !ECX || !out )
	{
		return;
	}

	const auto new_damage = data->m_Value.m_Int;
	const auto old_damage = *static_cast< i32* >( out );

	const auto& local = entity_cache.get_local( );
	if ( local && out >= ECX )
	{
		auto index = reinterpret_cast< u32 >( out ) - reinterpret_cast< u32 >( ECX );

		if ( index > 0 )
		{
			index /= 4;
		}

		if ( local->entindex( ) == index && crithack.last_hit_registered_melee && new_damage > old_damage )
		{
			crithack.total_round_melee_damage += new_damage - old_damage;
		}
	}

	*static_cast< i32* >( out ) = new_damage;
}

MAKE_HOOK( CL_LoadWhitelist, S::CL_LoadWhiteList.address, void*, __cdecl,
		   void* table, const char* name )
{
	if ( vars.visuals.bypass_svpure )
	{
		return nullptr;
	}

	return CALL_ORIGINAL( table, name );
}

/*static ProxyFnHook DamageResourceProxy{ };
void player_resource_damage_hook( const CRecvProxyData* data, void* ecx, void* out )
{
	if ( !data || !ecx || !out )
	{
		return;
	}

	const auto new_damage = data->m_Value.m_Int;
	const auto old_damage = *static_cast< i32* >( out );

	const auto& local = entity_cache.get_local( );
	if ( local && out >= ecx )
	{
		auto index = reinterpret_cast< u32 >( out ) - reinterpret_cast< u32 >( ecx );

		if ( index > 0 )
		{
			index /= 4;
		}

		if ( local->entindex( ) == index && crithack.last_hit_registered_melee && new_damage > old_damage )
		{
			crithack.total_round_melee_damage += new_damage - old_damage;
		}
	}

	*static_cast< i32* >( out ) = new_damage;
}

static ProxyFnHook HealthNetvarProxy{};
void health_netvar_hook( const CRecvProxyData* data, void* ecx, void* out )
{
	if ( !data || !ecx || !out )
	{
		return;
	}

	const auto new_health = data->m_Value.m_Int;
	auto       player = static_cast< C_TFPlayer* >( ecx );

	if ( player != nullptr )
	{
		const auto unlag = player->get_unlag_data( );

		if ( unlag != nullptr )
		{
			const auto old_health = *static_cast< i32* >( out );

			unlag->old_health = old_health;
			unlag->new_health = new_health;
			unlag->health_delta = unlag->new_health - unlag->old_health;

			if ( unlag->new_health > unlag->synced_health )
				unlag->synced_health = unlag->new_health;
		}
	}

	*static_cast< i32* >( out ) = new_health;
}

void __hook_manager::hk_netvar_init( )
{
	hook_netvar( { "CTFPlayerResource", "m_iDamage" }, DamageResourceProxy, player_resource_damage_hook );
	//hook_netvar( { "CBasePlayer", "m_iHealth" }, HealthNetvarProxy, health_netvar_hook );
}*/

/*MAKE_NETVAR_HOOK( CBasePlayer, m_iHealth )
{
	if ( !data || !ECX || !out )
	{
		return;
	}

	const auto new_health = data->m_Value.m_Int;
	auto       player = static_cast< C_TFPlayer* >( ECX );

	if ( player != nullptr )
	{
		const auto unlag = player->get_unlag_data( );

		if ( unlag != nullptr )
		{
			const auto old_health = *static_cast< i32* >( out );

			unlag->old_health = old_health;
			unlag->new_health = new_health;
			unlag->health_delta = unlag->new_health - unlag->old_health;

			if ( unlag->new_health > unlag->synced_health )
				unlag->synced_health = unlag->new_health;
		}
	}

	*static_cast< i32* >( out ) = new_health;
}*/

MAKE_HOOK( UseItemConfirm, S::UseItemConfirm.address, void, __cdecl, 
		   bool bConfirmed, void* pContext )
{
	i::cvar->ConsoleColorPrintf( { 0, 255, 255, 255 }, "Called UseItemConfirm.\n" );
	return;
}

MAKE_HOOK( SetAbsVelocity, S::SetAbsVelocity.address, void, __fastcall,
		   void* ECX, void* EDX, const vector& abs_velocity )
{
	if ( reinterpret_cast< DWORD >( _ReturnAddress( ) ) == S::C_BasePlayer_PostDataUpdate_SetAbsVelocityCall.address )
	{
		if ( auto pBasePlayer = reinterpret_cast< C_TFPlayer* >( EDX ) )
		{
			if ( global_info.vel_fix_records.find( pBasePlayer ) != global_info.vel_fix_records.end( ) )
			{
				const auto& Record = global_info.vel_fix_records[ pBasePlayer ];

				float flSimTimeDelta = pBasePlayer->m_flSimulationTime( ) - Record.simtime;

				if ( flSimTimeDelta > 0.0f )
				{
					vector vOldOrigin = Record.origin;

					int nCurFlags = pBasePlayer->m_fFlags( ).GetFlags( );
					int nOldFlags = Record.flags;

					if ( !( nCurFlags & FL_ONGROUND ) && !( nOldFlags & FL_ONGROUND ) )
					{
						bool bCorrected = false;

						if ( ( nCurFlags & FL_DUCKING ) && !( nOldFlags & FL_DUCKING ) ) {
							vOldOrigin.z += 20.0f;
							bCorrected = true;
						}

						if ( !( nCurFlags & FL_DUCKING ) && ( nOldFlags & FL_DUCKING ) ) {
							vOldOrigin.z -= 20.0f;
							bCorrected = true;
						}

						if ( bCorrected )
						{
							vector vNewVelocity = abs_velocity;
							vNewVelocity.z = ( pBasePlayer->m_vecOrigin( ).z - vOldOrigin.z ) / flSimTimeDelta;
							CALL_ORIGINAL( ECX, EDX, vNewVelocity );
							return;
						}
					}
				}
			}
		}
	}
	
	CALL_ORIGINAL( ECX, EDX, abs_velocity );
}

/*MAKE_HOOK( SetImage, S::ImagePanel_SetImage.address, void, __fastcall,
		   void* ECX, void* EDX, const char* image )
{
	if ( strcmp( image, "../HUD/defense_buff_bullet_red" ) == 0 )
	{
		i::cvar->ConsoleColorPrintf( { 0, 255, 255, 255 }, "Returning.\n" );
		return;
	}
	else
	{
		i::cvar->ConsoleColorPrintf( { 0, 255, 255, 255 }, "Calling original.\n" );
		CALL_ORIGINAL( ECX, EDX, image );
	}
}

MAKE_HOOK( CStaticPropMgr_ComputePropOpacity, S::CStaticPropMgr_ComputePropOpacity.address, void, __fastcall,
		   void* ECX, void* EDX, CStaticProp_* prop )
{
	global_info.drawing_prop = false;
	if ( const auto& model = i::model_info->GetStudiomodel( prop->m_pModel ) )
	{
		/*i::cvar->ConsoleColorPrintf( { 0, 255, 255, 255 }, "prop name: " );
		for ( int i = 0; i < sizeof( model->name ); i++ )
		{
			i::cvar->ConsoleColorPrintf( { 0, 255, 255, 255 }, "%c", model->name[ i ] );
		}
		i::cvar->ConsoleColorPrintf( { 0, 255, 255, 255 }, "\n" );

		global_info.prop_name = model->name;
	}
	CALL_ORIGINAL( ECX, EDX, prop );
}

MAKE_HOOK( StaticPropMgr_DrawStaticProps, S::StaticPropMgr_DrawStaticProps.address, void, __fastcall,
		   void* ECX, void* EDX, IClientRenderable** props, int count, bool shadow_depth, bool collide_wireframe )
{
	IClientRenderable* prop = *props;
	if ( const auto& model = i::model_info->GetStudiomodel( prop->GetModel( ) ) )
	{
		std::string name = model->name;
		if ( name.find( "bridge_cover001" ) != std::string::npos || name.find( "bridge_cover_sides001" ) != std::string::npos || name.find( "bridgesupports001" ) != std::string::npos )
		{
			global_info.drawing_prop = true;
			i::cvar->ConsoleColorPrintf( { 0, 255, 255, 255 }, "%s\n", name.c_str( ) );
		}
	}

	global_info.drawing_props = true;
	CALL_ORIGINAL( ECX, EDX, props, count, shadow_depth, collide_wireframe );
	global_info.drawing_props = false;
	global_info.drawing_prop = false;
}

MAKE_HOOK( SetAlphaModulation, memory->get_vfunc( i::studio_render, 28 ), void, __fastcall,
		   void* ECX, void* EDX, float alpha )
{
	if ( global_info.drawing_prop )
	{
		CALL_ORIGINAL( ECX, EDX, global_info.drawing_props ? 0.5f : alpha );
	}
	else
	{
		CALL_ORIGINAL( ECX, EDX, alpha );
	}
}*/