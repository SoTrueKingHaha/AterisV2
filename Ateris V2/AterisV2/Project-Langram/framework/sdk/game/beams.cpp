#include "beams.h"
#include "../../utils/utils.h"
#include "../../sdk/interfaces/interfaces.h"

void IViewRenderBeams::DrawBeam( const vector& source, const vector& end, const color& col )
{
	BeamInfo_t beamInfo;
	beamInfo.m_nType = 0;
	beamInfo.m_pszModelName = "sprites/physbeam.vmt";
	beamInfo.m_nModelIndex = -1; // will be set by CreateBeamPoints if its -1
	beamInfo.m_flHaloScale = 0.0f;
	beamInfo.m_flLife = i::global_vars->interval_per_tick * 2;
	beamInfo.m_flWidth = 2.f;
	beamInfo.m_flEndWidth = 2.f;
	beamInfo.m_flFadeLength = 0.f;
	beamInfo.m_flAmplitude = 0.f;
	beamInfo.m_flBrightness = 255.f;
	beamInfo.m_flSpeed = 0.f;
	beamInfo.m_nStartFrame = 0;
	beamInfo.m_flFrameRate = 0.f;
	beamInfo.m_flRed = col.r;
	beamInfo.m_flGreen = col.g;
	beamInfo.m_flBlue = col.b;
	beamInfo.m_nSegments = 2;
	beamInfo.m_bRenderable = true;
	beamInfo.m_nFlags = FBEAM_NOTILE;
	beamInfo.m_vecStart = source;
	beamInfo.m_vecEnd = end;

	Beam_t* coolBeam = i::beams->CreateBeamPoints( beamInfo );
	if ( coolBeam )
	{
		i::beams->DrawBeam( coolBeam );
	}
}

void IViewRenderBeams::DrawBeam( const vector& source, const vector& end, const color& col, float time, float size )
{
	BeamInfo_t beamInfo;
	beamInfo.m_nType = 0;
	beamInfo.m_pszModelName = "sprites/physbeam.vmt";
	beamInfo.m_nModelIndex = -1; // will be set by CreateBeamPoints if its -1
	beamInfo.m_flHaloScale = 0.0f;
	beamInfo.m_flLife = time;
	beamInfo.m_flWidth = size;
	beamInfo.m_flEndWidth = size;
	beamInfo.m_flFadeLength = 0.f;
	beamInfo.m_flAmplitude = 0.f;
	beamInfo.m_flBrightness = 255.f;
	beamInfo.m_flSpeed = 0.f;
	beamInfo.m_nStartFrame = 0;
	beamInfo.m_flFrameRate = 0.f;
	beamInfo.m_flRed = col.r;
	beamInfo.m_flGreen = col.g;
	beamInfo.m_flBlue = col.b;
	beamInfo.m_nSegments = 2;
	beamInfo.m_bRenderable = true;
	beamInfo.m_nFlags = FBEAM_NOTILE;
	beamInfo.m_vecStart = source;
	beamInfo.m_vecEnd = end;

	Beam_t* coolBeam = i::beams->CreateBeamPoints( beamInfo );
	if ( coolBeam )
	{
		i::beams->DrawBeam( coolBeam );
	}
}