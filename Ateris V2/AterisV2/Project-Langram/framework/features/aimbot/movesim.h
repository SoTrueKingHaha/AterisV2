#pragma once
#include "aimbot.h"

class __move_sim
{
public:
	C_TFPlayer* m_pPlayer = nullptr;
	CMoveData m_MoveData = {};

private:
	//void SetCurrentCommand(CBaseEntity* pPlayer, CUserCmd* pCmd);
	void SetupMoveData( C_TFPlayer* pPlayer, CMoveData* pMoveData );

private:
	bool bFirstRunTick = true;
	bool m_bOldInPrediction = false;
	bool m_bOldFirstTimePredicted = false;
	bool bDontPredict = false;
	float m_flOldFrametime = 0.0f;
	int iTick = 0;

	std::map<int, std::deque<vector>> m_Velocities;
public:
	bool init( C_TFPlayer* pPlayer );
	void restore( );
	void FillVelocities( );
	void run_tick( CMoveData& moveDataOut, vector& m_vecAbsOrigin );
	const vector& GetOrigin( ) { return m_MoveData.m_vecAbsOrigin; }
};

inline __move_sim move_sim;