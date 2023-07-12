#pragma once

#include "dt_recv.h"
#include "bitbuf.h"
#include "vector.h"
#include "vmatrix.h"
#include "checksum_crc.h"

class ClientClass;
struct model_t;
class CSentence;
struct vrect_t;
struct cmodel_t;
class IMaterial;
class CAudioSource;
class CMeasureSection;
class SurfInfo;
class ISpatialQuery;
struct cache_user_t;
class IMaterialSystem;
struct ScreenFade_t;
struct ScreenShake_t;
class CEngineSprite;
class CGlobalVarsBase;
class CPhysCollide;
class CSaveRestoreData;
struct datamap_t;
struct typedescription_t;
class CStandardRecvProxies;
struct client_textmessage_t;
class IAchievementMgr;
class CGamestatsData;
class KeyValues;
class IFileList;
class CRenamedRecvTableInfo;
class CMouthInfo;
class IConVar;
enum ButtonCode_t : size_t;
class vmode_s;

#define FLOW_OUTGOING 0
#define FLOW_INCOMING 1

typedef struct netadr_s
{
public:
	netadr_s()
	{
		SetIP(0);
		SetPort(0);
		//SetType(NA_IP);
	}
	netadr_s(const char* pch)
	{
		SetFromString(pch);
	}
	void Clear(); // invalids Address

				  //void SetType(netadrtype_t type);
	void SetPort(unsigned short port);
	bool SetFromSockadr(const struct sockaddr* s);
	void SetIP(unsigned int unIP); // Sets IP.  unIP is in host order (little-endian)
	void SetIPAndPort(unsigned int unIP, unsigned short usPort)
	{
		SetIP(unIP);
		SetPort(usPort);
	}
	void SetFromString(const char* pch, bool bUseDNS = false); // if bUseDNS is true then do a DNS lookup if needed

	bool CompareAdr(const netadr_s& a, bool onlyBase = false) const;
	bool CompareClassBAdr(const netadr_s& a) const;
	bool CompareClassCAdr(const netadr_s& a) const;

	//netadrtype_t GetType() const;
	unsigned short GetPort() const;
	const char* ToString(bool onlyBase = false) const; // returns xxx.xxx.xxx.xxx:ppppp
	void ToSockadr(struct sockaddr* s) const;
	unsigned int GetIP() const;

	bool IsLocalhost() const;   // true, if this is the localhost IP
	bool IsLoopback() const;	// true if engine loopback buffers are used
	bool IsReservedAdr() const; // true, if this is a private LAN IP
	bool IsValid() const;		// ip & port != 0
	void SetFromSocket(int hSocket);
	// These function names are decorated because the Xbox360 defines macros for ntohl and htonl
	unsigned long addr_ntohl() const;
	unsigned long addr_htonl() const;
	bool operator==(const netadr_s& netadr) const
	{
		return (CompareAdr(netadr));
	}
	bool operator<(const netadr_s& netadr) const;

public: // members are public to avoid to much changes
		//netadrtype_t type;
	unsigned char ip[4];
	unsigned short port;
} netadr_t;


class INetMessage;
class INetChannelHandler;
class INetChannelInfo
{
public:

	enum
	{
		GENERIC = 0,	// must be first and is default group
		LOCALPLAYER,	// bytes for local player entity update
		OTHERPLAYERS,	// bytes for other players update
		ENTITIES,		// all other entity bytes
		SOUNDS,			// game sounds
		EVENTS,			// event messages
		TEMPENTS,		// temp entities
		USERMESSAGES,	// user messages
		ENTMESSAGES,	// entity messages
		VOICE,			// voice data
		STRINGTABLE,	// a stringtable update
		MOVE,			// client move cmds
		STRINGCMD,		// string command
		SIGNON,			// various signondata
		TOTAL,			// must be last and is not a real group
	};

	virtual const char* GetName(void) const = 0;	// get channel name
	virtual const char* GetAddress(void) const = 0; // get channel IP address as string
	virtual float		GetTime(void) const = 0;	// current net time
	virtual float		GetTimeConnected(void) const = 0;	// get connection time in seconds
	virtual int			GetBufferSize(void) const = 0;	// netchannel packet history size
	virtual int			GetDataRate(void) const = 0; // send data rate in byte/sec

	virtual bool		IsLoopback(void) const = 0;	// true if loopback channel
	virtual bool		IsTimingOut(void) const = 0;	// true if timing out
	virtual bool		IsPlayback(void) const = 0;	// true if demo playback

	virtual float		GetLatency(int flow) const = 0;	 // current latency (RTT), more accurate but jittering
	virtual float		GetAverageLatency(int flow) const = 0; // average packet latency in seconds
	virtual float		GetAverageLoss(int flow) const = 0;	 // avg packet loss[0..1]
	virtual float		GetAverageChoke(int flow) const = 0;	 // avg packet choke[0..1]
	virtual float		GetAverageData(int flow) const = 0;	 // data flow in bytes/sec
	virtual float		GetAveragePackets(int flow) const = 0; // avg packets/sec
	virtual int			GetTotalData(int flow) const = 0;	 // total flow in/out in bytes
	virtual int			GetTotalPackets(int flow) const = 0;
	virtual int			GetSequenceNumber(int flow) const = 0;	// last send seq number
	virtual bool		IsValidPacket(int flow, int frame_number) const = 0; // true if packet was not lost/dropped/chocked/flushed
	virtual float		GetPacketTime(int flow, int frame_number) const = 0; // time when packet was send
	virtual int			GetPacketBytes(int flow, int frame_number, int group) const = 0; // group size of this packet
	virtual bool		GetStreamProgess(int flow, int* received, int* total) const = 0;  // TCP progress if transmitting
	virtual float		GetTimeSinceLastRecieved(void) const = 0;	// get time since last recieved packet in seconds
	virtual	float		GetCommandInterpolationAmount(int flow, int frame_number) const = 0;
	virtual void		GetPacketResponseLatency(int flow, int frame_number, int* pnLatencyMsecs, int* pnChoke) const = 0;
	virtual void		GetRemoveFramerate(float* pflFrameTime, float* pflFrameTimeStdDeviation, float* pflFrameStartTimeStdDeviation) const = 0;

	virtual float		GetTimeoutSeconds() const = 0;
};


#define MULTIPLAYER_BACKUP 90
#define WEAPON_SUBTYPE_BITS	6
#define	MAX_OSPATH		260			// max length of a filesystem pathname
#define MAX_STREAMS			2    
#define NET_FRAMES_BACKUP	64		// must be power of 2
#define NET_FRAMES_MASK		(NET_FRAMES_BACKUP-1)
#define MAX_SUBCHANNELS		8		// we have 8 alternative send&wait bits
#define SUBCHANNEL_FREE		0	// subchannel is free to use
#define SUBCHANNEL_TOSEND	1	// subchannel has data, but not send yet
#define SUBCHANNEL_WAITING	2   // sbuchannel sent data, waiting for ACK
#define SUBCHANNEL_DIRTY	3	// subchannel is marked as dirty during changelevel
#define DELTA_OFFSET_BITS 5
#define DELTA_OFFSET_MAX ( ( 1 << DELTA_OFFSET_BITS ) - 1 )
#define DELTASIZE_BITS 20	
#define NUM_NEW_COMMAND_BITS		4
#define MAX_NEW_COMMANDS			((1 << NUM_NEW_COMMAND_BITS)-1)
#define NUM_BACKUP_COMMAND_BITS		3
#define MAX_BACKUP_COMMANDS			((1 << NUM_BACKUP_COMMAND_BITS)-1)
#define MAX_PLAYER_NAME_LENGTH		32
#define MAX_CUSTOM_FILES		4		// max 4 files
#define NETMSG_TYPE_BITS	6	// must be 2^NETMSG_TYPE_BITS > SVC_LASTMSG
#define net_NOP 0        // nop command used for padding
#define net_Disconnect 1 // disconnect, last message in connection
#define net_File 2       // file transmission message request/deny
#define net_Tick 3        // send last world tick
#define net_StringCmd 4   // a string command
#define net_SetConVar 5   // sends one/multiple convar settings
#define net_SignonState 6 // signals current signon state
#define clc_ClientInfo 8       
#define clc_Move 9             
#define clc_VoiceData 10       
#define clc_BaselineAck 11     
#define clc_ListenEvents 12    
#define clc_RespondCvarValue 13
#define clc_FileCRCCheck 14    
#define clc_CmdKeyValues 16    
#define FLOW_OUTGOING	0		
#define FLOW_INCOMING	1
#define MAX_FLOWS		2		// in & out

#define DECLARE_BASE_MESSAGE(msgtype)       \
public:                                     \
    bool ReadFromBuffer(bf_read &buffer);   \
    bool WriteToBuffer(bf_write &buffer);   \
    const char *ToString() const;           \
    int GetType() const { return msgtype; } \
    const char *GetName() const { return #msgtype; }

#define DECLARE_NET_MESSAGE(name)     \
    DECLARE_BASE_MESSAGE(net_##name); \
    void *m_pMessageHandler;

#define DECLARE_CLC_MESSAGE(name)     \
    DECLARE_BASE_MESSAGE(clc_##name); \
    void *m_pMessageHandler;

#define DECLARE_NET_MESSAGE(name)     \
    DECLARE_BASE_MESSAGE(net_##name); \
    void *m_pMessageHandler;

#define DECLARE_SVC_MESSAGE( name )		\
	DECLARE_BASE_MESSAGE( svc_##name );	\
	void *m_pMessageHandler;

class INetMessage;
class INetChannel : public INetChannelInfo
{
public:
	virtual ~INetChannel(void)
	{
	};

	virtual void SetDataRate(float rate) = 0;
	virtual bool RegisterMessage(INetMessage* msg) = 0;
	virtual bool StartStreaming(unsigned int challengeNr) = 0;
	virtual void ResetStreaming(void) = 0;
	virtual void SetTimeout(float seconds) = 0;
	//virtual void SetDemoRecorder(IDemoRecorder *recorder) = 0;
	virtual void SetChallengeNr(unsigned int chnr) = 0;

	virtual void Reset(void) = 0;
	virtual void Clear(void) = 0;
	virtual void Shutdown(const char* reason) = 0;

	virtual void ProcessPlayback(void) = 0;
	virtual bool ProcessStream(void) = 0;
	virtual void ProcessPacket(struct netpacket_s* packet, bool bHasHeader) = 0;

	virtual bool SendNetMsg(INetMessage& msg, bool bForceReliable = false, bool bVoice = false) = 0;
#ifdef POSIX
	FORCEINLINE bool SendNetMsg(INetMessage const& msg, bool bForceReliable = false, bool bVoice = false)
	{
		return SendNetMsg(*((INetMessage*)&msg), bForceReliable, bVoice);
	}
#endif
	//virtual bool SendData(bf_write &msg, bool bReliable = true) = 0;
	virtual bool SendFile(const char* filename, unsigned int transferID) = 0;
	virtual void DenyFile(const char* filename, unsigned int transferID) = 0;
	virtual void RequestFile_OLD(const char* filename, unsigned int transferID) = 0; // get rid of this function when we version the
	virtual void SetChoked(void) = 0;
	//virtual int SendDatagram(bf_write *data) = 0;
	virtual bool Transmit(bool onlyReliable = false) = 0;

	virtual const netadr_t& GetRemoteAddress(void) const = 0;
	virtual INetChannelHandler* GetMsgHandler(void) const = 0;
	virtual int GetDropNumber(void) const = 0;
	virtual int GetSocket(void) const = 0;
	virtual unsigned int GetChallengeNr(void) const = 0;
	virtual void GetSequenceData(int& nOutSequenceNr, int& nInSequenceNr, int& nOutSequenceNrAck) = 0;
	virtual void SetSequenceData(int nOutSequenceNr, int nInSequenceNr, int nOutSequenceNrAck) = 0;

	virtual void UpdateMessageStats(int msggroup, int bits) = 0;
	virtual bool CanPacket(void) const = 0;
	virtual bool IsOverflowed(void) const = 0;
	virtual bool IsTimedOut(void) const = 0;
	virtual bool HasPendingReliableData(void) = 0;

	virtual void SetFileTransmissionMode(bool bBackgroundMode) = 0;
	virtual void SetCompressionMode(bool bUseCompression) = 0;
	virtual unsigned int RequestFile(const char* filename) = 0;
	virtual float GetTimeSinceLastReceived(void) const = 0; // get time since last received packet in seconds

	virtual void SetMaxBufferSize(bool bReliable, int nBYTEs, bool bVoice = false) = 0;

	virtual bool IsNull() const = 0;
	virtual int GetNumBitsWritten(bool bReliable) = 0;
	virtual void SetInterpolationAmount(float flInterpolationAmount) = 0;
	virtual void SetRemoteFramerate(float flFrameTime, float flFrameTimeStdDeviation) = 0;

	// Max # of payload BYTEs before we must split/fragment the packet
	virtual void SetMaxRoutablePayloadSize(int nSplitSize) = 0;
	virtual int GetMaxRoutablePayloadSize() = 0;

	virtual int GetProtocolVersion() = 0;
};


class INetMessage
{
public:
	virtual ~INetMessage() {};

	// Use these to setup who can hear whose voice.
	// Pass in client indices (which are their ent indices - 1).

	virtual void SetNetChannel(INetChannel* netchan) = 0; // netchannel this message is from/for
	virtual void SetReliable(bool state) = 0;             // set to true if it's a reliable message

	virtual bool Process(void) = 0; // calles the recently set handler to process this message

	virtual bool ReadFromBuffer(bf_read& buffer) = 0; // returns true if parsing was OK
	virtual bool WriteToBuffer(bf_write& buffer) = 0; // returns true if writing was OK

	virtual bool IsReliable(void) const = 0; // true, if message needs reliable handling

	virtual int GetType(void) const = 0;         // returns module specific header tag eg svc_serverinfo
	virtual int GetGroup(void) const = 0;        // returns net message group of this message
	virtual const char* GetName(void) const = 0; // returns network message name, eg "svc_serverinfo"
	virtual INetChannel* GetNetChannel(void) const = 0;
	virtual const char* ToString(void) const = 0; // returns a human readable string about message content
};

typedef void* FileHandle_t;
#define MAX_OSPATH 260

class CNetChannel : public INetChannel
{

public: // netchan structurs
	typedef struct dataFragments_s
	{
		FileHandle_t file;                 // open file handle
		char         filename[MAX_OSPATH]; // filename
		char* buffer;               // if NULL it's a file
		unsigned int BYTEs;                // size in BYTEs
		unsigned int bits;                 // size in bits
		unsigned int transferID;           // only for files
		bool         isCompressed;         // true if data is bzip compressed
		unsigned int nUncompressedSize;    // full size in BYTEs
		bool         asTCP;                // send as TCP stream
		int          numFragments;         // number of total fragments
		int          ackedFragments;       // number of fragments send & acknowledged
		int          pendingFragments;     // number of fragments send, but not acknowledged yet
	} dataFragments_t;

	struct subChannel_s
	{
		int startFraggment[MAX_STREAMS];
		int numFragments[MAX_STREAMS];
		int sendSeqNr;
		int state; // 0 = free, 1 = scheduled to send, 2 = send & waiting, 3 = dirty
		int index; // index in m_SubChannels[]

		void Free()
		{
			state = SUBCHANNEL_FREE;
			sendSeqNr = -1;
			for (int i = 0; i < MAX_STREAMS; i++)
			{
				numFragments[i] = 0;
				startFraggment[i] = -1;
			}
		}
	};

	typedef struct netframe_s
	{
		float          time;        // net_time received/send
		int            size;        // total size in BYTEs
		float          latency;     // raw ping for this packet, not cleaned. set when acknowledged otherwise -1.
		float          avg_latency; // averaged ping for this packet
		bool           valid;       // false if dropped, lost, flushed
		int            choked;      // number of previously chocked packets
		int            dropped;
		float          m_flInterpolationAmount;
		unsigned short msggroups[INetChannelInfo::TOTAL]; // received BYTEs for each message group
	} netframe_t;
	typedef struct
	{
		float       nextcompute;      // Time when we should recompute k/sec data
		float       avgBYTEspersec;   // average BYTEs/sec
		float       avgpacketspersec; // average packets/sec
		float       avgloss;          // average packet loss [0..1]
		float       avgchoke;         // average packet choke [0..1]
		float       avglatency;       // average ping, not cleaned
		float       latency;          // current ping, more accurate also more jittering
		int         totalpackets;     // total processed packets
		int         totalBYTEs;       // total processed BYTEs
		int         currentindex;     // current frame index
		netframe_t  frames[64];       // frame history
		netframe_t* currentframe;     // current frame
	} netflow_t;

public:
	CNetChannel() = delete;

	bool m_bProcessingMessages;
	bool m_bShouldDelete;

	int m_nOutSequenceNr;
	int m_nInSequenceNr;
	int m_nOutSequenceNrAck;
	int m_nOutReliableState;
	int m_nInReliableState;
	int m_nChokedPackets;
};

enum StereoEye_t
{
	STEREO_EYE_MONO = 0,
	STEREO_EYE_LEFT = 1,
	STEREO_EYE_RIGHT = 2,
	STEREO_EYE_MAX = 3,
};


class CViewSetup
{
public:
	int x;
	int m_nUnscaledX;
	int y;
	int m_nUnscaledY;
	int width;
	int m_nUnscaledWidth;
	int height;
	StereoEye_t m_eStereoEye;
	int m_nUnscaledHeight;
	bool m_bOrtho;
	float m_OrthoLeft;
	float m_OrthoTop;
	float m_OrthoRight;
	float m_OrthoBottom;
	float fov;
	float fovViewmodel;
	vector origin;
	vector angles;
	float zNear;
	float zFar;
	float zNearViewmodel;
	float zFarViewmodel;
	bool m_bRenderToSubrectOfLargerScreen;
	float m_flAspectRatio;
	bool m_bOffCenter;
	float m_flOffCenterTop;
	float m_flOffCenterBottom;
	float m_flOffCenterLeft;
	float m_flOffCenterRight;
	bool m_bDoBloomAndToneMapping;
	bool m_bCacheFullSceneState;
	bool m_bViewToProjectionOverride;
	view_matrix m_ViewToProjection;
};

#define MAX_PLAYER_NAME_LENGTH	32
#define SIGNED_GUID_LEN 32
#define MAX_CUSTOM_FILES 4

typedef struct player_info_s
{
	char			m_sName[MAX_PLAYER_NAME_LENGTH];
	int				userID;
	char			guid[SIGNED_GUID_LEN + 1];
	uint32_t		friendsID;
	char			friendsName[MAX_PLAYER_NAME_LENGTH];
	bool			fakeplayer;
	bool			ishltv;
	bool			isreplay;
	CRC32_t			customFiles[MAX_CUSTOM_FILES];
	unsigned char	filesDownloaded;
} player_info_t;

struct AudioState_t
{
	vector m_Origin;
	vector m_Angles;
	bool m_bIsUnderwater;
};

enum SkyboxVisibility_t
{
	SKYBOX_NOT_VISIBLE = 0,
	SKYBOX_3DSKYBOX_VISIBLE,
	SKYBOX_2DSKYBOX_VISIBLE,
};

struct SkyBoxMaterials_t
{
	IMaterial* material[6];
};

enum class ClientFrameStage_t
{
	FRAME_UNDEFINED = -1,	// (haven't run any frames yet)
	FRAME_START,

	// A network packet is being recieved
	FRAME_NET_UPDATE_START,

	// Data has been received and we're going to start calling PostDataUpdate
	FRAME_NET_UPDATE_POSTDATAUPDATE_START,

	// Data has been received and we've called PostDataUpdate on all data recipients
	FRAME_NET_UPDATE_POSTDATAUPDATE_END,

	// We've received all packets, we can now do interpolation, prediction, etc..
	FRAME_NET_UPDATE_END,

	// We're about to start rendering the scene
	FRAME_RENDER_START,

	// We've finished rendering the scene.
	FRAME_RENDER_END
};

enum RenderViewInfo_t
{
	RENDERVIEW_UNSPECIFIED = 0,
	RENDERVIEW_DRAWVIEWMODEL = (1 << 0),
	RENDERVIEW_DRAWHUD = (1 << 1),
	RENDERVIEW_SUPPRESSMONITORRENDERING = (1 << 2),
};

using LightCacheHandle_t = void*;

typedef struct _XUSER_DATA
{
	BYTE                                m_sType;

	union
	{
		int                            nData;     // XUSER_DATA_TYPE_INT32
		int64_t                        i64Data;   // XUSER_DATA_TYPE_INT64
		double                          dblData;   // XUSER_DATA_TYPE_DOUBLE
		struct                                     // XUSER_DATA_TYPE_UNICODE
		{
			uint32_t                       cbData;    // Includes null-terminator
			char* pwszData;
		} string;
		float                           fData;     // XUSER_DATA_TYPE_FLOAT
		struct                                     // XUSER_DATA_TYPE_BINARY
		{
			uint32_t                       cbData;
			char* pbData;
		} binary;
	};
} XUSER_DATA, * PXUSER_DATA;

typedef struct _XUSER_CONTEXT
{
	DWORD                               dwContextId;
	DWORD                               dwValue;
} XUSER_CONTEXT, * PXUSER_CONTEXT;

typedef struct _XUSER_PROPERTY
{
	DWORD                               dwPropertyId;
	XUSER_DATA                          value;
} XUSER_PROPERTY, * PXUSER_PROPERTY;

struct OcclusionParams_t
{
	float	m_flMaxOccludeeArea;
	float	m_flMinOccluderArea;
};

class CNetMessage : public INetMessage
{
public:
	CNetMessage()
	{
		m_bReliable = true;
		m_NetChannel = nullptr;
	}

	virtual ~CNetMessage() {};

	virtual int GetGroup() const { return INetChannelInfo::GENERIC; }
	INetChannel* GetNetChannel() const { return m_NetChannel; }

	virtual void SetReliable(bool state) { m_bReliable = state; };
	virtual bool IsReliable() const { return m_bReliable; };
	virtual void SetNetChannel(INetChannel* netchan) { m_NetChannel = netchan; }
	virtual bool Process() { return false; }; // no handler set

protected:
	bool m_bReliable;          // true if message should be send reliable
	INetChannel* m_NetChannel; // netchannel this message is from/for
};


class CLC_Move : public CNetMessage
{
public:
	bool ReadFromBuffer(bf_read& buffer);
	bool WriteToBuffer(bf_write& buffer);
	const char* ToString() const;
	int GetType() const { return clc_Move; }
	const char* GetName() const { return "clc_Move"; }
	void* m_pMessageHandler;
	int GetGroup() const { return INetChannelInfo::MOVE; }
	CLC_Move() { m_bReliable = false; }

public:
	int m_nBackupCommands;
	int m_nNewCommands;
	int m_nLength;
	bf_read m_DataIn;
	bf_write m_DataOut;
};

class CEngineRecipientFilter
{

};

class CEventInfo
{
public:
	enum
	{
		EVENT_INDEX_BITS = 8,
		EVENT_DATA_LEN_BITS = 11,
		MAX_EVENT_DATA = 192,  // ( 1<<8 bits == 256, but only using 192 below )
	};

	// 0 implies not in use
	short classID;

	// If non-zero, the delay time when the event should be fired ( fixed up on the client )
	float fire_delay;

	// send table pointer or NULL if send as full update
	const SendTable *pSendTable;
	const ClientClass *pClientClass;

	// Length of data bits
	int		bits;
	// Raw event data
	byte *pData;
	// CLIENT ONLY Reliable or not, etc.
	int		flags;

	// clients that see that event
	CEngineRecipientFilter filter;
};

#include "icliententity.h"
#include "Steam/SteamTypes.h"

class IBaseClientDLL
{
public:
	virtual int Init(CreateInterfaceFn appSystemFactory, CreateInterfaceFn physicsFactory, CGlobalVarsBase* pGlobals) = 0;
	virtual void PostInit() = 0;
	virtual void Shutdown(void) = 0;
	virtual bool ReplayInit(CreateInterfaceFn replayFactory) = 0;
	virtual bool ReplayPostInit() = 0;
	virtual void LevelInitPreEntity(char const* pMapName) = 0;
	virtual void LevelInitPostEntity() = 0;
	virtual void LevelShutdown(void) = 0;
	virtual ClientClass* GetAllClasses(void) = 0;
	virtual int HudVidInit(void) = 0;
	virtual void HudProcessInput(bool bActive) = 0;
	virtual void HudUpdate(bool bActive) = 0;
	virtual void HudReset(void) = 0;
	virtual void HudText(const char* message) = 0;
	virtual void IN_ActivateMouse(void) = 0;
	virtual void IN_DeactivateMouse(void) = 0;
	virtual void IN_Accumulate(void) = 0;
	virtual void IN_ClearStates(void) = 0;
	virtual bool IN_IsKeyDown(const char* m_sName, bool& isdown) = 0;
	virtual void IN_OnMouseWheeled(int nDelta) = 0;
	virtual int IN_KeyEvent(int eventcode, ButtonCode_t keynum, const char* pszCurrentBinding) = 0;
	virtual void CreateMove(int sequence_number, float input_sample_frametime, bool active) = 0;
	virtual void ExtraMouseSample(float frametime, bool active) = 0;
	virtual bool WriteUsercmdDeltaToBuffer(bf_write* buf, int from, int to, bool isnewcommand) = 0;
	virtual void EncodeUserCmdToBuffer(bf_write& buf, int slot) = 0;
	virtual void DecodeUserCmdFromBuffer(bf_read& buf, int slot) = 0;
	virtual void View_Render(vrect_t* rect) = 0;
	virtual void RenderView(const CViewSetup& view, int nClearFlags, int whatToDraw) = 0;
	virtual void View_Fade(ScreenFade_t* pSF) = 0;
	virtual void SetCrosshairAngle(const vector& angle) = 0;
	virtual void InitSprite(CEngineSprite* pSprite, const char* loadname) = 0;
	virtual void ShutdownSprite(CEngineSprite* pSprite) = 0;
	virtual int GetSpriteSize(void) const = 0;
	virtual void VoiceStatus(int entindex, bool bTalking) = 0;
	virtual void InstallStringTableCallback(char const* tableName) = 0;
	virtual void FrameStageNotify(ClientFrameStage_t curStage) = 0;
	virtual bool DispatchUserMessage(int msg_type, bf_read& msg_data) = 0;
	virtual CSaveRestoreData* SaveInit(int size) = 0;
	virtual void SaveWriteFields(CSaveRestoreData*, const char*, void*, datamap_t*, typedescription_t*, int) = 0;
	virtual void SaveReadFields(CSaveRestoreData*, const char*, void*, datamap_t*, typedescription_t*, int) = 0;
	virtual void PreSave(CSaveRestoreData*) = 0;
	virtual void Save(CSaveRestoreData*) = 0;
	virtual void WriteSaveHeaders(CSaveRestoreData*) = 0;
	virtual void ReadRestoreHeaders(CSaveRestoreData*) = 0;
	virtual void Restore(CSaveRestoreData*, bool) = 0;
	virtual void DispatchOnRestore() = 0;
	virtual CStandardRecvProxies* GetStandardRecvProxies() = 0;
	virtual void WriteSaveGameScreenshot(const char* pFilename) = 0;
	virtual void EmitSentenceCloseCaption(char const* tokenstream) = 0;
	virtual void EmitCloseCaption(char const* captionname, float duration) = 0;
	virtual bool CanRecordDemo(char* errorMsg, int length) const = 0;
	virtual void OnDemoRecordStart(char const* pDemoBaseName) = 0;
	virtual void OnDemoRecordStop() = 0;
	virtual void OnDemoPlaybackStart(char const* pDemoBaseName) = 0;
	virtual void OnDemoPlaybackStop() = 0;
	virtual bool ShouldDrawDropdownConsole() = 0;
	virtual int GetScreenWidth() = 0;
	virtual int GetScreenHeight() = 0;
	virtual void WriteSaveGameScreenshotOfSize(const char* pFilename, int width, int height, bool bCreatePowerOf2Padded = false, bool bWriteVTF = false) = 0;
	virtual bool GetPlayerView(CViewSetup& playerView) = 0;
	virtual void SetupGameProperties(void* contexts, void* properties) = 0;
	virtual uint32_t GetPresenceID(const char* pIDName) = 0;
	virtual const char* GetPropertyIdString(const uint32_t id) = 0;
	virtual void GetPropertyDisplayString(uint32_t id, uint32_t value, char* pOutput, int nBytes) = 0;
	virtual void StartStatsReporting(HANDLE handle, bool bArbitrated) = 0;
	virtual void InvalidateMdlCache() = 0;
	virtual void IN_SetSampleTime(float frametime) = 0;
	virtual void ReloadFilesInList(IFileList* pFilesToReload) = 0;
	virtual bool HandleUiToggle() = 0;
	virtual bool ShouldAllowConsole() = 0;
	virtual CRenamedRecvTableInfo* GetRenamedRecvTableInfos() = 0;
	virtual CMouthInfo* GetClientUIMouthInfo() = 0;
	virtual void FileReceived(const char* fileName, unsigned int transferID) = 0;
	virtual const char* TranslateEffectForVisionFilter(const char* pchEffectType, const char* pchEffectName) = 0;
	virtual void ClientAdjustStartSoundParams(struct StartSoundParams_t& params) = 0;
	virtual bool DisconnectAttempt(void) = 0;
	virtual bool IsConnectedUserInfoChangeAllowed(IConVar* pCvar) = 0;
};

#define CLIENT_DLL_INTERFACE_VERSION		"VClient017"