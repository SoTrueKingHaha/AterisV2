#pragma once
#include "c_baseanimating.h"

class C_CaptureFlag : public C_BaseAnimating
{
public:
	NETVAR( m_bDisabled, bool, "CCaptureFlag", "m_bDisabled" );
	NETVAR( m_bVisibleWhenDisabled, bool, "CCaptureFlag", "m_bVisibleWhenDisabled" );
	NETVAR( m_nType, int, "CCaptureFlag", "m_nType" );
	NETVAR( m_nFlagStatus, int, "CCaptureFlag", "m_nFlagStatus" );
	NETVAR( m_flResetTime, float, "CCaptureFlag", "m_flResetTime" );
	NETVAR( m_flMaxResetTime, float, "CCaptureFlag", "m_flMaxResetTime" );
	NETVAR( m_flNeutralTime, float, "CCaptureFlag", "m_flNeutralTime" );
	NETVAR( m_hPrevOwner, int, "CCaptureFlag", "m_hPrevOwner" );
	NETVAR( m_nPointValue, int, "CCaptureFlag", "m_nPointValue" );
	NETVAR( m_flAutoCapTime, float, "CCaptureFlag", "m_flAutoCapTime" );

	inline const wchar_t* GetIntelStatus( int nStatus )
	{
		switch ( nStatus )
		{
			case 0:
			{
				return L"AT BASE";
				break;
			}
			case 1:
			{
				return L"PICKED UP";
				break;
			}
			case 2:
			{
				return L"DROPPED";
				break;
			}
			default: return L"NULL"; break;
		}
		return L"NULL";
	}
};