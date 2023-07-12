#include "Input.h"

#include "vars.h"

bool __input::in_area(int x, int y, int w, int h)
{
	if (mouse_x >= x && mouse_x <= x + w && mouse_y >= y && mouse_y <= y + h)
	{
		return true;
	}

	return false;
}

void __input::update()
{
	for (int n = 0; n < 256; n++)
	{
		if (GetKeyState(n) & 0x8000)
		{
			if (Keys[n] == PRESSED)
				Keys[n] = HELD;

			else if (Keys[n] != HELD)
				Keys[n] = PRESSED;
		}

		else Keys[n] = NONE;
	}

	i::surface->SurfaceGetCursorPos(mouse_x, mouse_y);
}

bool __input::is_pressed(short key)
{
	if (prevent_inputs)
	{
		return false;

	}

	return (Keys[key] == PRESSED);
}

bool __input::is_held(short key)
{
	return (Keys[key] == HELD);
}

#include <chrono>

bool __input::is_pressed_and_held(short key)
{
	EKeyState key_state = Keys[key];

	static std::chrono::time_point<std::chrono::steady_clock> start[256] = { std::chrono::steady_clock::now() };

	if (key_state == PRESSED)
	{
		start[key] = std::chrono::steady_clock::now();
		return true;
	}

	if (key_state == HELD && std::chrono::duration_cast<std::chrono::milliseconds>(
		std::chrono::steady_clock::now() - start[key]).count() > 400)
		return true;

	return false;
}

void __input::drag(int& x, int& y, int w, int h, int offsety)
{
	static POINT delta;
	static bool drag = false;
	static bool move = false;
	bool held = is_held(VK_LBUTTON);

	auto mousex = mouse_x;
	auto mousey = mouse_y;

	if ((mousex > x && mousex < x + w && mousey > y - offsety && mousey < y - offsety + h) && held)
	{
		drag = true;

		if (!move)
		{
			delta.x = mousex - x;
			delta.y = mousey - y;
			move = true;
		}
	}

	if (drag)
	{
		x = mousex - delta.x;
		y = mousey - delta.y;
	}

	if (!held)
	{
		drag = false;
		move = false;
	}
}

bool __input::area_pressed(int x, int y, int w, int h)
{
	static POINT delta;
	static bool heldInArea = false;
	bool held = is_held(VK_LBUTTON);

	auto mousex = mouse_x;
	auto mousey = mouse_y;

	if ((mousex > x && mousex < x + w && mousey > y && mousey < y + h) && held)
	{
		heldInArea = true;
	}

	if (!held)
	{
		heldInArea = false;
	}

	return heldInArea;
}

void __input::null_key(short key)
{
	Keys[key] = EKeyState::NONE;
}

bool __input::in_focus()
{
	static HWND hwGame = 0;

	while (!hwGame)
	{
		hwGame = FindWindowA(0, "Team Fortress 2");
		return false;
	}

	return (GetForegroundWindow() == hwGame);
}