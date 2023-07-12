#include "netvars.h"

#include "../../sdk/interfaces/interfaces.h"

int __netvars::get_offset( RecvTable *table, const char *name )
{
	// Loop through props in RecvTable
	for ( int i = 0; i < table->m_nProps; i++ )
	{
		auto prop = table->m_pProps[ i ];

		if ( std::string_view( prop.m_pVarName ).compare( name ) == 0 ) // Matches our name
		{
			return prop.GetOffset( );
		}

		if ( auto datatable = prop.GetDataTable( ) )
		{
			auto offset = get_offset( datatable, name ); // Recursively loop through data table in prop

			if ( offset )
			{
				return offset + prop.GetOffset( );
			}
		}
	}

	return 0;
}

int __netvars::find_netvar( const char *classname, const char *netvar )
{
	auto classes = i::client->GetAllClasses( );

	// for (auto class : classes)
	for ( auto current = classes; current; current = current->m_pNext )
	{
		if ( std::string_view( classname ).compare( current->m_pNetworkName ) == 0 )
		{
			return get_offset( current->m_pRecvTable, netvar );
		}
	}

	return 0;
}

#ifdef NETVARS

#include <fstream>

std::ofstream File;
const char *szClassName;

void __netvars::dump_table( RecvTable *table, int depth, int classid )
{
	if ( !table )
	{
		return;
	}

	const char *Types[ 7 ] = { "int", "float", "Vector", "Vector2D", "const char *", "Array", "void *" };

	if ( depth == 0 )
	{
		File << "// Class ID: " << classid << "\n";
		File << "class " << szClassName << "\n{\npublic:\n";
	}

	for ( int n = 0; n < table->m_nProps; n++ )
	{
		RecvProp *pProp = &table->m_pProps[ n ];

		if ( !pProp )
		{
			continue;
		}

		std::string_view sVarName( pProp->m_pVarName );

		if ( !sVarName.find( "baseclass" ) || !sVarName.find( "0" ) || !sVarName.find( "1" ) || !sVarName.find( "2" ) )
		{
			continue;
		}

		const char *szType = Types[ pProp->m_RecvType ];

		if ( sVarName.find( "m_b" ) == 0 && pProp->m_RecvType == 0 )
		{
			szType = "bool";
		}

		if ( const auto &nOffset = pProp->GetOffset( ) )
		{
			File << "\tNETVAR(" << sVarName << ", " << szType << ", \"" << szClassName << "\", \"" << sVarName << "\"); //" << nOffset << "\n";
		}

		if ( auto DataTable = pProp->GetDataTable( ) )
		{
			dump_table( DataTable, depth + 1, classid );
		}
	}

	if ( depth == 0 )
	{
		File << "};\n";
	}
}

#include "../../sdk/interfaces/interfaces.h"

void __netvars::dump_tables( )
{
	File.open( "Netvar Dump.txt" );

	for ( ClientClass *pClass = i::client->GetAllClasses( ); pClass; pClass = pClass->m_pNext )
	{
		szClassName = pClass->m_pNetworkName;
		dump_table( pClass->m_pRecvTable, 0, pClass->m_ClassID );
	}

	File.close( );

	File.clear( );

	File.open( "ClassID Dump.txt" );

	File << "enum EClassID \n{\n";

	for ( ClientClass *pClass = i::client->GetAllClasses( ); pClass; pClass = pClass->m_pNext )
	{
		szClassName = pClass->m_pNetworkName;
		File << "\t" << szClassName << " = " << pClass->m_ClassID << ",\n";
	}

	File << "};";

	File.close( );
}

#endif

RecvProp* __netvars::get_netvar_prop( RecvTable* table, const char* name )
{
	// Loop through props in RecvTable
	for ( int i = 0; i < table->m_nProps; i++ )
	{
		RecvProp* prop = &table->m_pProps[ i ];

		if ( !prop )
		{
			continue;
		}

		if ( std::string_view( prop->m_pVarName ).compare( name ) == 0 ) // Matches our name
		{
			return prop;
		}

		if ( auto datatable = prop->GetDataTable( ) )
		{
			RecvProp* subprop = get_netvar_prop( datatable, name );

			if ( subprop && std::string_view( subprop->m_pVarName ).compare( name ) == 0 )
			{
				return subprop;
			}
		}
	}

	return nullptr;
}

RecvProp* __netvars::find_netvar_prop( const char* classname, const char* netvar )
{
	auto classes = i::client->GetAllClasses( );

	for ( auto current = classes; current; current = current->m_pNext )
	{
		if ( std::string_view( classname ).compare( current->m_pNetworkName ) == 0 )
		{
			return get_netvar_prop( current->m_pRecvTable, netvar );
		}
	}

	return nullptr;
}

void __netvars::init_hooks( )
{
	for ( const auto& hook : Hooks )
	{
		hook->init( );
	}
}

void __netvars::reset_hooks( )
{
	for ( const auto& hook : Hooks )
	{
		hook->reset( );
	}
}

netvar_hook::netvar_hook( const char* classname, const char* netvar, RecvVarProxyFn function )
{
	this->m_ClassName = classname;
	this->m_Netvar = netvar;
	this->m_Function = function;

	netvars.Hooks.push_back( this );
}

void netvar_hook::init( )
{
	RecvProp* prop = netvars.find_netvar_prop( m_ClassName, m_Netvar );

	this->m_Prop = prop;

	if ( !m_Prop )
	{
		std::string message = ( "Failed to find netvar %s::%s", m_ClassName, m_Netvar );
		MessageBoxA( 0, message.c_str( ), "Netvar Hooks", MB_ICONERROR );
		return;
	}

	//i::cvar->ConsolePrintf( "Original: %x\n", prop->m_ProxyFn );
	m_Original = prop->m_ProxyFn;
	m_Prop->SetProxyFn( m_Function );
	//i::cvar->ConsolePrintf( "Hooked %s::%s from %x to %x\n", m_ClassName, m_Netvar, m_Original, prop->m_ProxyFn );
}

void netvar_hook::reset( )
{
	m_Prop->SetProxyFn( m_Original );
}