#define WinMain WinMain_off
#include <sys/cdefs.h>
#include <sys/time.h>
#include <dbghelp.h>
#include <sddl.h>
#include <psapi.h>
#undef WinMain

#define getpwuid_r getpwuid_r_off
#define getgrgid_r getgrgid_r_off

#include <csignal>
#include <unistd.h>
#include <dirent.h>
#include <pwd.h>
#include <grp.h>
#include <sys/resource.h>
#include <sys/socket.h>
#include <iostream>

#undef getpwuid_r
#undef getgrgid_r
#undef readdir_r
#undef dirent

#include "hcore/base.hxx"
#include "hcore/memory.hxx"
#include "cxxabi.h"
#include "cleanup.hxx"
#include "crit.hxx"
#include "msio.hxx"
#include "msvcxx.hxx"

using namespace std;
using namespace yaal;
using namespace yaal::hcore;
using namespace msvcxx;

CMutex _backtraceMutex_;
__declspec( thread ) int SocketErrno::_errno = 0;
int ESCDELAY = 0;
static int const MAX_SYMBOL_NAME_LEN( 2048 );

namespace abi {

	char* __cxa_demangle( char const* const a, int, int, int* ) {
		char* buf = memory::calloc<char>( MAX_SYMBOL_NAME_LEN );
		::UnDecorateSymbolName( a, buf, MAX_SYMBOL_NAME_LEN - 1, 0 );
		return ( buf );
	}

}

extern "C"
int backtrace( void** buf_, int size_ ) {
	CLock l( _backtraceMutex_ );
/* Warning!
 *
 * Maximum number of frames to capture has changed! (63 => 62)
 *
 * Build date: 5/12/2011
 * Windows Server 2003 and Windows XP:
 *   The sum of the FramesToSkip and FramesToCapture parameters must be less than 63.
 */
	int toFetch( std::min( size_, 62 ) );
	::SetLastError( 0 );
	int got( ::CaptureStackBackTrace( 0, toFetch, buf_, NULL ) );
	if ( ( toFetch > 0 ) && ( got < 1 ) && ( ::GetLastError() != 0 ) )
		log_windows_error( "CaptureStackBackTrace" );
	static bool once( false );
	if ( ! once ) {
		once = true;
		/* ::SymSetOptions( SYMOPT_UNDNAME ); */
		if ( ! ::SymInitialize( ::GetCurrentProcess(), NULL, true ) ) {
			log_windows_error( "SymInitialize" );
			got = 0;
		}
	}
	char** strings = reinterpret_cast<char**>( buf_ );
	return ( got );
}

extern "C"
char** backtrace_symbols( void* const* buf_, int size_ ) {
	CLock l( _backtraceMutex_ );
	HANDLE process( ::GetCurrentProcess() );
	bool fail( false );
	char** strings = reinterpret_cast<char**>( memory::calloc<char>( size_ * ( MAX_SYMBOL_NAME_LEN + 2 + sizeof ( char* ) ) ) );
	for ( int i( 0 ); i < size_; ++ i )
		strings[ i ] = reinterpret_cast<char*>( strings + size_ ) + i * ( MAX_SYMBOL_NAME_LEN + 2 );
	char buffer[ sizeof ( SYMBOL_INFO ) + MAX_SYMBOL_NAME_LEN ];
	SYMBOL_INFO* tester( static_cast<SYMBOL_INFO*>( static_cast<void*>( buffer ) ) );
	::memset( tester, 0, sizeof ( SYMBOL_INFO ) );
	tester->SizeOfStruct = sizeof ( SYMBOL_INFO );
	tester->MaxNameLen = MAX_SYMBOL_NAME_LEN;
	for ( int i( 0 ); i < size_; ++ i ) {
		if ( ! ::SymFromAddr( process, reinterpret_cast<DWORD64>( buf_[ i ] ), 0, tester ) ) {
			if ( ! i ) {
				log_windows_error( "SymFromAddr" );
				fail = true;
			} else {
				char const invalid[] = "(invalid address";
				for ( ; i < size_; ++ i )
					::strncpy( strings[i], invalid, sizeof ( invalid ) );
			}
			break;
		} else {
			strings[i][0] = '(';
			if ( ! tester->NameLen && tester->Name && tester->Name[0] ) {
				tester->Name[MAX_SYMBOL_NAME_LEN - 1] = 0;
				tester->NameLen = ::strlen( tester->Name );
			}
			::strncpy( strings[i] + 1, tester->Name, tester->NameLen );
			strings[i][ tester->NameLen + 1 ] = 0;
		}
	}
	if ( fail ) {
		memory::free( strings );
		strings = NULL;
	}
	return ( strings );
}

extern "C" void* dlopen( char const*, int );
M_EXPORT_SYMBOL
void* dlopen_fix( char const* name_, int flag_ ) {
	HANDLE handle( 0 );
	if ( ! name_ )
		handle = GetModuleHandle( NULL );
	else
		handle = dlopen( name_, flag_ );
	return ( handle );
}

#undef unsetenv
extern "C" int unsetenv( char const* name_ );
int unsetenv_fix( char const* name_ ) {
	string n( name_ );
	n += "=";
	int ret( unsetenv( n.c_str() ) );
	return ( ret );
}

int unix_readdir_r( DIR* dir_, struct unix_dirent* entry_, struct unix_dirent** result_ ) {
	dirent* result( NULL );
	dirent broken;
	int error( readdir_r( dir_, &broken, &result ) );
	*result_ = reinterpret_cast<unix_dirent*>( result );
	if ( ( ! error ) && result ) {
		entry_->d_fileno = (*result_)->d_fileno;
		entry_->d_type = (*result_)->d_type;
		strncpy( entry_->d_name, (*result_)->d_name, NAME_MAX );
	}
	return ( error );
}

PSID get_base_sid( char* buffer_, int size_ ) {
	/* Open the access token associated with the calling process. */
	HANDLE hToken( NULL );
	if ( ! ::OpenProcessToken( ::GetCurrentProcess(), TOKEN_QUERY, &hToken ) ) {
		log_windows_error( "OpenProcessToken" );
		return ( NULL );
	}

	::memset( buffer_, 0, size_ );
	/* Retrieve the token information in a TOKEN_USER structure. */
	DWORD dummy( 0 );
	if ( ! ::GetTokenInformation( hToken, TokenUser, buffer_, size_, &dummy ) ) {
		log_windows_error( "GetTokenInformation" );
		return ( NULL );
	}

	::CloseHandle( hToken );
	PTOKEN_USER tokenUser( static_cast<PTOKEN_USER>( static_cast<void*>( buffer_ ) ) );
	if ( ! ::IsValidSid( tokenUser->User.Sid ) ) {
		log_windows_error( "IsValidSid" );
		return ( NULL );
	}
	return ( tokenUser->User.Sid );
}

M_EXPORT_SYMBOL
uid_t ms_getuid( void ) {
	static int const SID_SIZE( 128 );
	static int const TOKEN_USER_SIZE( sizeof ( TOKEN_USER ) + SID_SIZE );
	char tokenUserBuffer[ TOKEN_USER_SIZE ];
	PSID sid( get_base_sid( tokenUserBuffer, TOKEN_USER_SIZE ) );
	int uid( sid ? *::GetSidSubAuthority( sid, *::GetSidSubAuthorityCount( sid ) - 1 ) : -1 );
	return ( uid );
}

bool get_system_account_name( int id_, char* buf_, int size_ ) {
	static int const SID_SIZE( 128 );
	static int const TOKEN_USER_SIZE( sizeof ( TOKEN_USER ) + SID_SIZE );
	char tokenUserBuffer[ TOKEN_USER_SIZE ];
	PSID sid( get_base_sid( tokenUserBuffer, TOKEN_USER_SIZE ) );
	bool fail( true );
	if ( sid ) {
		LPTSTR sidStrBuffer( NULL );
		if ( ::ConvertSidToStringSid( sid, &sidStrBuffer ) ) {
			HString sidStr( sidStrBuffer );
			::LocalFree( sidStrBuffer );
			sidStr.erase( sidStr.find_last( '-' ) + 1 );
			sidStr += id_;
			PSID newSid( NULL );
			if ( ::ConvertStringSidToSid( sidStr.raw(), &newSid ) ) {
				DWORD size( size_ );
				static int const DUMMY_BUFFER_SIZE = 128;
				char dummy[DUMMY_BUFFER_SIZE];
				DWORD dummyLen( DUMMY_BUFFER_SIZE );
				SID_NAME_USE eUse = SidTypeUnknown;
				if ( ::LookupAccountSid( NULL, newSid, buf_, &size, dummy, &dummyLen, &eUse ) )
					fail = false;
				::LocalFree( newSid );
			}
		}
	}
	return ( fail );
}

extern "C"
int getpwuid_r( uid_t uid_, struct passwd* p_, char* buf_, int size_, struct passwd** result_ ) {
	p_->pw_name = buf_;
	int err( -1 );
	if ( ! get_system_account_name( uid_, buf_, size_ ) ) {
		err = 0;
		*result_ = p_;
	}
	return ( err );
}

extern "C"
int getgrgid_r( gid_t gid_, struct group* g_, char* buf_, int size_, struct group** result_ ) {
	g_->gr_name = buf_;
	int err( -1 );
	if ( ! get_system_account_name( gid_, buf_, size_ ) ) {
		err = 0;
		*result_ = g_;
	}
	return ( err );
}

int ms_gethostname( char* buf_, int len_ ) {
	static bool once( false );
	if ( ! once ) {
		once = true;
		WSADATA wsaData;
		WORD wVersionRequested( MAKEWORD( 2, 2 ) );
		int err( WSAStartup( wVersionRequested, &wsaData ) );
		SystemIO::get_instance();
	}
#undef gethostname
	return ( gethostname( buf_, len_ ) );
}

const DWORD MS_VC_EXCEPTION = 0x406D1388;

#pragma pack( push, 8 )
typedef struct tagTHREADNAME_INFO {
	DWORD dwType; // Must be 0x1000.
	LPCSTR szName; // Pointer to name (in user addr space).
	DWORD dwThreadID; // Thread ID (-1=caller thread).
	DWORD dwFlags; // Reserved for future use, must be zero.
} THREADNAME_INFO;
#pragma pack( pop )

void pthread_setname_np( void*, char const* name_ ) {
	THREADNAME_INFO info;
	info.dwType = 0x1000;
	info.szName = name_;
	info.dwThreadID = -1;
	info.dwFlags = 0;
	__try {
		RaiseException( MS_VC_EXCEPTION,
			0, sizeof( info ) / sizeof( ULONG_PTR ),
			reinterpret_cast<ULONG_PTR*>( &info ) );
	}
	/* __except(EXCEPTION_EXECUTE_HANDLER) */
	__except( EXCEPTION_CONTINUE_EXECUTION ) {
	}
}

void* get_module_func_address( char const* moduleName_, char const* funcName_ ) {
	HMODULE h( ::GetModuleHandle( moduleName_ ) );
	void* p( ::GetProcAddress( h, funcName_ ) );
	return ( p );
}

int* iconv_errno( void ) {
	static int* (*msvcrt_errno)( void ) = NULL;
	__declspec( thread ) static int errorNumber( 0 );
	if ( ! msvcrt_errno ) {
		void* p( get_module_func_address( "msvcrt.dll", "_errno" ) );
		msvcrt_errno = p ? static_cast<int* (*)( void )>( p ) : &_errno;
	}
	errorNumber = *msvcrt_errno();
	return ( &errorNumber );
}

M_EXPORT_SYMBOL
int WINAPI WinMain(
	 __in HINSTANCE, __in HINSTANCE, __in LPSTR,
	 __in int ) {
	LPWSTR cmdLineW( ::GetCommandLineW() );
	int nArgs( 0 );
	LPWSTR *szArglist( ::CommandLineToArgvW( cmdLineW, &nArgs  ) );
	char* cmdLine( ::GetCommandLine() );
	char** argv = new ( memory::yaal ) char*[ nArgs ];
	for ( int i( 1 ), offset( 0 ); i < nArgs; ++ i ) {
		offset += ::lstrlenW( szArglist[ i - 1 ] );
		cmdLine[offset] = 0;
		++ offset;
		argv[ i ] = cmdLine + offset;
	}
	::LocalFree( szArglist );
	argv[ 0 ] = cmdLine;
	HMODULE appHandle( ::GetModuleHandle( NULL ) );
	typedef int ( *main_type )( int, char** );
	main_type main = bit_cast<main_type>( ::GetProcAddress( appHandle, "main" ) );
	return ( main( nArgs, argv ) );
}

void __stdcall timer_expired( PVOID lpParameter, __in BOOLEAN ) {
	int sigNo( reinterpret_cast<int>( lpParameter ) );
	kill( getpid(), sigNo );
}

M_EXPORT_SYMBOL
int timer_create( clockid_t, struct sigevent*, timer_t* timer_ ) {
	int err( 0 );
	HANDLE h( NULL );
	if ( ! ::CreateTimerQueueTimer( &h,
			NULL, timer_expired, reinterpret_cast<PVOID>( SIGALRM ),
			ULONG_MAX, 0, WT_EXECUTEONLYONCE ) ) {
		log_windows_error( "CreateTimerQueueTimer" );
		err = -1;
	} else
		*timer_ = h;
	return ( err );
}

M_EXPORT_SYMBOL
int timer_settime( timer_t timer_, int, struct itimerspec const* due_, struct itimerspec* ) {
	int err( 0 );
	time_t miliseconds( due_->it_value.tv_sec * 1000 + due_->it_value.tv_nsec / 1000000 );
	if ( ! ::ChangeTimerQueueTimer( NULL, timer_, static_cast<ULONG>( miliseconds ? miliseconds : ULONG_MAX ), 0 ) ) {
		log_windows_error( "CreateTimerQueueTimer" );
		err = -1;
	}
	return ( err );
}

M_EXPORT_SYMBOL
int timer_delete( timer_t timer_ ) {
	int err( 0 );
	if ( ! ::DeleteTimerQueueTimer( NULL, timer_, NULL ) && ( ::GetLastError() != ERROR_IO_PENDING ) ) {
		log_windows_error( "DeleteTimerQueueTimer" );
		err = -1;
	}
	return ( err );
}

extern "C"
int getrusage( __rusage_who_t who_, struct rusage* usage_ ) __THROW {
	int err( 0 );
	if ( who_ != RUSAGE_SELF )
		err = EINVAL;
	else {
		PROCESS_MEMORY_COUNTERS pmc;
		::GetProcessMemoryInfo( ::GetCurrentProcess(), &pmc, sizeof ( pmc ) );
		usage_->ru_maxrss = pmc.WorkingSetSize;
	}
	return ( 0 );
}

driver_names_t get_drivers( void ) {
	static int const ARRAY_SIZE( 1024 );
	LPVOID drivers[ARRAY_SIZE];
	DWORD cbNeeded( 0 );
	driver_names_t driverNames;
	if ( ::EnumDeviceDrivers( drivers, sizeof ( drivers ), &cbNeeded ) && ( cbNeeded < sizeof ( drivers ) ) ) { 
		TCHAR szDriver[ARRAY_SIZE];
		int cDrivers( cbNeeded / sizeof ( drivers[0] ) );
		for ( int i( 0 ); i < cDrivers; ++ i ) {
			if ( ::GetDeviceDriverBaseName( drivers[i], szDriver, countof ( szDriver ) ) ) {
				int c( 0 );
				while ( ( c < countof ( szDriver ) ) && szDriver[c] ) {
					szDriver[c] = tolower( szDriver[c] );
					++ c;
				}
				driverNames.insert( szDriver );
			} else
				log_windows_error( "GetDeviceDriverBaseName" );
		}
	} else {
		log_windows_error( "EnumDeviceDrivers" );
	}
	return ( driverNames );
}

