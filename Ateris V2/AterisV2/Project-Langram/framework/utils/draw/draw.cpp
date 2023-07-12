#include "draw.h"

text_size::text_size( efonts font, const char *text )
{
	this->font = &font;
	this->text = text;
	if ( i::surface )
	{
		draw.get_text_size( font, this->w, this->h, text );
	}
	text_sizes.push_back( this );
}

void __drawutils::get_text_size( const efonts &font, int &w, int &h, const char *str, ... )
{
	if ( !str )
	{
		return;
	}

	va_list va_alist;
	char cbuffer[ 1024 ] = { '\0' };
	wchar_t wstr[ 1024 ] = { '\0' };

	va_start( va_alist, str );
	vsprintf_s( cbuffer, str, va_alist );
	va_end( va_alist );

	wsprintfW( wstr, L"%hs", cbuffer );

	i::surface->GetTextSize( fonts[ font ].font, wstr, w, h );
}

void __drawutils::get_text_size( const efonts &font, int &w, int &h, const wchar_t *str, ... )
{
	if ( !str )
	{
		return;
	}

	va_list va_alist;
	wchar_t wstr[ 1024 ] = { '\0' };

	va_start( va_alist, str );
	vswprintf_s( wstr, str, va_alist );
	va_end( va_alist );

	i::surface->GetTextSize( fonts[ font ].font, wstr, w, h );
}

void __drawutils::reload_fonts( )
{
	fonts[ efonts::MENU ] = { "Tahoma", 13, 0 };
	fonts[ efonts::ESP ] = { "Verdana", 12, 0 };
	fonts[ efonts::ESPSMALL ] = { "Small Fonts", 10, 0 };
	fonts[ efonts::ESPVERYSMALL ] = { "Small Fonts", 8, 0 };
	fonts[ efonts::INDICATOR ] = { "Segoe UI Semilight", 18, 0, FONTFLAG_ANTIALIAS };

	for ( auto &v : fonts )
		i::surface->SetFontGlyphSet( v.second.font = i::surface->CreateFont( ), v.second.name, v.second.tall, v.second.weight, 0, 0, v.second.flags );
}

void __drawutils::update_screen_size( )
{
	i::engine_client->GetScreenSize( this->screen_width, this->screen_height );
	this->screen_wcenter = ( this->screen_width / 2 );
	this->screen_hcenter = ( this->screen_height / 2 );
}

void __drawutils::update_w2s_matrix( )
{
	CViewSetup view_setup = {};

	if ( i::client->GetPlayerView( view_setup ) ) {
		view_matrix world_to_view = {}, view_to_projection = {}, world_to_pixels = {};
		i::render_view->GetMatricesForView( view_setup, &world_to_view, &view_to_projection, &draw.world_to_projection, &world_to_pixels );
	}
}

void __drawutils::string( const efonts font, int x, int y, color clr, short align, const char *str, ... )
{
	if ( str == 0 )
		return;

	va_list va_alist;
	char cbuffer[ 1024 ] = { '\0' };
	wchar_t wstr[ 1024 ] = { '\0' };

	va_start( va_alist, str );
	vsprintf_s( cbuffer, str, va_alist );
	va_end( va_alist );

	wsprintfW( wstr, L"%hs", cbuffer );

	switch ( align )
	{
		case ALIGN_DEFAULT: break;
		case ALIGN_CENTER: {
			int w = 0, h = 0;
			i::surface->GetTextSize( fonts[ font ].font, wstr, w, h );
			x -= ( w / 2 );
			y -= ( h / 2 );
			break;
		}
		case ALIGN_CENTERVERTICAL: {
			int w = 0, h = 0;
			i::surface->GetTextSize( fonts[ font ].font, wstr, w, h );
			y -= ( h / 2 );
			break;
		}
		case ALIGN_CENTERHORIZONTAL: {
			int w = 0, h = 0;
			i::surface->GetTextSize( fonts[ font ].font, wstr, w, h );
			x -= ( w / 2 );
			break;
		}
		case ALIGN_REVERSE: {
			int w = 0, h = 0;
			i::surface->GetTextSize( fonts[ font ].font, wstr, w, h );
			x -= ( w );
			break;
		}
	}

	i::surface->DrawSetTextPos( x, y );
	i::surface->DrawSetTextFont( fonts[ font ].font );
	i::surface->DrawSetTextColor( clr.r, clr.g, clr.b, clr.a );
	i::surface->DrawPrintText( wstr, wcslen( wstr ) );
}

void __drawutils::string( const efonts font, int x, int y, color clr, short align, const wchar_t *str, ... )
{
	if ( str == 0 )
		return;

	va_list va_alist;
	wchar_t wstr[ 1024 ] = { '\0' };

	va_start( va_alist, str );
	vswprintf_s( wstr, str, va_alist );
	va_end( va_alist );

	switch ( align )
	{
		case ALIGN_DEFAULT: break;
		case ALIGN_CENTER: {
			int w = 0, h = 0;
			i::surface->GetTextSize( fonts[ font ].font, wstr, w, h );
			x -= ( w / 2 );
			y -= ( h / 2 );
			break;
		}
		case ALIGN_CENTERVERTICAL: {
			int w = 0, h = 0;
			i::surface->GetTextSize( fonts[ font ].font, wstr, w, h );
			y -= ( h / 2 );
			break;
		}
		case ALIGN_CENTERHORIZONTAL: {
			int w = 0, h = 0;
			i::surface->GetTextSize( fonts[ font ].font, wstr, w, h );
			x -= ( w / 2 );
			break;
		}
		case ALIGN_REVERSE: {
			int w = 0, h = 0;
			i::surface->GetTextSize( fonts[ font ].font, wstr, w, h );
			x -= ( w );
			break;
		}
	}

	i::surface->DrawSetTextPos( x, y );
	i::surface->DrawSetTextFont( fonts[ font ].font );
	i::surface->DrawSetTextColor( clr.r, clr.g, clr.b, clr.a );
	i::surface->DrawPrintText( wstr, wcslen( wstr ) );
}

void __drawutils::outlined_string( const efonts font, int x, int y, color clr, short align, const char *str, ... )
{
	if ( str == 0 )
		return;

	va_list va_alist;
	char cbuffer[ 1024 ] = { '\0' };
	wchar_t wstr[ 1024 ] = { '\0' };

	va_start( va_alist, str );
	vsprintf_s( cbuffer, str, va_alist );
	va_end( va_alist );

	wsprintfW( wstr, L"%hs", cbuffer );

	switch ( align )
	{
		case ALIGN_DEFAULT: break;
		case ALIGN_CENTER: {
			int w = 0, h = 0;
			i::surface->GetTextSize( fonts[ font ].font, wstr, w, h );
			x -= ( w / 2 );
			y -= ( h / 2 );
			break;
		}
		case ALIGN_CENTERVERTICAL: {
			int w = 0, h = 0;
			i::surface->GetTextSize( fonts[ font ].font, wstr, w, h );
			y -= ( h / 2 );
			break;
		}
		case ALIGN_CENTERHORIZONTAL: {
			int w = 0, h = 0;
			i::surface->GetTextSize( fonts[ font ].font, wstr, w, h );
			x -= ( w / 2 );
			break;
		}
		case ALIGN_REVERSE: {
			int w = 0, h = 0;
			i::surface->GetTextSize( fonts[ font ].font, wstr, w, h );
			x -= ( w );
			break;
		}
	}

	i::surface->DrawSetTextFont( fonts[ font ].font );
	//outline
	{
		//left
		i::surface->DrawSetTextPos( x - 1, y );
		i::surface->DrawSetTextColor( 0, 0, 0, 255 );
		i::surface->DrawPrintText( wstr, wcslen( wstr ) );
		//right
		i::surface->DrawSetTextPos( x + 1, y );
		i::surface->DrawPrintText( wstr, wcslen( wstr ) );
		//top
		i::surface->DrawSetTextPos( x, y - 1 );
		i::surface->DrawPrintText( wstr, wcslen( wstr ) );
		//bottom
		i::surface->DrawSetTextPos( x, y + 1 );
		i::surface->DrawPrintText( wstr, wcslen( wstr ) );
	}
	//text
	{
		i::surface->DrawSetTextPos( x, y );
		i::surface->DrawSetTextColor( clr.r, clr.g, clr.b, clr.a );
		i::surface->DrawPrintText( wstr, wcslen( wstr ) );
	}
}

void __drawutils::outlined_string( const efonts font, int x, int y, color clr, short align, const wchar_t *str, ... )
{
	if ( str == 0 )
		return;

	va_list va_alist;
	wchar_t wstr[ 1024 ] = { '\0' };

	va_start( va_alist, str );
	vswprintf_s( wstr, str, va_alist );
	va_end( va_alist );

	switch ( align )
	{
		case ALIGN_DEFAULT: break;
		case ALIGN_CENTER: {
			int w = 0, h = 0;
			i::surface->GetTextSize( fonts[ font ].font, wstr, w, h );
			x -= ( w / 2 );
			y -= ( h / 2 );
			break;
		}
		case ALIGN_CENTERVERTICAL: {
			int w = 0, h = 0;
			i::surface->GetTextSize( fonts[ font ].font, wstr, w, h );
			y -= ( h / 2 );
			break;
		}
		case ALIGN_CENTERHORIZONTAL: {
			int w = 0, h = 0;
			i::surface->GetTextSize( fonts[ font ].font, wstr, w, h );
			x -= ( w / 2 );
			break;
		}
		case ALIGN_REVERSE: {
			int w = 0, h = 0;
			i::surface->GetTextSize( fonts[ font ].font, wstr, w, h );
			x -= ( w );
			break;
		}
	}

	i::surface->DrawSetTextFont( fonts[ font ].font );
	//outline
	{
		//left
		i::surface->DrawSetTextPos( x - 1, y );
		i::surface->DrawSetTextColor( 0, 0, 0, 255 );
		i::surface->DrawPrintText( wstr, wcslen( wstr ) );
		//right
		i::surface->DrawSetTextPos( x + 1, y );
		i::surface->DrawPrintText( wstr, wcslen( wstr ) );
		//top
		i::surface->DrawSetTextPos( x, y - 1 );
		i::surface->DrawPrintText( wstr, wcslen( wstr ) );
		//bottom
		i::surface->DrawSetTextPos( x, y + 1 );
		i::surface->DrawPrintText( wstr, wcslen( wstr ) );
	}
	//text
	{
		i::surface->DrawSetTextPos( x, y );
		i::surface->DrawSetTextColor( clr.r, clr.g, clr.b, clr.a );
		i::surface->DrawPrintText( wstr, wcslen( wstr ) );
	}
}

void __drawutils::line( int x, int y, int x1, int y1, color clr )
{
	i::surface->DrawSetColor( clr.r, clr.g, clr.b, clr.a );
	i::surface->DrawLine( x, y, x1, y1 );
}

void __drawutils::draw_textured_polygon( int count, Vertex_t *vertices, const color &clr )
{
	static int iTexture = i::surface->CreateNewTextureID( true );

	i::surface->DrawSetColor( clr.r, clr.g, clr.b, clr.a );
	i::surface->DrawSetTexture( iTexture );

	i::surface->DrawTexturedPolygon( count, vertices );
}

void __drawutils::dotted_line( int x, int y, int x1, int y1, color clr )
{
	i::surface->DrawSetColor( clr.r, clr.g, clr.b, clr.a );
	i::surface->DrawLine( x, y, x1, y1 );
	i::surface->DrawLine( x, y, x - 20, y + 10 );
}

void __drawutils::triangle( int x, int y, int range, int y1, color clr )
{
	i::surface->DrawSetColor( clr.r, clr.g, clr.b, clr.a );
	i::surface->DrawLine( x, y, x + range, y1 );
	i::surface->DrawLine( x, y, x - range, y1 );
	i::surface->DrawLine( x - range, y1, x + range + 1, y1 );
}

void __drawutils::rect( int x, int y, int w, int h, color clr )
{
	i::surface->DrawSetColor( clr.r, clr.g, clr.b, clr.a );
	i::surface->DrawFilledRect( x, y, x + w, y + h );
}

void __drawutils::outlined_rect( int x, int y, int w, int h, color clr )
{
	i::surface->DrawSetColor( clr.r, clr.g, clr.b, clr.a );
	i::surface->DrawOutlinedRect( x, y, x + w, y + h );
}

void __drawutils::black_outlined_rect( int x, int y, int w, int h, color clr )
{
	i::surface->DrawSetColor( clr.r, clr.g, clr.b, clr.a );
	i::surface->DrawOutlinedRect( x, y, x + w, y + h );
	i::surface->DrawSetColor( { 0, 0, 0, 200 } );
	i::surface->DrawOutlinedRect( x - 1, y - 1, x + w + 1, y + h + 1 );
	i::surface->DrawOutlinedRect( x + 1, y + 1, x + w - 1, y + h - 1 );
}

/*void __drawutils::gradient_rect( int x, int y, int x1, int y1, Color_t top_clr, Color_t bottom_clr, bool horizontal )
{
	i::MatSystemSurface->DrawSetColor( top_clr.r, top_clr.g, top_clr.b, top_clr.a );
	i::MatSystemSurface->DrawFilledRectFade( x, y, x1, y1, 255, 255, horizontal );

	i::MatSystemSurface->DrawSetColor( bottom_clr.r, bottom_clr.g, bottom_clr.b, bottom_clr.a );
	i::MatSystemSurface->DrawFilledRectFade( x, y, x1, y1, 0, 255, horizontal );
}*/

void __drawutils::gradient_rect( int x, int y, int w, int h, color top_clr, color bottom_clr, bool horizontal )
{
	i::surface->DrawSetColor( top_clr.r, top_clr.g, top_clr.b, top_clr.a );
	i::surface->DrawFilledRectFade( x, y, x + w, y + h, 255, 255, horizontal );
	i::surface->DrawSetColor( bottom_clr.r, bottom_clr.g, bottom_clr.b, bottom_clr.a );
	i::surface->DrawFilledRectFade( x, y, x + w, y + h, 0, 255, horizontal );
}

void __drawutils::outlined_circle( int x, int y, float radius, int segments, const color &clr )
{
	float Step = PI * 2.0 / segments;

	for ( float a = 0; a < ( PI * 2.0 ); a += Step )
	{
		float x1 = radius * cos( a ) + x;
		float y1 = radius * sin( a ) + y;
		float x2 = radius * cos( a + Step ) + x;
		float y2 = radius * sin( a + Step ) + y;
		line( x1, y1, x2, y2, clr );
	}
}

void __drawutils::filled_circle( const int &x, const int &y, const float &radius, const int &segments, const color &clr )
{
	CUtlVector<Vertex_t> vecVertixes;

	const float flStep = ( 6.28318530718f / static_cast< float >( segments ) );

	for ( float n = 0.0f; n < 6.28318530718f; n += flStep )
		vecVertixes.AddToTail( Vertex_t( { ( static_cast< float >( radius ) * cos( n ) + x ), ( static_cast< float >( radius ) * sinf( n ) + y ) }, { 0.0f, 0.0f } ) );

	if ( vecVertixes.Count( ) > 0 )
	{
		i::surface->DrawSetColor( clr.r, clr.g, clr.b, clr.a );
		i::surface->DrawTexturedPolygon( segments, vecVertixes.Base( ) );
	}
}

int __drawutils::create_texture_from_array( const unsigned char *array, int width, int height )
{
	int iTextureOut = i::surface->CreateNewTextureID( true );
	i::surface->DrawSetTextureRGBAEx( iTextureOut, array, width, height, IMAGE_FORMAT_BGRA8888 );
	created_textures.push_back( iTextureOut );
	return iTextureOut;
}

void __drawutils::draw_texture_rect( int x, int y, int w, int h, int texture, color clr )
{
	i::surface->DrawSetColor( { clr.r, clr.g, clr.b, clr.a } );
	i::surface->DrawSetTexture( texture );
	i::surface->DrawTexturedRect( x, y, x + w, y + h );
}

void __drawutils::delete_textures_from_memory( )
{
	for ( const auto &texture : created_textures )
	{
		i::surface->DeleteTextureByID( texture );
	}
}

float __drawutils::ease_out( float start, float end, float speed )
{
	if ( start > end || speed <= 1 )
		return end;

	if ( timer( ) )
	{
		if ( start < end )
			return start * speed;
		return end;
	}
	return start;
}

float __drawutils::ease_in( float start, float end, float speed )
{
	if ( start < end || speed >= 1 )
		return end;

	if ( timer( ) )
	{
		if ( start > end )
			return start * speed;
		return end;
	}
	return start;
}

float __drawutils::linear( float start, float end, float speed )
{
	if ( start < end )
		return start + speed;
	return end;
}

bool __drawutils::timer( ) // This is to make sure that the animations don't get calculated in a split-second
{
	int t = clock( );
	static int i = 0;

	if ( t > i )
	{
		i += 1;
		return true;
	}
	return false;
}