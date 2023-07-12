#pragma once

#include "C_BaseAnimatingOverlay.h"

struct flexsettinghdr_t
{
	int id;
	int version;
	char name[64];
	int length;
	int numflexsettings;
	int flexsettingindex;
	int nameindex;
	int numindexes;
	int indexindex;
	int numkeys;
	int keynameindex;
	int keymappingindex;
};

class IHasLocalToGlobalFlexSettings
{
public:
	virtual void EnsureTranslations(const flexsettinghdr_t* pSettinghdr) = 0;
};

class C_BaseFlex : public C_BaseAnimatingOverlay, public IHasLocalToGlobalFlexSettings
{
public:
	NETVAR(m_flexWeight, void*, "CBaseFlex", "m_flexWeight");
	NETVAR(m_blinktoggle, int, "CBaseFlex", "m_blinktoggle");
	NETVAR(m_viewtarget, vector, "CBaseFlex", "m_viewtarget");
};