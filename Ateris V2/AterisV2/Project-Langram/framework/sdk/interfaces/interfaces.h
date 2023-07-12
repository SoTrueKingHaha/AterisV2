#pragma once
#include "../../utils/memory/memory.h"

#include "../interface_list.h"
#include "../game/bitbuf.h"
#include "../game/checksum_crc.h"
#include "../game/md5.h"
#include "../game/beams.h"
#include "../game/panel.h"

struct game_interface_t
{
public:
	void **ptr = nullptr;
	const char *dll = nullptr;
	const char *sig = nullptr;
	const char *name = nullptr;
	int offset = -1;
	int deref = -1;

	game_interface_t( const char *name, void **pointer, const char *dll, const char *sig, int offset, int dereferences );
};

class __interface_manager
{
public:
	std::vector<game_interface_t *> interfaces;
	bool init( );
};
inline __interface_manager interface_manager;

#define MAKE_STEAM_INTERFACE(type, name) \
namespace i \
{ \
	inline type* name = nullptr; \
}

#define MAKE_INTERFACE(type, name, dll, location, offset, deref)\
namespace i \
{ \
	inline type* name = nullptr; \
} \
namespace inits \
{ \
	inline game_interface_t name##_init(#name, (void**)&i::name, dll, location, offset, deref); \
} 

MAKE_STEAM_INTERFACE( ISteamApps006, SteamApps );
MAKE_STEAM_INTERFACE( ISteamUser017, SteamUser );
MAKE_STEAM_INTERFACE( ISteamUtils007, SteamUtils );
MAKE_STEAM_INTERFACE( ISteamFriends002, SteamFriends02 );
MAKE_STEAM_INTERFACE( ISteamFriends015, SteamFriends15 );
MAKE_STEAM_INTERFACE( ISteamUserStats011, SteamUserStats );
MAKE_STEAM_INTERFACE( ISteamNetworkingUtils, SteamNetworkingUtils );
MAKE_INTERFACE( ICvar, cvar, "vstdlib.dll", "VEngineCvar004", -1, 1 );
MAKE_INTERFACE( IPhysics, physics, "vphysics.dll", "VPhysics031", -1, 1 );
MAKE_INTERFACE( CPanel, vgui_panel, "vgui2.dll", "VGUI_Panel009", -1, -1 );
MAKE_INTERFACE( IBaseClientDLL, client, "client.dll", "VClient017", -1, 1 );
MAKE_INTERFACE( IEngineVGui, engine_vgui, "engine.dll", "VEngineVGui002", -1, 1 );
MAKE_INTERFACE( IShaderAPI, shader_dx9, "shaderapidx9.dll", "ShaderApi030", -1, 1 );
MAKE_INTERFACE( IVModelRender, model_render, "engine.dll", "VEngineModel016", -1, 1 );
MAKE_INTERFACE( IVModelInfo, model_info, "engine.dll", "VModelInfoClient006", -1, 1 );
MAKE_INTERFACE( CGameMovement, game_movement, "client.dll", "GameMovement001", -1, 1 );
MAKE_INTERFACE( CPrediction, prediction, "client.dll", "VClientPrediction001", -1, 1 );
MAKE_INTERFACE( CEngineClient, engine_client, "engine.dll", "VEngineClient013", -1, 1 );
MAKE_INTERFACE( IVRenderView, render_view, "engine.dll", "VEngineRenderView014", -1, 1 );
MAKE_INTERFACE( IVDebugOverlay, debug_overlay, "engine.dll", "VDebugOverlay003", -1, 1 );
MAKE_INTERFACE( IEngineTrace, engine_trace, "engine.dll", "EngineTraceClient003", -1, 1 );
MAKE_INTERFACE( CGlobalVarsBase, global_vars, "engine.dll", "A1 ? ? ? ? 8B 11 68", 8, 1 );
MAKE_INTERFACE( IMemAlloc, mem_alloc, "engine.dll", "8B 0D ? ? ? ? 83 C4 ? 8B 09", 2, 2 );
MAKE_INTERFACE( ISteamClient017, SteamClient, "steamclient.dll", "SteamClient017", -1, 1 );
MAKE_INTERFACE( IMatSystemSurface, surface, "vguimatsurface.dll", "VGUI_Surface030", -1, 1 );
MAKE_INTERFACE( IShaderDevice, shader_device, "shaderapidx9.dll", "ShaderDevice001", -1, 1 );
MAKE_INTERFACE( IShaderShadow, shader_shadow, "shaderapidx9.dll", "ShaderShadow010", -1, 1 );
MAKE_INTERFACE( CGameEventManager, game_event, "engine.dll", "GAMEEVENTSMANAGER002", -1, 1 );
MAKE_INTERFACE( IStudioRender, studio_render, "studiorender.dll", "VStudioRender025", -1, 1 );
MAKE_INTERFACE( IClientEntityList, entity_list, "client.dll", "VClientEntityList003", -1, 1 );
MAKE_INTERFACE( IInputSystem, input_system, "inputsystem.dll", "InputSystemVersion001", -1, 1 );
MAKE_INTERFACE( IInput, input, "client.dll", "8B 0D ? ? ? ? 56 8B 01 FF 50 24 8B 45 FC", 2, 2 );
MAKE_INTERFACE( int32_t, random_seed, "client.dll", "C7 05 ? ? ? ? ? ? ? ? 5D C3 8B 40 34", 2, 1 );
MAKE_INTERFACE( IPhysicsCollision, physics_collision, "vphysics.dll", "VPhysicsCollision007", -1, 1 );
MAKE_INTERFACE( IMaterialSystem, material_system, "materialsystem.dll", "VMaterialSystem081", -1, 1 );
MAKE_INTERFACE( IShaderDeviceMgr, shader_device_mgr, "shaderapidx9.dll", "ShaderDeviceMgr001", -1, 1 );
MAKE_INTERFACE( IStaticPropMgrClient, static_prop_mgr, "engine.dll", "StaticPropMgrClient004", -1, 1 );
MAKE_INTERFACE( IMoveHelper, move_helper, "client.dll", "8B 0D ? ? ? ? 8B 01 FF 50 28 56 8B C8", 2, 2 );
MAKE_INTERFACE( CTFGCClientSystem, game_coordinator, "client.dll", "B9 ? ? ? ? 50 E8 ? ? ? ? 8B 5D F8", 1, 1 );
MAKE_INTERFACE( void *, prediction_player, "client.dll", "C7 05 ? ? ? ? ? ? ? ? F3 0F 10 40 ? 0F 2F 05", 2, 1 );
MAKE_INTERFACE( IViewRenderBeams, beams, "client.dll", "8B 0D ? ? ? ? 56 8B 01 FF 50 18 0F B7 96 ? ? ? ?", 2, 2 );
MAKE_INTERFACE( void, inventory_manager, "client.dll", "8B 15 ? ? ? ? B9 ? ? ? ? 89 45 F4 FF 52 58 3B C3", 2, 2 );
MAKE_INTERFACE( IViewRender, view_render, "client.dll", "8B 0D ? ? ? ? 8B 01 FF 50 ? 8B 06 8D 4D ? 51 8B CE", 2, 2 );
MAKE_INTERFACE( CBaseClientState, client_state, "engine.dll", "68 ? ? ? ? E8 ? ? ? ? 83 C4 08 5F 5E 5B 5D C3", 1, 1 );
MAKE_INTERFACE( IClientModeShared, client_mode, "client.dll", "8B 0D ? ? ? ? 83 EC ? 8B 01 FF 75 ? FF 50 ? 68", 2, 2 );
MAKE_INTERFACE( IUniformRandomStream, uniform_random_stream, "vstdlib.dll", "B9 ? ? ? ? 85 C0 0F 45 C8 89 0D ? ? ? ? 5D C3", 1, 1 );