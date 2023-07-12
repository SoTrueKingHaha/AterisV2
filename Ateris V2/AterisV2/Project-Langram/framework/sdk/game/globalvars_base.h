#pragma once

#include "ibaseclientdll.h"
#include "utlstring.h"
#include "md5.h"

class CSaveRestoreData;

class CGlobalVarsBase
{
public:
	float realtime;
	int framecount;
	float absoluteframetime;
	float curtime;
	float frametime;
	int maxClients;
	int tickcount;
	float interval_per_tick;
	float interpolation_amount;
	int simTicksThisFrame;
	int network_protocol;
	CSaveRestoreData* pSaveData;
	bool m_bClient;
	int nTimestampNetworkingBase;
	int nTimestampRandomizeWindow;
};

enum 
{ 
	NUM_CLOCKDRIFT_SAMPLES = 16 
};

class CClockDriftMgr
{
public:
	float clock_offsets[ NUM_CLOCKDRIFT_SAMPLES ];
	int current_offset = 0;
	int server_tick = 0;
	int client_tick = 0;
};

class CBaseClientState
{
private:
	char pad_0000[8]; //0x0000
public:
	void* thisptr = nullptr; //0x0008
private:
	char pad_000C[4]; //0x000C
public:
	CNetChannel* m_NetChannel = nullptr; //0x0010
private:
	char pad_0014[320]; //0x0014
public:
	CClockDriftMgr m_ClockDriftMgr = { }; //0x0154
	int m_nDeltaTick = 0; //0x01A0
private:
	char pad_01A4[16]; //0x01A4
public:
	char m_szLevelFileName[128]; //0x01B4
	char m_szLevelBaseName[128]; //0x0234
	int m_nMaxClients = 0; //0x02B4
private:
	char pad_02B8[18528]; //0x02B8
public:
	int	oldtickcount = 0;		//0x4B18
	float m_tickRemainder = 0.f;	//0x4B1C
	float m_frameTime = 0.f; //0x4B20
	int lastoutgoingcommand = 0; //0x4B24
	int chokedcommands = 0; //0x4B28
	int last_command_ack = 0; //0x4B2C
};