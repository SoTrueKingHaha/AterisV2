#include "entitycache.h"
#include "../../sdk/globals/globalinfo.h"
//#include "../menu/vars.h"

void __entity_cache::fill( )
{
	if ( !local )
	{
		local = GET_LOCAL_PLAYER( );
	}

	weapon = local->GetActiveWeapon( )->As<C_TFWeaponBase>( );

	const int localteam = local->m_iTeamNum( );

	int text_offset = 0;
	for ( int i = 1; i < i::entity_list->GetHighestEntityIndex( ); i++ )
	{
		if ( const auto &entity = GET_ENT_I( i )->As<C_BaseEntity>( ) )
		{
			if ( !entity )
			{
				continue;
			}

			auto classid = entity->GetClassID( );

			if ( classid == ETFClassID::CTFPlayer )
			{
				if ( entity->IsDormant( ) && !global_info.dormant_players.count( entity->entindex( ) ) )
				{
					continue;
				}
				
				if ( entity->IsDormant( ) )
				{
					if ( i::engine_client->Time( ) - global_info.dormant_players[ entity->entindex( ) ].last_update <= 5.0f )
					{
						if ( !global_info.dormant_players[ entity->entindex( ) ].location.is_zero( ) )
						{
							entity->As<C_TFPlayer>( )->SetAbsOrigin( global_info.dormant_players[ entity->entindex( ) ].location );
							entity->As<C_TFPlayer>( )->m_vecOrigin( ) = global_info.dormant_players[ entity->entindex( ) ].location;
						}
					}
				}
			}
			else
			{
				if ( entity->IsDormant( ) )
				{
					continue;
				}
			}

			//i::debug_overlay->AddEntityTextOverlay( entity->entindex( ), 0, i::global_vars->absoluteframetime * 4, 255, 255, 255, 255, "%i", ( int )classid );
			
			switch ( classid )
			{
				case ETFClassID::CTFPlayer:
				{
					const auto &player = entity->As<C_TFPlayer>( );
					if ( player == local )
					{
						break;
					}
					if ( player->IsInValidTeam( ) )
					{
						players[ TEAMSALL ].push_back( player );
						players[ localteam == player->m_iTeamNum( ) ? TEAM : ENEMY ].push_back( player );
					}
					break;
				}

				case ETFClassID::CObjectSentrygun:
				{
					const auto &object = entity->As<C_BaseObject>( );
					buildings[ TEAMSALL ].push_back( object );
					buildings[ localteam == object->m_iTeamNum( ) ? TEAM : ENEMY ].push_back( object );

					const auto &sentry = entity->As<C_ObjectSentrygun>( );
					sentries[ TEAMSALL ].push_back( sentry );
					sentries[ localteam == sentry->m_iTeamNum( ) ? TEAM : ENEMY ].push_back( sentry );
					break;
				}

				case ETFClassID::CObjectDispenser:
				{
					const auto &object = entity->As<C_BaseObject>( );
					buildings[ TEAMSALL ].push_back( object );
					buildings[ localteam == object->m_iTeamNum( ) ? TEAM : ENEMY ].push_back( object );

					const auto &dispenser = entity->As<C_ObjectDispenser>( );
					dispensers[ TEAMSALL ].push_back( dispenser );
					dispensers[ localteam == dispenser->m_iTeamNum( ) ? TEAM : ENEMY ].push_back( dispenser );
					break;
				}

				case ETFClassID::CObjectTeleporter:
				{
					const auto &object = entity->As<C_BaseObject>( );
					buildings[ TEAMSALL ].push_back( object );
					buildings[ localteam == object->m_iTeamNum( ) ? TEAM : ENEMY ].push_back( object );

					const auto &teleporter = entity->As<C_ObjectTeleporter>( );
					teleporters[ TEAMSALL ].push_back( teleporter );
					teleporters[ localteam == teleporter->m_iTeamNum( ) ? TEAM : ENEMY ].push_back( teleporter );
					break;
				}

				case ETFClassID::CBaseAnimating:
				{
					const auto &name = FNV1A::Hash( i::model_info->GetModelName( entity->GetModel( ) ) );

					if ( is_ammo( name ) )
					{
						world[ AMMO ].push_back( entity );
						world[ WORLDALL ].push_back( entity );
					}
					else if ( is_health( name ) )
					{
						world[ HEALTH ].push_back( entity );
						world[ WORLDALL ].push_back( entity );
					}
					break;
				}
				case ETFClassID::CTFAmmoPack:
				{
					world[ AMMO ].push_back( entity );
					world[ WORLDALL ].push_back( entity );
					break;
				}
				case ETFClassID::CCaptureFlag:
				{
					intel[ TEAMSALL ].push_back( entity );
					intel[ localteam == entity->m_iTeamNum( ) ? TEAM : ENEMY ].push_back( entity );
					world[ WORLDALL ].push_back( entity );
					break;
				}
				case ETFClassID::CTFPumpkinBomb:
				case ETFClassID::CTFGenericBomb:
				{
					world[ BOMBS ].push_back( entity );
					world[ WORLDALL ].push_back( entity );
					break;
				}
				case ETFClassID::CHeadlessHatman:
				case ETFClassID::CTFTankBoss:
				case ETFClassID::CMerasmus:
				case ETFClassID::CZombie:
				case ETFClassID::CEyeballBoss:
				{
					npc_entity[ TEAMSALL ].push_back( entity );
					if ( entity->IsInValidTeam( ) )
					{
						npc_entity[ localteam == entity->m_iTeamNum( ) ? TEAM : ENEMY ].push_back( entity );
					}
					break;
				}
				case ETFClassID::CTFProjectile_Rocket:
				case ETFClassID::CTFGrenadePipebombProjectile:
				case ETFClassID::CTFProjectile_Jar:
				case ETFClassID::CTFProjectile_JarGas:
				case ETFClassID::CTFProjectile_JarMilk:
				case ETFClassID::CTFProjectile_Arrow:
				case ETFClassID::CTFProjectile_SentryRocket:
				case ETFClassID::CTFProjectile_Flare:
				case ETFClassID::CTFProjectile_Cleaver:
				case ETFClassID::CTFProjectile_HealingBolt:
				case ETFClassID::CTFProjectile_EnergyRing:
				case ETFClassID::CTFProjectile_EnergyBall:
				case ETFClassID::CTFProjectile_ThrowableBreadMonster:
				case ETFClassID::CTFStunBall:
				case ETFClassID::CTFBall_Ornament:
				{
					world[ WORLDALL ].push_back( entity );
					projectiles[ TEAMSALL ].push_back( entity );
					projectiles[ localteam == entity->m_iTeamNum( ) ? TEAM : ENEMY ].push_back( entity );

					const auto& thrower = i::entity_list->GetClientEntityFromHandle( entity->As<C_TFGrenadePipebombProjectile>( )->m_hThrower( ) )->As<C_TFPlayer>( );
					if ( classid == ETFClassID::CTFGrenadePipebombProjectile && thrower && thrower == local && 
						 ( entity->m_nPipebombType( ) == TYPE_STICKY || thrower->GetWeaponFromSlot( SLOT_SECONDARY )->As<C_TFWeaponBase>( )->m_iItemDefinitionIndex( ) == Demoman_s_StickyJumper ) )
					{
						stickies.push_back( entity );
					}

					if ( classid == ETFClassID::CTFProjectile_Flare && local->GetWeaponFromSlot( SLOT_SECONDARY ) && local->GetWeaponFromSlot( SLOT_SECONDARY )->As<C_TFWeaponBase>( )->m_iItemDefinitionIndex( ) == Pyro_s_TheDetonator
						 && entity->m_hOwnerEntity( ).Get( ) == local )
					{
						flares.push_back( entity );
					}
					break;
				}
				case ETFClassID::CTFPlayerResource:
				{
					player_resource = reinterpret_cast< C_TFPlayerResource * >( entity );
					break;
				}
				case ETFClassID::CFish:
				{
					fish.push_back( entity );
					break;
				}
				default: break;
			}
		}
	}
}

bool __entity_cache::is_scope( const FNV1A_t uModelHash )
{
	switch ( uModelHash )
	{
		case FNV1A::HashConst( "HudScope" ): { return true; }
		default: { return false; }
	}
}

bool __entity_cache::is_vaccinator( const FNV1A_t uModelHash )
{
	switch ( uModelHash )
	{
		case FNV1A::HashConst( "PlayerStatus_MedicSmallBulletResistImage" ): { return true; }
		case FNV1A::HashConst( "PlayerStatus_MedicUberBulletResistImage" ): { return true; }
		default: { return false; }
	}
}

bool __entity_cache::is_health( const FNV1A_t uModelHash )
{
	switch ( uModelHash )
	{
		case FNV1A::HashConst( TF_MEDKIT_SMALL_BDAY ): { return true; }
		case FNV1A::HashConst( TF_MEDKIT_MEDIUM_BDAY ): { return true; }
		case FNV1A::HashConst( TF_MEDKIT_LARGE_BDAY ): { return true; }
		case FNV1A::HashConst( TF_MEDKIT_SMALL_HALLOWEEN ): { return true; }
		case FNV1A::HashConst( TF_MEDKIT_MEDIUM_HALLOWEEN ): { return true; }
		case FNV1A::HashConst( TF_MEDKIT_LARGE_HALLOWEEN ): { return true; }
		case FNV1A::HashConst( LUNCHBOX_DROP_MODEL ): { return true; }
		case FNV1A::HashConst( LUNCHBOX_STEAK_DROP_MODEL ): { return true; }
		case FNV1A::HashConst( LUNCHBOX_ROBOT_DROP_MODEL ): { return true; }
		case FNV1A::HashConst( LUNCHBOX_FESTIVE_DROP_MODEL ): { return true; }
		case FNV1A::HashConst( TF_MEDKIT_SMALL ): { return true; }
		case FNV1A::HashConst( TF_MEDKIT_MEDIUM ): { return true; }
		case FNV1A::HashConst( TF_MEDKIT_LARGE ): { return true; }
		default: { return false; }
	}
}

bool __entity_cache::is_ammo( const FNV1A_t uModelHash )
{
	switch ( uModelHash )
	{
		case FNV1A::HashConst( TF_AMMOPACK_SMALL_BDAY ): { return true; }
		case FNV1A::HashConst( TF_AMMOPACK_MEDIUM_BDAY ): { return true; }
		case FNV1A::HashConst( TF_AMMOPACK_LARGE_BDAY ): { return true; }
		case FNV1A::HashConst( TF_AMMOPACK_SMALL ): { return true; }
		case FNV1A::HashConst( TF_AMMOPACK_MEDIUM ): { return true; }
		case FNV1A::HashConst( TF_AMMOPACK_LARGE ): { return true; }
		default: { return false; }
	}
}

C_TFPlayer *__entity_cache::get_local( )
{
	return local;
}

C_TFWeaponBase *__entity_cache::get_weapon( )
{
	return weapon;
}

C_TFPlayerResource *__entity_cache::get_resource( )
{
	return player_resource;
}

void __entity_cache::reset( )
{
	local = nullptr;
	weapon = nullptr;
	player_resource = nullptr;
	players.clear( );
	buildings.clear( );
	world.clear( );
	intel.clear( );
	control_point.clear( );
	payload.clear( );
	projectiles.clear( );
	npc_entity.clear( );
	sentries.clear( );
	dispensers.clear( );
	teleporters.clear( );
	stickies.clear( );
	flares.clear( );
	fish.clear( );
}