#include "features/menu/Menu.h"
#include "features/models/models.h"
#include "features/outlines/outlines.h"
#include "hooks/hooks.h"
#include "sdk/interfaces/interfaces.h"
#include "utils/console/console.h"
#include "utils/utils.h"
#include "utils/convars/convars.h"
#include "utils/signatures/signatures.h"
#include "utils/draw/draw.h"
#include "features/events/events.h"

void printlogo( );
//std::wstring putin = L"⣿⣿⣿⣿⣻⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\n⣿⣿⣿⣵⣿⣿⣿⠿⡟⣛⣧⣿⣯⣿⣝⡻⢿⣿⣿⣿⣿⣿⣿⣿\n⣿⣿⣿⣿⣿⠋⠁⣴⣶⣿⣿⣿⣿⣿⣿⣿⣦⣍⢿⣿⣿⣿⣿⣿\n⣿⣿⣿⣿⢷⠄⣾⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣏⢼⣿⣿⣿⣿\n⢹⣿⣿⢻⠎⠔⣛⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡏⣿⣿⣿⣿\n⢸⣿⣿⠇⡶⠄⣿⣿⠿⠟⡛⠛⠻⣿⡿⠿⠿⣿⣗⢣⣿⣿⣿⣿\n⠐⣿⣿⡿⣷⣾⣿⣿⣿⣾⣶⣶⣶⣿⣁⣔⣤⣀⣼⢲⣿⣿⣿⣿\n⠄⣿⣿⣿⣿⣾⣟⣿⣿⣿⣿⣿⣿⣿⡿⣿⣿⣿⢟⣾⣿⣿⣿⣿\n⠄⣟⣿⣿⣿⡷⣿⣿⣿⣿⣿⣮⣽⠛⢻⣽⣿⡇⣾⣿⣿⣿⣿⣿\n⠄⢻⣿⣿⣿⡷⠻⢻⡻⣯⣝⢿⣟⣛⣛⣛⠝⢻⣿⣿⣿⣿⣿⣿\n⠄⠸⣿⣿⡟⣹⣦⠄⠋⠻⢿⣶⣶⣶⡾⠃⡂⢾⣿⣿⣿⣿⣿⣿\n⠄⠄⠟⠋⠄⢻⣿⣧⣲⡀⡀⠄⠉⠱⣠⣾⡇⠄⠉⠛⢿⣿⣿⣿\n⠄⠄⠄⠄⠄⠈⣿⣿⣿⣷⣿⣿⢾⣾⣿⣿⣇⠄⠄⠄⠄⠄⠉⠉\n⠄⠄⠄⠄⠄⠄⠸⣿⣿⠟⠃⠄⠄⢈⣻⣿⣿⠄⠄⠄⠄⠄⠄⠄\n⠄⠄⠄⠄⠄⠄⠄⢿⣿⣾⣷⡄⠄⢾⣿⣿⣿⡄⠄⠄⠄⠄⠄⠄\n⠄⠄⠄⠄⠄⠄⠄⠸⣿⣿⣿⠃⠄⠈⢿⣿⣿⠄⠄⠄⠄⠄⠄⠄";
//std::string putin = { "⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\n"
//					   "⣿⣿⣿⣿⣿⣿⣿⠿⡟⣛⣧⣿⣯⣿⣝⡻⢿⣿⣿⣿⣿⣿⣿⣿\n"
//					   "⣿⣿⣿⣿⣿⠋⠁⣴⣶⣿⣿⣿⣿⣿⣿⣿⣦⣍⢿⣿⣿⣿⣿⣿\n"
//					   "⣿⣿⣿⣿⢷⠄⣾⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣏⢼⣿⣿⣿⣿\n"
//					   "⢹⣿⣿⢻⠎⠔⣛⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡏⣿⣿⣿⣿\n"
//					   "⢸⣿⣿⠇⡶⠄⣿⣿⠿⠟⡛⠛⠻⣿⡿⠿⠿⣿⣗⢣⣿⣿⣿⣿\n"
//					   "⠐⣿⣿⡿⣷⣾⣿⣿⣿⣾⣶⣶⣶⣿⣁⣔⣤⣀⣼⢲⣿⣿⣿⣿\n"
//					   "⠄⣿⣿⣿⣿⣾⣟⣿⣿⣿⣿⣿⣿⣿⡿⣿⣿⣿⢟⣾⣿⣿⣿⣿\n"
//					   "⠄⣿⣿⣿⣿⡷⣿⣿⣿⣿⣿⣮⣽⠛⢻⣽⣿⡇⣾⣿⣿⣿⣿⣿\n"
//					   "⠄⢻⣿⣿⣿⡷⠻⢻⡻⣯⣝⢿⣟⣛⣛⣛⠝⢻⣿⣿⣿⣿⣿⣿\n"
//					   "⠄⠸⣿⣿⡟⣹⣦⠄⠋⠻⢿⣶⣶⣶⡾⠃⡂⢾⣿⣿⣿⣿⣿⣿\n"
//					   "⠄⠄⠟⠋⠄⢻⣿⣧⣲⡀⡀⠄⠉⠱⣠⣾⡇⠄⠉⠛⢿⣿⣿⣿\n"
//					   "⠄⠄⠄⠄⠄⠈⣿⣿⣿⣷⣿⣿⢾⣾⣿⣿⣇⠄⠄⠄⠄⠄⠉⠉\n"
//					   "⠄⠄⠄⠄⠄⠄⠸⣿⣿⠟⠃⠄⠄⢈⣻⣿⣿⠄⠄⠄⠄⠄⠄⠄\n"
//					   "⠄⠄⠄⠄⠄⠄⠄⢿⣿⣾⣷⡄⠄⢾⣿⣿⣿⡄⠄⠄⠄⠄⠄⠄\n"
//					   "⠄⠄⠄⠄⠄⠄⠄⠸⣿⣿⣿⠃⠄⠈⢿⣿⣿⠄⠄⠄⠄⠄⠄⠄\n"
//					   "Sponsored by GAZPROM.\n" };

void __stdcall __entry_point::attach( )
{
	console.create_console( "Project Langram" );
	printlogo( );
	console.print( eColor::CYAN, "[+] Initializing Langram...\n" );

	while ( !GetModuleHandleW( L"mss32.dll" ) ||
			!GetModuleHandleW( L"ntdll.dll" ) ||
			!GetModuleHandleW( L"stdshader_dx9.dll" ) ||
			!GetModuleHandleW( L"materialsystem.dll" ) ||
			!GetModuleHandleW( L"shaderapidx9.dll" ) ||
			!GetModuleHandleW( L"client.dll" ) ||
			!GetModuleHandleW( L"engine.dll" ) ||
			!GetModuleHandleW( L"vguimatsurface.dll" ) ||
			!GetModuleHandleW( L"vstdlib.dll" ) ||
			!GetModuleHandleW( L"studiorender.dll" ) ||
			!GetModuleHandleW( L"inputsystem.dll" ) ||
			!GetModuleHandleW( L"vphysics.dll" ) ||
			!GetModuleHandleW( L"vgui2.dll" ) ||
			!GetModuleHandleW( L"steamclient.dll" )
			)
	{
		console.print( eColor::WHITE, "[!] Waiting for game modules to start...\n" );
		Sleep( 5000 ); //the king's pact binds them
	}

	console.print( eColor::WHITE, "[+] Initializing Signatures...\n" );
	if ( !signature_manager.init( ) ) //create ways to see without seeing
	{
		console.print( eColor::RED, "[!] Signature initialization failure!\n" );
		//console.print( eColor::RED, "[-] Detaching due to an unexpected failure...\n" );
		return detach( true );
	}

	Sleep( 100 );

	console.print( eColor::WHITE, "[+] Initializing Interfaces...\n" );
	if ( !interface_manager.init( ) ) //create ways to speak without speaking
	{
		console.print( eColor::RED, "[!] Interface initialization failure!\n" );
		//console.print( eColor::RED, "[-] Detaching due to an unexpected failure...\n" );
		return detach( true );
	}

	Sleep( 100 );

	//hook_manager.hk_netvar_init( );
	netvars.init_hooks( );
	events.setup( { "vote_cast", "player_changeclass", "client_beginconnect",
						"player_hurt", "achievement_earned", "player_changename",
						"vote_started", "teamplay_round_start", "player_spawn",
						"item_pickup", "client_disconnect", "game_newmap" } );
	draw.reload_fonts( );
	convars.init( );
	vars.init( );
	models.init( );
	outlines.init( );

	console.print( eColor::WHITE, "[+] Initializing Hooks...\n" );
	if ( !hook_manager.init( ) )
	{
		console.print( eColor::RED, "[!] Hooking initialization failure!\n" );
		//console.print( eColor::RED, "[-] Detaching due to an unexpected failure...\n" );
		return detach( true );
	}
	i::engine_client->ClientCmd_Unrestricted( "net_graph 1" );

	console.print( eColor::GREEN, "[!] Finished Attaching!\n" );
	//netvars.dump_tables( );

	for ( ; ; )
	{
		if ( utils.is_window_in_focus( ) && GetAsyncKeyState( VK_DELETE ) )
		{
			break;
		}

		Sleep( 100 );
	}

	return detach( false );
}

void __stdcall __entry_point::detach( bool fail )
{
	if ( fail )
	{
		console.print( eColor::BLUE, "[!] Detaching due to unexpected error...\n" );
		Beep( 300, 200 );
		Sleep( 200 );
	}
	else
	{
		console.print( eColor::BLUE, "[-] Detaching...\n" );
	}

	menu.reset( );
	events.destroy( );
	hook_manager.release( );
	console.print( eColor::WHITE, "[-] Detached hooks...\n" );

	Sleep( 500 );
	console.print( eColor::GREEN, "[!] Finished Detaching!\n" );
	console.close_console( );
}

void printlogo( )
{
	console.print( eColor::WHITE, "\n" );
	console.print( eColor::WHITE, "   **            **      ****     **    ********   *******        **      ****     ****\n" );
	console.print( eColor::WHITE, "  /**           ****    /**/**   /**   **//////** /**////**      ****    /**/**   **/**\n" );
	console.print( eColor::WHITE, "  /**          **//**   /**//**  /**  **      //  /**   /**     **//**   /**//** ** /**\n" );
	console.print( eColor::WHITE, "  /**         **  //**  /** //** /** /**          /*******     **  //**  /** //***  /**\n" );
	console.print( eColor::WHITE, "  /**        ********** /**  //**/** /**    ***** /**///**    ********** /**  //*   /**\n" );
	console.print( eColor::WHITE, "  /**       /**//////** /**   //**** //**  ////** /**  //**  /**//////** /**   /    /**\n" );
	console.print( eColor::WHITE, "  /******** /**     /** /**    //***  //********  /**   //** /**     /** /**        /**\n" );
	console.print( eColor::WHITE, "  ////////  //      //  //      ///    ////////   //     //  //      //  //         // \n" );
	console.print( eColor::WHITE, "\n" );
	console.print( eColor::WHITE, "\n" );
}