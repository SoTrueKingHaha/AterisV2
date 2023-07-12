#pragma once
#include "ienginetrace.h"

class  OverlayText_t
{
public:
	OverlayText_t()
	{
		nextOverlayText = 0;
		origin.init();
		bUseOrigin = false;
		lineOffset = 0;
		flXPos = 0;
		flYPos = 0;
		text[0] = 0;
		m_flEndTime = 0.0f;
		m_nServerCount = -1;
		m_nCreationTick = -1;
		r = g = b = a = 255;
	}

	bool			IsDead();
	void			SetEndTime(float duration);

	vector			origin;
	bool			bUseOrigin;
	int				lineOffset;
	float			flXPos;
	float			flYPos;
	char			text[512];
	float			m_flEndTime;			// When does this text go away
	int				m_nCreationTick;		// If > 0, show only one server frame
	int				m_nServerCount;			// compare server spawn count to remove stale overlays
	int				r;
	int				g;
	int				b;
	int				a;
	OverlayText_t* nextOverlayText;
};

/*
CIVDebugOverlay__AddEntityTextOverlay
CIVDebugOverlay__AddBoxOverlay
CIVDebugOverlay__AddTriangleOverlay
CIVDebugOverlay__AddLineOverlay
CIVDebugOverlay__AddTextOverlay
CIVDebugOverlay__AddTextOverlay_
CIVDebugOverlay__AddScreenTextOverlay
*/


class IVDebugOverlay
{
public:
	virtual void AddEntityTextOverlay(int ent_index, int line_offset, float duration, int r, int g, int b, int a, const char* format, ...) = 0;
	virtual void AddBoxOverlay(const vector& origin, const vector& mins, const vector& max, vector const& orientation, int r, int g, int b, int a, float duration) = 0;
	virtual void AddTriangleOverlay(const vector& p1, const vector& p2, const vector& p3, int r, int g, int b, int a, bool noDepthTest, float duration) = 0;
	virtual void AddLineOverlay(const vector& origin, const vector& dest, int r, int g, int b, bool noDepthTest, float duration) = 0;
	virtual void AddTextOverlay(const vector& origin, float duration, const char* format, ...) = 0;
	virtual void AddTextOverlay(const vector& origin, int line_offset, float duration, const char* format, ...) = 0;
	virtual void AddScreenTextOverlay(float flXPos, float flYPos, int line_offset, float flDuration, int r, int g, int b, int a, char const* text) = 0; //float,float,float,int,int,int,int,char const*
	void AddScreenTextOverlayNonVirtual(float x, float y, int line_offset, float duration, int r, int g, int b, int a, char const* txt);
	virtual void AddSweptBoxOverlay(const vector& start, const vector& end, const vector& mins, const vector& max, const vector& angles, int r, int g, int b, int a, float flDuration) = 0;
	virtual void AddGridOverlay(const vector& origin) = 0;
	virtual int ScreenPosition(const vector& point, vector& screen) = 0;
	virtual int ScreenPosition(float flXPos, float flYPos, vector& screen) = 0;
	virtual OverlayText_t* GetFirst(void) = 0;
	virtual OverlayText_t* GetNext(OverlayText_t* current) = 0;
	virtual void ClearDeadOverlays(void) = 0;
	virtual void ClearAllOverlays() = 0;
	virtual void AddTextOverlayRGB(const vector& origin, int line_offset, float duration, float r, float g, float b, float alpha, const char* format, ...) = 0;
	virtual void AddTextOverlayRGB(const vector& origin, int line_offset, float duration, int r, int g, int b, int a, const char* format, ...) = 0;
	virtual void Unknown( ) = 0;
	virtual void AddLineOverlayAlpha(const vector& origin, const vector& dest, int r, int g, int b, int a, bool noDepthTest, float duration) = 0;
	virtual void AddBoxOverlay2(const vector& origin, const vector& mins, const vector& max, vector const& orientation, const Color& faceColor, const Color& edgeColor, float duration) = 0;
};

#define VDEBUG_OVERLAY_INTERFACE_VERSION	"VDebugOverlay003"