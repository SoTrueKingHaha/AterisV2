#pragma once

#include "../../utils/memory/memory.h"

class CBaseHudChat
{
public:
	void ChatPrintf(int pIndex, const char* fmt, ...)
	{
		reinterpret_cast<void(_cdecl*)(void*, int, int, const char*, ...)>( memory->get_vfunc(this, 19))(this, pIndex, 0, fmt);
	}

	void StartMessageMode(int iMessageModeType)
	{
		reinterpret_cast<void(__thiscall*)(void*, int)>(memory->get_vfunc(this, 20))(this, iMessageModeType);
	}
};

class IClientModeShared
{
private:
	char m_szPad[28];
public:
	CBaseHudChat* m_pChatElement;

	bool IsChatPanelOutOfFocus()
	{
		void* CHudChat = reinterpret_cast<void* (__thiscall*)(void*)>( memory->get_vfunc(this, 19))(this);
		return (CHudChat && !*(float*)((DWORD)CHudChat + 0xFC));
	}
};