#pragma once
#include "../../sdk/globals/globalinfo.h"
#include "../menu/vars.h"
#include "../../utils/utils.h"
#include "../../utils/convars/convars.h"
#include "../../utils/entitycache/entitycache.h"

struct ProjectileInfo
{
    ProjectileType_t m_type{};

    vector m_pos{};
    vector m_ang{};

    float m_speed{};
    float m_gravity_mod{};

    bool no_spin{};
};

namespace projectile_simulation
{
	const objectparams_t g_PhysDefaultObjectParams =
	{
		NULL,
		1.0, //mass
		1.0, // inertia
		0.1f, // damping
		0.1f, // rotdamping
		0.05f, // rotIntertiaLimit
		"DEFAULT",
		NULL,// game data
		0.f, // volume (leave 0 if you don't have one or call physcollision->CollideVolume() to compute it)
		1.0f, // drag coefficient
		true,// enable collisions?
	};

    bool getInfo( C_TFPlayer* player, C_TFWeaponBase* weapon, const vector& angles, ProjectileInfo& out );
    bool init( const ProjectileInfo& info );
    void runTick( );
    vector getOrigin( );
	void run_simulation( C_TFPlayer* local, C_TFWeaponBase* weapon, const vector& angles );
}