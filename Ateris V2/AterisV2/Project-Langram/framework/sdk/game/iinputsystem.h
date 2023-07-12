#pragma once

enum AnalogCode_t
{
	ANALOG_CODE_INVALID = -1,
	MOUSE_X = 0,
	MOUSE_Y,
	MOUSE_XY, // invoked when either x or y changes
	MOUSE_WHEEL,
	ANALOG_CODE_LAST = 10,
};

enum ButtonCode_t_Custom
{
	ISYS_KEY_INVALID = 0x0,
	ISYS_KEY_0 = 0x1,
	ISYS_KEY_1 = 0x2,
	ISYS_KEY_2 = 0x3,
	ISYS_KEY_3 = 0x4,
	ISYS_KEY_4 = 0x5,
	ISYS_KEY_5 = 0x6,
	ISYS_KEY_6 = 0x7,
	ISYS_KEY_7 = 0x8,
	ISYS_KEY_8 = 0x9,
	ISYS_KEY_9 = 0xA,
	ISYS_KEY_A = 0xB,
	ISYS_KEY_B = 0xC,
	ISYS_KEY_C = 0xD,
	ISYS_KEY_D = 0xE,
	ISYS_KEY_E = 0xF,
	ISYS_KEY_F = 0x10,
	ISYS_KEY_G = 0x11,
	ISYS_KEY_H = 0x12,
	ISYS_KEY_I = 0x13,
	ISYS_KEY_J = 0x14,
	ISYS_KEY_K = 0x15,
	ISYS_KEY_L = 0x16,
	ISYS_KEY_M = 0x17,
	ISYS_KEY_N = 0x18,
	ISYS_KEY_O = 0x19,
	ISYS_KEY_P = 0x1A,
	ISYS_KEY_Q = 0x1B,
	ISYS_KEY_R = 0x1C,
	ISYS_KEY_S = 0x1D,
	ISYS_KEY_T = 0x1E,
	ISYS_KEY_U = 0x1F,
	ISYS_KEY_V = 0x20,
	ISYS_KEY_W = 0x21,
	ISYS_KEY_X = 0x22,
	ISYS_KEY_Y = 0x23,
	ISYS_KEY_Z = 0x24,
	ISYS_KEY_PAD_0 = 0x25,
	ISYS_KEY_PAD_1 = 0x26,
	ISYS_KEY_PAD_2 = 0x27,
	ISYS_KEY_PAD_3 = 0x28,
	ISYS_KEY_PAD_4 = 0x29,
	ISYS_KEY_PAD_5 = 0x2A,
	ISYS_KEY_PAD_6 = 0x2B,
	ISYS_KEY_PAD_7 = 0x2C,
	ISYS_KEY_PAD_8 = 0x2D,
	ISYS_KEY_PAD_9 = 0x2E,
	ISYS_KEY_PAD_DIVIDE = 0x2F,
	ISYS_KEY_PAD_MULTIPLY = 0x30,
	ISYS_KEY_PAD_MINUS = 0x31,
	ISYS_KEY_PAD_PLUS = 0x32,
	ISYS_KEY_PAD_ENTER = 0x33,
	ISYS_KEY_PAD_DECIMAL = 0x34,
	ISYS_KEY_LBRACKET = 0x35,
	ISYS_KEY_RBRACKET = 0x36,
	ISYS_KEY_SEMICOLON = 0x37,
	ISYS_KEY_APOSTROPHE = 0x38,
	ISYS_KEY_BACKQUOTE = 0x39,
	ISYS_KEY_COMMA = 0x3A,
	ISYS_KEY_PERIOD = 0x3B,
	ISYS_KEY_SLASH = 0x3C,
	ISYS_KEY_BACKSLASH = 0x3D,
	ISYS_KEY_MINUS = 0x3E,
	ISYS_KEY_EQUAL = 0x3F,
	ISYS_KEY_ENTER = 0x40,
	ISYS_KEY_SPACE = 0x41,
	ISYS_KEY_BACKSPACE = 0x42,
	ISYS_KEY_TAB = 0x43,
	ISYS_KEY_CAPSLOCK = 0x44,
	ISYS_KEY_NUMLOCK = 0x45,
	ISYS_KEY_ESCAPE = 0x46,
	ISYS_KEY_SCROLLLOCK = 0x47,
	ISYS_KEY_INSERT = 0x48,
	ISYS_KEY_DELETE = 0x49,
	ISYS_KEY_HOME = 0x4A,
	ISYS_KEY_END = 0x4B,
	ISYS_KEY_PAGEUP = 0x4C,
	ISYS_KEY_PAGEDOWN = 0x4D,
	ISYS_KEY_BREAK = 0x4E,
	ISYS_KEY_LSHIFT = 0x4F,
	ISYS_KEY_RSHIFT = 0x50,
	ISYS_KEY_LALT = 0x51,
	ISYS_KEY_RALT = 0x52,
	ISYS_KEY_LCONTROL = 0x53,
	ISYS_KEY_RCONTROL = 0x54,
	ISYS_KEY_LWIN = 0x55,
	ISYS_KEY_RWIN = 0x56,
	ISYS_KEY_APP = 0x57,
	ISYS_KEY_UP = 0x58,
	ISYS_KEY_LEFT = 0x59,
	ISYS_KEY_DOWN = 0x5A,
	ISYS_KEY_RIGHT = 0x5B,
	ISYS_KEY_F1 = 0x5C,
	ISYS_KEY_F2 = 0x5D,
	ISYS_KEY_F3 = 0x5E,
	ISYS_KEY_F4 = 0x5F,
	ISYS_KEY_F5 = 0x60,
	ISYS_KEY_F6 = 0x61,
	ISYS_KEY_F7 = 0x62,
	ISYS_KEY_F8 = 0x63,
	ISYS_KEY_F9 = 0x64,
	ISYS_KEY_F10 = 0x65,
	ISYS_KEY_F11 = 0x66,
	ISYS_KEY_F12 = 0x67,
	ISYS_KEY_MOUSE_1 = 0x6B,
	ISYS_KEY_MOUSE_2 = 0x6C,
	ISYS_KEY_MOUSE_3 = 0x6D,
	ISYS_KEY_MOUSE_4 = 0x6E,
	ISYS_KEY_MOUSE_5 = 0x6F
};

class IInputSystem : public IAppSystem
{
public:
	// Attach, detach input system from a particular window
	// This window should be the root window for the application
	// Only 1 window should be attached at any given time.
	virtual void AttachToWindow(void* hWnd) = 0;
	virtual void DetachFromWindow() = 0;

	// Enables/disables input. PollInputState will not update current 
	// button/analog states when it is called if the system is disabled.
	virtual void EnableInput(bool bEnable) = 0;

	// Enables/disables the windows message pump. PollInputState will not
	// Peek/Dispatch messages if this is disabled
	virtual void EnableMessagePump(bool bEnable) = 0;

	// Polls the current input state
	virtual void PollInputState() = 0;

	// Gets the time of the last polling in ms
	virtual int GetPollTick() const = 0;

	// Is a button down? "Buttons" are binary-state input devices (mouse buttons, keyboard keys)
	virtual bool IsButtonDown(ButtonCode_t code) const = 0;

	// Returns the tick at which the button was pressed and released
	virtual int GetButtonPressedTick(ButtonCode_t code) const = 0;
	virtual int GetButtonReleasedTick(ButtonCode_t code) const = 0;

	// Gets the value of an analog input device this frame
	// Includes joysticks, mousewheel, mouse
	virtual int GetAnalogValue(AnalogCode_t code) const = 0;

	// Gets the change in a particular analog input device this frame
	// Includes joysticks, mousewheel, mouse
	virtual int GetAnalogDelta(AnalogCode_t code) const = 0;

	// Returns the input events since the last poll
	virtual int GetEventCount() const = 0;
	virtual const InputEvent_t* GetEventData() const = 0;

	// Posts a user-defined event into the event queue; this is expected
	// to be called in overridden wndprocs connected to the root panel.
	virtual void PostUserEvent(const InputEvent_t& event) = 0;

	// Returns the number of joysticks
	virtual int GetJoystickCount() const = 0;

	// Enable/disable joystick, it has perf costs
	virtual void EnableJoystickInput(int nJoystick, bool bEnable) = 0;

	// Enable/disable diagonal joystick POV (simultaneous POV buttons down)
	virtual void EnableJoystickDiagonalPOV(int nJoystick, bool bEnable) = 0;

	// Sample the joystick and append events to the input queue
	virtual void SampleDevices(void) = 0;

	// FIXME: Currently force-feedback is only supported on the Xbox 360
	virtual void SetRumble(float fLeftMotor, float fRightMotor, int userId) = 0;
	virtual void StopRumble(void) = 0;

	// Resets the input state
	virtual void ResetInputState() = 0;

	// Sets a player as the primary user - all other controllers will be ignored.
	virtual void SetPrimaryUserId(int userId) = 0;

	// Convert back + forth between ButtonCode/AnalogCode + strings
	virtual const char* ButtonCodeToString(ButtonCode_t code) const = 0;
	virtual const char* AnalogCodeToString(AnalogCode_t code) const = 0;
	virtual ButtonCode_t StringToButtonCode(const char* pString) const = 0;
	virtual AnalogCode_t StringToAnalogCode(const char* pString) const = 0;

	// Sleeps until input happens. Pass a negative number to sleep infinitely
	virtual void SleepUntilInput(int nMaxSleepTimeMS = -1) = 0;

	// Convert back + forth between virtual codes + button codes
	// FIXME: This is a temporary piece of code
	virtual ButtonCode_t VirtualKeyToButtonCode(int nVirtualKey) const = 0;
	virtual int ButtonCodeToVirtualKey(ButtonCode_t code) const = 0;
	virtual ButtonCode_t ScanCodeToButtonCode(int lParam) const = 0;

	// How many times have we called PollInputState?
	virtual int GetPollCount() const = 0;

	// Sets the cursor position
	virtual void SetCursorPosition(int x, int y) = 0;

	// NVNT get address to haptics interface
	virtual void* GetHapticsInterfaceAddress() const = 0;

	virtual void SetNovintPure(bool bPure) = 0;

	// read and clear accumulated raw input values
	virtual bool GetRawMouseAccumulators(int& accumX, int& accumY) = 0;

	// tell the input system that we're not a game, we're console text mode.
	// this is used for dedicated servers to not initialize joystick system.
	// this needs to be called before CInputSystem::Init (e.g. in PreInit of
	// some system) if you want ot prevent the joystick system from ever
	// being initialized.
	virtual void SetConsoleTextMode(bool bConsoleTextMode) = 0;

	virtual void* SteamControllerInterface() = 0;
	virtual void* GetNumSteamControllersConnected() = 0;
	virtual bool IsSteamControllerActive() = 0;
	virtual bool IsSteamControllerConnected() = 0;
	virtual int GetSteamControllerIndexForSlot(int nSlot) = 0;
	virtual bool GetRadialMenuStickValues(int nSlot, float& fX, float& fY) = 0;
	virtual void ActivateSteamControllerActionSetForSlot(void* nSlot, GameActionSet_t eActionSet) = 0;
};

#define VINPUTSYSTEM_INTERFACE "InputSystemVersion001"
