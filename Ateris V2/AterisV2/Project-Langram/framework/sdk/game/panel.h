#pragma once
#include "../../utils/memory/memory.h"

class CPanel
{
public:
	const char* GetName( unsigned int vgui_panel )
	{
		typedef const char* ( __thiscall* FN )( PVOID, unsigned int );
		return memory->get_vfunc<FN>( this, 36 )( this, vgui_panel );
	}

	void SetMouseInputEnabled( unsigned int panel, bool state )
	{
		typedef void( __thiscall* FN )( void*, int, bool );
		memory->get_vfunc<FN>( this, 32 )( this, panel, state );
	}

	void SetTopmostPopup( unsigned int panel, bool state )
	{
		typedef void( __thiscall* FN )( void*, int, bool );
		memory->get_vfunc<FN>( this, 59 )( this, panel, state );
	}
};