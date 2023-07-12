#pragma once

#include "vars.h"

class __cursor_backup
{
public:
	void *ECX = nullptr;
	void *EDX = nullptr;
};
inline __cursor_backup cursor_backup;

class __menu
{
public:
	bool open = false;
	bool typing = false;

	void reset( );
	void render( );
};
inline __menu menu;