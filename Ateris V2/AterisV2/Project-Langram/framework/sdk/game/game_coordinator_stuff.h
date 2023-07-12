#pragma once

#include "Steam/SteamTypes.h"
#include "convar.h"
#include "utlvector.h"
#include "igameevents.h"

using CTFGSLobby = void*;
using CMatchInfo = void*;

// Stored value, don't re-order
enum EMatchGroup
{
	k_nMatchGroup_Invalid = -1,
	k_nMatchGroup_First = 0,

	k_nMatchGroup_MvM_Practice = 0,
	k_nMatchGroup_MvM_MannUp,

	k_nMatchGroup_Ladder_6v6,
	k_nMatchGroup_Ladder_9v9,
	k_nMatchGroup_Ladder_12v12,

	k_nMatchGroup_Casual_6v6,
	k_nMatchGroup_Casual_9v9,
	k_nMatchGroup_Casual_12v12,

	k_nMatchGroup_Count,
	// When adding a new matchgroup, add case handling to GetMatchSizeForMatchGroup(), GetMatchGroupName(), GetServerPoolName(), GetMaxLobbySizeForMatchGroup(), YldWebAPIServersByDataCenter()
};

// Late join modes
enum EMatchMode
{
	// Uninitialized/unknown
	eMatchMode_Invalid,
	// Not late join / don't use late join
	eMatchMode_MatchMaker_CompleteFromQueue,
	// The add-one-player-at-a-time mode that doesn't work with the new scoring system, but still used for MvM and other
	// old-scoring-system stuff.
	eMatchMode_MatchMaker_LateJoinDropIn,
	// The new late join mode that re-evaulates complete matches with the missing spot(s) filled.
	eMatchMode_MatchMaker_LateJoinMatchBased,
	// A match that is being manually crafted
	eMatchMode_Manual,
};

// Separate penalty pools (and rules) for different classes of modes
enum EMMPenaltyPool
{
	eMMPenaltyPool_Invalid,
	eMMPenaltyPool_Casual, // Pool with lenient penalties for most casual/mainstream gamemodes
	eMMPenaltyPool_Ranked  // Pool with strict and cumulative penalties for ranked gamemodes where abandons tank matches
};

enum EMatchType_t
{
	MATCH_TYPE_NONE = 0,
	MATCH_TYPE_MVM,
	MATCH_TYPE_COMPETITIVE,
	MATCH_TYPE_CASUAL
};

struct LevelInfo_t
{
	uint32 m_nLevelNum;
	uint32 m_nStartXP;	// Inclusive
	uint32 m_nEndXP;	// Non-inclusive
	const char* m_pszLevelIcon; // Kill this when we do models
	const char* m_pszLevelTitle;
	const char* m_pszLevelUpSound;
	const char* m_pszLobbyBackgroundImage;
};

struct XPSourceDef_t
{
	const char* m_pszSoundName;
	const char* m_pszFormattingLocToken;
	const char* m_pszTypeLocToken;
	float m_flValueMultiplier;
};

struct MatchDesc_t
{
	EMatchMode		m_eLateJoinMode;
	EMMPenaltyPool  m_ePenaltyPool;
	bool			m_bUsesSkillRatings;
	bool			m_bSupportsLowPriorityQueue;
	bool			m_bRequiresMatchID;
	const ConVar* m_pmm_required_score;
	bool			m_bUseMatchHud;
	const char* m_pszExecFileName;
	const ConVar* m_pmm_match_group_size;
	const ConVar* m_pmm_match_group_size_minimum; // Optional
	EMatchType_t	m_eMatchType;
	bool			m_bShowPreRoundDoors;
	bool			m_bShowPostRoundDoors;
	const char* m_pszMatchEndKickWarning;
	const char* m_pszMatchStartSound;
	bool			m_bAutoReady;
	bool			m_bShowRankIcons;
	bool			m_bUseMatchSummaryStage;
	bool			m_bDistributePerformanceMedals;
	bool			m_bIsCompetitiveMode;
	bool			m_bUseFirstBlood;
	bool			m_bUseReducedBonusTime;
	bool			m_bUseAutoBalance;
	bool			m_bAllowTeamChange;
	bool			m_bRandomWeaponCrits;
	bool			m_bFixedWeaponSpread;
	// If we should not allow match to complete without a complete set of players.
	bool 			m_bRequireCompleteMatch;
	bool			m_bTrustedServersOnly;
	bool			m_bForceClientSettings;
	bool			m_bAllowDrawingAtMatchSummary;
	bool			m_bAllowSpecModeChange;
	bool			m_bAutomaticallyRequeueAfterMatchEnds;
	bool			m_bUsesMapVoteOnRoundEnd;
	bool			m_bUsesXP;
	bool			m_bUsesDashboardOnRoundEnd;
	bool			m_bUsesSurveys;
	// Be strict about finding quality matches, for more-competitive matchgroups that want to prioritize match quality
	// over speed.
	bool			m_bStrictMatchmakerScoring;
};

class IProgressionDesc
{
public:
	virtual void SetupBadgePanel(void* pModelPanel, const LevelInfo_t& level) const = 0;
	virtual const uint32 GetLocalPlayerLastAckdExperience() const = 0;
	virtual const uint32 GetPlayerExperienceBySteamID(CSteamID steamid) const = 0;
	virtual const LevelInfo_t& YieldingGetLevelForSteamID(const CSteamID& steamID) const = 0;
	virtual const LevelInfo_t& GetLevelForExperience(uint32 nExperience) const = 0;

	const CUtlString m_strBadgeName;
	const char* m_pszLevelToken;
	const char* m_pszProgressionResFile;

protected:

	const EMatchGroup m_eMatchGroup;
	CUtlVector< LevelInfo_t > m_vecLevels;
};

class IMatchGroupDescription
{
public:

	virtual bool BGetRoundStartBannerParameters(int& nSkin, int& nBodyGroup) const = 0;
	virtual bool BGetRoundDoorParameters(int& nSkin, int& nLogoBodyGroup) const = 0;
	virtual const char* GetMapLoadBackgroundOverride(bool bWideScreen) const = 0;

	// ! Check return, we might fail to setup
	virtual bool InitServerSettingsForMatch(const CTFGSLobby* pLobby) const = 0;
	virtual void InitGameRulesSettings() const = 0;
	virtual void InitGameRulesSettingsPostEntity() const = 0;
	virtual void PostMatchClearServerSettings() const = 0;
	virtual bool ShouldRequestLateJoin() const = 0;
	virtual bool BMatchIsSafeToLeaveForPlayer(const CMatchInfo* pMatchInfo, const void* pMatchPlayer) const = 0;
	virtual bool BPlayWinMusic(int nWinningTeam, bool bGameOver) const = 0;

	const EMatchGroup m_eMatchGroup;
	const MatchDesc_t m_params;
	const IProgressionDesc* m_pProgressionDesc;
};

class CTFPartyClient
{
public:
	void LoadSavedCasualCriteria();
	bool BInStandbyQueue();
	bool BCanQueueForMatch(EMatchGroup eMatchGroup, void* vecQueueEligibilityData);
	bool BCanQueueForStandby();
	bool BInQueueForMatchGroup(EMatchGroup eMatchGroup);
	bool BMakeUpdateMsg(void* pPartyOptions);
	void CancelOutgoingJoinRequestOrIncomingInvite(CSteamID steamID);
	void CheckResetSentOptions();
	void OnInQueueChanged();
	void RequestQueueForStandby();
	bool UpdateActiveParty();
	void RequestQueueForMatch(int eMatchGroup);
};

class CTFGCClientSystem
{
public:
	bool Init();
	void PreInitGC();
	void PostInit();
	void PostInitGC();
	void ReceivedClientWelcome(const int& msg);
	void Shutdown();
	void Update(float frametime);
	CTFPartyClient* GetParty();
	bool BHaveLiveMatch();
	int GetNumMatchInvites();
	bool JoinMMMatch();
	bool BConnectedToMatchServer(bool bLiveMatch);
	bool BGetLocalPlayerBadgeInfoForTour(int iTourIndex, uint32* pnBadgeLevel, uint32* pnCompletedChallenges);
	bool BHasCompetitiveAccess();
	bool BIsMatchGroupDisabled(EMatchGroup eMatchGroup);
	void ConnectToServer(const char* connect);
	void PingThink();
	void DumpPing();
	void FireGameEvent(CGameEvent* pEvent);
	void* GetLobby();
	void* GetMatchInvite(void* pUnknown1, void* pUnknown2);
	void DumpLobby();
	void DumpParty();
};