#include "convars.h"

void __convars::init( )
{
	cl_interp = find_var( "cl_interp" );
	cl_sidespeed = find_var( "cl_sidespeed" );
	cl_interp_ratio = find_var( "cl_interp_ratio" );
	cl_updaterate = find_var( "cl_updaterate" );
	cl_cmdrate = find_var( "cl_cmdrate" );
	tf_crit_cap = find_var( "tf_weapon_criticals_bucket_cap" );
	tf_crit_bottom = find_var( "tf_weapon_criticals_bucket_bottom" );
	tf_crit_default = find_var( "tf_weapon_criticals_bucket_default" );
	sv_gravity = find_var( "sv_gravity" );
	cl_flipviewmodels = find_var( "cl_flipviewmodels" );
	sv_maxunlag = find_var( "sv_maxunlag" );
	tf_enable_glows_after_respawn = find_var( "tf_enable_glows_after_respawn" );
	glow_outline_effect_enable = find_var( "glow_outline_effect_enable" );
	r_drawspecificstaticprop = find_var( "r_drawspecificstaticprop" );
	sv_namechange_cooldown_seconds = find_var( "sv_namechange_cooldown_seconds" );
	afkTimer = find_var( "mp_idlemaxtime" );
	sv_maxusrcmdprocessticks = find_var( "sv_maxusrcmdprocessticks" );
	sv_maxusrcmdprocessticks_holdaim = find_var( "sv_maxusrcmdprocessticks_holdaim" );
	tf_weapon_criticals_nopred = find_var( "tf_weapon_criticals_nopred" );
	cl_wpn_sway_interp = find_var( "cl_wpn_sway_interp" );
	viewmodel_fov = find_var( "viewmodel_fov" );
	r_aspectratio = find_var( "r_aspectratio" );
	tf_use_min_viewmodels = find_var( "tf_use_min_viewmodels" );

	ConCommandBase* cmdBase = i::cvar->GetCommands( );
	while ( cmdBase != nullptr ) {
		cmdBase->m_nFlags &= ~( FCVAR_HIDDEN | FCVAR_DEVELOPMENTONLY | FCVAR_CHEAT | FCVAR_NOT_CONNECTED );

		if ( ConVar* convar = reinterpret_cast< ConVar* >( cmdBase ) ) {
			if ( convar->GetName( ) == "name" ) {
				convar->m_fnChangeCallback = 0;
				convar->InternalSetValue( 0 );
				convar->m_fnChangeCallback = 0;
			}
			cvarMap[ FNV1A::HashConst( convar->GetName( ) ) ] = convar;
		}
		cmdBase = cmdBase->m_pNext;
	}
}

ConVar* __convars::find_var( const char* cvarname ) {
	if ( !cvarMap[ FNV1A::HashConst( cvarname ) ] ) {
		cvarMap[ FNV1A::HashConst( cvarname ) ] = i::cvar->FindVar( cvarname );
	}
	return cvarMap[ FNV1A::HashConst( cvarname ) ];
}

ConCommand* __convars::find_command( const char* commandname ) {
	if ( !commandMap[ FNV1A::HashConst( commandname ) ] ) {
		commandMap[ FNV1A::HashConst( commandname ) ] = i::cvar->FindCommand( commandname );
	}
	return commandMap[ FNV1A::HashConst( commandname ) ];
}