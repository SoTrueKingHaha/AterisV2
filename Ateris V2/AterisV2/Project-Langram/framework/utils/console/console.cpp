#include "console.h"

void __console::create_console( const char *chWindowName )
{
	AllocConsole( );

	auto pMenu = GetSystemMenu( GetConsoleWindow( ), 0 );

	if ( pMenu == nullptr )
		return;

	//DeleteMenu( pMenu, 0xF060, MF_BYCOMMAND ); // Delete Close button on taskbar for console

	_iobuf *pData;
	freopen_s( &pData, "CONIN$", "r", __acrt_iob_func( 0 ) );
	freopen_s( &pData, "CONOUT$", "w", __acrt_iob_func( 1 ) );

	SetConsoleTitleA( chWindowName );

	this->console_name = chWindowName;
	this->bConsoleVisible = true;
}

void __console::close_console( )
{
	FreeConsole( );
	HWND console = FindWindowA( 0, this->console_name );
	PostMessageA( console, WM_QUIT, 0, 0 );
}

void __console::set_text_color( eColor eColor )
{
	SetConsoleTextAttribute( GetStdHandle( static_cast< DWORD >( -11 ) ), *reinterpret_cast< WORD * >( &eColor ) );
}

void __console::set_visible( bool bVisible )
{
	this->bConsoleVisible = bVisible;
}

bool __console::get_visible( ) const
{
	return this->bConsoleVisible;
}

void __console::print( std::string text, ... ) const
{
	if ( !get_visible( ) )
		return;

	char chMergedBuffer[ 1024 ];
	va_list vlist;
	( void )( __vcrt_assert_va_start_is_not_reference<decltype( text )>( ), ( void )( vlist = static_cast< va_list >( &const_cast< char & >( reinterpret_cast< const volatile char & >( text ) ) ) + ( sizeof text + sizeof( int ) - 1 & ~( sizeof( int ) - 1 ) ) ) );
	vsprintf_s( chMergedBuffer, text.c_str( ), vlist );
	( void )( vlist = static_cast< va_list >( nullptr ) );

	// Now lets print the merged string
	set_text_color( eColor::WHITE );
	printf( "%s", chMergedBuffer );
	set_text_color( eColor::WHITE );
}

void __console::print( eColor eColor, std::string text, ... ) const
{
	if ( !get_visible( ) )
		return;

	char chMergedBuffer[ 1024 ];
	va_list vlist;
	( void )( __vcrt_assert_va_start_is_not_reference<decltype( text )>( ), ( void )( vlist = static_cast< va_list >( &const_cast< char & >( reinterpret_cast< const volatile char & >( text ) ) ) + ( sizeof text + sizeof( int ) - 1 & ~( sizeof( int ) - 1 ) ) ) );
	vsprintf_s( chMergedBuffer, text.c_str( ), vlist );
	( void )( vlist = static_cast< va_list >( nullptr ) );

	// Now lets print the merged string
	set_text_color( eColor );
	printf( "%s", chMergedBuffer );
	set_text_color( eColor::WHITE );
}