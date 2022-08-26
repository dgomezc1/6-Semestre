#include <windows.h>
#include <tlhelp32.h>
#include <tchar.h>
#include <cstdio>
#include<iostream>
#include<thread>
#include<math.h>
#include<chrono>
#include <vector>
#include <ctime> 


using namespace std;
double resultado = 0;


//  Forward declarations:
BOOL ListProcessThreads( DWORD dwOwnerPID );
void calculoPi(double start, double end, double uN);

int main( void ){
    unsigned t0, t1;
    int numProcessors = std::thread::hardware_concurrency();
    int iteraciones;
    cout << "Ingrese el numero de iteraciornes: ";
    cin >> iteraciones;
    _tprintf( TEXT("\nnumero ite = %d, numero Procesadores = %d"), iteraciones, numProcessors );
    int numOperationProccesor = iteraciones/numProcessors;
    int start = 0;
    int end = numOperationProccesor;
    int residuo = iteraciones%numProcessors;
    auto t_start = std::chrono::high_resolution_clock::now();
    t0=clock();

    calculoPi(0,iteraciones, iteraciones);
    ListProcessThreads(GetCurrentProcessId());

    t1 = clock();
    double time = (double(t1-t0)/CLOCKS_PER_SEC);
    auto t_end = std::chrono::high_resolution_clock::now();
    double elapsed_time_ms = std::chrono::duration<double, std::milli>(t_end-t_start).count();
    _tprintf(TEXT("\nTiempo consumo de procesador = %f\nTiempo real: %f"),elapsed_time_ms,time);
    _tprintf(TEXT("\nResultado de PI = %.16f"),resultado);
  return 0;
}


void calculoPi(double inicio, double fin, double n) {
    double N = n;
    double sumatoria = 0.0;
    for(double i = inicio; i < fin; i++){
        sumatoria += (4.0/(1.0+pow((i+0.5)/N,2.0)))*(1.0/N);
    }
    resultado+=sumatoria;
}

BOOL ListProcessThreads( DWORD dwOwnerPID ) { 
  HANDLE hThreadSnap = INVALID_HANDLE_VALUE;                                //inicializamos el hThreadSnap en invalido
  THREADENTRY32 te32;                                                       //Inicializamos un THREADENTRY32, es un tipo de dato que almacena una lista de entradas de un subproceso
  
  hThreadSnap = CreateToolhelp32Snapshot( TH32CS_SNAPTHREAD, 0 );           //Tomamos una captura de los subprocesos (hilos) que se ejecutan actualmente, 0 para indicar que es el proceso acutal
  if( hThreadSnap == INVALID_HANDLE_VALUE )                                 //Si la captura es invalida se retorna falso
    return( FALSE ); 
 
  te32.dwSize = sizeof(THREADENTRY32 );                                     //Antes de llamar la funcoin Thread32First es necesario inicializar el dwSize(tamaño) del te32 (objecto THREADENTRY32)
 
  if( !Thread32First(hThreadSnap, &te32 )){                                 //Retomamos la informacion del primer hilo y salimos si no es exitoso, se envia el snapshot y el elemento tipo THREADENTRY32
    CloseHandle( hThreadSnap );                                             //Limpiamos el objecto con la captura
    return(FALSE);                                                          //Retorno falso
  }
    _tprintf(TEXT("\n********LISTA DE HILOS*********"));
    _tprintf(TEXT("\nOWNER PROCCESS ID      = 0x%08X"),dwOwnerPID);
  do { 
    if( te32.th32OwnerProcessID == dwOwnerPID ){                            //En caso de que el hilo corresponda a nuestro proceso padre imprimimos la informacion
      _tprintf( TEXT("\n     THREAD ID      = 0x%08X"), te32.th32ThreadID );//ID del hilo
      _tprintf( TEXT("\n     base priority  = %d"), te32.tpBasePri );       //Prioridad del hilo de ejecuion, siendo 0 el menor y 31 el mayor
    }
  }while( Thread32Next(hThreadSnap, &te32 ) );                              //Recorremos todos los hilos siguientes hasta el ultimo

  _tprintf( TEXT("\n"));

  CloseHandle( hThreadSnap );                                               //Limpiamos el objecto con sl snapshot antes de terminar la funcion
  return( TRUE );                                                           //Retornamos verdadero
}       
