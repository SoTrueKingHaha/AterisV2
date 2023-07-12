#pragma once
#include "../../framework.h"
#include <vadefs.h>

enum class eColor
{
	BLACK,
	DARKBLUE,
	DARKGREEN,
	DARKCYAN,
	DARKRED,
	DARKPINK,
	DARKYELLOW,
	LIGHTGREY,
	DARKGREY,
	BLUE,
	GREEN,
	CYAN,
	RED,
	PINK,
	YELLOW,
	WHITE
};

class __console
{
	bool bConsoleVisible = false;
	const char *console_name;
public:

	void create_console( const char *chWindowName );
	void close_console( );
	static void set_text_color( eColor eColour );
	void set_visible( bool bVisible );
	bool get_visible( ) const;
	void print( std::string chText, ... ) const;
	void print( eColor eColour, std::string chText, ... ) const;
};
inline __console console;