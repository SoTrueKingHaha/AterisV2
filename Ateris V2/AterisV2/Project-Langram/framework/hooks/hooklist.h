#pragma once
#include "../sdk/interfaces/interfaces.h"
#include "../sdk/globals/globalinfo.h"
#include "../utils/draw/draw.h"
#include "../utils/entitycache/entitycache.h"
#include "../utils/convars/convars.h"
#include "../utils/console/console.h"
#include "../features/engine/prediction.h"
#include "../features/menu/Menu.h"
#include "../features/menu/Input.h"
#include "../features/aimbot/aimbot.h"
#include "../features/esp/esp.h"
#include "../features/misc/misc.h"
#include "hooks.h"

class CEconNotification
{
public:
	virtual ~CEconNotification( ) = 0;
	virtual void SetLifetime( float flSeconds ) = 0;
	virtual float GetExpireTime( ) const = 0;
	virtual float GetInGameLifeTime( ) const = 0;
	virtual bool BShowInGameElements( ) const = 0;
	virtual void MarkForDeletion( ) = 0;

	enum EType
	{
		// Can only be deleted
		eType_Basic,
		// Can be accept or declined
		eType_AcceptDecline,
		// Can be triggered or deleted
		eType_Trigger,
		// Can only be triggered
		eType_MustTrigger,
	};

	virtual EType NotificationType( ) = 0;
	virtual bool BHighPriority( ) = 0;
	virtual void Trigger( ) = 0;
	virtual void Accept( ) = 0;
	virtual void Decline( ) = 0;
	virtual void Deleted( ) = 0;
	virtual void Expired( ) = 0;
	virtual void UpdateTick( ) = 0;
	virtual const char *GetUnlocalizedHelpText( ) = 0;
	virtual void *CreateUIElement( bool bMainMenu ) const = 0;

protected:
	const char *m_pText;
	const char *m_pSoundFilename;
	float m_flExpireTime;
	KeyValues *m_pKeyValues;
	wchar_t m_wszBuffer[ 1024 ];
	CSteamID m_steamID;

private:
	friend class CEconNotificationQueue;
	int m_iID;
	bool m_bInUse;
};

using LookupAttachment_t = int( * )( C_BaseEntity*, const char* );

inline HWND gamehwnd{};
inline WNDPROC ogwndproc{};

namespace hooks
{
	namespace WndProc
	{
		void init( );
		inline hook func( "WndProc", init );
		LRESULT CALLBACK detour( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	}
}

namespace hooks
{
	namespace LockCursor
	{
		void lockcursor_fn( );
	}
}

inline void update_attack( ) {
	if ( !global_info.attacking ) { return; }

	if ( const auto& pLocal = entity_cache.get_local( ) ) {
		if ( const auto& pWeapon = entity_cache.get_weapon( ) )
		{
			const float flFireDelay = pWeapon->GetWeaponData( ).m_flTimeFireDelay;
			pWeapon->m_flNextPrimaryAttack( ) = static_cast< float >( pLocal->m_nTickBase( ) ) * i::global_vars->interval_per_tick + flFireDelay;
		}
	}
}