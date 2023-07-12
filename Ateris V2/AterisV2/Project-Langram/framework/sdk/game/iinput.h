#include "c_basecombatweapon.h"
#include "iclientmodeshared.h"
#include "c_usercmd.h"

struct kbutton_t;

enum GameActionSet_t
{
	GAME_ACTION_SET_NONE = -1,
	GAME_ACTION_SET_MENUCONTROLS = 0,
	GAME_ACTION_SET_FPSCONTROLS,
	GAME_ACTION_SET_IN_GAME_HUD,
	GAME_ACTION_SET_SPECTATOR,
};
//
enum GameActionSetFlags_t
{
	GAME_ACTION_SET_FLAGS_NONE = 0,
	GAME_ACTION_SET_FLAGS_TAUNTING = (1 << 0),
};
//
//enum JoystickType_t
//{
//	INPUT_TYPE_GENERIC_JOYSTICK = 0,
//	INPUT_TYPE_X360,
//	INPUT_TYPE_STEAMCONTROLLER,
//};

struct CameraThirdData_t
{
	float	m_flPitch;
	float	m_flYaw;
	float	m_flDist;
	float	m_flLag;
	vector	m_vecHullMin;
	vector	m_vecHullMax;
};

enum ESteamControllerPadIInput;

class CVerifiedUserCmd
{
public:
	CUserCmd	m_cmd;
	CRC32_t		m_crc;
};

inline CRC32_t GetChecksum(CUserCmd* cmd)
{
	CRC32_t crc;

	CRC32_Init(&crc);
	CRC32_ProcessBuffer(&crc, &cmd->command_number, sizeof(cmd->command_number));
	CRC32_ProcessBuffer(&crc, &cmd->tick_count, sizeof(cmd->tick_count));
	CRC32_ProcessBuffer(&crc, &cmd->viewangles, sizeof(cmd->viewangles));
	CRC32_ProcessBuffer(&crc, &cmd->forwardmove, sizeof(cmd->forwardmove));
	CRC32_ProcessBuffer(&crc, &cmd->sidemove, sizeof(cmd->sidemove));
	CRC32_ProcessBuffer(&crc, &cmd->upmove, sizeof(cmd->upmove));
	CRC32_ProcessBuffer(&crc, &cmd->buttons, sizeof(cmd->buttons));
	CRC32_ProcessBuffer(&crc, &cmd->impulse, sizeof(cmd->impulse));
	CRC32_ProcessBuffer(&crc, &cmd->weaponselect, sizeof(cmd->weaponselect));
	CRC32_ProcessBuffer(&crc, &cmd->weaponsubtype, sizeof(cmd->weaponsubtype));
	CRC32_ProcessBuffer(&crc, &cmd->random_seed, sizeof(cmd->random_seed));
	CRC32_ProcessBuffer(&crc, &cmd->mousedx, sizeof(cmd->mousedx));
	CRC32_ProcessBuffer(&crc, &cmd->mousedy, sizeof(cmd->mousedy));
	CRC32_Final(&crc);

	return crc;
}

class IInput
{
public:
	// Initialization/shutdown of the subsystem
	virtual	void		Init_All(void) = 0;
	virtual void		Shutdown_All(void) = 0;
	// Latching button states
	virtual int			GetButtonBits(int) = 0;
	// Create movement command
	virtual void		CreateMove(int sequence_number, float input_sample_frametime, bool active) = 0;
	virtual void		ExtraMouseSample(float frametime, bool active) = 0;
	virtual bool		WriteUsercmdDeltaToBuffer(bf_write* buf, int from, int to, bool isnewcommand) = 0;
	virtual void		EncodeUserCmdToBuffer(bf_write& buf, int slot) = 0;
	virtual void		DecodeUserCmdFromBuffer(bf_read& buf, int slot) = 0;

	virtual CUserCmd* GetUserCmd(int sequence_number) = 0;

	virtual void		MakeWeaponSelection(C_BaseCombatWeapon* weapon) = 0;

	// Retrieve key state
	virtual float		KeyState(kbutton_t* key) = 0;
	// Issue key event
	virtual int			KeyEvent(int eventcode, ButtonCode_t keynum, const char* pszCurrentBinding) = 0;
	// Look for key
	virtual kbutton_t* FindKey(const char* name) = 0;

	// Issue commands from controllers
	virtual void		ControllerCommands(void) = 0;
	// Extra initialization for some joysticks
	virtual void		Joystick_Advanced(void) = 0;
	virtual void		Joystick_SetSampleTime(float frametime) = 0;
	virtual void		IN_SetSampleTime(float frametime) = 0;

	// Accumulate mouse delta
	virtual void		AccumulateMouse(void) = 0;
	// Activate/deactivate mouse
	virtual void		ActivateMouse(void) = 0;
	virtual void		DeactivateMouse(void) = 0;

	// Clear mouse state data
	virtual void		ClearStates(void) = 0;
	// Retrieve lookspring setting
	virtual float		GetLookSpring(void) = 0;

	// Retrieve mouse position
	virtual void		GetFullscreenMousePos(int* mx, int* my, int* unclampedx = 0, int* unclampedy = 0) = 0;
	virtual void		SetFullscreenMousePos(int mx, int my) = 0;
	virtual void		ResetMouse(void) = 0;
	virtual	float		GetLastForwardMove(void) = 0;
	virtual	float		Joystick_GetForward(void) = 0;
	virtual	float		Joystick_GetSide(void) = 0;
	virtual	float		Joystick_GetPitch(void) = 0;
	virtual	float		Joystick_GetYaw(void) = 0;

	// Third Person camera ( TODO/FIXME:  Move this to a separate interface? )
	virtual void		CAM_Think(void) = 0;
	virtual int			CAM_IsThirdPerson(void) = 0;
	virtual void		CAM_ToThirdPerson(void) = 0;
	virtual void		CAM_ToFirstPerson(void) = 0;
	virtual void		CAM_StartMouseMove(void) = 0;
	virtual void		CAM_EndMouseMove(void) = 0;
	virtual void		CAM_StartDistance(void) = 0;
	virtual void		CAM_EndDistance(void) = 0;
	virtual int			CAM_InterceptingMouse(void) = 0;

	// orthographic camera info	( TODO/FIXME:  Move this to a separate interface? )
	virtual void		CAM_ToOrthographic() = 0;
	virtual	bool		CAM_IsOrthographic() const = 0;
	virtual	void		CAM_OrthographicSize(float& w, float& h) const = 0;

	virtual void		SetPreferredGameActionSet(GameActionSet_t action_set) = 0;
	virtual GameActionSet_t GetPreferredGameActionSet() = 0;
	virtual void		SetGameActionSetFlags(GameActionSetFlags_t action_set_flags) = 0;

	virtual void		LevelInit(void) = 0;

	// Causes an input to have to be re-pressed to become active
	virtual void		ClearInputButton(int bits) = 0;

	virtual	void		CAM_SetCameraThirdData(CameraThirdData_t* pCameraData, const QAngle& vecCameraOffset) = 0;
	virtual void		CAM_CameraThirdThink(void) = 0;

	virtual	bool		EnableJoystickMode() = 0;

	virtual bool		IsSteamControllerActive() = 0;

	// Private Data -- public now >:D
public:
	typedef struct
	{
		unsigned int AxisFlags;
		unsigned int AxisMap;
		unsigned int ControlMap;
	} joy_axis_t;

	void		DescribeJoystickAxis(char const* axis, joy_axis_t* mapping);
	char const* DescribeAxis(int index);

	enum
	{
		GAME_AXIS_NONE = 0,
		GAME_AXIS_FORWARD,
		GAME_AXIS_PITCH,
		GAME_AXIS_SIDE,
		GAME_AXIS_YAW,
		MAX_GAME_AXES
	};

	enum
	{
		CAM_COMMAND_NONE = 0,
		CAM_COMMAND_TOTHIRDPERSON = 1,
		CAM_COMMAND_TOFIRSTPERSON = 2
	};

	enum
	{
		MOUSE_ACCEL_THRESHHOLD1 = 0,	// if mouse moves > this many mickey's double it
		MOUSE_ACCEL_THRESHHOLD2,		// if mouse moves > this many mickey's double it a second time
		MOUSE_SPEED_FACTOR,				// 0 = disabled, 1 = threshold 1 enabled, 2 = threshold 2 enabled

		NUM_MOUSE_PARAMS,
	};

	// Has the mouse been initialized?
	bool		m_fMouseInitialized;
	// Is the mosue active?
	bool		m_fMouseActive;
	// Has the joystick advanced initialization been run?
	bool		m_fJoystickAdvancedInit;
	// Used to support hotplugging by reinitializing the advanced joystick system when we toggle between some/none joysticks.
	bool		m_fHadJoysticks;

	// Accumulated mouse deltas
	float		m_flAccumulatedMouseXMovement;
	float		m_flAccumulatedMouseYMovement;
	float		m_flPreviousMouseXPosition;
	float		m_flPreviousMouseYPosition;
	float		m_flRemainingJoystickSampleTime;
	float		m_flKeyboardSampleTime;

	// Flag to restore systemparameters when exiting
	bool		m_fRestoreSPI;
	// Original mouse parameters
	int			m_rgOrigMouseParms[NUM_MOUSE_PARAMS];
	// Current mouse parameters.
	int			m_rgNewMouseParms[NUM_MOUSE_PARAMS];
	bool		m_rgCheckMouseParam[NUM_MOUSE_PARAMS];
	// Are the parameters valid
	bool		m_fMouseParmsValid;
	// Joystick Axis data
	joy_axis_t m_rgAxes[6];
	// List of queryable keys
	void* m_pKeys;

	// Is the 3rd person camera using the mouse?
	bool		m_fCameraInterceptingMouse;
	// Are we in 3rd person view?
	bool		m_fCameraInThirdPerson;
	// Should we move view along with mouse?
	bool		m_fCameraMovingWithMouse;


	// Is the camera in distance moving mode?
	bool		m_fCameraDistanceMove;
	// Old and current mouse position readings.
	int			m_nCameraOldX;
	int			m_nCameraOldY;
	int			m_nCameraX;
	int			m_nCameraY;

	// orthographic camera settings
	bool		m_CameraIsOrthographic;

	QAngle		m_angPreviousViewAngles;

	float		m_flLastForwardMove;

	float m_flPreviousJoystickForward;
	float m_flPreviousJoystickSide;
	float m_flPreviousJoystickPitch;
	float m_flPreviousJoystickYaw;

	// Steam controller stuff
	int m_nControllerType[1 + 8];

	GameActionSet_t m_PreferredGameActionSet;
	GameActionSetFlags_t m_GameActionSetFlags;

	bool m_bSteamControllerGameActionsInitialized;

	CUserCmd* m_pCommands;
	CVerifiedUserCmd* m_pVerifiedCommands;

	CameraThirdData_t* m_pCameraThirdData;

	inline CVerifiedUserCmd* GetVerifiedCmds()
	{
		return *reinterpret_cast<CVerifiedUserCmd**>(reinterpret_cast<uintptr_t>(this) + 0x100);
	}

	inline CUserCmd* GetCmds()
	{
		return *reinterpret_cast<CUserCmd**>(reinterpret_cast<uintptr_t>(this) + 0xFC);
	}

};