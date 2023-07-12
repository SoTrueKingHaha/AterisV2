#include "game_coordinator_stuff.h"

#include "../../utils/signatures/signatures.h"

void CTFPartyClient::LoadSavedCasualCriteria()
{
	static auto FN = reinterpret_cast<void(__thiscall*)(void*)>(S::CTFPartyClient_LoadSavedCasualCriteria.address);
	return FN(this);
}

bool CTFPartyClient::BInStandbyQueue()
{
	static auto FN = reinterpret_cast<bool(__thiscall*)(void*)>(S::CTFPartyClient_BInStandbyQueue.address);
	return FN(this);
}

//CTFPartyClient::BCanQueueForMatch(ETFMatchGroup, CUtlVector<CTFPartyClient::QueueEligibilityData_t, CUtlMemory<CTFPartyClient::QueueEligibilityData_t, int>>&)
bool CTFPartyClient::BCanQueueForMatch(EMatchGroup eMatchGroup, void* vecQueueEligibilityData)
{
	static auto FN = reinterpret_cast<bool(__thiscall*)(void*, int, void*)>(S::CTFPartyClient_BCanQueueForMatch.address);
	return FN(this, eMatchGroup, vecQueueEligibilityData);
}

bool CTFPartyClient::BCanQueueForStandby()
{
	static auto FN = reinterpret_cast<bool(__thiscall*)(void*)>(S::CTFPartyClient_BCanQueueForStandby.address);
	return FN(this);
}

bool CTFPartyClient::BInQueueForMatchGroup(EMatchGroup eMatchGroup)
{
	static auto FN = reinterpret_cast<bool(__thiscall*)(void*, int)>(S::CTFPartyClient_BInQueueForMatchGroup.address);
	return FN(this, eMatchGroup);
}

//CTFPartyOptions&
bool CTFPartyClient::BMakeUpdateMsg(void* pPartyOptions)
{
	static auto FN = reinterpret_cast<bool(__thiscall*)(void*, void*)>(S::CTFPartyClient_BMakeUpdateMsg.address);
	return FN(this, pPartyOptions);
}

void CTFPartyClient::CancelOutgoingJoinRequestOrIncomingInvite(CSteamID steamID)
{
	static auto FN = reinterpret_cast<void(__thiscall*)(void*, CSteamID)>(S::CTFPartyClient_CancelOutgoingJoinRequestOrIncomingInvite.address);
	return FN(this, steamID);
}

void CTFPartyClient::CheckResetSentOptions()
{
	static auto FN = reinterpret_cast<void(__thiscall*)(void*)>(S::CTFPartyClient_CheckResetSentOptions.address);
	return FN(this);
}

void CTFPartyClient::OnInQueueChanged()
{
	static auto FN = reinterpret_cast<void(__thiscall*)(void*)>(S::CTFPartyClient_OnInQueueChanged.address);
	return FN(this);
}

void CTFPartyClient::RequestQueueForStandby()
{
	static auto FN = reinterpret_cast<void(__thiscall*)(void*)>(S::CTFPartyClient_RequestQueueForStandby.address);
	return FN(this);
}

bool CTFPartyClient::UpdateActiveParty()
{
	static auto FN = reinterpret_cast<bool(__thiscall*)(void*)>(S::CTFPartyClient_UpdateActiveParty.address);
	return FN(this);
}

void CTFPartyClient::RequestQueueForMatch(int eMatchGroup)
{
	static auto FN = reinterpret_cast<void(__thiscall*)(void*, int)>(S::CTFPartyClient_RequestQueueForMatch.address);
	return FN(this, eMatchGroup);
}

#define MAKE_CLIENTSYSFUNC(name, type, ...) static auto FN = reinterpret_cast<type(__thiscall*)(void*)>(S::CTFGCClientSystem_##name.address);
#define MAKE_CLIENTSYSFUNCVARS(name, type, ...) static auto FN = reinterpret_cast<type(__thiscall*)(void*, __VA_ARGS__)>(S::CTFGCClientSystem_##name.address);

bool CTFGCClientSystem::Init()
{
	MAKE_CLIENTSYSFUNC(Init, bool);
	return FN(this);
}

void CTFGCClientSystem::PreInitGC()
{
	MAKE_CLIENTSYSFUNC(PreInitGC, void);
	return FN(this);
}

void CTFGCClientSystem::PostInit()
{
	MAKE_CLIENTSYSFUNC(PostInit, void);
	return FN(this);
}

void CTFGCClientSystem::PostInitGC()
{
	MAKE_CLIENTSYSFUNC(PostInitGC, void);
	return FN(this);
}

void CTFGCClientSystem::ReceivedClientWelcome(const int& msg)
{
	MAKE_CLIENTSYSFUNCVARS(ReceivedClientWelcome, void, const int&);
	return FN(this, msg);
}

void CTFGCClientSystem::Shutdown()
{
	MAKE_CLIENTSYSFUNC(Shutdown, void);
	return FN(this);
}

CTFPartyClient* CTFGCClientSystem::GetParty()
{
	MAKE_CLIENTSYSFUNC(GetParty, CTFPartyClient*);
	return FN(this);
}

bool CTFGCClientSystem::BHaveLiveMatch()
{
	MAKE_CLIENTSYSFUNC(BHaveLiveMatch, bool);
	return FN(this);
}

int CTFGCClientSystem::GetNumMatchInvites()
{
	MAKE_CLIENTSYSFUNC(GetNumMatchInvites, int);
	return FN(this);
}

bool CTFGCClientSystem::JoinMMMatch()
{
	MAKE_CLIENTSYSFUNC(JoinMMMatch, bool);
	return FN(this);
}

bool CTFGCClientSystem::BConnectedToMatchServer(bool bLiveMatch)
{
	MAKE_CLIENTSYSFUNCVARS(BConnectedToMatchServer, bool, bool);
	return FN(this, bLiveMatch);
}

bool CTFGCClientSystem::BGetLocalPlayerBadgeInfoForTour(int iTourIndex, uint32* pnBadgeLevel, uint32* pnCompletedChallenges)
{
	MAKE_CLIENTSYSFUNCVARS(BGetLocalPlayerBadgeInfoForTour, bool, int, uint32*, uint32*);
	return FN(this, iTourIndex, pnBadgeLevel, pnCompletedChallenges);
}

bool CTFGCClientSystem::BHasCompetitiveAccess()
{
	MAKE_CLIENTSYSFUNC(BHasCompetitiveAccess, bool);
	return FN(this);
}

bool CTFGCClientSystem::BIsMatchGroupDisabled(EMatchGroup eMatchGroup)
{
	MAKE_CLIENTSYSFUNCVARS(BIsMatchGroupDisabled, bool, EMatchGroup);
	return FN(this, eMatchGroup);
}

void CTFGCClientSystem::ConnectToServer(const char* connect)
{
	MAKE_CLIENTSYSFUNCVARS(ConnectToServer, void, const char*);
	return FN(this, connect);
}

void CTFGCClientSystem::PingThink()
{
	MAKE_CLIENTSYSFUNC(PingThink, void);
	return FN(this);
}

void CTFGCClientSystem::DumpPing()
{
	MAKE_CLIENTSYSFUNC(DumpPing, void);
	return FN(this);
}

void CTFGCClientSystem::FireGameEvent(CGameEvent* pEvent)
{
	MAKE_CLIENTSYSFUNCVARS(FireGameEvent, void, CGameEvent*);
	return FN(this, pEvent);
}

void* CTFGCClientSystem::GetLobby()
{
	MAKE_CLIENTSYSFUNC(GetLobby, void*);
	return FN(this);
}

void* CTFGCClientSystem::GetMatchInvite(void* pUnknown1, void* pUnknown2)
{
	MAKE_CLIENTSYSFUNCVARS(GetMatchInvite, void*, void*, void*);
	return FN(this, pUnknown1, pUnknown2);
}

void CTFGCClientSystem::DumpLobby()
{
	MAKE_CLIENTSYSFUNC(DumpLobby, void);
	return FN(this);
}

void CTFGCClientSystem::DumpParty()
{
	MAKE_CLIENTSYSFUNC(DumpParty, void);
	return FN(this);
}
