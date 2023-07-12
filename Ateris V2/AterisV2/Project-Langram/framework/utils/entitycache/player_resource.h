#pragma once
#include "../netvars/netvars.h"

class C_PlayerResource {
public:
	NETVAR( m_iPing, void *, "CPlayerResource", "m_iPing" );
	NETVAR( m_iScore, void *, "CPlayerResource", "m_iScore" );
	NETVAR( m_iDeaths, void *, "CPlayerResource", "m_iDeaths" );
	NETVAR( m_bConnected, void *, "CPlayerResource", "m_bConnected" );
	NETVAR( m_iTeam, void *, "CPlayerResource", "m_iTeam" );
	NETVAR( m_bAlive, void *, "CPlayerResource", "m_bAlive" );
	NETVAR( m_iHealth, void *, "CPlayerResource", "m_iHealth" );
	NETVAR( m_iAccountID, void *, "CPlayerResource", "m_iAccountID" );
	NETVAR( m_bValid, void *, "CPlayerResource", "m_bValid" );
	NETVAR( m_iUserID, void *, "CPlayerResource", "m_iUserID" );
};

class C_TFPlayerResource : public C_PlayerResource {
public:
	NETVAR( m_iTotalScore, void *, "CTFPlayerResource", "m_iTotalScore" );
	NETVAR( m_iMaxHealth, void *, "CTFPlayerResource", "m_iMaxHealth" );
	NETVAR( m_iMaxBuffedHealth, void *, "CTFPlayerResource", "m_iMaxBuffedHealth" );
	NETVAR( m_iPlayerClass, void *, "CTFPlayerResource", "m_iPlayerClass" );
	NETVAR( m_bArenaSpectator, void *, "CTFPlayerResource", "m_bArenaSpectator" );
	NETVAR( m_iActiveDominations, void *, "CTFPlayerResource", "m_iActiveDominations" );
	NETVAR( m_flNextRespawnTime, void *, "CTFPlayerResource", "m_flNextRespawnTime" );
	NETVAR( m_iChargeLevel, void *, "CTFPlayerResource", "m_iChargeLevel" );
	NETVAR( m_iDamage, void *, "CTFPlayerResource", "m_iDamage" );
	NETVAR( m_iDamageAssist, void *, "CTFPlayerResource", "m_iDamageAssist" );
	NETVAR( m_iDamageBoss, void *, "CTFPlayerResource", "m_iDamageBoss" );
	NETVAR( m_iHealing, void *, "CTFPlayerResource", "m_iHealing" );
	NETVAR( m_iHealingAssist, void *, "CTFPlayerResource", "m_iHealingAssist" );
	NETVAR( m_iDamageBlocked, void *, "CTFPlayerResource", "m_iDamageBlocked" );
	NETVAR( m_iCurrencyCollected, void *, "CTFPlayerResource", "m_iCurrencyCollected" );
	NETVAR( m_iBonusPoints, void *, "CTFPlayerResource", "m_iBonusPoints" );
	NETVAR( m_iPlayerLevel, void *, "CTFPlayerResource", "m_iPlayerLevel" );
	NETVAR( m_iStreaks, void *, "CTFPlayerResource", "m_iStreaks" );
	NETVAR( m_iUpgradeRefundCredits, void *, "CTFPlayerResource", "m_iUpgradeRefundCredits" );
	NETVAR( m_iBuybackCredits, void *, "CTFPlayerResource", "m_iBuybackCredits" );
	NETVAR( m_iPartyLeaderRedTeamIndex, int, "CTFPlayerResource", "m_iPartyLeaderRedTeamIndex" );
	NETVAR( m_iPartyLeaderBlueTeamIndex, int, "CTFPlayerResource", "m_iPartyLeaderBlueTeamIndex" );
	NETVAR( m_iEventTeamStatus, int, "CTFPlayerResource", "m_iEventTeamStatus" );
	NETVAR( m_iPlayerClassWhenKilled, void *, "CTFPlayerResource", "m_iPlayerClassWhenKilled" );
	NETVAR( m_iConnectionState, void *, "CTFPlayerResource", "m_iConnectionState" );
	NETVAR( m_flConnectTime, void *, "CTFPlayerResource", "m_flConnectTime" );

	/* CPlayerResource */

	int get_ping( int idx )
	{
		if ( !this ) { return 0; }
		static auto offset = netvars.find_netvar( "CTFPlayerResource", "m_iPing" );
		return *reinterpret_cast< int * >( reinterpret_cast<DWORD>( this ) + offset + 4 * idx );
	}

	int get_kills( int idx )
	{
		if ( !this ) { return 0; }
		static auto offset = netvars.find_netvar( "CTFPlayerResource", "m_iScore" );
		return *reinterpret_cast< int * >( reinterpret_cast<DWORD>( this ) + offset + 4 * idx );
	}

	int get_deaths( int idx )
	{
		if ( !this ) { return 0; }
		static auto offset = netvars.find_netvar( "CTFPlayerResource", "m_iDeaths" );
		return *reinterpret_cast< int * >( reinterpret_cast<DWORD>( this ) + offset + 4 * idx );
	}

	bool get_connected( int idx )
	{
		if ( !this ) { return false; }
		static auto offset = netvars.find_netvar( "CTFPlayerResource", "m_bConnected" );
		return *reinterpret_cast< bool * >( reinterpret_cast<DWORD>( this ) + offset + idx );
	}

	int get_team( int idx )
	{
		if ( !this ) { return 0; }
		static auto offset = netvars.find_netvar( "CTFPlayerResource", "m_iTeam" );
		return *reinterpret_cast< int * >( reinterpret_cast<DWORD>( this ) + offset + 4 * idx );
	}

	bool is_alive( int idx )
	{
		if ( !this ) { return false; }
		static auto offset = netvars.find_netvar( "CTFPlayerResource", "m_bAlive" );
		return *reinterpret_cast< bool * >( reinterpret_cast<DWORD>( this ) + offset + idx );
	}

	int get_health( int idx )
	{
		if ( !this ) { return 0; }
		static auto offset = netvars.find_netvar( "CTFPlayerResource", "m_iHealth" );
		return *reinterpret_cast< int * >( reinterpret_cast<DWORD>( this ) + offset + 4 * idx );
	}

	unsigned get_account_id( int idx )
	{
		if ( !this ) { return 0; }
		static auto offset = netvars.find_netvar( "CTFPlayerResource", "m_iAccountID" );
		return *reinterpret_cast< int * >( reinterpret_cast<DWORD>( this ) + offset + 4 * idx );
	}

	bool get_valid( int idx )
	{
		if ( !this ) { return false; }
		static auto offset = netvars.find_netvar( "CTFPlayerResource", "m_bValid" );
		return *reinterpret_cast< bool * >( reinterpret_cast<DWORD>( this ) + offset + idx );
	}

	int get_user_id( int idx )
	{
		if ( !this ) { return 0; }
		static auto offset = netvars.find_netvar( "CTFPlayerResource", "m_iUserID" );
		return *reinterpret_cast< int * >( reinterpret_cast<DWORD>( this ) + offset + 4 * idx );
	}

	const char *get_name( int idx )
	{
		if ( !this ) { return ""; }
		return *reinterpret_cast< const char ** >( this + 0x0554 + 4 * idx );
	}

	/* CTFPlayerResource */

	int get_score( int idx )
	{
		if ( !this ) { return 0; }
		static auto offset = netvars.find_netvar( "CTFPlayerResource", "m_iTotalScore" );
		return *reinterpret_cast< int * >( reinterpret_cast<DWORD>( this ) + offset + 4 * idx );
	}

	int get_level( int idx )
	{
		if ( !this ) { return 0; }
		static auto offset = netvars.find_netvar( "CTFPlayerResource", "m_iPlayerLevel" );
		return *reinterpret_cast< int * >( reinterpret_cast<DWORD>( this ) + offset + 4 * idx );
	}

	int get_damage( int idx )
	{
		if ( !this ) { return 0; }
		static auto offset = netvars.find_netvar( "CTFPlayerResource", "m_iDamage" );
		return *reinterpret_cast< int * >( reinterpret_cast<DWORD>( this ) + offset + 4 * idx );
	}

	int get_class( int idx )
	{
		if ( !this ) { return 0; }
		static auto offset = netvars.find_netvar( "CTFPlayerResource", "m_iPlayerClass" );
		return *reinterpret_cast< int * >( reinterpret_cast<DWORD>( this ) + offset + 4 * idx );
	}

	int get_max_health( int idx )
	{
		if ( !this ) { return 0; }
		static auto offset = netvars.find_netvar( "CTFPlayerResource", "m_iMaxHealth" );
		return *reinterpret_cast< int * >( reinterpret_cast<DWORD>( this ) + offset + 4 * idx );
	}

	int *get_streaks( int idx )
	{
		if ( !this ) { return nullptr; }
		static auto offset = netvars.find_netvar( "CTFPlayerResource", "m_iStreaks" );
		return reinterpret_cast< int * >( reinterpret_cast<DWORD>( this ) + offset + 4 * idx );
	}

	float get_connection_time( int idx )
	{
		if ( !this ) { return 0.f; }
		static auto offset = netvars.find_netvar( "CTFPlayerResource", "m_flConnectTime" );
		return *reinterpret_cast< float * >( reinterpret_cast<DWORD>( this ) + offset + 4 * idx );
	}
};