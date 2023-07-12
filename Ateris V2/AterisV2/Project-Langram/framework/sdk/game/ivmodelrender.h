#pragma once
#include "imaterialsystem.h"
#include "icliententity.h"

struct mstudioanimdesc_t;
struct mstudioseqdesc_t;
struct model_t;
class IClientRenderable;
struct studiohdr_t;

struct studiohwdata_t;
using StudioDecalHandle_t = void*;
using QAngle = vector;
using LightCacheHandle_t = void*;
struct Ray_t;
class ColorMeshInfo_t;

struct DrawModelState_t
{
	studiohdr_t* m_pStudioHdr;
	studiohwdata_t* m_pStudioHWData;
	IClientRenderable* m_pRenderable;
	const matrix3x4_t* m_pModelToWorld;
	StudioDecalHandle_t	m_decals;
	int m_drawFlags;
	int m_lod;
};

typedef unsigned short ModelInstanceHandle_t;

enum
{
	MODEL_INSTANCE_INVALID = (ModelInstanceHandle_t)~0
};

struct ModelRenderInfo_t
{
	vector origin;
	QAngle angles;
	IClientRenderable* pRenderable;
	const model_t* pModel;
	const matrix3x4_t* pModelToWorld;
	const matrix3x4_t* pLightingOffset;
	const vector* pLightingOrigin;
	int flags;
	int entity_index;
	int skin;
	int body;
	int hitboxset;
	ModelInstanceHandle_t instance;
};

struct ShaderStencilState_t
{
	bool                        m_bEnable;
	StencilOperation_t          m_FailOp;
	StencilOperation_t          m_ZFailOp;
	StencilOperation_t          m_PassOp;
	StencilComparisonFunction_t m_CompareFunc;
	int                         m_nReferenceValue;
	uint32_t                      m_nTestMask;
	uint32_t                      m_nWriteMask;

	ShaderStencilState_t();
	void SetStencilState(IMatRenderContext* pRenderContext);
};

inline ShaderStencilState_t::ShaderStencilState_t()
{
	m_bEnable = false;
	m_PassOp = m_FailOp = m_ZFailOp = STENCILOPERATION_KEEP;
	m_CompareFunc = STENCILCOMPARISONFUNCTION_ALWAYS;
	m_nReferenceValue = 0;
	m_nTestMask = m_nWriteMask = 0xFFFFFFFF;
}

inline void ShaderStencilState_t::SetStencilState	(IMatRenderContext* pRenderContext)
{
	pRenderContext->SetStencilEnable(m_bEnable);
	pRenderContext->SetStencilFailOperation(m_FailOp);
	pRenderContext->SetStencilZFailOperation(m_ZFailOp);
	pRenderContext->SetStencilPassOperation(m_PassOp);
	pRenderContext->SetStencilCompareFunction(m_CompareFunc);
	pRenderContext->SetStencilReferenceValue(m_nReferenceValue);
	pRenderContext->SetStencilTestMask(m_nTestMask);
	pRenderContext->SetStencilWriteMask(m_nWriteMask);
}

struct StaticPropRenderInfo_t
{
	const matrix3x4_t* pModelToWorld;
	const model_t* pModel;
	IClientRenderable* pRenderable;
	vector* pLightingOrigin;
	short skin;
	ModelInstanceHandle_t instance;
};

enum OverrideType_t
{
	OVERRIDE_NORMAL = 0,
	OVERRIDE_BUILD_SHADOWS,
	OVERRIDE_DEPTH_WRITE,
	OVERRIDE_SSAO_DEPTH_WRITE,
};

enum
{
	ADDDECAL_TO_ALL_LODS = -1
};

struct DrawModelInfo_t
{
	studiohdr_t* m_pStudioHdr;
	studiohwdata_t* m_pHardwareData;
	StudioDecalHandle_t m_Decals;
	int				m_Skin;
	int				m_Body;
	int				m_HitboxSet;
	void* m_pClientEntity;
	int				m_Lod;
	ColorMeshInfo_t* m_pColorMeshes;
	bool			m_bStaticLighting;
	vector			m_vecAmbientCube[6];
	int				m_nLocalLightCount;
	LightDesc_t		m_LocalLightDescs[4];
};

class IVModelRender
{
public:
	virtual int	DrawModel(int flags,
						  IClientRenderable* pRenderable,
						  ModelInstanceHandle_t instance,
						  int entity_index,
						  const model_t* model,
						  vector const& origin,
						  QAngle const& angles,
						  int skin,
						  int body,
						  int hitboxset,
						  const matrix3x4_t* modelToWorld = NULL,
						  const matrix3x4_t* pLightingOffset = NULL) = 0;

	virtual void ForcedMaterialOverride(IMaterial* newMaterial, OverrideType_t nOverrideType = OVERRIDE_NORMAL) = 0;
	virtual void SetViewTarget(const CStudioHdr* pStudioHdr, int nBodyIndex, const vector& target) = 0;
	virtual ModelInstanceHandle_t CreateInstance(IClientRenderable* pRenderable, LightCacheHandle_t* pCache = NULL) = 0;
	virtual void DestroyInstance(ModelInstanceHandle_t handle) = 0;
	virtual void SetStaticLighting(ModelInstanceHandle_t handle, LightCacheHandle_t* pHandle) = 0;
	virtual LightCacheHandle_t GetStaticLighting(ModelInstanceHandle_t handle) = 0;
	virtual bool ChangeInstance(ModelInstanceHandle_t handle, IClientRenderable* pRenderable) = 0;
	virtual void AddDecal(ModelInstanceHandle_t handle, Ray_t const& ray, vector const& decalUp, int decalIndex, int body, bool noPokeThru = false, int maxLODToDecal = ADDDECAL_TO_ALL_LODS) = 0;
	virtual void RemoveAllDecals(ModelInstanceHandle_t handle) = 0;
	virtual void RemoveAllDecalsFromAllModels() = 0;
	virtual matrix3x4_t* DrawModelShadowSetup(IClientRenderable* pRenderable, int body, int skin, DrawModelInfo_t* pInfo, matrix3x4_t* pCustomBoneToWorld = NULL) = 0;
	virtual void DrawModelShadow(IClientRenderable* pRenderable, const DrawModelInfo_t& info, matrix3x4_t* pCustomBoneToWorld = NULL) = 0;
	virtual bool RecomputeStaticLighting(ModelInstanceHandle_t handle) = 0;
	virtual void ReleaseAllStaticPropColorData(void) = 0;
	virtual void RestoreAllStaticPropColorData(void) = 0;
	virtual int	DrawModelEx(ModelRenderInfo_t& pInfo) = 0;
	virtual int	DrawModelExStaticProp(ModelRenderInfo_t& pInfo) = 0;
	virtual bool DrawModelSetup(ModelRenderInfo_t& pInfo, DrawModelState_t* pState, matrix3x4_t* pCustomBoneToWorld, matrix3x4_t** ppBoneToWorldOut) = 0;
	virtual void DrawModelExecute(const DrawModelState_t& state, const ModelRenderInfo_t& pInfo, matrix3x4_t* pCustomBoneToWorld = NULL) = 0;
	virtual void SetupLighting(const vector& vecCenter) = 0;
	virtual int DrawStaticPropArrayFast(StaticPropRenderInfo_t* pProps, int count, bool bShadowDepth) = 0;
	virtual void SuppressEngineLighting(bool bSuppress) = 0;
	virtual void SetupColorMeshes(int nTotalVerts) = 0;
	virtual void AddColoredDecal(ModelInstanceHandle_t handle, Ray_t const& ray, vector const& decalUp, int decalIndex, int body, Color cColor, bool noPokeThru = false, int maxLODToDecal = ADDDECAL_TO_ALL_LODS) = 0;
	virtual void GetMaterialOverride(IMaterial** ppOutForcedMaterial, OverrideType_t* pOutOverrideType) = 0;
};
