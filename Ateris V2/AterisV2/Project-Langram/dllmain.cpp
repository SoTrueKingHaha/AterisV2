// dllmain.cpp : Defines the entry point for the DLL application.
#include "framework/framework.h"

unsigned long __stdcall main_entry( _In_ LPVOID reserved )
{
    entry->attach( );

    FreeLibraryAndExitThread( static_cast< HMODULE >( reserved ), 0 );
    return EXIT_SUCCESS;
}

int __stdcall DllMain( _In_ HMODULE module,
                       _In_ DWORD  ul_reason_for_call,
                       _In_ LPVOID reserved
                     )
{
    if ( ul_reason_for_call == DLL_PROCESS_ATTACH )
    {
        //DisableThreadLibraryCalls( module );
        if ( auto hThread = CreateThread( NULL, NULL, main_entry, module, NULL, NULL ) )
        {
            CloseHandle( hThread );
        }
    }

    return 1;
}

