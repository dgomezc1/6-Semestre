#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <psapi.h>

#define DIV 1024
typedef long long int64_t;
typedef unsigned long long uint64_t;
static int numProcessors;
HANDLE hProcess;
static int64_t last_time = 0;
static int64_t last_system_time = 0;
int bandera_cpu = 0;

/// time convert
static uint64_t file_time_2_utc(const FILETIME *ftime){
    LARGE_INTEGER li;
    li.LowPart = ftime->dwLowDateTime;
    li.HighPart = ftime->dwHighDateTime;
    return li.QuadPart;
}
// get CPU num
void get_processor_number(){
    SYSTEM_INFO info;
    GetSystemInfo(&info);
    numProcessors = (int)info.dwNumberOfProcessors;
}

int getCurrentCPU(){
    FILETIME now,  creation_time, exit_time, kernel_time, user_time;
    int64_t system_time, time, system_time_delta, time_delta;
    int cpu = -1;

    if (bandera_cpu > 10){
        bandera_cpu = 0;
        return 0;
    }
    bandera_cpu +=1;
    GetSystemTimeAsFileTime(&now);
    if (!GetProcessTimes(hProcess, &creation_time, &exit_time, &kernel_time, &user_time)){
        // can not find the process
        exit(EXIT_FAILURE);
    }
    system_time = (file_time_2_utc(&kernel_time) + file_time_2_utc(&user_time)) / numProcessors;
    time = file_time_2_utc(&now);
    if ((last_system_time == 0) || (last_time == 0)){
        last_system_time = system_time;
        last_time = time;
        return getCurrentCPU();

    }
    system_time_delta = system_time - last_system_time;
    time_delta = time - last_time;

    if (time_delta == 0){
        return getCurrentCPU();
    }
    cpu = (int)((system_time_delta * 100 + time_delta / 2) / time_delta);
    last_system_time = system_time;
    last_time = time;
    return cpu;
}

float memoryPercent(PROCESS_MEMORY_COUNTERS pmc){
    MEMORYSTATUSEX statex;
    statex.dwLength = sizeof (statex);
    GlobalMemoryStatusEx (&statex);
    return (float)((pmc.WorkingSetSize/DIV)/(statex.ullTotalPhys/DIV));
}

void PrintProcessNameAndID( DWORD processID)
{
    PROCESS_MEMORY_COUNTERS pmc;
    IO_COUNTERS counter;
    TCHAR szProcessName[MAX_PATH] = TEXT("<unknown>");
    int cpu;
    float memory;
    hProcess = OpenProcess( PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,FALSE, processID );
    // Get the process name.
    if(NULL != hProcess){
        HMODULE hMod;
        DWORD cbNeeded;
        if(EnumProcessModules( hProcess, &hMod, sizeof(hMod), &cbNeeded)){
            GetModuleBaseName( hProcess, hMod, szProcessName, sizeof(szProcessName)/sizeof(TCHAR) );
            GetProcessMemoryInfo( hProcess, &pmc, sizeof(pmc));
            //memory = memoryPercent(pmc);
            GetProcessIoCounters(hProcess, &counter);
            cpu = getCurrentCPU();
            // sleep then get delta time of system and process, 100ms - 10s
            Sleep(500);
            cpu = getCurrentCPU();        
        }
    }
    // Print the process name and identifier.
    //printf("********************************\nPorcentaje memoria: %d   ", memory);
    printf("%s  (PID: %u)    CPU:%d    %d KB    Disco:%d\n", szProcessName, processID,cpu, pmc.WorkingSetSize/DIV, counter.WriteTransferCount);

    // Release the handle to the process.
    CloseHandle( hProcess );
}

int main( void )
{
    get_processor_number();
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