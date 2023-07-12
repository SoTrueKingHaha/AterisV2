#pragma once
#undef snprintf
#include <vector>
#include <typeinfo>
#include <fstream>
#include <iomanip>
#include "../../../utils/draw/color.h"
#include "../../../utils/json/json.hpp"

namespace C
{
	struct VariableInitializer
	{
		const char* m_szName = nullptr;
		void* m_pValue = nullptr;
		size_t m_nTypeHash = 0;
	};

	inline std::vector<VariableInitializer> Vars{};
	inline std::string m_sConfigPath{ };

	void Save( const char* name );
	void Load( const char* name );
}

#define CFGVAR( path, var )\
bool var##_initializer = []()\
{\
	C::Vars.push_back(C::VariableInitializer{ #var, &path.var, typeid(path.var).hash_code() });\
	return true;\
}();


#define CFGVAR_NOSAVE(var, val) namespace V { inline auto var { val }; }