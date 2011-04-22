#define WinMain WinMain_off
#include <sys/cdefs.h>
#include <sys/time.h>
#include <dbghelp.h>
#include <sddl.h>
#undef WinMain

#define getpwuid_r getpwuid_r_off
#define getgrgid_r getgrgid_r_off

#include <csignal>
#include <unistd.h>
#include <dirent.h>
#include <pwd.h>
#include <grp.h>
#include <sys/socket.h>

#undef getpwuid_r
#undef getgrgid_r
#undef readdir_r
#undef dirent

#include "hcore/base.hxx"
#include "hcore/xalloc.hxx"
#include "cxxabi.h"
#include "cleanup.hxx"

using namespace std;
using namespace yaal;
using namespace yaal::hcore;
using namespace msvcxx;

__declspec( thread ) int SocketErrno::_errno = 0;
int ESCDELAY = 0;

namespace abi
	{

	char* __cxa_demangle( char const* const a, int, int, int* )
		{
		char* buf = xcalloc<char>( 1024 );
		UnDecorateSymbolName( a, buf, 1023, 0 );
		return ( buf );
		}

	}

extern "C" 
int backtrace( void** buf_, int size_ )
	{
	return ( CaptureStackBackTrace( 0, std::min( size_, 63 ), buf_, NULL ) );
	}

extern "C"
char** backtrace_symbols( void* const* buf_, int size_ )
	{
	char** strings = xcalloc<char*>( size_ );
	for ( int i( 0 ); i < size_; ++ i )
		strings[i] = reinterpret_cast<char*>( buf_[i] );
	return ( strings );
	}

extern "C" void* dlopen( char const*, int );
M_EXPORT_SYMBOL
void* dlopen_fix( char const* name_, int flag_ )
	{
	HANDLE handle( 0 );
	if ( ! name_ )
		handle = GetModuleHandle( NULL );
	else
		handle = dlopen( name_, flag_ );
	return ( handle );
	}

int unix_readdir_r( DIR* dir_, struct unix_dirent* entry_, struct unix_dirent** result_ )
	{
	dirent* result;
	dirent broken;
	int error( readdir_r( dir_, &broken, &result ) );
	if ( ( ! error ) && result )
		{
		*result_ = reinterpret_cast<unix_dirent*>( result );
		entry_->d_fileno = (*result_)->d_fileno;
		entry_->d_type = (*result_)->d_type;
		strncpy( entry_->d_name, (*result_)->d_name, NAME_MAX );
		}
	return ( error );
	}

PSID get_base_sid( char* buffer_, int size_ )
	{
	/* Open the access token associated with the calling process. */
	HANDLE hToken( NULL );
	if ( ! ::OpenProcessToken( ::GetCurrentProcess(), TOKEN_QUERY, &hToken ) )
		{
		log_windows_error( "OpenProcessToken" );
		return ( NULL );
		}

	::memset( buffer_, 0, size_ );
	/* Retrieve the token information in a TOKEN_USER structure. */
	DWORD dummy( 0 );
	if ( ! ::GetTokenInformation( hToken, TokenUser, buffer_, size_, &dummy ) )
		{
		log_windows_error( "GetTokenInformation" );
		return ( NULL );
		}

	::CloseHandle( hToken );
	PTOKEN_USER tokenUser( static_cast<PTOKEN_USER>( static_cast<void*>( buffer_ ) ) );
	if ( ! ::IsValidSid( tokenUser->User.Sid ) )
		{
		log_windows_error( "IsValidSid" );
		return ( NULL );
		}
	return ( tokenUser->User.Sid );
	}

M_EXPORT_SYMBOL
uid_t ms_getuid( void )
	{
	static int const SID_SIZE( 128 );
	static int const TOKEN_USER_SIZE( sizeof ( TOKEN_USER ) + SID_SIZE );
	char tokenUserBuffer[ TOKEN_USER_SIZE ];
	PSID sid( get_base_sid( tokenUserBuffer, TOKEN_USER_SIZE ) );
	int uid( sid ? *::GetSidSubAuthority( sid, *::GetSidSubAuthorityCount( sid ) - 1 ) : -1 );
	return ( uid );
	}

bool get_system_account_name( int id_, char* buf_, int size_ )
	{
	static int const SID_SIZE( 128 );
	static int const TOKEN_USER_SIZE( sizeof ( TOKEN_USER ) + SID_SIZE );
	char tokenUserBuffer[ TOKEN_USER_SIZE ];
	PSID sid( get_base_sid( tokenUserBuffer, TOKEN_USER_SIZE ) );
	bool fail( true );
	if ( sid )
		{
		LPTSTR sidStrBuffer( NULL );
		if ( ::ConvertSidToStringSid( sid, &sidStrBuffer ) )
			{
			HString sidStr( sidStrBuffer );
			::LocalFree( sidStrBuffer );
			sidStr.erase( sidStr.find_last( '-' ) + 1 );
			sidStr += id_;
			PSID newSid( NULL );
			if ( ::ConvertStringSidToSid( sidStr.raw(), &newSid ) )
				{
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
int getpwuid_r( uid_t uid_, struct passwd* p_, char* buf_, int size_, struct passwd** result_ )
	{
	p_->pw_name = buf_;
	int err( -1 );
	if ( ! get_system_account_name( uid_, buf_, size_ ) )
		{
		err = 0;
		*result_ = p_;
		}
	return ( err );
	}

extern "C"
int getgrgid_r( gid_t gid_, struct group* g_, char* buf_, int size_, struct group** result_ )
	{
	g_->gr_name = buf_;
	int err( -1 );
	if ( ! get_system_account_name( gid_, buf_, size_ ) )
		{
		err = 0;
		*result_ = g_;
		}
	return ( err );
	}

int ms_gethostname( char* buf_, int len_ )
	{
	WSADATA wsaData;
	WORD wVersionRequested( MAKEWORD( 2, 2 ) );
	int err( WSAStartup( wVersionRequested, &wsaData ) );
#undef gethostname
	return ( gethostname( buf_, len_ ) );
	}

const DWORD MS_VC_EXCEPTION = 0x406D1388;

#pragma pack( push, 8 )
typedef struct tagTHREADNAME_INFO
	{
	DWORD dwType; // Must be 0x1000.
	LPCSTR szName; // Pointer to name (in user addr space).
	DWORD dwThreadID; // Thread ID (-1=caller thread).
	DWORD dwFlags; // Reserved for future use, must be zero.
	} THREADNAME_INFO;
#pragma pack( pop )

void pthread_setname_np( void*, char const* name_ )
	{
	THREADNAME_INFO info;
	info.dwType = 0x1000;
	info.szName = name_;
	info.dwThreadID = -1;
	info.dwFlags = 0;
	__try
		{
		RaiseException( MS_VC_EXCEPTION,
			0, sizeof( info ) / sizeof( ULONG_PTR ),
			reinterpret_cast<ULONG_PTR*>( &info ) );
		}
	/* __except(EXCEPTION_EXECUTE_HANDLER) */
	__except( EXCEPTION_CONTINUE_EXECUTION )
		{
		}
	}

void* get_module_func_address( char const* moduleName_, char const* funcName_ )
	{
	HMODULE h( ::GetModuleHandle( moduleName_ ) );
	void* p( ::GetProcAddress( h, funcName_ ) );
	return ( p );
	}

int* iconv_errno( void )
	{
	static int* (*msvcrt_errno)( void ) = NULL;
	__declspec( thread ) static int errorNumber( 0 );
	if ( ! msvcrt_errno )
		{
		void* p( get_module_func_address( "msvcrt.dll", "_errno" ) );
		msvcrt_errno = p ? static_cast<int* (*)( void )>( p ) : &_errno;
		}
	errorNumber = *msvcrt_errno();
	return ( &errorNumber );
	}

M_EXPORT_SYMBOL
int WINAPI WinMain(
	 __in HINSTANCE, __in HINSTANCE, __in LPSTR,
	 __in int )
	{
	LPWSTR cmdLineW( ::GetCommandLineW() );
	int nArgs( 0 );
	LPWSTR *szArglist( ::CommandLineToArgvW( cmdLineW, &nArgs  ) );
	char* cmdLine( ::GetCommandLine() );
	char** argv = new char*[ nArgs ];
	for ( int i( 1 ), offset( 0 ); i < nArgs; ++ i )
		{
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

void __stdcall timer_expired( PVOID lpParameter, __in BOOLEAN )
	{
	int sigNo( reinterpret_cast<int>( lpParameter ) );
	kill( getpid(), sigNo );
	}

M_EXPORT_SYMBOL
int timer_create( clockid_t, struct sigevent*, timer_t* timer_ )
	{
	int err( 0 );
	HANDLE h( NULL );
	if ( ! ::CreateTimerQueueTimer( &h,
			NULL, timer_expired, reinterpret_cast<PVOID>( SIGALRM ),
			ULONG_MAX, 0, WT_EXECUTEONLYONCE ) )
		{
		log_windows_error( "CreateTimerQueueTimer" );
		err = -1;
		}
	else
		*timer_ = h;
	return ( err );
	}

M_EXPORT_SYMBOL
int timer_settime( timer_t timer_, int, struct itimerspec const* due_, struct itimerspec* )
	{
	int err( 0 );
	int long miliseconds( due_->it_value.tv_sec * 1000 + due_->it_value.tv_nsec / 1000000 );
	if ( ! ::ChangeTimerQueueTimer( NULL, timer_, miliseconds ? miliseconds : ULONG_MAX, 0 ) )
		{
		log_windows_error( "CreateTimerQueueTimer" );
		err = -1;
		}
	return ( err );
	}

M_EXPORT_SYMBOL
int timer_delete( timer_t timer_ )
	{
	int err( 0 );
	if ( ! ::DeleteTimerQueueTimer( NULL, timer_, NULL ) )
		{
		log_windows_error( "DeleteTimerQueueTimer" );
		err = -1;
		}
	return ( err );
	}
