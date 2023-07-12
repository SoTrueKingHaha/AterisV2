#include "C_TFPlayer.h"
#include "c_tfweaponbase.h"

#include "../interfaces/interfaces.h"
#include "../../utils/signatures/signatures.h"
#include "../../features/menu/vars.h"


//C_BaseCombatWeapon* C_TFPlayer::GetActiveWeapon()
//{
//	return reinterpret_cast<C_BaseCombatWeapon*>(i::ClientEntityList->GetClientEntityFromHandle(*reinterpret_cast<CBaseHandle*>(&m_hMyWeapons())));
//}
//
//C_BaseCombatWeapon* C_TFPlayer::GetWeaponFromSlot(const int nSlot)
//{
//	static DWORD dwMyWeapons = GetNetVar(XOR("CBaseCombatCharacter"), XOR("m_hMyWeapons"));
//	int hWeapon = *reinterpret_cast<int*>(this + (dwMyWeapons + (nSlot * 0x4)));
//	return reinterpret_cast<C_BaseCombatWeapon*>(i::ClientEntityList->GetClientEntityFromHandle(hWeapon));
//}

C_BaseCombatWeapon* C_TFPlayer::GetActiveWeapon()
{
    return m_hActiveWeapon().Get();
}

C_BaseCombatWeapon* C_TFPlayer::GetWeaponFromSlot(const int nSlot)
{
    return m_hMyWeapons().at(nSlot).Get();
}

CEntitySphereQuery::CEntitySphereQuery(const vector& center, float radius, int flagMask, int partitionMask)
{
    static DWORD dwAddress = ( DWORD )memory->find_signature( "client.dll", "55 8B EC 83 EC 14 D9 45 0C" );
    reinterpret_cast< void( __thiscall* )( void*, const vector&, float, int, int ) >( dwAddress )(this, center, radius, flagMask, partitionMask);
}

C_BaseEntity* CEntitySphereQuery::GetCurrentEntity()
{
    return (m_listIndex < m_listCount) ? m_pList[m_listIndex] : nullptr;
}

void C_TFPlayer::OverrideView(CViewSetup* view)
{
    static auto FN = memory->get_vfunc<void(__thiscall*)(C_TFPlayer*, CViewSetup*)>(this, 276);
    FN(this, view);
}

void C_TFPlayer::UpdateWearables()
{
    static auto FN = reinterpret_cast<void(__thiscall*)(void*)>(S::C_TFPlayer_UpdateWearables.address);
    FN(this);
}

bool C_TFPlayer::IsSwimming()
{
    return ( (BYTE)m_nWaterLevel() > 1 );
}

bool C_TFPlayer::IsHealthCritical( )
{
    return m_iHealth( ) < GetMaxHealth( ) * 0.49f;
}

bool C_TFPlayer::IsAmmoCritical( )
{
    return GetWeaponFromSlot( SLOT_PRIMARY )->As<C_TFWeaponBase>( )->IsAmmoCritical( ) || GetWeaponFromSlot( SLOT_SECONDARY )->As<C_TFWeaponBase>( )->IsAmmoCritical( );
}

bool C_TFPlayer::IsUsingJumperWeapons( )
{
    const auto& weapon = GetWeaponFromSlot( SLOT_PRIMARY );
    int index = weapon->As<C_TFWeaponBase>( )->m_iItemDefinitionIndex( );
    return weapon && ( index == Soldier_m_RocketJumper || index == Demoman_s_StickyJumper );
}

bool C_TFPlayer::IsUsingArcProjectiles( )
{
    switch ( m_hActiveWeapon( ).Get( )->As<C_TFWeaponBase>( )->GetWeaponID( ) )
    {
        case TF_WEAPON_CLEAVER:
        case TF_WEAPON_FLAREGUN:
        case TF_WEAPON_FLAREGUN_REVENGE:
        case TF_WEAPON_PIPEBOMBLAUNCHER:
        case TF_WEAPON_GRENADELAUNCHER:
        case TF_WEAPON_CANNON:
        case TF_WEAPON_LUNCHBOX:
        case TF_WEAPON_SHOTGUN_BUILDING_RESCUE:
        case TF_WEAPON_CROSSBOW:
        case TF_WEAPON_SYRINGEGUN_MEDIC:
        case TF_WEAPON_COMPOUND_BOW:
        {
            return true;
            break;
        }
        default: return false;
    }

    return false;
}

bool C_TFPlayer::HasHighKD( )
{
    int deaths = m_iDeaths( ) == 0 ? 1 : m_iDeaths( );
    float kd = ( float )m_iKills( ) / ( float )deaths;
    if ( kd > vars.esp.players.players_kd && m_iKills( ) > 6 )
    {
        return true;
    }

    return false;
}

bool C_TFPlayer::HasHighHeadshotRatio( )
{
    int kills = m_iKills( ) == 0 ? 1 : m_iKills( );
    float ratio = m_iHeadshots( ) / ( float )kills;
    if ( ratio > vars.esp.players.players_headshot && m_iHeadshots( ) > 6 )
    {
        return true;
    }

    return false;
}

int C_TFPlayer::GetNumOfHitboxes( )
{
    if ( auto pModel = GetModel( ) )
    {
        if ( auto pHDR = i::model_info->GetStudiomodel( pModel ) )
        {
            if ( auto pSet = pHDR->GetHitboxSet( m_nHitboxSet( ) ) )
                return pSet->numhitboxes;
        }
    }

    return 0;
}