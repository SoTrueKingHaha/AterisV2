#include "crits.h"

bool __crits::AreRandomCritsEnabled( )
{
	static auto tf_weapon_criticals = convars.find_var( "tf_weapon_criticals" );
	if ( tf_weapon_criticals )
	{
		return tf_weapon_criticals->GetBool( );
	}
	return true;
}

bool __crits::IsEnabled( )
{
	if ( !vars.misc.crits.crits_active ) { return false; }
	if ( !AreRandomCritsEnabled( ) ) { return false; }
	if ( !i::engine_client->IsInGame( ) ) { return false; }

	return true;
}

int __crits::LastGoodCritTick( const CUserCmd* pCmd )
{
	int retVal = -1;
	bool popBack = false;

	for ( const auto& cmd : force_cmds )
	{
		if ( cmd >= pCmd->command_number )
		{
			retVal = cmd;
		}
		else
		{
			popBack = true;
		}
	}

	if ( popBack )
	{
		force_cmds.pop_back( );
	}

	return retVal;
}

int __crits::LastGoodSkipTick( const CUserCmd* pCmd )
{
	int retVal = -1;
	bool popBack = false;

	for ( const auto& cmd : skip_cmds )
	{
		if ( cmd >= pCmd->command_number )
		{
			retVal = cmd;
		}
		else
		{
			popBack = true;
		}
	}

	if ( popBack )
	{
		skip_cmds.pop_back( );
	}

	return retVal;
}

bool __crits::IsAttacking( const CUserCmd* pCmd, C_TFWeaponBase* pWeapon )
{
	if ( pWeapon->m_iItemDefinitionIndex( ) == Soldier_m_TheBeggarsBazooka )
	{
		static bool bLoading = false;

		if ( pWeapon->m_iClip1( ) > 0 )
		{
			bLoading = true;
		}

		if ( !( pCmd->buttons & IN_ATTACK ) && bLoading ) {
			bLoading = false;
			return true;
		}
	}
	else
	{
		const int id = pWeapon->GetWeaponID( );
		switch ( id ) {
			case TF_WEAPON_COMPOUND_BOW:
			case TF_WEAPON_PIPEBOMBLAUNCHER:
			case TF_WEAPON_STICKY_BALL_LAUNCHER:
			case TF_WEAPON_GRENADE_STICKY_BALL:
			case TF_WEAPON_CANNON:
			{
				static bool bCharging = false;

				if ( pWeapon->As<C_TFPipebombLauncher>( )->m_flChargeBeginTime( ) > 0.0f )
					bCharging = true;

				if ( !( pCmd->buttons & IN_ATTACK ) && bCharging ) {
					bCharging = false;
					return true;
				}
				break;
			}
			case TF_WEAPON_JAR:
			case TF_WEAPON_JAR_MILK:
			case TF_WEAPON_JAR_GAS:
			case TF_WEAPON_GRENADE_JAR_GAS:
			case TF_WEAPON_CLEAVER:
			{
				static float flThrowTime = 0.0f;

				if ( ( pCmd->buttons & IN_ATTACK ) && global_info.can_attack && !flThrowTime )
				{
					flThrowTime = i::global_vars->curtime + i::global_vars->interval_per_tick;
				}

				if ( flThrowTime && i::global_vars->curtime >= flThrowTime ) {
					flThrowTime = 0.0f;
					return true;
				}
				break;
			}
			case TF_WEAPON_MINIGUN:
			{
				const auto& minigun = pWeapon->As<CTFMinigun>( );
				if ( minigun->m_iWeaponState( ) == AC_STATE_FIRING )
				{
					return true;
				}
				break;
			}
			default:
			{
				if ( ( pCmd->buttons & IN_ATTACK ) && global_info.can_attack )
				{
					return true;
				}
				break;
			}
		}
	}

	return false;
}

bool __crits::CalcIsAttackCriticalHandler( C_TFPlayer* local, C_TFWeaponBase* weapon )
{
	if ( i::prediction->m_bFirstTimePredicted )
	{
		return false;
	}

	if ( !weapon || local )
	{
		return false;
	}

	{
		static int s_nPreviousTickcount = 0;

		if ( s_nPreviousTickcount == i::global_vars->tickcount )
		{
			return false;
		}

		s_nPreviousTickcount = i::global_vars->tickcount;
	}

	{
		if ( weapon->GetWeaponID( ) == TF_WEAPON_MINIGUN ||
			 weapon->GetWeaponID( ) == TF_WEAPON_FLAMETHROWER )
		{
			auto nPreviousAmmoCount = local->GetAmmoCount( weapon->m_iPrimaryAmmoType( ) );
			static auto nNewAmmoCount = nPreviousAmmoCount;

			const auto bHasFiredBullet = nNewAmmoCount != nPreviousAmmoCount;

			if ( !bHasFiredBullet )
			{
				return false;
			}
		}
	}

	if ( wish_random_seed != 0 )
	{
		*i::random_seed = wish_random_seed;
		wish_random_seed = 0;
	}

	return true;
}

void __crits::fill( C_TFPlayer* local, C_TFWeaponBase* weapon, const CUserCmd* pCmd, int loops )
{
	static int prev_weapon = 0;
	static int previousCrit = 0;
	static int starting_num = pCmd->command_number;

	if ( global_info.attacking || utils.is_attacking( pCmd, local, weapon )/* || pCmd->buttons & IN_ATTACK*/ )
	{
		return;
	}

	if ( prev_weapon != weapon->entindex( ) )
	{
		starting_num = pCmd->command_number;
		prev_weapon = weapon->entindex( );
		force_cmds.clear( );
		skip_cmds.clear( );
	}
	
	racist = true;
	const int seed_backup = MD5_PseudoRandom( pCmd->command_number ) & 0x7FFFFFFF;
	for ( int i = 0; i < loops; i++ )
	{
		const int cmd_num = starting_num + i;
		*i::random_seed = wish_random_seed = MD5_PseudoRandom( cmd_num ) & 0x7FFFFFFF;
		if ( force_cmds.size( ) < 15 && is_pure_crit_command( cmd_num ) )
		{
			force_cmds.push_back( cmd_num );
		}
		else if ( skip_cmds.size( ) < 15 && !is_pure_crit_command( cmd_num ) )
		{
			skip_cmds.push_back( cmd_num );
		}

		if ( force_cmds.size( ) >= 15 || skip_cmds.size( ) >= 15 )
		{
			break;
		}
	}
	starting_num += loops;
	racist = false;
	*reinterpret_cast< int* >( reinterpret_cast< DWORD >( weapon ) + 0xA5C ) = 0;
	*i::random_seed = seed_backup;
}

void __crits::run( C_TFPlayer* local, C_TFWeaponBase* weapon, CUserCmd* pCmd )
{
	if ( !IsEnabled( ) ) { return; }
	const auto& resource = entity_cache.get_resource( );
	if ( !local || !weapon || !resource || local->deadflag( ) || local->IsCritBoosted( ) || !weapon->CanFireCriticalShot( false ) ) { return; }
	bool pressed = ( GetAsyncKeyState( vars.misc.crits.crit_key ) & 0x8000 );
	static int weapon_index = weapon->entindex( );

	if ( weapon->m_nCritSeedRequests( ) > 0 && weapon->m_nCritChecks( ) > 0 )
		crithack.crit_mult = weapon->GetSlot( ) == SLOT_MELEE ? 0.5f : math.remap_val_clamped( static_cast< float >( weapon->m_nCritSeedRequests( ) ) / static_cast< float >( weapon->m_nCritChecks( ) ), 0.1f, 1.f, 1.f, 3.f );
	else
		crithack.crit_mult = 1.f;

	update_damage( local, weapon, resource );
	if ( !global_info.shifting )
	{
		get_damage_til_unban( );
		fill( local, weapon, pCmd, 15 ); //	fill our vector slowly.
	}

	if ( damage_correct[ weapon->GetSlot( ) ] == 0 /* || weapon_index != weapon->entindex( )*/ )
	{
		weapon->WillCrit( );
		get_total_crits( local, weapon );
		//weapon_index = weapon->entindex( );
	}

	/*if ( crit_banned && crit_damage_til_unban == 0 )
	{
		crit_banned = false;
	}*/

	const int closest_crit = LastGoodCritTick( pCmd ); //	retrieve our wish
	const int closest_skip = LastGoodSkipTick( pCmd ); //	retrieve our wish
	if ( IsAttacking( pCmd, weapon ) && !crit_banned && !global_info.shifting ) //	is it valid & should we even use it
	{
		if ( pressed )
		{
			racist = true;
			if ( closest_crit < 0 ) { return; }
			pCmd->command_number = closest_crit;
			wish_random_seed = MD5_PseudoRandom( closest_crit ) & 0x7FFFFFFF;
			racist = false;
			if ( weapon->IsStreamingWeapon( ) )
			{
				bucket[ weapon->GetSlot( ) ] = 0;
			}
			else
			{
				bucket[ weapon->GetSlot( ) ] = std::max( bucket[ weapon->GetSlot( ) ] - ( int )crithack.cost, 0 );
			}
		}
		else if ( vars.misc.crits.avoid_random ) //	we don't want to crit
		{
			if ( closest_skip < 0 ) { return; }
			pCmd->command_number = closest_skip; //	set our cmdnumber to our wish
			wish_random_seed = MD5_PseudoRandom( closest_skip ) & 0x7FFFFFFF;
			if ( !is_pure_crit_command( closest_skip ) || is_pure_crit_command( pCmd->command_number ) )
			{
				for ( int tries = 1; tries < 25; tries++ )
				{
					if ( std::find( force_cmds.begin( ), force_cmds.end( ), pCmd->command_number + tries ) != force_cmds.end( ) )
					{
						continue; //	what a useless attempt
					}
					pCmd->command_number += tries;
					wish_random_seed = MD5_PseudoRandom( pCmd->command_number ) & 0x7FFFFFFF;
					break; //	we found a seed that we can use to avoid a crit and have skipped to it, woohoo
				}
			}

			if ( global_info.can_attack )
			{
				bucket[ weapon->GetSlot( ) ] = std::min( bucket[ weapon->GetSlot( ) ] + ( int )crithack.damage_correct[ weapon->GetSlot( ) ], 1000 );
			}
		}
		else
		{
			if ( global_info.can_attack )
			{
				bucket[ weapon->GetSlot( ) ] = std::min( bucket[ weapon->GetSlot( ) ] + ( int )crithack.damage_correct[ weapon->GetSlot( ) ], 1000 );
			}
		}
	}
	else if ( vars.misc.crits.avoid_random ) //	we don't want to crit
	{
		if ( closest_skip < 0 ) { return; }
		if ( !is_pure_crit_command( closest_skip ) || is_pure_crit_command( pCmd->command_number ) )
		{
			pCmd->command_number = closest_skip; //	set our cmdnumber to our wish
			wish_random_seed = MD5_PseudoRandom( closest_skip ) & 0x7FFFFFFF;
		}
	}

	if ( !global_info.shifting )
	{
		get_total_crits( local, weapon );
	}
}

u32 __crits::decrypt_or_encrypt_seed( C_TFPlayer* local, C_TFWeaponBase* weapon, u32 seed )
{
	if ( !weapon )
	{
		return 0;
	}

	unsigned int iMask = weapon->entindex( ) << 8 | local->entindex( );

	if ( weapon->GetSlot( ) == SLOT_MELEE )
	{
		iMask <<= 8;
	}

	return iMask ^ seed;
}

#define TF_DAMAGE_CRIT_CHANCE_RAPID		0.02f
#define TF_DAMAGE_CRIT_DURATION_RAPID	2.0f
bool __crits::is_pure_crit_command( const i32 command_number ) {

	const auto& local = entity_cache.get_local( );
	const auto& weapon = entity_cache.get_weapon( );
	if ( !local || !weapon )
	{
		return false;
	}
	
	{
		float multiplier = crit_mult;
		multiplier *= 100;

		const auto random_seed = MD5_PseudoRandom( command_number ) & 0x7FFFFFFF;
		math.random_seed( decrypt_or_encrypt_seed( local, weapon, random_seed ) );

		//return math.random_int( 0, 9999 ) < ( 0.01f * 10000 );
		return math.random_int( 0, 9999 ) < multiplier && math.random_int( 0, 9999 ) != 0;
	}

	return false;
}

void __crits::reset( )
{
	racist = false;
	can_crit = false;
	last_hit_registered_melee = false;
	crit_banned = false;
	total_round_melee_damage = 0;
	crit_damage_til_unban = 0;
	boostracist = 0;
	Crematorium1488 = 0;
	Yid = 0;
	damage_correct = { 0, 0, 0 };
	crit_chance_correct = 0.f;
	cost = 0.f;
	crithack.potential_crits = { 0, 0, 0 };
	crithack.crits = { 0, 0, 0 };
	crithack.bucket = { 300, 300, 300 };
}

void __crits::can_fire_critical_shot_handler( float crit_chance )
{
	const auto& local = entity_cache.get_local( );
	const auto& weapon = entity_cache.get_weapon( );
	if ( !local || !weapon || local->deadflag( ) )
	{
		return;
	}

	weapon->SetObservedCritChance( 0.f );
	crit_chance_correct = 0.f;
	crit_banned = false;

	const auto racist = Crematorium1488;
	const auto fag = Yid - total_round_melee_damage;

	if ( fag <= 0.f || racist <= 0.f || weapon->GetSlot( ) == SLOT_MELEE )
	{
		return;
	}

	const auto hebrew_racist = racist / 3.f;
	const auto pizza = hebrew_racist + ( fag - racist );

	if ( pizza <= 0.f || hebrew_racist <= 0.f )
	{
		return;
	}

	weapon->SetObservedCritChance( hebrew_racist / pizza );

	crit_chance_correct = crit_chance + 0.1f;
	crit_banned = weapon->m_flObservedCritChance( ) >= crit_chance_correct;
}

void __crits::get_damage_til_unban( )
{
	crit_damage_til_unban = 0;

	const auto racist = Crematorium1488;
	const auto fag = Yid - total_round_melee_damage;

	if ( !crit_banned || racist <= 0.f )
		return;

	u32 damage_required = 0;
	for ( ; damage_required < 2000; damage_required++ ) {
		const auto hebrew_racist = racist * 0.33333333333f;
		const auto pizza = hebrew_racist + ( fag + static_cast< float >( damage_required ) - racist );

		if ( pizza <= 0.f || hebrew_racist <= 0.f )
			return;

		if ( hebrew_racist / pizza < crit_chance_correct )
			break;
	}

	crit_damage_til_unban = damage_required;
}

void __crits::update_damage( C_TFPlayer* local, C_TFWeaponBase* weapon, C_TFPlayerResource* resource )
{
	if ( !local || !resource || local->deadflag( ) )
	{
		return;
	}

	Yid = resource->get_damage( local->entindex( ) );

	if ( Yid <= 0 )
	{
		total_round_melee_damage = 0;
		Crematorium1488 = 0;
		boostracist = 0;
	}
}

bool __crits::get_total_crits( C_TFPlayer* local, C_TFWeaponBase* weapon ) const
{
	if ( !local || !weapon || local->deadflag( ) )
	{
		return false;
	}

	if ( !vars.misc.crits.crits_active || damage_correct[ weapon->GetSlot( ) ] <= 0 )
	{
		return false;
	}

	if ( weapon->GetSlot( ) == SLOT_MELEE )
	{
		crithack.cost = ( damage_correct[ weapon->GetSlot( ) ] / 2.f );
	}
	else
	{
		crithack.cost = ( damage_correct[ weapon->GetSlot( ) ] * 3 );
		/*if ( weapon->GetWeaponData( ).m_bUseRapidFireCrits )
		{
			crithack.cost *= ( TF_DAMAGE_CRIT_DURATION_RAPID / weapon->GetWeaponData( ).m_flTimeFireDelay );
		}*/
	}

	auto bucket_ = bucket[ weapon->GetSlot( ) ];

	crithack.crits[ weapon->GetSlot( ) ] = std::floor( bucket_ / cost );

	if ( convars.tf_crit_cap->GetInt( ) > 0 && damage_correct[ weapon->GetSlot( ) ] > 0 )
	{
		crithack.potential_crits[ weapon->GetSlot( ) ] = static_cast< unsigned int >( convars.tf_crit_cap->GetInt( ) / cost );
	}

	return true;
}

void __crits::fix_heavy_rev_bug( CUserCmd* pCmd )
{
	const auto& local = entity_cache.get_local( );
	const auto& weapon = entity_cache.get_weapon( );
	if ( !local || !weapon || local->deadflag( ) )
	{
		return;
	}

	if ( !local->IsClass( CLASS_HEAVY ) || weapon->GetWeaponID( ) != TF_WEAPON_MINIGUN )
		return;

	if ( pCmd->buttons & IN_ATTACK )
		pCmd->buttons &= ~IN_ATTACK2;
}

bool __crits::weapon_can_crit( C_TFWeaponBase* weapon )
{
	bool result = true;
	switch ( weapon->m_iItemDefinitionIndex( ) )
	{
		case Scout_s_MadMilk:
		case Scout_s_MutatedMilk:
		case Scout_s_BonkAtomicPunch:
		case Scout_s_CritaCola:
		case Soldier_m_RocketJumper:
		case Soldier_t_TheMarketGardener:
		case Soldier_s_TheBuffBanner:
		case Soldier_s_FestiveBuffBanner:
		case Soldier_s_TheConcheror:
		case Soldier_s_TheBattalionsBackup:
		case Pyro_m_DragonsFury:
		case Pyro_m_ThePhlogistinator:
		case Pyro_s_TheManmelter:
		case Pyro_s_GasPasser:
		case Pyro_t_NeonAnnihilator:
		case Pyro_t_NeonAnnihilatorG:
		case Pyro_t_TheAxtinguisher:
		case Pyro_t_TheFestiveAxtinguisher:
		case Pyro_t_ThePostalPummeler:
		case Demoman_s_StickyJumper:
		case Demoman_t_UllapoolCaber:
		case Engi_m_ThePomson6000:
		case Engi_m_TheFrontierJustice:
		case Engi_m_FestiveFrontierJustice:
		case Engi_s_TheShortCircuit:
		case Engi_s_TheWrangler:
		case Engi_s_FestiveWrangler:
		case Engi_s_TheGigarCounter:
		case Engi_t_TheGunslinger:
		case Engi_t_TheSouthernHospitality:
		case Sniper_m_TheHuntsman:
		case Sniper_m_TheFortifiedCompound:
		case Sniper_s_Jarate:
		case Sniper_s_TheSelfAwareBeautyMark:
		case Sniper_t_TheBushwacka:
		case Spy_m_TheAmbassador:
		case Spy_m_FestiveAmbassador:
		case Spy_m_TheDiamondback:
		case Spy_m_TheEnforcer:
		{
			result = false;
			break;
		}
		default:
		{
			switch ( weapon->GetWeaponID( ) )
			{
				case TF_WEAPON_SNIPERRIFLE:
				case TF_WEAPON_SNIPERRIFLE_CLASSIC:
				case TF_WEAPON_SNIPERRIFLE_DECAP:
				case TF_WEAPON_CLEAVER:
				case TF_WEAPON_FLAMETHROWER_ROCKET:
				case TF_WEAPON_MEDIGUN:
				case TF_WEAPON_SWORD:
				case TF_WEAPON_KNIFE:
				case TF_WEAPON_PDA_SPY:
				case TF_WEAPON_BUILDER:
				case TF_WEAPON_PDA_SPY_BUILD:
				case TF_WEAPON_PDA:
				case TF_WEAPON_PDA_ENGINEER_BUILD:
				case TF_WEAPON_PDA_ENGINEER_DESTROY:
				case TF_WEAPON_PARTICLE_CANNON:
				case TF_WEAPON_LUNCHBOX:
				{
					result = false;
					break;
				}
			}
		}
	}

	return result;
}