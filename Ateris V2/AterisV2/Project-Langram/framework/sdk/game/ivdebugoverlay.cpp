#include "ivdebugoverlay.h"

#include "../../utils/signatures/signatures.h"

MAKE_SIGNATURE(AddScreenTextOverlay, "engine.dll", "55 8B EC 83 EC ? B9 ? ? ? ? E8 ? ? ? ? 84 C0 0F 85 ? ? ? ? 56 6A ? 68 ? ? ? ? 68 ? ? ? ? 68 ? ? ? ? 8D 45 ? 68 ? ? ? ? 50 E8 ? ? ? ? 68 ? ? ? ? E8 ? ? ? ? 83 C4 ? 85 C0 74 ? 8B C8 E8 ? ? ? ? 8B F0 EB ? 33 F6 68", 0);


void IVDebugOverlay::AddScreenTextOverlayNonVirtual(float x, float y, int line_offset, float duration, int r, int g, int b, int a, char const* txt)
{
	static auto FN = reinterpret_cast<void(__cdecl*)(float, float, int, float, int, int, int, int, char const*)>(S::AddScreenTextOverlay.address);
	FN(x, y, line_offset, duration, r, g, b, a, txt);
}