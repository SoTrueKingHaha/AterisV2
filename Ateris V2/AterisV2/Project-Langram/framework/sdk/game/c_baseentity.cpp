#include "C_BaseEntity.h"

#include "../interfaces/interfaces.h"
#include "../../utils/signatures/signatures.h"
#include "../../utils/math/math.h"



//bool C_BaseEntity::ComputeHitboxSurroundingBox(Vector* mins, Vector* maxs)
//{
//	return reinterpret_cast<bool(__thiscall*)(C_BaseEntity*, Vector*, Vector*)>(S::ComputeHitboxSurroundingBox.m_nAddress)(this, mins, maxs);
//}

//inline bool C_BaseEntity::PhysicsRunThink(const thinkmethods_t thinkMethod)
//{
//	return reinterpret_cast<bool(__thiscall*)(void*, thinkmethods_t)>(S::PhysicsRunThink.m_dwLoc)(this, thinkMethod);
//}


void C_BaseEntity::UpdateVisibility()
{
	static auto FN = reinterpret_cast<void(__thiscall*)(void*)>(S::C_BaseEntity_UpdateVisibility.address);
	FN(this);
}

C_BaseEntity* C_BaseEntity::FirstMoveChild()
{
	return i::entity_list->GetClientEntity(*reinterpret_cast<int*>(this + 0x1B0) & 0xFFF)->As<C_BaseEntity>();
}

C_BaseEntity* C_BaseEntity::NextMovePeer()
{
	return i::entity_list->GetClientEntity(*reinterpret_cast<int*>(this + 0x1B4) & 0xFFF)->As<C_BaseEntity>();
}

ETFClassID C_BaseEntity::GetClassID()
{
	const auto& pClientClass = this->GetClientClass();
	return pClientClass ? static_cast<ETFClassID>(pClientClass->m_ClassID) : static_cast<ETFClassID>(0);
}

bool C_BaseEntity::IsWearable()
{
	return GetClassID() == ETFClassID::CTFWearable;
}

vector C_BaseEntity::GetWorldSpaceCenter()
{
	vector vMin, vMax; GetRenderBounds(vMin, vMax);
	vector vWorldSpaceCenter = GetAbsOrigin();
	vWorldSpaceCenter.z += (vMin.z + vMax.z) / 2.0f;
	return vWorldSpaceCenter;
}

C_BaseEntity* C_BaseEntity::GetOwnerEntity()
{
	return m_hOwnerEntity().Get();
}

bool C_BaseEntity::ShouldCollide(int collisionGroup, int contentsMask)
{
	if (m_CollisionGroup() == COLLISION_GROUP_DEBRIS)
	{
		if (!(contentsMask & CONTENTS_DEBRIS))
			return false;
	}
	return true;
}

void C_BaseEntity::SetAbsOrigin(const vector& vOrigin)
{
	static auto FN = reinterpret_cast<void(__thiscall*)( C_BaseEntity *, const vector&)>(S::SetAbsOrigin.address);
	FN( this, vOrigin );
}

vector C_BaseEntity::EstimateAbsVelocity()
{
	static auto FN = reinterpret_cast<void(__thiscall*)(C_BaseEntity*, vector&)>(S::C_BaseEntity_EstimateAbsVelocity.address);
	vector v;
	FN(this, v);
	return v;
}

int C_BaseEntity::SaveData( const char* const context, const int slot, const int type )
{
	static auto FN = reinterpret_cast< int( __thiscall* )( const char*, int, int ) >( S::C_BaseEntity_SaveData.address );
	return FN( context, slot, type );
}

int C_BaseEntity::RestoreData( const char* const context, const int slot, const int type )
{
	static auto FN = reinterpret_cast< int( __thiscall* )( const char*, int, int ) >( S::C_BaseEntity_RestoreData.address );
	return FN( context, slot, type );
}

void C_BaseEntity::SetAbsAngles(const QAngle& vAngles)
{
	QAngle* pAbsAngles = const_cast<QAngle*>(&this->GetAbsAngles());
	*pAbsAngles = vAngles;
}


SolidType_t C_BaseEntity::GetSolid()
{
	static auto FN = reinterpret_cast<SolidType_t(__thiscall*)(void*)>(S::C_BaseEntity_GetSolid.address);
	return FN(this);
}

vector C_BaseEntity::GetHitboxPosition(const int nHitbox)
{
	if (const auto& pModel = GetModel())
	{
		if (const auto& pHDR = i::model_info->GetStudiomodel(pModel))
		{
			matrix3x4_t mBoneMatrix[128];
			if (SetupBones(mBoneMatrix, 128, BONE_USED_BY_HITBOX, i::global_vars->curtime))
			{
				if (const auto& pHitboxSet = pHDR->GetHitboxSet(this->As<C_BaseAnimating>()->m_nHitboxSet()))
				{
					if (const auto& pHitbox = pHitboxSet->hitbox(nHitbox))
					{
						vector vPosition = (pHitbox->bbmin + pHitbox->bbmax) * 0.5f;
						vector vReturnedPosition;
						math.vector_transform(vPosition, mBoneMatrix[pHitbox->bone], vReturnedPosition);
						return vReturnedPosition;
					}
				}
			}
		}
	}
	return vector(0.f, 0.f, 0.f);
}

bool C_BaseEntity::OnSteamFriendsList()
{
	PlayerInfo_t pi = { };

	if (i::engine_client->GetPlayerInfo(this->entindex(), &pi) && pi.friendsID)
	{
		CSteamID steamID{ pi.friendsID, 1, k_EUniversePublic, k_EAccountTypeIndividual };
		return i::SteamFriends02->HasFriend(steamID, k_EFriendFlagImmediate);
	}

	return false;
}

float G::AttribHookValue(float flValue, const char* pszAttribHook, C_BaseEntity* pEntity, CUtlVector<C_BaseEntity*>* pItemList, bool bIsGlobalConstString)
{
	static auto FN = reinterpret_cast<float(__cdecl*)(float, const char*, C_BaseEntity*, CUtlVector<C_BaseEntity*> *, bool)>(S::CAttributeManager_AttribHookValue_float.address);
	return FN(flValue, pszAttribHook, pEntity, pItemList, bIsGlobalConstString);	
}
