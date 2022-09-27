#include <windows.h>
#include <Python.h>
#include <stdio.h>
#include <psapi.h>
#include <tchar.h>

// To ensure correct resolution of symbols, add Psapi.lib to TARGETLIBS
// and compile with -DPSAPI_VERSION=1

#define DIV 1024
#define WIDTH 7

static PyObject* retorno_memoria(PyObject* self, PyObject* args){
    DWORD aProcesses[1024], cbNeeded, cProcesses;
    unsigned int i;

    EnumProcesses( aProcesses, sizeof(aProcesses), &cbNeeded );
    cProcesses = cbNeeded / sizeof(DWORD);
    PyObject* results = PyList_New(cProcesses);
    for ( i = 0; i < cProcesses; i++ ){
        int dato;
        dato = PrintMemoryInfo(aProcesses[i]);
        PyObject* pytho_int = Py_BuildValue("i", dato);
        PyList_SetItem(results, i, pytho_int);
    }
    return results;
}

int PrintMemoryInfo( DWORD processID ){
    HANDLE hProcess;
    PROCESS_MEMORY_COUNTERS pmc;
    printf( "\nProcess ID: %u\n", processID );
    hProcess = OpenProcess(  PROCESS_QUERY_INFORMATION |
                                    PROCESS_VM_READ,
                                    FALSE, processID );
    if (NULL == hProcess) return;
    if ( GetProcessMemoryInfo( hProcess, &pmc, sizeof(pmc)) ){
        return pmc.WorkingSetSize/DIV;
    }

    CloseHandle( hProcess );
}

static PyMethodDef OperacionesMethos[]={
    {"memoria", retorno_memoria, METH_VARARGS,"Retorno de memoria"}
};

static struct  PyModuleDef OperacionesM={
    PyModuleDef_HEAD_INIT, "retorno_memoria", "", -1, OperacionesMethos
};

PyMODINIT_FUNC initOperacion(void){
    return PyModule_Create(&OperacionesM);
}