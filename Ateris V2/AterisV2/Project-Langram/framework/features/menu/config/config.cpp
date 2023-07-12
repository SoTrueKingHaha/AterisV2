#include "config.h"

void C::Save( const char* name )
{
	m_sConfigPath = std::filesystem::current_path( ).string( ) + ( "\\langram\\" );
	if ( !std::filesystem::exists( m_sConfigPath ) )
	{
		std::filesystem::create_directory( m_sConfigPath );
	}

	std::ofstream outputFile( m_sConfigPath + name + ( ".json" ) );

	if ( !outputFile.is_open( ) )
	{
		return;
	}

	nlohmann::json j{};

	for ( const auto& Var : Vars )
	{
		if ( Var.m_nTypeHash == typeid( bool ).hash_code( ) )
		{
			j[ Var.m_szName ] = *static_cast< bool* >( Var.m_pValue );
		}

		if ( Var.m_nTypeHash == typeid( int ).hash_code( ) )
		{
			j[ Var.m_szName ] = *static_cast< int* >( Var.m_pValue );
		}

		if ( Var.m_nTypeHash == typeid( float ).hash_code( ) )
		{
			j[ Var.m_szName ] = *static_cast< float* >( Var.m_pValue );
		}

		if ( Var.m_nTypeHash == typeid( std::string ).hash_code( ) )
		{
			j[ Var.m_szName ] = *static_cast< std::string* >( Var.m_pValue );
		}

		if ( Var.m_nTypeHash == typeid( color ).hash_code( ) )
		{
			const auto clr = *static_cast< color* >( Var.m_pValue );
			j[ Var.m_szName ] = { clr.r, clr.g, clr.b, clr.a };
		}
	}

	outputFile << std::setw( 4 ) << j;

	outputFile.close( );
}

void C::Load( const char* name )
{
	m_sConfigPath = std::filesystem::current_path( ).string( ) + ( "\\langram\\" );
	if ( !std::filesystem::exists( m_sConfigPath ) )
	{
		std::filesystem::create_directory( m_sConfigPath );
	}

	std::ifstream inputFile( m_sConfigPath + name + ( ".json" ) );

	if ( !inputFile.is_open( ) )
	{
		return;
	}

	nlohmann::json j{};

	inputFile >> j;

	for ( const auto& Var : Vars )
	{
		if ( j.find( Var.m_szName ) == j.end( ) )
		{
			continue;
		}

		if ( Var.m_nTypeHash == typeid( bool ).hash_code( ) )
		{
			*static_cast< bool* >( Var.m_pValue ) = j[ Var.m_szName ];
		}

		if ( Var.m_nTypeHash == typeid( int ).hash_code( ) )
		{
			*static_cast< int* >( Var.m_pValue ) = j[ Var.m_szName ];
		}

		if ( Var.m_nTypeHash == typeid( float ).hash_code( ) )
		{
			*static_cast< float* >( Var.m_pValue ) = j[ Var.m_szName ];
		}

		if ( Var.m_nTypeHash == typeid( std::string ).hash_code( ) )
		{
			*static_cast< std::string* >( Var.m_pValue ) = j[ Var.m_szName ];
		}

		if ( Var.m_nTypeHash == typeid( color ).hash_code( ) )
		{
			color clr = { j[ Var.m_szName ][ 0 ], j[ Var.m_szName ][ 1 ], j[ Var.m_szName ][ 2 ], j[ Var.m_szName ][ 3 ] };
			*static_cast< color* >( Var.m_pValue ) = clr;
		}
	}

	inputFile.close( );
}