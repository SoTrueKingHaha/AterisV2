#include "c_baseplayer.h"

int C_BasePlayer::GetMaxHealth()
{
	static auto FN = reinterpret_cast<int(__thiscall*)(void*)>( memory->get_vfunc(this, 107));
	return FN(this);
}

#include "../../utils/signatures/signatures.h"

MAKE_SIGNATURE(C_BasePlayer_BoneCache, "client.dll", "55 8B EC 83 EC ? 56 8B F1 57 FF B6", 0);

CBoneCacheHandler* C_BasePlayer::GetBoneCache()
{
	static auto FN = reinterpret_cast<CBoneCacheHandler * (__thiscall*)(void*, void*)>(S::C_BasePlayer_BoneCache.address);
	return FN(this, NULL);
}

bool C_BasePlayer::IsAlive()
{
	return !deadflag();
}