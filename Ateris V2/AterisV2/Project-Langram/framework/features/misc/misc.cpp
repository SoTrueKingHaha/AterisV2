#include "misc.h"
#include "../antiaim/antiaim.h"

void __misc::run( CUserCmd* cmd )
{
	if ( const auto& local = entity_cache.get_local( ) )
	{
		accurate_movement( local, cmd );
		if ( !vars.global.legit_only )
		{
			stop_movement( local, cmd );
			auto_strafe( local, cmd );
		}
		if ( const auto& weapon = entity_cache.get_weapon( ) )
		{
			if ( !vars.global.legit_only )
			{
				auto_jump( local, weapon, cmd );
				auto_scout_jump( local, weapon, cmd );
			}
		}
		viewmodel( );
	}
}

void __misc::run_late( CUserCmd* cmd )
{
	if ( const auto& local = entity_cache.get_local( ) )
	{
		fast_stop( local, cmd );
		leg_jitter( cmd, local );
	}
}

void __misc::auto_jump( C_TFPlayer* local, C_TFWeaponBase* weapon, CUserCmd* cmd )
{
	if ( !vars.misc.auto_jump
		 || local->deadflag( )
		 || local->IsSwimming( )
		 || local->IsAGhost( )
		 || local->IsInBumperKart( ) )
	{
		return;
	}

	if ( local->movetype( ) == MOVETYPE_NOCLIP
		 || local->movetype( ) == MOVETYPE_LADDER
		 || local->movetype( ) == MOVETYPE_OBSERVER )
	{
		return;
	}

	static bool s_bState = false;

	if ( fabsf( local->m_vecVelocity( ).x ) < ( local->m_flMaxspeed( ) ) && fabsf( local->m_vecVelocity( ).y ) < ( local->m_flMaxspeed( ) ) && fabsf( local->m_vecVelocity( ).x ) + fabsf( local->m_vecVelocity( ).y ) < ( local->m_flMaxspeed( ) * 0.8f ) && local->IsOnGround( ) && vars.misc.break_jump )
	{
		cmd->buttons &= ~IN_JUMP;
		if ( ( GetAsyncKeyState( VK_SPACE ) && utils.is_window_in_focus( ) && !i::engine_vgui->IsGameUIVisible( ) && !i::surface->IsCursorVisible( ) ) && ( cmd->buttons != IN_FORWARD && cmd->buttons != IN_BACK && cmd->buttons != IN_LEFT && cmd->buttons != IN_RIGHT ) )
		{
			cmd->forwardmove = 450.f;
		}
	}
	else
	{
		if ( cmd->buttons & IN_JUMP )
		{
			if ( !s_bState && !local->OnSolid( ) )
			{
				cmd->buttons &= ~IN_JUMP;
			}
			else if ( s_bState )
			{
				s_bState = false;
			}

			if ( vars.misc.tomislav_rev && weapon && weapon->m_iItemDefinitionIndex( ) == Heavy_m_Tomislav )
			{
				if ( local->IsOnGround( ) )
					cmd->buttons |= IN_ATTACK2;
				else
					cmd->buttons &= ~IN_ATTACK2;
			}
		}
		else if ( !s_bState )
		{
			s_bState = true;
		}

		if ( vars.misc.break_jump )
		{
			static Timer cmdTimer{};
			if ( !local->OnSolid( ) ) {
				cmd->buttons |= IN_DUCK;

			}
			if ( !local->OnSolid( ) && cmdTimer.Run( 20 ) )
			{
				cmd->buttons &= ~IN_DUCK;
			}
		}
	}
}

void __misc::auto_strafe( C_TFPlayer* local, CUserCmd* cmd )
{
	if ( !vars.misc.auto_strafe )
	{
		return;
	}

	if ( local->deadflag( )
		 || local->IsSwimming( )
		 || local->IsAGhost( )
		 || local->IsInBumperKart( )
		 || local->OnSolid( ) )
	{
		return;
	}

	if ( local->movetype( ) == MOVETYPE_NOCLIP
		 || local->movetype( ) == MOVETYPE_LADDER
		 || local->movetype( ) == MOVETYPE_OBSERVER )
	{
		return;
	}

	static auto cl_sidespeed = 450.f;

	static bool wasJumping = false;
	const bool isJumping = cmd->buttons & IN_JUMP;

	switch ( vars.misc.auto_strafe )
	{
		case 1:
		{
			if ( cmd->mousedx && ( !isJumping || wasJumping ) )
			{
				cmd->sidemove = cmd->mousedx > 1 ? 450.f : -450.f;
			}
			wasJumping = isJumping;
			break;
		}

		case 2:
		{
			const float speed = local->EstimateAbsVelocity( ).length_2d( );

			if ( speed < 2.0f )
			{
				return;
			}

			const auto vel = local->EstimateAbsVelocity( );

			constexpr auto perfectDelta = [ ] ( float speed ) noexcept {
				if ( const auto& local = entity_cache.get_local( ) )
				{
					static auto speedVar = local->m_flMaxspeed( );
					static auto airVar = 10.f;
					static auto wishSpeed = 30.0f;

					const auto term = wishSpeed / 10.f / speedVar * 100.f / speed;

					if ( term < 1.0f && term > -1.0f )
					{
						return acosf( term );
					}
				}
				return 0.0f;
			};

			const float pDelta = perfectDelta( speed ); //1.56892
			if ( ( !isJumping || wasJumping ) && pDelta )
			{
				const float yaw = DEG2RAD( cmd->viewangles.y );
				const float velDir = atan2f( vel.y, vel.x ) - yaw;
				const float wishAng = atan2f( -cmd->sidemove, cmd->forwardmove );
				const float delta = math.angle_diff_radians( velDir, wishAng );

				const float moveDir = delta < 0.0f ? velDir + pDelta : velDir - pDelta;

				cmd->forwardmove = cosf( moveDir ) * 450.f;
				cmd->sidemove = -sinf( moveDir ) * 450.f;
			}
			wasJumping = isJumping;
			break;
		}

		default: break;
	}
}

void __misc::auto_scout_jump( C_TFPlayer* local, C_TFWeaponBase* weapon, CUserCmd* cmd )
{
	if ( !vars.misc.auto_scout_jump ) { return; }

	if ( local->deadflag( )
		 || local->IsSwimming( )
		 || local->IsAGhost( )
		 || local->IsInBumperKart( ) )
	{
		return;
	}

	if ( local->movetype( ) == MOVETYPE_NOCLIP
		 || local->movetype( ) == MOVETYPE_LADDER
		 || local->movetype( ) == MOVETYPE_OBSERVER )
	{
		return;
	}

	static bool shot = false;
	if ( utils.is_window_in_focus( ) && ( GetAsyncKeyState( vars.misc.scout_jump_key ) & 1 ) && !i::surface->IsCursorVisible( ) )
	{
		if ( local && !local->deadflag( ) && !local->IsDormant( ) && local->m_iClass( ) == CLASS_SCOUT && local->IsOnGround( ) )
		{
			if ( weapon->m_iItemDefinitionIndex( ) == Scout_m_ForceANature || weapon->m_iItemDefinitionIndex( ) == Scout_m_FestiveForceANature )
			{
				vector vAngle = { 37.f, cmd->viewangles.y, 0.0f };
				global_info.scout_jumping = true;
				if ( !shot )
				{
					cmd->buttons |= IN_JUMP;
					cmd->buttons |= IN_ATTACK;
					shot = true;
				}
				shot = false;

				utils.fix_movement( cmd, vAngle );
				cmd->viewangles = vAngle;
			}
		}
	}
}

void __misc::stop_movement( C_TFPlayer* local, CUserCmd* cmd )
{
	if ( !global_info.should_stop ) { return; }
	utils.stop_movement( local, cmd, !global_info.shifting );
	if ( global_info.should_stop ) { return; }
}

void __misc::accurate_movement( C_TFPlayer* local, CUserCmd* cmd )
{
	if ( !vars.misc.accurate_movement )
	{
		return;
	}

	if ( local->deadflag( )
		 || local->IsSwimming( )
		 || local->IsAGhost( )
		 || local->IsInBumperKart( )
		 || !local->OnSolid( ) )
	{
		return;
	}

	if ( local->movetype( ) == MOVETYPE_NOCLIP
		 || local->movetype( ) == MOVETYPE_LADDER
		 || local->movetype( ) == MOVETYPE_OBSERVER )
	{
		return;
	}

	if ( cmd->buttons & ( IN_JUMP | IN_MOVELEFT | IN_MOVERIGHT | IN_FORWARD | IN_BACK ) )
	{
		return;
	}

	const float speed = local->m_vecVelocity( ).length_2d( );
	const float speedLimit = 10.f;

	if ( speed > speedLimit )
	{
		vector direction = local->m_vecVelocity( ).to_angle( );
		direction.y = cmd->viewangles.y - direction.y;
		const vector negatedDirection = direction.from_angle( ) * -speed;
		cmd->forwardmove = negatedDirection.x;
		cmd->sidemove = negatedDirection.y;
	}
	else
	{
		cmd->forwardmove = 0.0f;
		cmd->sidemove = 0.0f;
	}
}

void __misc::fast_stop( C_TFPlayer* local, CUserCmd* cmd )
{
	if ( !vars.misc.cl_move.antiwarp || !vars.misc.cl_move.doubletap )
	{
		return;
	}

	static vector start_origin = { };
	static vector start_velocity = { };

	if ( global_info.shifting && local && !local->deadflag( ) && local->EstimateAbsVelocity( ).length_2d( ) > 5.f )
	{
		cmd->forwardmove = 0.f; cmd->sidemove = 0.f;

		if ( start_origin.is_zero( ) )
		{
			start_origin = local->GetAbsOrigin( );
		}

		if ( start_velocity.is_zero( ) )
		{
			start_velocity = local->EstimateAbsVelocity( );
		}
		vector predicted = start_origin + ( start_velocity * TICKS_TO_TIME( global_info.shifted ) );
		vector predicted_max = start_origin + ( start_velocity * TICKS_TO_TIME( 22 - global_info.ticks ) );

		float scale = math.remap_val_clamped( predicted.dist_to( start_origin ), 0.0f, predicted_max.dist_to( start_origin ) * 1.27f, 1.0f, 0.0f );
		float scale_ = math.remap_val_clamped( start_velocity.length_2d( ), 0.0f, 520.0f, 0.0f, 1.0f );

		if ( local->IsClass( CLASS_SCOUT ) )
		{
			utils.walk_to( cmd, local, predicted, start_origin, TICKS_TO_TIME( 22 - global_info.ticks ) );
		}
		else
		{
			utils.walk_to( cmd, local, predicted_max, start_origin, scale * scale_ );
		}
	}
	else
	{
		start_origin = { 0, 0, 0 };
		start_velocity = { 0, 0, 0 };
	}
}

void __misc::dormant_esp( StartSoundParams_t& params )
{
	if ( vars.esp.global.esp_dormant ) {
		// Credits go to reestart entirely
		vector origin;
		if ( params.soundsource > 0 ) //Make sure we even have a valid entity to begin with
		{
			math.vector_copy( params.origin, origin );
			int index = params.soundsource;
			C_TFPlayer* entity = GET_ENT_I( index )->As<C_TFPlayer>( );
			C_TFPlayerResource* resource = entity_cache.get_resource( );
			if ( entity && index != i::engine_client->GetLocalPlayer( ) && resource->is_alive( entity->entindex( ) ) && entity->GetClassID( ) == ETFClassID::CTFPlayer )
			{
				if ( entity->IsDormant( ) )
				{
					if ( !origin.is_zero( ) )
					{
						entity->SetAbsOrigin( origin );
					}
					entity->m_iHealth( ) = resource->get_health( entity->entindex( ) );
					global_info.dormant_players[ entity->entindex( ) ].last_update = i::engine_client->Time( );
				}
			}
		}
	}
}

bool __misc::remove_scope( unsigned int panel )
{
	const auto& local = entity_cache.get_local( );
	int nHudZoom = 0;

	if ( !nHudZoom && entity_cache.is_scope( FNV1A::Hash( i::vgui_panel->GetName( panel ) ) ) )
	{
		nHudZoom = panel;
	}

	if ( local && !local->deadflag( ) && vars.visuals.visuals_remove_scope && local->IsClass( CLASS_SNIPER ) && panel == nHudZoom )
	{
		return true;
	}

	return false;
}

void __misc::third_person( C_TFPlayer* local, CViewSetup* pView )
{
	if ( local && !local->deadflag( ) && !local->IsDormant( ) && pView )
	{
		if ( utils.is_window_in_focus( ) && ( GetAsyncKeyState( vars.visuals.visuals_thirdperson_key ) & 1 ) )
		{
			InThirdperson = !InThirdperson;
		}

		if ( local->InCond( TF_COND_TAUNTING ) )
		{
			InThirdperson = true;
		}

		if ( InThirdperson )
		{
			i::input->CAM_ToThirdPerson( );
			local->m_nForceTauntCam( ) = InThirdperson;
		}
		else
		{
			i::input->CAM_ToFirstPerson( );
			return;
		}

		if ( InThirdperson )
		{
			if ( vars.antiaim.antiaim_active )
			{
				i::prediction->SetLocalViewAngles( global_info.real_viewangles );
				if ( const auto& pAnimState = local->m_PlayerAnimState( ) )
				{
					pAnimState->m_flCurrentFeetYaw = global_info.real_viewangles.y;
				}
			}
			else
			{
				i::prediction->SetLocalViewAngles( global_info.real_angles );
				if ( const auto& pAnimState = local->m_PlayerAnimState( ) )
				{
					pAnimState->m_flCurrentFeetYaw = global_info.real_angles.y;
				}
			}
		}

		local->UpdateVisibility( );
		local->UpdateWearables( );

		vector forward{ }, right{ }, up{ };
		math.angle_vectors( pView->angles, &forward, &right, &up );

		int distance = vars.visuals.visuals_thirdperson_offset ? vars.visuals.visuals_thirdperson_offset_forwards : 150;
		vector offset = ( forward * distance ) - ( right * vars.visuals.visuals_thirdperson_offset_sideways ) - ( up * vars.visuals.visuals_thirdperson_offset_upwards );
		vector delta = pView->origin - offset;
		Ray_t ray;
		ray.Init( pView->origin, delta, { -10.f, -10.f, -10.f }, { 10.f, 10.f, 10.f } );
		CTraceFilterWorldAndPropsOnly Filter = { };
		trace_t Trace = {};
		i::engine_trace->TraceRay( ray, MASK_SOLID, &Filter, &Trace );

		if ( InThirdperson )
		{
			pView->origin -= offset * Trace.m_flFraction;
		}
	}
}

void __misc::fov( C_TFPlayer* local, CViewSetup* pView )
{
	if ( local && pView )
	{
		if ( local->m_iObserverMode( ) == OBS_MODE_FIRSTPERSON || ( local->InCond( TF_COND_ZOOMED ) && !vars.visuals.visuals_remove_scope ) )
			return;

		if ( vars.visuals.visuals_remove_scope && vars.global.legit_only )
			return;

		pView->fov = vars.visuals.visuals_fov;
		
		if ( !local->deadflag( ) ) {
			local->m_iFOV( ) = vars.visuals.visuals_fov;
			local->m_iDefaultFOV( ) = vars.visuals.visuals_fov;
		}
	}
}

void RenderLine( const vector& v1, const vector& v2, color col )
{
	static auto RenderLine = reinterpret_cast< void( __cdecl* )( const vector&, const vector&, color, bool ) >( S::RenderLine.address );
	RenderLine( v1, v2, col, false );
}

void __misc::draw_prediction( )
{
	if ( !vars.aimbot.projectile.projectile_predicted_path || global_info.prediction_lines.empty( ) )
	{
		return;
	}

	static float curtime = i::global_vars->curtime;
	if ( global_info.attacking )
	{
		curtime = i::global_vars->curtime;
	}

	for ( size_t i = 1; i < global_info.prediction_lines.size( ); i++ )
	{
		RenderLine( global_info.prediction_lines.at( i - 1 ),
					global_info.prediction_lines.at( i ),
					{ 255, 255, 255, 255 } );
	}

	if ( curtime + vars.aimbot.projectile.projectile_path_duration < i::global_vars->curtime )
	{
		global_info.prediction_lines.clear( );
	}
}

void __misc::draw_projectile( )
{
	if ( global_info.projectile_lines.empty( ) )
	{
		return;
	}

	static float curtime = i::global_vars->curtime;
	if ( global_info.attacking )
	{
		curtime = i::global_vars->curtime;
	}

	for ( size_t i = 1; i < global_info.projectile_lines.size( ); i++ )
	{
		if ( !utils.vis_pos( global_info.projectile_lines.at( i - 1 ), global_info.projectile_lines.at( i ) ) )
		{
			break;
		}
		RenderLine( global_info.projectile_lines.at( i - 1 ),
					global_info.projectile_lines.at( i ),
					{ 255, 255, 255, 255 } );
	}

	if ( curtime + 0.5f < i::global_vars->curtime )
	{
		global_info.projectile_lines.clear( );
	}
}

void __misc::viewmodel( )
{
	static bool flip = false;
	if ( convars.cl_flipviewmodels )
	{
		if ( vars.visuals.viewmodel_flip )
		{
			if ( !convars.cl_flipviewmodels->GetInt( ) )
			{
				convars.cl_flipviewmodels->SetValue( 1 );
				flip = true;
			}
		}
		else
		{
			if ( flip || convars.cl_flipviewmodels->GetInt( ) )
			{
				convars.cl_flipviewmodels->SetValue( 0 );
				flip = false;
			}
		}
	}

	static bool min = false;
	if ( convars.tf_use_min_viewmodels )
	{
		if ( vars.visuals.viewmodel_min )
		{
			if ( !convars.tf_use_min_viewmodels->GetInt( ) )
			{
				convars.tf_use_min_viewmodels->SetValue( 1 );
				min = true;
			}
		}
		else
		{
			if ( min || convars.tf_use_min_viewmodels->GetInt( ) )
			{
				convars.tf_use_min_viewmodels->SetValue( 0 );
				min = false;
			}
		}
	}

	static bool bSet = false;
	if ( vars.visuals.viewmodel_fov && convars.viewmodel_fov )
	{
		if ( !bSet )
		{
			vars.visuals.viewmodel_fov = convars.viewmodel_fov->GetInt( );
			prev_fov = convars.viewmodel_fov->GetInt( );
		}

		convars.viewmodel_fov->SetValue( vars.visuals.viewmodel_fov );
		bSet = true;
	}

	if ( convars.cl_wpn_sway_interp )
	{
		if ( vars.visuals.viewmodel_sway )
		{
			convars.cl_wpn_sway_interp->SetValue( 0.05f );
		}
		else
		{
			if ( convars.cl_wpn_sway_interp->GetFloat( ) )
			{
				convars.cl_wpn_sway_interp->SetValue( 0.0f );
			}
		}
	}

	static bool bToggle = false;
	if ( vars.visuals.aspect_ratio && convars.r_aspectratio )
	{
		if ( !bToggle )
		{
			vars.visuals.aspect_ratio = convars.r_aspectratio->GetFloat( );
			prev_aspect_ratio = convars.r_aspectratio->GetFloat( );
		}

		if ( vars.visuals.aspect_ratio < 0.02f )
		{
			convars.r_aspectratio->SetValue( prev_aspect_ratio );
		}
		else
		{
			convars.r_aspectratio->SetValue( vars.visuals.aspect_ratio );
		}
		bToggle = true;
	}
}

void __misc::leg_jitter( CUserCmd* pCmd, C_TFPlayer* pLocal )
{
	static bool pos = true;
	const float scale = pLocal->IsDucking( ) ? 14.f : 2.f;
	if ( global_info.attacking || global_info.shifting || global_info.antiaim.second ) { return; }
	if ( pCmd->forwardmove == 0.f && pCmd->sidemove == 0.f && pLocal->m_vecVelocity( ).length_2d( ) < 10.f && vars.antiaim.antiaim_active && antiaim.toggle/* && I::GlobalVars->tickcount % 2*/ )
	{
		pos ? pCmd->forwardmove = scale : pCmd->forwardmove = -scale;
		pos ? pCmd->sidemove = scale : pCmd->sidemove = -scale;
		pos = !pos;
	}
}