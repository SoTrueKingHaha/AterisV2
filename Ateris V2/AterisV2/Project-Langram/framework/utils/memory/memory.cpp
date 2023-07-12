#include "memory.h"

void *__memory::find_signature( const char *module, const char *search )
{
	void *hModule = GetModuleHandleA( module );

	static auto PatternToVectorInts = [ ] ( const char *pattern )
	{
		auto bytes = std::vector<int>{};
		auto start = const_cast< char * >( pattern );
		auto end = const_cast< char * >( pattern ) + strlen( pattern );

		for ( auto current = start; current < end; ++current )
		{
			if ( *current == '?' )
			{
				++current;
				if ( *current == '?' )
					++current;
				bytes.push_back( -1 );
			}
			else
			{
				bytes.push_back( strtoul( current, &current, 16 ) );
			}
		}
		return bytes;
	};

	auto pImageDOSHeaders = reinterpret_cast< PIMAGE_DOS_HEADER >( hModule );

	if ( !pImageDOSHeaders )
	{
		return nullptr;
	}

	auto pNTHeaders = reinterpret_cast< PIMAGE_NT_HEADERS >( reinterpret_cast< std::uint8_t * >( hModule ) + pImageDOSHeaders->e_lfanew );
	const auto dwImageSize = pNTHeaders->OptionalHeader.SizeOfImage;
	auto vecPatternBytes = PatternToVectorInts( search );
	auto pbScannedBytes = reinterpret_cast< std::uint8_t * >( hModule );

	auto nPatternBytesSize = vecPatternBytes.size( );
	auto pPatternBytes = vecPatternBytes.data( );

	for ( auto i = 0ul; i < dwImageSize - nPatternBytesSize; ++i )
	{
		bool bPatternFound = true;
		for ( auto j = 0ul; j < nPatternBytesSize; ++j )
		{
			if ( pbScannedBytes[ i + j ] != pPatternBytes[ j ] && pPatternBytes[ j ] != -1 )
			{
				bPatternFound = false;
				break;
			}
		}
		if ( bPatternFound )
		{
			return &pbScannedBytes[ i ];
		}
	}
	return 0x0;
}

void *__memory::find_interface( const char *const dll, const char *const version )
{
	HMODULE hModule = GetModuleHandleA( dll );
	if ( hModule )
	{
		auto CreateInterface = reinterpret_cast< void *( __cdecl * )( const char *, int * ) >( GetProcAddress( hModule, "CreateInterface" ) );
		int nReturnCode = 0;
		void *pInterface = CreateInterface( version, &nReturnCode );
		if ( pInterface )
		{
			return pInterface;
		}
	}
	return nullptr;
}