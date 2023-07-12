#pragma once

#include "../memory/memory.h"
#include "../../sdk/game/ibaseclientdll.h"
#include <vector>

class netvar_hook
{
public:
	netvar_hook( const char* classname, const char* netvar, RecvVarProxyFn function );
	void init( );
	void reset( );

	RecvProp* m_Prop;
	RecvVarProxyFn m_Original;
	RecvVarProxyFn m_Function;
	const char* m_ClassName;
	const char* m_Netvar;
};

/// const CRecvProxyData* pData, void* pStruct, void* pOut
#define MAKE_NETVAR_HOOK(datatable, name) \
namespace netvar_hook_##datatable_##name {\
	void fn(const CRecvProxyData* data, void* ECX, void* out);\
	inline netvar_hook hook(#datatable, #name, fn);\
}\
void netvar_hook_##datatable_##name::fn(const CRecvProxyData* data, void* ECX, void* out)\

//#define NETVARS
class __netvars
{
public:
	int get_offset( RecvTable *table, const char *name );
	RecvProp* get_netvar_prop( RecvTable* table, const char* name );
	RecvProp* find_netvar_prop( const char* classname, const char* netvar );
	int find_netvar( const char *classname, const char *netvar );
	void init_hooks( );
	void reset_hooks( );
#ifdef NETVARS
	void dump_table( RecvTable *table, int depth, int classid );
	void dump_tables( );
#endif
	std::vector<netvar_hook*> Hooks;
};
inline __netvars netvars;

#define NETVAR(_name, type, table, name) inline type &_name() \
{ \
	static int offset = netvars.find_netvar(table, name); \
	return *reinterpret_cast<type *>(reinterpret_cast<DWORD>(this) + offset); \
}

#define NETVAR_OFFSET_FROM(_name, type, table, name, addition) inline type &_name() \
{ \
	static int offset = netvars.find_netvar(table, name); \
	return *reinterpret_cast<type *>(reinterpret_cast<DWORD>(this) + offset + addition); \
}

#define NETVAR_OFFSET_PURE(_name, type, offset) inline type &_name() \
{ \
	return *reinterpret_cast<type *>(reinterpret_cast<DWORD>(this) + offset); \
}

#define M_CONDGET(name, conditions, cond) __inline bool Is##name() \
{ \
	return (conditions & cond); \
}