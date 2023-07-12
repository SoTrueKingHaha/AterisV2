#pragma once

#include "iprediction.h"

enum
{
	WL_NotInWater = 0,
	WL_Feet,
	WL_Waist,
	WL_Eyes
};

enum PLAYER_ANIM;
enum struct soundlevel_t;
class IPhysicsSurfaceProps;

class IMoveHelper
{
public:
	virtual	char const* GetName(EntityHandle_t handle) const = 0;
	virtual void ResetTouchList(void) = 0;
	virtual bool AddToTouched(const CGameTrace& tr, const vector& impactvelocity) = 0;
	virtual void ProcessImpacts(void) = 0;
	virtual void Con_NPrintf(int idx, char const* fmt, ...) = 0;
	virtual void StartSound(const vector& origin, int channel, char const* sample, float volume, soundlevel_t soundlevel, int fFlags, int pitch) = 0;
	virtual void StartSound(const vector& origin, const char* soundname) = 0;
	virtual void PlaybackEventFull(int flags, int clientindex, unsigned short eventindex, float delay, vector& origin, vector& angles, float fparam1, float fparam2, int iparam1, int iparam2, int bparam1, int bparam2) = 0;
	virtual bool PlayerFallingDamage(void) = 0;
	virtual void PlayerSetAnimation(PLAYER_ANIM playerAnim) = 0;
	virtual IPhysicsSurfaceProps* GetSurfaceProps(void) = 0;
	virtual bool IsWorldEntity(const CBaseHandle& handle) = 0;
	virtual ~IMoveHelper() {}

	void SetHost(C_BaseEntity* host);
};

struct StartSoundParams_t
{
	bool			staticsound;
	int				userdata;
	int				soundsource;
	int				entchannel;
	void* pSfx; //CSfxTable
	vector			origin;
	vector			direction;
	bool			bUpdatePositions;
	float			fvol;
	soundlevel_t	soundlevel;
	int				flags;
	int				pitch;
	int				specialdsp;
	bool			fromserver;
	float			delay;
	int				speakerentity;
	bool			suppressrecording;
	int				initialStreamPosition;
};