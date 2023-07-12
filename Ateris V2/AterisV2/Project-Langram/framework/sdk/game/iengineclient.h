#pragma once

#include "ienginetrace.h"
#include "ibaseclientdll.h"

class ISpatialLeafEnumerator
{
public:
	virtual bool EnumerateLeaf(int leaf, int context) = 0;
};

struct PlayerInfo_t
{
	// scoreboard information
	char            name[32];
	// local server user ID, unique while server is running
	int				userID;
	// global unique player identifer
	char			guid[32 + 1];
	// friends identification number
	uint32_t		friendsID;
	// friends name
	char			friendsName[32];
	// true, if player is a bot controlled by game.dll
	bool			fakeplayer;
	// true if player is the HLTV proxy
	bool			ishltv;
	// custom files CRC for this player
	unsigned long	customFiles[4];
	// this counter increases each time the server downloaded a new file
	unsigned char	filesDownloaded;
};

class ISpatialQuery
{
public:
	virtual int LeafCount() const = 0;
	virtual bool EnumerateLeavesAtPoint(vector const& pt, ISpatialLeafEnumerator* pEnum, int context) = 0;
	virtual bool EnumerateLeavesInBox(vector const& mins, vector const& maxs, ISpatialLeafEnumerator* pEnum, int context) = 0;
	virtual bool EnumerateLeavesInSphere(vector const& center, float radius, ISpatialLeafEnumerator* pEnum, int context) = 0;
	virtual bool EnumerateLeavesAlongRay(Ray_t const& ray, ISpatialLeafEnumerator* pEnum, int context) = 0;
};

class CEngineClient
{
public:
	void GetScreenSize(int& width_out, int& height_out);
	void ServerCmd(const char* szCmdString, bool bReliable);
	bool GetPlayerInfo(int nEntityIndex, PlayerInfo_t* pPlayerInfo);
	int GetPlayerForUserID(int userID);
	bool Con_IsVisible();
	int GetLocalPlayer();
	float Time();
	void GetViewAngles(vector& va);
	vector GetViewAngles()
	{
		vector out;
		GetViewAngles(out);
		return out;
	}
	void SetViewAngles(vector& va);
	int GetMaxClients();
	bool IsInGame();
	bool IsConnected();
	bool IsDrawingLoadingImage();
	const matrix3x4_t& WorldToScreenMatrix();
	bool IsTakingScreenshot();
	bool IsHLTV();
	void ClientCmd_Unrestricted(const char* szCommandString);
	void ServerCmdKeyValues(void* pKV);
	ISpatialQuery* GetBSPTreeQuery();
	CNetChannel* GetNetChannelInfo();
	char const* GetLevelName();
	void FireEvents( );
};