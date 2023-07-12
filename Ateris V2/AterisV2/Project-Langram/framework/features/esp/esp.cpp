#include "esp.h"

void __esp::render_players( C_TFPlayer* local )
{
	if ( !vars.esp.global.esp_global || !vars.esp.players.esp_players )
	{
		return;
	}

	for ( const auto& player : entity_cache.players[ TEAMSALL ] )
	{
		const auto& resource = entity_cache.get_resource( );
		if ( player->deadflag( ) )
		{
			continue;
		}

		color esp_color = { 255, 255, 255, 255 };
		if ( player->IsDormant( ) )
		{
			esp_color = { 170, 170, 170, 255 };
			if ( i::engine_client->Time( ) - global_info.dormant_players[ player->entindex( ) ].last_update >= 5.0f )
			{
				continue;
			}
		}

		PlayerInfo_t pi{ };
		if ( !i::engine_client->GetPlayerInfo( player->entindex( ), &pi ) )
		{
			continue;
		}

		if ( vars.esp.players.esp_players_enemy_only && player->m_iTeamNum( ) == local->m_iTeamNum( ) && !player->PlayerOnFriendsList( ) )
		{
			continue;
		}

		if ( !vars.esp.players.esp_players_friends && player->PlayerOnFriendsList( ) )
		{
			continue;
		}

		int x = 0, y = 0, w = 0, h = 0;
		vector percent48[ 8 ];
		if ( utils.get_draw_bounds( player, percent48, x, y, w, h ) )
		{
			if ( vars.global.legit_only )
			{
				if ( vars.esp.players.players_high_kd && !player->HasHighKD( ) )
				{
					continue;
				}

				if ( vars.esp.players.players_high_headshot && !player->HasHighHeadshotRatio( ) )
				{
					continue;
				}
			}

			if ( vars.esp.players.players_name )
			{
				draw.outlined_string( ESP, x + w / 2, y - 9, esp_color, ALIGN_CENTER, utils.utf8_to_wide( pi.name ).c_str( ) );
			}

			if ( vars.esp.players.players_box )
			{
				draw.black_outlined_rect( x, y, w, h + 1, colors.esp.get_entity_color( player, local ) );
			}

			if ( vars.esp.players.players_healthbar )
			{
				int nHealth = player->m_iHealth( );
				int nMaxHealth = player->GetMaxHealth( );
				static const int nWidth = 2;

				float flHealth = ( float )nHealth;
				float flMaxHealth = ( float )nMaxHealth;
				int nHeight = h + ( flHealth < flMaxHealth ? 1 : 0 );

				if ( health[ player->entindex( ) ] < 0 || health[ player->entindex( ) ] > nMaxHealth )
				{
					health[ player->entindex( ) ] = flHealth;
				}
				float& health_anim = health[ player->entindex( ) ];
				float ratio;
				if ( vars.esp.players.players_animated_healthbar )
				{
					ratio = std::clamp( health_anim / flMaxHealth, 0.0f, 1.0f );

					if ( health_anim > nHealth )
					{
						if ( nMaxHealth < 451 )
							health_anim -= ( flMaxHealth * 2 ) * i::global_vars->frametime;
						else
							health_anim -= ( flMaxHealth * 2 ) * i::global_vars->frametime * flMaxHealth / 300.f;
					}
					else
					{
						health_anim = nHealth;
					}
				}
				else
				{
					ratio = std::clamp( flHealth / flMaxHealth, 0.0f, 1.0f );
				}

				color health = vars.esp.players.players_custom_healthbar ? colors.esp.get_entity_color( player, local ) : utils.get_health_color( nHealth, nMaxHealth );

				draw.rect( x - nWidth - 3, y, nWidth, nHeight * ( 1.0f - ratio ) + 1, { 0, 0, 0, 180 } );
				if ( vars.esp.players.players_gradient_healthbar )
				{
					if ( nHealth <= nMaxHealth )
					{
						color top_clr = vars.colors.esp.players.player_healthbar_top;
						color bottom_clr = vars.colors.esp.players.player_healthbar_bottom;
						i::surface->DrawSetColor( top_clr.r, top_clr.g, top_clr.b, top_clr.a );
						i::surface->DrawFilledRectFade( x - nWidth - 3, y + nHeight - ( nHeight * ratio ), x - nWidth, y + nHeight - ( nHeight * ratio ) + ( nHeight * ratio + 1 ), 255, 255, false );
						i::surface->DrawSetColor( bottom_clr.r, bottom_clr.g, bottom_clr.b, bottom_clr.a );
						i::surface->DrawFilledRectFade( x - nWidth - 3, y + nHeight - ( nHeight * ratio ), x - nWidth, y + nHeight - ( nHeight * ratio ) + ( nHeight * ratio + 1 ), utils.GetAlpha( vars.esp.players.players_animated_healthbar ? health_anim : flHealth, nMaxHealth ), 255, false );
					}
				}
				else
				{
					if ( nHealth <= nMaxHealth )
					{
						draw.rect( x - nWidth - 3, y + nHeight - ( nHeight * ratio ), nWidth, nHeight * ratio + 1, health );
					}
				}
				draw.outlined_rect( x - nWidth - 4, y - 1, nWidth * 2, h + 3, { 0, 0, 0, 180 } );

				if ( vars.esp.players.players_healthbar_lines )
				{
					for ( int i = 1; i <= vars.esp.players.players_healthbar_lines_count; i++ )
					{
						if ( i == 1 )
							healthbar_yline = 1.f / ( vars.esp.players.players_healthbar_lines_count + 1 );

						if ( y + h * healthbar_yline > y + nHeight - ( nHeight * ratio ) )
						{
							draw.line( x - nWidth - 3, y + h * healthbar_yline, x - nWidth - 1, y + h * healthbar_yline, { 0, 0, 0, 180 } );
						}
						healthbar_yline += 1.f / ( vars.esp.players.players_healthbar_lines_count + 1 );
					}
				}

				if ( nHealth > nMaxHealth )
				{
					draw.rect( x - nWidth - 3, y, nWidth, nHeight + 1, { 65, 155, 220, 180 } );

					if ( vars.esp.players.players_healthbar_lines )
					{
						for ( int i = 1; i <= vars.esp.players.players_healthbar_lines_count; i++ )
						{
							if ( i == 1 )
								healthbar_yline = 1.f / ( vars.esp.players.players_healthbar_lines_count + 1 );

							draw.line( x - nWidth - 3, y + h * healthbar_yline, x - nWidth - 1, y + h * healthbar_yline, { 0, 0, 0, 180 } );
							healthbar_yline += 1.f / ( vars.esp.players.players_healthbar_lines_count + 1 );
						}
					}
				}
			}

			if ( vars.esp.players.players_conds )
			{
				y -= 2;
				int nTextOffset = 0;

				if ( player->m_iHealth( ) > player->GetMaxHealth( ) )
				{
					draw.outlined_string( ESPVERYSMALL, x + w + 2, y + nTextOffset, { 150, 255, 150, 255 }, ALIGN_DEFAULT, L"+%i HP", player->m_iHealth( ) - player->GetMaxHealth( ) );
					nTextOffset += fonts[ ESPVERYSMALL ].tall;
				}

				if ( player->IsCloaked( ) )
				{
					draw.outlined_string( ESPVERYSMALL, x + w + 2, y + nTextOffset, { 200, 200, 200, 255 }, ALIGN_DEFAULT, L"CLOAKED" );
					nTextOffset += fonts[ ESPVERYSMALL ].tall;
				}

				if ( player->IsInvulnerable( ) )
				{
					draw.outlined_string( ESPVERYSMALL, x + w + 2, y + nTextOffset, { 255, 110, 255, 255 }, ALIGN_DEFAULT, L"UBERCHARGE" );
					nTextOffset += fonts[ ESPVERYSMALL ].tall;
				}

				if ( player->IsCritBoosted( ) )
				{
					draw.outlined_string( ESPVERYSMALL, x + w + 2, y + nTextOffset, { 255, 150, 255, 255 }, ALIGN_DEFAULT, L"CRIT BOOST" );
					nTextOffset += fonts[ ESPVERYSMALL ].tall;
				}

				if ( player->HasHighKD( ) )
				{
					draw.outlined_string( ESPVERYSMALL, x + w + 2, y + nTextOffset, { 255, 255, 0, 255 }, ALIGN_DEFAULT, L"HIGH K/D" );
					nTextOffset += fonts[ ESPVERYSMALL ].tall;
				}

				if ( player->HasHighHeadshotRatio( ) )
				{
					draw.outlined_string( ESPVERYSMALL, x + w + 2, y + nTextOffset, { 255, 150, 0, 255 }, ALIGN_DEFAULT, L"HIGH HEADSHOT RATIO" );
					nTextOffset += fonts[ ESPVERYSMALL ].tall;
				}

				for ( int i = 0; i < 130; i++ )
				{
					if ( player->InCond( static_cast< ETFCond >( i ) ) )
					{
						auto conds = utils.get_cond( static_cast< ETFCond >( i ) );

						draw.outlined_string( ESPVERYSMALL, x + w + 2, y + nTextOffset, conds.color, ALIGN_DEFAULT, conds.name );
						if ( wcslen( conds.name ) != 0 )
						{
							nTextOffset += fonts[ ESPVERYSMALL ].tall;
						}
					}
				}
				nTextOffset += fonts[ ESPVERYSMALL ].tall;
				y += 2;
			}

			int esptext = 0;
			//weapontext
			if ( vars.esp.players.players_weapons )
			{
				if ( const auto& weapon = GET_ENT_H( player->m_hActiveWeapon( ) )->As<C_TFWeaponBase>( ) )
				{
					draw.outlined_string( ESPVERYSMALL, x + ( w / 2 ), y + h + 6, esp_color, ALIGN_CENTER, utils.get_weapon_name( weapon ) );
				}
				esptext += fonts[ ESPVERYSMALL ].tall;
			}

			if ( vars.esp.players.players_distance )
			{
				if ( local && !local->deadflag( ) && !local->IsDormant( ) && player != local )
				{
					const vector localOrigin = local->GetAbsOrigin( );
					const vector playerOrigin = player->GetAbsOrigin( );

					float distance = sqrt( powf( localOrigin.x - playerOrigin.x, 2 ) + powf( localOrigin.y - playerOrigin.y, 2 ) + powf( localOrigin.z - playerOrigin.z, 2 ) ) * 0.01905f;
					int realdistance = round( distance / 5 ) * 5;
					draw.outlined_string( ESPSMALL, x + w / 2, y + h + 7 + esptext, esp_color, ALIGN_CENTER, L"| %im |", /*real*/( int )distance );
					esptext += fonts[ ESPSMALL ].tall;
				}
			}
		}

		if ( player->IsClass( CLASS_SNIPER ) && player->InCond( TF_COND_ZOOMED ) )
		{
			vector vForward = {};
			math.angle_vectors( player->m_angEyeAngles( ), &vForward );
			vector vFrom = player->GetHitboxPosition( HITBOX_HEAD );
			vector vTo = ( vFrom + ( vForward * 8192.0f ) );

			CGameTrace Trace = {};
			CTraceFilterHitscan Filter = {};
			Filter.pSkip = player;
			utils.trace( vFrom, vTo, ( MASK_SHOT | CONTENTS_GRATE ), &Filter, &Trace );

			i::beams->DrawBeam( Trace.m_vStartPos, Trace.m_vEndPos, player->m_iTeamNum ( ) == TEAM_RED ? color{ 255, 0, 0, 255 } : color{ 50, 50, 255, 255 } );
		}

		if ( vars.esp.players.players_alert && player->m_iTeamNum( ) != local->m_iTeamNum( ) )
		{
			const vector local_pos = local->GetWorldSpaceCenter( );
			vector player_pos = player->GetWorldSpaceCenter( );
			vector screen;

			vector player_angle = player->m_angEyeAngles( );
			vector angle_to = math.calc_angle( player_pos, local_pos );
			float fov = math.calc_fov( player_angle, angle_to );
			color playercol = player->IsCloaked( ) ? ( vars.colors.esp.players.player_cloaked ) : ( fov < 10.f ? color{ 255, 50, 50, 180 } : colors.esp.get_entity_color( player, local ) );

			const vector local_origin = local->GetAbsOrigin( );
			const vector player_origin = player->GetAbsOrigin( );
			float distance = sqrt( powf( local_origin.x - player_origin.x, 2 ) + powf( local_origin.y - player_origin.y, 2 ) + powf( local_origin.z - player_origin.z, 2 ) ) * 0.01905f;

			const auto& weapon = GET_ENT_H( player->m_hActiveWeapon( ) )->As<C_TFWeaponBase>( );

			if ( !utils.is_on_screen( local, player ) && player->m_iTeamNum( ) != local->m_iTeamNum( ) && utils.vis_pos( local, player, local_pos, player_pos ) )
			{
				DrawPlayer( local_pos, player_pos, playercol, utils.utf8_to_wide( pi.name ).c_str( ), utils.get_weapon_name( weapon ), player->m_iHealth( ), player->GetMaxHealth( ), ( int )distance );
			}
		}
	}

	if ( local && vars.esp.players.esp_player_local )
	{
		PlayerInfo_t pi{ };
		if ( !i::engine_client->GetPlayerInfo( local->entindex( ), &pi ) )
		{
			return;
		}
		
		color esp_color = { 255, 255, 255, 255 };
		int x = 0, y = 0, w = 0, h = 0;
		vector percent48[ 8 ];
		if ( utils.get_draw_bounds( local, percent48, x, y, w, h ) )
		{
			bool thirdperson = i::input->CAM_IsThirdPerson( ) || local->InCond( TF_COND_TAUNTING );
			if ( local && !thirdperson )
			{
				return;
			}

			if ( vars.esp.players.players_name )
			{
				draw.outlined_string( ESP, x + w / 2, y - 9, esp_color, ALIGN_CENTER, utils.utf8_to_wide( pi.name ).c_str( ) );
			}

			if ( vars.esp.players.players_box )
			{
				draw.black_outlined_rect( x, y, w, h + 1, colors.esp.get_entity_color( local, local ) );
			}

			if ( vars.esp.players.players_healthbar )
			{
				int nHealth = local->m_iHealth( );
				int nMaxHealth = local->GetMaxHealth( );
				static const int nWidth = 2;

				float flHealth = ( float )nHealth;
				float flMaxHealth = ( float )nMaxHealth;
				int nHeight = h + ( flHealth < flMaxHealth ? 1 : 0 );

				if ( health[ local->entindex( ) ] < 0 || health[ local->entindex( ) ] > nMaxHealth )
				{
					health[ local->entindex( ) ] = flHealth;
				}
				float& health_anim = health[ local->entindex( ) ];
				float ratio;
				if ( vars.esp.players.players_animated_healthbar )
				{
					ratio = std::clamp( health_anim / flMaxHealth, 0.0f, 1.0f );

					if ( health_anim > nHealth )
					{
						if ( nMaxHealth < 451 )
							health_anim -= ( flMaxHealth * 2 ) * i::global_vars->frametime;
						else
							health_anim -= ( flMaxHealth * 2 ) * i::global_vars->frametime * flMaxHealth / 300.f;
					}
					else
					{
						health_anim = nHealth;
					}
				}
				else
				{
					ratio = std::clamp( flHealth / flMaxHealth, 0.0f, 1.0f );
				}

				color health = vars.esp.players.players_custom_healthbar ? colors.esp.get_entity_color( local, local ) : utils.get_health_color( nHealth, nMaxHealth );

				draw.rect( x - nWidth - 3, y, nWidth, nHeight* ( 1.0f - ratio ) + 1, { 0, 0, 0, 180 } );
				if ( vars.esp.players.players_gradient_healthbar )
				{
					if ( nHealth <= nMaxHealth )
					{
						color top_clr = vars.colors.esp.players.player_healthbar_top;
						color bottom_clr = vars.colors.esp.players.player_healthbar_bottom;
						i::surface->DrawSetColor( top_clr.r, top_clr.g, top_clr.b, top_clr.a );
						i::surface->DrawFilledRectFade( x - nWidth - 3, y + nHeight - ( nHeight * ratio ), x - nWidth, y + nHeight - ( nHeight * ratio ) + ( nHeight * ratio + 1 ), 255, 255, false );
						i::surface->DrawSetColor( bottom_clr.r, bottom_clr.g, bottom_clr.b, bottom_clr.a );
						i::surface->DrawFilledRectFade( x - nWidth - 3, y + nHeight - ( nHeight * ratio ), x - nWidth, y + nHeight - ( nHeight * ratio ) + ( nHeight * ratio + 1 ), utils.GetAlpha( vars.esp.players.players_animated_healthbar ? health_anim : flHealth, nMaxHealth ), 255, false );
					}
				}
				else
				{
					if ( nHealth <= nMaxHealth )
					{
						draw.rect( x - nWidth - 3, y + nHeight - ( nHeight * ratio ), nWidth, nHeight * ratio + 1, health );
					}
				}
				draw.outlined_rect( x - nWidth - 4, y - 1, nWidth * 2, h + 3, { 0, 0, 0, 180 } );

				if ( vars.esp.players.players_healthbar_lines )
				{
					for ( int i = 1; i <= vars.esp.players.players_healthbar_lines_count; i++ )
					{
						if ( i == 1 )
							healthbar_yline = 1.f / ( vars.esp.players.players_healthbar_lines_count + 1 );

						if ( y + h * healthbar_yline > y + nHeight - ( nHeight * ratio ) )
						{
							draw.line( x - nWidth - 3, y + h * healthbar_yline, x - nWidth - 1, y + h * healthbar_yline, { 0, 0, 0, 180 } );
						}
						healthbar_yline += 1.f / ( vars.esp.players.players_healthbar_lines_count + 1 );
					}
				}

				if ( nHealth > nMaxHealth )
				{
					draw.rect( x - nWidth - 3, y, nWidth, nHeight + 1, { 65, 155, 220, 180 } );

					if ( vars.esp.players.players_healthbar_lines )
					{
						for ( int i = 1; i <= vars.esp.players.players_healthbar_lines_count; i++ )
						{
							if ( i == 1 )
								healthbar_yline = 1.f / ( vars.esp.players.players_healthbar_lines_count + 1 );

							draw.line( x - nWidth - 3, y + h * healthbar_yline, x - nWidth - 1, y + h * healthbar_yline, { 0, 0, 0, 180 } );
							healthbar_yline += 1.f / ( vars.esp.players.players_healthbar_lines_count + 1 );
						}
					}
				}
			}

			if ( vars.esp.players.players_conds )
			{
				y -= 2;
				int nTextOffset = 0;

				if ( local->m_iHealth( ) > local->GetMaxHealth( ) )
				{
					draw.outlined_string( ESPVERYSMALL, x + w + 2, y + nTextOffset, { 150, 255, 150, 255 }, ALIGN_DEFAULT, L"+%i HP", local->m_iHealth( ) - local->GetMaxHealth( ) );
					nTextOffset += fonts[ ESPVERYSMALL ].tall;
				}

				if ( local->InCond( TF_COND_FEIGN_DEATH ) || local->InCond( TF_COND_STEALTHED_BLINK ) || local->InCond( TF_COND_STEALTHED ) )
				{
					draw.outlined_string( ESPVERYSMALL, x + w + 2, y + nTextOffset, { 200, 200, 200, 255 }, ALIGN_DEFAULT, L"CLOAKED" );
					nTextOffset += fonts[ ESPVERYSMALL ].tall;
				}

				if ( local->InCond( TF_COND_INVULNERABLE ) )
				{
					draw.outlined_string( ESPVERYSMALL, x + w + 2, y + nTextOffset, { 255, 110, 255, 255 }, ALIGN_DEFAULT, L"UBERCHARGE" );
					nTextOffset += fonts[ ESPVERYSMALL ].tall;
				}

				if ( local->IsCritBoosted( ) )
				{
					draw.outlined_string( ESPVERYSMALL, x + w + 2, y + nTextOffset, { 255, 150, 255, 255 }, ALIGN_DEFAULT, L"CRIT BOOST" );
					nTextOffset += fonts[ ESPVERYSMALL ].tall;
				}

				if ( local->HasHighKD( ) )
				{
					draw.outlined_string( ESPVERYSMALL, x + w + 2, y + nTextOffset, { 255, 255, 0, 255 }, ALIGN_DEFAULT, L"HIGH K/D" );
					nTextOffset += fonts[ ESPVERYSMALL ].tall;
				}

				if ( local->HasHighHeadshotRatio( ) )
				{
					draw.outlined_string( ESPVERYSMALL, x + w + 2, y + nTextOffset, { 255, 150, 0, 255 }, ALIGN_DEFAULT, L"HIGH HEADSHOT RATIO" );
					nTextOffset += fonts[ ESPVERYSMALL ].tall;
				}

				for ( int i = 0; i < 130; i++ )
				{
					if ( local->InCond( static_cast< ETFCond >( i ) ) )
					{
						auto conds = utils.get_cond( static_cast< ETFCond >( i ) );

						draw.outlined_string( ESPVERYSMALL, x + w + 2, y + nTextOffset, conds.color, ALIGN_DEFAULT, conds.name );
						if ( wcslen( conds.name ) != 0 )
						{
							nTextOffset += fonts[ ESPVERYSMALL ].tall;
						}
					}
				}
				nTextOffset += fonts[ ESPVERYSMALL ].tall;
				y += 2;
			}

			int esptext = 0;
			//weapontext
			if ( vars.esp.players.players_weapons )
			{
				if ( const auto& weapon = GET_ENT_H( local->m_hActiveWeapon( ) )->As<C_TFWeaponBase>( ) )
				{
					draw.outlined_string( ESPVERYSMALL, x + ( w / 2 ), y + h + 6, esp_color, ALIGN_CENTER, utils.get_weapon_name( weapon ) );
				}
				esptext += fonts[ ESPVERYSMALL ].tall;
			}
		}
	}
}

void __esp::render_buildings( C_TFPlayer* local )
{
	if ( !vars.esp.global.esp_global || !vars.esp.buildings.esp_buildings )
	{
		return;
	}

	for ( const auto& building : entity_cache.buildings[ TEAMSALL ] )
	{
		int x = 0, y = 0, w = 0, h = 0;
		vector percent48[ 8 ];

		if ( !building || building->IsDormant( ) )
		{
			continue;
		}

		if ( vars.esp.buildings.esp_buildings_enemy_only && building->m_iTeamNum( ) == local->m_iTeamNum( ) && building->m_hBuilder( ).Get( ) != local && !building->m_hBuilder( ).Get( )->As<C_TFPlayer>( )->PlayerOnFriendsList( ) )
		{
			continue;
		}

		if ( !vars.esp.buildings.esp_buildings_local && building->m_hBuilder( ).Get( ) == local )
		{
			continue;
		}
		
		if ( !vars.esp.buildings.esp_buildings_friends && building->m_hBuilder( ).Get( )->As<C_TFPlayer>( )->PlayerOnFriendsList( ) )
		{
			continue;
		}

		if ( utils.get_draw_bounds( building, percent48, x, y, w, h ) )
		{
			if ( vars.esp.buildings.buildings_name )
			{
				draw.outlined_string( ESP, x + w / 2, y - 6, { 255, 255, 255, 255 }, ALIGN_CENTER, utils.get_building_name( building ).c_str( ) );
			}

			color sentrycol = colors.esp.get_building_color( building, local );
			const auto& enemy = building->As<C_ObjectSentrygun>( )->m_hEnemy( ).Get( );
			if ( enemy )
			{
				sentrycol = building->As<C_ObjectSentrygun>( )->m_hEnemy( ).Get( ) == local ? color{ 255, 50, 50, 180 } : colors.esp.get_building_color( building, local );
			}
			if ( vars.esp.buildings.buildings_box )
			{
				draw.black_outlined_rect( x, y, w, h + 1, sentrycol );
			}

			if ( vars.esp.buildings.buildings_healthbar )
			{
				int nHealth = building->m_iHealth( );
				int nMaxHealth = building->m_iMaxHealth( );
				static const int nWidth = 2;

				float flHealth = ( float )nHealth;
				float flMaxHealth = ( float )nMaxHealth;

				int nHeight = h + ( flHealth < flMaxHealth ? 1 : 0 );
				if ( health_buildings[ building->entindex( ) ] < 0 || health_buildings[ building->entindex( ) ] > nMaxHealth )
				{
					health_buildings[ building->entindex( ) ] = flHealth;
				}
				float& health_anim = health_buildings[ building->entindex( ) ];
				float ratio;
				if ( vars.esp.buildings.buildings_animated_healthbar )
				{
					ratio = std::clamp( health_anim / flMaxHealth, 0.0f, 1.0f );

					if ( health_anim <= nHealth )
					{
						health_anim = nHealth;
					}
					else
					{
						health_anim -= ( 150.f * 2 ) * i::global_vars->frametime;
					}
				}
				else
				{
					ratio = std::clamp( flHealth / flMaxHealth, 0.0f, 1.0f );
				}

				color health = vars.esp.buildings.buildings_custom_healthbar ? colors.esp.get_building_color( building, local ) : utils.get_health_color( nHealth, nMaxHealth );

				draw.rect( x - nWidth - 3, y, nWidth, nHeight * ( 1.0f - ratio ) + 1, { 0, 0, 0, 180 } );
				if ( vars.esp.buildings.buildings_gradient_healthbar )
				{
					color top_clr = vars.colors.esp.buildings.building_healthbar_top;
					color bottom_clr = vars.colors.esp.buildings.building_healthbar_bottom;
					i::surface->DrawSetColor( top_clr.r, top_clr.g, top_clr.b, top_clr.a );
					i::surface->DrawFilledRectFade( x - nWidth - 3, y + nHeight - ( nHeight * ratio ), x - nWidth, y + nHeight - ( nHeight * ratio ) + ( nHeight * ratio + 1 ), 255, 255, false );
					i::surface->DrawSetColor( bottom_clr.r, bottom_clr.g, bottom_clr.b, bottom_clr.a );
					i::surface->DrawFilledRectFade( x - nWidth - 3, y + nHeight - ( nHeight * ratio ), x - nWidth, y + nHeight - ( nHeight * ratio ) + ( nHeight * ratio + 1 ), utils.GetAlpha( vars.esp.buildings.buildings_animated_healthbar ? health_anim : flHealth, nMaxHealth ), 255, false );
				}
				else
				{
					draw.rect( x - nWidth - 3, y + nHeight - ( nHeight * ratio ), nWidth, nHeight * ratio + 1, health );
				}
				draw.outlined_rect( x - nWidth - 4, y - 1, nWidth * 2, h + 3, { 0, 0, 0, 180 } );

				if ( vars.esp.buildings.buildings_healthbar_lines )
				{
					for ( int i = 1; i <= vars.esp.buildings.buildings_healthbar_lines_count; i++ )
					{
						if ( i == 1 )
							healthbar_yline = 1.f / ( vars.esp.buildings.buildings_healthbar_lines_count + 1 );

						if ( y + h * healthbar_yline > y + nHeight - ( nHeight * ratio ) )
						{
							draw.line( x - nWidth - 4, y + h * healthbar_yline, x - nWidth - 1, y + h * healthbar_yline, { 0, 0, 0, 255 } );
						}
						healthbar_yline += 1.f / ( vars.esp.buildings.buildings_healthbar_lines_count + 1 );
					}
				}
			}

			if ( vars.esp.buildings.buildings_conds )
			{
				int nTextOffset = 0;
				y -= 2;
				if ( building->GetClassID( ) == ETFClassID::CObjectTeleporter )
				{
					float recharge = building->As<C_ObjectTeleporter>( )->m_flRechargeTime( ) - i::global_vars->curtime + 0.05f;
					if ( building->As<C_ObjectTeleporter>( )->m_iState( ) == 6 && recharge > 0 )
					{
						draw.outlined_string( ESPVERYSMALL, x + w + 3, y + nTextOffset, { 255, 255, 255, 255 }, ALIGN_DEFAULT, L"RECHARGES IN %.1f SEC", ( recharge ) );
						nTextOffset += fonts[ ESPVERYSMALL ].tall;
					}
				}
				int level = building->m_iUpgradeLevel( );
				if ( level )
				{

					draw.outlined_string( ESPVERYSMALL, x + w + 3, y + nTextOffset, { 255, 255, 255, 255 }, ALIGN_DEFAULT, L"LEVEL %i", ( level ) );
					nTextOffset += fonts[ ESPVERYSMALL ].tall;
				}
				y += 2;
			}

			if ( vars.esp.buildings.buildings_distance )
			{
				if ( local && !local->deadflag( ) && !local->IsDormant( ) )
				{
					const vector localOrigin = local->GetAbsOrigin( );
					const vector buildingOrigin = building->GetAbsOrigin( );

					float distance = sqrt( powf( localOrigin.x - buildingOrigin.x, 2 ) + powf( localOrigin.y - buildingOrigin.y, 2 ) + powf( localOrigin.z - buildingOrigin.z, 2 ) ) * 0.01905f;
					int realdistance = round( distance / 5 ) * 5;
					draw.outlined_string( ESPSMALL, x + w / 2, y + h + 7, { 255, 255, 255, 255 }, ALIGN_CENTER, L"| %im |", /*real*/( int )distance );
				}
			}
		}
	}

	if ( vars.esp.buildings.sentry_alert )
	{
		for ( const auto& entity : entity_cache.sentries[ ENEMY ] )
		{
			int x = 0, y = 0, w = 0, h = 0;
			vector percent48[ 8 ];

			const auto& sentry = entity->As<C_ObjectSentrygun>( );
			if ( !sentry || sentry->IsDormant( ) )
			{
				continue;
			}

			if ( utils.get_draw_bounds( sentry, percent48, x, y, w, h ) )
			{
				if ( sentry->m_hEnemy( ).Get( ) == local )
				{
					draw.outlined_string( ESP, x + w / 2, y - 17, { 255, 25, 25, 255 }, ALIGN_CENTER, L"Targeting you" );
				}
			}

			const vector local_pos = local->GetWorldSpaceCenter( );
			vector sentry_pos = sentry->GetWorldSpaceCenter( );
			vector screen;

			color sentrycol = sentry->m_hEnemy( ).Get( ) == local ? color{ 255, 50, 50, 180 } : colors.esp.get_building_color( sentry, local );
			const vector local_origin = local->GetAbsOrigin( );
			const vector sentry_origin = sentry->GetAbsOrigin( );
			float distance = sqrt( powf( local_origin.x - sentry_origin.x, 2 ) + powf( local_origin.y - sentry_origin.y, 2 ) + powf( local_origin.z - sentry_origin.z, 2 ) ) * 0.01905f;

			if ( !utils.is_on_screen( local, sentry ) && sentry->m_iTeamNum( ) != local->m_iTeamNum( ) && utils.vis_pos( local, sentry, local_pos, sentry_pos ) )
			{
				DrawSentry( local_pos, sentry_pos, sentrycol, utils.get_building_name( sentry ).c_str( ), sentry->m_iHealth( ), sentry->m_iMaxHealth( ), sentry->m_iUpgradeLevel( ), ( int )distance );
			}
		}
	}
}

void __esp::render_world( C_TFPlayer* local, C_TFWeaponBase* weapon )
{
	if ( !vars.esp.global.esp_global || !vars.esp.world.esp_world )
	{
		return;
	}

	for ( const auto& healthpack : entity_cache.world[ HEALTH ] )
	{
		int x = 0, y = 0, w = 0, h = 0;
		vector percent48[ 8 ];

		if ( !healthpack || healthpack->IsDormant( ) )
		{
			continue;
		}

		if ( utils.get_draw_bounds( healthpack, percent48, x, y, w, h ) )
		{
			if ( local && !local->deadflag( ) )
			{
				if ( vars.global.legit_only && vars.esp.world.low_health_esp && !local->IsHealthCritical( ) && !local->InCond( TF_COND_BURNING ) )
				{
					continue;
				}
			}

			if ( vars.esp.world.world_name )
			{
				draw.outlined_string( ESP, x + w / 2, y - 6, { 255, 255, 255, 255 }, ALIGN_CENTER, L"Healthpack" );
			}

			if ( vars.esp.world.world_box )
			{
				draw.black_outlined_rect( x, y, w, h + 1, vars.colors.esp.world.esp_world_health );
			}

			if ( vars.esp.world.world_distance )
			{
				if ( local && !local->deadflag( ) && !local->IsDormant( ) )
				{
					const vector localOrigin = local->GetAbsOrigin( );
					const vector healthOrigin = healthpack->GetAbsOrigin( );

					float distance = sqrt( powf( localOrigin.x - healthOrigin.x, 2 ) + powf( localOrigin.y - healthOrigin.y, 2 ) + powf( localOrigin.z - healthOrigin.z, 2 ) ) * 0.01905f;
					int realdistance = round( distance / 5 ) * 5;
					draw.outlined_string( ESPSMALL, x + w / 2, y + h + 7, { 255, 255, 255, 255 }, ALIGN_CENTER, L"| %im |", /*real*/( int )distance );
				}
			}
		}
	}

	for ( const auto& ammopack : entity_cache.world[ AMMO ] )
	{
		int x = 0, y = 0, w = 0, h = 0;
		vector percent48[ 8 ];

		if ( !ammopack || ammopack->IsDormant( ) )
		{
			continue;
		}

		if ( utils.get_draw_bounds( ammopack, percent48, x, y, w, h ) )
		{
			if ( local && !local->deadflag( ) && weapon )
			{
				if ( vars.global.legit_only && vars.esp.world.low_health_esp && !weapon->IsAmmoCritical( ) )
				{
					continue;
				}
			}

			if ( vars.esp.world.world_name )
			{
				draw.outlined_string( ESP, x + w / 2, y - 6, { 255, 255, 255, 255 }, ALIGN_CENTER, L"Ammopack" );
			}

			if ( vars.esp.world.world_box )
			{
				draw.black_outlined_rect( x, y, w, h + 1, vars.colors.esp.world.esp_world_ammo );
			}

			if ( vars.esp.world.world_distance )
			{
				if ( local && !local->deadflag( ) && !local->IsDormant( ) )
				{
					const vector localOrigin = local->GetAbsOrigin( );
					const vector ammoOrigin = ammopack->GetAbsOrigin( );

					float distance = sqrt( powf( localOrigin.x - ammoOrigin.x, 2 ) + powf( localOrigin.y - ammoOrigin.y, 2 ) + powf( localOrigin.z - ammoOrigin.z, 2 ) ) * 0.01905f;
					int realdistance = round( distance / 5 ) * 5;
					draw.outlined_string( ESPSMALL, x + w / 2, y + h + 7, { 255, 255, 255, 255 }, ALIGN_CENTER, L"| %im |", /*real*/( int )distance );
				}
			}
		}
	}

	for ( const auto& intelcase : entity_cache.intel[ TEAMSALL ] )
	{
		int x = 0, y = 0, w = 0, h = 0;
		vector percent48[ 8 ];

		if ( !intelcase || intelcase->IsDormant( ) )
		{
			continue;
		}

		const auto& intel = intelcase->As<C_CaptureFlag>( );
		if ( utils.get_draw_bounds( intelcase, percent48, x, y, w, h ) )
		{
			if ( intel->m_nFlagStatus( ) == TF_FLAGINFO_STOLEN )
				continue;

			y -= 2;
			int nTextOffset = 0;
			{
				draw.outlined_string( ESPVERYSMALL, x + w + 3, y + nTextOffset, { 255, 255, 255, 255 }, ALIGN_DEFAULT, intel->GetIntelStatus( intel->m_nFlagStatus( ) ) );
			}
			nTextOffset += fonts[ ESPVERYSMALL ].tall;
			if ( intel->m_nFlagStatus( ) == TF_FLAGINFO_DROPPED )
			{
				draw.outlined_string( ESPVERYSMALL, x + w + 3, y + nTextOffset, { 255, 255, 255, 255 }, ALIGN_DEFAULT, L"RETURNS IN %.1f SECS", ( intel->m_flResetTime( ) - i::global_vars->curtime ) );
			}
			if ( local && weapon )
			{
				if ( intel->m_iTeamNum( ) != local->m_iTeamNum( ) && local->IsUsingJumperWeapons( ) )
				{
					draw.outlined_string( ESPVERYSMALL, x + w + 3, y + nTextOffset, { 255, 25, 25, 255 }, ALIGN_DEFAULT, L"CANNOT PICK UP" );
				}
			}

			y += 2;

			if ( vars.esp.world.world_name )
			{
				draw.outlined_string( ESP, x + w / 2, y - 6, { 255, 255, 255, 255 }, ALIGN_CENTER, L"Intel" );
			}

			if ( vars.esp.world.world_box )
			{
				draw.black_outlined_rect( x, y, w, h + 1, colors.esp.get_entity_color( intel, local ) );
			}

			if ( vars.esp.world.world_distance )
			{
				if ( local && !local->deadflag( ) && !local->IsDormant( ) )
				{
					const vector localOrigin = local->GetAbsOrigin( );
					const vector intelOrigin = intel->GetAbsOrigin( );

					float distance = sqrt( powf( localOrigin.x - intelOrigin.x, 2 ) + powf( localOrigin.y - intelOrigin.y, 2 ) + powf( localOrigin.z - intelOrigin.z, 2 ) ) * 0.01905f;
					int realdistance = round( distance / 5 ) * 5;
					draw.outlined_string( ESPSMALL, x + w / 2, y + h + 7, { 255, 255, 255, 255 }, ALIGN_CENTER, L"| %im |", /*real*/( int )distance );
				}
			}
		}
	}

	for ( const auto& projectile : entity_cache.projectiles[ TEAMSALL ] )
	{
		int x = 0, y = 0, w = 0, h = 0;
		vector percent48[ 8 ];

		if ( !projectile || projectile->IsDormant( ) )
		{
			continue;
		}

		if ( projectile->GetClassID( ) == ETFClassID::CTFGrenadePipebombProjectile )
		{
			const auto& thrower = GET_ENT_H( projectile->As<C_TFGrenadePipebombProjectile>( )->m_hThrower( ) )->As<C_TFPlayer>( );
			if ( vars.esp.world.esp_world_enemy_only && thrower && thrower->m_iTeamNum( ) == local->m_iTeamNum( ) && thrower != local && !thrower->PlayerOnFriendsList( ) )
			{
				continue;
			}

			if ( !vars.esp.world.esp_world_local && thrower && thrower == local )
			{
				continue;
			}

			if ( !vars.esp.world.esp_world_friends && thrower && thrower->PlayerOnFriendsList( ) )
			{
				continue;
			}
		}
		else
		{
			if ( vars.esp.world.esp_world_enemy_only && projectile->m_hOwnerEntity( ).Get( ) && projectile->m_hOwnerEntity( ).Get( )->m_iTeamNum( ) == local->m_iTeamNum( ) && projectile->m_hOwnerEntity( ).Get( ) != local && !projectile->m_hOwnerEntity( ).Get( )->As<C_TFPlayer>( )->PlayerOnFriendsList( ) )
			{
				continue;
			}

			if ( !vars.esp.world.esp_world_local && projectile->m_hOwnerEntity( ).Get( ) && projectile->m_hOwnerEntity( ).Get( ) == local )
			{
				continue;
			}

			if ( !vars.esp.world.esp_world_friends && projectile->m_hOwnerEntity( ).Get( ) && projectile->m_hOwnerEntity( ).Get( )->As<C_TFPlayer>( )->PlayerOnFriendsList( ) )
			{
				continue;
			}
		}

		if ( utils.get_draw_bounds( projectile, percent48, x, y, w, h ) )
		{
			if ( vars.esp.world.world_name )
			{
				draw.outlined_string( ESP, x + w / 2, y - 6, { 255, 255, 255, 255 }, ALIGN_CENTER, utils.get_projectile_name( projectile ).c_str( ) );
			}

			if ( vars.esp.world.world_box )
			{
				draw.black_outlined_rect( x, y, w, h + 1, colors.esp.get_entity_color( projectile, local ) );
			}

			if ( vars.esp.world.world_distance )
			{
				if ( local && !local->deadflag( ) && !local->IsDormant( ) )
				{
					const vector localOrigin = local->GetAbsOrigin( );
					const vector intelOrigin = projectile->GetAbsOrigin( );

					float distance = sqrt( powf( localOrigin.x - intelOrigin.x, 2 ) + powf( localOrigin.y - intelOrigin.y, 2 ) + powf( localOrigin.z - intelOrigin.z, 2 ) ) * 0.01905f;
					int realdistance = round( distance / 5 ) * 5;
					draw.outlined_string( ESPSMALL, x + w / 2, y + h + 7, { 255, 255, 255, 255 }, ALIGN_CENTER, L"| %im |", /*real*/( int )distance );
				}
			}
		}
	}

	for ( const auto& fish : entity_cache.fish )
	{
		int x = 0, y = 0, w = 0, h = 0;
		vector percent48[ 8 ];

		if ( !fish || fish->IsDormant( ) )
		{
			continue;
		}

		if ( utils.get_draw_bounds( fish, percent48, x, y, w, h ) )
		{
			if ( vars.esp.world.world_name )
			{
				draw.outlined_string( ESP, x + w / 2, y - 6, { 255, 255, 255, 255 }, ALIGN_CENTER, L"Fish" );
			}

			if ( vars.esp.world.world_box )
			{
				draw.black_outlined_rect( x, y, w, h + 1, { 255, 255, 255, 255 } );
			}

			if ( vars.esp.world.world_distance )
			{
				if ( local && !local->deadflag( ) && !local->IsDormant( ) )
				{
					const vector localOrigin = local->GetAbsOrigin( );
					const vector intelOrigin = fish->GetAbsOrigin( );

					float distance = sqrt( powf( localOrigin.x - intelOrigin.x, 2 ) + powf( localOrigin.y - intelOrigin.y, 2 ) + powf( localOrigin.z - intelOrigin.z, 2 ) ) * 0.01905f;
					int realdistance = round( distance / 5 ) * 5;
					draw.outlined_string( ESPSMALL, x + w / 2, y + h + 7, { 255, 255, 255, 255 }, ALIGN_CENTER, L"| %im |", /*real*/( int )distance );
				}
			}
		}
	}
}

void __esp::DrawSentry( const vector& vecFromPos, const vector& vecToPos, color col, std::wstring name, int health, int maxhealth, int level, int distance )
{
	auto GetClockwiseAngle = [ & ] ( const vector& vecViewAngle, const vector& vecAimAngle ) -> float {
		auto vecAngle = vector( );
		math.angle_vectors( vecViewAngle, &vecAngle );

		auto vecAim = vector( );
		math.angle_vectors( vecAimAngle, &vecAim );

		return -atan2( vecAngle.x * vecAim.y - vecAngle.y * vecAim.x, vecAngle.x * vecAim.x + vecAngle.y * vecAim.y );
	};

	auto MapFloat = [ & ] ( float x, float in_min, float in_max, float out_min, float out_max ) -> float {
		return ( x - in_min ) * ( out_max - out_min ) / ( in_max - in_min ) + out_min;
	};

	const vector vecAngleTo = math.calc_angle( vecFromPos, vecToPos );
	const vector vecViewAngle = i::engine_client->GetViewAngles( );

	const float deg = GetClockwiseAngle( vecViewAngle, vecAngleTo );
	const float xrot = cos( deg - PI / 2 );
	const float yrot = sin( deg - PI / 2 );

	const float x1 = ( draw.screen_width * vars.esp.buildings.buildings_screen_distance * ( vars.esp.buildings.buildings_screen_shape ? ( ( float )draw.screen_width / ( float )draw.screen_height ) : 1.0f ) + 5.0f ) * xrot;
	const float y1 = ( draw.screen_width * vars.esp.buildings.buildings_screen_distance + 5.0f ) * yrot;
	const float x2 = ( draw.screen_width * vars.esp.buildings.buildings_screen_distance * ( vars.esp.buildings.buildings_screen_shape ? ( ( float )draw.screen_width / ( float )draw.screen_height ) : 1.0f ) + 15.0f ) * xrot;
	const float y2 = ( draw.screen_width * vars.esp.buildings.buildings_screen_distance + 15.0f ) * yrot;

	//constexpr float arrow_angle = DEG2RAD(60.0f);
	const float arrowAngle = DEG2RAD( 60.f );
	//constexpr float arrow_length = 20.0f;
	const float arrowLength = 40.f;

	const vector line{ x2 - x1, y2 - y1, 0.0f };
	const float length = line.length( );

	const float fpointOnLine = arrowLength / ( atanf( arrowAngle ) * length );
	const vector pointOnLine = vector( x2, y2, 0 ) + ( line * fpointOnLine * -1.0f );
	const vector normalVector{ -line.y, line.x, 0.0f };
	const vector normal = vector( arrowLength, arrowLength, 0.0f ) / ( length * 2 );

	const vector rotation = normal * normalVector;
	const vector left = pointOnLine + rotation;
	const vector right = pointOnLine - rotation;

	const auto cx = static_cast< float >( draw.screen_width / 2 );
	const auto cy = static_cast< float >( draw.screen_height / 2 );

	//float fMap = std::clamp(MapFloat(vecFromPos.DistTo(vecToPos), 1000.0f, 100.0f, 0.0f, 1.0f), 0.0f, 1.0f);
	const float fMap = std::clamp( MapFloat( vecFromPos.dist_to( vecToPos ), 2000.f,
								   5.f, 0.0f, 1.0f ), 0.0f, 1.0f );

	/*if ( Vars::Visuals::ArrowsDistanceAlpha.var )
		alpha = static_cast< byte >( fMap * 255.0f );*/

	color heatColor = { col.r, col.g, col.b, col.a };

	Vertex_t t1, t2, t3;
	t1.Init( { cx + left.x, cy + left.y } );
	t2.Init( { cx + right.x, cy + right.y } );
	t3.Init( { cx + x2, cy + y2 } );
	std::array<Vertex_t, 3> verts{ t1, t2, t3 };
	//draw.draw_textured_polygon( 3, verts.data( ), heatColor );

	/*draw.outlined_string( ESPVERYSMALL, cx + left.x, cy + left.y, { 255, 255, 255, 255 }, ALIGN_CENTER, L"1" );
	draw.outlined_string( ESPVERYSMALL, cx + left.x + ( ( right.x - left.x ) * 0.5f ), cy + left.y + ( ( right.y - left.y ) * 0.5f ), { 255, 255, 255, 255 }, ALIGN_CENTER, L"2", level );
	draw.outlined_string( ESPVERYSMALL, cx + right.x, cy + right.y, { 255, 255, 255, 255 }, ALIGN_CENTER, L"3" );
	draw.outlined_string( ESPVERYSMALL, cx + x2, cy + y2, { 255, 255, 255, 255 }, ALIGN_CENTER, L"4" );*/

	//draw.outlined_string( ESPVERYSMALL, cx + ( x2 * 0.9f ), cy + ( y2 * 0.9f ), { 255, 255, 255, 255 }, ALIGN_CENTER, L"LVL%i", level );

	float x = cx + ( x2 * 0.9f );
	float y = cy + ( y2 * 0.9f );
	draw.black_outlined_rect( x - 20, y - 20, 40, 40, heatColor );
	draw.outlined_string( ESP, x, y - 26, { 255, 255, 255, 255 }, ALIGN_CENTER, name.c_str( ) );
	draw.outlined_string( ESPVERYSMALL, x + 29, y - 18, { 255, 255, 255, 255 }, ALIGN_CENTER, L"LVL%i", level );
	if ( vars.esp.buildings.buildings_distance )
	{
		draw.outlined_string( ESPSMALL, x - 20 + 40.f / 2, y - 20 + 40 + 7, { 255, 255, 255, 255 }, ALIGN_CENTER, L"| %im |", distance );
	}

	int nHealth = health;
	int nMaxHealth = maxhealth;
	static const int nWidth = 2;

	float flHealth = ( float )nHealth;
	float flMaxHealth = ( float )nMaxHealth;

	float ratio = std::clamp( flHealth / flMaxHealth, 0.0f, 1.0f );
	int nHeight = 39 + ( flHealth < flMaxHealth ? 1 : 0 );

	color healthcol = utils.get_health_color( nHealth, nMaxHealth );

	draw.rect( x - 20 - nWidth - 3, y - 20, nWidth, nHeight * ( 1.0f - ratio ) + 1, { 0, 0, 0, 180 } );
	draw.rect( x - 20 - nWidth - 3, y - 20 + nHeight - ( nHeight * ratio ), nWidth, nHeight * ratio + 1, healthcol );
	draw.outlined_rect( x - 20 - nWidth - 4, y - 20 - 1, nWidth * 2, 39 + 3, { 0, 0, 0, 180 } );
}

void __esp::DrawPlayer( const vector& vecFromPos, const vector& vecToPos, color col, std::wstring name, std::wstring weapon, int health, int maxhealth, int distance )
{
	auto GetClockwiseAngle = [ & ] ( const vector& vecViewAngle, const vector& vecAimAngle ) -> float {
		auto vecAngle = vector( );
		math.angle_vectors( vecViewAngle, &vecAngle );

		auto vecAim = vector( );
		math.angle_vectors( vecAimAngle, &vecAim );

		return -atan2( vecAngle.x * vecAim.y - vecAngle.y * vecAim.x, vecAngle.x * vecAim.x + vecAngle.y * vecAim.y );
	};

	auto MapFloat = [ & ] ( float x, float in_min, float in_max, float out_min, float out_max ) -> float {
		return ( x - in_min ) * ( out_max - out_min ) / ( in_max - in_min ) + out_min;
	};

	const vector vecAngleTo = math.calc_angle( vecFromPos, vecToPos );
	const vector vecViewAngle = i::engine_client->GetViewAngles( );

	const float deg = GetClockwiseAngle( vecViewAngle, vecAngleTo );
	const float xrot = cos( deg - PI / 2 );
	const float yrot = sin( deg - PI / 2 );

	const float x1 = ( draw.screen_width * vars.esp.players.players_screen_distance * ( vars.esp.players.players_screen_shape ? ( ( float )draw.screen_width / ( float )draw.screen_height ) : 1.0f ) + 5.0f ) * xrot;
	const float y1 = ( draw.screen_width * vars.esp.players.players_screen_distance + 5.0f ) * yrot;
	const float x2 = ( draw.screen_width * vars.esp.players.players_screen_distance * ( vars.esp.players.players_screen_shape ? ( ( float )draw.screen_width / ( float )draw.screen_height ) : 1.0f ) + 15.0f ) * xrot;
	const float y2 = ( draw.screen_width * vars.esp.players.players_screen_distance + 15.0f ) * yrot;

	//constexpr float arrow_angle = DEG2RAD(60.0f);
	const float arrowAngle = DEG2RAD( 60.f );
	//constexpr float arrow_length = 20.0f;
	const float arrowLength = 40.f;

	const vector line{ x2 - x1, y2 - y1, 0.0f };
	const float length = line.length( );

	const float fpointOnLine = arrowLength / ( atanf( arrowAngle ) * length );
	const vector pointOnLine = vector( x2, y2, 0 ) + ( line * fpointOnLine * -1.0f );
	const vector normalVector{ -line.y, line.x, 0.0f };
	const vector normal = vector( arrowLength, arrowLength, 0.0f ) / ( length * 2 );

	const vector rotation = normal * normalVector;
	const vector left = pointOnLine + rotation;
	const vector right = pointOnLine - rotation;

	const auto cx = static_cast< float >( draw.screen_width / 2 );
	const auto cy = static_cast< float >( draw.screen_height / 2 );

	//float fMap = std::clamp(MapFloat(vecFromPos.DistTo(vecToPos), 1000.0f, 100.0f, 0.0f, 1.0f), 0.0f, 1.0f);
	const float fMap = std::clamp( MapFloat( vecFromPos.dist_to( vecToPos ), 2000.f,
								   5.f, 0.0f, 1.0f ), 0.0f, 1.0f );

	/*if ( Vars::Visuals::ArrowsDistanceAlpha.var )
		alpha = static_cast< byte >( fMap * 255.0f );*/

	color heatColor = { col.r, col.g, col.b, col.a };

	Vertex_t t1, t2, t3;
	t1.Init( { cx + left.x, cy + left.y } );
	t2.Init( { cx + right.x, cy + right.y } );
	t3.Init( { cx + x2, cy + y2 } );
	std::array<Vertex_t, 3> verts{ t1, t2, t3 };
	//draw.draw_textured_polygon( 3, verts.data( ), heatColor );

	/*draw.outlined_string( ESPVERYSMALL, cx + left.x, cy + left.y, { 255, 255, 255, 255 }, ALIGN_CENTER, L"1" );
	draw.outlined_string( ESPVERYSMALL, cx + left.x + ( ( right.x - left.x ) * 0.5f ), cy + left.y + ( ( right.y - left.y ) * 0.5f ), { 255, 255, 255, 255 }, ALIGN_CENTER, L"2", level );
	draw.outlined_string( ESPVERYSMALL, cx + right.x, cy + right.y, { 255, 255, 255, 255 }, ALIGN_CENTER, L"3" );
	draw.outlined_string( ESPVERYSMALL, cx + x2, cy + y2, { 255, 255, 255, 255 }, ALIGN_CENTER, L"4" );*/

	//draw.outlined_string( ESPVERYSMALL, cx + ( x2 * 0.9f ), cy + ( y2 * 0.9f ), { 255, 255, 255, 255 }, ALIGN_CENTER, L"LVL%i", level );

	float x = cx + ( x2 * 0.9f );
	float y = cy + ( y2 * 0.9f );
	draw.black_outlined_rect( x - 20, y - 20, 40, 40, heatColor );
	draw.outlined_string( ESP, x, y - 26, { 255, 255, 255, 255 }, ALIGN_CENTER, name.c_str( ) );

	int esptext = 0;
	if ( vars.esp.players.players_weapons )
	{
		draw.outlined_string( ESPSMALL, x - 20 + 40.f / 2, y - 20 + 40 + 7, { 255, 255, 255, 255 }, ALIGN_CENTER, weapon.c_str( ) );
		esptext += fonts[ ESPSMALL ].tall;
	}

	if ( vars.esp.players.players_distance )
	{
		draw.outlined_string( ESPSMALL, x - 20 + 40.f / 2, y - 20 + 40 + 7 + esptext, { 255, 255, 255, 255 }, ALIGN_CENTER, L"| %im |", distance );
		esptext += fonts[ ESPSMALL ].tall;
	}

	int nHealth = health;
	int nMaxHealth = maxhealth;
	static const int nWidth = 2;

	float flHealth = ( float )nHealth;
	float flMaxHealth = ( float )nMaxHealth;

	float ratio = std::clamp( flHealth / flMaxHealth, 0.0f, 1.0f );
	int nHeight = 39 + ( flHealth < flMaxHealth ? 1 : 0 );

	color healthcol = utils.get_health_color( nHealth, nMaxHealth );

	draw.rect( x - 20 - nWidth - 3, y - 20, nWidth, nHeight * ( 1.0f - ratio ) + 1, { 0, 0, 0, 180 } );
	draw.rect( x - 20 - nWidth - 3, y - 20 + nHeight - ( nHeight * ratio ), nWidth, nHeight * ratio + 1, healthcol );
	draw.outlined_rect( x - 20 - nWidth - 4, y - 20 - 1, nWidth * 2, 39 + 3, { 0, 0, 0, 180 } );
}

void __esp::render( )
{
	const auto& local = entity_cache.get_local( );
	const auto& weapon = entity_cache.get_weapon( );

	if ( !i::engine_client->IsInGame( ) || i::engine_vgui->IsGameUIVisible( ) || i::engine_client->Con_IsVisible( ) )
		return;

	render_world( local, weapon );
	render_buildings( local );
	render_players( local );
}