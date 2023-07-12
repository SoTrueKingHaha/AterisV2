#pragma once
#include "../memory/memory.h"

struct signature_t
{
public:
	DWORD address{ 0x0 };
	const char *dll{ nullptr };
	const char *sig{ nullptr };
	const char *name{ nullptr };
	int offset{ 0 };

	signature_t( const char *dll, const char *sig, const char *name, int offset );
};

class __signature_manager
{
public:
	std::vector<signature_t *> signatures;
	bool init( );
};
inline __signature_manager signature_manager;

#define MAKE_SIGNATURE(name, dll, sig, offset) namespace S { inline signature_t name(dll, sig, #name, offset); }

MAKE_SIGNATURE( PlayerOnFriendsList, "client.dll", "55 8B EC 81 EC ? ? ? ? E8 ? ? ? ? 85 C0 74 7E 8B 4D 08 85 C9 74 77 A1 ? ? ? ? 83 78 08 00 74 6C 83 78 0C 00 74 66 A1 ? ? ? ? 83 C1 08 56 8B 30 8D 85 ? ? ? ? 50 8B 01", 0 );
MAKE_SIGNATURE( CalcMaxSpeed, "client.dll", "E8 ? ? ? ? D9 96 ? ? ? ? D9 EE DB F1", 5 )
MAKE_SIGNATURE( CreateTFRagdoll, "client.dll", "55 8B EC B8 ? ? ? ? E8 ? ? ? ? 53 56 57 8B F9 8B 8F ? ? ? ? 85 C9 0F 85", 0 )
MAKE_SIGNATURE( ProcessDSP, "engine.dll", "55 8B EC 51 56 8B 75 08 83 FE 1F 0F 87 ? ? ? ? 8B 0D ? ? ? ? 8B 01 8B 40 2C FF D0 84 C0 0F 84 ? ? ? ? A1 ? ? ? ? 6B F6 70 81 C6 ? ? ? ? 83 78 30 00 0F 85 ? ? ? ? 83", 0 )
MAKE_SIGNATURE( KeyValues_SetInt, "client.dll", "55 8B EC 6A 01 FF 75 08 E8 ? ? ? ? 85 C0 74 0A 8B 4D 0C 89 48 0C C6 40 10 02", 0 )
MAKE_SIGNATURE( IsPlayerDominated, "client.dll", "55 8B EC 56 57 8B F1 E8 ? ? ? ? 8B F8 85 FF 75 08 5F 32 C0 5E 5D C2 04 00", 0 )
MAKE_SIGNATURE( CL_FireEvents, "engine.dll", "55 8B EC 51 80 B9 ? ? ? ? ? F3 0F 2A 81 ? ? ? ? F3 0F 59 05 ? ? ? ?", 0 )

MAKE_SIGNATURE( Scoreboard_ReturnAddressDesired, "client.dll", "8B 03 8B CB 56 57 FF 90 ? ? ? ? 6A", 0 );
MAKE_SIGNATURE( Scoreboard_ReturnAddressJump, "client.dll", "89 45 BC E8 ? ? ? ? 3B C7 75 1D 80 7D F8 00 75 17 8B 4D C0", 0 );
MAKE_SIGNATURE( IsPlayerDominated_ReturnAddressDesired, "client.dll", "84 C0 74 ? 80 7D ? ? 74 ? 8B 83", 0 );
MAKE_SIGNATURE( NotificationQueue_Add, "client.dll", "55 8B EC 8B 0D ? ? ? ? 56 57 8B 01 8B 40 68 FF D0 8B 75 08 84 C0 75 09 80 BE ? ? ? ? ? 75 29", 0 );
MAKE_SIGNATURE( CTFPlayer_OnHasNewItems_NotificationQueue_Add_Call, "client.dll", "83 C4 ? 5E C3 CC CC CC CC CC CC CC 56 8B F1 E8", 0 );
MAKE_SIGNATURE( C_BaseEntity_FireBullets, "client.dll", "55 8B EC 81 EC ? ? ? ? 53 56 57 8B F9 8B 5D", 0 );
MAKE_SIGNATURE( C_BasePlayer_PlayerOnFriendsList, "client.dll", "55 8B EC 81 EC ? ? ? ? E8 ? ? ? ? 85 C0 74 7E 8B 4D 08 85 C9 74 77 A1 ? ? ? ? 83 78 08 00 74 6C 83 78 0C 00 74 66 A1 ? ? ? ? 83 C1 08 56 8B 30 8D 85 ? ? ? ? 50 8B 01", 0 );
MAKE_SIGNATURE( C_BaseEntity_PredictionPlayer, "client.dll", "89 3D ? ? ? ? F3 0F 2A 87 ? ? ? ? F3 0F 59 40 ? F3 0F 11 40 ? 80 7E", 0 );
MAKE_SIGNATURE( C_BaseEntity_SaveData, "client.dll", "55 8B EC 83 EC ? 53 57 8B 7D ? 8B D9 83 FF ? 75", 0 );
MAKE_SIGNATURE( C_BaseEntity_RestoreData, "client.dll", "55 8B EC 83 EC ? 53 56 57 8B 7D ? 8B D9 83 FF ? 75", 0 );
MAKE_SIGNATURE( C_BaseEntity_UpdateButtonState, "client.dll", "55 8B EC 8B 81 ? ? ? ? 8B D0 56 8B 75 ?", 0 );
MAKE_SIGNATURE( C_BasePlayer_PhysicsRunThink, "client.dll", "55 8B EC 53 8B D9 56 57 8B 83 ? ? ? ? C1 E8 16 A8 01 B0 01 0F 85 ? ? ? ?", 0 );
MAKE_SIGNATURE( C_BasePlayer_UsingStandardWeaponsInVehicle, "client.dll", "E8 ? ? ? ? 84 C0 74 ? 0F B6 43 ? 89 87", 5 );
MAKE_SIGNATURE( FX_FireBullets, "client.dll", "55 8B EC 81 EC ? ? ? ? 53 8B 5D ? 56 53", 0 );
MAKE_SIGNATURE( GetWeaponSpread, "client.dll", "55 8B EC 83 EC 08 56 8B F1 57 6A 01 6A 00 8B 96 ? ? ? ? 8B 86 ? ? ? ? C1 E2 06 56 68 ? ? ? ? 51", 0 );
MAKE_SIGNATURE( GetMaxItemCount, "client.dll", "8B 49 68 56", 0 );
MAKE_SIGNATURE( FrameAdvance, "client.dll", "55 8B EC 83 EC 14 56 8B F1 57 80", 0 );
MAKE_SIGNATURE( RunSimulation, "client.dll", "55 8B EC 51 53 56 8B 75 ? 57 FF 75 ? 8B F9 8D 8E", 0 );
MAKE_SIGNATURE( PhysicsSimulate, "client.dll", "55 8B EC A1 ? ? ? ? 83 EC ? 8B 40 ? 56 8B F1 39 86", 0 );
MAKE_SIGNATURE( CheckForSequenceChange, "client.dll", "55 8B EC 53 8B 5D 08 57 8B F9 85 DB 0F 84 ? ? ? ? 83 7F 0C 00 75 05 E8 ? ? ? ? 6B 4F 0C 2C 0F 57 C0 56 8B 37 83 C6 D4 03 F1 F3 0F 10 4E ? 0F 2E C8 9F F6 C4 44 7B 62 8B 45 0C", 0 );
MAKE_SIGNATURE( Connect, "engine.dll", "55 8B EC 53 56 57 68 ? ? ? ? 8B F1", 0 );
MAKE_SIGNATURE( Disconnect, "engine.dll", "55 8B EC 81 EC ? ? ? ? 57 8B 7D 08 83 BF ? ? ? ? ? 0F 84 ? ? ? ? 56 8D 77 FC 56 E8 ? ? ? ? 8B C8 E8 ? ? ? ? C7 86 ? ? ? ? ? ? ? ? C7 86 ? ? ? ? ? ? ? ? 8B", 0 );
MAKE_SIGNATURE( AddToCritBucket, "client.dll", "55 8B EC A1 ? ? ? ? F3 0F 10 81 ? ? ? ? F3 0F 10 48 ? 0F 2F C8 76 1D F3 0F 58 45 ? 0F 2F C8 F3 0F 11 81 ? ? ? ? 77 03 0F 28 C1 F3 0F 11 81 ? ? ? ? 5D C2 04 00", 0 );
MAKE_SIGNATURE( IsAllowedToWithdrawFromCritBucket, "client.dll", "55 8B EC 56 8B F1 0F B7 86 ? ? ? ? FF 86 ? ? ? ? 50 E8 ? ? ? ? 83 C4 04 80 B8 ? ? ? ? ? 74 0A F3 0F 10 15", 0 );
MAKE_SIGNATURE( GetAmmoCount, "client.dll", "55 8B EC 56 8B 75 08 57 8B F9 83 FE FF 75 08 5F 33 C0 5E 5D C2 04 00", 0 );
MAKE_SIGNATURE( CanFireRandomCriticalShot, "client.dll", "55 8B EC F3 0F 10 4D ? F3 0F 58 0D ? ? ? ? F3 0F 10 81 ? ? ? ? 0F 2F C1 76 ? 32 C0 5D C2 ? ? B0 ? 5D C2", 0 );
MAKE_SIGNATURE( ViewmodelPos, "client.dll", "55 8B EC 83 EC 70 8B 55 0C 53 8B 5D 08 89 4D FC 8B 02 89 45 E8 8B 42 04 89 45 EC 8B 42 08 89 45 F0 56 57", 0 );
MAKE_SIGNATURE( UseItemConfirm, "client.dll", "55 8B EC A1 ? ? ? ? 83 EC 20 A8 01", 0 );
MAKE_SIGNATURE( SetAbsVelocity, "client.dll", "55 8B EC 83 EC ? 56 57 8B 7D ? 8B F1 F3 0F", 0 );
MAKE_SIGNATURE( C_BasePlayer_PostDataUpdate_SetAbsVelocityCall, "client.dll", "E8 ? ? ? ? 8D 47 ? 39 05", -0x3 );
MAKE_SIGNATURE( ImagePanel_SetImage, "client.dll", "55 8B EC 53 57 8B 7D ? 8B D9 85 FF 74 ? 8B 83 ? ? ? ? 85 C0 74 ? 50 57 E8 ? ? ? ? 83 C4 ? 85 C0 74", 0 );
MAKE_SIGNATURE( CTFHudPlayerHealth_OnThink, "client.dll", "55 8B EC A1 ? ? ? ? 83 EC ? F3 0F 10 40 ? 57 8B F9 0F 2F 87", 0 );
MAKE_SIGNATURE( CTFHudPlayerHealth_CTFHudPlayerHealth, "client.dll", "55 8B EC 83 EC ? 83 7D ? ? 53 56 57 8B F9 74 ? C7 47 ? ? ? ? ? 6A ? FF 75 ? FF 75 ? E8 ? ? ? ? C7 07 ? ? ? ? 80 3D", 0 );
MAKE_SIGNATURE( CHudMedicChargeMeter_OnTick, "client.dll", "55 8B EC 83 EC ? 56 57 8B F1 E8 ? ? ? ? 8B F8 85 FF 0F 84 ? ? ? ? 53 8B CF", 0 );
MAKE_SIGNATURE( C_BaseEntity_Interpolate, "client.dll", "55 8B EC 83 EC 28 53 57 8D 45 FC 8B F9 50 8D 45 F0 50 8D 45 E4 50 8D 45 D8 50 8D 45 08 50 E8 ? ? ? ? 83 7D FC 00 8B D8 74 46 0F B7 8F ? ? ? ? B8 ? ? ? ? 66 3B C8", 0 );
MAKE_SIGNATURE( C_BaseAnimating_Interpolate, "client.dll", "55 8B EC 83 EC 2C 56 8B F1 83 BE ? ? ? ? ? 0F 85 ? ? ? ? F3 0F 10 86 ? ? ? ? 57 33 FF F3 0F 11 45 ? 80 BE ? ? ? ? ? 75 13 FF B6 ? ? ? ? E8 ? ? ? ? 8B 8E ? ? ? ? 88 01 8D 45 FC 8B CE 50", 0 );
MAKE_SIGNATURE( CInterpolatedVarArrayBase_Interpolate, "client.dll", "55 8B EC 83 EC 1C D9 45 0C 8D 45 FC 56 50 83 EC 08 C7 45 ? ? ? ? ? 8D 45 E4 8B F1 D9 5C 24 04 D9 45 08 D9 1C 24 50 E8 ? ? ? ? 84 C0 0F 84 ? ? ? ? F6 46 2C 01 53 57 74 31 83 7D FC 00 B9 ? ? ? ? D9 45 08 B8 ? ? ? ? 0F 44 C1 8B CE 50 8B 06 83 EC 08 DD 1C 24 FF 50 28 50 68 ? ? ? ? FF 15 ? ? ? ? 83 C4 14 80 7D E4 00 74 60 0F B7 5E 0E 8B 55 F0 0F B7 4E 0C 03", 0 );

MAKE_SIGNATURE( ClientTable, "client.dll", "8B 0D ? ? ? ? 8B 02 D9 05", 0 );
MAKE_SIGNATURE( CL_ReadPackets, "engine.dll", "55 8B EC 83 EC 34 8B 0D", 0 );
MAKE_SIGNATURE( CTFPartyClient_BInStandbyQueue, "client.dll", "8A 41 40 C3", 0 );
MAKE_SIGNATURE( KeyValues_GetName, "client.dll", "FF 31 FF 15 ? ? ? ? 83 C4", 0 );
MAKE_SIGNATURE( CTFPlayerInventory_GetMaxItemCount, "client.dll", "8B 49 68 56", 0 );
MAKE_SIGNATURE( PlayerShouldDraw_InCond, "client.dll", "E8 ? ? ? ? 84 C0 75 C5", 5 );
MAKE_SIGNATURE( CL_SendMove, "engine.dll", "55 8B EC 81 EC ? ? ? ? A1 ? ? ? ? 8D", 0 );
MAKE_SIGNATURE( CTFGCClientSystem_PostInit, "client.dll", "56 83 EC ? 8B F1 8B C4", 0 );
MAKE_SIGNATURE( C_BaseEntity_GetSolid, "client.dll", "81 C1 ? ? ? ? 8B 01 FF 60 34", 0 );
MAKE_SIGNATURE( CTFGCClientSystem_Update, "client.dll", "55 8B EC D9 45 08 83 EC 08", 0 );
MAKE_SIGNATURE( RenderBox_NoMaterial, "engine.dll", "55 8B EC 51 8B 45 ? 8B C8 FF 75", 0 );
MAKE_SIGNATURE( RenderWireframeSweptBox, "engine.dll", "55 8B EC 81 EC ? ? ? ? 57 E8", 0 );
MAKE_SIGNATURE( C_TFGameMovement_Duck, "client.dll", "55 8B EC 83 EC 20 53 56 8B D9 57", 0 );
MAKE_SIGNATURE( KeyValues_LoadFromBuffer, "client.dll", "55 8B EC 83 EC 38 53 8B 5D 0C", 0 );
MAKE_SIGNATURE( WearableShouldDraw_InCond, "client.dll", "E8 ? ? ? ? 84 C0 75 E1 6A 03", 5 );
MAKE_SIGNATURE( KeyValues_Initialize, "engine.dll", "FF 15 ? ? ? ? 83 C4 08 89 06 8B C6", 0 );
MAKE_SIGNATURE( C_PlayerResource_GetTeamColor, "client.dll", "55 8B EC 8B 45 08 83 F8 1F", 0 );
MAKE_SIGNATURE( CTFGCClientSystem_DumpLobby, "client.dll", "8B 0D ? ? ? ? 85 C9 74 3D 68", 0 );
MAKE_SIGNATURE( RenderTriangle_NoMaterial, "engine.dll", "55 8B EC 51 8B 45 ? 8B C8 C1 E9", 0 );
MAKE_SIGNATURE( UTIL_ParticleTracer, "client.dll", "55 8B EC FF 75 08 E8 ? ? ? ? D9 EE 83", 0 );
MAKE_SIGNATURE( S_StartDynamicSound, "engine.dll", "55 8B EC 8B 0D ? ? ? ? 81 EC ? ? ? ? 85 C9", 0 );
MAKE_SIGNATURE( SERVER_CTFGrenadePipebombProjectile_Create, "server.dll", "55 8B EC 8B 45 20 53", 0 );
MAKE_SIGNATURE( C_BasePlayer_GetViewModel, "client.dll", "55 8B EC 8B 55 ? 53 8B D9 56 8B 94 93", 0 );
MAKE_SIGNATURE( CEntitySphereQuery_CEntitySphereQuery, "client.dll", "55 8B EC 83 EC 14 D9 45 0C", 0 );
MAKE_SIGNATURE( SetAbsOrigin, "client.dll", "55 8B EC 56 57 8B F1 E8 ? ? ? ? 8B 7D 08 F3 0F 10 07", 0 );
MAKE_SIGNATURE( C_TFWeaponBase_GetWeaponData, "client.dll", "55 8B EC 66 8B ? ? 66 3B 05 ? ? ? ? 73", 0 );
MAKE_SIGNATURE( SERVER_C_BaseEntity_GetBaseAnimating, "server.dll", "55 8B EC 8B 55 ? 85 D2 7E ? A1", 0 );
MAKE_SIGNATURE( C_BaseEntity_AddToInterpolationList, "client.dll", "E8 ? ? ? ? 8B CB E8 ? ? ? ? 85 C0", 5 );
MAKE_SIGNATURE( CTFGCClientSystem_PreInitGC, "client.dll", "56 8B F1 80 BE ? ? ? ? ? 0F 85 ? ? ? ? 68", 0 );
MAKE_SIGNATURE( CTFPlayerShared_InCond, "client.dll", "55 8B EC 83 EC 08 56 57 8B 7D 08 8B F1 83 FF 20", 0 );
MAKE_SIGNATURE( C_BasePlayer_UpdateWearables, "client.dll", "53 8B D9 56 33 F6 39 B3 ? ? ? ? 7E ? 57 90", 0 );
MAKE_SIGNATURE( C_BasePlayer_GetRenderedWeaponModel, "client.dll", "56 8B F1 E8 ? ? ? ? 84 C0 75 ? 39 35", 0 );
MAKE_SIGNATURE( C_BasePlayer_CalcViewModelView, "client.dll", "53 8B D9 56 33 F6 39 B3 ? ? ? ? 7E ? 57 90", 0 );
MAKE_SIGNATURE( ModelRender_DrawModelShadow, "engine.dll", "55 8B EC 83 EC 0C 8B 0D ? ? ? ? 56 57 8B 7D 0C", 0 );
MAKE_SIGNATURE( C_BaseEntity_EstimateAbsVelocity, "client.dll", "55 8B EC 83 EC ? 56 8B F1 E8 ? ? ? ? 3B F0", 0 );
MAKE_SIGNATURE( CL_Move, "engine.dll", "55 8B EC 83 EC ? 83 3D ? ? ? ? 02 0F 8C ? ? 00 00 E8 ? ? ? 00 84 C0", 0 );
MAKE_SIGNATURE( GetMatchGroupDescription, "client.dll", "55 8B EC 8B 45 08 8B 00 83 F8 FF 74 12 85 C0 78 0E", 0 );
MAKE_SIGNATURE( RenderTriangle, "engine.dll", "55 8B EC 81 EC ? ? ? ? 56 E8 ? ? ? ? 8B 0D ? ? ? ? 8B 01 8B 80", 0 );
MAKE_SIGNATURE( CTFGCClientSystem_JoinMMMatch, "client.dll", "56 8B F1 57 8D 8E ? ? ? ? E8 ? ? ? ? 84 C0 74 32", 0 );
MAKE_SIGNATURE( CParticleProperty_Create, "client.dll", "55 8B EC 83 EC 08 53 56 57 8B F9 8B 0D ? ? ? ? 89 7D FC", 0 );
MAKE_SIGNATURE( C_BaseAnimating_DrawModel, "client.dll", "55 8B EC 83 EC 20 8B 15 ? ? ? ? 53 33 DB 56 89 5D E0 8B F1", 0 );
MAKE_SIGNATURE( InterpolateServerEntities, "client.dll", "55 8B EC 83 EC 30 8B 0D ? ? ? ? 53 33 DB 89 5D DC 89 5D E0", 0 );
MAKE_SIGNATURE( CMatchInviteNotification_AcceptMatch, "client.dll", "55 8B EC 83 EC 10 56 8B F1 8B 86 ? ? ? ? 83 E8 00", 0 );
MAKE_SIGNATURE( C_TFPlayer_AvoidPlayers, "client.dll", "55 8B EC 81 EC ? ? ? ? A1 ? ? ? ? 57 8B F9 89 7D D4 83 78 30 00", 0 );
MAKE_SIGNATURE( CNewParticleEffect_DrawModel, "client.dll", "55 8B EC 83 EC 30 53 8B D9 8B 0D ? ? ? ? 56 33 F6 89 75 DC", 0 );
MAKE_SIGNATURE( C_TFWeaponBaseMelee_CalcIsAttackCriticalHelper, "client.dll", "55 8B EC A1 ? ? ? ? 83 EC 08 83 78 30 00 57", 0 );
MAKE_SIGNATURE( RenderLine, "engine.dll", "55 8B EC 81 EC ? ? ? ? 56 E8 ? ? ? ? 8B 0D ? ? ? ? 8B 01 FF 90 ? ? ? ? 8B F0 85 F6", 0 );
MAKE_SIGNATURE( CTFClientScoreBoardDialog_UpdatePlayerList, "client.dll", "55 8B EC 81 EC ? ? ? ? 53 56 8B D9 E8 ? ? ? ? 8B 8B", 0 );
MAKE_SIGNATURE( CNetChan_SendNetMsg, "engine.dll", "55 8B EC 57 8B F9 8D 8F ? ? ? ? E8 ? ? ? ? 85 C0 75 07 B0 01 5F 5D C2 0C 00", 0 );
MAKE_SIGNATURE( CTFPlayerAnimState_Update, "client.dll", "55 8B EC 81 EC ? ? ? ? 53 57 8B F9 8B 9F ? ? ? ? 89 5D E0 85 DB 0F 84", 0 );
MAKE_SIGNATURE( KeyValues_FindKey, "client.dll", "55 8B EC 81 EC ? ? ? ? 56 8B 75 08 57 8B F9 85 F6 0F 84 ? ? ? ? 80 3E 00 0F 84", 0 );
MAKE_SIGNATURE( C_BaseAnimating_UpdateVisibility, "client.dll", "56 8B F1 E8 ? ? ? ? 8B 46 04 8D 4E 04 8B 40 0C FF D0 84 C0 74 2E", 0 );
MAKE_SIGNATURE( CTFGCClientSystem_ReceivedClientWelcome, "client.dll", "55 8B EC 81 EC ? ? ? ? 56 57 FF 75 08 8B F9 E8 ? ? ? ? 68", 0 );
MAKE_SIGNATURE( ISurface_StartDrawing, "vguimatsurface.dll", "55 8B EC 64 A1 ? ? ? ? 6A FF 68 ? ? ? ? 50 64 89 25 ? ? ? ? 83 EC 14", 0 );
MAKE_SIGNATURE( DrawOpaqueRenderable_1_C_BaseAnimating_DrawModel_Call, "client.dll", "8B 0D ? ? ? ? 6A ? 8B 01 FF 50 ? 8B 45 ? 85 C0", 0 );
MAKE_SIGNATURE( CHudCrosshair_GetDrawPosition, "client.dll", "55 8B EC 81 EC ? ? ? ? 53 57 E8 ? ? ? ? 8B 08 89 4D C8 8B 48 04 8B 40 08", 0 );
MAKE_SIGNATURE( CL_LoadWhiteList, "engine.dll", "55 8B EC 83 EC 30 56 8B 75 08 8B CE FF 75 0C 8B 06 FF 50 30 8B C8 81 F9 ? ? ? ? 74 56", 0 );
MAKE_SIGNATURE( ISurface_FinishDrawing, "vguimatsurface.dll", "55 8B EC 6A FF 68 ? ? ? ? 64 A1 ? ? ? ? 50 64 89 25 ? ? ? ? 51 56 6A 00", 0 );
MAKE_SIGNATURE( CTraceFilterSimple_CTraceFilterSimple, "client.dll", "55 8B EC 8B 45 ? 89 41 ? 8B 45 ? 89 41 ? 8B 45 ? 89 41 ? 8B C1 C7 01", 0 );
MAKE_SIGNATURE( C_TFWeaponBase_IsViewModelFlipped, "client.dll", "56 6A 00 68 ? ? ? ? 68 ? ? ? ? 6A 00 8B F1 E8 ? ? ? ? 50 E8 ? ? ? ? 8B C8", 0 );
MAKE_SIGNATURE( CStaticPropMgr_ComputePropOpacity, "engine.dll", "55 8B EC 83 EC 0C A1 ? ? ? ? 53 56 57 8B 7D 08 8B D9 8B 30 8D 4F 04 8B 01", 0 );
MAKE_SIGNATURE( CTFGCClientSystem_GetLobby, "client.dll", "8B 89 ? ? ? ? 85 C9 74 33 68 ? ? ? ? E8 ? ? ? ? 85 C0 74 25 8B 48 18 85 C9 74 1E", 0 );
MAKE_SIGNATURE( C_BaseEntity_UpdateVisibility, "client.dll", "55 8B EC 83 EC 08 56 8B F1 8B 0D ? ? ? ? 8B 01 8B 80 ? ? ? ? FF D0 84 C0 0F 85", 0 );
MAKE_SIGNATURE( SERVER_CBasePlayer_DetermineSimulationTicks, "server.dll", "56 8B F1 33 D2 57 33 FF 39 96 ? ? ? ? 7E 30 53 33 DB 85 D2 78 12", 0 );
MAKE_SIGNATURE( CTFGCClientSystem_GetNumMatchInvites, "client.dll", "8B 89 ? ? ? ? 85 C9 74 12 68 ? ? ? ? E8 ? ? ? ? 85 C0 74 04 8B 40 18 C3", 0 );
MAKE_SIGNATURE( CTFGCClientSystem_DumpParty, "client.dll", "8B 0D ? ? ? ? 85 C9 74 24 68 ? ? ? ? E8 ? ? ? ? 85 C0 74 16 8B 48 18 85 C9 74 0F", 0 );
MAKE_SIGNATURE( R_DrawSkyBox, "engine.dll", "55 8B EC 81 EC ? ? ? ? 8B 0D ? ? ? ? 33 D2 89 55 EC 89 55 F0 89 55 F4 8B 01 89 55 F8 85 C0 74 40", 0 );
MAKE_SIGNATURE( RenderSphere, "engine.dll", "53 8B DC 83 EC ? 83 E4 ? 83 C4 ? 55 8B 6B ? 89 6C 24 ? 8B EC 81 EC ? ? ? ? 56 57 E8 ? ? ? ? 8B 0D", 0 );
MAKE_SIGNATURE( CMatchInviteNotification_OnTick, "client.dll", "55 8B EC 83 EC 0C 56 8B F1 E8 ? ? ? ? 8B 86 ? ? ? ? 85 C0 74 09 83 F8 02 0F 85", 0 );
MAKE_SIGNATURE( CTFGCClientSystem_Shutdown, "client.dll", "55 8B EC 83 EC 08 56 68 ? ? ? ? 8B F1 FF 15 ? ? ? ? A1 ? ? ? ? 83 C4 04 85 C0 74 5B", 0 );
MAKE_SIGNATURE( CTFPartyClient_OnInQueueChanged, "client.dll", "55 8B EC 56 8B 75 08 57 56 8B F9 E8 ? ? ? ? 83 C4 04 84 C0 74 38 83 FE FF 74 33", 0 );
MAKE_SIGNATURE( CTFPartyClient_RequestQueueForStandby, "client.dll", "55 8B EC 83 EC 18 56 8B F1 83 7E 18 00 0F 84 ? ? ? ? 80 BE ? ? ? ? ? 0F 85", 0 );
MAKE_SIGNATURE( CTFGCClientSystem_BHaveLiveMatch, "client.dll", "56 8B F1 8D 8E ? ? ? ? E8 ? ? ? ? 84 C0 74 0D 80 BE ? ? ? ? ? 75 04 B0 01 5E C3", 0 );
MAKE_SIGNATURE( C_TFPlayer_UpdateKillstreakEffects, "client.dll", "55 8B EC 83 EC 50 53 8B D9 56 57 8B 83 ? ? ? ? 85 C0 74 68 6A 00 6A 00 50 8D 8B", 0 );
MAKE_SIGNATURE( CTFPartyClient_BInQueueForMatchGroup, "client.dll", "55 8B EC 56 8B 75 08 57 56 8B F9 E8 ? ? ? ? 83 C4 04 84 C0 74 2C 83 FE FF 74 27", 0 );
MAKE_SIGNATURE( ClientState_GetClientInterpAmount, "engine.dll", "55 8B EC 8B 0D ? ? ? ? 83 EC 10 85 C9 75 26 8B 0D ? ? ? ? 68 ? ? ? ? 8B 01 FF 50 34", 0 );
MAKE_SIGNATURE( C_TF_PlayerResource_GetPlayerConnectionState, "client.dll", "55 8B EC 8B 45 08 85 C0 74 10 83 F8 21 7F 0B 8B 84 81 ? ? ? ? 5D C2 04 00", 0 );
MAKE_SIGNATURE( StaticPropMgr_DrawStaticProps, "engine.dll", "55 8B EC 83 EC 10 8B 15 ? ? ? ? 53 56 57 33 F6 89 4D FC 33 FF 89 75 F0 89 7D F4 8B 42 08", 0 );
MAKE_SIGNATURE( CTFPartyClient_CancelOutgoingJoinRequestOrIncomingInvite, "client.dll", "55 8B EC 83 EC 14 56 8B F1 83 7E 18 00 74 0A 80 7E 28 00 0F 84", 0 );
MAKE_SIGNATURE( HudScopeShouldDraw_InCond, "client.dll", "84 C0 74 ? 8B CE E8 ? ? ? ? 85 C0 74 ? 8B CE E8 ? ? ? ? 8B C8 8B 10 FF 92 ? ? ? ? 83 F8 ? 0F 94", 0 );
MAKE_SIGNATURE( CTFClientScoreBoardDialog_UpdatePlayerList_C_TF_PlayerResource_GetPlayerConnectionState_Call, "client.dll", "33 C9 89 45 ? 89 4D ? 85 C0 75", 0 );
MAKE_SIGNATURE( CTFPartyClient_BCanQueueForMatch, "client.dll", "55 8B EC 81 EC ? ? ? ? 8B 45 0C 89 45 FC 8D 45 08 56 50 8B F1 E8 ? ? ? ? 83 C4 04 89 45 F4", 0 );
MAKE_SIGNATURE( CSharedObjectCache_FindBaseTypeCache, "client.dll", "55 8B EC 83 EC ? 8B 45 ? 53 56 8B D9 89 45 ? 57 8D 45 ? 50 8D 4B ? E8 ? ? ? ? 0F B7 F0", 0 );
MAKE_SIGNATURE( CStaticProp_DrawModel, "engine.dll", "55 8B EC 83 EC 58 53 56 8B F1 33 D2 8B 0D ? ? ? ? 33 DB 89 5D EC 89 55 F0 57 8B 41 08 89 55 FC 85 C0 74 3E", 0 );
MAKE_SIGNATURE( DoEnginePostProcessing, "client.dll", "55 8B EC 81 EC ? ? ? ? 8B 0D ? ? ? ? 53 33 DB 89 5D 8C 89 5D 90 8B 01 89 5D A4 89 5D A8 56 57 85 C0 74 3C", 0 );
MAKE_SIGNATURE( C_TFPlayer_TurnOnTauntCam, "client.dll", "56 8B F1 E8 ? ? ? ? 84 C0 0F 84 ? ? ? ? A1 ? ? ? ? 85 C0 74 0D 80 B8 ? ? ? ? ? 0F 85 ? ? ? ? F3 0F 10 86", 0 );
MAKE_SIGNATURE( CAttributeManager_AttribHookValue_float, "client.dll", "55 8B EC 83 EC 0C 8B 0D ? ? ? ? 53 56 57 33 F6 33 FF 89 75 F4 89 7D F8 8B 41 08 85 C0 74 38", 0 );
MAKE_SIGNATURE( RenderBox, "engine.dll", "55 8B EC 81 EC ? ? ? ? 56 E8 ? ? ? ? 8B 0D ? ? ? ? 8B 01 FF 90 ? ? ? ? 8B F0 89 75 ? 85 F6 74 ? 8B 06 8B CE FF 50 ? 8B 06", 0 );
MAKE_SIGNATURE( ViewRender_DrawViewModels, "client.dll", "55 8B EC 81 EC ? ? ? ? 8B 15 ? ? ? ? 53 8B D9 33 C9 89 4D C4 89 4D C8 8B 02 89 5D D4 89 4D CC 89 4D D0 56", 0 );
MAKE_SIGNATURE( CTFPartyClient_CheckResetSentOptions, "client.dll", "55 8B EC 83 EC 3C A1 ? ? ? ? 53 56 57 83 78 30 00 8B D9 C7 45 ? ? ? ? ? 74 0E 68 ? ? ? ? FF 15", 0 );
MAKE_SIGNATURE( CTFGCClientSystem_GetParty, "client.dll", "8B 89 ? ? ? ? 85 C9 74 1D 68 ? ? ? ? E8 ? ? ? ? 85 C0 74 0F 8B 48 18 85 C9 74 08 8B 40 04 8B 44 88 FC C3", 0 );
MAKE_SIGNATURE( C_TFWeaponBase_CalcIsAttackCritical, "client.dll", "53 57 6A ? 68 ? ? ? ? 68 ? ? ? ? 6A ? 8B F9 E8 ? ? ? ? 50 E8 ? ? ? ? 8B D8 83 C4 ? 85 DB 0F 84", 0 );
MAKE_SIGNATURE( CTFGCClientSystem_PingThink, "client.dll", "55 8B EC 81 EC ? ? ? ? 53 56 8B F1 89 75 CC FF 15 ? ? ? ? 8B D8 85 DB 0F 84 ? ? ? ? 80 BE ? ? ? ? ? 75 3D", 0 );
MAKE_SIGNATURE( RenderWireframeBox, "engine.dll", "55 8B EC 81 EC ? ? ? ? 56 E8 ? ? ? ? 8B 0D ? ? ? ? 8B 01 FF 90 ? ? ? ? 8B F0 89 75 ? 85 F6 74 ? 8B 06 8B CE FF 50 ? A1", 0 );
MAKE_SIGNATURE( CTFGCClientSystem_GetMatchInvite, "client.dll", "55 8B EC 83 EC 10 56 57 8B F9 8B 8F ? ? ? ? 85 C9 74 13 68 ? ? ? ? E8 ? ? ? ? 85 C0 74 07 8B 48 18 EB 04", 0 );
MAKE_SIGNATURE( CTFGCClientSystem_FireGameEvent, "client.dll", "55 8B EC 53 8B 5D 08 56 8B F1 8B CB 57 8B 03 FF 50 04 8B F8 68 ? ? ? ? 57 E8 ? ? ? ? 83 C4 08 85 C0 75 65", 0 );
MAKE_SIGNATURE( CTFGCClientSystem_Init, "client.dll", "56 57 8B F9 8D B7 ? ? ? ? C6 46 04 01 8B 0D ? ? ? ? 85 C9 74 0D 8B 01 6A 00 68 ? ? ? ? 56 FF 50 0C C6 46 04 01 8B 0D", 0 );
MAKE_SIGNATURE( UTIL_TraceHull, "client.dll", "53 8B DC 83 EC 08 83 E4 F0 83 C4 04 55 8B 6B 04 89 6C 24 04 8B EC 83 EC 6C 8D 4D A0 56 FF 73 14 FF 73 10 FF 73 0C FF 73 08 E8", 0 );
MAKE_SIGNATURE( CTFPartyClient_BCanQueueForStandby, "client.dll", "55 8B EC 83 EC 14 56 6A 00 8B F1 E8 ? ? ? ? 84 C0 0F 85 ? ? ? ? 38 46 40 0F 85 ? ? ? ? 8B 4E 18 85 C9 0F 84", 0 );
MAKE_SIGNATURE( CTFPartyClient_BMakeUpdateMsg, "client.dll", "55 8B EC 83 EC 60 53 56 8B F1 33 C9 8B 86 ? ? ? ? 8D 96 ? ? ? ? 3B 86 ? ? ? ? 75 0B 8B 42 14 3B 86 ? ? ? ? 74 0F", 0 );
MAKE_SIGNATURE( CTFGCClientSystem_BConnectedToMatchServer, "client.dll", "55 8B EC 53 8A 5D 08 56 8B F1 84 DB 74 20 8D 8E ? ? ? ? E8 ? ? ? ? 84 C0 74 09 80 BE ? ? ? ? ? 74 08", 0 );
MAKE_SIGNATURE( StaticPropMgr_DrawStaticPropsSlow, "engine.dll", "55 8B EC 83 EC 6C 57 8B 3D ? ? ? ? 8B CF 89 7D E8 8B 07 FF 50 64 80 7D 10 00 B9 ? ? ? ? B8 ? ? ? ? 0F 44 C1 8B C8", 0 );
MAKE_SIGNATURE( CTFGCClientSystem_PostInitGC, "client.dll", "55 8B EC 83 EC 08 56 68 ? ? ? ? 8B F1 FF 15 ? ? ? ? A1 ? ? ? ? 83 C4 04 85 C0 74 4E 83 78 04 00 74 48 68 ? ? ? ? FF 15", 0 );
MAKE_SIGNATURE( CTFPartyClient_LoadSavedCasualCriteria, "client.dll", "83 79 18 00 C6 81 ? ? ? ? ? 74 06 80 79 28 00 74 07 C6 81 ? ? ? ? ? FF 35 ? ? ? ? 81 C1 ? ? ? ? E8 ? ? ? ? C3", 0 );
MAKE_SIGNATURE( CTFGCClientSystem_DumpPing, "client.dll", "55 8B EC 83 EC 20 57 8B F9 C7 45 ? ? ? ? ? 8B 87 ? ? ? ? 85 C0 75 23 0F B6 87 ? ? ? ? 50 0F B6 87 ? ? ? ? 50 68 ? ? ? ? FF 15", 0 );
MAKE_SIGNATURE( CTFPartyClient_RequestQueueForMatch, "client.dll", "55 8B EC 83 EC 38 56 8B 75 08 57 56 8B F9 E8 ? ? ? ? 83 C4 04 84 C0 74 14 83 FE FF 74 0F 8A 84 3E ? ? ? ? 84 C0 0F 85", 0 );
MAKE_SIGNATURE( C_TFweaponBase_CalcIsAttackCriticalHelper, "client.dll", "55 8B EC 83 EC 18 56 57 6A 00 68 ? ? ? ? 68 ? ? ? ? 6A 00 8B F9 E8 ? ? ? ? 50 E8 ? ? ? ? 8B F0 83 C4 14 89 75 EC", 0 );
MAKE_SIGNATURE( C_EconItemView_GetStaticData, "client.dll", "0F B7 41 ? 50 E8 ? ? ? ? 8B C8 E8 ? ? ? ? 6A ? 68 ? ? ? ? 68 ? ? ? ? 6A ? 50 E8 ? ? ? ? 83 C4 ? C3 CC CC CC CC CC CC CC 83 C1", 0 );
MAKE_SIGNATURE( SERVER_CTFWeaponBaseGun_FirePipeBomb, "server.dll", "53 8B DC 83 EC 08 83 E4 F0 83 C4 04 55 8B 6B 04 89 6C 24 04 8B EC 81 EC ? ? ? ? 56 8B F1 57 8B 06 FF 90 ? ? ? ? 8B 7B 08", 0 );
MAKE_SIGNATURE( VSTDLIB_RandomFloat, "vstdlib.dll", "55 8B EC 51 E8 ? ? ? ? 89 45 FC DB 45 FC DC 0D ? ? ? ? DD 05 ? ? ? ? D9 C9 DB F1 DD D9 76 ? DD D8 C7 45 FC FE FF 7F 3F D9 45 FC D9 45 0C", 0 );
MAKE_SIGNATURE( SERVER_ATTRIB_HOOK, "server.dll", "55 8B EC 83 EC 10 8B 0D ? ? ? ? 53 56 57 33 FF 33 DB 89 7D F0 89 5D F4 8B 41 08 85 C0 74 ? 68 ? ? ? ? 68 ? ? ? ? 68 ? ? ? ? 68 ? ? ? ? 6A 6B", 0 );
MAKE_SIGNATURE( C_TFWeaponBase_GetWeaponInfo, "client.dll", "55 8B EC FF 75 08 E8 ? ? ? ? 83 C4 04 85 C0 75 02 5D C3 56 50 E8 ? ? ? ? 83 C4 04 0F B7 F0 E8 ? ? ? ? 66 3B F0 75 05 33 C0 5E 5D C3", 0 );
MAKE_SIGNATURE( CTFGCClientSystem_BIsMatchGroupDisabled, "client.dll", "55 8B EC 51 56 8B B1 ? ? ? ? 33 D2 C7 45 ? ? ? ? ? 85 F6 7E 15 8B 81 ? ? ? ? 8B 4D 08 39 08 74 11 42 83 C0 04 3B D6 7C F4", 0 );
MAKE_SIGNATURE( SERVER_CBaseAnimating_DrawServerHitboxes, "server.dll", "55 8B EC 83 EC ? 57 8B F9 80 BF ? ? ? ? ? 0F 85 ? ? ? ? 83 BF ? ? ? ? ? 75 ? E8 ? ? ? ? 85 C0 74 ? 8B CF E8 ? ? ? ? 8B 97", 0 );
MAKE_SIGNATURE( CTFGCClientSystem_ConnectToServer, "client.dll", "55 8B EC 51 57 FF 75 08 8B F9 68 ? ? ? ? FF 15 ? ? ? ? FF 75 08 8D 45 FC C7 45 ? ? ? ? ? 68 ? ? ? ? 50 E8 ? ? ? ? 83 C4 14 83 3D ?", 0 );
MAKE_SIGNATURE( C_BaseViewModel_ShouldFlipViewModel, "client.dll", "8B 89 ? ? ? ? 85 C9 74 ? BA ? ? ? ? 83 F9 ? 74 ? 0F B7 D1 A1 ? ? ? ? C1 E2 ? 83 C0 ? 03 C2 74 ? C1 E9 ? 39 48 ? 75 ? 8B 08 85 C9 74 ? A1", 0 );
MAKE_SIGNATURE( CTFPartyClient_UpdateActiveParty, "client.dll", "55 8B EC 81 EC ? ? ? ? 8B 15 ? ? ? ? 53 56 57 33 FF 8B F1 89 BD ? ? ? ? 89 BD ? ? ? ? 8B 02 89 75 FC 89 BD ? ? ? ? 89 BD ? ? ? ? 85 C0 74 4B", 0 );
MAKE_SIGNATURE( C_TFPlayer_UpdateWearables, "client.dll", "56 8B F1 E8 ? ? ? ? 8B 06 8B CE 6A 00 68 ? ? ? ? 68 ? ? ? ? 6A 00 FF 90 ? ? ? ? 50 E8 ? ? ? ? 83 C4 14 5E 85 C0 74 0A 8B 10 8B C8 FF A2 6C 01 00 00 C3", 0 );
MAKE_SIGNATURE( CEconItemSchema_GetItemDefinition, "client.dll", "55 8B EC 56 8B F1 8D 45 08 57 50 8D 8E ? ? ? ? E8 ? ? ? ? 85 C0 78 22 3B 86 ? ? ? ? 7D 1A 8B 8E ? ? ? ? 8D 14 40 83 7C 91 ? ? 7C 0A 8B 44 91 04 5F 5E 5D C2 04 00", 0 );
MAKE_SIGNATURE( CTFGCClientSystem_BGetLocalPlayerBadgeInfoForTour, "client.dll", "55 8B EC 83 EC 08 8B 45 0C 56 C7 00 ? ? ? ? 8B 45 10 C7 00 ? ? ? ? E8 ? ? ? ? 8B C8 8B 10 FF 52 58 8B F0 89 75 F8 85 F6 75 07 32 C0 5E 8B E5 5D C3", 0 );
MAKE_SIGNATURE( C_TFPlayer_TauntCamInterpolation, "client.dll", "55 8B EC 81 EC ? ? ? ? 56 57 8B F1 E8 ? ? ? ? 8B F8 85 FF 0F 84 ? ? ? ? 8B 17 8B CF 8B 92 ? ? ? ? FF D2 84 C0 0F 84 ? ? ? ? 80 BE ? ? ? ? ? 0F 84 ? ? ? ? F3 0F 10 8E", 0 );
MAKE_SIGNATURE( GetClientInterpAmount, "client.dll", "55 8B EC A1 ? ? ? ? 83 EC 08 56 A8 01 75 22 8B 0D ? ? ? ? 83 C8 01 A3 ? ? ? ? 68 ? ? ? ? 8B 01 FF 50 34 8B F0 89 35 ? ? ? ? EB 06 8B 35 ? ? ? ? 85 F6 74 68 8B 06 8B CE 8B 40 3C FF D0 8B 0D", 0 );
MAKE_SIGNATURE( C_TFWeaponBase_GetProjectileFireSetup, "client.dll", "53 8B DC 83 EC 08 83 E4 F0 83 C4 04 55 8B 6B 04 89 6C 24 04 8B EC 81 EC ? ? ? ? 56 8B F1 57 8B 06 8B 80 ? ? ? ? FF D0 84 C0 74 12 F3 0F 10 43 ? F3 0F 59 05 ? ? ? ? F3 0F 11 43", 0 );
MAKE_SIGNATURE( CTFGCClientSystem_BHasCompetitiveAccess, "client.dll", "E8 ? ? ? ? 8B C8 8B 10 FF 52 58 85 C0 75 03 32 C0 C3 8B 48 68 85 C9 74 F6 6A 07 E8 ? ? ? ? 85 C0 74 25 83 78 18 01 75 1F 8B 40 04 8B 08 85 C9 74 16 8B 41 0C C1 E8 0A A8 01 74 0C 80 79 48 00 74 06 B8", 0 );
MAKE_SIGNATURE( ViewRender_PerformScreenSpaceEffects, "client.dll", "55 8B EC 83 EC ? 8B 0D ? ? ? ? 53 56 57 33 F6 33 FF 89 75 ? 89 7D ? 8B 01 85 C0 74 ? 68 ? ? ? ? 68 ? ? ? ? 68 ? ? ? ? 68 ? ? ? ? 68 ? ? ? ? 57 57 57 57 8D 4D ? 51 50 8B 40 ? FF D0 8B 7D ? 83 C4 ? 8B 75 ? 8B 0D ? ? ? ? 8B 19 8B 0D", 0 );
MAKE_SIGNATURE( CTraceFilterSimple_ShouldHitEntity, "client.dll", "55 8B EC 53 8B 5D ? 56 8B 75 ? 57 53 56 8B F9 E8 ? ? ? ? 83 C4", 0 );