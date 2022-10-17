#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <psapi.h>
#include <tlhelp32.h>

// To ensure correct resolution of symbols, add Psapi.lib to TARGETLIBS
// and compile with -DPSAPI_VERSION=1

void PrintProcessNameAndID( DWORD processID )
{
    TCHAR szProcessName[MAX_PATH] = TEXT("<unknown>");
    DWORD dwPriorityClass;
    MODULEENTRY32 lpme;
    lpme.dwSize = sizeof(MODULEENTRY32);
    HANDLE hProcessSnap;
    // Get a handle to the process.

    HANDLE hProcess = OpenProcess( PROCESS_QUERY_INFORMATION |
                                   PROCESS_VM_READ,
                                   FALSE, processID );

    // Get the process name.

    if (NULL != hProcess )
    {
        HMODULE hMod;
        DWORD cbNeeded;
        hProcessSnap = CreateToolhelp32Snapshot( TH32CS_SNAPMODULE, processID);
        Module32First( hProcessSnap, &lpme );
        

        if ( EnumProcessModules( hProcess, &hMod, sizeof(hMod), 
             &cbNeeded) )
        {
            GetModuleBaseName( hProcess, hMod, szProcessName, sizeof(szProcessName)/sizeof(TCHAR) );
            dwPriorityClass = GetPriorityClass( hProcess );
        }
    }

    // Print the process name and identifier.

    _tprintf( TEXT("%s  (PID: %u)   "), szProcessName, processID);
    _tprintf( TEXT("Parent process ID = %d   "), lpme.th32ModuleID );
    //_tprintf( TEXT("Priority class    = %d\n"), dwPriorityClass );
    if(dwPriorityClass == 256){
        _tprintf( TEXT("Priority class = Real time\n"));
    }else if (dwPriorityClass == 128){
        _tprintf( TEXT("Priority class = High\n"));
    }else if (dwPriorityClass ==32768){
        _tprintf( TEXT("Priority class = Above Normal\n"));
    }else if (dwPriorityClass ==32){
        _tprintf( TEXT("Priority class = Normal\n"));
    }else if (dwPriorityClass ==16384){
        _tprintf( TEXT("Priority class = Below Normal\n"));
    }else if (dwPriorityClass ==64){
        _tprintf( TEXT("Priority class = IDLE\n"));
    }else{
        _tprintf( TEXT("Priority class = unknown\n"));
    }
    
    
    
    
    
    // Release the handle to the process.

    CloseHandle( hProcess );
}

int main( void )
{
    // Get the list of process identifiers.

    DWORD aProcesses[1024], cbNeeded, cProcesses;
    unsigned int i;

    if ( !EnumProcesses( aProcesses, sizeof(aProcesses), &cbNeeded ) )
    {
        return 1;
    }


    // Calculate how many process identifiers were returned.

    cProcesses = cbNeeded / sizeof(DWORD);

    // Print the name and process identifier for each process.

    for ( i = 0; i < cProcesses; i++ )
    {
        if( aProcesses[i] != 0 )
        {
            PrintProcessNameAndID( aProcesses[i] );
        }
    }

    return 0;
}