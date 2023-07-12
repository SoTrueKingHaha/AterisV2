#include "iengineclient.h"

#include "../../utils/memory/memory.h"

void CEngineClient::GetScreenSize(int& width_out, int& height_out)
{
	static auto FN = reinterpret_cast<void(__thiscall*)(void*, int&, int&)>(memory->get_vfunc(this, 5));
	return FN(this, width_out, height_out);
}

void CEngineClient::ServerCmd(const char* szCmdString, bool bReliable)
{
	static auto FN = reinterpret_cast<void(__thiscall*)(void*, const char*, bool)>(memory->get_vfunc(this, 6));
	return FN(this, szCmdString, bReliable);
}

bool CEngineClient::GetPlayerInfo(int nEntityIndex, PlayerInfo_t* pPlayerInfo)
{
	static auto FN = reinterpret_cast<bool(__thiscall*)(void*, int, PlayerInfo_t*)>(memory->get_vfunc(this, 8));
	return FN(this, nEntityIndex, pPlayerInfo);
}

int CEngineClient::GetPlayerForUserID(int userID)
{
	static auto FN = reinterpret_cast<bool(__thiscall*)(void*, int)>(memory->get_vfunc(this, 9));
	return FN(this, userID);
}

bool CEngineClient::Con_IsVisible()
{
	static auto FN = reinterpret_cast<bool(__thiscall*)(void*)>(memory->get_vfunc(this, 11));
	return FN(this);
}

int CEngineClient::GetLocalPlayer()
{
	static auto FN = reinterpret_cast<int(__thiscall*)(PVOID)>(memory->get_vfunc(this, 12));
	return FN(this);
}
float CEngineClient::Time()
{
	static auto FN = reinterpret_cast<float(__thiscall*)(PVOID)>(memory->get_vfunc(this, 14));
	return FN(this);
}

void CEngineClient::GetViewAngles(vector& va)
{
	static auto FN = reinterpret_cast<void(__thiscall*)(PVOID, vector & va)>(memory->get_vfunc(this, 19));
	return FN(this, va);
}

void CEngineClient::SetViewAngles(vector& va)
{
	static auto FN = reinterpret_cast<void(__thiscall*)(PVOID, vector & va)>(memory->get_vfunc(this, 20));
	return FN(this, va);
}

int CEngineClient::GetMaxClients()
{
	static auto FN = reinterpret_cast<int(__thiscall*)(PVOID)>(memory->get_vfunc(this, 21));
	return FN(this);
}

bool CEngineClient::IsInGame()
{
	static auto FN = reinterpret_cast<bool(__thiscall*)(PVOID)>(memory->get_vfunc(this, 26));
	return FN(this);
}

bool CEngineClient::IsConnected()
{
	static auto FN = reinterpret_cast<bool(__thiscall*)(PVOID)>(memory->get_vfunc(this, 27));
	return FN(this);
}

bool CEngineClient::IsDrawingLoadingImage()
{
	static auto FN = reinterpret_cast<bool(__thiscall*)(PVOID)>(memory->get_vfunc(this, 28));
	return FN(this);
}

const matrix3x4_t& CEngineClient::WorldToScreenMatrix()
{
	static auto FN = reinterpret_cast<const matrix3x4_t & (__thiscall*)(void*)>(memory->get_vfunc(this, 36));
	return FN(this);
}

bool CEngineClient::IsTakingScreenshot()
{
	static auto FN = reinterpret_cast<bool(__thiscall*)(PVOID)>(memory->get_vfunc(this, 85));
	return FN(this);
}

bool CEngineClient::IsHLTV()
{
	static auto FN = reinterpret_cast<bool(__thiscall*)(PVOID)>(memory->get_vfunc(this, 86));
	return FN(this);
}

void CEngineClient::ClientCmd_Unrestricted(const char* szCommandString)
{
	static auto FN = reinterpret_cast<void(__thiscall*)(PVOID, const char*)>(memory->get_vfunc(this, 106));
	return FN(this, szCommandString);
}

void CEngineClient::ServerCmdKeyValues(void* pKV)
{
	static auto FN = reinterpret_cast<void(__thiscall*)(void*, void*)>(memory->get_vfunc(this, 128));
	return FN(this, pKV);
}

ISpatialQuery* CEngineClient::GetBSPTreeQuery()
{
	static auto FN = reinterpret_cast<ISpatialQuery * (__thiscall*)(void*)>(memory->get_vfunc(this, 42));
	return FN(this);
}

CNetChannel* CEngineClient::GetNetChannelInfo()
{
	static auto FN = reinterpret_cast<CNetChannel * (__thiscall*)(void*)>(memory->get_vfunc(this, 72));
	return FN(this);
}

char const* CEngineClient::GetLevelName()
{
	static auto FN = reinterpret_cast<char const* (__thiscall*)(void*)>(memory->get_vfunc(this, 51));
	return FN(this);
}

void CEngineClient::FireEvents( )
{
	static auto FN = reinterpret_cast< void( __thiscall * )( void * ) >( memory->get_vfunc( this, 56 ) );
	return FN( this );
}