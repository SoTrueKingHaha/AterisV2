#include "signatures.h"

signature_t::signature_t( const char *dll, const char *sig, const char *name, int offset )
{
	this->dll = dll;
	this->sig = sig;
	this->name = name;
	this->offset = offset;

	signature_manager.signatures.push_back( this );
}

bool __signature_manager::init( )
{
	for ( auto &signature : signature_manager.signatures )
	{
		signature->address = reinterpret_cast< DWORD >( memory->find_signature( signature->dll, signature->sig ) );

		assert( signature->address != 0 );

		if ( !signature->address )
		{
			std::string format = ( "Failed to find signature (%s): %s in %s!", signature->name, signature->sig, signature->dll );
			MessageBoxA( 0, format.c_str( ), "Signature Manager", MB_ICONERROR );
			return false;
		}

		signature->address += signature->offset;
	}

	return true;
}