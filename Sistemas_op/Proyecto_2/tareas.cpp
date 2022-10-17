#include <windows.h>
#include <tlhelp32.h>
#include <tchar.h>
#include <stdio.h>
//  Forward declarations:
BOOL GetProcessList( );
void printError( TCHAR* msg );
int main( void )
{
  GetProcessList( );
  return 0;
}
BOOL GetProcessList( )
{
  HANDLE hProcessSnap;
  HANDLE hProcess;
  PROCESSENTRY32 pe32;
  DWORD dwPriorityClass;
  // Take a snapshot of all processes in the system.
  hProcessSnap = CreateToolhelp32Snapshot( TH32CS_SNAPMODULE32, 0 );
  if( hProcessSnap == INVALID_HANDLE_VALUE )
  {
    return( FALSE );
  }
  // Set the size of the structure before using it.
  pe32.dwSize = sizeof( PROCESSENTRY32 );
  // Retrieve information about the first process,
  // and exit if unsuccessful
  if( !Process32First( hProcessSnap, &pe32 ) )
  {
    CloseHandle( hProcessSnap );          // clean the snapshot object
    return( FALSE );
  }
  // Now walk the snapshot of processes, and
  // display information about each process in turn
  do
  {
    _tprintf( TEXT("\n\n=====================================================" ));
    _tprintf( TEXT("\nPROCESS NAME:  %s"), pe32.szExeFile );
    _tprintf( TEXT("\n-------------------------------------------------------" ));
    // Retrieve the priority class.
    dwPriorityClass = 0;
    hProcess = OpenProcess( PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID );
    {
      dwPriorityClass = GetPriorityClass( hProcess );
      CloseHandle( hProcess );
    }
    _tprintf( TEXT("\n  Process ID        = %d"), pe32.th32ProcessID );
    _tprintf( TEXT("\n  Parent process ID = %d"), pe32.th32ParentProcessID );
    _tprintf( TEXT("\n  Priority base     = %d"), pe32.pcPriClassBase );
    if( dwPriorityClass )
      _tprintf( TEXT("\n  Priority class    = %d"), dwPriorityClass );
  } while( Process32Next( hProcessSnap, &pe32 ) );
  CloseHandle( hProcessSnap );
  return( TRUE );
}