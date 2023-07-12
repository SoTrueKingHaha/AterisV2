#pragma once
#include "../../sdk/interfaces/interfaces.h"
#include "../../utils/entitycache/entitycache.h"
#include "../../utils/utils.h"
#include "../../utils/convars/convars.h"

struct ChamInfo {
	int $flags = 0;
	int $flags_defined = 0;
	int $flags2 = 0;
	int $flags_defined2 = 0;
	int $frame = 0;
};

class __models
{
private:
	IMaterial* m_pMatFresnelHDR0;
	IMaterial* m_pMatFresnelHDR1;
	IMaterial* m_pMatOverlay;

	IMaterial* m_pMatPlayer;
	IMaterial* m_pMatBuilding;
	IMaterial* m_pMatWorld;
	std::map<IClientEntity*, bool> m_DrawnEntities;

	bool ShouldRun( );

	void DrawModel( IClientEntity* pEntity, bool overlayMat, color color, float flAlpha );
	void Override( C_TFPlayer* pLocal, IMatRenderContext* pRenderContext );

public:
	//IMaterial *overlaymaterial;
	IMaterial* m_pMatFlat;
	IMaterial* m_pMatShaded;
	IMaterial* m_pMatFresnel;
	void init( );
	IMaterial* CreateNRef( char const* szName, KeyValues* pKV, bool bSave = true );
	void Render( );

	bool HasDrawn( IClientEntity* pEntity )
	{
		return m_DrawnEntities.find( pEntity ) != m_DrawnEntities.end( );
	}
	bool IsChamsMaterial( IMaterial* pMat )
	{
		return ( pMat == m_pMatFlat || pMat == m_pMatOverlay );
	}

	bool m_bRendering;
	bool m_bHasSetStencil;
	bool drawing_fake;
	bool bSetup = false;
	int iInitCount, iValidCount, iDelCount;
	std::vector<IMaterial*>matList = { };
	std::vector<IMaterial*>matListGlobal = { };
	std::unordered_map<IMaterial*, ChamInfo> backupInformation;
};

inline __models models;