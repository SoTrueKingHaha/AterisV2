#include "backtrack.h"
#include "../aimbot/aimbot.h"

void __backtrack::reset( )
{
	data.clear( );
}

bool __backtrack::is_valid( float simtime )
{
	auto nci = i::engine_client->GetNetChannelInfo( );

	if ( !nci )
		return false;

	auto delta = std::clamp( nci->GetLatency( 0 ) + utils.lerp_time( ), 0.f, convars.sv_maxunlag->GetFloat( ) ) - ( i::global_vars->curtime - simtime );

	return std::fabsf( delta ) <= 0.2f;
}

void __backtrack::fill( )
{
	if ( !vars.backtrack.backtrack_active )
	{
		return;
	}

	for ( const auto& player : entity_cache.players[ ENEMY ] )
	{
		if ( !player || player->deadflag( ) || player->IsDormant( ) || player->InCond( TF_COND_HALLOWEEN_GHOST_MODE ) )
		{
			data[ player ].clear( );
			continue;
		}

		backtrack_data temp_data{ };

		temp_data.simtime = player->m_flSimulationTime( );
		const float diff = player->m_flSimulationTime( ) - player->m_flOldSimulationTime( );
		int ticks = std::clamp( TIME_TO_TICKS( diff ), 0, 24 );

		if ( ticks >= 12 )
		{
			data[ player ].clear( );
			continue;
		}

		temp_data.lag_ticks = ticks;
		temp_data.pos = player->m_vecOrigin( );
		temp_data.headpos = player->GetHitboxPosition( HITBOX_HEAD );
		temp_data.bodypos = player->GetHitboxPosition( HITBOX_PELVIS );

		/*temp_data.angles = player->m_angEyeAngles( );
		temp_data.velocity = player->m_vecVelocity( );
		temp_data.mins = player->m_vecMins( );
		temp_data.maxs = player->m_vecMaxs( );*/

		player->SetupBones( temp_data.bones, MAXSTUDIOBONES, BONE_USED_BY_ANYTHING, i::global_vars->curtime );
		data[ player ].push_front( temp_data );

		while ( data[ player ].size( ) > 13 )
		{
			data[ player ].pop_back( );
		}
	}
}

backtrack_data __backtrack::get_last( C_TFPlayer* player )
{
	if ( !player || player->deadflag( ) )
	{
		return { };
	}

	backtrack_data last;
	for ( auto& record : data[ player ] )
	{
		last = record;
	}

	return last;
}

bool __backtrack::push_target( C_TFPlayer* local, C_TFWeaponBase* weapon, backtrack_target& out )
{
	targets.clear( );

	for ( const auto& player : entity_cache.players[ ENEMY ] )
	{
		if ( !player || player->deadflag( ) || player->IsDormant( ) || player->InCond( TF_COND_HALLOWEEN_GHOST_MODE ) )
			continue;

		vector angle_to = math.calc_angle( local->GetShootPosition( ), player->GetHitboxPosition( HITBOX_PELVIS ) );
		float fov_to = math.calc_fov( i::engine_client->GetViewAngles( ), angle_to );

		targets.push_back( { player, fov_to } );
	}

	std::sort( targets.begin( ), targets.end( ), [ & ] ( const backtrack_target& a, const backtrack_target& b ) -> bool
	{
		return ( a.fov_to < b.fov_to );
	} );

	for ( auto& target : targets )
	{
		out = target;
		return true;
	}
	return false;
}

bool __backtrack::push_record( C_TFPlayer* local, C_TFWeaponBase* weapon, backtrack_target target, backtrack_data& out )
{
	if ( !target.player || target.player->deadflag( ) )
	{
		return false;
	}

	backtrack_data last;
	for ( auto& record : data[ target.player ] )
	{
		last = record;
	}

	out = last;
	return true;
}

void __backtrack::run( C_TFPlayer* local, C_TFWeaponBase* weapon, CUserCmd* cmd )
{
	if ( !local || !weapon || !vars.backtrack.backtrack_active )
	{
		return;
	}

	is_backtracking = false;

	if ( !push_target( local, weapon, best_target ) )
	{
		//i::cvar->ConsoleColorPrintf( { 0, 255, 255, 255 }, "no main target\n" );
		return;
	}

	if ( vars.backtrack.debug )
	{
		i::debug_overlay->AddTextOverlay( best_target.player->GetHitboxPosition( HITBOX_PELVIS ), i::global_vars->absoluteframetime * 4, "main target, %i", data[ best_target.player ].size( ) );
	}

	if ( !push_record( local, weapon, best_target, best_data ) )
	{
		i::cvar->ConsoleColorPrintf( { 0, 255, 255, 255 }, "|Backtrack| No main record found.\n" );
		return;
	}

	is_backtracking = true;
	int target_ticks = TIME_TO_TICKS( best_data.simtime + utils.lerp_time( ) );

	if ( best_data.simtime > 0.f && best_target.player && aimbot.should_backtrack && cmd->tick_count - target_ticks < 13 )
	{
		i::cvar->ConsoleColorPrintf( { 0, 255, 255, 255 }, "|Backtrack| Set tickcount from: %i to: %i\n", cmd->tick_count, TIME_TO_TICKS( best_data.simtime + utils.lerp_time( ) ) );
		cmd->tick_count = target_ticks;
	}
}