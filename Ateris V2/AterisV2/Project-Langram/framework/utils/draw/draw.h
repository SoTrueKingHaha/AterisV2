#pragma once
#include "color.h"
#include "../../sdk/interfaces/interfaces.h"
#include "../../framework.h"

class cfont
{
public:
	const char *name;
	int tall, weight, flags;
	DWORD font;
};

enum efonts {
	MENU,
	ESP,
	ESPSMALL,
	ESPVERYSMALL,
	INDICATOR
};

struct text_size
{
	efonts *font = nullptr;
	int w, h;
	const char *text;
	text_size( efonts font, const char *text );
};

enum EStringAlign
{
	ALIGN_DEFAULT,
	ALIGN_CENTER,
	ALIGN_CENTERVERTICAL,
	ALIGN_CENTERHORIZONTAL,
	ALIGN_REVERSE
};

inline std::map<efonts, cfont> fonts;
inline std::vector<text_size *> text_sizes;

class __drawutils
{
public:
	void reload_fonts( );
	void update_screen_size( );
	void update_w2s_matrix( );
	std::vector<int> created_textures = { };

public:
	int screen_width = 0, screen_height = 0, screen_wcenter = 0, screen_hcenter = 0;
	view_matrix world_to_projection;

public:
	void get_text_size( const efonts &font, int &w, int &h, const char *str, ... );
	void get_text_size( const efonts &font, int &w, int &h, const wchar_t *str, ... );
	void string( const efonts font, int x, int y, color clr, short align, const char *str, ... );
	void string( const efonts font, int x, int y, color clr, short align, const wchar_t *str, ... );
	void outlined_string( const efonts font, int x, int y, color clr, short align, const char *str, ... );
	void outlined_string( const efonts font, int x, int y, color clr, short align, const wchar_t *str, ... );
	void line( int x, int y, int x1, int y1, color clr );
	void draw_textured_polygon( int count, Vertex_t *vertices, const color &clr );
	void triangle( int x, int y, int range, int y1, color clr );
	void dotted_line( int x, int y, int x1, int y1, color clr );
	void rect( int x, int y, int w, int h, color clr );
	void outlined_rect( int x, int y, int w, int h, color clr );
	void black_outlined_rect( int x, int y, int w, int h, color clr );
	//void gradient_rect( int x, int y, int x1, int y1, Color_t top_clr, Color_t bottom_clr, bool horizontal );
	void gradient_rect( int x, int y, int w, int h, color top_clr, color bottom_clr, bool horizontal );
	void outlined_circle( int x, int y, float radius, int segments, const color &clr );
	void filled_circle( const int &x, const int &y, const float &radius, const int &segments, const color &clr );
	void Texture( int x, int y, int w, int h, const color &clr, int nIndex );
	void DrawHudTextureByName( float x0, float y0, float s0, const char *textureName, color col0 );
	//void DrawHudTexture( float x0, float y0, float s0, CHudTexture *texture, Color_t col0 );
	int create_texture_from_array( const unsigned char *array, int width, int height );
	void draw_texture_rect( int x, int y, int w, int h, int texture, color clr );
	void delete_textures_from_memory( );
	float ease_out( float start, float end, float speed );
	float ease_in( float start, float end, float speed );
	float linear( float start, float end, float speed );
	//CHudTexture *GetIcon( const char *szIcon, int eIconFormat = 0 );
	bool timer( );
};

inline __drawutils draw;