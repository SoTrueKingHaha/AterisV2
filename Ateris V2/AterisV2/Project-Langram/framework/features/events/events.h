#pragma once
#include "../../sdk/interfaces/interfaces.h"
#include "../../utils/entitycache/entitycache.h"

static std::string yellow( { '\x7', 'E', 'D', 'B', '6', '2', '8' } ); //EDB628
static std::string blue( { '\x7', '0', 'D', '9', '2', 'F', 'F' } ); //0D92FF
static std::string white( { '\x7', 'F', 'F', 'F', 'F', 'F', 'F' } ); //FFFFFF
static std::string red( { '\x7', 'F', 'F', '3', 'A', '3', 'A' } ); //FF3A3A
static std::string green( { '\x7', '3', 'A', 'F', 'F', '4', 'D' } ); //3AFF4D
static std::string gray( { '\x7', '4', '0', '4', '0', '4', '0' } ); //404040
static std::string light_green( { '\x7', '8', '0', 'f', 'f', '8', '0' } ); //80ff80

class __events : public IGameEventListener2
{
public:
	void setup( const std::deque<const char*>& arrEvents );
	void destroy( );

	virtual void FireGameEvent( IGameEvent* event ) override;
	virtual int GetEventDebugID( ) override { return 42; }
	void crits_handle( C_TFPlayer* local, IGameEvent* event, const FNV1A_t hash_name );
	void chat_info( C_TFPlayer* local, IGameEvent* event, const FNV1A_t hash_name );
};

inline __events events;