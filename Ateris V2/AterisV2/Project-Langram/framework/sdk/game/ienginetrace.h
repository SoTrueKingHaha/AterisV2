#pragma once

#include "icollideable.h"

class __declspec(align(16)) VectorAligned : public vector
{
public:
	__forceinline VectorAligned() {}

	__forceinline VectorAligned(const vector& vec)
	{
		x = vec.x;
		y = vec.y;
		z = vec.z;
		w = 0.f;
	}

	float w;
};

#pragma warning (disable : 26495)

class C_BaseEntity;

struct cplane_t
{
	vector	normal;
	float	dist;
	BYTE	type;
	BYTE	signbits;
	BYTE	pad[2];
};

struct Ray_t
{
	VectorAligned  m_Start;	// starting point, centered within the extents
	VectorAligned  m_Delta;	// direction + length of the ray
	VectorAligned  m_StartOffset;	// Add this to m_Start to get the actual ray start
	VectorAligned  m_Extents;	// Describes an axis aligned box extruded along a ray
	bool m_IsRay;	// are the extents zero?
	bool m_IsSwept;	// is delta != 0?

	void Init(vector const& start, vector const& end)
	{
		m_Delta = end - start;
		m_IsSwept = (m_Delta.length_sqr() != 0);
		m_Extents = vector();
		m_IsRay = true;
		m_StartOffset = vector();
		m_Start = start;
	}

	void Init(vector const& start, vector const& end, vector const& mins, vector const& maxs)
	{
		m_Delta = end - start;
		m_IsSwept = (m_Delta.length_sqr() != 0);
		m_Extents = maxs - mins;
		m_Extents *= 0.5f;
		m_IsRay = (m_Extents.length_sqr() < 1e-6);
		m_StartOffset = mins + maxs;
		m_StartOffset *= 0.5f;
		m_Start = start + m_StartOffset;
		m_StartOffset *= -1.0f;
	}

	vector InvDelta() const
	{
		vector vecInvDelta;
		for (int iAxis = 0; iAxis < 3; ++iAxis)
		{
			if (m_Delta[iAxis] != 0.0f)
			{
				vecInvDelta[iAxis] = 1.0f / m_Delta[iAxis];
			}
			else
			{
				vecInvDelta[iAxis] = FLT_MAX;
			}
		}
		return vecInvDelta;
	}
};

class CTraceListData;

#define DISPSURF_FLAG_SURFACE		(1 << 0)
#define DISPSURF_FLAG_WALKABLE		(1 << 1)
#define DISPSURF_FLAG_BUILDABLE		(1 << 2)
#define DISPSURF_FLAG_SURFPROP1		(1 << 3)
#define DISPSURF_FLAG_SURFPROP2		(1 << 4)

class CBaseTrace
{
public:
	bool IsDispSurface(void) { return ((m_sDispFlags & DISPSURF_FLAG_SURFACE) != 0); }
	bool IsDispSurfaceWalkable(void) { return ((m_sDispFlags & DISPSURF_FLAG_WALKABLE) != 0); }
	bool IsDispSurfaceBuildable(void) { return ((m_sDispFlags & DISPSURF_FLAG_BUILDABLE) != 0); }
	bool IsDispSurfaceSurfProp1(void) { return ((m_sDispFlags & DISPSURF_FLAG_SURFPROP1) != 0); }
	bool IsDispSurfaceSurfProp2(void) { return ((m_sDispFlags & DISPSURF_FLAG_SURFPROP2) != 0); }

public:
	vector			m_vStartPos = vector();
	vector			m_vEndPos = vector();
	cplane_t		m_Plane = {};
	float			m_flFraction = 0.0f;
	int				m_iContents = 0;
	unsigned short	m_sDispFlags = 0;
	bool			m_bAllSolid = false;
	bool			m_bStartSolid = false;

	CBaseTrace() {}

private:
	CBaseTrace(const CBaseTrace& othr);
};

struct csurface_t
{
	const char* name;
	short		surfaceProps;
	unsigned short	flags;		// BUGBUG: These are declared per surface, not per material, but this database is per-material now
};

class CGameTrace : public CBaseTrace
{
public:
	bool DidHitWorld() const;
	bool DidHitNonWorldEntity() const;
	int  GetEntityIndex() const;

	bool DidHit() const
	{
		return (m_flFraction < 1 || m_bAllSolid || m_bStartSolid);
	}

public:
	float			flFractionLeftSolid;
	csurface_t		surface;
	int				nHitGroup;
	short			sPhysicsBone;
	C_BaseEntity* m_pEnt;
	int				nHitbox;
};

enum TraceType_t
{
	TRACE_EVERYTHING = 0,
	TRACE_WORLD_ONLY,				// NOTE: This does *not* test static props!!!
	TRACE_ENTITIES_ONLY,			// NOTE: This version will *not* test static props
	TRACE_EVERYTHING_FILTER_PROPS,	// NOTE: This version will pass the IHandleEntity for props through the filter, unlike all other filters
};

#define DECLARE_CLASS_NOBASE(className) typedef className ThisClass; template <typename T> friend int CheckDeclareClass_Access(T *, const char *pShouldBe); static int CheckDeclareClass( const char *pShouldBe ) { return InternalCheckDeclareClass( pShouldBe, #className, 0, 0 ); }

class ITraceFilter
{
public:
	virtual bool ShouldHitEntity(IHandleEntity* pEntity, int contentsMask) = 0;
	virtual TraceType_t	GetTraceType() const = 0;
};

// This is the one most normal traces will inherit from
class CTraceFilter : public ITraceFilter
{
public:
	virtual TraceType_t	GetTraceType() const
	{
		return TRACE_EVERYTHING;
	}
};

class CTraceFilterHitscan : public CTraceFilter
{
public:
	virtual bool ShouldHitEntity( IHandleEntity *pEntityHandle, int nContentsMask );
	virtual TraceType_t GetTraceType( ) const;
	C_BaseEntity *pSkip = nullptr;
};

class CTraceFilterWorldAndPropsOnly : public CTraceFilter
{
public:
	virtual bool ShouldHitEntity( IHandleEntity *pEntityHandle, int nContentsMask );
	virtual TraceType_t	GetTraceType( ) const;
	C_BaseEntity *pSkip = nullptr;
};

class IEntityEnumerator
{
public:
	virtual bool EnumEntity(IHandleEntity* pHandleEntity) = 0;
};

class IEngineTrace
{
public:
	virtual int GetPointContents(const vector& vecAbsPosition, IHandleEntity** ppEntity = NULL) = 0;
	virtual int GetPointContents_Collideable(ICollideable* pCollide, const vector& vecAbsPosition) = 0;
	virtual void ClipRayToEntity(const Ray_t& ray, unsigned int fMask, IHandleEntity* pEnt, trace_t* pTrace) = 0;
	virtual void ClipRayToCollideable(const Ray_t& ray, unsigned int fMask, ICollideable* pCollide, trace_t* pTrace) = 0;
	virtual void TraceRay(const Ray_t& ray, unsigned int fMask, CTraceFilter* pTraceFilter, trace_t* pTrace) = 0;
	virtual void SetupLeafAndEntityListRay(const Ray_t& ray, CTraceListData& traceData) = 0;
	virtual void SetupLeafAndEntityListBox(const vector& vecBoxMin, const vector& vecBoxMax, CTraceListData& traceData) = 0;
	virtual void TraceRayAgainstLeafAndEntityList(const Ray_t& ray, CTraceListData& traceData, unsigned int fMask, CTraceFilter* pTraceFilter, trace_t* pTrace) = 0;
	virtual void SweepCollideable(ICollideable* pCollide, const vector& vecAbsStart, const vector& vecAbsEnd,
								  const vector& vecAngles, unsigned int fMask, CTraceFilter* pTraceFilter, trace_t* pTrace) = 0;
	virtual void EnumerateEntities(const Ray_t& ray, bool triggers, IEntityEnumerator* pEnumerator) = 0;
	virtual void EnumerateEntities(const vector& vecAbsMins, const vector& vecAbsMaxs, IEntityEnumerator* pEnumerator) = 0;
	virtual ICollideable* GetCollideable(IHandleEntity* pEntity) = 0;
	virtual int GetStatByIndex(int index, bool bClear) = 0;
	virtual void GetBrushesInAABB(const vector& vMins, const vector& vMaxs, void* pOutput, int iContentsMask = 0xFFFFFFFF) = 0;
	virtual CPhysCollide* GetCollidableFromDisplacementsInAABB(const vector& vMins, const vector& vMaxs) = 0;
	virtual bool GetBrushInfo(int iBrush, void* pPlanesOut, int* pContentsOut) = 0;
	virtual bool PointOutsideWorld(const vector& ptTest) = 0;
	virtual int GetLeafContainingPoint(const vector& ptTest) = 0;
};