#include "ienginetrace.h"
#include "c_tfplayer.h"

bool CTraceFilterHitscan::ShouldHitEntity( IHandleEntity *pEntityHandle, int nContentsMask )
{
	C_BaseEntity *pEntity = reinterpret_cast< C_BaseEntity * >( pEntityHandle );
	C_TFPlayer *pLocal = reinterpret_cast< C_TFPlayer * >( this->pSkip );

	switch ( pEntity->GetClassID( ) )
	{
		case ETFClassID::CFuncAreaPortalWindow:
		case ETFClassID::CFuncRespawnRoomVisualizer:
		case ETFClassID::CSniperDot:
		case ETFClassID::CTFMedigunShield:
		case ETFClassID::CTFReviveMarker: {
			return false;
		}
	}

	return ( pEntityHandle != pSkip );
}

TraceType_t CTraceFilterHitscan::GetTraceType( ) const
{
	return TRACE_EVERYTHING;
}

bool CTraceFilterWorldAndPropsOnly::ShouldHitEntity( IHandleEntity *pEntityHandle, int nContentsMask )
{
	C_BaseEntity *pEntity = reinterpret_cast< C_BaseEntity * >( pEntityHandle );

	switch ( pEntity->GetClassID( ) )
	{
		case ETFClassID::CFuncAreaPortalWindow:
		case ETFClassID::CFuncRespawnRoomVisualizer:
		case ETFClassID::CSniperDot:
		case ETFClassID::CTFMedigunShield:
		case ETFClassID::CTFReviveMarker:
		case ETFClassID::CTFAmmoPack:
		case ETFClassID::CTFProjectile_Arrow:
		case ETFClassID::CTFProjectile_BallOfFire:
		case ETFClassID::CTFProjectile_Cleaver:
		case ETFClassID::CTFProjectile_Jar:
		case ETFClassID::CTFProjectile_JarMilk:
		case ETFClassID::CTFProjectile_EnergyBall:
		case ETFClassID::CTFProjectile_EnergyRing:
		case ETFClassID::CTFProjectile_Flare:
		case ETFClassID::CTFProjectile_HealingBolt:
		case ETFClassID::CTFProjectile_MechanicalArmOrb:
		case ETFClassID::CTFProjectile_Rocket:
		case ETFClassID::CTFProjectile_SentryRocket:
		case ETFClassID::CTFGrenadePipebombProjectile:
		case ETFClassID::CTFPlayer:
		case ETFClassID::CMerasmus:
		case ETFClassID::CMerasmusDancer:
		case ETFClassID::CEyeballBoss:
		case ETFClassID::CHeadlessHatman:
		case ETFClassID::CZombie: {
			return false;
		} break;
		default:
			break;
	}

	return ( pEntityHandle != pSkip );
}

TraceType_t CTraceFilterWorldAndPropsOnly::GetTraceType( ) const
{
	return TRACE_EVERYTHING;
}