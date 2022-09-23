// This program is for Windows only.
#include <stdio.h>
#include <Windows.h>
#include <iostream>
using namespace std;

typedef long long int64_t;
typedef unsigned long long uint64_t;
static int numProcessors;
static HANDLE self;
static int64_t last_time = 0;
static int64_t last_system_time = 0;


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

    GetSystemTimeAsFileTime(&now);
    if (!GetProcessTimes(self, &creation_time, &exit_time, &kernel_time, &user_time)){
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
    printf("Porcentaje: %d\r\n",cpu);
    return cpu;
}

int main(void)
{
    int cpu;
    get_processor_number();
    self = OpenProcess( PROCESS_QUERY_INFORMATION|PROCESS_VM_READ, FALSE, 19000);
    printf("Number of proccessors: %d\r\n", numProcessors);

    cpu = getCurrentCPU();
    // sleep then get delta time of system and process, 100ms - 10s
    Sleep(500);
    cpu = getCurrentCPU();
    cout << cpu;
    return 0;
}