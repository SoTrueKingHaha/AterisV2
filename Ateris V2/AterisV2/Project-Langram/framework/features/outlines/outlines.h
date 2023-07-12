#pragma once
#include "../models/models.h"

class __outlines
{
private:
	ITexture* m_pRtFullFrame;
	ITexture* m_pRenderBuffer1;
	ITexture* m_pRenderBuffer2;
	IMaterial* m_pMatBlurXwf;
	IMaterial* m_pMatBlurX;
	IMaterial* m_pMatBlurYwf;
	IMaterial* m_pMatBlurY;
	IMaterial* m_pMatHaloAddToScreen;

	struct GlowEnt_t
	{
		C_BaseEntity* m_pEntity;
		color m_Color;
		bool viewmodel;
	};

	std::vector<GlowEnt_t> m_vecGlowEntities;
	std::map<C_BaseEntity*, bool> m_DrawnEntities;

private:
	void DrawModel( C_BaseEntity* pEntity, int nFlags, bool bIsDrawingModels );
	void SetScale( int nScale, bool bReset );
	bool ShouldRun( );

public:
	void init( );
	void Outline( C_TFPlayer* pLocal, IMatRenderContext* pRenderContext );
	void Render( );

	bool HasDrawn( C_BaseEntity* pEntity )
	{
		return m_DrawnEntities.find( pEntity ) != m_DrawnEntities.end( );
	}

	bool IsGlowMaterial( IMaterial* pMat )
	{
		return pMat == m_pMatGlowColor;
	}

public:
	IMaterial* m_pMatGlowColor;
	bool m_bDrawingGlow;
	bool m_bRendering;
	bool bDrawViewmodel = false;
	bool bColoringViewmodel = false;
	bool bSetup;
};

inline __outlines outlines;