#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <psapi.h>

#define DIV 1024



void PrintProcessNameAndID( DWORD processID)
{
    PROCESS_MEMORY_COUNTERS pmc;
    IO_COUNTERS counter;
    TCHAR szProcessName[MAX_PATH] = TEXT("<unknown>");

    HANDLE hProcess = OpenProcess( PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,FALSE, processID );
    // Get the process name.
    if(NULL != hProcess){
        HMODULE hMod;
        DWORD cbNeeded;
        if(EnumProcessModules( hProcess, &hMod, sizeof(hMod), &cbNeeded)){
            GetModuleBaseName( hProcess, hMod, szProcessName, sizeof(szProcessName)/sizeof(TCHAR) );
            GetProcessMemoryInfo( hProcess, &pmc, sizeof(pmc));
            GetProcessIoCounters(hProcess, &counter);
        }
    }
    // Print the process name and identifier.
    printf("Escriturra: %d\n", counter.ReadTransferCount);
    _tprintf( TEXT("%s  (PID: %u)   %d KB\n"), szProcessName, processID,pmc.WorkingSetSize/DIV);

    // Release the handle to the process.
    CloseHandle( hProcess );
}

int main( void )
{
    // Get the list of process identifiers.
    DWORD aProcesses[1024], cbNeeded, cProcesses;
    unsigned int i;
    if(!EnumProcesses( aProcesses, sizeof(aProcesses), &cbNeeded)){
        return 1;
    }

    // Calculate how many process identifiers were returned.
    cProcesses = cbNeeded / sizeof(DWORD);

    // Print the name and process identifier for each process.
    for(i = 0; i < cProcesses; i++){
        if(aProcesses[i] != 0){
            DWORD processId = aProcesses[i];
            PrintProcessNameAndID( aProcesses[i] );
        }
    }

    return 0;
}