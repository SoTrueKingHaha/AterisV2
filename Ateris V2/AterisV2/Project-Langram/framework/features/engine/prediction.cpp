#include "prediction.h"
#include "../../sdk/globals/globalinfo.h"

int __engine_prediction::get_tickbase( C_TFPlayer* local, CUserCmd* cmd )
{
	static int nTick = 0;
	static CUserCmd* pLastCmd = nullptr;

	if ( cmd )
	{
		if ( !pLastCmd || pLastCmd->hasbeenpredicted )
			nTick = local->m_nTickBase( );

		else nTick++;

		pLastCmd = cmd;
	}

	return nTick;
}

#undef max
void __engine_prediction::start( C_TFPlayer* local, CUserCmd* cmd )
{
	if ( !local || local->deadflag( ) || !i::move_helper || global_info.shifting )
	{
		return;
	}

	memset( &move_data, 0, sizeof( CMoveData ) );

	local->m_pCurrentCommand( ) = cmd;
	*i::random_seed = MD5_PseudoRandom( cmd->command_number ) & std::numeric_limits<int>::max( );
	C_BasePlayer::SetPredictionPlayer( local );

	old_flags = local->m_fFlags( );
	old_current_time = i::global_vars->curtime;
	old_frame_time = i::global_vars->frametime;
	old_tick_count = i::global_vars->tickcount;

	old_tickbase = local->m_nTickBase( );
	old_is_first_prediction = i::prediction->m_bFirstTimePredicted;
	old_in_prediction = i::prediction->m_bInPrediction;

	i::global_vars->curtime = TICKS_TO_TIME( get_tickbase( local, cmd ) );
	i::global_vars->frametime = ( i::prediction->m_bEnginePaused ? 0.0f : TICK_INTERVAL );
	i::global_vars->tickcount = get_tickbase( local, cmd );

	i::prediction->m_bFirstTimePredicted = false;
	i::prediction->m_bInPrediction = true;

	i::game_movement->StartTrackPredictionErrors( local );

	if ( cmd->weaponselect != 0 )
	{
		C_TFWeaponBase* weapon = i::entity_list->GetClientEntity( cmd->weaponselect )->As<C_TFWeaponBase>( );

		if ( weapon )
		{
			local->SelectItem( weapon->GetName( ), cmd->weaponsubtype );
		}
	} 

	const auto& vehicle = local->m_hVehicle( ).Get( );
	if ( cmd->impulse )
	{
		if ( !vehicle || local->UsingStandardWeaponsInVehicle( ) )
		{
			local->SetImpulse( cmd->impulse );
		}
	}

	local->UpdateButtonState( cmd->buttons );

	i::prediction->SetLocalViewAngles( cmd->viewangles );

	if ( local->PhysicsRunThink( ) )
	{
		local->PreThink( );
	}

	const int nThinkTick = local->m_nNextThinkTick( );

	if ( ( nThinkTick > 0 ) && ( nThinkTick <= get_tickbase( local, cmd ) ) )
	{
		local->SetNextThinkTick( -1 );
		local->Think( );
	}

	i::move_helper->SetHost( local );

	i::prediction->SetupMove( local, cmd, nullptr, &move_data );
	i::game_movement->ProcessMovement( local, &move_data );
	i::prediction->FinishMove( local, cmd, &move_data );
}

void __engine_prediction::end( C_TFPlayer* local, CUserCmd* cmd )
{
	if ( !local || local->deadflag( ) || !i::move_helper || global_info.shifting )
	{
		return;
	}

	i::game_movement->FinishTrackPredictionErrors( local );

	local->m_pCurrentCommand( ) = nullptr;
	*i::random_seed = -1;
	C_BasePlayer::SetPredictionPlayer( nullptr );

	i::move_helper->SetHost( nullptr );

	i::global_vars->curtime = old_current_time;
	i::global_vars->frametime = old_frame_time;
	i::global_vars->tickcount = old_tick_count;
	local->m_fFlags( ) = old_flags;

	i::prediction->m_bInPrediction = old_in_prediction;
	i::prediction->m_bFirstTimePredicted = old_is_first_prediction;

	local->m_nTickBase( ) = old_tickbase;
}