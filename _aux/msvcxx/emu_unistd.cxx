#include <sys/cdefs.h>
#define log cmath_log
#include <sstream>
#undef log
#include <io.h>
#include <sys/time.h>
#include <../include/sys/stat.h>
#define YAAL_MSVCXX_OPENSSL_RSA_H_INCLUDED 1
#include <openssl/ssl.h>

#define getpid getpid_off
#define isatty isatty_off
#define getpwuid_r getpwuid_r_off
#define access access_off
#define lseek lseek_off
#define dup dup_off

#define fill fill_off
#include <unistd.h>
#undef fill
#include <dirent.h>
#include <pwd.h>
#define _FCNTL_H 1
#include <bits/fcntl.h>
#undef getpwuid_r
#undef getpid
#undef readdir_r
#undef dirent

#include "emu_pwd.hxx"
#include "hcore/base.hxx"
#include "hcore/hlog.hxx"
#include "cleanup.hxx"

#include "cxxabi.h"

#include "cleanup.hxx"
#include "msio.hxx"

using namespace std;
using namespace yaal;
using namespace yaal::hcore;
using namespace yaal::tools;

namespace msvcxx {

extern "C" int fcntl( int fd_, int cmd_, ... );

int APIENTRY CreatePipeEx( LPHANDLE lpReadPipe,
	LPHANDLE lpWritePipe,
	LPSECURITY_ATTRIBUTES lpPipeAttributes,
	DWORD nSize,
	DWORD dwReadMode,
	DWORD dwWriteMode ) {
	HANDLE ReadPipeHandle, WritePipeHandle;
	DWORD dwError;

	// Only one valid OpenMode flag - FILE_FLAG_OVERLAPPED
	if ( ( dwReadMode | dwWriteMode ) & ( ~FILE_FLAG_OVERLAPPED ) ) {
		SetLastError( ERROR_INVALID_PARAMETER );
		return FALSE;
	}

	if ( nSize == 0 )
		nSize = 4096;
	static int PipeSerialNumber = 0;
	stringstream PipeName;
	PipeName << "\\\\.\\Pipe\\RemoteExeAnon." << GetCurrentProcessId() << ( PipeSerialNumber ++ );

	ReadPipeHandle = CreateNamedPipe(
		PipeName.str().c_str(),
		PIPE_ACCESS_INBOUND | dwReadMode,
		PIPE_TYPE_BYTE | PIPE_WAIT,
		1,             // Number of pipes
		nSize,         // Out buffer size
		nSize,         // In buffer size
		//  Set the default timeout to 120 seconds
		120 * 1000,    // Timeout in ms
		lpPipeAttributes );

	if ( ! ReadPipeHandle )
		return FALSE;

	WritePipeHandle = CreateFile(
		PipeName.str().c_str(),
		GENERIC_WRITE,
		0,                         // No sharing
		lpPipeAttributes,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL | dwWriteMode,
		NULL );

	if ( INVALID_HANDLE_VALUE == WritePipeHandle ) {
		dwError = GetLastError();
		CloseHandle( ReadPipeHandle );
		SetLastError( dwError );
		return FALSE;
	}

	*lpReadPipe = ReadPipeHandle;
	*lpWritePipe = WritePipeHandle;
	return ( 1 );
}

M_EXPORT_SYMBOL
int fcntl( int fd_, int cmd_, int arg_ ) {
#undef fcntl
	int ret( 0 );
	if ( fd_ < SystemIO::MANAGED_IO )
		ret = fcntl( fd_, cmd_, arg_ );
	else {
		SystemIO& sysIo( SystemIO::get_instance() );
		IO& io( *( sysIo.get_io( fd_ ).second ) );
		ret = io.fcntl( cmd_, arg_ );
	}
	return ( ret );
}

M_EXPORT_SYMBOL
int dup2( int fd1_, int fd2_ ) {
	int ret( -1 );
	if ( ( fd1_ < SystemIO::MANAGED_IO ) && ( fd2_ < SystemIO::MANAGED_IO ) )
		ret = _dup2( fd1_, fd2_ );
	else {
		SystemIO& sysIo( SystemIO::get_instance() );
		ret = sysIo.dup2_io( fd1_, fd2_ );
	}
	return ( ret );
}

M_EXPORT_SYMBOL
int pipe( int* fds_ ) {
	SystemIO& sysIo( SystemIO::get_instance() );
	HANDLE hRead( NULL );
	HANDLE hWrite( NULL );
	bool ok( CreatePipeEx( &hRead, &hWrite, NULL, 4096, FILE_FLAG_OVERLAPPED , FILE_FLAG_OVERLAPPED ) ? true : false );
	if ( ok ) {
		SystemIO::io_t readIO( sysIo.create_io( IO::TYPE::PIPE, hRead ) );
		SystemIO::io_t writeIO( sysIo.create_io( IO::TYPE::PIPE, hWrite ) );
		fds_[0] = readIO.first;
		fds_[1] = writeIO.first;
	}
	return ( ok ? 0 : -1 );
}

int close( int const& fd_ ) {
	int ret( 0 );
	if ( fd_ < SystemIO::MANAGED_IO )
		ret = ::close( fd_ );
	else
		ret = SystemIO::get_instance().close_io( fd_ );
	if ( ret < 0 )
		log_windows_error( "close" );
	return ( ret );
}

M_EXPORT_SYMBOL
int long read( int const& fd_, void* buf_, int long size_ ) {
	int long nRead( -1 );
	if ( fd_ < SystemIO::MANAGED_IO )
		nRead = ::read( fd_, buf_, size_ ); 
	else {
		SystemIO& sysIo( SystemIO::get_instance() );
		IO& io( *( sysIo.get_io( fd_ ).second ) );
		nRead = io.read( buf_, size_ );
	}
	return ( nRead );
}

M_EXPORT_SYMBOL
int long write( int const& fd_, void const* buf_, int long size_ ) {
	int long nWritten( -1 );
	if ( fd_ < SystemIO::MANAGED_IO )
		nWritten = ::write( fd_, buf_, size_ );
	else {
		SystemIO& sysIo( SystemIO::get_instance() );
		IO& io( *( sysIo.get_io( fd_ ).second ) );
		nWritten = io.write( buf_, size_ );
	}
	return ( nWritten );
}

M_EXPORT_SYMBOL
int stat( char const* path_, struct stat* s_ ) {
	string path( path_ );
	string::size_type lastNonSeparator( path.find_last_not_of( "/\\" ) );
	int len( path.length() );
	if ( lastNonSeparator != string::npos )
		path.erase( lastNonSeparator + 1 );
	else
		path.erase( 1 );
	int res( ::stat( path.c_str(), s_ ) );
	if ( ! res ) {
		if ( ( len != path.length() ) && ! ( S_IFDIR & s_->st_mode ) ) {
			res = -1;
			errno = ENOTDIR;
		} else {
			owner_t owner( get_path_owner( path ) );
			s_->st_uid = owner.first;
			s_->st_gid = owner.second;
		}
	}
	return ( res );
}

typedef __mode_t mode_t;

M_EXPORT_SYMBOL
mode_t umask( mode_t umask_ ) {
	static bool initialized( false );
	static mode_t currentUmask( 0 );
	mode_t oldUmask( ::umask( umask_ ) );
	if ( initialized )
		oldUmask = currentUmask;
	else {
		initialized = true;
		if ( ! oldUmask ) {
			/* Windows default umask is screwed. */
			oldUmask = 7;
		}
	}
	currentUmask = umask_;
	return ( oldUmask );
}

M_EXPORT_SYMBOL
char const* windows_strerror( int code_ ) {
	static int const MAX_MSG_LEN( 512 );
	static char msg[512];
	char* p( msg );

	FormatMessage(
		FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, code_, MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ),
		p, MAX_MSG_LEN - 1, NULL );

	return ( msg );
}

M_EXPORT_SYMBOL
void log_windows_error( char const* api_ ) {
	int err( GetLastError() );
	log << "ERROR: API = " << api_
		<< ", code = " << err
		<< ", message = " << windows_strerror( err ) << endl;
	return;
}

void get_memory_size_info( i64_t& free_, i64_t& total_ ) {
	MEMORYSTATUS ms;
	::GlobalMemoryStatus( &ms );
	free_ = ms.dwAvailVirtual;
	total_ = ms.dwTotalVirtual;
	return;
}

}

