#include "interfaces.h"

game_interface_t::game_interface_t( const char *name, void **pointer, const char *dll, const char *sig, int offset, int dereferences )
{
	this->name = name;
	this->ptr = pointer;
	this->dll = dll;
	this->sig = sig;
	this->offset = offset;
	this->deref = dereferences;

	interface_manager.interfaces.push_back( this );
}

bool __interface_manager::init( )
{
	for ( const auto &iface : interfaces )
	{
		if ( !iface->ptr || !iface->name || !iface->sig )
		{
			continue;
		}

		if ( iface->offset == -1 )
		{
			*iface->ptr = memory->find_interface( iface->dll, iface->sig );
			if ( !iface->ptr )
			{
				MessageBoxA( 0, iface->sig, iface->name, MB_ICONERROR );
				return false;
			}
		}
		else
		{
			auto address = reinterpret_cast< DWORD >( memory->find_signature( iface->dll, iface->sig ) );
			if ( !address )
			{
				MessageBoxA( 0, iface->sig, iface->name, MB_ICONERROR );
				return false;
			}

			*iface->ptr = reinterpret_cast< void * >( address + iface->offset );

			for ( int deref = 0; deref < iface->deref; deref++ )
			{
				if ( iface->ptr )
				{
					*iface->ptr = *reinterpret_cast< void ** >( *iface->ptr );
				}
			}
		}
	}

	if ( !i::SteamClient )
	{
		return false;
	}

	const auto &hSteamPipe = i::SteamClient->CreateSteamPipe( );

	if ( !hSteamPipe )
	{
		return false;
	}

	const auto &hSteamUser = i::SteamClient->ConnectToGlobalUser( hSteamPipe );

	if ( !hSteamUser )
	{
		return false;
	}

	i::SteamFriends02 = reinterpret_cast< ISteamFriends002 * >( i::SteamClient->GetISteamFriends( hSteamUser, hSteamPipe, STEAMFRIENDS_INTERFACE_VERSION_002 ) );
	i::SteamFriends15 = reinterpret_cast< ISteamFriends015 * >( i::SteamClient->GetISteamFriends( hSteamUser, hSteamPipe, STEAMFRIENDS_INTERFACE_VERSION_015 ) );
	i::SteamApps = reinterpret_cast< ISteamApps006 * >( i::SteamClient->GetISteamApps( hSteamUser, hSteamPipe, STEAMAPPS_INTERFACE_VERSION_006 ) );
	i::SteamUserStats = reinterpret_cast< ISteamUserStats011 * >( i::SteamClient->GetISteamUserStats( hSteamUser, hSteamPipe, STEAMUSERSTATS_INTERFACE_VERSION_011 ) );
	i::SteamUser = reinterpret_cast< ISteamUser017 * >( i::SteamClient->GetISteamUser( hSteamUser, hSteamPipe, STEAMUSER_INTERFACE_VERSION_017 ) );
	i::SteamUtils = reinterpret_cast< ISteamUtils007 * >( i::SteamClient->GetISteamUtils( hSteamPipe, STEAMUTILS_INTERFACE_VERSION_007 ) );

	static auto GetNetworkingUtils = reinterpret_cast< ISteamNetworkingUtils * ( __cdecl * )( ) >( GetProcAddress( GetModuleHandleA( "steamnetworkingsockets.dll" ), "SteamNetworkingUtils_LibV4" ) );
	i::SteamNetworkingUtils = GetNetworkingUtils( );

	if ( !( i::SteamFriends02 && i::SteamFriends15 && i::SteamApps && i::SteamUserStats && i::SteamUser && i::SteamUtils && i::SteamNetworkingUtils ) )
	{
		return false;
	}

	return true;
}