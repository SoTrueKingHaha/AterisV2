#pragma once

#include "ishaderapi.h"

//-----------------------------------------------------------------------------
// Mode chance callback
//-----------------------------------------------------------------------------
typedef void (*ShaderModeChangeCallbackFunc_t)(void);

class IShaderDeviceMgr : public IAppSystem
{
public:
	// Gets the number of adapters...
	virtual int	 GetAdapterCount() const = 0;

	// Returns info about each adapter
	virtual void GetAdapterInfo(int nAdapter, MaterialAdapterInfo_t& info) const = 0;

	// Gets recommended congifuration for a particular adapter at a particular dx level
	virtual bool GetRecommendedConfigurationInfo(int nAdapter, int nDXLevel, KeyValues* pConfiguration) = 0;

	// Returns the number of modes
	virtual int	 GetModeCount(int nAdapter) const = 0;

	// Returns mode information..
	virtual void GetModeInfo(ShaderDisplayMode_t* pInfo, int nAdapter, int nMode) const = 0;

	// Returns the current mode info for the requested adapter
	virtual void GetCurrentModeInfo(ShaderDisplayMode_t* pInfo, int nAdapter) const = 0;

	// Initialization, shutdown
	virtual bool SetAdapter(int nAdapter, int nFlags) = 0;

	// Sets the mode
	// Use the returned factory to get at an IShaderDevice and an IShaderRender
	// and any other interfaces we decide to create.
	// A returned factory of NULL indicates the mode was not set properly.
	virtual CreateInterfaceFn SetMode(void* hWnd, int nAdapter, const ShaderDeviceInfo_t& mode) = 0;

	// Installs a callback to get called 
	virtual void AddModeChangeCallback(ShaderModeChangeCallbackFunc_t func) = 0;
	virtual void RemoveModeChangeCallback(ShaderModeChangeCallbackFunc_t func) = 0;
};


//-----------------------------------------------------------------------------
// A shader buffer returns a block of memory which must be released when done with it
//-----------------------------------------------------------------------------
class IShaderBuffer
{
public:
	virtual size_t GetSize() const = 0;
	virtual const void* GetBits() const = 0;
	virtual void Release() = 0;
};

class CUtlBuffer;

//-----------------------------------------------------------------------------
// For vertex/index buffers. What type is it?
// (NOTE: mirror this with a similarly named enum at the material system level for backwards compatability.)
//-----------------------------------------------------------------------------
enum ShaderBufferType_t
{
	SHADER_BUFFER_TYPE_STATIC = 0,
	SHADER_BUFFER_TYPE_DYNAMIC,
	SHADER_BUFFER_TYPE_STATIC_TEMP,
	SHADER_BUFFER_TYPE_DYNAMIC_TEMP,

	SHADER_BUFFER_TYPE_COUNT,
};

inline bool IsDynamicBufferType(ShaderBufferType_t type)
{
	return ((type == SHADER_BUFFER_TYPE_DYNAMIC) || (type == SHADER_BUFFER_TYPE_DYNAMIC_TEMP));
}

//-----------------------------------------------------------------------------
// Info for non-interactive mode
//-----------------------------------------------------------------------------
struct ShaderNonInteractiveInfo_t
{
	ShaderAPITextureHandle_t m_hTempFullscreenTexture;
	int m_nPacifierCount;
	ShaderAPITextureHandle_t m_pPacifierTextures[64];
	float m_flNormalizedX;
	float m_flNormalizedY;
	float m_flNormalizedSize;
};


//-----------------------------------------------------------------------------
// Methods related to control of the device
//-----------------------------------------------------------------------------
#define SHADER_DEVICE_INTERFACE_VERSION			"ShaderDevice001"
class IShaderDevice
{
public:
	// Releases/reloads resources when other apps want some memory
	virtual void ReleaseResources() = 0;
	virtual void ReacquireResources() = 0;

	// returns the backbuffer format and dimensions
	virtual ImageFormat GetBackBufferFormat() const = 0;
	virtual void GetBackBufferDimensions(int& width, int& height) const = 0;

	// Returns the current adapter in use
	virtual int GetCurrentAdapter() const = 0;

	// Are we using graphics?
	virtual bool IsUsingGraphics() const = 0;

	// Use this to spew information about the 3D layer 
	virtual void SpewDriverInfo() const = 0;

	// What's the bit depth of the stencil buffer?
	virtual int StencilBufferBits() const = 0;

	// Are we using a mode that uses MSAA
	virtual bool IsAAEnabled() const = 0;

	// Does a page flip
	virtual void Present() = 0;

	// Returns the window size
	virtual void GetWindowSize(int& nWidth, int& nHeight) const = 0;

	// Gamma ramp control
	virtual void SetHardwareGammaRamp(float fGamma, float fGammaTVRangeMin, float fGammaTVRangeMax, float fGammaTVExponent, bool bTVEnabled) = 0;

	// Creates/ destroys a child window
	virtual bool AddView(void* hWnd) = 0;
	virtual void RemoveView(void* hWnd) = 0;

	// Activates a view
	virtual void SetView(void* hWnd) = 0;

	// Shader compilation
	virtual IShaderBuffer* CompileShader(const char* pProgram, size_t nBufLen, const char* pShaderVersion) = 0;

	// Shader creation, destruction
	virtual VertexShaderHandle_t CreateVertexShader(IShaderBuffer* pShaderBuffer) = 0;
	virtual void DestroyVertexShader(VertexShaderHandle_t hShader) = 0;
	virtual GeometryShaderHandle_t CreateGeometryShader(IShaderBuffer* pShaderBuffer) = 0;
	virtual void DestroyGeometryShader(GeometryShaderHandle_t hShader) = 0;
	virtual PixelShaderHandle_t CreatePixelShader(IShaderBuffer* pShaderBuffer) = 0;
	virtual void DestroyPixelShader(PixelShaderHandle_t hShader) = 0;

	// Utility methods to make shader creation simpler
	// NOTE: For the utlbuffer version, use a binary buffer for a compiled shader
	// and a text buffer for a source-code (.fxc) shader
	VertexShaderHandle_t CreateVertexShader(const char* pProgram, size_t nBufLen, const char* pShaderVersion);
	VertexShaderHandle_t CreateVertexShader(CUtlBuffer& buf, const char* pShaderVersion = NULL);
	GeometryShaderHandle_t CreateGeometryShader(const char* pProgram, size_t nBufLen, const char* pShaderVersion);
	GeometryShaderHandle_t CreateGeometryShader(CUtlBuffer& buf, const char* pShaderVersion = NULL);
	PixelShaderHandle_t CreatePixelShader(const char* pProgram, size_t nBufLen, const char* pShaderVersion);
	PixelShaderHandle_t CreatePixelShader(CUtlBuffer& buf, const char* pShaderVersion = NULL);

	// NOTE: Deprecated!! Use CreateVertexBuffer/CreateIndexBuffer instead
	// Creates/destroys Mesh
	virtual IMesh* CreateStaticMesh(VertexFormat_t vertexFormat, const char* pTextureBudgetGroup, IMaterial* pMaterial = NULL) = 0;
	virtual void DestroyStaticMesh(IMesh* mesh) = 0;

	// Creates/destroys static vertex + index buffers
	virtual IVertexBuffer* CreateVertexBuffer(ShaderBufferType_t type, VertexFormat_t fmt, int nVertexCount, const char* pBudgetGroup) = 0;
	virtual void DestroyVertexBuffer(IVertexBuffer* pVertexBuffer) = 0;

	virtual IIndexBuffer* CreateIndexBuffer(ShaderBufferType_t bufferType, MaterialIndexFormat_t fmt, int nIndexCount, const char* pBudgetGroup) = 0;
	virtual void DestroyIndexBuffer(IIndexBuffer* pIndexBuffer) = 0;

	// Do we need to specify the stream here in the case of locking multiple dynamic VBs on different streams?
	virtual IVertexBuffer* GetDynamicVertexBuffer(int nStreamID, VertexFormat_t vertexFormat, bool bBuffered = true) = 0;
	virtual IIndexBuffer* GetDynamicIndexBuffer(MaterialIndexFormat_t fmt, bool bBuffered = true) = 0;

	// A special path used to tick the front buffer while loading on the 360
	virtual void EnableNonInteractiveMode(MaterialNonInteractiveMode_t mode, ShaderNonInteractiveInfo_t* pInfo = NULL) = 0;
	virtual void RefreshFrontBufferNonInteractive() = 0;
	virtual void HandleThreadEvent(unsigned int threadEvent) = 0;

#ifdef DX_TO_GL_ABSTRACTION
	virtual void DoStartupShaderPreloading(void) = 0;
#endif
	virtual char* GetDisplayDeviceName() = 0;

};