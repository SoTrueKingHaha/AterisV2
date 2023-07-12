#pragma once
#include "typedefs.h"
//#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files
#include <windows.h>
#include <stdlib.h>
#include <process.h>
#include <assert.h>
#include <winuser.h>
#include <Wincon.h>
#include <stdio.h>
#include <stdlib.h>
#include <cstdint>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <numeric>
#include <array>
#include <vector>
#include <string>
#include <ctime>
#include <map>
#include <unordered_map>
#include <vector>
#include <deque>
#include <random>
#include <chrono>
#include <functional>
#include <future>

class __entry_point
{
public:
	void __stdcall attach( );
	void __stdcall detach( bool fail );
};
inline __entry_point *entry;