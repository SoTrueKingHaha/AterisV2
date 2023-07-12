#include "movesim.h"


//we'll use this to set current player's command, without it CGameMovement::CheckInterval will try to access a nullptr
static CUserCmd DummyCmd = {};

//since we're going to call game functions some entity data will be modified (we'll modify it too), we'll have to restore it after running
class CPlayerDataBackup
{
public:
	vector m_vecOrigin = {};
	vector m_vecVelocity = {};
	vector m_vecBaseVelocity = {};
	vector m_vecViewOffset = {};
	vector m_vecWorldSpaceCenter = {};
	C_BaseEntity* m_hGroundEntity = 0;
	int m_fFlags = 0;
	float m_flDucktime = 0.0f;
	float m_flDuckJumpTime = 0.0f;
	bool m_bDucked = false;
	bool m_bDucking = false;
	bool m_bInDuckJump = false;
	float m_flModelScale = 0.0f;

public:
	void Store( C_TFPlayer* pPlayer )
	{
		m_vecOrigin = pPlayer->m_vecOrigin( );
		m_vecVelocity = pPlayer->m_vecVelocity( );
		m_vecBaseVelocity = pPlayer->m_vecBaseVelocity( );
		m_vecViewOffset = pPlayer->m_vecViewOffset( );
		m_hGroundEntity = pPlayer->m_hGroundEntity( ).Get( );
		m_vecWorldSpaceCenter = pPlayer->GetWorldSpaceCenter( );
		m_fFlags = pPlayer->m_fFlags( ).GetFlags( );
		m_flDucktime = pPlayer->m_flDucktime( );
		m_flDuckJumpTime = pPlayer->m_flDuckJumpTime( );
		m_bDucked = pPlayer->m_bDucked( );
		m_bDucking = pPlayer->m_bDucking( );
		m_bInDuckJump = pPlayer->m_bInDuckJump( );
		m_flModelScale = pPlayer->m_flModelScale( );
	}

	void Restore( C_TFPlayer* pPlayer )
	{
		pPlayer->m_vecOrigin( ) = m_vecOrigin;
		pPlayer->m_vecVelocity( ) = m_vecVelocity;
		pPlayer->m_vecBaseVelocity( ) = m_vecBaseVelocity;
		pPlayer->m_vecViewOffset( ) = m_vecViewOffset;
		pPlayer->m_hGroundEntity( ).Set( m_hGroundEntity );
		pPlayer->m_fFlags( ) = m_fFlags;
		pPlayer->m_flDucktime( ) = m_flDucktime;
		pPlayer->m_flDuckJumpTime( ) = m_flDuckJumpTime;
		pPlayer->m_bDucked( ) = m_bDucked;
		pPlayer->m_bDucking( ) = m_bDucking;
		pPlayer->m_bInDuckJump( ) = m_bInDuckJump;
		pPlayer->m_flModelScale( ) = m_flModelScale;
	}
};

static CPlayerDataBackup PlayerDataBackup = {};

void __move_sim::SetupMoveData( C_TFPlayer* pPlayer, CMoveData* pMoveData )
{
	if ( !pPlayer || !pMoveData )
	{
		return;
	}

	pMoveData->m_bFirstRunOfFunctions = false;
	pMoveData->m_bGameCodeMovedPlayer = false;
	pMoveData->m_nPlayerHandle = reinterpret_cast< IHandleEntity* >( pPlayer )->GetRefEHandle( );
	pMoveData->m_vecVelocity = pPlayer->m_vecVelocity( );	//	m_vecBaseVelocity hits -1950?
	pMoveData->m_vecAbsOrigin = pPlayer->m_vecOrigin( );
	pMoveData->m_flMaxSpeed = pPlayer->CalculateMaxSpeed( );

	if ( PlayerDataBackup.m_fFlags & FL_DUCKING )
	{
		pMoveData->m_flMaxSpeed *= 0.3333f;
	}

	pMoveData->m_flClientMaxSpeed = pMoveData->m_flMaxSpeed;

	//need a better way to determine angles probably
	pMoveData->m_vecViewAngles = { 0.0f, math.velocity_to_angles( pMoveData->m_vecVelocity ).y, 0.0f };

	vector vForward = {}, vRight = {};
	math.angle_vectors( pMoveData->m_vecViewAngles, &vForward, &vRight, nullptr );

	pMoveData->m_flForwardMove = ( pMoveData->m_vecVelocity.y - vRight.y / vRight.x * pMoveData->m_vecVelocity.x ) / ( vForward.y - vRight.y / vRight.x * vForward.x );
	pMoveData->m_flSideMove = ( pMoveData->m_vecVelocity.x - vForward.x * pMoveData->m_flForwardMove ) / vRight.x;
}

bool __move_sim::init( C_TFPlayer* pPlayer )
{
	if ( !i::game_movement || !pPlayer || pPlayer->deadflag( ) )
	{
		return false;
	}
	
	//set player
	m_pPlayer = pPlayer;

	//set current command
	m_pPlayer->SetCurrentCmd( &DummyCmd );
	//m_pPlayer->SetCurrentCommand(&DummyCmd);

	//store player's data
	PlayerDataBackup.Store( m_pPlayer );

	//store vars
	m_bOldInPrediction = i::prediction->m_bInPrediction;
	m_bOldFirstTimePredicted = i::prediction->m_bFirstTimePredicted;
	m_flOldFrametime = i::global_vars->frametime;
	bDontPredict = false;

	//the hacks that make it work
	{
		if ( pPlayer->m_fFlags( ).IsFlagSet( FL_DUCKING ) )
		{
			pPlayer->m_fFlags( ).SetFlag( ~FL_DUCKING ); //breaks origin's z if FL_DUCKING is not removed
			pPlayer->m_bDucked( ) = true; //(mins/maxs will be fine when ducking as long as m_bDucked is true)
			pPlayer->m_flDucktime( ) = 0.0f;
			pPlayer->m_flDuckJumpTime( ) = 0.0f;
			pPlayer->m_bDucking( ) = false;
			pPlayer->m_bInDuckJump( ) = false;
		}

		if ( pPlayer != GET_LOCAL_PLAYER( ) )
		{
			pPlayer->m_hGroundEntity( ) = 0; //without this nonlocal players get snapped to the floor
		}

		pPlayer->m_flModelScale( ) -= 0.03125f; //fixes issues with corners

		if ( pPlayer->m_fFlags( ).IsFlagSet( FL_ONGROUND ) )
		{
			pPlayer->m_vecOrigin( ).z += 0.03125f; //to prevent getting stuck in the ground
		}

		//for some reason if xy vel is zero it doesn't predict
		if ( fabsf( pPlayer->m_vecVelocity( ).x ) < 0.01f )
		{
			pPlayer->m_vecVelocity( ).x = 0.015f;
		}

		if ( fabsf( pPlayer->m_vecVelocity( ).y ) < 0.01f )
		{
			pPlayer->m_vecVelocity( ).y = 0.015f;
		}
	}

	//setup move data
	SetupMoveData( m_pPlayer, &m_MoveData );

	return true;
}

void __move_sim::restore( )
{
	if ( !m_pPlayer )
	{
		return;
	}

	m_pPlayer->SetCurrentCmd( nullptr );

	PlayerDataBackup.Restore( m_pPlayer );

	i::prediction->m_bInPrediction = m_bOldInPrediction;
	i::prediction->m_bFirstTimePredicted = m_bOldFirstTimePredicted;
	i::global_vars->frametime = m_flOldFrametime;

	m_pPlayer = nullptr;

	memset( &m_MoveData, 0, sizeof( CMoveData ) );
	memset( &PlayerDataBackup, 0, sizeof( CPlayerDataBackup ) );
}

void __move_sim::FillVelocities( )
{
	const auto& pLocal = entity_cache.get_local( );

	for ( const auto& pEntity : entity_cache.players[ TEAMSALL ] )
	{
		const auto& pPlayer = pEntity->As<C_TFPlayer>( );
		const int iEntIndex = pPlayer->entindex( );

		if ( pPlayer == pLocal || pPlayer->m_iTeamNum( ) == pLocal->m_iTeamNum( ) )
		{
			continue;
		}

		if ( pPlayer->deadflag( ) || pPlayer->IsDormant( ) || pPlayer->m_vecVelocity( ).is_zero( ) )
		{
			m_Velocities[ iEntIndex ].clear( );
			continue;
		}

		const vector vVelocity = pPlayer->m_vecVelocity( );
		m_Velocities[ iEntIndex ].push_front( vVelocity );

		while ( m_Velocities[ iEntIndex ].size( ) > 64 /*Vars::Aimbot::Projectile::StrafePredictionSamples.var*/ )
		{
			m_Velocities[ iEntIndex ].pop_back( );
		}
	}
}

void __move_sim::run_tick( CMoveData& moveDataOut, vector& m_vecAbsOrigin )
{
	if ( !i::game_movement || !m_pPlayer || bDontPredict )
	{
		return;
	}

	if ( m_pPlayer->GetClassID( ) != ETFClassID::CTFPlayer )
	{
		return;
	}

	bFirstRunTick = true;

	//make sure frametime and prediction vars are right
	i::prediction->m_bInPrediction = true;
	i::prediction->m_bFirstTimePredicted = false;
	i::global_vars->frametime = i::prediction->m_bEnginePaused ? 0.0f : TICK_INTERVAL;

	//if ( !StrafePrediction( ) )
	m_MoveData.m_vecViewAngles = { 0.0f, math.velocity_to_angles( m_MoveData.m_vecVelocity ).y, 0.0f };

	//call CTFGameMovement::ProcessMovement
	using ProcessMovement_FN = void( __thiscall* )( void*, C_BaseEntity*, CMoveData* );
	reinterpret_cast< ProcessMovement_FN >( memory->get_vfunc( i::game_movement, 1 ) )( i::game_movement, m_pPlayer, &m_MoveData );

	moveDataOut = m_MoveData;
	m_vecAbsOrigin = m_MoveData.m_vecAbsOrigin;
}