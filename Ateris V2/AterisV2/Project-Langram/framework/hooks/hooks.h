#pragma once
#include "MinHook/include/MinHook.h"
#include <vector>
#include <string>
#include "../sdk/game/proxyfnhook.h"
#include "../sdk/interfaces/interfaces.h"

struct hook
{
public:
	void *original_fn = nullptr;
	void *init_fn = nullptr;
	const char *name_fn = nullptr;

	void create_hook( void *source, void *dest );
	template <typename FN>
	inline FN original( )
	{
		return reinterpret_cast< FN >( original_fn );
	}
	hook( const char *name, void *initFunction );
};

class __hook_manager
{
public:
	std::vector<hook *> hooks;
	bool init( );
	bool release( );
	bool hook_netvar( std::vector<std::string> path, ProxyFnHook& hook, RecvVarProxyFn function );
	void hk_netvar_init( );
};
inline __hook_manager hook_manager;

#define MAKE_HOOK(name, address, type, callconvo, ...) namespace hooks \
{\
	namespace name\
	{\
		void init(); \
		inline hook func(#name, init); \
		using FN = type(callconvo *)(__VA_ARGS__); \
		type callconvo detour(__VA_ARGS__); \
	}\
} \
void hooks::name::init() { func.create_hook(reinterpret_cast<void *>(address), reinterpret_cast<void *>(detour)); } \
type callconvo hooks::name::detour(__VA_ARGS__)

#define CALL_ORIGINAL func.original<FN>()