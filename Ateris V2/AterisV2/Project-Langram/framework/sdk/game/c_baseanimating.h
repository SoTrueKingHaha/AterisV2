#pragma once

#include "C_BaseEntity.h"

class IModelLoadCallback
{
public:
	virtual void OnModelLoadComplete(const model_t * pModel) = 0;

protected:
	// Protected destructor so that nobody tries to delete via this interface.
	// Automatically unregisters if the callback is destroyed while still pending.
	~IModelLoadCallback();
};

class C_BaseAnimating : public C_BaseEntity, private IModelLoadCallback
{
public:
	NETVAR(m_nSequence, int, "CBaseAnimating", "m_nSequence");
	NETVAR(m_nForceBone, int, "CBaseAnimating", "m_nForceBone");
	NETVAR(m_vecForce, vector, "CBaseAnimating", "m_vecForce");
	NETVAR(m_nSkin, int, "CBaseAnimating", "m_nSkin");
	NETVAR(m_nBody, int, "CBaseAnimating", "m_nBody");
	NETVAR(m_nHitboxSet, int, "CBaseAnimating", "m_nHitboxSet");
	NETVAR(m_flModelScale, float, "CBaseAnimating", "m_flModelScale");
	NETVAR(m_flModelWidthScale, float, "CBaseAnimating", "m_flModelWidthScale");
	NETVAR(m_flPoseParameter, float*, "CBaseAnimating", "m_flPoseParameter");
	NETVAR(m_flPlaybackRate, float, "CBaseAnimating", "m_flPlaybackRate");
	NETVAR(m_flEncodedController, void*, "CBaseAnimating", "m_flEncodedController");
	NETVAR(m_bClientSideAnimation, bool, "CBaseAnimating", "m_bClientSideAnimation");
	NETVAR(m_bClientSideFrameReset, bool, "CBaseAnimating", "m_bClientSideFrameReset");
	NETVAR(m_nNewSequenceParity, int, "CBaseAnimating", "m_nNewSequenceParity");
	NETVAR(m_nResetEventsParity, int, "CBaseAnimating", "m_nResetEventsParity");
	NETVAR(m_nMuzzleFlashParity, int, "CBaseAnimating", "m_nMuzzleFlashParity");
	NETVAR(m_hLightingOrigin, int, "CBaseAnimating", "m_hLightingOrigin");
	NETVAR(m_hLightingOriginRelative, int, "CBaseAnimating", "m_hLightingOriginRelative");
	NETVAR(m_flCycle, float, "CBaseAnimating", "m_flCycle");
	NETVAR(m_fadeMinDist, float, "CBaseAnimating", "m_fadeMinDist");
	NETVAR(m_fadeMaxDist, float, "CBaseAnimating", "m_fadeMaxDist");
	NETVAR(m_flFadeScale, float, "CBaseAnimating", "m_flFadeScale");

	void SetSequence(int nSequence);
	void UpdateVisibility();
	
	class CBoneAccessor
	{
	public:
		C_BaseAnimating* m_pAnimating;
		matrix3x4_t* m_pBones;
	};

	inline CUtlVector<matrix3x4_t>* GetCachedBoneData()
	{
		return reinterpret_cast<CUtlVector<matrix3x4_t> *>(reinterpret_cast<DWORD>(this) + 0x848); //0x844?
	}

	NETVAR_OFFSET_PURE(m_pIK, void*, 1400);
	NETVAR_OFFSET_PURE(m_flOldModelScale, float, 2108);
	NETVAR_OFFSET_PURE(m_noldSequence, int, 2112);
	NETVAR_OFFSET_PURE(m_pBoneMergeCache, void*, 2116);
	NETVAR_OFFSET_PURE(m_CachedBoneData, CUtlVector<matrix3x4_t>, 2120);
	NETVAR_OFFSET_PURE(m_hitboxBoneCacheHandle, unsigned int, 2140);
	NETVAR_OFFSET_PURE(m_flLastBoneSetupTime, float, 2144);
	NETVAR_OFFSET_PURE(m_pJiggleBones, void*, 2148);
	NETVAR_OFFSET_PURE(m_bDynamicModelPending, bool, 2185);
	NETVAR_OFFSET_PURE(m_bResetSequenceInfoOnLoad, bool, 2186);
	NETVAR_OFFSET_PURE(m_BoneAccessor, CBoneAccessor, 1452);
};