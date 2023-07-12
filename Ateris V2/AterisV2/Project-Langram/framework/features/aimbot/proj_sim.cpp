#include "proj_sim.h"

IPhysicsEnvironment* env{};
IPhysicsObject* obj{};

//rebuilt without unneeded things
void GetProjectileFireSetupRebuilt( C_TFPlayer* player,  vector offset, const  vector& ang_in,  vector& pos_out,  vector& ang_out, bool pipes )
{
    if ( convars.cl_flipviewmodels->GetBool( ) )
    {
        offset.y *= -1.0f;
    }

     vector forward{}, right{}, up{};
    math.angle_vectors( ang_in, &forward, &right, &up );

    auto shoot_pos{ player->GetShootPosition( ) };

    pos_out = shoot_pos + ( forward * offset.x ) + ( right * offset.y ) + ( up * offset.z );

    if ( pipes )
    {
        ang_out = ang_in;
    }

    else
    {
        auto end_pos{ shoot_pos + ( forward * 2000.0f ) };

        math.vector_angles( end_pos - pos_out, ang_out );
    }
}

bool projectile_simulation::getInfo( C_TFPlayer* player, C_TFWeaponBase* weapon, const  vector& angles, ProjectileInfo& out )
{
    if ( !player || !weapon )
    {
        return false;
    }

    auto cur_time{ static_cast< float >( player->m_nTickBase( ) ) * TICK_INTERVAL };
    auto ducking{ player->m_fFlags( ).IsFlagSet( FL_DUCKING ) };

     vector pos{};
     vector ang{};

    switch ( weapon->GetWeaponID( ) )
    {
        case TF_WEAPON_GRENADELAUNCHER:
        {
            GetProjectileFireSetupRebuilt( player, { 16.0f, 8.0f, -6.0f }, angles, pos, ang, true );

            auto is_lochnload{ weapon->m_iItemDefinitionIndex( ) == Demoman_m_TheLochnLoad };
            auto speed{ is_lochnload ? 1490.0f : 1200.0f };

            out = { TF_PROJECTILE_PIPEBOMB, pos, ang, speed, 1.0f, is_lochnload };

            return true;
        }

        case TF_WEAPON_PIPEBOMBLAUNCHER:
        {
            GetProjectileFireSetupRebuilt( player, { 16.0f, 8.0f, -6.0f }, angles, pos, ang, true );

            auto charge_begin_time{ weapon->As<C_TFPipebombLauncher>( )->m_flChargeBeginTime( ) };
            auto charge{ cur_time - charge_begin_time };
            auto speed{ math.remap_val_clamped( charge, 0.0f, G::AttribHookValue( 4.0f, "stickybomb_charge_rate", weapon ), 900.0f, 2400.0f ) };

            if ( charge_begin_time <= 0.0f )
            {
                speed = 900.0f;
            }

            out = { TF_PROJECTILE_PIPEBOMB_REMOTE, pos, ang, speed, 1.0f, false };

            return true;
        }

        case TF_WEAPON_CANNON:
        {
            GetProjectileFireSetupRebuilt( player, { 16.0f, 8.0f, -6.0f }, angles, pos, ang, true );

            out = { TF_PROJECTILE_CANNONBALL, pos, ang, 1454.0f, 1.0f, false };

            return true;
        }

        case TF_WEAPON_FLAREGUN:
        {
            GetProjectileFireSetupRebuilt( player, { 23.5f, 12.0f, ducking ? 8.0f : -3.0f }, angles, pos, ang, false );

            out = { TF_PROJECTILE_FLARE, pos, ang, 2000.0f, 0.3f, true };

            return true;
        }

        case TF_WEAPON_FLAREGUN_REVENGE:
        {
            GetProjectileFireSetupRebuilt( player, { 23.5f, 12.0f, ducking ? 8.0f : -3.0f }, angles, pos, ang, false );

            out = { TF_PROJECTILE_FLARE, pos, ang, 3000.0f, 0.45f, true };

            return true;
        }

        case TF_WEAPON_COMPOUND_BOW:
        {
            GetProjectileFireSetupRebuilt( player, { 23.5f, 8.0f, -3.0f }, angles, pos, ang, false );

            auto charge_begin_time{ weapon->As<C_TFPipebombLauncher>( )->m_flChargeBeginTime( ) };
            auto charge{ cur_time - charge_begin_time };
            auto speed{ math.remap_val_clamped( charge, 0.0f, 1.0f, 1800.0f, 2600.0f ) };
            auto grav_mod{ math.remap_val_clamped( charge, 0.0f, 1.0f, 0.5f, 0.1f ) };

            if ( charge_begin_time <= 0.0f )
            {
                speed = 1800.0f;
                grav_mod = 0.5f;
            }

            out = { TF_PROJECTILE_ARROW, pos, ang, speed, grav_mod, true };

            return true;
        }

        case TF_WEAPON_CROSSBOW:
        case TF_WEAPON_SHOTGUN_BUILDING_RESCUE:
        {
            GetProjectileFireSetupRebuilt( player, { 23.5f, 8.0f, -3.0f }, angles, pos, ang, false );

            out = { TF_PROJECTILE_ARROW, pos, ang, 2400.0f, 0.2f, true };

            return true;
        }

        default:
        {
            return false;
        }
    }
}

bool projectile_simulation::init( const ProjectileInfo& info )
{
    if ( !env )
    {
        env = i::physics->CreateEnvironment( );
    }

    if ( !obj )
    {
        //it doesn't matter what the size is for non drag affected projectiles
        //pipes use the size below so it works out just fine
        auto col{ i::physics_collision->BBoxToCollide( { -2.0f, -2.0f, -2.0f }, { 2.0f, 2.0f, 2.0f } ) };

        auto params{ g_PhysDefaultObjectParams };

        params.damping = 0.0f;
        params.rotdamping = 0.0f;
        params.inertia = 0.0f;
        params.rotInertiaLimit = 0.0f;
        params.enableCollisions = false;

        obj = env->CreatePolyObject( col, 0, info.m_pos, info.m_ang, &params );

        obj->Wake( );
    }

    if ( !env || !obj )
    {
        return false;
    }

    //set position and velocity
    {
         vector forward{}, up{};

        math.angle_vectors( info.m_ang, &forward, nullptr, &up );

         vector vel{ forward * info.m_speed };
         vector ang_vel{};

        switch ( info.m_type )
        {
            case TF_PROJECTILE_PIPEBOMB:
            case TF_PROJECTILE_PIPEBOMB_REMOTE:
            case TF_PROJECTILE_PIPEBOMB_PRACTICE:
            case TF_PROJECTILE_CANNONBALL:
            {
                //CTFWeaponBaseGun::FirePipeBomb
                //pick your poison

                vel += up * 200.0f;
                ang_vel = { 600.0f, -1200.0f, 0.0f };

                break;
            }

            default:
            {
                break;
            }
        }

        if ( info.no_spin )
        {
            ang_vel.zero( );
        }

        obj->SetPosition( info.m_pos, info.m_ang, true );
        obj->SetVelocity( &vel, &ang_vel );
    }

    //set drag
    {
        float drag{};
         vector drag_basis{};
         vector ang_drag_basis{};

        //these values were dumped from the server by firing the projectiles with 0 0 0 angles
        //they are calculated in CPhysicsObject::RecomputeDragBases
        switch ( info.m_type )
        {
            case TF_PROJECTILE_PIPEBOMB:
            {
                drag = 1.0f;
                drag_basis = { 0.003902f, 0.009962f, 0.009962f };
                ang_drag_basis = { 0.003618f, 0.001514f, 0.001514f };

                break;
            }

            case TF_PROJECTILE_PIPEBOMB_REMOTE:
            case TF_PROJECTILE_PIPEBOMB_PRACTICE:
            {
                drag = 1.0f;
                drag_basis = { 0.007491f, 0.007491f, 0.007306f };
                ang_drag_basis = { 0.002777f, 0.002842f, 0.002812f };

                break;
            }

            case TF_PROJECTILE_CANNONBALL:
            {
                drag = 1.0f;
                drag_basis = { 0.020971f, 0.019420f, 0.020971f };
                ang_drag_basis = { 0.012997f, 0.013496f, 0.013714f };

                break;
            }

            default:
            {
                break;
            }
        }

        obj->SetDragCoefficient( &drag, &drag );

        obj->m_dragBasis = drag_basis;
        obj->m_angDragBasis = ang_drag_basis;
    }

    //set env params
    {
        auto max_vel{ 1000000.0f };
        auto max_ang_vel{ 1000000.0f };

        //only pipes need k_flMaxVelocity and k_flMaxAngularVelocity
        switch ( info.m_type )
        {
            case TF_PROJECTILE_PIPEBOMB:
            case TF_PROJECTILE_PIPEBOMB_REMOTE:
            case TF_PROJECTILE_PIPEBOMB_PRACTICE:
            case TF_PROJECTILE_CANNONBALL:
            {
                max_vel = k_flMaxVelocity;
                max_ang_vel = k_flMaxAngularVelocity;

                break;
            }

            default:
            {
                break;
            }
        }

        physics_performanceparams_t params{};
        params.Defaults( );

        params.maxVelocity = max_vel;
        params.maxAngularVelocity = max_ang_vel;

        env->SetPerformanceSettings( &params );
        env->SetAirDensity( 2.0f );
        env->SetGravity( { 0.0f, 0.0f, -( 800.0f * info.m_gravity_mod ) } );

        env->ResetSimulationClock( ); //not needed?
    }

    return true;
}

void projectile_simulation::runTick( )
{
    if ( !env )
    {
        return;
    }

    env->Simulate( TICK_INTERVAL );
}

vector projectile_simulation::getOrigin( )
{
    if ( !obj )
    {
        return {};
    }

     vector out{};

    obj->GetPosition( &out, nullptr );

    return out;
}

void projectile_simulation::run_simulation( C_TFPlayer* local, C_TFWeaponBase* weapon, const vector& angles )
{
    if ( !global_info.projectile_lines.empty( ) )
    {
        global_info.projectile_lines.clear( );
    }

    ProjectileInfo info{};

    if ( !projectile_simulation::getInfo( local, weapon, angles, info ) )
    {
        return;
    }

    if ( !projectile_simulation::init( info ) )
    {
        return;
    }

    for ( auto n{ 0 }; n < TIME_TO_TICKS( 5.0f ); n++ )
    {
        projectile_simulation::runTick( );
        global_info.projectile_lines.push_back( projectile_simulation::getOrigin( ) );
    }
}