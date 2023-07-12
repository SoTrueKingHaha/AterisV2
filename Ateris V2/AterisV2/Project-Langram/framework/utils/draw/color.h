#pragma once
#include <string>
#include <sstream>
#include <algorithm>
#pragma warning( disable : 4244 )
struct color {
	unsigned char r, g, b, a;
	[[nodiscard]] std::string to_hex( ) const
	{
		std::stringstream ss;
		ss << "\x7";
		ss << std::hex << ( static_cast< int >( r ) << 16 | static_cast< int >( g ) << 8 | static_cast< int >( b ) );
		return ss.str( );
	}

	[[nodiscard]] std::string to_hex_alpha( ) const
	{
		std::stringstream ss;
		ss << "\x8";
		ss << std::hex << ( static_cast< int >( r ) << 16 | static_cast< int >( g ) << 8 | static_cast< int >( b ) );
		ss << std::hex << static_cast< int >( a );
		return ss.str( );
	}
};

inline color HSVtoColor( float H, float S, float V )
{
	if ( H > 360 || H < 0 || S>100 || S < 0 || V>100 || V < 0 )
	{
		return { 0, 0, 0, 0 };
	}
	float s = S / 100;
	float v = V / 100;
	float C = s * v;
	float X = C * ( 1 - ( float )fabs( fmod( H / 60.0f, 2 ) - 1 ) );
	float m = v - C;
	float r, g, b;
	if ( H >= 0 && H < 60 )
	{
		r = C, g = X, b = 0;
	}
	else if ( H >= 60 && H < 120 )
	{
		r = X, g = C, b = 0;
	}
	else if ( H >= 120 && H < 180 )
	{
		r = 0, g = C, b = X;
	}
	else if ( H >= 180 && H < 240 )
	{
		r = 0, g = X, b = C;
	}
	else if ( H >= 240 && H < 300 )
	{
		r = X, g = 0, b = C;
	}
	else
	{
		r = C, g = 0, b = X;
	}
	unsigned char R = static_cast< unsigned char >( ( r + m ) * 255 );
	unsigned char G = static_cast< unsigned char >( ( g + m ) * 255 );
	unsigned char B = static_cast< unsigned char >( ( b + m ) * 255 );

	return { R, G, B, 255 };
}
#undef min
#undef max
inline void CLR2HSV( color& col, float* hsv )
{
	float r = col.r / 255.f;
	float g = col.g / 255.f;
	float b = col.b / 255.f;

	float cmax = std::max( r, std::max( g, b ) );
	float cmin = std::min( r, std::min( g, b ) );
	float diff = cmax - cmin;
	float h = -1, s = -1;

	if ( cmax == cmin )
		h = 0;

	// if cmax equal r then compute h
	else if ( cmax == r )
		h = fmod( 60 * ( ( g - b ) / diff ) + 360, 360 );

	// if cmax equal g then compute h
	else if ( cmax == g )
		h = fmod( 60 * ( ( b - r ) / diff ) + 120, 360 );

	// if cmax equal b then compute h
	else if ( cmax == b )
		h = fmod( 60 * ( ( r - g ) / diff ) + 240, 360 );

	// if cmax equal zero
	if ( cmax == 0 )
		s = 0;
	else
		s = ( diff / cmax ) * 100;

	float v = cmax * 100.f;

	hsv[ 0 ] = h;
	hsv[ 1 ] = s;
	hsv[ 2 ] = v;
}