#include <sys/cdefs.h>
#include <sys/time.h>
#include <dbghelp.h>

#define getpwuid_r getpwuid_r_off

#include <unistd.h>
#include <dirent.h>
#include <pwd.h>
#include <sys/socket.h>

#undef getpwuid_r
#undef readdir_r
#undef dirent

#include "hcore/base.hxx"
#include "hcore/xalloc.hxx"
#include "cxxabi.h"

#include "cleanup.hxx"

using namespace std;
using namespace yaal;
using namespace yaal::hcore;

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

extern "C"
int getpwuid_r( uid_t, struct passwd* p, char* buf, int size, struct passwd** )
	{
	p->pw_name = buf;
	DWORD s = size;
	int err( ! GetUserName( buf, &s ) );
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
