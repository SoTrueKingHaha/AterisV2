#pragma once

//==================================================================================================
// **this goes into both platforms which run the translator, either the real Mac client or
// the Windows client running with r_emulategl mode **
//
// size of the VS register bank in ARB / GLSL we expose
// it's not 256, because you can't use all 256 slots in 10.5.x.
// use this constant everywhere you might normally use "256" in reference to a parameter array size.
// The highest shader constant is c218, plus we allocate c219 and c220 for two clip planes
#define DXABSTRACT_VS_PARAM_SLOTS   228
#define DXABSTRACT_VS_FIRST_BONE_SLOT VERTEX_SHADER_MODEL
#define DXABSTRACT_VS_LAST_BONE_SLOT (VERTEX_SHADER_SHADER_SPECIFIC_CONST_13-1)

// user clip plane 0 goes in DXABSTRACT_VS_CLIP_PLANE_BASE... plane 1 goes in the slot after that
// dxabstract uses these constants to check plane index limit and to deliver planes to shader for DP4 -> oCLP[n]
#define	DXABSTRACT_VS_CLIP_PLANE_BASE (DXABSTRACT_VS_PARAM_SLOTS-2)

#include "imaterialsystem.h"
#include "ishaderapi.h"
#include "ishadershadow.h"

enum ShaderParamType_t
{
	SHADER_PARAM_TYPE_TEXTURE,
	SHADER_PARAM_TYPE_INTEGER,
	SHADER_PARAM_TYPE_COLOR,
	SHADER_PARAM_TYPE_VEC2,
	SHADER_PARAM_TYPE_VEC3,
	SHADER_PARAM_TYPE_VEC4,
	SHADER_PARAM_TYPE_ENVMAP,	// obsolete
	SHADER_PARAM_TYPE_FLOAT,
	SHADER_PARAM_TYPE_BOOL,
	SHADER_PARAM_TYPE_FOURCC,
	SHADER_PARAM_TYPE_MATRIX,
	SHADER_PARAM_TYPE_MATERIAL,
	SHADER_PARAM_TYPE_STRING,
	SHADER_PARAM_TYPE_MATRIX4X2
};

//-----------------------------------------------------------------------------
// Information about each shader parameter
//-----------------------------------------------------------------------------
struct ShaderParamInfo_t
{
	const char* m_pName;
	const char* m_pHelp;
	ShaderParamType_t m_Type;
	const char* m_pDefaultValue;
	int m_nFlags;
};

#define VERTEX_SHADER_BONE_TRANSFORM( k )	( VERTEX_SHADER_MODEL + 3 * (k) )

class CBasePerMaterialContextData								// shaders can keep per material data in classes descended from this
{
public:
	unsigned int m_nVarChangeID;
	bool m_bMaterialVarsChanged;							// set by mat system when material vars change. shader should rehtink and then clear the var

	FORCEINLINE CBasePerMaterialContextData(void)
	{
		m_bMaterialVarsChanged = true;
		m_nVarChangeID = 0xffffffff;
	}

	// virtual destructor so that derived classes can have their own data to be cleaned up on
	// delete of material
	virtual ~CBasePerMaterialContextData(void)
	{
	}
};

class IShader
{
public:
	// Returns the shader name
	virtual char const* GetName() const = 0;

	// returns the shader fallbacks
	virtual char const* GetFallbackShader(IMaterialVar** params) const = 0;

	// Shader parameters
	virtual int GetNumParams() const = 0;

	// These functions must be implemented by the shader
	virtual void InitShaderParams(IMaterialVar** ppParams, const char* pMaterialName) = 0;
	virtual void InitShaderInstance(IMaterialVar** ppParams, IShaderInit* pShaderInit, const char* pMaterialName, const char* pTextureGroupName) = 0;
	virtual void DrawElements(IMaterialVar** params, int nModulationFlags,
		IShaderShadow* pShaderShadow, IShaderDynamicAPI* pShaderAPI, VertexCompressionType_t vertexCompression, CBasePerMaterialContextData** pContextDataPtr) = 0;

	virtual char const* GetParamName(int paramIndex) const = 0;
	virtual char const* GetParamHelp(int paramIndex) const = 0;
	virtual ShaderParamType_t GetParamType(int paramIndex) const = 0;
	virtual char const* GetParamDefault(int paramIndex) const = 0;

	// FIXME: Figure out a better way to do this?
	virtual int ComputeModulationFlags(IMaterialVar** params, IShaderDynamicAPI* pShaderAPI) = 0;
	virtual bool NeedsPowerOfTwoFrameBufferTexture(IMaterialVar** params, bool bCheckSpecificToThisFrame = true) const = 0;
	virtual bool NeedsFullFrameBufferTexture(IMaterialVar** params, bool bCheckSpecificToThisFrame) const = 0;
	virtual bool IsTranslucent(IMaterialVar** params) const = 0;

	virtual int GetParamFlags(int paramIndex) const = 0;

	virtual int GetFlags() const = 0;

	// FIXME: Remove GetParamName, etc. above
//	virtual const ShaderParamInfo_t& GetParamInfo( int paramIndex ) const = 0;
};