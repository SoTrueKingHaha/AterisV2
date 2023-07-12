#pragma once
#include "../../framework.h"

#define INRANGE(x, a, b) (x >= a && x <= b) 
#define GET_BITS(x) (INRANGE((x & (~0x20)),'A','F') ? ((x & (~0x20)) - 'A' + 0xA) : (INRANGE(x,'0','9') ? x - '0' : 0))
#define GET_BYTES(x) (GET_BITS(x[0]) << 4 | GET_BITS(x[1]))

class __memory
{
public:
	//std::uintptr_t FindSignature(const char* module, const char* search);
	void *find_signature( const char *module, const char *search );

	inline void *get_vfunc( void *ptr, std::size_t index )
	{
		return reinterpret_cast< void * >( ( *static_cast< int ** >( ptr ) )[ index ] );
	}

	inline void **get_vfunc_table( void *instance, size_t offset )
	{
		return *reinterpret_cast< void *** >( reinterpret_cast< size_t >( instance ) + offset );
	}

	inline const void **get_vfunc_table( const void *instance, size_t offset )
	{
		return *reinterpret_cast< const void *** >( reinterpret_cast< size_t >( instance ) + offset );
	}

	template<typename T>
	inline T get_vfunc( const void *instance, size_t index, size_t offset = 0 )
	{
		return reinterpret_cast< T >( get_vfunc_table( instance, offset )[ index ] );
	}

	inline std::uintptr_t relative_to_absolute( std::uintptr_t address )
	{
		return *reinterpret_cast< std::uintptr_t * >( address + 1 ) + address + 5;
	}

	void *find_interface( const char *const dll, const char *const version );
};
inline __memory *memory;