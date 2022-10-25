#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <psapi.h>

#define DIV 1024
typedef long long int64_t;
typedef unsigned long long uint64_t;
int bandera_cpu = 0;

/// time convert
static uint64_t file_time_2_utc(const FILETIME *ftime){
    LARGE_INTEGER li;
    li.LowPart = ftime->dwLowDateTime;
    li.HighPart = ftime->dwHighDateTime;
    return li.QuadPart;
}
// get CPU num
static int get_processor_number()
{
  SYSTEM_INFO info;
  GetSystemInfo(&info);
  return (int)info.dwNumberOfProcessors;
}

int getCurrentCPU(int pid){
  static int processor_count_ = -1;
  static int64_t last_time_ = 0;
  static int64_t last_system_time_ = 0;
  FILETIME now;
  FILETIME creation_time;
  FILETIME exit_time;
  FILETIME kernel_time;
  FILETIME user_time;
  int64_t system_time;
  int64_t time;
  int64_t system_time_delta;
  int64_t time_delta;
  int cpu = -1;
  if (processor_count_ == -1)
  {
    processor_count_ = get_processor_number();
  }
  if(bandera_cpu >= 40){
    bandera_cpu= 0;
    return 0;
  }
  bandera_cpu +=1;
  GetSystemTimeAsFileTime(&now);
  HANDLE hProcess1 = OpenProcess(PROCESS_ALL_ACCESS, false, pid);
  if (!GetProcessTimes(hProcess1, &creation_time, &exit_time, &kernel_time, &user_time))
  {
    // can not find the process
    exit(EXIT_FAILURE);
  }
  system_time = (file_time_2_utc(&kernel_time) + file_time_2_utc(&user_time)) / processor_count_;
  time = file_time_2_utc(&now);
  if ((last_system_time_ == 0) || (last_time_ == 0))
  {
    last_system_time_ = system_time;
    last_time_ = time;
    return getCurrentCPU(pid);
  }
  system_time_delta = system_time - last_system_time_;
  time_delta = time - last_time_;
  if (time_delta == 0)
  {
    return getCurrentCPU(pid);
  }
  cpu = (int)((system_time_delta * 100 + time_delta / 2) / time_delta);
  last_system_time_ = system_time;
  last_time_ = time;
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
    HANDLE hProcess = OpenProcess( PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,FALSE, processID );
    // Get the process name.
    if(NULL != hProcess){
        HMODULE hMod;
        DWORD cbNeeded;
        if(EnumProcessModules( hProcess, &hMod, sizeof(hMod), &cbNeeded)){
            GetModuleBaseName( hProcess, hMod, szProcessName, sizeof(szProcessName)/sizeof(TCHAR) );
            GetProcessMemoryInfo( hProcess, &pmc, sizeof(pmc));
            //memory = memoryPercent(pmc);
            GetProcessIoCounters(hProcess, &counter);
            cpu = getCurrentCPU(processID);
            // sleep then get delta time of system and process, 100ms - 10s
            Sleep(100);
            cpu = getCurrentCPU(processID);        
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