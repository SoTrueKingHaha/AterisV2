#include "hooks.h"

hook::hook( const char *name, void *initFunction )
{
	init_fn = initFunction;
	name_fn = name;

	hook_manager.hooks.push_back( this );
}

void hook::create_hook( void *source, void *dest )
{
	auto status = MH_CreateHook( source, dest, &original_fn );
}

bool __hook_manager::init( )
{
	auto status = MH_Initialize( );
	if ( status != MH_OK )
	{
		return false;
	}

	for ( const auto &hook : hooks )
	{
		reinterpret_cast< void( __cdecl * )( ) >( hook->init_fn )( );
	}

	status = MH_EnableHook( MH_ALL_HOOKS );
	if ( status != MH_OK )
	{
		return false;
	}

	return true;
}

bool __hook_manager::release( )
{
	auto status = MH_Uninitialize( );
	if ( status != MH_OK )
	{
		return false;
	}

	return true;
}

bool __hook_manager::hook_netvar( std::vector<std::string> path, ProxyFnHook& hook, RecvVarProxyFn function )
{
	auto pClass = i::client->GetAllClasses( );

	if ( path.size( ) < 2 )
	{
		return false;
	}

	while ( pClass )
	{
		// Main class found
		if ( !strcmp( pClass->m_pRecvTable->m_pNetTableName, path[ 0 ].c_str( ) ) )
		{
			RecvTable* curr_table = pClass->m_pRecvTable;
			for ( size_t i = 1; i < path.size( ); i++ )
			{
				bool found = false;
				for ( int j = 0; j < curr_table->m_nProps; j++ )
				{
					RecvPropRedef* pProp = ( RecvPropRedef* )&( curr_table->m_pProps[ j ] );
					if ( !pProp )
						continue;
					if ( !strcmp( path[ i ].c_str( ), pProp->m_pVarName ) )
					{
						// Detect last iteration
						if ( i == path.size( ) - 1 )
						{
							hook.init( pProp );
							hook.setHook( function );
							return true;
						}
						curr_table = pProp->m_pDataTable;
						found = true;
					}
				}
				// We tried searching the netvar but found nothing
				if ( !found )
				{
					std::string full_path;
					for ( auto& s : path )
						full_path += s + "";
					std::string stringMessage( "Required member not found: " + full_path );
					const char* errorMessage = stringMessage.c_str( );
					MessageBoxA( 0, errorMessage, "Hooking netvar failed", MB_ICONERROR );
					//logging::Info("Hooking netvar with path \"%s\" failed. Required member not found.");
					return false;
				}
			}
		}

		pClass = pClass->m_pNext;
	}

	return false;
}