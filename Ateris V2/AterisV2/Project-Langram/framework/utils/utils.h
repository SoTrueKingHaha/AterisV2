#pragma once
#include "winapi/winapi.h"
#include "draw/draw.h"
#include "math/math.h"
#include "../sdk/globals/globalinfo.h"
#include "../framework.h"
#include "../sdk/game/c_tfweaponbase.h"
#include "../features/menu/vars.h"
#include "entitycache/entitycache.h"
#include "convars/convars.h"

#undef min
#undef max
class __utils
{
public:
	inline float clr2flt( unsigned char x ) {
		return ( static_cast< float >( x ) / 255.0f );
	}

	inline unsigned char flt2clr( float x )
	{
		return ( unsigned char )( 255.0f * x );
	}

	inline bool w2s( const vector& vOrigin, vector& vScreen )
	{
		const matrix3x4_t& w2s = draw.world_to_projection.As3x4( );

		float w = w2s[ 3 ][ 0 ] * vOrigin[ 0 ] + w2s[ 3 ][ 1 ] * vOrigin[ 1 ] + w2s[ 3 ][ 2 ] * vOrigin[ 2 ] + w2s[ 3 ][ 3 ];
		vScreen.z = 0;

		if ( w > 0.001f ) {
			float flsw = static_cast< float >( draw.screen_width );
			float flsh = static_cast< float >( draw.screen_height );
			float fl1dbw = 1 / w;
			vScreen.x = ( flsw / 2.0f ) + ( 0.5f * ( ( w2s[ 0 ][ 0 ] * vOrigin[ 0 ] + w2s[ 0 ][ 1 ] * vOrigin[ 1 ] + w2s[ 0 ][ 2 ] * vOrigin[ 2 ] + w2s[ 0 ][ 3 ] ) * fl1dbw ) * flsw + 0.5f );
			vScreen.y = ( flsh / 2.0f ) - ( 0.5f * ( ( w2s[ 1 ][ 0 ] * vOrigin[ 0 ] + w2s[ 1 ][ 1 ] * vOrigin[ 1 ] + w2s[ 1 ][ 2 ] * vOrigin[ 2 ] + w2s[ 1 ][ 3 ] ) * fl1dbw ) * flsh + 0.5f );
			return true;
		}

		return false;
	}

	inline vector get_rotated_position( vector vOrigin, const float flRotation, const float flDistance )
	{
		const auto rad = DEG2RAD( flRotation );
		vOrigin.x += cosf( rad ) * flDistance;
		vOrigin.y += sinf( rad ) * flDistance;

		return vOrigin;
	}

	inline bool get_draw_bounds( C_BaseEntity* pEntity, vector* vTrans, int& x, int& y, int& w, int& h )
	{
		vector vMins, vMaxs;
		bool bIsPlayer = false;

		if ( pEntity->GetClassID( ) == ETFClassID::CTFPlayer )
		{
			bIsPlayer = true;
		}

		vMins = pEntity->m_vecMins( );
		vMaxs = pEntity->m_vecMaxs( );

		matrix3x4_t& transform = pEntity->RenderableToWorldTransform( );

		if ( pEntity->entindex( ) == i::engine_client->GetLocalPlayer( ) )
		{
			auto angles = i::engine_client->GetViewAngles( );
			angles.x = angles.z = 0.0f;
			math.angle_matrix( angles, transform );
			math.matrix_set_column( pEntity->GetRenderOrigin( ), 3, transform );
		}

		const vector vPoints[ ] =
		{
			vector( vMins.x, vMins.y, vMins.z ),
			vector( vMins.x, vMaxs.y, vMins.z ),
			vector( vMaxs.x, vMaxs.y, vMins.z ),
			vector( vMaxs.x, vMins.y, vMins.z ),
			vector( vMaxs.x, vMaxs.y, vMaxs.z ),
			vector( vMins.x, vMaxs.y, vMaxs.z ),
			vector( vMins.x, vMins.y, vMaxs.z ),
			vector( vMaxs.x, vMins.y, vMaxs.z )
		};

		for ( int n = 0; n < 8; n++ )
			math.vector_transform( vPoints[ n ], transform, vTrans[ n ] );

		vector flb, brt, blb, frt, frb, brb, blt, flt;

		if ( w2s( vTrans[ 3 ], flb ) && w2s( vTrans[ 5 ], brt )
			 && w2s( vTrans[ 0 ], blb ) && w2s( vTrans[ 4 ], frt )
			 && w2s( vTrans[ 2 ], frb ) && w2s( vTrans[ 1 ], brb )
			 && w2s( vTrans[ 6 ], blt ) && w2s( vTrans[ 7 ], flt ) )
		{
			const vector arr[ ] = { flb, brt, blb, frt, frb, brb, blt, flt };

			float left = flb.x;
			float top = flb.y;
			float right = flb.x;
			float bottom = flb.y;

			for ( int n = 1; n < 8; n++ )
			{
				if ( left > arr[ n ].x )
					left = arr[ n ].x;

				if ( top < arr[ n ].y )
					top = arr[ n ].y;

				if ( right < arr[ n ].x )
					right = arr[ n ].x;

				if ( bottom > arr[ n ].y )
					bottom = arr[ n ].y;
			}

			float x_ = left;
			const float y_ = bottom;
			float w_ = right - left;
			const float h_ = top - bottom;

			if ( bIsPlayer )
			{
				x_ += ( ( right - left ) / 8.0f );
				w_ -= ( ( ( right - left ) / 8.0f ) * 2.0f );
			}

			x = static_cast< int >( x_ );
			y = static_cast< int >( y_ );
			w = static_cast< int >( w_ );
			h = static_cast< int >( h_ );

			return !( x > draw.screen_width || x + w < 0 || y > draw.screen_height || y + h < 0 );
		}

		return false;
	}

	inline void particle_tracer( const char* pszTracerEffectName, const vector& vecStart, const vector& vecEnd, int iEntIndex, int iAttachment, bool bWhiz )
	{
		reinterpret_cast< void( __cdecl* )( const char*, const vector&, const vector&, int, int, bool ) >( S::UTIL_ParticleTracer.address )( pszTracerEffectName, vecStart, vecEnd, iEntIndex, iAttachment, bWhiz );
	}

	inline void fix_movement( CUserCmd* pCmd, const vector& vecTargetAngle )
	{
		vector vecMove( pCmd->forwardmove, pCmd->sidemove, pCmd->upmove );
		vector vecMoveAng = vector( );

		math.vector_angles( vecMove, vecMoveAng );

		float fSpeed = math.fast_sqrt( vecMove.x * vecMove.x + vecMove.y * vecMove.y );
		float fYaw = DEG2RAD( vecTargetAngle.y - pCmd->viewangles.y + vecMoveAng.y );

		pCmd->forwardmove = ( cos( fYaw ) * fSpeed );
		pCmd->sidemove = ( sin( fYaw ) * fSpeed );
	}

	inline color get_health_color( int nHP, int nMAXHP )
	{
		nHP = std::max( 0, std::min( nHP, nMAXHP ) );
		int r = std::min( ( ( 510 * ( nMAXHP - nHP ) ) / nMAXHP ) + 130, 255 );
		int g = std::min( ( ( 510 * nHP ) / nMAXHP ), 255 );
		return { static_cast< byte >( r ), static_cast< byte >( g ), 40, 180 };
	}

	inline unsigned char GetAlpha( int nCurrent, int nMax )
	{
		nCurrent = std::max( 0, std::min( nCurrent, nMax ) );
		int a = std::min( ( ( 255 * ( nMax - nCurrent ) ) / nMax ), 255 );
		return static_cast< byte >( a );
	}

	inline const char* get_class_by_index( const int nClass )
	{
		static const char* szClasses[ ] = { "unknown", "scout", "sniper", "soldier", "demoman",
										   "medic",   "heavy", "pyro",   "spy",     "engineer" };

		return ( nClass < 10 && nClass > 0 ) ? szClasses[ nClass ] : szClasses[ 0 ];
	}

	float lerp_time( )
	{
		const auto update_rate = convars.cl_updaterate->GetInt( );
		const auto interp_ratio = convars.cl_interp->GetFloat( );

		auto lerp = interp_ratio / update_rate;

		if ( lerp <= convars.cl_interp->GetFloat( ) )
			lerp = convars.cl_interp->GetFloat( );

		return lerp;
	}

	class Cond
	{
	public:
		const wchar_t* name;
		color color;
	};

	inline Cond get_cond( ETFCond id )
	{
		switch ( id )
		{
			case TF_COND_AIMING: {return { L"SLOW MOTION", { 110, 110, 110, 255 } }; break; }
			case TF_COND_ZOOMED: {return { L"ZOOM", { 50, 200, 255, 255 } }; break; }
			case TF_COND_DISGUISING: {return { L"DISGUISING", { 150, 150, 150, 255 } }; break; }
			case TF_COND_DISGUISED: {return { L"DISGUISED", { 200, 200, 200, 255 } }; break; }
								  //case TF_COND_TELEPORTED: { return { L"JUST TELEPORTED", { 200, 200, 200, 255 } }; break; }
			case TF_COND_TAUNTING: { return { L"TAUNTING", { 200, 100, 255, 255 } }; break; }
			case TF_COND_PHASE: { return { L"BONKED", { 255, 144, 110, 255 } }; break; }
			case TF_COND_STUNNED: { return { L"STUNNED", { 255, 255, 0, 255 } }; break; }
			case TF_COND_OFFENSEBUFF: { return { L"MINI CRIT", { 255, 150, 255, 255 } }; break; }
			case TF_COND_SHIELD_CHARGE: { return { L"DEMO CHARGE", { 50, 128, 255, 255 } }; break; }
			case TF_COND_ENERGY_BUFF: { return { L"MINI CRIT", { 255, 150, 255, 255 } }; break; }
			case TF_COND_HEALTH_BUFF: { return { L"HEALED", { 128, 255, 45, 255 } }; break; }
			case TF_COND_BURNING: { return { L"BURN", { 255, 150, 0, 255 } }; break; }
			case TF_COND_URINE: { return { L"JARATE", { 255, 255, 0, 255 } }; break; }
			case TF_COND_BLEEDING: { return { L"BLEED", { 255, 50, 50, 255 } }; break; }
			case TF_COND_DEFENSEBUFF: { return { L"BATTALIONS", { 125, 125, 200, 255 } }; break; }
			case TF_COND_MAD_MILK: { return { L"MAD MILK", { 255, 255, 255, 255 } }; break; }
			case TF_COND_MEGAHEAL: { return { L"QUICKFIX UBER", { 255, 110, 255, 255 } }; break; }
			case TF_COND_REGENONDAMAGEBUFF: { return { L"CONCHEROR", { 255, 255, 150, 255 } }; break; }
			case TF_COND_MARKEDFORDEATH: { return { L"FAN O'WAR", { 200, 255, 50, 255 } }; break; }
			case TF_COND_NOHEALINGDAMAGEBUFF: { return { L"NO-HEAL MINI CRIT", { 255, 150, 255, 255 } }; break; }
			case TF_COND_SPEED_BOOST: { return { L"SPEED BOOST", { 0, 190, 255, 255 } }; break; }
			case TF_COND_SODAPOPPER_HYPE: { return { L"SODA POPPER", { 200, 128, 255, 255 } }; break; }
			case TF_COND_CANNOT_SWITCH_FROM_MELEE: { return { L"MELEE LOCKED", { 255, 255, 0, 255 } }; break; }
			case TF_COND_DEFENSEBUFF_NO_CRIT_BLOCK: { return { L"BOMB CARRIER", { 255, 200, 150, 255 } }; break; }
			case TF_COND_DEFENSEBUFF_HIGH: { return { L"CRIT ACTIVATION", { 255, 25, 25, 255 } }; break; }
			case TF_COND_MARKEDFORDEATH_SILENT: { return { L"MARKED", { 200, 255, 50, 255 } }; break; }
			case TF_COND_SAPPED: { return { L"SAPPED", { 150, 150, 150, 255 } }; break; }
			case TF_COND_INVULNERABLE_HIDE_UNLESS_DAMAGE: { return { L"UBERCHARGE HIDDEN", { 255, 110, 255, 255 } }; break; }
			case TF_COND_MEDIGUN_UBER_BULLET_RESIST: { return { L"BULLET RESISTANCE CHARGE", { 255, 110, 255, 255 } }; break; }
			case TF_COND_MEDIGUN_UBER_BLAST_RESIST: { return { L"BLAST RESISTANCE CHARGE", { 255, 110, 255, 255 } }; break; }
			case TF_COND_MEDIGUN_UBER_FIRE_RESIST: { return { L"FIRE RESISTANCE CHARGE", { 255, 110, 255, 255 } }; break; }
			case TF_COND_MEDIGUN_SMALL_BULLET_RESIST: { return { L"BULLET RESISTANCE", { 255, 160, 255, 255 } }; break; }
			case TF_COND_MEDIGUN_SMALL_BLAST_RESIST: { return { L"BLAST RESISTANCE", { 255, 160, 255, 255 } }; break; }
			case TF_COND_MEDIGUN_SMALL_FIRE_RESIST: { return { L"FIRE RESISTANCE", { 255, 160, 255, 255 } }; break; }
			case TF_COND_STEALTHED_USER_BUFF: { return { L"CLOAKED", { 200, 200, 200, 255 } }; break; }
			case TF_COND_STEALTHED_USER_BUFF_FADING: { return { L"CLOAKED", { 200, 200, 200, 255 } }; break; }
			case TF_COND_HALLOWEEN_SPEED_BOOST: { return { L"SPEED BOOST", { 0, 190, 255, 255 } }; break; }
			case TF_COND_HALLOWEEN_QUICK_HEAL: { return { L"HEAL", { 128, 255, 45, 255 } }; break; }
			case TF_COND_RUNE_STRENGTH: { return { L"STRENGTH", { 200, 200, 200, 255 } }; break; }
			case TF_COND_RUNE_HASTE: { return { L"HASTE", { 200, 200, 200, 255 } }; break; }
			case TF_COND_RUNE_REGEN: { return { L"REGENERATION", { 200, 200, 200, 255 } }; break; }
			case TF_COND_RUNE_RESIST: { return { L"DAMAGE RESIST", { 200, 200, 200, 255 } }; break; }
			case TF_COND_RUNE_VAMPIRE: { return { L"LIFE STEAL", { 200, 200, 200, 255 } }; break; }
			case TF_COND_RUNE_REFLECT: { return { L"DAMAGE REFLECT", { 200, 200, 200, 255 } }; break; }
			case TF_COND_RUNE_PRECISION: { return { L"PRECISION", { 200, 200, 200, 255 } }; break; }
			case TF_COND_RUNE_AGILITY: { return { L"AGILITY", { 200, 200, 200, 255 } }; break; }
			case TF_COND_RUNE_KNOCKOUT: { return { L"KNOCKOUT", { 200, 200, 200, 255 } }; break; }
			case TF_COND_RUNE_IMBALANCE: { return { L"REVENGE", { 200, 200, 200, 255 } }; break; }
			case TF_COND_RUNE_KING: { return { L"KING", { 200, 200, 200, 255 } }; break; }
			case TF_COND_RUNE_PLAGUE: { return { L"PLAGUE", { 200, 200, 200, 255 } }; break; }
			case TF_COND_RUNE_SUPERNOVA: { return { L"SUPERNOVA (FAG)", { 200, 200, 200, 255 } }; break; }
			case TF_COND_PLAGUE: { return { L"PLAGUE", { 50, 255, 50, 255 } }; break; }
			case TF_COND_KING_BUFFED: { return { L"KING BUFFED", { 255, 150, 0, 255 } }; break; }
			case TF_COND_PARACHUTE_DEPLOYED: { return { L"PARACHUTE", { 200, 200, 200, 255 } }; break; }
			case TF_COND_GAS: { return { L"GAS", { 50, 255, 50, 255 } }; break; }
							//case TF_COND_ROCKETPACK: { return { L"ROCKETPACK", { 200, 200, 200, 255 } }; break; }
			case TF_COND_HALLOWEEN_HELL_HEAL: { return { L"HEAL", { 128, 255, 45, 255 } }; break; }
			case TF_COND_POWERUPMODE_DOMINANT: { return { L"DOMINANT (FAG)", { 200, 200, 200, 255 } }; break; }
			case TF_COND_FREEZE_INPUT: { return { L"FROZEN", { 200, 200, 200, 255 } }; break; }
			default: {return { L"", {0, 0, 0, 0 } }; break; }
		}
	}

	inline const wchar_t* get_weapon_name( C_TFWeaponBase* pWeapon )
	{
		switch ( pWeapon->m_iItemDefinitionIndex( ) )
		{
			case Scout_m_ForceANature:
			case Scout_m_FestiveForceANature: { return L"FORCE-A-NATURE"; break; }
			case Scout_m_FestiveScattergun: { return L"SCATTERGUN"; break; }
			case Scout_m_BackcountryBlaster: { return L"BACK SCATTER"; break; }
			case Scout_s_MutatedMilk: { return L"MAD MILK"; break; }
			case Scout_s_TheWinger: { return L"WINGER"; break; }
			case Scout_s_FestiveBonk:
			case Scout_s_BonkAtomicPunch: { return L"BONK ATOMIC PUNCH"; break; }
			case Scout_s_PrettyBoysPocketPistol: { return L"POCKET PISTOL"; break; }
			case Scout_s_CritaCola: { return L"CRIT A COLA"; break; }
			case Scout_t_FestiveBat: { return L"BAT"; break; }
			case Scout_t_FestiveHolyMackerel: {return L"HOLY MACKEREL"; break; }
			case Scout_t_TheAtomizer: { return L"ATOMIZER"; break; }
			case Scout_t_TheCandyCane: { return L"CANDY CANE"; break; }
			case Scout_t_TheFanOWar: { return L"FAN O WAR"; break; }
			case Scout_t_SunonaStick: { return L"SUN ON A STICK"; break; }
			case Scout_t_TheBostonBasher: { return L"BOSTON BASHER"; break; }
			case Scout_t_ThreeRuneBlade: { return L"THREE RUNE BLADE"; break; }
			case Scout_t_TheFreedomStaff: { return L"FREEDOM STAFF"; break; }
			case Scout_t_TheBatOuttaHell: { return L"BAT OUTTA HELL"; break; }
			case Scout_s_Lugermorph: { return L"LUGERMORPH"; break; }
			case Scout_s_VintageLugermorph: { return L"LUGERMORPH"; break; }
			case Scout_s_TheCAPPER: { return L"C.A.P.P.E.R"; break; }
			case Scout_t_UnarmedCombat: { return L"UNARMED COMBAT"; break; }
			case Scout_t_Batsaber: { return L"BATSABER"; break; }
			case Scout_t_TheHamShank: { return L"HAM SHANK"; break; }
			case Scout_t_TheNecroSmasher: { return L"NEGRO SMASHER"; break; }
			case Scout_t_TheConscientiousObjector: { return L"OBJECTOR"; break; }
			case Scout_t_TheCrossingGuard: { return L"CROSSING GUARD"; break; }
			case Scout_t_TheMemoryMaker: { return L"MEMORY MAKER"; break; }

			case Soldier_m_FestiveRocketLauncher: {return L"ROCKET LAUNCHER"; break; }
			case Soldier_m_RocketJumper: {return L"ROCKET JUMPER"; break; }
			case Soldier_m_TheAirStrike: { return L"AIR STRIKE"; break; }
			case Soldier_m_TheLibertyLauncher: { return L"LIBERTY LAUNCHER"; break; }
			case Soldier_m_TheOriginal: { return L"ORIGINAL"; break; }
			case Soldier_m_FestiveBlackBox:
			case Soldier_m_TheBlackBox: { return L"BLACK BOX"; break; }
			case Soldier_m_TheBeggarsBazooka: { return L"BEGGARS BAZOOKA"; break; }
			case Soldier_s_FestiveShotgun: { return L"SHOTGUN"; break; }
			case Soldier_s_FestiveBuffBanner: { return L"BUFF BANNER"; break; }
			case Soldier_s_TheConcheror: { return L"CONCHEROR"; break; }
			case Soldier_s_TheBattalionsBackup: { return L"BATTALIONS BACKUP"; break; }
			case Soldier_s_PanicAttack: { return L"PANIC ATTACK"; break; }
			case Soldier_t_TheMarketGardener: { return L"MARKET GARDENER"; break; }
			case Soldier_t_TheDisciplinaryAction: { return L"DISCIPLINARY ACTION"; break; }
			case Soldier_t_TheEqualizer: { return L"EQUALIZER"; break; }
			case Soldier_t_ThePainTrain: { return L"PAIN TRAIN"; break; }
			case Soldier_t_TheHalfZatoichi: { return L"HALF ZATOICHI"; break; }

			case Pyro_m_FestiveFlameThrower: { return L"FLAME THROWER"; break; }
			case Pyro_m_ThePhlogistinator: { return L"PHLOGISTINATOR"; break; }
			case Pyro_m_FestiveBackburner:
			case Pyro_m_TheBackburner: { return L"BACKBURNER"; break; }
			case Pyro_m_TheRainblower: { return L"RAINBLOWER"; break; }
			case Pyro_m_TheDegreaser: { return L"DEGREASER"; break; }
			case Pyro_m_NostromoNapalmer: { return L"NOSTROMO NAPALMER"; break; }
			case Pyro_s_FestiveFlareGun: { return L"FLARE GUN"; break; }
			case Pyro_s_TheScorchShot: { return L"SCORCH SHOT"; break; }
			case Pyro_s_TheDetonator: { return L"DETONATOR"; break; }
			case Pyro_s_TheReserveShooter: { return L"RESERVE SHOOTER"; break; }
			case Pyro_t_TheFestiveAxtinguisher:
			case Pyro_t_TheAxtinguisher: { return L"AXTINGUISHER"; break; }
			case Pyro_t_Homewrecker: { return L"HOMEWRECKER"; break; }
			case Pyro_t_ThePowerjack: { return L"POWERJACK"; break; }
			case Pyro_t_TheBackScratcher: { return L"BACK SCRATCHER"; break; }
			case Pyro_t_TheThirdDegree: { return L"THIRD DEGREE"; break; }
			case Pyro_t_ThePostalPummeler: { return L"POSTAL PUMMELER"; break; }
			case Pyro_t_PrinnyMachete: { return L"PRINNY MACHETE"; break; }
			case Pyro_t_SharpenedVolcanoFragment: { return L"VOLCANO FRAGMENT"; break; }
			case Pyro_t_TheMaul: { return L"MAUL"; break; }
			case Pyro_t_TheLollichop: { return L"LOLLICHOP"; break; }

			case Demoman_m_FestiveGrenadeLauncher: { return L"GRENADE LAUNCHER"; break; }
			case Demoman_m_TheIronBomber: { return L"IRON BOMBER"; break; }
			case Demoman_m_TheLochnLoad: {return L"LOCH N LOAD"; break; }
			case Demoman_s_FestiveStickybombLauncher: { return L"STICKYBOMB LAUNCHER"; break; }
			case Demoman_s_StickyJumper: {return L"STICKY JUMPER"; break; }
			case Demoman_s_TheQuickiebombLauncher: { return L"QUICKIEBOMB LAUNCHER"; break; }
			case Demoman_s_TheScottishResistance: { return L"SCOTTISH RESISTANCE"; break; }
			case Demoman_t_HorselessHeadlessHorsemannsHeadtaker: { return L"HORSEMANNS HEADTAKER"; break; }
			case Demoman_t_TheScottishHandshake: { return L"SCOTTISH HANDSHAKE"; break; }
			case Demoman_t_FestiveEyelander:
			case Demoman_t_TheEyelander: { return L"EYELANDER"; break; }
			case Demoman_t_TheScotsmansSkullcutter: { return L"SCOTSMANS SKULLCUTTER"; break; }
			case Demoman_t_ThePersianPersuader: { return L"PERSIAN PERSUADER"; break; }
			case Demoman_t_NessiesNineIron: { return L"NESSIES NINE IRON"; break; }
			case Demoman_t_TheClaidheamhMor: { return L"CLAIDHEAMH MOR"; break; }

			case Heavy_m_IronCurtain: { return L"IRON CURTAIN"; break; }
			case Heavy_m_FestiveMinigun: { return L"MINIGUN"; break; }
			case Heavy_m_Tomislav: { return L"TOMISLAV"; break; }
			case Heavy_m_TheBrassBeast: { return L"BRASS BEAST"; break; }
			case Heavy_m_Natascha: { return L"NATASCHA"; break; }
			case Heavy_m_TheHuoLongHeaterG:
			case Heavy_m_TheHuoLongHeater: { return L"HUO-LONG HEATER"; break; }
			case Heavy_s_TheFamilyBusiness: { return L"FAMILY BUSINESS"; break; }
			case Heavy_s_FestiveSandvich:
			case Heavy_s_RoboSandvich:
			case Heavy_s_Sandvich: { return L"SANDVICH"; break; }
			case Heavy_s_Fishcake: { return L"FISHCAKE"; break; }
			case Heavy_s_SecondBanana: { return L"BANANA"; break; }
			case Heavy_s_TheDalokohsBar: { return L"CHOCOLATE"; break; }
			case Heavy_s_TheBuffaloSteakSandvich: { return L"STEAK"; break; }
			case Heavy_t_FistsofSteel: {return L"FISTS OF STEEL"; break; }
			case Heavy_t_TheHolidayPunch: { return L"HOLIDAY PUNCH"; break; }
			case Heavy_t_WarriorsSpirit: { return L"WARRIORS SPIRIT"; break; }
			case Heavy_t_TheEvictionNotice: { return L"EVICTION NOTICE"; break; }
			case Heavy_t_TheKillingGlovesofBoxing: { return L"KILLING GLOVES OF BOXING"; break; }
			case Heavy_t_ApocoFists: { return L"APOCO-FISTS"; break; }
			case Heavy_t_FestiveGlovesofRunningUrgently:
			case Heavy_t_GlovesofRunningUrgently: { return L"GLOVES OF RUNNING URGENTLY"; break; }
			case Heavy_t_TheBreadBite: { return L"BREAD BITE"; break; }

			case Engi_m_FestiveFrontierJustice: { return L"FRONTIER JUSTICE"; break; }
			case Engi_m_TheWidowmaker: { return L"WIDOWMAKER"; break; }
			case Engi_s_TheGigarCounter:
			case Engi_s_FestiveWrangler: { return L"WRANGLER"; break; }
			case Engi_s_TheShortCircuit: { return L"SHORT CIRCUIT"; break; }
			case Engi_t_FestiveWrench: { return L"WRENCH"; break; }
			case Engi_t_GoldenWrench: { return L"GOLDEN WRENCH"; break; }
			case Engi_t_TheGunslinger: { return L"GUNSLINGER"; break; }
			case Engi_t_TheJag: { return L"JAG"; break; }
			case Engi_t_TheEurekaEffect: { return L"EUREKA EFFECT"; break; }
			case Engi_t_TheSouthernHospitality: { return L"SOUTHERN HOSPITALITY"; break; }

			case Medic_m_FestiveCrusadersCrossbow: { return L"CRUSADERS CROSSBOW"; break; }
			case Medic_m_TheOverdose: { return L"OVERDOSE"; break; }
			case Medic_m_TheBlutsauger: { return L"BLUTSAUGER"; break; }
			case Medic_s_FestiveMediGun: { return L"MEDIGUN"; break; }
			case Medic_s_TheQuickFix: { return L"QUICK FIX"; break; }
			case Medic_s_TheKritzkrieg: { return L"KRITZKRIEG"; break; }
			case Medic_s_TheVaccinator: { return L"VACCINATOR"; break; }
			case Medic_t_FestiveBonesaw: { return L"BONESAW"; break; }
			case Medic_t_FestiveUbersaw:
			case Medic_t_TheUbersaw: { return L"UBERSAW"; break; }
			case Medic_t_TheVitaSaw: { return L"VITASAW"; break; }
			case Medic_t_TheSolemnVow: { return L"SOLEMN VOW"; break; }
			case Medic_t_Amputator: { return L"AMPUTATOR"; break; }

			case Sniper_m_FestiveSniperRifle: { return L"SNIPER RIFLE"; break; }
			case Sniper_m_FestiveHuntsman:
			case Sniper_m_TheHuntsman: { return L"HUNTSMAN"; break; }
			case Sniper_m_TheMachina: { return L"MACHINA"; break; }
			case Sniper_m_TheAWPerHand: { return L"AWPER HAND"; break; }
			case Sniper_m_TheHitmansHeatmaker: { return L"HITMANS HEATMAKER"; break; }
			case Sniper_m_TheSydneySleeper: { return L"SYDNEY SLEEPER"; break; }
			case Sniper_m_ShootingStar: { return L"SHOOTING STAR"; break; }
			case Sniper_s_FestiveJarate: { return L"JARATE"; break; }
			case Sniper_s_TheSelfAwareBeautyMark: { return L"JARATE"; break; }
			case Sniper_s_FestiveSMG: { return L"SMG"; break; }
			case Sniper_t_TheBushwacka: { return L"BUSHWACKA"; break; }
			case Sniper_t_KukriR:
			case Sniper_t_Kukri: { return L"KUKRI"; break; }
			case Sniper_t_TheShahanshah: { return L"SHAHANSHAH"; break; }
			case Sniper_t_TheTribalmansShiv: { return L"TRIBALMANS SHIV"; break; }

			case Spy_m_FestiveRevolver: { return L"REVOLVER"; break; }
			case Spy_m_FestiveAmbassador:
			case Spy_m_TheAmbassador: { return L"AMBASSADOR"; break; }
			case Spy_m_BigKill: { return L"BIG KILL"; break; }
			case Spy_m_TheDiamondback: { return L"DIAMONDBACK"; break; }
			case Spy_m_TheEnforcer: { return L"ENFORCER"; break; }
			case Spy_m_LEtranger: { return L"LETRANGER"; break; }
			case Spy_s_Sapper:
			case Spy_s_SapperR:
			case Spy_s_FestiveSapper: { return L"SAPPER"; break; }
			case Spy_s_TheRedTapeRecorder:
			case Spy_s_TheRedTapeRecorderG: { return L"RED TAPE RECORDER"; break; }
			case Spy_s_TheApSapG: { return L"AP-SAP"; break; }
			case Spy_s_TheSnackAttack: { return L"SNACK ATTACK"; break; }
			case Spy_t_FestiveKnife: { return L"KNIFE"; break; }
			case Spy_t_ConniversKunai: { return L"KUNAI"; break; }
			case Spy_t_YourEternalReward: { return L"YOUR ETERNAL REWARD"; break; }
			case Spy_t_TheBigEarner: { return L"BIG EARNER"; break; }
			case Spy_t_TheSpycicle: { return L"SPYCICLE"; break; }
			case Spy_t_TheSharpDresser: { return L"SHARP DRESSER"; break; }
			case Spy_t_TheWangaPrick: { return L"WANGA PRICK"; break; }
			case Spy_t_TheBlackRose: { return L"BLACK ROSE"; break; }

			case Heavy_m_Deflector_mvm: { return L"DEFLECTOR"; break; }
			case Misc_t_FryingPan: { return L"FRYING PAN"; break; }
			case Misc_t_GoldFryingPan: { return L"GOLDEN FRYING PAN"; break; }
			case Misc_t_Saxxy: { return L"SAXXY"; break; }

			default:
			{
				switch ( pWeapon->GetWeaponID( ) )
				{
					//scout
					case TF_WEAPON_SCATTERGUN: { return L"SCATTERGUN"; break; }
					case TF_WEAPON_HANDGUN_SCOUT_PRIMARY: { return L"SHORTSTOP"; break; }
					case TF_WEAPON_HANDGUN_SCOUT_SECONDARY: {return L"PISTOL"; break; }
					case TF_WEAPON_SODA_POPPER: { return L"SODA POPPER"; break; }
					case TF_WEAPON_PEP_BRAWLER_BLASTER: { return L"BABY FACES BLASTER"; break; }
					case TF_WEAPON_PISTOL_SCOUT: { return L"PISTOL"; break; }
					case TF_WEAPON_JAR_MILK: { return L"MAD MILK"; break; }
					case TF_WEAPON_CLEAVER: { return L"CLEAVER"; break; }
					case TF_WEAPON_BAT: { return L"BAT"; break; }
					case TF_WEAPON_BAT_WOOD: { return L"SANDMAN"; break; }
					case TF_WEAPON_BAT_FISH: { return L"HOLY MACKEREL"; break; }
					case TF_WEAPON_BAT_GIFTWRAP: { return L"WRAP ASSASSIN"; break; }

											   //soldier
					case TF_WEAPON_ROCKETLAUNCHER: { return L"ROCKET LAUNCHER"; break; }
					case TF_WEAPON_ROCKETLAUNCHER_DIRECTHIT: { return L"DIRECT HIT"; break; }
					case TF_WEAPON_PARTICLE_CANNON: { return L"COW MANGLER 5000"; break; }
					case TF_WEAPON_SHOTGUN_SOLDIER: { return L"SHOTGUN"; break; }
					case TF_WEAPON_BUFF_ITEM: { return L"BUFF BANNER"; break; }
					case TF_WEAPON_RAYGUN: { return L"RIGHTEOUS BISON"; break; }
					case TF_WEAPON_SHOVEL: { return L"SHOVEL"; break; }

										 //pyro
					case TF_WEAPON_FLAMETHROWER: { return L"FLAME THROWER"; break; }
					case TF_WEAPON_FLAME_BALL: { return L"DRAGONS FURY"; break; }
					case TF_WEAPON_SHOTGUN_PYRO: { return L"SHOTGUN"; break; }
					case TF_WEAPON_FLAREGUN: { return L"FLAREGUN"; break; }
					case TF_WEAPON_FLAREGUN_REVENGE: { return L"MANMELTER"; break; }
					case TF_WEAPON_JAR_GAS: { return L"GAS PASSER"; break; }
					case TF_WEAPON_ROCKETPACK: { return L"THERMAL THRUSTER"; break; }
					case TF_WEAPON_FIREAXE: { return L"FIRE AXE"; break; }
					case TF_WEAPON_BREAKABLE_SIGN: {return L"NEON ANNIHILATOR"; break; }
					case TF_WEAPON_SLAP: {return L"HOT HAND"; break; }

									   //demoman
					case TF_WEAPON_GRENADELAUNCHER: { return L"GRENADE LAUNCHER"; break; }
					case TF_WEAPON_PIPEBOMBLAUNCHER: { return L"STICKYBOMB LAUNCHER"; break; }
					case TF_WEAPON_CANNON: {return L"LOOSE CANNON"; break; }
					case TF_WEAPON_BOTTLE: { return L"BOTTLE"; break; }
					case TF_WEAPON_SWORD: { return L"SWORD"; break; }
					case TF_WEAPON_STICKBOMB: { return L"ULLAPOOL CABER"; break; }

											//heavy
					case TF_WEAPON_MINIGUN: { return L"MINIGUN"; break; }
					case TF_WEAPON_SHOTGUN_HWG: { return L"SHOTGUN"; break; }
					case TF_WEAPON_LUNCHBOX: { return L"LUNCHBOX"; break; }
					case TF_WEAPON_FISTS: { return L"FISTS"; break; }

										//engineer
					case TF_WEAPON_SHOTGUN_PRIMARY: { return L"SHOTGUN"; break; }
					case TF_WEAPON_SHOTGUN_BUILDING_RESCUE: { return L"RESCUE RANGER"; break; }
					case TF_WEAPON_SENTRY_REVENGE: { return L"FRONTIER JUSTICE"; break; }
					case TF_WEAPON_DRG_POMSON: { return L"POMSON 6000"; break; }
					case TF_WEAPON_PISTOL: { return L"PISTOL"; break; }
					case TF_WEAPON_LASER_POINTER: { return L"WRANGLER"; break; }
					case TF_WEAPON_MECHANICAL_ARM: { return L"MECHANICAL ARM"; break; }
					case TF_WEAPON_WRENCH: { return L"WRENCH"; break; }
					case TF_WEAPON_PDA_ENGINEER_DESTROY: { return L"DESTRUCTION PDA"; break; }
					case TF_WEAPON_PDA_ENGINEER_BUILD: { return L"CONSTRUCTION PDA"; break; }
					case TF_WEAPON_BUILDER: { return L"TOOLBOX"; break; }

										  //medic
					case TF_WEAPON_SYRINGEGUN_MEDIC: { return L"SYRINGE GUN"; break; }
					case TF_WEAPON_CROSSBOW: { return L"CROSSBOW"; break; }
					case TF_WEAPON_MEDIGUN: { return L"MEDIGUN"; break; }
					case TF_WEAPON_BONESAW: { return L"BONESAW"; break; }

										  //sniper
					case TF_WEAPON_SNIPERRIFLE: { return L"SNIPER RIFLE"; break; }
					case TF_WEAPON_COMPOUND_BOW: { return L"COMPOUND BOW"; break; }
					case TF_WEAPON_SNIPERRIFLE_DECAP: { return L"BAZAAR BARGAIN"; break; }
					case TF_WEAPON_SNIPERRIFLE_CLASSIC: { return L"CLASSIC"; break; }
					case TF_WEAPON_SMG: { return L"SMG"; break; }
					case TF_WEAPON_CHARGED_SMG: { return L"CLEANERS CARBINE"; break; }
					case TF_WEAPON_JAR: { return L"JARATE"; break; }
					case TF_WEAPON_CLUB: { return L"CLUB"; break; }

									   //spy
					case TF_WEAPON_REVOLVER: { return L"REVOLVER"; break; }
					case TF_WEAPON_PDA_SPY_BUILD: { return L"SAPPER"; break; }
					case TF_WEAPON_KNIFE: { return L"KNIFE"; break; }
					case TF_WEAPON_PDA_SPY: { return L"DISGUISE KIT"; break; }
					case TF_WEAPON_INVIS: { return L"INVIS WATCH"; break; }

					case TF_WEAPON_GRAPPLINGHOOK: { return L"GRAPPLING HOOK"; break; }

												//misc
					default: { return L"NULL"; break; }
				}
				return L"NULL"; break;
			}
		}

		return L"NULL";
	}

	inline std::wstring utf8_to_wide( const std::string_view& str )
	{
		int count = MultiByteToWideChar( CP_UTF8, 0, str.data( ), str.length( ), NULL, 0 );
		std::wstring wstr( count, 0 );
		MultiByteToWideChar( CP_UTF8, 0, str.data( ), str.length( ), &wstr[ 0 ], count );
		return wstr;
	}

	inline std::wstring get_building_name( C_BaseEntity* entity )
	{
		switch ( entity->GetClassID( ) )
		{
			case ETFClassID::CObjectSentrygun:
			{
				const auto& pSentry = entity->As<C_ObjectSentrygun>( );
				if ( pSentry->m_bMiniBuilding( ) )
					return L"Mini Sentry";
				else
					return L"Sentry";
				break;
			}
			case ETFClassID::CObjectDispenser:
			{
				return L"Dispenser";
				break;
			}
			case ETFClassID::CObjectTeleporter:
			{
				return L"Teleporter";
				break;
			}
			default: return L"NULL"; break;
		}
		return L"NULL";
	}

	inline std::wstring get_projectile_name( C_BaseEntity* entity )
	{
		switch ( entity->GetClassID( ) )
		{
			case ETFClassID::CTFProjectile_Rocket:
			case ETFClassID::CTFProjectile_SentryRocket:
			{
				return L"Rocket";
			}
			case ETFClassID::CTFGrenadePipebombProjectile:
			{
				const auto& pProjectile = entity->As<C_TFGrenadePipebombProjectile>( );
				const auto& thrower = i::entity_list->GetClientEntityFromHandle( pProjectile->m_hThrower( ) )->As<C_TFPlayer>( );
				if ( pProjectile && pProjectile->As<C_TFGrenadePipebombProjectile>( )->m_iType( ) == TYPE_STICKY  )
				{
					return L"Sticky";
				}
				else
				{
					return L"Pipebomb";
				}
			}
			case ETFClassID::CTFProjectile_Jar:
			case ETFClassID::CTFProjectile_ThrowableRepel:
			{
				return L"Jarate";
			}
			case ETFClassID::CTFProjectile_JarGas:
			{
				return L"Gas Passer";
			}
			case ETFClassID::CTFProjectile_JarMilk:
			case ETFClassID::CTFProjectile_ThrowableBreadMonster:
			{
				return L"Mad Milk";
			}
			case ETFClassID::CTFProjectile_Arrow:
			{
				return L"Arrow";
			}
			case ETFClassID::CTFProjectile_Flare:
			{
				return L"Flare";
			}
			case ETFClassID::CTFProjectile_Cleaver:
			{
				return L"Cleaver";
			}
			case ETFClassID::CTFProjectile_HealingBolt:
			{
				return L"Healing Bolt";
			}
			case ETFClassID::CTFProjectile_EnergyRing:
			{
				return L"Energy Ring";
			}
			case ETFClassID::CTFProjectile_EnergyBall:
			{
				return L"Energy Ball";
			}
			case ETFClassID::CTFStunBall:
			{
				return L"Baseball";
			}
			case ETFClassID::CTFBall_Ornament:
			{
				return L"Ornament Ball";
			}
			default: return L"null";
		}
		return L"null";
	}

	inline bool is_window_in_focus( )
	{
		static HWND hwGame = 0;

		while ( !hwGame ) {
			hwGame = WinAPI::FindWindowW( 0, ( L"Team Fortress 2" ) );
			return false;
		}

		return ( WinAPI::GetForegroundWindow( ) == hwGame );
	}

	inline bool is_on_screen( C_TFPlayer* pLocal, C_BaseEntity* pEntity )
	{
		vector WSC = pEntity->GetWorldSpaceCenter( );

		if ( WSC.dist_to( pLocal->GetWorldSpaceCenter( ) ) > 300.0f )
		{
			vector vScreen = {};

			if ( w2s( pEntity->GetWorldSpaceCenter( ), vScreen ) )
			{
				if ( vScreen.x < -400
					 || vScreen.x > draw.screen_width + 400
					 || vScreen.y < -400
					 || vScreen.y > draw.screen_height + 400 )
					return false;
			}

			else return false;
		}

		return true;
	}

	inline vector compute_move( const CUserCmd* pCmd, C_TFPlayer* pLocal, vector& a, vector& b )
	{
		const vector diff = ( b - a );
		if ( diff.length( ) == 0.0f )
		{
			return { 0.0f, 0.0f, 0.0f };
		}
		const float x = diff.x;
		const float y = diff.y;
		const vector vSilent( x, y, 0 );
		vector ang;
		math.vector_angles( vSilent, ang );
		const float yaw = DEG2RAD( ang.y - pCmd->viewangles.y );
		const float pitch = DEG2RAD( ang.x - pCmd->viewangles.x );
		vector move = { cos( yaw ) * 450.0f, -sin( yaw ) * 450.0f, -cos( pitch ) * 450.0f };

		// Only apply upmove in water
		if ( !( i::engine_trace->GetPointContents( pLocal->GetShootPosition( ) ) & CONTENTS_WATER ) )
		{
			move.z = pCmd->upmove;
		}
		return move;
	}

	inline void walk_to( CUserCmd* pCmd, C_TFPlayer* pLocal, vector& a, vector& b, float scale )
	{
		// Calculate how to get to a vector
		const auto result = compute_move( pCmd, pLocal, a, b );

		// Push our move to usercmd
		pCmd->forwardmove = result.x * scale;
		pCmd->sidemove = result.y * scale;
		pCmd->upmove = result.z * scale;
	}

	inline void walk_to( CUserCmd* pCmd, C_TFPlayer* pLocal, vector& pDestination )
	{
		vector localPos = pLocal->m_vecOrigin( );
		walk_to( pCmd, pLocal, localPos, pDestination, 1.f );
	}

	inline void stop_movement( C_TFPlayer* pLocal, CUserCmd* pCmd, bool safe = true ) {
		if ( safe && global_info.attacking ) { return; }

		if ( pLocal ) {
			const float direction = math.velocity_to_angles( pLocal->m_vecVelocity( ) ).y;
			pCmd->viewangles.x = -90;	//	on projectiles we would be annoyed if we shot the ground.
			pCmd->viewangles.y = direction;
			pCmd->viewangles.z = 0;
			pCmd->sidemove = 0; pCmd->forwardmove = 0;
			global_info.should_stop = false;
		}
	}

	inline void trace_hull( const vector& vecStart, const vector& vecEnd, const vector& vecHullMin, const vector& vecHullMax,
						   unsigned int nMask, CTraceFilter* pFilter, CGameTrace* pTrace )
	{
		Ray_t ray;
		ray.Init( vecStart, vecEnd, vecHullMin, vecHullMax );
		i::engine_trace->TraceRay( ray, nMask, pFilter, pTrace );
	}

	inline void trace( const vector& start, const vector& end, unsigned int mask, CTraceFilter* filter, trace_t* trace )
	{
		Ray_t ray = {};
		ray.Init( start, end );
		i::engine_trace->TraceRay( ray, mask, filter, trace );
	}

	inline bool vis_pos( const vector& from, const vector& to )
	{
		trace_t game_trace = {};
		CTraceFilterHitscan filter = {};
		trace( from, to, ( MASK_SHOT | CONTENTS_GRATE ), &filter, &game_trace ); //MASK_SOLID
		return game_trace.m_flFraction > 0.99f;
	}

	inline bool vis_pos( C_BaseEntity* pSkip, C_BaseEntity* pEntity, const vector& from, const vector& to )
	{
		CGameTrace game_trace = {};
		CTraceFilterHitscan filter = {};
		filter.pSkip = pSkip;
		trace( from, to, ( MASK_SHOT | CONTENTS_GRATE ), &filter, &game_trace );
		return ( ( game_trace.m_pEnt && game_trace.m_pEnt == pEntity ) || game_trace.m_flFraction > 0.99f );
	}

	inline bool vis_pos_mask( C_BaseEntity* pSkip, const C_BaseEntity* pEntity, const vector& from, const vector& to, unsigned int nMask )
	{
		CGameTrace game_trace = {};
		CTraceFilterHitscan filter = {};
		filter.pSkip = pSkip;
		trace( from, to, nMask, &filter, &game_trace );
		return ( ( game_trace.m_pEnt && game_trace.m_pEnt == pEntity ) || game_trace.m_flFraction > 0.99f );
	}

	inline bool vis_pos_hitbox_id( C_BaseEntity* pSkip, C_BaseEntity* pEntity, const vector& from, const vector& to, int nHitbox )
	{
		CGameTrace game_trace = {};
		CTraceFilterHitscan filter = {};
		filter.pSkip = pSkip;
		trace( from, to, ( MASK_SHOT | CONTENTS_GRATE ), &filter, &game_trace );
		return ( game_trace.m_pEnt && game_trace.m_pEnt == pEntity && game_trace.nHitbox == nHitbox );
	}

	inline bool vis_pos_hitbox_id_out( C_BaseEntity* pSkip, C_BaseEntity* pEntity, const vector& from, const vector& to, int& nHitboxOut )
	{
		CGameTrace game_trace = {};
		CTraceFilterHitscan filter = {};
		filter.pSkip = pSkip;
		trace( from, to, ( MASK_SHOT | CONTENTS_GRATE ), &filter, &game_trace );

		if ( game_trace.m_pEnt && game_trace.m_pEnt == pEntity ) {
			nHitboxOut = game_trace.nHitbox;
			return true;
		}

		return false;
	}

	inline bool vis_pos_fraction( C_BaseEntity* pSkip, const vector& from, const vector& to )
	{
		CGameTrace game_trace = {};
		CTraceFilterHitscan filter = {};
		filter.pSkip = pSkip;
		trace( from, to, ( MASK_SHOT | CONTENTS_GRATE ), &filter, &game_trace );
		return ( game_trace.m_flFraction > 0.99f );
	}

	inline void clamp_angles( vector& v ) {
		v.x = std::max( -89.0f, std::min( 89.0f, math.normalize_angle( v.x ) ) );
		v.y = math.normalize_angle( v.y );
		v.z = 0.0f;
	}

	template < typename t >
	__forceinline void clamp( t& n, const t& lower, const t& upper ) {
		n = std::max( lower, std::min( n, upper ) );
	}

	inline EWeaponType get_weapon_type( C_TFWeaponBase* pWeapon )
	{
		if ( !pWeapon )
			return EWeaponType::UNKNOWN;

		if ( pWeapon->GetSlot( ) == EWeaponSlots::SLOT_MELEE )
			return EWeaponType::MELEE;

		switch ( pWeapon->GetWeaponID( ) )
		{
			case TF_WEAPON_ROCKETLAUNCHER:
			case TF_WEAPON_FLAME_BALL:
			case TF_WEAPON_GRENADELAUNCHER:
			case TF_WEAPON_PIPEBOMBLAUNCHER:
			case TF_WEAPON_FLAREGUN:
			case TF_WEAPON_COMPOUND_BOW:
			case TF_WEAPON_ROCKETLAUNCHER_DIRECTHIT:
			case TF_WEAPON_CROSSBOW:
			case TF_WEAPON_PARTICLE_CANNON:
			case TF_WEAPON_DRG_POMSON:
			case TF_WEAPON_FLAREGUN_REVENGE:
			case TF_WEAPON_CANNON:
			case TF_WEAPON_SYRINGEGUN_MEDIC:
			case TF_WEAPON_SHOTGUN_BUILDING_RESCUE:
			case TF_WEAPON_FLAMETHROWER:
			case TF_WEAPON_CLEAVER:
			{
				return EWeaponType::PROJECTILE;
			}

			default:
			{
				int nDamageType = pWeapon->GetDamageType( );

				if ( nDamageType & DMG_BULLET || nDamageType && DMG_BUCKSHOT )
					return EWeaponType::HITSCAN;

				break;
			}
		}

		return EWeaponType::UNKNOWN;
	}

	inline void projectile_setup( C_TFPlayer* pPlayer, const vector& vViewAngles, vector vOffset, vector* vSrc )
	{
		if ( convars.cl_flipviewmodels->GetInt( ) )
			vOffset.y *= -1.0f;

		vector vecForward = vector( ), vecRight = vector( ), vecUp = vector( );
		math.angle_vectors( vViewAngles, &vecForward, &vecRight, &vecUp );

		*vSrc = pPlayer->GetShootPosition( ) + ( vecForward * vOffset.x ) + ( vecRight * vOffset.y ) + ( vecUp * vOffset.z );
	}

	inline bool is_attacking( const CUserCmd* pCmd, C_TFPlayer* local, C_TFWeaponBase* pWeapon )
	{
		if ( pWeapon->GetSlot( ) == SLOT_MELEE )
		{
			if ( pWeapon->GetWeaponID( ) == TF_WEAPON_KNIFE )
			{
				return ( ( pCmd->buttons & IN_ATTACK ) && global_info.can_attack );
			}

			return fabs( pWeapon->m_flSmackTime( ) - i::global_vars->curtime ) < i::global_vars->interval_per_tick * 4;
		}


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
};
inline __utils utils;

class Timer {
private:
	std::chrono::steady_clock::time_point Last;

	[[nodiscard]] bool Check( unsigned ms ) const
	{
		const auto currentTime = std::chrono::steady_clock::now( );
		return std::chrono::duration_cast< std::chrono::milliseconds >( currentTime - Last ).count( ) >= ms;
	}
public:
	Timer( )
	{
		Last = std::chrono::steady_clock::now( );
	}

	bool Run( unsigned ms )
	{
		if ( Check( ms ) ) {
			Last = std::chrono::steady_clock::now( );
			return true;
		}
		return false;
	}
};

class __colors
{
public:
	class __esp
	{
	public:
		inline color get_entity_color( C_BaseEntity* pEntity, C_TFPlayer* pLocal )
		{
			if ( pEntity->IsPlayer( ) )
			{
				if ( pEntity == pLocal || pEntity->As<C_TFPlayer>( )->PlayerOnFriendsList( ) )
					return vars.colors.esp.players.esp_local_player;

				if ( pEntity->As<C_TFPlayer>( )->InCond( TF_COND_STEALTHED ) )
					return vars.colors.esp.players.player_cloaked;
			}
			/*switch ( pEntity->m_iTeamNum( ) ) {
				case 2: return Vars::Colors::Enemy;
				case 3: return Vars::Colors::Teammate;
				default: return Vars::Colors::Default;
			}*/

			if ( vars.colors.esp.players.esp_player_color_scheme == 0 )
			{
				if ( pEntity->m_iTeamNum( ) != pLocal->m_iTeamNum( ) )
					return vars.colors.esp.players.esp_player_enemy;
				else if ( pEntity->m_iTeamNum( ) == pLocal->m_iTeamNum( ) )
					return vars.colors.esp.players.esp_player_teammate;
				else
					return vars.colors.esp.players.esp_player_default;
			}

			if ( vars.colors.esp.players.esp_player_color_scheme == 1 )
			{
				if ( pEntity->m_iTeamNum( ) == TEAM_RED )
					return vars.colors.esp.players.esp_player_red;
				else if ( pEntity->m_iTeamNum( ) == TEAM_BLU )
					return vars.colors.esp.players.esp_player_blu;
				else
					return vars.colors.esp.players.esp_player_default;
			}

			return vars.colors.esp.players.esp_player_default;
		}

		inline color get_building_color( C_BaseObject* pObject, C_TFPlayer* pLocal )
		{
			if ( pObject->m_hBuilder( ).Get( ) == pLocal || pObject->m_hBuilder( ).Get( )->As<C_TFPlayer>( )->PlayerOnFriendsList( ) )
				return vars.colors.esp.buildings.esp_local_building;

			/*switch ( pObject->m_iTeamNum( ) ) {
				case 2: return Vars::Colors::EnemyBuilding;
				case 3: return Vars::Colors::TeammateBuilding;
				default: return Vars::Colors::Default;
			}*/

			if ( vars.colors.esp.buildings.esp_building_color_scheme == 0 )
			{
				if ( pObject->m_iTeamNum( ) != pLocal->m_iTeamNum( ) )
					return vars.colors.esp.buildings.esp_building_enemy;
				else if ( pObject->m_iTeamNum( ) == pLocal->m_iTeamNum( ) )
					return vars.colors.esp.buildings.esp_building_teammate;
				else
					return vars.colors.esp.buildings.esp_building_default;
			}

			if ( vars.colors.esp.buildings.esp_building_color_scheme == 1 )
			{
				if ( pObject->m_iTeamNum( ) == TEAM_RED )
					return vars.colors.esp.buildings.esp_building_red;
				else if ( pObject->m_iTeamNum( ) == TEAM_BLU )
					return vars.colors.esp.buildings.esp_building_blu;
				else
					return vars.colors.esp.buildings.esp_building_default;
			}

			return vars.colors.esp.buildings.esp_building_default;
		}

		inline color get_projectile_color( C_BaseGrenade* pProjectile, C_TFPlayer* pLocal )
		{
			if ( ( GET_ENT_H( pProjectile->m_hThrower( ) ) && ( GET_ENT_H( pProjectile->m_hThrower( ) ) == pLocal || GET_ENT_H( pProjectile->m_hThrower( ) )->As<C_TFPlayer>( )->PlayerOnFriendsList( ) ) )
				 ||
				 ( ( pProjectile->m_hOwnerEntity( ).Get( ) ) && ( pProjectile->m_hOwnerEntity( ).Get( ) == pLocal || pProjectile->m_hOwnerEntity( ).Get( )->As<C_TFPlayer>( )->PlayerOnFriendsList( ) ) ) )
				return vars.colors.esp.world.esp_local_world;

			/*switch ( pProjectile->m_iTeamNum( ) ) {
				case 2: return Vars::Colors::Enemy;
				case 3: return Vars::Colors::Teammate;
				default: return Vars::Colors::Default;
			}*/

			if ( vars.colors.esp.world.esp_world_color_scheme == 0 )
			{
				if ( pProjectile->m_iTeamNum( ) != pLocal->m_iTeamNum( ) )
					return vars.colors.esp.world.esp_world_enemy;
				else if ( pProjectile->m_iTeamNum( ) == pLocal->m_iTeamNum( ) )
					return vars.colors.esp.world.esp_world_teammate;
				else
					return vars.colors.esp.world.esp_world_default;
			}

			if ( vars.colors.esp.world.esp_world_color_scheme == 1 )
			{
				if ( pProjectile->m_iTeamNum( ) == TEAM_RED )
					return vars.colors.esp.world.esp_world_red;
				else if ( pProjectile->m_iTeamNum( ) == TEAM_BLU )
					return vars.colors.esp.world.esp_world_blu;
				else
					return vars.colors.esp.world.esp_world_default;
			}

			return vars.colors.esp.world.esp_world_default;
		}

		inline color get_world_color( C_BaseEntity* entity, C_TFPlayer* local )
		{
			const auto& name = FNV1A::Hash( i::model_info->GetModelName( entity->GetModel( ) ) );
			switch ( entity->GetClassID( ) )
			{
				case ETFClassID::CBaseAnimating:
				{
					if ( entity_cache.is_health( name ) )
					{
						return vars.colors.esp.world.esp_world_health;
						break;
					}

					if ( entity_cache.is_ammo( name ) )
					{
						return vars.colors.esp.world.esp_world_ammo;
						break;
					}
				}

				case ETFClassID::CTFAmmoPack:
				{
					return vars.colors.esp.world.esp_world_ammo;
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
					return get_projectile_color( entity->As<C_BaseGrenade>( ), local );
					break;
				}

				case ETFClassID::CTFPumpkinBomb:
				case ETFClassID::CTFGenericBomb:
				{
					return vars.colors.esp.world.esp_world_bomb;
					break;
				}

				case ETFClassID::CCaptureFlag:
				{
					if ( vars.colors.esp.world.esp_world_color_scheme == 0 )
					{
						if ( entity->m_iTeamNum( ) != local->m_iTeamNum( ) )
							return vars.colors.esp.world.esp_world_enemy;
						else if ( entity->m_iTeamNum( ) == local->m_iTeamNum( ) )
							return vars.colors.esp.world.esp_world_teammate;
						else
							return vars.colors.esp.world.esp_world_default;
					}

					if ( vars.colors.esp.world.esp_world_color_scheme == 1 )
					{
						if ( entity->m_iTeamNum( ) == TEAM_RED )
							return vars.colors.esp.world.esp_world_red;
						else if ( entity->m_iTeamNum( ) == TEAM_BLU )
							return vars.colors.esp.world.esp_world_blu;
						else
							return vars.colors.esp.world.esp_world_default;
					}
					break;
				}

				default: return vars.colors.esp.world.esp_world_default;
			}
			return vars.colors.esp.world.esp_world_default;
		}
	};
	__esp esp;

	class __models
	{
	public:
		inline color get_entity_color( C_BaseEntity* pEntity, C_TFPlayer* pLocal )
		{
			if ( pEntity->IsPlayer( ) )
			{
				if ( pEntity == pLocal || pEntity->As<C_TFPlayer>( )->PlayerOnFriendsList( ) )
					return vars.colors.models.players.models_local_player;

				if ( pEntity->As<C_TFPlayer>( )->InCond( TF_COND_STEALTHED ) )
					return vars.colors.models.players.models_player_cloaked;
			}
			/*switch ( pEntity->m_iTeamNum( ) ) {
				case 2: return Vars::Colors::Enemy;
				case 3: return Vars::Colors::Teammate;
				default: return Vars::Colors::Default;
			}*/

			if ( vars.colors.models.players.models_player_color_scheme == 0 )
			{
				if ( pEntity->m_iTeamNum( ) != pLocal->m_iTeamNum( ) )
					return vars.colors.models.players.models_player_enemy;
				else if ( pEntity->m_iTeamNum( ) == pLocal->m_iTeamNum( ) )
					return vars.colors.models.players.models_player_teammate;
				else
					return vars.colors.models.players.models_player_default;
			}

			if ( vars.colors.models.players.models_player_color_scheme == 1 )
			{
				if ( pEntity->m_iTeamNum( ) == TEAM_RED )
					return vars.colors.models.players.models_player_red;
				else if ( pEntity->m_iTeamNum( ) == TEAM_BLU )
					return vars.colors.models.players.models_player_blu;
				else
					return vars.colors.models.players.models_player_default;
			}

			return vars.colors.models.players.models_player_default;
		}

		inline color get_building_color( C_BaseObject* pObject, C_TFPlayer* pLocal )
		{
			if ( pObject->m_hBuilder( ).Get( ) == pLocal || pObject->m_hBuilder( ).Get( )->As<C_TFPlayer>( )->PlayerOnFriendsList( ) )
				return vars.colors.models.buildings.models_local_building;

			/*switch ( pObject->m_iTeamNum( ) ) {
				case 2: return Vars::Colors::EnemyBuilding;
				case 3: return Vars::Colors::TeammateBuilding;
				default: return Vars::Colors::Default;
			}*/

			if ( vars.colors.models.buildings.models_building_color_scheme == 0 )
			{
				if ( pObject->m_iTeamNum( ) != pLocal->m_iTeamNum( ) )
					return vars.colors.models.buildings.models_building_enemy;
				else if ( pObject->m_iTeamNum( ) == pLocal->m_iTeamNum( ) )
					return vars.colors.models.buildings.models_building_teammate;
				else
					return vars.colors.models.buildings.models_building_default;
			}

			if ( vars.colors.models.buildings.models_building_color_scheme == 1 )
			{
				if ( pObject->m_iTeamNum( ) == TEAM_RED )
					return vars.colors.models.buildings.models_building_red;
				else if ( pObject->m_iTeamNum( ) == TEAM_BLU )
					return vars.colors.models.buildings.models_building_blu;
				else
					return vars.colors.models.buildings.models_building_default;
			}

			return vars.colors.models.buildings.models_building_default;
		}

		inline color get_projectile_color( C_BaseGrenade* pProjectile, C_TFPlayer* pLocal )
		{
			if ( ( GET_ENT_H( pProjectile->m_hThrower( ) ) && ( GET_ENT_H( pProjectile->m_hThrower( ) ) == pLocal || GET_ENT_H( pProjectile->m_hThrower( ) )->As<C_TFPlayer>( )->PlayerOnFriendsList( ) ) ) 
				 ||
			   ( ( pProjectile->m_hOwnerEntity( ).Get( ) ) && ( pProjectile->m_hOwnerEntity( ).Get( ) == pLocal || pProjectile->m_hOwnerEntity( ).Get( )->As<C_TFPlayer>( )->PlayerOnFriendsList( ) ) ) )
				return vars.colors.models.world.models_local_world;

			/*switch ( pProjectile->m_iTeamNum( ) ) {
				case 2: return Vars::Colors::Enemy;
				case 3: return Vars::Colors::Teammate;
				default: return Vars::Colors::Default;
			}*/

			if ( vars.colors.models.world.models_world_color_scheme == 0 )
			{
				if ( pProjectile->m_iTeamNum( ) != pLocal->m_iTeamNum( ) )
					return vars.colors.models.world.models_world_enemy;
				else if ( pProjectile->m_iTeamNum( ) == pLocal->m_iTeamNum( ) )
					return vars.colors.models.world.models_world_teammate;
				else
					return vars.colors.models.world.models_world_default;
			}

			if ( vars.colors.models.world.models_world_color_scheme == 1 )
			{
				if ( pProjectile->m_iTeamNum( ) == TEAM_RED )
					return vars.colors.models.world.models_world_red;
				else if ( pProjectile->m_iTeamNum( ) == TEAM_BLU )
					return vars.colors.models.world.models_world_blu;
				else
					return vars.colors.models.world.models_world_default;
			}

			return vars.colors.models.world.models_world_default;
		}

		inline color get_world_color( C_BaseEntity* entity, C_TFPlayer* local )
		{
			const auto& name = FNV1A::Hash( i::model_info->GetModelName( entity->GetModel( ) ) );
			switch ( entity->GetClassID( ) )
			{
				case ETFClassID::CBaseAnimating:
				{
					if ( entity_cache.is_health( name ) )
					{
						return vars.colors.models.world.models_world_health;
						break;
					}

					if ( entity_cache.is_ammo( name ) )
					{
						return vars.colors.models.world.models_world_ammo;
						break;
					}
				}

				case ETFClassID::CTFAmmoPack:
				{
					return vars.colors.models.world.models_world_ammo;
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
					return get_projectile_color( entity->As<C_BaseGrenade>( ), local );
					break;
				}

				case ETFClassID::CTFPumpkinBomb:
				case ETFClassID::CTFGenericBomb:
				{
					return vars.colors.models.world.models_world_bomb;
					break;
				}

				case ETFClassID::CCaptureFlag:
				{
					if ( vars.colors.models.world.models_world_color_scheme == 0 )
					{
						if ( entity->m_iTeamNum( ) != local->m_iTeamNum( ) )
							return vars.colors.models.world.models_world_enemy;
						else if ( entity->m_iTeamNum( ) == local->m_iTeamNum( ) )
							return vars.colors.models.world.models_world_teammate;
						else
							return vars.colors.models.world.models_world_default;
					}

					if ( vars.colors.models.world.models_world_color_scheme == 1 )
					{
						if ( entity->m_iTeamNum( ) == TEAM_RED )
							return vars.colors.models.world.models_world_red;
						else if ( entity->m_iTeamNum( ) == TEAM_BLU )
							return vars.colors.models.world.models_world_blu;
						else
							return vars.colors.models.world.models_world_default;
					}
					break;
				}

				default: return vars.colors.models.world.models_world_default;
			}
			return vars.colors.models.world.models_world_default;
		}
	};
	__models models;

	class __outlines
	{
	public:
		inline color get_entity_color( C_BaseEntity* pEntity, C_TFPlayer* pLocal )
		{
			if ( pEntity->IsPlayer( ) )
			{
				if ( pEntity == pLocal || pEntity->As<C_TFPlayer>( )->PlayerOnFriendsList( ) )
					return vars.colors.outlines.players.outlines_local_player;

				if ( pEntity->As<C_TFPlayer>( )->InCond( TF_COND_STEALTHED ) )
					return vars.colors.outlines.players.outlines_player_cloaked;
			}
			/*switch ( pEntity->m_iTeamNum( ) ) {
				case 2: return Vars::Colors::Enemy;
				case 3: return Vars::Colors::Teammate;
				default: return Vars::Colors::Default;
			}*/

			if ( vars.colors.outlines.players.outlines_player_color_scheme == 0 )
			{
				if ( pEntity->m_iTeamNum( ) != pLocal->m_iTeamNum( ) )
					return vars.colors.outlines.players.outlines_player_enemy;
				else if ( pEntity->m_iTeamNum( ) == pLocal->m_iTeamNum( ) )
					return vars.colors.outlines.players.outlines_player_teammate;
				else
					return vars.colors.outlines.players.outlines_player_default;
			}

			if ( vars.colors.outlines.players.outlines_player_color_scheme == 1 )
			{
				if ( pEntity->m_iTeamNum( ) == TEAM_RED )
					return vars.colors.outlines.players.outlines_player_red;
				else if ( pEntity->m_iTeamNum( ) == TEAM_BLU )
					return vars.colors.outlines.players.outlines_player_blu;
				else
					return vars.colors.outlines.players.outlines_player_default;
			}

			return vars.colors.outlines.players.outlines_player_default;
		}

		inline color get_building_color( C_BaseObject* pObject, C_TFPlayer* pLocal )
		{
			if ( pObject->m_hBuilder( ).Get( ) == pLocal || pObject->m_hBuilder( ).Get( )->As<C_TFPlayer>( )->PlayerOnFriendsList( ) )
				return vars.colors.outlines.buildings.outlines_local_building;

			/*switch ( pObject->m_iTeamNum( ) ) {
				case 2: return Vars::Colors::EnemyBuilding;
				case 3: return Vars::Colors::TeammateBuilding;
				default: return Vars::Colors::Default;
			}*/

			if ( vars.colors.outlines.buildings.outlines_building_color_scheme == 0 )
			{
				if ( pObject->m_iTeamNum( ) != pLocal->m_iTeamNum( ) )
					return vars.colors.outlines.buildings.outlines_building_enemy;
				else if ( pObject->m_iTeamNum( ) == pLocal->m_iTeamNum( ) )
					return vars.colors.outlines.buildings.outlines_building_teammate;
				else
					return vars.colors.outlines.buildings.outlines_building_default;
			}

			if ( vars.colors.outlines.buildings.outlines_building_color_scheme == 1 )
			{
				if ( pObject->m_iTeamNum( ) == TEAM_RED )
					return vars.colors.outlines.buildings.outlines_building_red;
				else if ( pObject->m_iTeamNum( ) == TEAM_BLU )
					return vars.colors.outlines.buildings.outlines_building_blu;
				else
					return vars.colors.outlines.buildings.outlines_building_default;
			}

			return vars.colors.outlines.buildings.outlines_building_default;
		}

		inline color get_projectile_color( C_BaseGrenade* pProjectile, C_TFPlayer* pLocal )
		{
			if ( ( GET_ENT_H( pProjectile->m_hThrower( ) ) && ( GET_ENT_H( pProjectile->m_hThrower( ) ) == pLocal || GET_ENT_H( pProjectile->m_hThrower( ) )->As<C_TFPlayer>( )->PlayerOnFriendsList( ) ) )
				 ||
				 ( ( pProjectile->m_hOwnerEntity( ).Get( ) ) && ( pProjectile->m_hOwnerEntity( ).Get( ) == pLocal || pProjectile->m_hOwnerEntity( ).Get( )->As<C_TFPlayer>( )->PlayerOnFriendsList( ) ) ) )
				return vars.colors.outlines.world.outlines_local_world;

			/*switch ( pProjectile->m_iTeamNum( ) ) {
				case 2: return Vars::Colors::Enemy;
				case 3: return Vars::Colors::Teammate;
				default: return Vars::Colors::Default;
			}*/

			if ( vars.colors.outlines.world.outlines_world_color_scheme == 0 )
			{
				if ( pProjectile->m_iTeamNum( ) != pLocal->m_iTeamNum( ) )
					return vars.colors.outlines.world.outlines_world_enemy;
				else if ( pProjectile->m_iTeamNum( ) == pLocal->m_iTeamNum( ) )
					return vars.colors.outlines.world.outlines_world_teammate;
				else
					return vars.colors.outlines.world.outlines_world_default;
			}

			if ( vars.colors.outlines.world.outlines_world_color_scheme == 1 )
			{
				if ( pProjectile->m_iTeamNum( ) == TEAM_RED )
					return vars.colors.outlines.world.outlines_world_red;
				else if ( pProjectile->m_iTeamNum( ) == TEAM_BLU )
					return vars.colors.outlines.world.outlines_world_blu;
				else
					return vars.colors.outlines.world.outlines_world_default;
			}

			return vars.colors.outlines.world.outlines_world_default;
		}

		inline color get_world_color( C_BaseEntity* entity, C_TFPlayer* local )
		{
			const auto& name = FNV1A::Hash( i::model_info->GetModelName( entity->GetModel( ) ) );
			switch ( entity->GetClassID( ) )
			{
				case ETFClassID::CBaseAnimating:
				{
					if ( entity_cache.is_health( name ) )
					{
						return vars.colors.outlines.world.outlines_world_health;
						break;
					}

					if ( entity_cache.is_ammo( name ) )
					{
						return vars.colors.outlines.world.outlines_world_ammo;
						break;
					}
				}

				case ETFClassID::CTFAmmoPack:
				{
					return vars.colors.outlines.world.outlines_world_ammo;
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
					return get_projectile_color( entity->As<C_BaseGrenade>( ), local );
					break;
				}

				case ETFClassID::CTFPumpkinBomb:
				case ETFClassID::CTFGenericBomb:
				{
					return vars.colors.outlines.world.outlines_world_bomb;
					break;
				}

				case ETFClassID::CCaptureFlag:
				{
					if ( vars.colors.outlines.world.outlines_world_color_scheme == 0 )
					{
						if ( entity->m_iTeamNum( ) != local->m_iTeamNum( ) )
							return vars.colors.outlines.world.outlines_world_enemy;
						else if ( entity->m_iTeamNum( ) == local->m_iTeamNum( ) )
							return vars.colors.outlines.world.outlines_world_teammate;
						else
							return vars.colors.outlines.world.outlines_world_default;
					}

					if ( vars.colors.outlines.world.outlines_world_color_scheme == 1 )
					{
						if ( entity->m_iTeamNum( ) == TEAM_RED )
							return vars.colors.outlines.world.outlines_world_red;
						else if ( entity->m_iTeamNum( ) == TEAM_BLU )
							return vars.colors.outlines.world.outlines_world_blu;
						else
							return vars.colors.outlines.world.outlines_world_default;
					}
					break;
				}

				default: return vars.colors.outlines.world.outlines_world_default;
			}
			return vars.colors.outlines.world.outlines_world_default;
		}
	};
	__outlines outlines;
};
inline __colors colors;