#pragma once
#include "iprediction.h"

class CGameMovement
{
public:
	virtual			~CGameMovement(void) {}

	// Process the current movement command
	virtual void	ProcessMovement(C_BaseEntity* pPlayer, CMoveData* pMove) = 0;
	virtual void	StartTrackPredictionErrors(C_BaseEntity* pPlayer) = 0;
	virtual void	FinishTrackPredictionErrors(C_BaseEntity* pPlayer) = 0;
	virtual void	DiffPrint(char const* fmt, ...) = 0;

	// Allows other parts of the engine to find out the normal and ducked player bbox sizes
	virtual vector GetPlayerMins(bool ducked) const = 0;
	virtual vector GetPlayerMaxs(bool ducked) const = 0;
	virtual vector GetPlayerViewOffset(bool ducked) const = 0;
};