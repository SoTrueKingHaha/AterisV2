#include "events.h"
#include "../crits/crits.h"

void __events::setup( const std::deque<const char*>& events )
{
	if ( events.empty( ) )
		return;

	for ( auto event : events )
	{
		i::game_event->AddListener( this, event, false );

		if ( !i::game_event->FindListener( this, event ) )
		{
			std::string what = ( "Listener failed: %s", event );
			throw std::runtime_error( what );
		}
	}
}

void __events::destroy( )
{
	i::game_event->RemoveListener( this );
}

void __events::crits_handle( C_TFPlayer* local, IGameEvent* event, const FNV1A_t hash_name )
{
	if ( hash_name == FNV1A::HashConst( "player_hurt" ) )
	{
		if ( !local )
		{
			return;
		}

		const auto attacked		= i::engine_client->GetPlayerForUserID( event->GetInt( "userid" ) );
		const auto attacker		= i::engine_client->GetPlayerForUserID( event->GetInt( "attacker" ) );
		auto	   crit			= event->GetInt( "crit" );
		auto       damage		= event->GetInt( "damageamount" );
		const auto health		= event->GetInt( "health" );
		const auto weapon_id	= event->GetInt( "weaponid" );

		if ( attacker == local->entindex( ) && attacked != attacker )
		{
			const auto& entity = i::entity_list->GetClientEntity( attacked )->As<C_TFPlayer>( );
			if ( entity == nullptr || !entity->IsPlayer( ) )
			{
				return;
			}

			const auto& unlag = entity->get_unlag_data( );
			if ( unlag == nullptr )
			{
				return;
			}

			auto* weapon = i::entity_list->GetClientEntity( weapon_id )->As<C_TFWeaponBase>( );
			if ( weapon )
			{
				if ( weapon->GetSlot( ) == SLOT_MELEE )
				{
					crithack.last_hit_registered_melee = true;
					return;
				}
				else
				{
					crithack.last_hit_registered_melee = false;
				}
			}
			else
			{
				weapon = entity_cache.get_weapon( );
				if ( weapon && weapon->GetSlot( ) == SLOT_MELEE )
				{
					crithack.last_hit_registered_melee = true;
					return;
				}
				else
				{
					crithack.last_hit_registered_melee = false;
				}
			}

			if ( crit )
			{
				if ( local->IsCritBoosted( ) || local->IsMiniCritBoosted( ) )
				{
					crithack.boostracist += damage;
					i::cvar->ConsoleColorPrintf( { 0, 255, 255, 255 }, "Adding full crit.\n" );
				}
				else
				{
					crithack.Crematorium1488 += damage;
					i::cvar->ConsoleColorPrintf( { 0, 255, 255, 255 }, "Adding random crit.\n" );
				}
			}
		}
	}
	else if ( hash_name == FNV1A::HashConst( "teamplay_round_start" ) || hash_name == FNV1A::HashConst( "client_disconnect" )
			  || hash_name == FNV1A::HashConst( "client_beginconnect" ) || hash_name == FNV1A::HashConst( "game_newmap" ) )
	{
		i::cvar->ConsoleColorPrintf( { 0, 255, 255, 255 }, "Resetting.\n" );
		crithack.reset( );
	}
}

void __events::chat_info( C_TFPlayer* local, IGameEvent* event, const FNV1A_t hash_name )
{
	if ( !i::engine_client->IsConnected( ) || !i::engine_client->IsInGame( ) || !vars.misc.chatlog.log_active )
		return;

	if ( !local )
	{
		return;
	}

	if ( vars.misc.chatlog.log_connects && hash_name == FNV1A::HashConst( "player_connect" ) )
	{
		if ( const auto& entity = GET_ENT_I( i::engine_client->GetPlayerForUserID( event->GetInt( "userid" ) ) ) )
		{
			if ( !entity ) { return; }

			int index = entity->entindex( );
			PlayerInfo_t pi = {};
			i::engine_client->GetPlayerInfo( index, &pi );
			const auto& player = entity->As<C_TFPlayer>( );
			std::string team = player->m_iTeamNum( ) != local->m_iTeamNum( ) ? white + "ENEMY" + gray : white + "TEAM" + gray;
			std::string name = yellow + pi.name + white;
			std::string guid = yellow + pi.guid + white;

			std::string connect = gray + "[" + light_green + "ateris" + gray + "]  [" + team + "] " + name + " has joined. [" + guid + "]";
			i::client_mode->m_pChatElement->ChatPrintf( 0, connect.c_str( ) );
		}
	}

	if ( vars.misc.chatlog.log_disconnects && hash_name == FNV1A::HashConst( "player_disconnect" ) )
	{
		if ( const auto& entity = GET_ENT_I( i::engine_client->GetPlayerForUserID( event->GetInt( "userid" ) ) ) )
		{
			int index = entity->entindex( );
			PlayerInfo_t pi = {};
			i::engine_client->GetPlayerInfo( index, &pi );
			const auto& player = entity->As<C_TFPlayer>( );
			std::string name = yellow + event->GetString( "name" ) + white;

			std::string disconnect = gray + "[" + light_green + "ateris" + gray + "] " + name + " has left.";
			i::client_mode->m_pChatElement->ChatPrintf( 0, disconnect.c_str( ) );
		}
	}

	if ( vars.misc.chatlog.log_class_changes && hash_name == FNV1A::HashConst( "player_changeclass" ) )
	{
		if ( const auto& entity = GET_ENT_I( i::engine_client->GetPlayerForUserID( event->GetInt( "userid" ) ) ) )
		{
			if ( !entity ) { return; }

			int index = entity->entindex( );

			PlayerInfo_t pi = {};

			i::engine_client->GetPlayerInfo( index, &pi );
			const auto& player = entity->As<C_TFPlayer>( );
			std::string team = player->m_iTeamNum( ) != local->m_iTeamNum( ) ? white + "ENEMY" + gray : white + "TEAM" + gray;
			std::string name = yellow + pi.name + white;
			std::string szClass = utils.get_class_by_index( event->GetInt( "class" ) );

			std::string classchange = gray + "[" + light_green + "ateris" + gray + "]  [" + team + "] " + name + " changed class to " + szClass + ".";
			i::client_mode->m_pChatElement->ChatPrintf( 0, classchange.c_str( ) );
		}
	}

	if ( vars.misc.chatlog.log_name_changes && hash_name == FNV1A::HashConst( "player_changename" ) )
	{
		if ( const auto& entity = GET_ENT_I( i::engine_client->GetPlayerForUserID( event->GetInt( "userid" ) ) ) )
		{
			if ( !entity ) { return; }

			std::string szNewName = event->GetString( "newname" );
			std::string szOldName = event->GetString( "oldname" );

			const auto& player = entity->As<C_TFPlayer>( );
			std::string team = player->m_iTeamNum( ) != local->m_iTeamNum( ) ? white + "ENEMY" + gray : white + "TEAM" + gray;

			std::string namechange = gray + "[" + light_green + "ateris" + gray + "]  [" + team + "] " + yellow + szOldName + white + " changed name to " + yellow + szNewName + white + ".";
			i::client_mode->m_pChatElement->ChatPrintf( 0, namechange.c_str( ) );
		}
	}

	if ( vars.misc.chatlog.log_votes && hash_name == FNV1A::HashConst( "vote_cast" ) )
	{
		const auto& entity = GET_ENT_I( event->GetInt( "entityid" ) )->As<C_BaseEntity>( );
		if ( entity && entity->GetClassID( ) == ETFClassID::CTFPlayer )
		{
			if ( !entity ) { return; }

			const bool voted_yes = event->GetInt( "vote_option" ) == 0;
			PlayerInfo_t pi = {};

			i::engine_client->GetPlayerInfo( entity->entindex( ), &pi );
			const auto& player = entity->As<C_TFPlayer>( );
			std::string team = player->m_iTeamNum( ) != local->m_iTeamNum( ) ? white + "ENEMY" + gray : white + "TEAM" + gray;
			std::string name = yellow + pi.name + white;
			std::string szVote = voted_yes ? red + "Yes" + white : green + "No" + white;

			std::string vote = gray + "[" + light_green + "ateris" + gray + "]  [" + team + "] " + name + " voted " + szVote + ".";
			i::client_mode->m_pChatElement->ChatPrintf( 0, vote.c_str( ) );
		}
	}
}

void __events::FireGameEvent( IGameEvent* event )
{
	const auto& local = entity_cache.get_local( );
	if ( !event || !local )
		return;

	const FNV1A_t hash_name = FNV1A::Hash( event->GetName( ) );
	crits_handle( local, event, hash_name );
	chat_info( local, event, hash_name );

	/*if ( hash_name == FNV1A::HashConst( "player_death" ) )
	{

	}*/
}