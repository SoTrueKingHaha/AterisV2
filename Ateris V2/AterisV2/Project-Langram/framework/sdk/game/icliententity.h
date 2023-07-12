#pragma once
#pragma warning ( disable : 4005 )

#include "color.h"
#include "vector.h"
#include "dt_recv.h"
#include "bitbuf.h"
#include "EHandle.h"

class CStudioHdr;

enum SolidType_t
{
	SOLID_NONE = 0,	// no solid model
	SOLID_BSP = 1,	// a BSP tree
	SOLID_BBOX = 2,	// an AABB
	SOLID_OBB = 3,	// an OBB (not implemented yet)
	SOLID_OBB_YAW = 4,	// an OBB, constrained so that it can only yaw
	SOLID_CUSTOM = 5,	// Always call into the entity for tests
	SOLID_VPHYSICS = 6,	// solid vphysics object, get vcollide from the model and collide with that
	SOLID_LAST,
};

#define abstract_class class

class CBaseHandle;

class IClientNetworkable;
class C_BaseEntity;
class IClientRenderable;
class ICollideable;
class IClientEntity;
class IClientThinkable;

abstract_class IClientUnknown : public IHandleEntity
{
public:
	virtual ICollideable * GetCollideable() = 0;
	virtual IClientNetworkable* GetClientNetworkable() = 0;
	virtual IClientRenderable* GetClientRenderable() = 0;
	virtual IClientEntity* GetIClientEntity() = 0;
	virtual C_BaseEntity* GetBaseEntity() = 0;
	virtual IClientThinkable* GetClientThinkable() = 0;
};

typedef unsigned short ClientShadowHandle_t;

typedef unsigned short ClientRenderHandle_t;

typedef unsigned short ModelInstanceHandle_t;

struct model_t;

enum ShadowType_t
{
	SHADOWS_NONE = 0,
	SHADOWS_SIMPLE,
	SHADOWS_RENDER_TO_TEXTURE,
	SHADOWS_RENDER_TO_TEXTURE_DYNAMIC,	// the shadow is always changing state
	SHADOWS_RENDER_TO_DEPTH_TEXTURE,
};

abstract_class IPVSNotify
{
public:
	virtual void OnPVSStatusChanged(bool bInPVS) = 0;
};

abstract_class IClientRenderable
{
public:
	// Gets at the containing class...
	virtual IClientUnknown * GetIClientUnknown() = 0;

	// Data accessors
	virtual vector const& GetRenderOrigin(void) = 0;
	virtual QAngle const& GetRenderAngles(void) = 0;
	virtual bool					ShouldDraw(void) = 0;
	virtual bool					IsTransparent(void) = 0;
	virtual bool					UsesPowerOfTwoFrameBufferTexture() = 0;
	virtual bool					UsesFullFrameBufferTexture() = 0;

	virtual ClientShadowHandle_t	GetShadowHandle() const = 0;

	// Used by the leaf system to store its render handle.
	virtual ClientRenderHandle_t& RenderHandle() = 0;

	// Render baby!
	virtual model_t* GetModel() const = 0;
	virtual int						DrawModel(int flags) = 0;

	// Get the body parameter
	virtual int		GetBody() = 0;

	// Determine alpha and blend amount for transparent objects based on render state info
	virtual void	ComputeFxBlend() = 0;
	virtual int		GetFxBlend(void) = 0;

	// Determine the color modulation amount
	virtual void	GetColorModulation(float* color) = 0;

	// Returns false if the entity shouldn't be drawn due to LOD. 
	// (NOTE: This is no longer used/supported, but kept in the vtable for backwards compat)
	virtual bool	LODTest() = 0;

	// Call this to get the current bone transforms for the model.
	// currentTime parameter will affect interpolation
	// nMaxBones specifies how many matrices pBoneToWorldOut can hold. (Should be greater than or
	// equal to studiohdr_t::numbones. Use MAXSTUDIOBONES to be safe.)
	virtual bool	SetupBones(matrix3x4_t* pBoneToWorldOut, int nMaxBones, int boneMask, float currentTime) = 0;

	virtual void	SetupWeights(const matrix3x4_t* pBoneToWorld, int nFlexWeightCount, float* pFlexWeights, float* pFlexDelayedWeights) = 0;
	virtual void	DoAnimationEvents(void) = 0;

	// Return this if you want PVS notifications. See IPVSNotify for more info.	
	// Note: you must always return the same value from this function. If you don't,
	// undefined things will occur, and they won't be good.
	virtual IPVSNotify* GetPVSNotifyInterface() = 0;

	// Returns the bounds relative to the origin (render bounds)
	virtual void	GetRenderBounds(vector& mins, vector& maxs) = 0;

	// returns the bounds as an AABB in worldspace
	virtual void	GetRenderBoundsWorldspace(vector& mins, vector& maxs) = 0;

	// These normally call through to GetRenderAngles/GetRenderBounds, but some entities custom implement them.
	virtual void	GetShadowRenderBounds(vector& mins, vector& maxs, ShadowType_t shadowType) = 0;

	// Should this object be able to have shadows cast onto it?
	virtual bool	ShouldReceiveProjectedTextures(int flags) = 0;

	// These methods return true if we want a per-renderable shadow cast direction + distance
	virtual bool	GetShadowCastDistance(float* pDist, ShadowType_t shadowType) const = 0;
	virtual bool	GetShadowCastDirection(vector* pDirection, ShadowType_t shadowType) const = 0;

	// Other methods related to shadow rendering
	virtual bool	IsShadowDirty() = 0;
	virtual void	MarkShadowDirty(bool bDirty) = 0;

	// Iteration over shadow hierarchy
	virtual IClientRenderable* GetShadowParent() = 0;
	virtual IClientRenderable* FirstShadowChild() = 0;
	virtual IClientRenderable* NextShadowPeer() = 0;

	// Returns the shadow cast type
	virtual ShadowType_t ShadowCastType() = 0;

	// Create/get/destroy model instance
	virtual void CreateModelInstance() = 0;
	virtual ModelInstanceHandle_t GetModelInstance() = 0;

	// Returns the transform from RenderOrigin/RenderAngles to world
	virtual matrix3x4_t& RenderableToWorldTransform() = 0;

	// Attachments
	virtual int LookupAttachment(const char* pAttachmentName) = 0;
	virtual	bool GetAttachment(int number, vector& origin, QAngle& angles) = 0;
	virtual bool GetAttachment(int number, matrix3x4_t& matrix) = 0;

	// Rendering clip plane, should be 4 floats, return value of NULL indicates a disabled render clip plane
	virtual float* GetRenderClipPlane(void) = 0;

	// Get the skin parameter
	virtual int		GetSkin() = 0;

	// Is this a two-pass renderable?
	virtual bool	IsTwoPass(void) = 0;

	virtual void	OnThreadedDrawSetup() = 0;

	virtual bool	UsesFlexDelayedWeights() = 0;

	virtual void	RecordToolMessage() = 0;

	virtual bool	IgnoresZBuffer(void) const = 0;
};

#define GET_ENT_I(n) i::entity_list->GetClientEntity(n)
#define GET_ENT_H(n) i::entity_list->GetClientEntityFromHandle(n)
#define GET_LOCAL() i::entity_list->GetClientEntity(i::engine_client->GetLocalPlayer())
#define GET_LOCAL_PLAYER() i::entity_list->GetClientEntity(i::engine_client->GetLocalPlayer())->As<C_TFPlayer>()

class ClientClass;
// Linked list of all known client classes
extern ClientClass* g_pClientClassHead;

// The serial number that gets passed in is used for ehandles.
typedef IClientNetworkable* (*CreateClientClassFn)(int entnum, int serialNum);
typedef IClientNetworkable* (*CreateEventFn)();

//-----------------------------------------------------------------------------
// Purpose: Client side class definition
//-----------------------------------------------------------------------------
class ClientClass
{
public:
	ClientClass(const char* pNetworkName, CreateClientClassFn createFn, CreateEventFn createEventFn, RecvTable* pRecvTable)
	{
		m_pNetworkName = pNetworkName;
		m_pCreateFn = createFn;
		m_pCreateEventFn = createEventFn;
		m_pRecvTable = pRecvTable;

		// Link it in
		m_pNext = g_pClientClassHead;
		g_pClientClassHead = this;
	}

	const char* GetName()
	{
		return m_pNetworkName;
	}

public:
	CreateClientClassFn		m_pCreateFn;
	CreateEventFn			m_pCreateEventFn;	// Only called for event objects.
	const char* m_pNetworkName;
	RecvTable* m_pRecvTable;
	ClientClass* m_pNext;
	int						m_ClassID;	// Managed by the engine.
};

enum ShouldTransmitState_t
{
	SHOULDTRANSMIT_START = 0,	// The entity is starting to be transmitted (maybe it entered the PVS).

	SHOULDTRANSMIT_END		// Called when the entity isn't being transmitted by the server.
							// This signals a good time to hide the entity until next time
							// the server wants to transmit its state.
};

// has all these in them. Left it as an enum in case we want to go back though
enum DataUpdateType_t
{
	DATA_UPDATE_CREATED = 0,	// indicates it was created +and+ entered the pvs
//	DATA_UPDATE_ENTERED_PVS,
DATA_UPDATE_DATATABLE_CHANGED,
//	DATA_UPDATE_LEFT_PVS,
//	DATA_UPDATE_DESTROYED,		// FIXME: Could enable this, but it's a little worrying
								// since it changes a bunch of existing code
};

abstract_class IClientNetworkable
{
public:
	// Gets at the containing class...
	virtual IClientUnknown * GetIClientUnknown() = 0;

	// Called by the engine when the server deletes the entity.
	virtual void			Release() = 0;

	// Supplied automatically by the IMPLEMENT_CLIENTCLASS macros.
	virtual ClientClass* GetClientClass() = 0;

	// This tells the entity what the server says for ShouldTransmit on this entity.
	// Note: This used to be EntityEnteredPVS/EntityRemainedInPVS/EntityLeftPVS.
	virtual void			NotifyShouldTransmit(ShouldTransmitState_t state) = 0;



	//
	// NOTE FOR ENTITY WRITERS: 
	//
	// In 90% of the cases, you should hook OnPreDataChanged/OnDataChanged instead of 
	// PreDataUpdate/PostDataUpdate.
	//
	// The DataChanged events are only called once per frame whereas Pre/PostDataUpdate
	// are called once per packet (and sometimes multiple times per frame).
	//
	// OnDataChanged is called during simulation where entity origins are correct and 
	// attachments can be used. whereas PostDataUpdate is called while parsing packets
	// so attachments and other entity origins may not be valid yet.
	//

	virtual void			OnPreDataChanged(DataUpdateType_t updateType) = 0;
	virtual void			OnDataChanged(DataUpdateType_t updateType) = 0;

	// Called when data is being updated across the network.
	// Only low-level entities should need to know about these.
	virtual void			PreDataUpdate(DataUpdateType_t updateType) = 0;
	virtual void			PostDataUpdate(DataUpdateType_t updateType) = 0;


	// Objects become dormant on the client if they leave the PVS on the server.
	virtual bool			IsDormant(void) = 0;

	// Ent Index is the server handle used to reference this entity.
	// If the index is < 0, that indicates the entity is not known to the server
	virtual int				entindex(void) const = 0;

	// Server to client entity message received
	virtual void			ReceiveMessage(int classID, bf_read& msg) = 0;

	// Get the base pointer to the networked data that GetClientClass->m_pRecvTable starts at.
	// (This is usually just the "this" pointer).
	virtual void* GetDataTableBasePtr() = 0;

	// Tells the entity that it's about to be destroyed due to the client receiving
	// an uncompressed update that's caused it to destroy all entities & recreate them.
	virtual void			SetDestroyedOnRecreateEntities(void) = 0;

	virtual void			OnDataUnchangedInPVS() = 0;
};

class CClientThinkHandlePtr;
typedef CClientThinkHandlePtr* ClientThinkHandle_t;


// Entities that implement this interface can be put into the client think list.
abstract_class IClientThinkable
{
public:
	// Gets at the containing class...
	virtual IClientUnknown * GetIClientUnknown() = 0;

	virtual void				ClientThink() = 0;

	// Called when you're added to the think list.
	// GetThinkHandle's return value must be initialized to INVALID_THINK_HANDLE.
	virtual ClientThinkHandle_t	GetThinkHandle() = 0;
	virtual void				SetThinkHandle(ClientThinkHandle_t hThink) = 0;

	// Called by the client when it deletes the entity.
	virtual void				Release() = 0;
};

struct Ray_t;
class CGameTrace;
typedef CGameTrace trace_t;
class CMouthInfo;
class IClientEntityInternal;
struct SpatializationInfo_t;

abstract_class IClientEntity : public IClientUnknown, public IClientRenderable, public IClientNetworkable, public IClientThinkable
{
public:
	// Delete yourself.
	virtual void			Release(void) = 0;

	// Network origin + angles
	virtual const vector& GetAbsOrigin(void) const = 0;
	virtual const QAngle& GetAbsAngles(void) const = 0;

	virtual CMouthInfo* GetMouth(void) = 0;

	// Retrieve sound spatialization info for the specified sound on this entity
	// Return false to indicate sound is not audible
	virtual bool			GetSoundSpatialization(SpatializationInfo_t& info) = 0;
};
// This class can be used to implement default versions of some of the 
// functions of IClientRenderable.
abstract_class CDefaultClientRenderable : public IClientUnknown, public IClientRenderable
{
public:
	virtual const vector & GetRenderOrigin(void) = 0;
	virtual const QAngle& GetRenderAngles(void) = 0;
	virtual matrix3x4_t& RenderableToWorldTransform() = 0;
	virtual bool					ShouldDraw(void) = 0;
	virtual bool					IsTransparent(void) = 0;
	virtual bool					IsTwoPass(void) = 0;
	virtual void					OnThreadedDrawSetup() = 0;
	virtual bool					UsesPowerOfTwoFrameBufferTexture(void) = 0;
	virtual bool					UsesFullFrameBufferTexture(void) = 0;
	virtual ClientShadowHandle_t	GetShadowHandle() const = 0;
	virtual ClientRenderHandle_t& RenderHandle() = 0;
	virtual int						GetBody() = 0;
	virtual int						GetSkin() = 0;
	virtual bool					UsesFlexDelayedWeights() = 0;
	virtual model_t* GetModel() const = 0;
	virtual int						DrawModel(int flags) = 0;
	virtual void					ComputeFxBlend() = 0;
	virtual int						GetFxBlend() = 0;
	virtual bool					LODTest() = 0;
	virtual bool					SetupBones(matrix3x4_t* pBoneToWorldOut, int nMaxBones, int boneMask, float currentTime) = 0;
	virtual void					SetupWeights(const matrix3x4_t* pBoneToWorld, int nFlexWeightCount, float* pFlexWeights, float* pFlexDelayedWeights) = 0;
	virtual void					DoAnimationEvents(void) = 0;
	virtual IPVSNotify* GetPVSNotifyInterface() = 0;
	virtual void					GetRenderBoundsWorldspace(vector& absMins, vector& absMaxs) = 0;
	virtual void	GetColorModulation(float* color) = 0;
	virtual bool	ShouldReceiveProjectedTextures(int flags) = 0;
	virtual bool	GetShadowCastDistance(float* pDist, ShadowType_t shadowType) const = 0;
	virtual bool	GetShadowCastDirection(vector* pDirection, ShadowType_t shadowType) const = 0;
	virtual void	GetShadowRenderBounds(vector& mins, vector& maxs, ShadowType_t shadowType) = 0;
	virtual bool IsShadowDirty() = 0;
	virtual void MarkShadowDirty(bool bDirty) = 0;
	virtual IClientRenderable* GetShadowParent() = 0;
	virtual IClientRenderable* FirstShadowChild() = 0;
	virtual IClientRenderable* NextShadowPeer() = 0;
	virtual ShadowType_t ShadowCastType() = 0;
	virtual void CreateModelInstance() = 0;
	virtual ModelInstanceHandle_t GetModelInstance() = 0;
	virtual int LookupAttachment(const char* pAttachmentName) = 0;
	virtual	bool GetAttachment(int number, vector& origin, QAngle& angles) = 0;
	virtual bool GetAttachment(int number, matrix3x4_t& matrix) = 0;
	virtual float* GetRenderClipPlane() = 0;
	virtual void RecordToolMessage() = 0;
	virtual bool IgnoresZBuffer(void) const = 0;
	virtual void SetRefEHandle(const CBaseHandle& handle) = 0;
	virtual const CBaseHandle& GetRefEHandle() const = 0;
	virtual IClientUnknown* GetIClientUnknown() = 0;
	virtual ICollideable* GetCollideable() = 0;
	virtual IClientRenderable* GetClientRenderable() = 0;
	virtual IClientNetworkable* GetClientNetworkable() = 0;
	virtual IClientEntity* GetIClientEntity() = 0;
	virtual C_BaseEntity* GetBaseEntity() = 0;
	virtual IClientThinkable* GetClientThinkable() = 0;
public:
	ClientRenderHandle_t m_hRenderHandle;
};