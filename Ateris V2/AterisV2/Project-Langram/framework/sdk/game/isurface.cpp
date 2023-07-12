#include "isurface.h"

#include "../../utils/signatures/signatures.h"

void ISurface::FinishDrawing()
{
	static auto FN = reinterpret_cast<void(__thiscall*)(void*)>(S::ISurface_FinishDrawing.address);
	FN(this);
}

void ISurface::StartDrawing()
{
	static auto FN = reinterpret_cast<void(__thiscall*)(void*)>(S::ISurface_StartDrawing.address);
	FN(this);
}