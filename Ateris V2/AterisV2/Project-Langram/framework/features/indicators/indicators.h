#pragma once
#include "../../utils/draw/draw.h"
#include "../../utils/utils.h"
#include "../../sdk/globals/globalinfo.h"
#include "../menu/vars.h"

struct Spectator_t
{
	C_TFPlayer* observer;
	C_TFPlayer* observed;
	color color;
};

class __indicators
{
	std::vector<Spectator_t>m_Spectators;

	void draw_dt( C_TFPlayer* local, C_TFWeaponBase* weapon );
	void draw_crits( C_TFPlayer* local, C_TFWeaponBase* weapon );
	void draw_aa( C_TFPlayer* local );
	void draw_bt( C_TFPlayer* local );
	void draw_spectators( C_TFPlayer* local );
	bool get_spectators( C_TFPlayer* local );
public:
	void render( );
};
inline __indicators indicators;