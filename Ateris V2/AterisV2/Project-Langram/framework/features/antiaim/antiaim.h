#pragma once
#include "../menu/vars.h"
#include "../../utils/entitycache/entitycache.h"
#include "../../utils/utils.h"

struct FakeMatrixes
{
	float BoneMatrix[ 128 ][ 3 ][ 4 ];
};

class __antiaim
{
	bool bPacketFlip = false;
	bool bFakeJitter = false;
	bool bRealJitter = false;
	bool bPitchJitter = false;
	float lastRealAngle = -90.f;
	float lastFakeAngle = 90.f;
	int edgeToEdgeOn = 0;
	float EdgeDistance( float edgeRayYaw );
	bool FindEdge( float edgeOrigYaw );

	//legit
	float get_legit_yaw( ); //fake yaw static forward + real

	//rage
	float get_real_rage_yaw( ); 	// real - static + jitter + spin
	float get_fake_rage_yaw( ); 	// fake - static + jitter + spin
	std::pair<float, float> get_rage_pitch( ); //real + fake
	// automatic + manual
	// real - up + down / fake
	//bool IsScopedPlayer( );

	//main
	void fix_movement( CUserCmd* pCmd, const vector& vOldAngles, float fOldSideMove, float fOldForwardMove );
public:
	bool bRealFlip = false;
	bool bFakeFlip = false;
	bool toggle = true;
	void run_real( C_TFPlayer* pLocal, C_TFWeaponBase* weapon, CUserCmd* pCmd, bool* bSendPacket );
	void run_fake( C_TFPlayer* pLocal, C_TFWeaponBase* weapon, CUserCmd* pCmd, bool* bSendPacket );
};

inline __antiaim antiaim;