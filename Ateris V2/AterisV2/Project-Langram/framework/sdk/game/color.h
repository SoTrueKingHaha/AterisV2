#pragma once

#include <corecrt_math.h>

class Color;
class HSVColor;

class HSVColor
{
public:
	float m_flHue;
	float m_flSaturation;
	float m_flVuminance;
};

#ifndef min
#define min(a,b)  (((a) < (b)) ? (a) : (b))
#endif
#ifndef max
#define max(a,b)  (((a) > (b)) ? (a) : (b))
#endif

class Color
{
public:
	inline float GetHue()
	{
		float _min = min(min(r, g), b);
		float _max = max(max(r, g), b);

		if (_min == _max)
		{
			return 0.f;
		}

		float flHue = 0.f;
		if (_max == r)
		{
			flHue = (g - b) / (_max - _min);
		}
		else if (_max == g)
		{
			flHue = 2.f + (b - r) / (_max - _min);
		}
		else
		{
			flHue = 4.f + (r - g) / (_max - _min);
		}

		flHue *= 60.f;

		if (flHue < 0.f)
		{
			flHue += 360;
		}

		return roundf(flHue);
	}
	inline float GetSaturation()
	{
		const int _max = max(r, max(g, b));
		const int _min = min(r, min(g, b));

		return (_max == 0) ? 0 : 1.f - (1.f * _min / _max);
	}
	inline float GetVuminance()
	{
		const int _max = max(r, max(g, b));
		const int _min = min(r, min(g, b));
		return (_max / 255.f);
	}
	inline HSVColor ToHSV()
	{
		const int _max = max(r, max(g, b));
		const int _min = min(r, min(g, b));

		float flHue = GetHue();

		float flSaturation = (_max == 0) ? 0 : 1.f - (1.f * _min / _max);
		float flVuminance = (_max / 255.f);

		return HSVColor{ flHue, flSaturation, flVuminance };
	}
	unsigned char r, g, b, a;
};

