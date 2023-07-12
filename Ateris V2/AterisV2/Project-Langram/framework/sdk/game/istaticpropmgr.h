#pragma once

#include "vector.h"
#include "utlvector.h"
#include "cbasehandle.h"
#include "color.h"
#include "ivmodelrender.h"
#include "vphysics_interface.h"

//-----------------------------------------------------------------------------
// Interface versions for static props
//-----------------------------------------------------------------------------
#define INTERFACEVERSION_STATICPROPMGR_CLIENT		"StaticPropMgrClient004"
#define INTERFACEVERSION_STATICPROPMGR_SERVER		"StaticPropMgrServer002"

//-----------------------------------------------------------------------------
// Purpose: Pass this into the parser to handle the keys that vphysics does not
// parse.
//-----------------------------------------------------------------------------
class IVPhysicsKeyHandler
{
public:
	virtual void ParseKeyValue(void* pData, const char* pKey, const char* pValue) = 0;
	virtual void SetDefaults(void* pData) = 0;
};


//-----------------------------------------------------------------------------
// Interface for static props
//-----------------------------------------------------------------------------
class IStaticPropMgr
{
public:
	// Create physics representations of props
	virtual void	CreateVPhysicsRepresentations(IPhysicsEnvironment * physenv, IVPhysicsKeyHandler * pDefaults, void* pGameData) = 0;

	// Purpose: Trace a ray against the specified static Prop. Returns point of intersection in trace_t
	virtual void	TraceRayAgainstStaticProp(const Ray_t& ray, int staticPropIndex, trace_t& tr) = 0;

	// Is a base handle a static prop?
	virtual bool	IsStaticProp(IHandleEntity* pHandleEntity) const = 0;
	virtual bool	IsStaticProp(CBaseHandle handle) const = 0;

	// returns a collideable interface to static props
	virtual ICollideable* GetStaticPropByIndex(int propIndex) = 0;
};

class IStaticPropMgrClient : public IStaticPropMgr
{
public:
	// Recomputes the static prop opacity given a view origin
	virtual void	ComputePropOpacity(const vector & viewOrigin, float factor) = 0;

	// Adds decals to static props, returns point of decal in trace_t
	virtual void	AddDecalToStaticProp(const vector& rayStart, const vector& rayEnd,
		int staticPropIndex, int decalIndex, bool doTrace, trace_t& tr) = 0;
	// Adds/removes shadows from static props
	virtual void	AddShadowToStaticProp(unsigned short shadowHandle, IClientRenderable* pRenderable) = 0;
	virtual void	RemoveAllShadowsFromStaticProp(IClientRenderable* pRenderable) = 0;

	// Gets the lighting + material color of a static prop
	virtual void	GetStaticPropMaterialColorAndLighting(trace_t* pTrace,
		int staticPropIndex, vector& lighting, vector& matColor) = 0;

	//Changes made specifically to support the Portal mod (smack Dave Kircher if something breaks) (Added separately to both client and server to not mess with versioning)
	//===================================================================
	virtual void GetAllStaticProps(CUtlVector<ICollideable*>* pOutput) = 0; //testing function that will eventually be removed
	virtual void GetAllStaticPropsInAABB(const vector& vMins, const vector& vMaxs, CUtlVector<ICollideable*>* pOutput) = 0; //get all static props that exist wholly or partially in an AABB
	virtual void GetAllStaticPropsInOBB(const vector& ptOrigin, const vector& vExtent1, const vector& vExtent2, const vector& vExtent3, CUtlVector<ICollideable*>* pOutput) = 0; //get all static props that exist wholly or partially in an OBB
	//===================================================================

	virtual void DrawStaticProps(IClientRenderable** pProps, int count, bool bShadowDepth, bool drawVCollideWireframe) = 0;
	virtual void AddColorDecalToStaticProp(vector const& rayStart, vector const& rayEnd,
		int staticPropIndex, int decalIndex, bool doTrace, trace_t& tr, bool bUseColor, Color cColor) = 0;
};

class IStaticPropMgrServer : public IStaticPropMgr
{
public:


	//Changes made specifically to support the Portal mod (smack Dave Kircher if something breaks) (Added separately to both client and server to not mess with versioning)
	//===================================================================
	virtual void GetAllStaticProps(CUtlVector<ICollideable*>* pOutput) = 0; //testing function that will eventually be removed
	virtual void GetAllStaticPropsInAABB(const vector& vMins, const vector& vMaxs, CUtlVector<ICollideable*>* pOutput) = 0; //get all static props that exist wholly or partially in an AABB
	virtual void GetAllStaticPropsInOBB(const vector& ptOrigin, const vector& vExtent1, const vector& vExtent2, const vector& vExtent3, CUtlVector<ICollideable*>* pOutput) = 0; //get all static props that exist wholly or partially in an OBB
	//===================================================================
};

class IStaticPropMgrEngine
{
public:
	virtual bool Init() = 0;
	virtual void Shutdown() = 0;

	// Call at the beginning of the level, will unserialize all static
	// props and add them to the main collision tree
	virtual void LevelInit() = 0;

	// Call this when there's a client, *after* LevelInit, and after the world entity is loaded
	virtual void LevelInitClient() = 0;

	// Call this when there's a client, *before* LevelShutdown
	virtual void LevelShutdownClient() = 0;

	// Call at the end of the level, cleans up the static props
	virtual void LevelShutdown() = 0;

	// Call this to recompute static prop lighting when necessary
	virtual void RecomputeStaticLighting() = 0;

	// Check if a static prop is in a particular PVS.
	virtual bool IsPropInPVS(IHandleEntity* pHandleEntity, const byte* pVis) const = 0;

	// returns a collideable interface to static props
	virtual ICollideable* GetStaticProp(IHandleEntity* pHandleEntity) = 0;

	// returns the lightcache handle associated with a static prop
	virtual LightCacheHandle_t GetLightCacheHandleForStaticProp(IHandleEntity* pHandleEntity) = 0;

	// Is a base handle a static prop?
	virtual bool IsStaticProp(IHandleEntity* pHandleEntity) const = 0;
	virtual bool IsStaticProp(CBaseHandle handle) const = 0;

	// Returns the static prop index (useful for networking)
	virtual int GetStaticPropIndex(IHandleEntity* pHandleEntity) const = 0;

	virtual bool PropHasBakedLightingDisabled(IHandleEntity* pHandleEntity) const = 0;
};



typedef unsigned short SpatialPartitionHandle_t;

class CStaticProp : public IClientUnknown, public IClientRenderable, public ICollideable
{

};

class CStaticProp_
{
public:
	byte pad[11];
	vector						m_Origin;
	QAngle						m_Angles;
	model_t* m_pModel;
	SpatialPartitionHandle_t	m_Partition;
	ModelInstanceHandle_t		m_ModelInstance;
	unsigned char				m_Alpha;
	unsigned char				m_nSolidType;
	unsigned char				m_Skin;
	unsigned char				m_Flags;
	unsigned short				m_FirstLeaf;
	unsigned short				m_LeafCount;
	CBaseHandle					m_EntHandle;	// FIXME: Do I need client + server handles?
	ClientRenderHandle_t		m_RenderHandle;
	unsigned short				m_FadeIndex;	// Index into the m_StaticPropFade dictionary
	float						m_flForcedFadeScale;

	// bbox is the same for both GetBounds and GetRenderBounds since static props never move.
	// GetRenderBounds is interpolated data, and GetBounds is last networked.
	vector					m_RenderBBoxMin;
	vector					m_RenderBBoxMax;
	matrix3x4_t				m_ModelToWorld;
	float					m_flRadius;

	vector					m_WorldRenderBBoxMin;
	vector					m_WorldRenderBBoxMax;

	// FIXME: This sucks. Need to store the lighting origin off
	// because the time at which the static props are unserialized
	// doesn't necessarily match the time at which we can initialize the light cache
	vector					m_LightingOrigin;
};


struct StaticPropLeafLump_t
{
	static datamap_t m_DataMap; static datamap_t* GetBaseMap(); template <typename T> friend void DataMapAccess(T*, datamap_t** p); template <typename T> friend datamap_t* DataMapInit(T*);
	unsigned short	m_Leaf;
};


//-----------------------------------------------------------------------------
// The engine's static prop manager
//-----------------------------------------------------------------------------
class CStaticPropMgr : public IStaticPropMgrEngine, public IStaticPropMgrClient, public IStaticPropMgrServer
{
public:
	// constructor, destructor
	CStaticPropMgr();
	virtual ~CStaticPropMgr();

	// methods of IStaticPropMgrEngine
	virtual bool Init();
	virtual void Shutdown();
	virtual void LevelInit();
	virtual void LevelInitClient();
	virtual void LevelShutdown();
	virtual void LevelShutdownClient();
	virtual bool IsPropInPVS(IHandleEntity* pHandleEntity, const byte* pVis) const;
	virtual ICollideable* GetStaticProp(IHandleEntity* pHandleEntity);
	virtual void RecomputeStaticLighting();
	virtual LightCacheHandle_t GetLightCacheHandleForStaticProp(IHandleEntity* pHandleEntity);
	virtual bool IsStaticProp(IHandleEntity* pHandleEntity) const;
	virtual bool IsStaticProp(CBaseHandle handle) const;
	virtual int GetStaticPropIndex(IHandleEntity* pHandleEntity) const;
	virtual ICollideable* GetStaticPropByIndex(int propIndex);

	// methods of IStaticPropMgrClient
	virtual void ComputePropOpacity(const vector& viewOrigin, float factor);
	virtual void TraceRayAgainstStaticProp(const Ray_t& ray, int staticPropIndex, trace_t& tr);
	virtual void AddDecalToStaticProp(vector const& rayStart, vector const& rayEnd,
									  int staticPropIndex, int decalIndex, bool doTrace, trace_t& tr);
	virtual void AddColorDecalToStaticProp(vector const& rayStart, vector const& rayEnd,
										   int staticPropIndex, int decalIndex, bool doTrace, trace_t& tr, bool bUseColor, Color cColor);
	virtual void AddShadowToStaticProp(unsigned short shadowHandle, IClientRenderable* pRenderable);
	virtual void RemoveAllShadowsFromStaticProp(IClientRenderable* pRenderable);
	virtual void GetStaticPropMaterialColorAndLighting(trace_t* pTrace,
													   int staticPropIndex, vector& lighting, vector& matColor);
	virtual void CreateVPhysicsRepresentations(IPhysicsEnvironment* physenv, IVPhysicsKeyHandler* pDefaults, void* pGameData);

	// methods of IStaticPropMgrServer

	//Changes made specifically to support the Portal mod (smack Dave Kircher if something breaks)
	//===================================================================
	virtual void GetAllStaticProps(CUtlVector<ICollideable*>* pOutput);
	virtual void GetAllStaticPropsInAABB(const vector& vMins, const vector& vMaxs, CUtlVector<ICollideable*>* pOutput);
	virtual void GetAllStaticPropsInOBB(const vector& ptOrigin, const vector& vExtent1, const vector& vExtent2, const vector& vExtent3, CUtlVector<ICollideable*>* pOutput);
	//===================================================================

	virtual bool PropHasBakedLightingDisabled(IHandleEntity* pHandleEntity) const;

	// Internal methods
	const vector& ViewOrigin() const { return m_vecLastViewOrigin; }

	// Computes the opacity for a single static prop
	void ComputePropOpacity(CStaticProp& prop);
	void DrawStaticProps(IClientRenderable** pProps, int count, bool bShadowDepth, bool drawVCollideWireframe);
	void DrawStaticProps_Slow(IClientRenderable** pProps, int count, bool bShadowDepth, bool drawVCollideWireframe);
	void DrawStaticProps_Fast(IClientRenderable** pProps, int count, bool bShadowDepth);
	void DrawStaticProps_FastPipeline(IClientRenderable** pProps, int count, bool bShadowDepth);

private:
	void OutputLevelStats(void);
	void PrecacheLighting();

	// Methods associated with unserializing static props
	void UnserializeModelDict(CUtlBuffer& buf);
	void UnserializeLeafList(CUtlBuffer& buf);
	void UnserializeModels(CUtlBuffer& buf);
	void UnserializeStaticProps();

	int HandleEntityToIndex(IHandleEntity* pHandleEntity) const;

	// Computes fade from screen-space fading
	unsigned char ComputeScreenFade(CStaticProp& prop, float flMinSize, float flMaxSize, float flFalloffFactor);
	void ChangeRenderGroup(CStaticProp& prop);

public:
	// Unique static prop models
	struct StaticPropDict_t
	{
		model_t* m_pModel;
		MDLHandle_t m_hMDL;
	};

	// Static props that fade use this data to fade
	struct StaticPropFade_t
	{
		int		m_Model;
		union
		{
			float	m_MinDistSq;
			float	m_MaxScreenWidth;
		};
		union
		{
			float	m_MaxDistSq;
			float	m_MinScreenWidth;
		};
		float	m_FalloffFactor;
	};

	// The list of all static props
	CUtlVector <StaticPropDict_t>	m_StaticPropDict;
	CUtlVector <CStaticProp>		m_StaticProps;
	CUtlVector <StaticPropLeafLump_t> m_StaticPropLeaves;

	// Static props that fade...
	CUtlVector<StaticPropFade_t>	m_StaticPropFade;

	bool							m_bLevelInitialized;
	bool							m_bClientInitialized;
	vector							m_vecLastViewOrigin;
	float							m_flLastViewFactor;
};