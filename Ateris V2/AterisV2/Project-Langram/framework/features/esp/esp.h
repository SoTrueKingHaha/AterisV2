#pragma once
#include "../../utils/entitycache/entitycache.h"
#include "../../utils/draw/draw.h"
#include "../../utils/utils.h"
#include "../../sdk/globals/globalinfo.h"
#include "../../sdk/game/c_captureflag.h"
#include "../menu/vars.h"

class __esp
{
	void render_players( C_TFPlayer* local );
	void render_buildings( C_TFPlayer* local );
	void render_world( C_TFPlayer* local, C_TFWeaponBase* weapon );
	float healthbar_yline = 0.f;
	std::array<float, MAX_PLAYERS * 2> health;
	std::array<float, MAX_PLAYERS * 8> health_buildings;

	void DrawPlayer( const vector& vecFromPos, const vector& vecToPos, color color, std::wstring name, std::wstring weapon, int health, int maxhealth, int distance );
	void DrawSentry( const vector& vecFromPos, const vector& vecToPos, color color, std::wstring name, int health, int maxhealth, int level, int distance );
public:
	void render( );
};
inline __esp esp;