#pragma once
#include "utlvector.h"
#include "imaterialsystem.h"
#include "ivmodelrender.h"
#include "ivmodelinfo.h"

#define Assert

//-----------------------------------------------------------------------------
// DrawModel flags
//-----------------------------------------------------------------------------
enum
{
	STUDIORENDER_DRAW_ENTIRE_MODEL = 0,
	STUDIORENDER_DRAW_OPAQUE_ONLY = 0x01,
	STUDIORENDER_DRAW_TRANSLUCENT_ONLY = 0x02,
	STUDIORENDER_DRAW_GROUP_MASK = 0x03,

	STUDIORENDER_DRAW_NO_FLEXES = 0x04,
	STUDIORENDER_DRAW_STATIC_LIGHTING = 0x08,

	STUDIORENDER_DRAW_ACCURATETIME = 0x10,		// Use accurate timing when drawing the model.
	STUDIORENDER_DRAW_NO_SHADOWS = 0x20,
	STUDIORENDER_DRAW_GET_PERF_STATS = 0x40,

	STUDIORENDER_DRAW_WIREFRAME = 0x80,

	STUDIORENDER_DRAW_ITEM_BLINK = 0x100,

	STUDIORENDER_SHADOWDEPTHTEXTURE = 0x200,

	STUDIORENDER_SSAODEPTHTEXTURE = 0x1000,

	STUDIORENDER_GENERATE_STATS = 0x8000,
};


struct StudioRenderConfig_t
{
	float fEyeShiftX;	// eye X position
	float fEyeShiftY;	// eye Y position
	float fEyeShiftZ;	// eye Z position
	float fEyeSize;		// adjustment to iris textures
	float fEyeGlintPixelWidthLODThreshold;

	int maxDecalsPerModel;
	int drawEntities;
	int skin;
	int fullbright;

	bool bEyeMove : 1;		// look around
	bool bSoftwareSkin : 1;
	bool bNoHardware : 1;
	bool bNoSoftware : 1;
	bool bTeeth : 1;
	bool bEyes : 1;
	bool bFlex : 1;
	bool bWireframe : 1;
	bool bDrawNormals : 1;
	bool bDrawTangentFrame : 1;
	bool bDrawZBufferedWireframe : 1;
	bool bSoftwareLighting : 1;
	bool bShowEnvCubemapOnly : 1;
	bool bWireframeDecals : 1;

	// Reserved for future use
	int m_nReserved[4];
};



class CCycleCount
{
	friend class CFastTimer;

public:
	CCycleCount();
	CCycleCount(uint64_t cycles);

	void			Sample();	// Sample the clock. This takes about 34 clocks to execute (or 26,000 calls per millisecond on a P900).

	void			Init();		// Set to zero.
	void			Init(float initTimeMsec);
	void			Init(double initTimeMsec) { Init((float)initTimeMsec); }
	void			Init(uint64_t cycles);
	bool			IsLessThan(CCycleCount const& other) const;					// Compare two counts.

	// Convert to other time representations. These functions are slow, so it's preferable to call them
	// during display rather than inside a timing block.
	unsigned long	GetCycles()  const;
	uint64_t			GetLongCycles() const;

	unsigned long	GetMicroseconds() const;
	uint64_t			GetUlMicroseconds() const;
	double			GetMicrosecondsF() const;
	void			SetMicroseconds(unsigned long nMicroseconds);

	unsigned long	GetMilliseconds() const;
	double			GetMillisecondsF() const;

	double			GetSeconds() const;

	CCycleCount& operator+=(CCycleCount const& other);

	// dest = rSrc1 + rSrc2
	static void		Add(CCycleCount const& rSrc1, CCycleCount const& rSrc2, CCycleCount& dest);	// Add two samples together.

	// dest = rSrc1 - rSrc2
	static void		Sub(CCycleCount const& rSrc1, CCycleCount const& rSrc2, CCycleCount& dest);	// Add two samples together.

	static uint64_t	GetTimestamp();

	uint64_t			m_Int64;
};

class CFastTimer
{
public:
	// These functions are fast to call and should be called from your sampling code.
	void				Start();
	void				End();

	const CCycleCount& GetDuration() const;	// Get the elapsed time between Start and End calls.
	CCycleCount 		GetDurationInProgress() const; // Call without ending. Not that cheap.

	// Return number of cycles per second on this processor.
	static inline unsigned long	GetClockSpeed();

private:
	CCycleCount	m_Duration;
};

// beyond this number of materials, you won't get info back from DrawModel
#define MAX_DRAW_MODEL_INFO_MATERIALS 8

struct DrawModelResults_t
{
	int m_ActualTriCount;
	int m_TextureMemoryBytes;
	int m_NumHardwareBones;
	int m_NumBatches;
	int m_NumMaterials;
	int m_nLODUsed;
	int m_flLODMetric;
	CFastTimer m_RenderTime;
	void* m_Materials;//CUtlVectorFixed<IMaterial*, MAX_DRAW_MODEL_INFO_MATERIALS> m_Materials;
};

struct GetTriangles_Vertex_t
{
	vector m_Position;
	vector m_Normal;
	vector_4d m_TangentS;
	vector_2d m_TexCoord;
	vector_4d m_BoneWeight;
	int m_BoneIndex[4];
	int m_NumBones;
};

struct GetTriangles_MaterialBatch_t
{
	IMaterial* m_pMaterial;
	CUtlVector<GetTriangles_Vertex_t> m_Verts;
	CUtlVector<int> m_TriListIndices;
};

#define MAXSTUDIOSKINS		32		// total textures
#define MAXSTUDIOBONES		128		// total bones actually used
#define MAXSTUDIOFLEXDESC	1024	// maximum number of low level flexes (actual morph targets)
#define MAXSTUDIOFLEXCTRL	96		// maximum number of flexcontrollers (input sliders)
#define MAXSTUDIOPOSEPARAM	24
#define MAXSTUDIOBONECTRLS	4
#define MAXSTUDIOANIMBLOCKS 256

struct GetTriangles_Output_t
{
	CUtlVector<GetTriangles_MaterialBatch_t> m_MaterialBatches;
	matrix3x4_t m_PoseToWorld[MAXSTUDIOBONES];
};

struct model_array_instance_t
{
	matrix3x4_t		modelToWorld;

	// UNDONE: Per instance lighting values?
};

class IStudioRender : public IAppSystem
{
public:
	virtual void BeginFrame(void) = 0;
	virtual void EndFrame(void) = 0;

	// Used for the mat_stub console command.
	virtual void Mat_Stub(IMaterialSystem* pMatSys) = 0;

	// Updates the rendering configuration 
	virtual void UpdateConfig(const StudioRenderConfig_t& config) = 0;
	virtual void GetCurrentConfig(StudioRenderConfig_t& config) = 0;

	// Load, unload model data
	virtual bool LoadModel(studiohdr_t* pStudioHdr, void* pVtxData, studiohwdata_t* pHardwareData) = 0;
	virtual void UnloadModel(studiohwdata_t* pHardwareData) = 0;

	// Refresh the studiohdr since it was lost...
	virtual void RefreshStudioHdr(studiohdr_t* pStudioHdr, studiohwdata_t* pHardwareData) = 0;

	// This is needed to do eyeglint and calculate the correct texcoords for the eyes.
	virtual void SetEyeViewTarget(const studiohdr_t* pStudioHdr, int nBodyIndex, const vector& worldPosition) = 0;

	// Methods related to lighting state
	// NOTE: SetAmbientLightColors assumes that the arraysize is the same as 
	// returned from GetNumAmbientLightSamples
	virtual int GetNumAmbientLightSamples() = 0;
	virtual const vector* GetAmbientLightDirections() = 0;
	virtual void SetAmbientLightColors(const vector_4d* pAmbientOnlyColors) = 0;
	virtual void SetAmbientLightColors(const vector* pAmbientOnlyColors) = 0;
	virtual void SetLocalLights(int numLights, const LightDesc_t* pLights) = 0;

	// Sets information about the camera location + orientation
	virtual void SetViewState(const vector& viewOrigin, const vector& viewRight,
							  const vector& viewUp, const vector& viewPlaneNormal) = 0;

						  // Allocates flex weights for use in rendering
						  // NOTE: Pass in a non-null second parameter to lock delayed flex weights
	virtual void LockFlexWeights(int nWeightCount, float** ppFlexWeights, float** ppFlexDelayedWeights = NULL) = 0;
	virtual void UnlockFlexWeights() = 0;

	// Used to allocate bone matrices to be used to pass into DrawModel
	virtual matrix3x4_t* LockBoneMatrices(int nBoneCount) = 0;
	virtual void UnlockBoneMatrices() = 0;

	// LOD stuff
	virtual int GetNumLODs(const studiohwdata_t& hardwareData) const = 0;
	virtual float GetLODSwitchValue(const studiohwdata_t& hardwareData, int lod) const = 0;
	virtual void SetLODSwitchValue(studiohwdata_t& hardwareData, int lod, float switchValue) = 0;

	// Sets the color/alpha modulation
	virtual void SetColorModulation(float const* pColor) = 0;
	virtual void SetAlphaModulation(float flAlpha) = 0;

	// Draws the model
	virtual void DrawModel(DrawModelResults_t* pResults, const DrawModelInfo_t& info,
						   matrix3x4_t* pBoneToWorld, float* pFlexWeights, float* pFlexDelayedWeights, const vector& modelOrigin, int flags = STUDIORENDER_DRAW_ENTIRE_MODEL) = 0;

					   // Methods related to static prop rendering
	virtual void DrawModelStaticProp(const DrawModelInfo_t& drawInfo, const matrix3x4_t& modelToWorld, int flags = STUDIORENDER_DRAW_ENTIRE_MODEL) = 0;
	virtual void DrawStaticPropDecals(const DrawModelInfo_t& drawInfo, const matrix3x4_t& modelToWorld) = 0;
	virtual void DrawStaticPropShadows(const DrawModelInfo_t& drawInfo, const matrix3x4_t& modelToWorld, int flags) = 0;

	// Causes a material to be used instead of the materials the model was compiled with
	virtual void ForcedMaterialOverride(IMaterial* newMaterial, OverrideType_t nOverrideType = OVERRIDE_NORMAL) = 0;

	// Create, destroy list of decals for a particular model
	virtual StudioDecalHandle_t CreateDecalList(studiohwdata_t* pHardwareData) = 0;
	virtual void DestroyDecalList(StudioDecalHandle_t handle) = 0;

	// Add decals to a decal list by doing a planar projection along the ray
	// The BoneToWorld matrices must be set before this is called
	virtual void AddDecal(StudioDecalHandle_t handle, studiohdr_t* pStudioHdr, matrix3x4_t* pBoneToWorld,
						  const Ray_t& ray, const vector& decalUp, IMaterial* pDecalMaterial, float radius, int body, bool noPokethru = false, int maxLODToDecal = ADDDECAL_TO_ALL_LODS) = 0;

					  // Compute the lighting at a point and normal
	virtual void ComputeLighting(const vector* pAmbient, int lightCount,
								 LightDesc_t* pLights, const vector& pt, const vector& normal, vector& lighting) = 0;

							 // Compute the lighting at a point, constant directional component is passed
							 // as flDirectionalAmount
	virtual void ComputeLightingConstDirectional(const vector* pAmbient, int lightCount,
												 LightDesc_t* pLights, const vector& pt, const vector& normal, vector& lighting, float flDirectionalAmount) = 0;

											 // Shadow state (affects the models as they are rendered)
	virtual void AddShadow(IMaterial* pMaterial, void* pProxyData, FlashlightState_t* m_pFlashlightState = NULL, view_matrix* pWorldToTexture = NULL, ITexture* pFlashlightDepthTexture = NULL) = 0;
	virtual void ClearAllShadows() = 0;

	// Gets the model LOD; pass in the screen size in pixels of a sphere 
	// of radius 1 that has the same origin as the model to get the LOD out...
	virtual int ComputeModelLod(studiohwdata_t* pHardwareData, float unitSphereSize, float* pMetric = NULL) = 0;

	// Return a number that is usable for budgets, etc.
	// Things that we care about:
	// 1) effective triangle count (factors in batch sizes, state changes, etc)
	// 2) texture memory usage
	// Get Triangles returns the LOD used
	virtual void GetPerfStats(DrawModelResults_t* pResults, const DrawModelInfo_t& info, CUtlBuffer* pSpewBuf = NULL) const = 0;
	virtual void GetTriangles(const DrawModelInfo_t& info, matrix3x4_t* pBoneToWorld, GetTriangles_Output_t& out) = 0;

	// Returns materials used by a particular model
	virtual int GetMaterialList(studiohdr_t* pStudioHdr, int count, IMaterial** ppMaterials) = 0;
	virtual int GetMaterialListFromBodyAndSkin(MDLHandle_t studio, int nSkin, int nBody, int nCountOutputMaterials, IMaterial** ppOutputMaterials) = 0;
	// draw an array of models with the same state
	virtual void DrawModelArray(const DrawModelInfo_t& drawInfo, int arrayCount, model_array_instance_t* pInstanceData, int instanceStride, int flags = STUDIORENDER_DRAW_ENTIRE_MODEL) = 0;

	virtual void GetMaterialOverride(IMaterial** ppOutForcedMaterial, OverrideType_t* pOutOverrideType) = 0;

};
