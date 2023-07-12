#include "indicators.h"
#include "../crits/crits.h"
#include "../antiaim/antiaim.h"
#include "../backtrack/backtrack.h"

void __indicators::draw_dt( C_TFPlayer* local, C_TFWeaponBase* weapon )
{
	if ( !vars.misc.cl_move.dt_indicator )
	{
		return;
	}

	draw.rect( ( draw.screen_width * 0.5f ) - 68, draw.screen_height * 0.62f + 1, 136, 16, { 40, 40, 40, 150 } );
	draw.outlined_rect( ( draw.screen_width * 0.5f ) - 69, draw.screen_height * 0.62f, 138, 18, vars.colors.menu.menu_accent );
	//draw.outlined_rect( ( draw.screen_width * 0.5f ) - 70, draw.screen_height * 0.55f - 1, 140, 18, vars.colors.menu.menu_background2 );

	for ( int i = 0; i < global_info.shifted; i++ )
	{
		draw.rect( ( draw.screen_width * 0.5f ) - 62 + i * 6, draw.screen_height * 0.62f + 3, 1, 12, vars.colors.menu.menu_accent );
		draw.rect( ( draw.screen_width * 0.5f ) - 65 + i * 6, draw.screen_height * 0.62f + 3, 1, 12, vars.colors.menu.menu_accent );
	}

	draw.outlined_string( ESP, draw.screen_width * 0.5f, draw.screen_height * 0.62f - 12, { 255, 255, 255, 255 }, ALIGN_CENTERHORIZONTAL, L"Doubletap" );
	draw.outlined_string( ESP, draw.screen_width * 0.5f, draw.screen_height * 0.62f + 18, { 255, 255, 255, 255 }, ALIGN_CENTERHORIZONTAL, L"%i/22", global_info.shifted );
}

void __indicators::draw_crits( C_TFPlayer* local, C_TFWeaponBase* weapon )
{
	/*draw.outlined_string( ESP, 5, 576, { 255, 255, 255, 255 }, ALIGN_DEFAULT, L"m_flNextPrimaryAttack: %.2f", weapon->m_flNextPrimaryAttack( ) );
	draw.outlined_string( ESP, 5, 588, { 255, 255, 255, 255 }, ALIGN_DEFAULT, L"curtime: %.2f", local->m_nTickBase( ) * i::global_vars->interval_per_tick );*/
	if ( vars.debug.crit )
	{
		draw.outlined_string( ESP, 5, 300, { 255, 255, 255, 255 }, ALIGN_DEFAULT, L"can_crit: %i", crithack.can_crit );
		draw.outlined_string( ESP, 5, 312, { 255, 255, 255, 255 }, ALIGN_DEFAULT, L"crit_banned: %i", crithack.crit_banned );
		draw.outlined_string( ESP, 5, 324, { 255, 255, 255, 255 }, ALIGN_DEFAULT, L"last_hit_registered_melee: %i", crithack.last_hit_registered_melee );
		draw.outlined_string( ESP, 5, 336, { 255, 255, 255, 255 }, ALIGN_DEFAULT, L"total_round_melee_damage: %i", crithack.total_round_melee_damage );
		draw.outlined_string( ESP, 5, 348, { 255, 255, 255, 255 }, ALIGN_DEFAULT, L"crit_damage_til_unban: %i", crithack.crit_damage_til_unban );
		draw.outlined_string( ESP, 5, 360, { 255, 255, 255, 255 }, ALIGN_DEFAULT, L"boostracist: %i", crithack.boostracist );
		draw.outlined_string( ESP, 5, 372, { 255, 255, 255, 255 }, ALIGN_DEFAULT, L"Crematorium1488: %i", crithack.Crematorium1488 );
		draw.outlined_string( ESP, 5, 384, { 255, 255, 255, 255 }, ALIGN_DEFAULT, L"Yid: %i", crithack.Yid );
		draw.outlined_string( ESP, 5, 396, { 255, 255, 255, 255 }, ALIGN_DEFAULT, L"crit_mult: %.2f", crithack.crit_mult );
		draw.outlined_string( ESP, 5, 408, { 255, 255, 255, 255 }, ALIGN_DEFAULT, L"crit_chance_correct: %.2f", crithack.crit_chance_correct );
		draw.outlined_string( ESP, 5, 420, { 255, 255, 255, 255 }, ALIGN_DEFAULT, L"crits: %i", crithack.crits[ SLOT_PRIMARY ] );
		draw.outlined_string( ESP, 5, 432, { 255, 255, 255, 255 }, ALIGN_DEFAULT, L"crits: %i", crithack.crits[ SLOT_SECONDARY ] );
		draw.outlined_string( ESP, 5, 444, { 255, 255, 255, 255 }, ALIGN_DEFAULT, L"crits: %i", crithack.crits[ SLOT_MELEE ] );
		draw.outlined_string( ESP, 5, 456, { 255, 255, 255, 255 }, ALIGN_DEFAULT, L"potential_crits: %i", crithack.potential_crits[ SLOT_PRIMARY ] );
		draw.outlined_string( ESP, 5, 468, { 255, 255, 255, 255 }, ALIGN_DEFAULT, L"potential_crits: %i", crithack.potential_crits[ SLOT_SECONDARY ] );
		draw.outlined_string( ESP, 5, 480, { 255, 255, 255, 255 }, ALIGN_DEFAULT, L"potential_crits: %i", crithack.potential_crits[ SLOT_MELEE ] );
		draw.outlined_string( ESP, 5, 492, { 255, 255, 255, 255 }, ALIGN_DEFAULT, L"m_flCritTokenBucket: %i", crithack.crit_token_bucket );
		draw.outlined_string( ESP, 5, 504, { 255, 255, 255, 255 }, ALIGN_DEFAULT, L"damage_correct: %i", crithack.damage_correct[ SLOT_PRIMARY ] );
		draw.outlined_string( ESP, 5, 516, { 255, 255, 255, 255 }, ALIGN_DEFAULT, L"damage_correct: %i", crithack.damage_correct[ SLOT_SECONDARY ] );
		draw.outlined_string( ESP, 5, 528, { 255, 255, 255, 255 }, ALIGN_DEFAULT, L"damage_correct: %i", crithack.damage_correct[ SLOT_MELEE ] );
		draw.outlined_string( ESP, 5, 540, { 255, 255, 255, 255 }, ALIGN_DEFAULT, L"crit_time: %.2f", weapon->m_flLastCritCheckTime( ) );
		draw.outlined_string( ESP, 5, 552, { 255, 255, 255, 255 }, ALIGN_DEFAULT, L"real time: %.2f", weapon->m_flLastRapidFireCritCheckTime( ) );
		draw.outlined_string( ESP, 5, 564, { 255, 255, 255, 255 }, ALIGN_DEFAULT, L"cost: %.2f", crithack.cost );
		draw.outlined_string( ESP, 5, 576, { 255, 255, 255, 255 }, ALIGN_DEFAULT, L"bucket: %i", crithack.bucket[ SLOT_PRIMARY ] );
		draw.outlined_string( ESP, 5, 588, { 255, 255, 255, 255 }, ALIGN_DEFAULT, L"bucket: %i", crithack.bucket[ SLOT_SECONDARY ] );
		draw.outlined_string( ESP, 5, 600, { 255, 255, 255, 255 }, ALIGN_DEFAULT, L"bucket: %i", crithack.bucket[ SLOT_MELEE ] );
		draw.outlined_string( ESP, 5, 612, { 255, 255, 255, 255 }, ALIGN_DEFAULT, L"bucket: %.2f", weapon->m_flCritTokenBucket( ) );
	}

	if ( vars.misc.crits.crits_active && vars.misc.crits.crits_indicator && crithack.weapon_can_crit( weapon ) )
	{
		if ( local->IsCritBoosted( ) )
		{
			draw.outlined_string( INDICATOR, draw.screen_width * 0.5f, draw.screen_height * 0.57f, { 60, 200, 255, 255 }, ALIGN_CENTERHORIZONTAL, L"crit boosted" );
		}
		else
		{
			color crit = crithack.crits[ weapon->GetSlot( ) ] == 0 || crithack.potential_crits[ weapon->GetSlot( ) ] == 0 ? color{ 255, 25, 25, 255 } : color{ 25, 255, 25, 255 };

			if ( !crithack.crit_banned )
			{
				if ( crithack.potential_crits[ weapon->GetSlot( ) ] > 0 )
				{
					draw.outlined_string( INDICATOR, draw.screen_width * 0.5f, draw.screen_height * 0.57f, crit, ALIGN_CENTERHORIZONTAL, L"%i/%i potential crits", crithack.crits[ weapon->GetSlot( ) ], crithack.potential_crits[ weapon->GetSlot( ) ] );
				}
				else
				{
					draw.outlined_string( INDICATOR, draw.screen_width * 0.5f, draw.screen_height * 0.57f, { 255, 255, 25, 255 }, ALIGN_CENTERHORIZONTAL, L"calculating" );
				}
			}
			else
			{
				draw.outlined_string( INDICATOR, draw.screen_width * 0.5f, draw.screen_height * 0.57f, { 255, 25, 25, 255 }, ALIGN_CENTERHORIZONTAL, L"crit banned" );
				std::wstring damage = crithack.crit_damage_til_unban >= 2000 ? L"%i+ damage required" : L"%i damage required";
				draw.outlined_string( INDICATOR, draw.screen_width * 0.5f, draw.screen_height * 0.57f + 12, { 255, 255, 25, 255 }, ALIGN_CENTERHORIZONTAL, damage.c_str( ), crithack.crit_damage_til_unban );
			}
		}
	}
}

void __indicators::draw_aa( C_TFPlayer* local )
{
	int nTextOffset = 0;

	if ( ( vars.antiaim.antiaim_active ) && !local->deadflag( ) && antiaim.toggle )
	{
		static constexpr color realColour = { 0, 255, 0, 255 };
		static constexpr color fakeColour = { 255, 0, 0, 255 };

		const auto& vOrigin = local->GetAbsOrigin( );

		vector vScreen1, vScreen2;
		if ( utils.w2s( vOrigin, vScreen1 ) /* && !g_GlobalInfo.m_bAvoidingBackstab */ && i::input->CAM_IsThirdPerson( ) )
		{
			constexpr auto distance = 50.f;
			if ( utils.w2s( utils.get_rotated_position( vOrigin, global_info.real_viewangles.y, distance ), vScreen2 ) )
			{
				draw.line( vScreen1.x, vScreen1.y, vScreen2.x, vScreen2.y, realColour );
				draw.outlined_string( ESPVERYSMALL, vScreen2.x, vScreen2.y - 4, { 255, 255, 255, 255 }, ALIGN_CENTER, L"REAL" );
			}

			if ( utils.w2s( utils.get_rotated_position( vOrigin, global_info.fake_viewangles.y, distance ), vScreen2 ) )
			{
				draw.line( vScreen1.x, vScreen1.y, vScreen2.x, vScreen2.y, fakeColour );
				draw.outlined_string( ESPVERYSMALL, vScreen2.x, vScreen2.y - 4, { 255, 255, 255, 255 }, ALIGN_CENTER, L"FAKE" );
			}

		}
	}
}

void __indicators::draw_bt( C_TFPlayer* local )
{
	if ( !backtrack.best_target.player || backtrack.best_target.player->deadflag( ) || !vars.backtrack.backtrack_active || !vars.backtrack.debug )
	{
		return;
	}

	if ( backtrack.best_target.player == nullptr )
	{
		return;
	}

	vector screen, screen2;
	vector pos = backtrack.best_target.player->GetHitboxPosition( vars.aimbot.hitscan.hitscan_hitbox );
	vector hitbox = vars.aimbot.hitscan.hitscan_hitbox == HITBOX_HEAD ? backtrack.best_data.headpos : backtrack.best_data.bodypos;

	if ( utils.vis_pos( local, backtrack.best_target.player, local->GetShootPosition( ), pos ) )
	{
		utils.w2s( pos, screen );
		draw.line( draw.screen_width / 2, draw.screen_height, screen.x, screen.y, { 0, 0, 255, 255 } );
	}

	if ( utils.vis_pos( local, backtrack.best_target.player, local->GetShootPosition( ), hitbox ) )
	{
		if ( !hitbox.is_zero( ) )
		{
			utils.w2s( hitbox, screen2 );
			draw.line( draw.screen_width / 2, draw.screen_height, screen2.x, screen2.y, { 255, 0, 0, 255 } );
		}
	}
}

bool __indicators::get_spectators( C_TFPlayer* local )
{
	m_Spectators.clear( );

	for ( const auto& player : entity_cache.players[ TEAMSALL ] )
	{
		const auto& observed_player = GET_ENT_H( player->m_hObserverTarget( ) )->As<C_TFPlayer>( );

		if ( player && player->deadflag( ) && observed_player && !observed_player->deadflag( ) )
		{
			color color = { 255, 255, 255, 255 };
			switch ( player->m_iObserverMode( ) )
			{
				case OBS_MODE_FIRSTPERSON:
				{
					if ( observed_player == local )
					{
						color = { 255, 50, 50, 255 };
					}
					else
					{
						color = { 255, 150, 50, 255 };
					}
					break;
				}
				case OBS_MODE_THIRDPERSON:
				{
					color = { 255, 255, 255, 255 };
					break;
				}
				default: continue;
			}

			if ( player->PlayerOnFriendsList( ) )
				color = { 150, 255, 150, 255 };

			m_Spectators.push_back( { player, observed_player, color } );
		}
	}

	return !m_Spectators.empty( );
}

void __indicators::draw_spectators( C_TFPlayer* local )
{
	if ( !get_spectators( local ) )
	{
		return; 
	}

	int text_offset = 0;
	draw.outlined_string( ESP, 5, 260, { 255, 255, 255, 255 }, ALIGN_DEFAULT, L"Spectators:" );
	for ( size_t i = 0; i < m_Spectators.size( ); i++ )
	{
		const auto& observer = m_Spectators[ i ].observer;
		const auto& observed = m_Spectators[ i ].observed;
		PlayerInfo_t obsrvd, obsrvr;

		if ( !i::engine_client->GetPlayerInfo( observer->entindex( ), &obsrvr ) || !i::engine_client->GetPlayerInfo( observed->entindex( ), &obsrvd ) )
		{
			return;
		}

		std::wstring observer_name = utils.utf8_to_wide( obsrvr.name );
		std::wstring observed_name = utils.utf8_to_wide( obsrvd.name );

		if ( observer_name.length( ) > 32 )
		{
			observer_name.replace( 32, observer_name.length( ), ( L"..." ) );
		}

		if ( observed_name.length( ) > 32 )
		{
			observed_name.replace( 32, observed_name.length( ), ( L"..." ) );
		}

		/*int wide, tall;
		std::wstring name = m_Spectators[ i ].observer + L" -> " + m_Spectators[ i ].observed;
		I::Surface->GetTextSize( g_Fonts[ EFonts::NAME ].m_dwFont, name.c_str( ), wide, tall );
		g_Draw.OutlinedString( g_Fonts[ EFonts::NAME ].m_dwFont, 5, 200 + nTextOffset, m_Spectators[ i ].color, ALIGN_DEFAULT, name.c_str( ) );
		nTextOffset += g_Fonts[ EFonts::NAME ].m_nTall;*/

		std::wstring spectator = observer_name + L" -> " + observed_name;
		draw.outlined_string( ESP, 5, 272 + text_offset, m_Spectators[ i ].color, ALIGN_DEFAULT, spectator.c_str( ) );
		text_offset += 12;
	}
}

void __indicators::render( )
{
	const auto& local = entity_cache.get_local( );
	const auto& weapon = entity_cache.get_weapon( );

	if ( !local || !weapon )
	{
		return;
	}

	if ( i::engine_vgui->IsGameUIVisible( ) || !i::engine_client->IsConnected( ) || !i::engine_client->IsInGame( ) )
	{
		return;
	}

	draw_spectators( local );

	if ( local->deadflag( ) )
	{
		return;
	}

	draw_dt( local, weapon );
	draw_crits( local, weapon );
	draw_aa( local );
	draw_bt( local );
}