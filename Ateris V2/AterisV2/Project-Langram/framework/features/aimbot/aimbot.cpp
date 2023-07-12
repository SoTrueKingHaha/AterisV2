#include "aimbot.h"
#include "../crits/crits.h"
#include "proj_sim.h"
#include "../backtrack/backtrack.h"

bool __aimbot::should_run( C_TFPlayer* local, C_TFWeaponBase* weapon )
{
	if ( !vars.aimbot.global.global_enabled )
		return false;

	if ( i::engine_vgui->IsGameUIVisible( ) || i::surface->IsCursorVisible( ) )
		return false;

	if ( local->deadflag( )
		 || local->InCond( TF_COND_TAUNTING )
		 || local->InCond( TF_COND_PHASE )
		 || local->m_bFeignDeathReady( )
		 || local->InCond( TF_COND_STEALTHED )
		 || local->InCond( TF_COND_HALLOWEEN_KART )
		 || local->InCond( TF_COND_HALLOWEEN_GHOST_MODE ) )
		return false;

	switch ( weapon->m_iItemDefinitionIndex( ) ) {
		case Soldier_m_RocketJumper:
		case Demoman_s_StickyJumper: return false;
		default: break;
	}

	switch ( weapon->GetWeaponID( ) )
	{
		case TF_WEAPON_PDA:
		case TF_WEAPON_PDA_ENGINEER_BUILD:
		case TF_WEAPON_PDA_ENGINEER_DESTROY:
		case TF_WEAPON_PDA_SPY:
		case TF_WEAPON_PDA_SPY_BUILD:
		case TF_WEAPON_BUILDER:
		case TF_WEAPON_INVIS:
		case TF_WEAPON_LUNCHBOX:
		case TF_WEAPON_BUFF_ITEM:
		case TF_WEAPON_GRAPPLINGHOOK:
		{
			return false;
		}

		default: break;
	}

	return true;
}

void __aimbot::run( CUserCmd* cmd, bool* send_packet )
{
	const auto& local = entity_cache.get_local( );
	const auto& weapon = entity_cache.get_weapon( );

	if ( !local || !weapon )
	{
		return;
	}

	if ( !should_run( local, weapon ) )
	{
		return;
	}

	switch ( weapon_type )
	{
		case EWeaponType::HITSCAN:
		{
			hitscan_run( local, weapon, cmd );
			break;
		}
		case EWeaponType::PROJECTILE:
		{
			projectile_run( local, weapon, cmd, send_packet );
			break;
		}
		default: break;
	}
}

bool __aimbot::is_key_held( )
{
	return !vars.aimbot.global.aimbot_key ? true : ( ( GetAsyncKeyState( vars.aimbot.global.aimbot_key ) & 0x8000 ) && utils.is_window_in_focus( ) );
}

void __aimbot::sort_targets( )
{
	std::sort( m_vecTargets.begin( ), m_vecTargets.end( ), [ & ] ( const Target_t& a, const Target_t& b ) -> bool
	{
		return ( a.m_flFOVTo < b.m_flFOVTo );
	} );
}

bool __aimbot::get_targets( C_TFPlayer* local, C_TFWeaponBase* weapon )
{
	m_vecTargets.clear( );
	global_info.aimbot_fov = vars.aimbot.hitscan.hitscan_fov;

	vector local_pos = local->GetShootPosition( );
	vector local_angles = i::engine_client->GetViewAngles( );

	if ( vars.aimbot.global.aimbot_aimplayers )
	{
		int hitbox = vars.aimbot.hitscan.hitscan_hitbox;
		bool is_medigun = weapon->GetWeaponID( ) == TF_WEAPON_MEDIGUN;

		for ( const auto& player : entity_cache.players[ is_medigun ? TEAM : ENEMY ] )
		{
			if ( !player || player->deadflag( ) || player->IsDormant( ) || player->InCond( TF_COND_HALLOWEEN_GHOST_MODE ) || player == local )
				continue;

			if ( vars.aimbot.global.aimbot_ignore_invincible && ( player->InCond( TF_COND_INVULNERABLE ) || player->InCond( TF_COND_PHASE ) ) )
				continue;

			if ( vars.aimbot.global.aimbot_ignore_cloaked && player->InCond( TF_COND_STEALTHED ) )
				continue;

			if ( vars.aimbot.global.aimbot_ignore_taunting && player->InCond( TF_COND_TAUNTING ) )
				continue;

			if ( vars.aimbot.global.aimbot_ignore_friends && player->PlayerOnFriendsList( ) && player->m_iTeamNum( ) != local->m_iTeamNum( ) )
				continue;

			vector pos = player->GetHitboxPosition( hitbox );
			vector angle_to = math.calc_angle( local_pos, pos );
			float fov_to = math.calc_fov( local_angles, angle_to );

			if ( fov_to > vars.aimbot.hitscan.hitscan_fov )
				continue;

			m_vecTargets.push_back( { player, ETargetType::PLAYER, pos, angle_to, fov_to } );
		}
	}

	if ( vars.aimbot.global.aimbot_aimbuildings )
	{
		for ( const auto& sentry : entity_cache.sentries[ ENEMY ] )
		{
			if ( !sentry || sentry->m_iTeamNum( ) == local->m_iTeamNum( ) || sentry->IsDormant( ) )
				continue;

			vector pos = sentry->GetWorldSpaceCenter( );
			vector angle_to = math.calc_angle( local_pos, pos );
			float fov_to = math.calc_fov( local_angles, angle_to );

			if ( fov_to > vars.aimbot.hitscan.hitscan_fov )
				continue;

			m_vecTargets.push_back( { sentry, ETargetType::BUILDING, pos, angle_to, fov_to } );
		}
	}

	if ( vars.aimbot.global.aimbot_aimstickies )
	{
		for ( const auto& projectile : entity_cache.projectiles[ ENEMY ] )
		{
			if ( !projectile || projectile->m_iTeamNum( ) == local->m_iTeamNum( ) || projectile->IsDormant( ) )
				continue;

			if ( projectile->m_nPipebombType( ) != TYPE_STICKY )
			{
				continue;
			}

			vector pos = projectile->GetWorldSpaceCenter( );
			vector angle_to = math.calc_angle( local_pos, pos );
			float fov_to = math.calc_fov( local_angles, angle_to );

			if ( fov_to > vars.aimbot.hitscan.hitscan_fov )
				continue;

			m_vecTargets.push_back( { projectile, ETargetType::STICKY, pos, angle_to, fov_to } );
		}
	}

	return !m_vecTargets.empty( );
}

bool __aimbot::hitscan_use_all_player_hitboxes( C_TFPlayer* local, Target_t& target )
{
	if ( !vars.aimbot.hitscan.hitscan_multipoint )
	{
		return false;
	}

	if ( target.m_TargetType == ETargetType::PLAYER )
	{
		vector local_pos = local->GetShootPosition( );

		for ( int hitbox = 1; hitbox < target.m_pEntity->As<C_TFPlayer>( )->GetNumOfHitboxes( ); hitbox++ )
		{
			if ( hitbox == HITBOX_PELVIS || hitbox == HITBOX_LOWERARM_L || hitbox == HITBOX_HAND_L || hitbox == HITBOX_LOWERARM_R || hitbox ==  HITBOX_HAND_R )
			{
				continue;
			}

			vector hitbox_pos = target.m_pEntity->As<C_TFPlayer>( )->GetHitboxPosition( hitbox );
			//i::debug_overlay->AddTextOverlay( hitbox_pos, 0.09f, "%i", hitbox );

			if ( utils.vis_pos( local, target.m_pEntity, local_pos, hitbox_pos ) )
			{
				target.m_vAngleTo = math.calc_angle( local_pos, hitbox_pos );
				return true;
			}
		}
	}

	return false;
}

bool __aimbot::hitscan_multipoint_buildings( C_TFPlayer* local, Target_t& target )
{
	vector vLocalPos = local->GetShootPosition( );

	vector vMins = target.m_pEntity->m_vecMins( );
	vector vMaxs = target.m_pEntity->m_vecMaxs( );

	const std::vector<vector> vecPoints = {
		vector( vMins.x, ( ( vMins.y + vMaxs.y ) * 0.5f ), ( ( vMins.z + vMaxs.z ) * 0.5f ) ),
		vector( vMaxs.x, ( ( vMins.y + vMaxs.y ) * 0.5f ), ( ( vMins.z + vMaxs.z ) * 0.5f ) ),
		vector( ( ( vMins.x + vMaxs.x ) * 0.5f ), vMins.y, ( ( vMins.z + vMaxs.z ) * 0.5f ) ),
		vector( ( ( vMins.x + vMaxs.x ) * 0.5f ), vMaxs.y, ( ( vMins.z + vMaxs.z ) * 0.5f ) ),
		vector( ( ( vMins.x + vMaxs.x ) * 0.5f ), ( ( vMins.y + vMaxs.y ) * 0.5f ), vMins.z ),
		vector( ( ( vMins.x + vMaxs.x ) * 0.5f ), ( ( vMins.y + vMaxs.y ) * 0.5f ), vMaxs.z )
	};

	const matrix3x4_t& Transform = target.m_pEntity->RenderableToWorldTransform( );

	for ( const auto& Point : vecPoints )
	{
		vector vTransformed = {};
		math.vector_transform( Point, Transform, vTransformed );
		//i::debug_overlay->AddTextOverlay( vTransformed, 0.09f, "x" );

		if ( utils.vis_pos( local, target.m_pEntity, vLocalPos, vTransformed ) ) {
			target.m_vPos = vTransformed;
			target.m_vAngleTo = math.calc_angle( vLocalPos, vTransformed );
			return true;
		}
	}

	return false;
}

bool __aimbot::hitscan_multipoint_head( C_TFPlayer* local, Target_t& target )
{
	auto pModel = target.m_pEntity->GetModel( );
	if ( !pModel )
		return false;

	studiohdr_t* pHDR = reinterpret_cast< studiohdr_t* >( i::model_info->GetStudiomodel( pModel ) );
	if ( !pHDR )
		return false;

	matrix3x4_t BoneMatrix[ 128 ];
	if ( !target.m_pEntity->SetupBones( BoneMatrix, 128, 0x100, i::global_vars->curtime ) )
		return false;

	mstudiohitboxset_t* pSet = pHDR->GetHitboxSet( target.m_pEntity->As<C_TFPlayer>( )->m_nHitboxSet( ) );
	if ( !pSet )
		return false;

	mstudiobbox_t* pBox = pSet->hitbox( HITBOX_HEAD );
	if ( !pBox )
		return false;

	vector vLocalPos = local->GetShootPosition( );
	vector vMins = pBox->bbmin;
	vector vMaxs = pBox->bbmax;

	const float fScale = vars.aimbot.hitscan.hitscan_multipoint_head_scale;
	const std::vector<vector> vecPoints = {
		vector( ( ( vMins.x + vMaxs.x ) * 0.5f ), ( vMins.y * fScale ), ( ( vMins.z + vMaxs.z ) * 0.5f ) ),
		vector( ( ( vMins.x + vMaxs.x ) * 0.5f ), ( vMaxs.y * fScale ), ( ( vMins.z + vMaxs.z ) * 0.5f ) ),
		vector( ( vMins.x * fScale ), ( ( vMins.y + vMaxs.y ) * 0.5f ), ( ( vMins.z + vMaxs.z ) * 0.5f ) ),
		vector( ( vMaxs.x * fScale ), ( ( vMins.y + vMaxs.y ) * 0.5f ), ( ( vMins.z + vMaxs.z ) * 0.5f ) )
	};

	for ( const auto& Point : vecPoints )
	{
		vector vTransformed = {};
		math.vector_transform( Point, BoneMatrix[ pBox->bone ], vTransformed );
		//i::debug_overlay->AddTextOverlay( vTransformed, 0.09f, "x" );

		if ( utils.vis_pos( local, target.m_pEntity, vLocalPos, vTransformed ) ) {
			target.m_vPos = vTransformed;
			target.m_vAngleTo = math.calc_angle( vLocalPos, vTransformed );
			return true;
		}
	}

	return false;
}

bool __aimbot::hitscan_multipoint_pelvis( C_TFPlayer * local, Target_t & target )
{
	auto pModel = target.m_pEntity->GetModel( );
	if ( !pModel )
		return false;

	studiohdr_t* pHDR = reinterpret_cast< studiohdr_t* >( i::model_info->GetStudiomodel( pModel ) );
	if ( !pHDR )
		return false;

	matrix3x4_t BoneMatrix[ 128 ];
	if ( !target.m_pEntity->SetupBones( BoneMatrix, 128, 0x100, i::global_vars->curtime ) )
		return false;

	mstudiohitboxset_t* pSet = pHDR->GetHitboxSet( target.m_pEntity->As<C_TFPlayer>( )->m_nHitboxSet( ) );
	if ( !pSet )
		return false;

	mstudiobbox_t* pBox = pSet->hitbox( HITBOX_PELVIS );
	if ( !pBox )
		return false;

	vector vLocalPos = local->GetShootPosition( );
	vector vMins = pBox->bbmin;
	vector vMaxs = pBox->bbmax;

	const float fScale = vars.aimbot.hitscan.hitscan_multipoint_pelvis_scale;
	const std::vector<vector> vecPoints = {
		vector( ( ( vMins.x + vMaxs.x ) * 0.5f ), ( vMins.y * fScale ), ( ( vMins.z + vMaxs.z ) * 0.5f ) ),
		vector( ( ( vMins.x + vMaxs.x ) * 0.5f ), ( vMaxs.y * fScale ), ( ( vMins.z + vMaxs.z ) * 0.5f ) ),
		vector( ( vMins.x * fScale ), ( ( vMins.y + vMaxs.y ) * 0.5f ), ( ( vMins.z + vMaxs.z ) * 0.5f ) ),
		vector( ( vMaxs.x * fScale ), ( ( vMins.y + vMaxs.y ) * 0.5f ), ( ( vMins.z + vMaxs.z ) * 0.5f ) )
	};

	for ( const auto& Point : vecPoints )
	{
		vector vTransformed = {};
		math.vector_transform( Point, BoneMatrix[ pBox->bone ], vTransformed );
		//i::debug_overlay->AddTextOverlay( vTransformed, 0.09f, "x" );

		if ( utils.vis_pos( local, target.m_pEntity, vLocalPos, vTransformed ) ) {
			target.m_vPos = vTransformed;
			target.m_vAngleTo = math.calc_angle( vLocalPos, vTransformed );
			return true;
		}
	}

	return false;
}

bool __aimbot::hitscan_verify_target( C_TFPlayer* local, C_TFWeaponBase* weapon, Target_t& target )
{
	switch ( target.m_TargetType )
	{
		case ETargetType::PLAYER:
		{
			if ( vars.aimbot.hitscan.hitscan_hitbox == HITBOX_HEAD && vars.aimbot.hitscan.hitscan_multipoint_head )
			{
				int nHit = -1;

				if ( !utils.vis_pos( local, target.m_pEntity, local->GetShootPosition( ), target.m_vPos ) )
				{
					if ( !utils.vis_pos_hitbox_id_out( local, target.m_pEntity, local->GetShootPosition( ), target.m_vPos, nHit ) && nHit != HITBOX_HEAD )
					{
						if ( !hitscan_multipoint_head( local, target ) )
							return false;
					}
				}
			}

			else if ( vars.aimbot.hitscan.hitscan_hitbox == HITBOX_PELVIS && vars.aimbot.hitscan.hitscan_multipoint_pelvis )
			{
				int nHit = -1;

				if ( !utils.vis_pos( local, target.m_pEntity, local->GetShootPosition( ), target.m_vPos ) )
				{
					if ( !utils.vis_pos_hitbox_id_out( local, target.m_pEntity, local->GetShootPosition( ), target.m_vPos, nHit ) && nHit != HITBOX_PELVIS )
					{
						if ( !hitscan_multipoint_pelvis( local, target ) )
							return false;
					}
				}
			}

			else
			{
				if ( !utils.vis_pos( local, target.m_pEntity, local->GetShootPosition( ), target.m_vPos ) && !hitscan_use_all_player_hitboxes( local, target ) )
				{
					if ( backtrack.is_backtracking && vars.backtrack.backtrack_active )
					{
						if ( !utils.vis_pos( local, target.m_pEntity, local->GetShootPosition( ), vars.aimbot.hitscan.hitscan_hitbox == HITBOX_PELVIS ? backtrack.best_data.bodypos : backtrack.best_data.headpos ) )
						{
							return false;
						}
						else
						{
							should_backtrack = true;
							target.m_vAngleTo = math.calc_angle( local->GetShootPosition( ), vars.aimbot.hitscan.hitscan_hitbox == HITBOX_PELVIS ? backtrack.best_data.bodypos : backtrack.best_data.headpos );
						}
					}
					else
					{
						return false;
					}
				}
			}
			break;
		}
		case ETargetType::BUILDING:
		{
			if ( !utils.vis_pos( local, target.m_pEntity, local->GetShootPosition( ), target.m_vPos ) && !hitscan_multipoint_buildings( local, target ) )
			{
				return false;
			}
			break;
		}
		default:
		{
			if ( !utils.vis_pos( local, target.m_pEntity, local->GetShootPosition( ), target.m_vPos ) )
				return false;

			break;
		}
	}

	return true;
}

bool __aimbot::hitscan_should_fire( C_TFPlayer* local, C_TFWeaponBase* weapon, CUserCmd* cmd, const Target_t& target )
{
	if ( !vars.aimbot.global.aimbot_autoshoot )
	{
		return false;
	}

	if ( weapon->m_iItemDefinitionIndex( ) != Engi_m_TheWidowmaker && weapon->GetWeaponID( ) != TF_WEAPON_MINIGUN && weapon->GetWeaponID( ) != TF_WEAPON_SNIPERRIFLE && weapon->GetWeaponID( ) != TF_WEAPON_SNIPERRIFLE_DECAP && weapon->GetWeaponID( ) != TF_WEAPON_SNIPERRIFLE_CLASSIC )
	{
		if ( weapon->m_iClip1( ) < 1 )
		{
			return false;
		}
	}

	return true;
}

void __aimbot::hitscan_aim( CUserCmd* pCmd, vector& vAngle )
{
	vAngle -= global_info.punch_angles;
	utils.clamp_angles( vAngle );

	switch ( vars.aimbot.hitscan.hitscan_aim )
	{
		case 0: //Plain
		{
			pCmd->viewangles = vAngle;
			i::engine_client->SetViewAngles( pCmd->viewangles );
			break;
		}

		case 1: //Silent
		{
			utils.fix_movement( pCmd, vAngle );
			pCmd->viewangles = vAngle;
			break;
		}

		default: break;
	}
}

bool __aimbot::push_target( C_TFPlayer* local, C_TFWeaponBase* weapon, Target_t& out )
{
	if ( !get_targets( local, weapon ) )
	{
		return false;
	}

	sort_targets( );

	for ( auto& target : m_vecTargets )
	{
		if ( !hitscan_verify_target( local, weapon, target ) )
		{
			continue;
		}
		out = target;
		return true;
	}
	return false;
}

void __aimbot::hitscan_run( C_TFPlayer* local, C_TFWeaponBase* weapon, CUserCmd* cmd )
{
	if ( !vars.aimbot.hitscan.hitscan_active || !utils.is_window_in_focus( ) )
	{
		return;
	}

	should_backtrack = false;
	Target_t main_target{ };
	bool bIsAttacking = cmd->buttons & IN_ATTACK;
	const bool bShouldAim = ( vars.aimbot.global.aimbot_key == VK_LBUTTON ? ( bIsAttacking ) : is_key_held( ) );

	if ( !bShouldAim )
	{
		return;
	}

	if ( !push_target( local, weapon, main_target ) )
	{
		return;
	}

	if ( !hitscan_verify_target( local, weapon, main_target ) )
	{
		return;
	}

	global_info.hitscan_running = true;
	global_info.hitscan_silent = vars.aimbot.hitscan.hitscan_aim == 1;

	if ( hitscan_should_fire( local, weapon, cmd, main_target ) )
	{
		cmd->buttons |= IN_ATTACK;
		
		if ( vars.misc.cl_move.doubletap && global_info.shifted > 22 - global_info.ticks && local->IsOnGround( ) )
		{
			global_info.shifting = true;
		}

		if ( weapon->GetWeaponID( ) == TF_WEAPON_MINIGUN && !global_info.shifting && main_target.m_pEntity )
		{
			bool bDo = local->GetAbsOrigin( ).dist_to( main_target.m_pEntity->GetAbsOrigin( ) ) > 1000.0f;

			if ( bDo && weapon->GetWeaponSpread( ) )
			{
				float flTimeSinceLastShot = ( local->m_nTickBase( ) * TICK_INTERVAL ) - weapon->m_flLastFireTime( );

				if ( weapon->GetWeaponData( ).m_nBulletsPerShot > 1 )
				{
					if ( flTimeSinceLastShot <= 0.25f )
						cmd->buttons &= ~IN_ATTACK;
				}

				else if ( flTimeSinceLastShot <= 1.25f )
					cmd->buttons &= ~IN_ATTACK;
			}
		}

		if ( vars.misc.disable_interp && main_target.m_TargetType == ETargetType::PLAYER && !should_backtrack )
		{
			cmd->tick_count = TIME_TO_TICKS( main_target.m_pEntity->As<C_TFPlayer>( )->m_flSimulationTime( ) + utils.lerp_time( ) );
		}
	}

	if ( !main_target.m_vAngleTo.is_zero( ) )
	{
		hitscan_aim( cmd, main_target.m_vAngleTo );
	}
}

bool __aimbot::GetProjectileInfo( C_TFWeaponBase* pWeapon, ProjectileInfo_t& out )
{
	switch ( pWeapon->GetWeaponID( ) )
	{
		case TF_WEAPON_ROCKETLAUNCHER:
		case TF_WEAPON_ROCKETLAUNCHER_DIRECTHIT:
		case TF_WEAPON_PARTICLE_CANNON:
		{
			m_bIsBoosted = true;
			out = { G::AttribHookValue( 1100.0f, "mult_projectile_speed", pWeapon ), 0.0f };
			break;
		}

		case TF_WEAPON_GRENADELAUNCHER:
		{
			bool isLochnLoad = pWeapon->m_iItemDefinitionIndex( ) == Demoman_m_TheLochnLoad;
			float speed = isLochnLoad ? 1490.0f : 1200.0f;

			m_bIsBoosted = true;
			out = { speed, 0.5f, G::AttribHookValue( 3.0f, "fuse_mult", pWeapon ) };
			break;

		}

		case TF_WEAPON_PIPEBOMBLAUNCHER:
		{
			float charge = ( i::global_vars->curtime - pWeapon->As<C_TFPipebombLauncher>( )->m_flChargeBeginTime( ) );
			float speed = math.remap_val_clamped( charge, 0.0f, G::AttribHookValue( 4.0f, "stickybomb_charge_rate", pWeapon ), 900.0f, 2400.0f );
			float grav_mod = math.remap_val_clamped( charge, 0.0f, G::AttribHookValue( 4.0f, "stickybomb_charge_rate", pWeapon ), 0.5f, 0.1f );

			out = { speed, grav_mod };
			break;
		}

		case TF_WEAPON_CANNON:
		{
			m_bIsBoosted = true;
			out = { 1454.0f, 0.5f };
			break;
		}

		case TF_WEAPON_FLAREGUN:
		{
			out = { 2000.0f, 0.2f };
			break;
		}

		case TF_WEAPON_CLEAVER:
		case TF_WEAPON_FLAREGUN_REVENGE:
		{
			out = { 3000.0f, 0.45f };
			break;
		}

		case TF_WEAPON_COMPOUND_BOW:
		{
			float charge = ( i::global_vars->curtime - pWeapon->As<C_TFPipebombLauncher>( )->m_flChargeBeginTime( ) );
			float speed = math.remap_val_clamped( charge, 0.0f, 1.0f, 1800.0f, 2600.0f );
			float grav_mod = math.remap_val_clamped( charge, 0.0f, 1.0f, 0.5f, 0.1f );

			out = { speed, grav_mod };
			break;
		}

		case TF_WEAPON_SYRINGEGUN_MEDIC:
		{
			out = { 1000.0f, 0.2f };
			break;
		}

		case TF_WEAPON_FLAMETHROWER:
		{
			out = { 1000.0f, 0.0f, 0.33f };
			m_bIsFlameThrower = true;
			break;
		}

		case TF_WEAPON_FLAME_BALL: //dragon's fury
		{
			out = { 3000.0f, 0.0f, 0.1753f };
			m_bIsFlameThrower = true;
			break;
		}

		case TF_WEAPON_RAYGUN:
		{
			out = { 1200.0f, 0.0f };
			break;
		}

		case TF_WEAPON_CROSSBOW:
		case TF_WEAPON_SHOTGUN_BUILDING_RESCUE:
		{
			out = { 2400.0f, 0.2f };
			break;
		}
	}

	return out.m_flVelocity;
}

bool __aimbot::CalcProjAngle( const vector& vLocalPos, const vector& vTargetPos, const ProjectileInfo_t& projInfo, Solution_t& out )
{
	const vector v = vTargetPos - vLocalPos;
	const float dx = sqrt( v.x * v.x + v.y * v.y );
	const float dy = v.z;
	const float v0 = projInfo.m_flVelocity;

	//Ballistics
	if ( const float g = convars.sv_gravity->GetFloat( ) * projInfo.m_flGravity )
	{
		const float root = v0 * v0 * v0 * v0 - g * ( g * dx * dx + 2.0f * dy * v0 * v0 );

		if ( root < 0.0f )
		{
			return false;
		}

		out.m_flPitch = atan( ( v0 * v0 - sqrt( root ) ) / ( g * dx ) );
		out.m_flYaw = atan2( v.y, v.x );
	}

	//Straight trajectory (the time stuff later doesn't make sense with this but hey it works!)
	else
	{
		const vector vecAngle = math.calc_angle( vLocalPos, vTargetPos );
		out.m_flPitch = -DEG2RAD( vecAngle.x );
		out.m_flYaw = DEG2RAD( vecAngle.y );
	}

	out.m_flTime = dx / ( cos( out.m_flPitch ) * v0 );

	return true;
}

bool __aimbot::SolveProjectile( C_TFPlayer* pLocal, C_TFWeaponBase* pWeapon, CUserCmd* pCmd, Predictor_t& predictor, const ProjectileInfo_t& projInfo, Solution_t& out )
{
	auto pNetChannel = i::engine_client->GetNetChannelInfo( );
	static ConVar* cl_flipviewmodels = convars.cl_flipviewmodels;

	global_info.prediction_lines.clear( );

	if ( !pNetChannel || !cl_flipviewmodels )
	{
		return false;
	}

	static bool oValue = cl_flipviewmodels->GetBool( ); // assume false

	Ray_t traceRay = {};
	CGameTrace trace = {};
	static CTraceFilterWorldAndPropsOnly traceFilter = {};
	traceFilter.pSkip = predictor.m_pEntity;

	vector vLocalPos = pLocal->GetShootPosition( );
	const float maxTime = predictor.m_pEntity->IsPlayer( )
		? ( projInfo.m_flMaxTime == 0.f ? 3.0f : projInfo.m_flMaxTime )
		: ( projInfo.m_flMaxTime == 0.f ? 1024.f : projInfo.m_flMaxTime );
	const float fLatency = pNetChannel->GetLatency( MAX_FLOWS );

	//This should now be able to predict anything that moves.
	//Should also stop wasting time predicting static players.
	
	const bool useTPred = !predictor.m_pEntity->As<C_TFPlayer>( )->m_vecVelocity( ).is_zero( ) ? true : false;

	if ( !useTPred ) {

		vector staticPos = predictor.m_pEntity->IsPlayer( ) ? GetAimPos( pLocal, predictor.m_pEntity, predictor.m_vPosition ) : GetAimPosBuilding( pLocal, predictor.m_pEntity );
		if ( staticPos.is_zero( ) ) {
			return false;
		}

		// get angle offsets for demoman weapons?weew
		switch ( pWeapon->GetWeaponID( ) )
		{
			case TF_WEAPON_GRENADELAUNCHER:
			case TF_WEAPON_PIPEBOMBLAUNCHER:
			case TF_WEAPON_STICKBOMB:
			case TF_WEAPON_STICKY_BALL_LAUNCHER:
			{
				vector vDelta = ( staticPos - vLocalPos );
				const float fRange = math.vector_normalize( vDelta );
				const float fElevationAngle = std::min( fRange * ( pWeapon->m_iItemDefinitionIndex( ) == Demoman_m_TheLochnLoad ? 0.0075f : 0.013f ), 45.f ); // if our angle is above 45 degree will we even hit them? shouldn't we just return???

				float s = 0.0f, c = 0.0f;
				math.sin_cos( ( fElevationAngle * PI / 180.0f ), &s, &c );

				const float fElevation = ( fRange * ( s / c ) );
				staticPos.z += ( c > 0.0f ? fElevation : 0.0f );
				break;
			}

			default: break;
		}

		// trace hull of projectile
		utils.trace_hull( predictor.m_vPosition, staticPos, vector( -3.8f, -3.8f, -3.8f ), vector( 3.8f, 3.8f, 3.8f ), MASK_SOLID_BRUSHONLY, &traceFilter, &trace );
		if ( trace.DidHit( ) )
		{
			staticPos.z = trace.m_vEndPos.z;
		}

		switch ( pWeapon->GetWeaponID( ) )
		{
			case TF_WEAPON_GRENADELAUNCHER:
			case TF_WEAPON_PIPEBOMBLAUNCHER:
			case TF_WEAPON_STICKBOMB:
			case TF_WEAPON_STICKY_BALL_LAUNCHER:
			{
				vector vecOffset( 16.0f, 8.0f, -6.0f );
				utils.projectile_setup( pLocal, pCmd->viewangles, vecOffset, &vLocalPos );
				break;
			}

			default: break;
		}

		if ( !CalcProjAngle( vLocalPos, staticPos, projInfo, out ) )
		{
			return false;
		}

		if ( out.m_flTime > maxTime ) {
			return false;
		}

		if ( WillProjectileHit( pLocal, pWeapon, pCmd, staticPos, out, projInfo, predictor ) ) {
			global_info.predicted_pos = staticPos;
			return true;
		}
	}
	else {
		vector vPredictedPos = {};
		CMoveData moveData = {};
		vector absOrigin = {};

		if ( move_sim.init( predictor.m_pEntity->As<C_TFPlayer>( ) ) )
		{
			if ( !predictor.m_pEntity ) {
				move_sim.restore( );
				return false;
			}

			for ( int n = 0; n < TIME_TO_TICKS( maxTime ); n++ )
			{
				if ( predictor.m_pEntity == nullptr )
				{
					break;
				}
				move_sim.run_tick( moveData, absOrigin );
				vPredictedPos = absOrigin;

				const vector aimPosition = GetAimPos( pLocal, predictor.m_pEntity, vPredictedPos );
				if ( aimPosition.is_zero( ) ) {
					break;
				} // don't remove.

				//const vector vAimDelta = predictor.m_pEntity->GetAbsOrigin() - aimPosition;
				//vPredictedPos.x += abs(vAimDelta.x);
				//vPredictedPos.y += abs(vAimDelta.y);
				//vPredictedPos.z += abs(vAimDelta.z);
				vPredictedPos = aimPosition;

				//Utils::RotateVec2( *( Vec2 * )&vPredictedPos, *( Vec2 * )&absOrigin, DEG2RAD( 0 * ( vPredictedPos - absOrigin ).Length2D( ) ) );
				//global_info.PredictionLines.push_back( vPredictedPos );

				// get angle offsets for demoman weapons?
				switch ( pWeapon->GetWeaponID( ) )
				{
					case TF_WEAPON_GRENADELAUNCHER:
					case TF_WEAPON_PIPEBOMBLAUNCHER:
					case TF_WEAPON_STICKBOMB:
					case TF_WEAPON_STICKY_BALL_LAUNCHER:
					{
						vector vDelta = ( vPredictedPos - vLocalPos );
						const float fRange = math.vector_normalize( vDelta );
						const float fElevationAngle = std::min( fRange * ( pWeapon->m_iItemDefinitionIndex( ) == Demoman_m_TheLochnLoad ? 0.0075f : 0.013f ), 45.f ); // if our angle is above 45 degree will we even hit them? shouldn't we just return???

						float s = 0.0f, c = 0.0f;
						math.sin_cos( ( fElevationAngle * PI / 180.0f ), &s, &c );

						const float fElevation = ( fRange * ( s / c ) );
						vPredictedPos.z += ( c > 0.0f ? fElevation : 0.0f );
						break;
					}
					default: break;
				}

				//utils.trace_hull(predictor.m_vPosition, vPredictedPos, vector(-3.8f, -3.8f, -3.8f), vector(3.8f, 3.8f, 3.8f),
				//	MASK_SOLID_BRUSHONLY, &traceFilter, &trace);

				//if (trace.DidHit())
				//{
				//	vPredictedPos.z = trace.vm_vEndPos.z;
				global_info.predicted_pos = vPredictedPos;
				global_info.prediction_lines.push_back( vPredictedPos );
				//}

				switch ( pWeapon->GetWeaponID( ) )
				{
					case TF_WEAPON_GRENADELAUNCHER:
					case TF_WEAPON_PIPEBOMBLAUNCHER:
					case TF_WEAPON_STICKBOMB:
					case TF_WEAPON_STICKY_BALL_LAUNCHER:
					{
						vector vecOffset( 16.0f, 8.0f, -6.0f );
						utils.projectile_setup( pLocal, pCmd->viewangles, vecOffset, &vLocalPos );
						break;
					}

					default: break;
				}

				if ( !CalcProjAngle( vLocalPos, vPredictedPos, projInfo, out ) )
				{
					break;
				}

				if ( TIME_TO_TICKS( out.m_flTime ) > ( n + 1 ) ) {
					continue;
				}

				out.m_flTime += fLatency;

				if ( out.m_flTime < TICKS_TO_TIME( n ) )
				{
					if ( WillProjectileHit( pLocal, pWeapon, pCmd, vPredictedPos, out, projInfo, predictor ) ) {

						global_info.predicted_pos = vPredictedPos;
						move_sim.restore( );
						return true;
					}
				}
			}
			move_sim.restore( );
		}
	}
	return false;
}

bool IsPointAllowed( int nHitbox ) {
	switch ( nHitbox ) {
		case 0: return vars.aimbot.projectile.projectile_head_allowed;
		case 1: return vars.aimbot.projectile.projectile_body_allowed;
		case 2: return vars.aimbot.projectile.projectile_feet_allowed;
		case 3:
		case 4:
		case 5:
		case 6: return vars.aimbot.projectile.projectile_head_allowed;
		case 7:
		case 8:
		case 9:
		case 10: return vars.aimbot.projectile.projectile_body_allowed;
		case 11:
		case 12:
		case 13:
		case 14: return vars.aimbot.projectile.projectile_feet_allowed;
		default: return true;
	}
	return true; // never
}

vector getHeadOffset( C_BaseEntity* pEntity ) {
	const vector headPos = pEntity->As<C_TFPlayer>( )->GetHitboxPosition( HITBOX_HEAD );
	const vector entPos = pEntity->GetAbsOrigin( );
	const vector delta = entPos - headPos;
	return delta * -1.f;
}

//	Tries to find the best position to aim at on our target.
vector __aimbot::GetAimPos( C_TFPlayer* pLocal, C_BaseEntity* pEntity, const vector& targetPredPos )
{
	vector retVec = pLocal->GetAbsOrigin( );
	vector localPos = pLocal->GetAbsOrigin( );

	const vector vLocalPos = pLocal->GetShootPosition( );

	const bool bIsDucking = ( pEntity->As<C_TFPlayer>( )->m_bDucked( ) || pEntity->As<C_TFPlayer>( )->IsDucking( ) );

	const float bboxScale = 0.9f; // stop shoot flor (:D)

	// this way overshoots players that are crouching and I don't know why.
	const vector vMaxs = i::game_movement->GetPlayerMaxs( bIsDucking ) * bboxScale;
	const vector vMins = vector( -vMaxs.x, -vMaxs.y, vMaxs.z - vMaxs.z ) * bboxScale;

	const vector headDelta = getHeadOffset( pEntity );

	const std::vector vecPoints = {									// oh you don't like 15 points because it fucks your fps??? TOO BAD!//
		vector( headDelta.x, headDelta.y, vMaxs.z ),					//	head bone probably
		vector( 0, 0, ( vMins.z + vMaxs.z ) / 2 ),					//	middles (scan first bc they are more accurate)
		vector( 0, 0, vMins.z ),										//	-
		vector( vMins.x, vMins.y, vMaxs.z ),							//	top four corners
		vector( vMins.x, vMaxs.y, vMaxs.z ),							//	-
		vector( vMaxs.x, vMaxs.y, vMaxs.z ),							//	-
		vector( vMaxs.x, vMins.y, vMaxs.z ),							//	-
		vector( vMins.x, vMins.y, ( vMins.z + vMaxs.z ) / 2 ),		//	middle four corners
		vector( vMins.x, vMaxs.y, ( vMins.z + vMaxs.z ) / 2 ),		//	-
		vector( vMaxs.x, vMaxs.y, ( vMins.z + vMaxs.z ) / 2 ),		//	-
		vector( vMaxs.x, vMins.y, ( vMins.z + vMaxs.z ) / 2 ),		//	-
		vector( vMins.x, vMins.y, vMins.z ),							//	bottom four corners
		vector( vMins.x, vMaxs.y, vMins.z ),							//	-
		vector( vMaxs.x, vMaxs.y, vMins.z ),							//	-
		vector( vMaxs.x, vMins.y, vMins.z )							//	-
	};

	std::vector<vector> visiblePoints{};
	const matrix3x4_t transform = {
		1.f, 0.f, 0.f, targetPredPos.x,
		0.f, 1.f, 0.f, targetPredPos.y,
		0.f, 0.f, 1.f, pEntity->As<C_TFPlayer>( )->m_vecVelocity( ).is_zero( ) ? pEntity->GetAbsOrigin( ).z : targetPredPos.z
	};

	aim_method = vars.aimbot.projectile.projectile_aim_position;
	int curPoint = 0, testPoints = 0; //maybe better way to do this
	for ( const auto& point : vecPoints )
	{
		if ( testPoints > 15 ) { break; }
		if ( static_cast< int >( visiblePoints.size( ) ) >= 7 ) { break; }
		if ( !IsPointAllowed( curPoint ) ) { curPoint++; continue; }

		vector vTransformed = {};
		math.vector_transform( point, transform, vTransformed );

		if ( utils.vis_pos_mask( pLocal, pEntity, vLocalPos, vTransformed, MASK_SHOT_HULL ) )
		{
			if ( curPoint == aim_method && aim_method < 3 ) { return vTransformed; }	// return this value now if it is going to get returned anyway, avoid useless scanning.
			visiblePoints.push_back( vTransformed );
			//i::debug_overlay->AddTextOverlay( vTransformed, 0.09f, "x" );
		}
		curPoint++;
		testPoints++;	// Only increment this if we actually tested.
	}
	if ( visiblePoints.empty( ) ) {
		return { 0, 0, 0 };
	}

	vector HeadPoint, TorsoPoint, FeetPoint;

	const int classNum = pLocal->m_iClass( );

	if ( GetAsyncKeyState( vars.misc.crits.crit_key ) & 0x8000 )
	{
		aim_method = 1;
	}

	if ( aim_method == 3 && classNum ) { // auto
		switch ( classNum ) {
			case CLASS_SOLDIER:
			{
				if ( vars.aimbot.projectile.projectile_feet_aim_on_ground && pEntity->As<C_TFPlayer>( )->IsOnGround( ) )
				{
					aim_method = 2;
					break;
				}
				else
				{
					aim_method = 1;
					break;
				}
			}
			case CLASS_DEMOMAN:
			{
				aim_method = 2;
				break;
			}
			case CLASS_SNIPER:
			{
				aim_method = 0;
				break;
			}
			default:
			{
				aim_method = 1;
				break;
			}
		}
	}

	switch ( aim_method ) {
		case 0: {	//head
			math.vector_transform( vecPoints.at( 0 ), transform, HeadPoint );			//	get transformed location of our "best point" for our selected prio hitbox
			for ( const auto& aimPoint : visiblePoints ) {							//	iterate through visible points
				if ( aimPoint.dist_to( HeadPoint ) < retVec.dist_to( HeadPoint ) ) {		//	if the distance to our best point is lower than the previous selected point,
					retVec = aimPoint;												//	set the new point to our currently selected point
				}
			}
			break;
		}
		case 1: {	//torso
			math.vector_transform( vecPoints.at( 1 ), transform, TorsoPoint );
			for ( const auto& aimPoint : visiblePoints ) {
				if ( aimPoint.dist_to( TorsoPoint ) < retVec.dist_to( TorsoPoint ) ) {
					retVec = aimPoint;
				}
			}
			break;
		}
		case 2: {	//feet
			math.vector_transform( vecPoints.at( 2 ), transform, FeetPoint );
			for ( const auto& aimPoint : visiblePoints ) {
				if ( aimPoint.dist_to( FeetPoint ) < retVec.dist_to( FeetPoint ) ) {
					retVec = aimPoint;
				}
			}
			break;
		}
	}
	return retVec;
}

vector __aimbot::GetAimPosBuilding( C_TFPlayer* pLocal, C_BaseEntity* pEntity ) {
	const vector vLocalPos = pLocal->GetShootPosition( );

	const float bboxScale = std::max( 1.0f - 0.05f, 0.5f );	// set the maximum scale for buildings at .95f

	const vector vMins = pEntity->m_vecMins( ) * bboxScale;
	const vector vMaxs = pEntity->m_vecMaxs( ) * bboxScale;

	const std::vector vecPoints = {
		vector( vMaxs.x / 2, vMaxs.y / 2, vMaxs.z / 2 ),								//	middle
		vector( vMins.x, vMins.y, vMaxs.z ),											//	top four corners
		vector( vMins.x, vMaxs.y, vMaxs.z ),											//	-
		vector( vMaxs.x, vMaxs.y, vMaxs.z ),											//	-
		vector( vMaxs.x, vMins.y, vMaxs.z ),											//	-
		vector( vMins.x, vMins.y, vMins.z ),											//	bottom four corners
		vector( vMins.x, vMaxs.y, vMins.z ),											//	-
		vector( vMaxs.x, vMaxs.y, vMins.z ),											//	-
		vector( vMaxs.x, vMins.y, vMins.z )											//	-
	};

	const matrix3x4_t& transform = pEntity->RenderableToWorldTransform( );

	for ( const auto& point : vecPoints )
	{
		vector vTransformed = {};
		math.vector_transform( point, transform, vTransformed );

		if ( utils.vis_pos( pLocal, pEntity, vLocalPos, vTransformed ) )
		{
			return vTransformed; // just return the first point we see
		}
	}

	return { 0, 0, 0 };
}

bool __aimbot::WillProjectileHit( C_TFPlayer* pLocal, C_TFWeaponBase* pWeapon, CUserCmd* pCmd, const vector& vPredictedPos, Solution_t& out, const ProjectileInfo_t& projInfo,
										   const Predictor_t& predictor )
{
	vector hullSize = { 3.8f, 3.8f, 3.8f };
	vector vVisCheck = pLocal->GetShootPosition( );
	const vector predictedViewAngles = { -RAD2DEG( out.m_flPitch ), RAD2DEG( out.m_flYaw ), 0.0f };
	CGameTrace trace = {};
	static CTraceFilterWorldAndPropsOnly traceFilter = {};
	traceFilter.pSkip = predictor.m_pEntity;

	// this shit's messy
	{
		switch ( pWeapon->GetWeaponID( ) )
		{
			case TF_WEAPON_PARTICLE_CANNON: {
				hullSize = { 1.f, 1.f, 1.f };
				vector vecOffset( 23.5f, 12.0f, -3.0f ); //tf_weaponbase_gun.cpp @L529 & @L760
				if ( pLocal->IsDucking( ) )
				{
					vecOffset.z = 8.0f;
				}
				utils.projectile_setup( pLocal, predictedViewAngles, vecOffset, &vVisCheck );
				break;
			}
			case TF_WEAPON_CROSSBOW: {
				hullSize = { 3.f, 3.f, 3.f };
				const vector vecOffset( 23.5f, 12.0f, -3.0f );
				utils.projectile_setup( pLocal, predictedViewAngles, vecOffset, &vVisCheck );
				break;
			}
			case TF_WEAPON_FLAREGUN_REVENGE:
			case TF_WEAPON_ROCKETLAUNCHER:
			case TF_WEAPON_ROCKETLAUNCHER_DIRECTHIT:
			{
				hullSize = { 0.f, 3.7f, 3.7f };

				vector vecOffset( 23.5f, 12.0f, -3.0f ); //tf_weaponbase_gun.cpp @L529 & @L760
				if ( pWeapon->m_iItemDefinitionIndex( ) == Soldier_m_TheOriginal )
				{
					vecOffset.y = 0.f;
				}
				if ( pLocal->IsDucking( ) )
				{
					vecOffset.z = 8.0f;
				}
				utils.projectile_setup( pLocal, predictedViewAngles, vecOffset, &vVisCheck );
				break;
			}
			case TF_WEAPON_SYRINGEGUN_MEDIC:
			{
				hullSize = { 0.f, 1.f, 1.f };

				const vector vecOffset( 16.f, 6.f, -8.f ); //tf_weaponbase_gun.cpp @L628
				utils.projectile_setup( pLocal, predictedViewAngles, vecOffset, &vVisCheck );
				break;
			}
			case TF_WEAPON_COMPOUND_BOW:
			{
				hullSize = { 1.f, 1.f, 1.f };

				const vector vecOffset( 23.5f, 12.0f, -3.0f ); //tf_weapon_grapplinghook.cpp @L355 ??
				utils.projectile_setup( pLocal, predictedViewAngles, vecOffset, &vVisCheck );
				break;
			}
			case TF_WEAPON_RAYGUN:
			case TF_WEAPON_DRG_POMSON:
			{
				hullSize = { 0.1f, 0.1f, 0.1f };
				vector vecOffset( 23.5f, -8.0f, -3.0f ); //tf_weaponbase_gun.cpp @L568
				if ( pLocal->IsDucking( ) )
				{
					vecOffset.z = 8.0f;
				}
				utils.projectile_setup( pLocal, predictedViewAngles, vecOffset, &vVisCheck );
				break;
			}
			case TF_WEAPON_GRENADELAUNCHER:
			case TF_WEAPON_PIPEBOMBLAUNCHER:
			case TF_WEAPON_STICKBOMB:
			case TF_WEAPON_STICKY_BALL_LAUNCHER:
			{

				hullSize = { 2.f, 2.f, 2.f };

				auto vecAngle = vector( ), vecForward = vector( ), vecRight = vector( ), vecUp = vector( );
				math.angle_vectors( { -RAD2DEG( out.m_flPitch ), RAD2DEG( out.m_flYaw ), 0.0f }, &vecForward, &vecRight, &vecUp );
				const vector vecVelocity = ( ( vecForward * projInfo.m_flVelocity ) - ( vecUp * 200.0f ) );
				math.vector_angles( vecVelocity, vecAngle );
				out.m_flPitch = -DEG2RAD( vecAngle.x );
				
				// see relevant code @tf_weaponbase_gun.cpp L684
				const float fRight = convars.cl_flipviewmodels->GetInt( ) ? -8.f : 8.f;
				vVisCheck = pLocal->GetShootPosition( ) + vecForward * 16.0f + vecRight * fRight + vecUp * -6.0f;

				break;
			}
			default: break;
		}
	}

	if ( projInfo.m_flGravity > 0.0f )
	{
		projectile_simulation::run_simulation( pLocal, pWeapon, predictedViewAngles );
		if ( global_info.projectile_lines.empty( ) )
		{
			return false;
		}

		for ( size_t i = 1; i < global_info.projectile_lines.size( ); i++ )
		{
			utils.trace_hull( global_info.projectile_lines.at( i - 1 ), global_info.projectile_lines.at( i ), hullSize * 1.01f, hullSize * -1.01f, MASK_SHOT_HULL, &traceFilter, &trace );
		}
	}
	else
	{
		utils.trace_hull( vVisCheck, vPredictedPos, hullSize * 1.01f, hullSize * -1.01f, MASK_SHOT_HULL, &traceFilter, &trace );
	}

	return !trace.DidHit( ) && !trace.m_bStartSolid;
}

bool __aimbot::GetTargets( C_TFPlayer* pLocal, C_TFWeaponBase* pWeapon )
{
	m_vecTargets.clear( );

	const vector vLocalPos = pLocal->GetShootPosition( );
	const vector vLocalAngles = i::engine_client->GetViewAngles( );

	// Players
	if ( vars.aimbot.global.aimbot_aimplayers )
	{
		const bool bIsCrossbow = pWeapon->GetWeaponID( ) == TF_WEAPON_CROSSBOW;

		for ( const auto& pEntity : entity_cache.players[ bIsCrossbow ? TEAMSALL : ENEMY ] )
		{
			if ( pEntity->GetClassID( ) != ETFClassID::CTFPlayer )
			{
				continue;
			}

			const auto& player = pEntity->As<C_TFPlayer>( );

			if ( !player || player->deadflag( ) || player->IsDormant( ) || player->InCond( TF_COND_HALLOWEEN_GHOST_MODE ) || player == pLocal )
				continue;

			if ( vars.aimbot.global.aimbot_ignore_invincible && ( player->InCond( TF_COND_INVULNERABLE ) || player->InCond( TF_COND_PHASE ) ) )
				continue;

			if ( vars.aimbot.global.aimbot_ignore_cloaked && player->InCond( TF_COND_STEALTHED ) )
				continue;

			if ( vars.aimbot.global.aimbot_ignore_taunting && player->InCond( TF_COND_TAUNTING ) )
				continue;

			if ( vars.aimbot.global.aimbot_ignore_friends && player->PlayerOnFriendsList( ) && player->m_iTeamNum( ) != pLocal->m_iTeamNum( ) )
				continue;

			/*player_info_t pi;
			if ( i::engine_client->GetPlayerInfo( pPlayer->entindex( ), &pi ) )
			{
				if ( global_info.PlayerPriority[ pi.friendsID ].Mode == 1 )
				{
					continue;
				}
			}*/

			vector vPos = player->GetWorldSpaceCenter( );
			vector vAngleTo = math.calc_angle( vLocalPos, vPos );
			float flFOVTo = math.calc_fov( vLocalAngles, vAngleTo );

			if ( flFOVTo > vars.aimbot.projectile.projectile_aim_fov )
				continue;

			m_vecTargets.push_back( { player, ETargetType::PLAYER, vPos, vAngleTo, flFOVTo } );
		}
	}

	// Buildings
	if ( vars.aimbot.global.aimbot_aimbuildings )
	{
		const bool bIsRescueRanger = pWeapon->GetWeaponID( ) == TF_WEAPON_SHOTGUN_BUILDING_RESCUE;

		if ( bIsRescueRanger )
		{
			for ( const auto& object : entity_cache.buildings[ TEAMSALL ] )
			{
				if ( !object || object->IsDormant( ) )
					continue;

				vector vPos = object->GetWorldSpaceCenter( );
				vector vAngleTo = math.calc_angle( vLocalPos, vPos );
				float flFOVTo = math.calc_fov( vLocalAngles, vAngleTo );

				if ( flFOVTo > vars.aimbot.projectile.projectile_aim_fov )
					continue;

				m_vecTargets.push_back( { object, ETargetType::BUILDING, vPos, vAngleTo, flFOVTo } );
			}
		}
		else
		{
			for ( const auto& sentry : entity_cache.sentries[ ENEMY ] )
			{
				if ( !sentry || sentry->IsDormant( ) )
					continue;

				vector vPos = sentry->GetWorldSpaceCenter( );
				vector vAngleTo = math.calc_angle( vLocalPos, vPos );
				float flFOVTo = math.calc_fov( vLocalAngles, vAngleTo );

				if ( flFOVTo > vars.aimbot.projectile.projectile_aim_fov )
					continue;

				m_vecTargets.push_back( { sentry, ETargetType::BUILDING, vPos, vAngleTo, flFOVTo } );
			}
		}
	}

	/*if ( !Vars::Aimbot::Global::IgnoreNPCs.var )
	{
		for ( const auto &pEntity : g_EntityCache.GetGroup( EEntGroup::WORLD_NPC ) )
		{
			const auto &pNpc = pEntity->As<C_BaseEntity>( );

			if ( !pNpc || pNpc->IsDormant( ) )
				continue;

			vector vPos = pNpc->GetWorldSpaceCenter( );
			vector vAngleTo = math.calc_angle( vLocalPos, vPos );
			float flFOVTo = SortMethod == ESortMethod::FOV ? Utils::CalcFov( vLocalAngles, vAngleTo ) : 0.0f;

			if ( SortMethod == ESortMethod::FOV && flFOVTo > Vars::Aimbot::Hitscan::AimFOV.var )
				continue;

			float fldist_to = SortMethod == ESortMethod::DISTANCE ? vLocalPos.dist_to( vPos ) : 0.0f;

			g_AimbotGlobal.m_vecTargets.push_back( { pNpc, ETargetType::BUILDING, vPos, vAngleTo, flFOVTo, fldist_to } );
		}
	}*/

	return !m_vecTargets.empty( );
}

bool __aimbot::VerifyTarget( C_TFPlayer* local, C_TFWeaponBase* weapon, CUserCmd* pCmd, Target_t& target )
{
	ProjectileInfo_t projInfo = {};
	if ( !GetProjectileInfo( weapon, projInfo ) )
	{
		return false;
	}

	if ( local->InCond( TF_COND_RUNE_PRECISION ) && m_bIsBoosted ) {
		projInfo.m_flVelocity *= 2.5f;

	}

	auto vVelocity = vector( );
	auto vAcceleration = vector( );

	switch ( target.m_TargetType )
	{
		case ETargetType::PLAYER:
		{
			vVelocity = target.m_pEntity->EstimateAbsVelocity( );
			vAcceleration = vector( 0.0f, 0.0f, convars.sv_gravity->GetFloat( ) * ( ( target.m_pEntity->As<C_TFPlayer>( )->InCond( TF_COND_PARACHUTE_ACTIVE ) ) ? 0.224f : 1.0f ) );
			float distance = sqrt( powf( local->GetAbsOrigin( ).x - target.m_pEntity->GetAbsOrigin( ).x, 2 ) + powf( local->GetAbsOrigin( ).y - target.m_pEntity->GetAbsOrigin( ).y, 2 ) + powf( local->GetAbsOrigin( ).z - target.m_pEntity->GetAbsOrigin( ).z, 2 ) );
			int max_dist = !aim_method ? 1000 : ( aim_method == 1 ? 950 : 850 );
			switch ( weapon->GetWeaponID( ) )
			{
				case TF_WEAPON_GRENADELAUNCHER:
				{
					if ( distance > max_dist )
					{
						return false;
					}
				}
			}
			break;
		}

		default: break;
	}

	Predictor_t predictor = {
		target.m_pEntity,
		target.m_vPos,
		vVelocity,
		vAcceleration
	};

	Solution_t solution = {};

	if ( !SolveProjectile( local, weapon, pCmd, predictor, projInfo, solution ) )
	{
		return false;
	}

	target.m_vAngleTo = { -RAD2DEG( solution.m_flPitch ), RAD2DEG( solution.m_flYaw ), 0.0f };

	return true;
}

// Returns the best target
bool __aimbot::GetTarget( C_TFPlayer* pLocal, C_TFWeaponBase* pWeapon, CUserCmd* pCmd, Target_t& outTarget )
{
	if ( !GetTargets( pLocal, pWeapon ) )
	{
		return false;
	}

	sort_targets( );

	//instead of this just limit to like 4-6 targets, should save perf without any noticeable changes in functionality
	for ( auto& target : m_vecTargets )
	{
		if ( !VerifyTarget( pLocal, pWeapon, pCmd, target ) )
		{
			continue;
		}

		outTarget = target;
		return true;
	}

	return false;
}

// Aims at the given angles
void __aimbot::Aim( CUserCmd* pCmd, C_TFWeaponBase* pWeapon, vector& vAngle )
{
	//vAngle -= global_info.m_vPunchAngles;
	utils.clamp_angles( vAngle );

	/*if ( Vars::Misc::StacBypass.var )
	{
		global_info.m_bNullCmd = true;
	}*/

	switch ( vars.aimbot.projectile.projectile_aim_method )
	{
		case 0:
		{
			// Plain
			pCmd->viewangles = vAngle;
			i::engine_client->SetViewAngles( pCmd->viewangles );
			break;
		}

		case 1:
		{
			// Silent
			utils.fix_movement( pCmd, vAngle );
			pCmd->viewangles = vAngle;
			break;
		}

		default: break;
	}

	/*if ( Vars::Misc::StacBypass.var )
	{
		global_info.m_bNullCmd = false;
	}*/
}

bool __aimbot::ShouldFire( CUserCmd* pCmd, Target_t target )
{
	//return ( Vars::Aimbot::Global::AutoShoot.var && global_info.m_bWeaponCanAttack );
	if ( !vars.aimbot.global.aimbot_autoshoot )
		return false;

	return true;
}

bool __aimbot::IsAttacking( const CUserCmd* pCmd, C_TFWeaponBase* pWeapon )
{
	if ( pWeapon->m_iItemDefinitionIndex( ) == Soldier_m_TheBeggarsBazooka )
	{
		static bool bLoading = false;

		if ( pWeapon->m_iClip1( ) > 0 )
		{
			bLoading = true;
		}

		if ( !( pCmd->buttons & IN_ATTACK ) && bLoading )
		{
			bLoading = false;
			return true;
		}
	}
	else
	{
		if ( pWeapon->GetWeaponID( ) == TF_WEAPON_COMPOUND_BOW || pWeapon->GetWeaponID( ) == TF_WEAPON_PIPEBOMBLAUNCHER )
		{
			static bool bCharging = false;

			if ( pWeapon->As<CTFPipebombLauncher>( )->m_flChargeBeginTime( ) > 0.0f )
			{
				bCharging = true;
			}

			if ( !( pCmd->buttons & IN_ATTACK ) && bCharging )
			{
				bCharging = false;
				return true;
			}
		}
		else if ( pWeapon->GetWeaponID( ) == TF_WEAPON_CANNON )
		{
			static bool Charging = false;

			if ( pWeapon->As<CTFGrenadeLauncher>( )->m_flDetonateTime( ) > 0.0f )
				Charging = true;

			if ( !( pCmd->buttons & IN_ATTACK ) && Charging )
			{
				Charging = false;
				return true;
			}
		}

		//pssst..
		//Dragon's Fury has a gauge (seen on the weapon model) maybe it would help for pSilent hmm..
		//
		//if (pWeapon->GetWeaponID() == 109) {
		//}

		else
		{
			if ( ( pCmd->buttons & IN_ATTACK ) && global_info.can_attack )
			{
				return true;
			}
		}
	}

	return false;
}

void __aimbot::aimbot_psilent( CUserCmd* cmd, const vector& oldview, const float side, const float forward, bool* send_packet )
{
	if ( vars.aimbot.projectile.projectile_aim_method != 1 )
	{
		return;
	}

	static bool wasset = false;

	if ( global_info.silent_time )
	{
		*send_packet = false;
		wasset = true;
	}
	else
	{
		if ( wasset )
		{
			*send_packet = true;
			cmd->viewangles = oldview;
			cmd->sidemove = side;
			cmd->forwardmove = forward;
			wasset = false;
		}
	}
}

void __aimbot::projectile_run( C_TFPlayer* pLocal, C_TFWeaponBase* pWeapon, CUserCmd* pCmd, bool* send_packet )
{
	static int nLastTracerTick = pCmd->tick_count;

	m_bIsFlameThrower = false;

	if ( !vars.aimbot.global.global_enabled || !utils.is_window_in_focus( ) )
	{
		return;
	}

	const bool bShouldAim = ( vars.aimbot.global.aimbot_key == VK_LBUTTON
							  ? ( pCmd->buttons & IN_ATTACK )
							  : is_key_held( ) );
	if ( !bShouldAim ) { return; }

	Target_t target{ };
	if ( GetTarget( pLocal, pWeapon, pCmd, target ) && bShouldAim )
	{
		// Aim at the current target
		//global_info.m_nCurrentTargetIdx = target.m_pEntity->entindex( );

		if ( vars.aimbot.projectile.projectile_aim_method == 1 )
		{
			global_info.aim_pos = global_info.predicted_pos;
		}

		if ( ShouldFire( pCmd, target ) )
		{
			/*if ( Vars::Misc::StacBypass.var )
			{
				global_info.m_bNullCmd = true;
			}*/
			pCmd->buttons |= IN_ATTACK;
			global_info.silent_time = true;
			/*if ( vars.aimbot.projectile.projectile_aim_method == 1 )
			{
				static int tickcount = pCmd->tick_count;
				*send_packet = false;
				global_info.projectile_viewangles = pCmd->viewangles;

				if ( pCmd->tick_count == tickcount + 1 )
				{
					*send_packet = true;
					pCmd->viewangles = global_info.projectile_viewangles;
				}
			}*/
			/*if ( Vars::Misc::StacBypass.var )
			{
				global_info.m_bNullCmd = false;
			}*/

			/*if ( Vars::Misc::CL_Move::Enabled.var && Vars::Misc::CL_Move::Doubletap.var && ( pCmd->buttons & IN_ATTACK ) && !global_info.m_nShifted && !global_info.m_nWaitForShift && pLocal->IsOnGround( ) )
			{
				global_info.m_bShouldShift = true;
			}

			if ( global_info.m_bAAActive && !global_info.m_bWeaponCanAttack )
				pCmd->buttons &= ~IN_ATTACK;*/

			if ( pWeapon->m_iItemDefinitionIndex( ) == Soldier_m_TheBeggarsBazooka )
			{
				if ( pWeapon->m_iClip1( ) > 0 )
					pCmd->buttons &= ~IN_ATTACK;
			}
			else
			{
				if ( ( pWeapon->GetWeaponID( ) == TF_WEAPON_COMPOUND_BOW || pWeapon->GetWeaponID( ) == TF_WEAPON_PIPEBOMBLAUNCHER )
					 && pWeapon->As<CTFPipebombLauncher>( )->m_flChargeBeginTime( ) > 0.0f )
				{
					pCmd->buttons &= ~IN_ATTACK;
				}
				else if ( pWeapon->GetWeaponID( ) == TF_WEAPON_CANNON && pWeapon->As<CTFGrenadeLauncher>( )->m_flDetonateTime( ) > 0.0f )
				{
					const vector vEyePos = pLocal->GetShootPosition( );
					float BestCharge = vEyePos.dist_to( global_info.predicted_pos ) / 1453.9f;

					if ( vars.aimbot.projectile.projectile_charge_loose_cannon )
					{
						if ( pWeapon->As<CTFGrenadeLauncher>( )->m_flDetonateTime( ) - i::global_vars->curtime <= BestCharge )
							pCmd->buttons &= ~IN_ATTACK;
					}
					else
						pCmd->buttons &= ~IN_ATTACK;
				}
			}
		}

		const bool bIsAttacking = IsAttacking( pCmd, pWeapon );

		if ( bIsAttacking )
		{
			global_info.attacking = true;
			if ( abs( pCmd->tick_count - nLastTracerTick ) > 1 )
			{
				//ProjectileTracer( pLocal, target );
				nLastTracerTick = pCmd->tick_count;
			}
			global_info.pred_lines_backup.clear( );
			global_info.pred_lines_backup = global_info.prediction_lines;

			//I::DebugOverlay->AddLineOverlayAlpha( target.m_vPos, global_info.PredictedPos, 0, 255, 0, 255, true, 2 ); // Predicted aim pos
		}

		if ( vars.aimbot.projectile.projectile_aim_method == 1 )
		{
			if ( m_bIsFlameThrower && ( target.m_vAngleTo.x && target.m_vAngleTo.y && target.m_vAngleTo.z ) )
			{
				Aim( pCmd, pWeapon, target.m_vAngleTo );
				global_info.projectile_silent = true;
			}

			else
			{
				if ( bIsAttacking )
				{
					Aim( pCmd, pWeapon, target.m_vAngleTo );
				}
			}
		}
		else
		{
			Aim( pCmd, pWeapon, target.m_vAngleTo );
		}
	}
}

void __aimbot::detonator_run( C_TFPlayer* pLocal, C_TFWeaponBase* pWeapon, CUserCmd* pCmd )
{
	bool pressed = ( GetAsyncKeyState( vars.aimbot.global.aimbot_key ) & 0x8000 );
	if ( !vars.aimbot.projectile.projectile_auto_detonate || !pressed ) { return; }

	float flStickyRadius = G::AttribHookValue( 148.0f, "mult_explosion_radius", pWeapon );
	for ( const auto& pExplosive : entity_cache.stickies )
	{
		float flRadiusMod = 1.0f;
		if ( pExplosive->m_nPipebombType( ) == TYPE_STICKY )
		{
			if ( !pExplosive->As<C_TFGrenadePipebombProjectile>( )->m_bTouched( ) )
			{
				float flArmTime = i::cvar->FindVar( "tf_grenadelauncher_livetime" )->GetFloat( ); //0.8
				float RampTime = i::cvar->FindVar( "tf_sticky_radius_ramp_time" )->GetFloat( ); //2.0
				float AirDetRadius = i::cvar->FindVar( "tf_sticky_airdet_radius" )->GetFloat( ); //0.85
				flRadiusMod *= math.remap_val_clamped( i::global_vars->curtime - pExplosive->m_flCreationTime( ), flArmTime, flArmTime + RampTime, AirDetRadius, 1.0f );
			}
			flStickyRadius *= flRadiusMod;
		}
	}

	bool shouldDetonate = false;

	// Check sticky detonation
	if ( vars.aimbot.projectile.projectile_auto_detonate_stickies )
	{
		for ( const auto& explosive : entity_cache.stickies )
		{
			if ( !explosive )
			{
				continue;
			}

			if ( vars.aimbot.projectile.projectile_detonate_players )
			{
				for ( const auto& target : entity_cache.players[ ENEMY ] )
				{
					if ( !target || target->deadflag( ) )
					{
						continue;
					}

					if ( explosive->m_vecOrigin( ).dist_to( target->m_vecOrigin( ) ) > flStickyRadius * 0.9f )
					{
						continue;
					}

					if ( utils.vis_pos( explosive, target, explosive->m_vecOrigin( ), target->m_vecOrigin( ) ) )
					{
						pCmd->buttons |= IN_ATTACK2;
					}
				}
			}

			if ( vars.aimbot.projectile.projectile_detonate_sentries )
			{
				for ( const auto& target : entity_cache.sentries[ ENEMY ] )
				{
					if ( !target )
					{
						continue;
					}

					if ( explosive->m_vecOrigin( ).dist_to( target->m_vecOrigin( ) ) > flStickyRadius * 0.9f )
					{
						continue;
					}

					if ( utils.vis_pos( explosive, target, explosive->m_vecOrigin( ), target->m_vecOrigin( ) ) )
					{
						pCmd->buttons |= IN_ATTACK2;
					}
				}
			}

			if ( vars.aimbot.projectile.projectile_detonate_bombs )
			{
				for ( const auto& target : entity_cache.world[ BOMBS ] )
				{
					if ( !target )
					{
						continue;
					}

					if ( explosive->m_vecOrigin( ).dist_to( target->m_vecOrigin( ) ) > flStickyRadius * 0.9f )
					{
						continue;
					}

					if ( utils.vis_pos( explosive, target, explosive->m_vecOrigin( ), target->m_vecOrigin( ) ) )
					{
						pCmd->buttons |= IN_ATTACK2;
					}
				}
			}
		}
	}

	// Check flare detonation
	if ( vars.aimbot.projectile.projectile_auto_detonate_flares )
	{
		for ( const auto& explosive : entity_cache.flares )
		{
			if ( !explosive )
			{
				continue;
			}

			if ( vars.aimbot.projectile.projectile_detonate_players )
			{
				for ( const auto& target : entity_cache.players[ ENEMY ] )
				{
					if ( !target || target->deadflag( ) )
					{
						continue;
					}

					if ( explosive->m_vecOrigin( ).dist_to( target->m_vecOrigin( ) ) > 110.0f * 0.9f )
					{
						continue;
					}

					if ( utils.vis_pos( explosive, target, explosive->m_vecOrigin( ), target->m_vecOrigin( ) ) )
					{
						pCmd->buttons |= IN_ATTACK2;
					}
				}
			}

			if ( vars.aimbot.projectile.projectile_detonate_sentries )
			{
				for ( const auto& target : entity_cache.sentries[ ENEMY ] )
				{
					if ( !target )
					{
						continue;
					}

					if ( explosive->m_vecOrigin( ).dist_to( target->m_vecOrigin( ) ) > 110.0f * 0.9f )
					{
						continue;
					}

					if ( utils.vis_pos( explosive, target, explosive->m_vecOrigin( ), target->m_vecOrigin( ) ) )
					{
						pCmd->buttons |= IN_ATTACK2;
					}
				}
			}

			if ( vars.aimbot.projectile.projectile_detonate_bombs )
			{
				for ( const auto& target : entity_cache.world[ BOMBS ] )
				{
					if ( !target )
					{
						continue;
					}

					if ( explosive->m_vecOrigin( ).dist_to( target->m_vecOrigin( ) ) > 110.0f * 0.9f )
					{
						continue;
					}

					if ( utils.vis_pos( explosive, target, explosive->m_vecOrigin( ), target->m_vecOrigin( ) ) )
					{
						pCmd->buttons |= IN_ATTACK2;
					}
				}
			}
		}
	}
}