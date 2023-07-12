#pragma once
#include "../../sdk/interfaces/interfaces.h"
#include "../../sdk/game/c_tfweaponbase.h"
#include "../../sdk/game/c_captureflag.h"
#include "FNV1A.h"
#include "../../hooks/hooks.h"
#include "player_resource.h"
#include "../draw/draw.h"

#define TF_AMMOPACK_SMALL_BDAY		"models/items/ammopack_small_bday.mdl"
#define TF_AMMOPACK_MEDIUM_BDAY		"models/items/ammopack_medium_bday.mdl"
#define TF_AMMOPACK_LARGE_BDAY		"models/items/ammopack_large_bday.mdl"
#define TF_AMMOPACK_SMALL "models/items/ammopack_small.mdl"
#define TF_AMMOPACK_MEDIUM "models/items/ammopack_medium.mdl"
#define TF_AMMOPACK_LARGE "models/items/ammopack_large.mdl"

#define TF_MEDKIT_SMALL_BDAY	"models/items/medkit_small_bday.mdl"
#define TF_MEDKIT_MEDIUM_BDAY	"models/items/medkit_medium_bday.mdl"
#define TF_MEDKIT_LARGE_BDAY	"models/items/medkit_large_bday.mdl"
#define TF_MEDKIT_SMALL_HALLOWEEN	"models/props_halloween/halloween_medkit_small.mdl"
#define TF_MEDKIT_MEDIUM_HALLOWEEN  "models/props_halloween/halloween_medkit_medium.mdl"
#define TF_MEDKIT_LARGE_HALLOWEEN	"models/props_halloween/halloween_medkit_large.mdl"
#define LUNCHBOX_DROP_MODEL  "models/items/plate.mdl"
#define LUNCHBOX_STEAK_DROP_MODEL  "models/items/plate_steak.mdl"
#define LUNCHBOX_ROBOT_DROP_MODEL  "models/items/plate_robo_sandwich.mdl"
#define LUNCHBOX_FESTIVE_DROP_MODEL  "models/items/plate_sandwich_xmas.mdl"
#define TF_MEDKIT_SMALL "models/items/medkit_small.mdl"
#define TF_MEDKIT_MEDIUM "models/items/medkit_medium.mdl"
#define TF_MEDKIT_LARGE "models/items/medkit_large.mdl"

enum eteams
{
	TEAMSALL = 0,
	ENEMY,
	TEAM
};

enum eworld
{
	WORLDALL = 0,
	HEALTH,
	AMMO,
	BOMBS
};

class __entity_cache
{
	C_TFPlayer* local = nullptr;
	C_TFWeaponBase* weapon = nullptr;
	C_TFPlayerResource* player_resource = nullptr;
public:
	std::unordered_map<eteams, std::vector<C_TFPlayer*>> players;
	std::unordered_map<eteams, std::vector<C_BaseObject*>> buildings;
	std::unordered_map<eteams, std::vector<C_ObjectSentrygun*>> sentries;
	std::unordered_map<eteams, std::vector<C_ObjectDispenser*>> dispensers;
	std::unordered_map<eteams, std::vector<C_ObjectTeleporter*>> teleporters;
	std::unordered_map<eworld, std::vector<C_BaseEntity*>> world;
	std::unordered_map<eteams, std::vector<C_BaseEntity*>> intel;
	std::unordered_map<eteams, std::vector<C_BaseEntity*>> control_point;
	std::unordered_map<eteams, std::vector<C_BaseEntity*>> payload;
	std::unordered_map<eteams, std::vector<C_BaseEntity*>> projectiles;
	std::unordered_map<eteams, std::vector<C_BaseEntity*>> npc_entity;
	std::vector<C_BaseEntity*> fish;
	std::vector<C_BaseEntity*> stickies;
	std::vector<C_BaseEntity*> flares;
	/*std::vector<C_BaseEntity*> world;
	std::vector<C_BaseEntity*> health;
	std::vector<C_BaseEntity*> ammo;
	std::vector<C_BaseEntity*> bombs;
	std::vector<C_BaseEntity*> intel;
	std::vector<C_BaseEntity*> projectiles;
	std::vector<C_BaseEntity*> npc;*/

	bool is_scope( const FNV1A_t uModelHash );
	bool is_vaccinator( const FNV1A_t uModelHash );
	bool is_ammo( const FNV1A_t uModelHash );
	bool is_health( const FNV1A_t uModelHash );
	void fill( );
	void reset( );
	C_TFPlayer* get_local( );
	C_TFWeaponBase* get_weapon( );
	C_TFPlayerResource* get_resource( );

	inline bool w2s( const vector& vOrigin, vector& vScreen )
	{
		const matrix3x4_t& w2s = draw.world_to_projection.As3x4( );

		float w = w2s[ 3 ][ 0 ] * vOrigin[ 0 ] + w2s[ 3 ][ 1 ] * vOrigin[ 1 ] + w2s[ 3 ][ 2 ] * vOrigin[ 2 ] + w2s[ 3 ][ 3 ];
		vScreen.z = 0;

		if ( w > 0.001f ) {
			float flsw = static_cast< float >( draw.screen_width );
			float flsh = static_cast< float >( draw.screen_height );
			float fl1dbw = 1 / w;
			vScreen.x = ( flsw / 2.0f ) + ( 0.5f * ( ( w2s[ 0 ][ 0 ] * vOrigin[ 0 ] + w2s[ 0 ][ 1 ] * vOrigin[ 1 ] + w2s[ 0 ][ 2 ] * vOrigin[ 2 ] + w2s[ 0 ][ 3 ] ) * fl1dbw ) * flsw + 0.5f );
			vScreen.y = ( flsh / 2.0f ) - ( 0.5f * ( ( w2s[ 1 ][ 0 ] * vOrigin[ 0 ] + w2s[ 1 ][ 1 ] * vOrigin[ 1 ] + w2s[ 1 ][ 2 ] * vOrigin[ 2 ] + w2s[ 1 ][ 3 ] ) * fl1dbw ) * flsh + 0.5f );
			return true;
		}

		return false;
	}
};
inline __entity_cache entity_cache;