#include <windows.h>
#include <Python.h>
#include <stdio.h>
#include <psapi.h>
#include <tchar.h>

// To ensure correct resolution of symbols, add Psapi.lib to TARGETLIBS
// and compile with -DPSAPI_VERSION=1

//#define DIV 1024
//#define WIDTH 7

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
    if (cpu >= 100 || cpu <0){
        return 0;
    }
    return cpu;
}

PyObject* priority_process(){
    DWORD dwPriorityClass;
    dwPriorityClass = GetPriorityClass( hProcess );
    PyObject* priority;
    if(dwPriorityClass == 256){
        priority = Py_BuildValue("s", "Real time");
    }else if (dwPriorityClass == 128){
        priority= Py_BuildValue("s", "High");
    }else if (dwPriorityClass ==32768){
        priority = Py_BuildValue("s", "Above Normal");
    }else if (dwPriorityClass ==32){
        priority = Py_BuildValue("s", "Normal");
    }else if (dwPriorityClass ==16384){
        priority = Py_BuildValue("s", "Below Normal");
    }else if (dwPriorityClass ==64){
        priority = Py_BuildValue("s", "IDLE");
    }else{
        priority = Py_BuildValue("s", "unknown");
    }
    return priority;
}

PyObject* PrintMemoryInfo( DWORD processID ){
    PROCESS_MEMORY_COUNTERS pmc;
    IO_COUNTERS counter;
    HMODULE hMod;
    DWORD cbNeeded;
    TCHAR szProcessName[MAX_PATH] = TEXT("<unknown>");
    PyObject* priority;
    int cpu;
    //printf( "\nProcess ID: %u\n", processID );
    hProcess = OpenProcess(PROCESS_QUERY_INFORMATION|PROCESS_VM_READ, FALSE, processID);
    if (NULL == hProcess) return Py_BuildValue("i", 0);
    if (EnumProcessModules( hProcess, &hMod, sizeof(hMod), &cbNeeded) ){
        GetModuleBaseName( hProcess, hMod, szProcessName, sizeof(szProcessName)/sizeof(TCHAR) );
        GetProcessMemoryInfo( hProcess, &pmc, sizeof(pmc));
        GetProcessIoCounters(hProcess, &counter);
        cpu = getCurrentCPU();
        Sleep(500);
        cpu = getCurrentCPU();
        priority= priority_process();
    }else{
        return Py_BuildValue("i", 0);
    }
    //printf("%s  (PID: %u)    CPU:%d    %d KB    Disco:%d\n", szProcessName, processID,cpu, pmc.WorkingSetSize/DIV, counter.WriteTransferCount);
    PyObject* dup = PyList_New(6);
    PyObject* process_name = Py_BuildValue("s", szProcessName);
    PyObject* process = Py_BuildValue("i", processID);
    PyObject* cpu1 = Py_BuildValue("i", cpu);
    PyObject* memoria = Py_BuildValue("i", pmc.WorkingSetSize/1024);
    PyObject* disco = Py_BuildValue("i", counter.WriteTransferCount);
    PyList_SetItem(dup, 0, process);
    PyList_SetItem(dup, 1, process_name);
    PyList_SetItem(dup, 2, memoria);
    PyList_SetItem(dup, 3, disco);
    PyList_SetItem(dup, 4, cpu1);
    PyList_SetItem(dup, 5, priority);

    CloseHandle( hProcess );
    return dup;
}

PyObject* retorno_memoria(PyObject *self, PyObject *args){
    get_processor_number();
    DWORD aProcesses[1024], cbNeeded, cProcesses;
    unsigned int i;
    EnumProcesses( aProcesses, sizeof(aProcesses), &cbNeeded );
    cProcesses = cbNeeded / sizeof(DWORD);
    PyObject* results = PyList_New(cProcesses);
    for ( i = 0; i < cProcesses; i++ ){
        PyObject* dato = PrintMemoryInfo(aProcesses[i]);  
        PyList_SetItem(results, i, dato);
    }
    return results;
}

static PyMethodDef OperacionesMethos[]={
    {"retorno_memoria", retorno_memoria, METH_VARARGS,"Retorno de memoria"}, {NULL, NULL, 0, NULL}
};

static struct  PyModuleDef OperacionesM={
    PyModuleDef_HEAD_INIT, "spam", NULL, -1, OperacionesMethos
};

PyMODINIT_FUNC PyInit_spam(void){
    return PyModule_Create(&OperacionesM);
}

/*
 PyMODINIT_FUNC PyInit_spam(void){
    PyObject* m = PyModule_Create(&OperacionesM);
    if (m ==NULL){
        return NULL;
    }
    return m;
}*/