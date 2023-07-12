#include "c_baseanimating.h"

#include "../../utils/signatures/signatures.h"

void C_BaseAnimating::SetSequence(int nSequence)
{
	static auto FN = reinterpret_cast<void(__thiscall*)(void*, int)>( memory->get_vfunc(this, 187));
	FN(this, nSequence);
}

void C_BaseAnimating::UpdateVisibility()
{
	static auto FN = reinterpret_cast<void(__thiscall*)(void*)>(S::C_BaseAnimating_UpdateVisibility.address);
	FN(this);
}