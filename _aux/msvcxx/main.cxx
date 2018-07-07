#define WinMain WinMain_off
#define fd_set fd_set_off
#include <winsock2.h>
#include <windows.h>
#include <process.h>
#undef fd_set
#include <dbghelp.h>
#include <sddl.h>
#include <psapi.h>
#undef WinMain
#include <sstream>
#include <iomanip>
#include <iostream>
#include <ctime>

#include "unistd.h"
#include "csignal"
#include "dirent.h"
#include "pwd.h"
#include "grp.h"
#include "dlfcn.h"
#include "termios.h"
#include "sys/mman.h"
#include "sys/resource.h"
#include "sys/socket.h"
#include "sys/time.h"
#include "sys/statvfs.h"

#include "hcore/base.hxx"
#include "hcore/memory.hxx"
#include "cxxabi.h"
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
namespace {
SystemIO const& _systemIOInit_( SystemIO::get_instance() );
namespace limits {
rlimit _data_{ RLIM_INFINITY, RLIM_INFINITY };
rlimit _stack_{ RLIM_INFINITY, RLIM_INFINITY };
rlimit _nofile_{ RLIM_INFINITY, RLIM_INFINITY };
}
}

namespace abi {

char* __cxa_demangle( char const* a, int, int, int* ) {
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
	int got( ::CaptureStackBackTrace( 0, toFetch, buf_, nullptr ) );
	if ( ( toFetch > 0 ) && ( got < 1 ) && ( ::GetLastError() != 0 ) )
		log_windows_error( "CaptureStackBackTrace" );
	static bool once( false );
	if ( ! once ) {
		once = true;
		/* ::SymSetOptions( SYMOPT_UNDNAME ); */
		if ( ! ::SymInitialize( ::GetCurrentProcess(), nullptr, true ) ) {
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
		strings = nullptr;
	}
	return ( strings );
}

int setenv( char const* name_, char const* value_, int replace_ ) {
	char const* orig( ::getenv( name_ ) );
	int ret( 1 );
	if ( !orig || replace_ ) {
		HString s( name_ );
		s.append( "=" ).append( value_ );
		HUTF8String utf8( s );
		ret = ::putenv( utf8.c_str() );
	}
	return ( ret );
}

int unsetenv( char const* name_ ) {
	HString n( name_ );
	n += "=";
	HUTF8String utf8( n );
	int ret( ::putenv( utf8.c_str() ) );
	return ( ret );
}

char* basename( char* path_ ) {
	char* u( strrchr( path_, '/' ) );
	char* w( strrchr( path_, '\\' ) );
	char* r( path_ );
	if ( u || w ) {
		if ( u && w ) {
			r = ( u - w > 0 ) ? u + 1 : w + 1;
		} else {
			r = u ? u + 1 : w + 1;
		}
	}
	return ( r );
}

char* strptime( char const* source_, char const* format_, struct tm* broken_ ) {
	string s( source_ );
	s.append( "~\b~\b~\b" );
	istringstream source( s );
	source.imbue( locale( setlocale( LC_ALL, nullptr ) ) );
	source >> get_time( broken_, format_ );
	return ( ! source.fail() ? const_cast<char*>( source_ + static_cast<int>( source.tellg() ) ) : nullptr );
}

struct DIR {
	HANDLE _handle;
	bool _hasData;
	int _index;
	WIN32_FIND_DATA _data;
	dirent _payload;
};

DIR* opendir( char const* path_ ) {
	WIN32_FIND_DATA d;
	HString path( path_ );
	path.append( "/*" );
	HUTF8String utf8( path );
	HANDLE h( ::FindFirstFile( utf8.c_str(), &d ) );
	DIR* dir( nullptr );
	if ( h != INVALID_HANDLE_VALUE ) {
		dir = memory::calloc<DIR>( 1 );
		dir->_handle = h;
		::memcpy( &dir->_data, &d, sizeof( d ) );
		dir->_hasData = true;
		dir->_index = 0;
	}
	return ( dir );
}

int closedir( DIR* dir_ ) {
	BOOL ok( ::FindClose( dir_->_handle ) );
	memory::free0( dir_ );
	return ( ok ? 0 : -1 );
}

struct dirent* readdir( DIR* dir_ ) {
	bool found( false );
	if ( dir_->_hasData ) {
		dir_->_payload.d_ino = dir_->_index;
		if ( dir_->_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) {
			dir_->_payload.d_type = DT_DIR;
		} else if ( dir_->_data.dwFileAttributes & FILE_ATTRIBUTE_DEVICE ) {
			dir_->_payload.d_type = DT_BLK;
		} else if ( dir_->_data.dwFileAttributes & FILE_ATTRIBUTE_NORMAL ) {
			dir_->_payload.d_type = DT_REG;
		} else if ( dir_->_data.dwFileAttributes & ( FILE_ATTRIBUTE_ARCHIVE | FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_SYSTEM ) ) {
			dir_->_payload.d_type = DT_REG;
		} else {
			dir_->_payload.d_type = DT_UNKNOWN;
		}
		strncpy( dir_->_payload.d_name, dir_->_data.cFileName, NAME_MAX );
		if ( ::FindNextFile( dir_->_handle, &dir_->_data ) ) {
			++ dir_->_index;
		} else {
			dir_->_hasData = false;
		}
		found = true;
	}
	return ( found ? &dir_->_payload : nullptr );
}

PSID get_base_sid( char* buffer_, int size_, TOKEN_INFORMATION_CLASS tic_ ) {
	/* Open the access token associated with the calling process. */
	HANDLE hToken( nullptr );
	if ( ! ::OpenProcessToken( ::GetCurrentProcess(), TOKEN_QUERY, &hToken ) ) {
		log_windows_error( "OpenProcessToken" );
		return ( nullptr );
	}

	::memset( buffer_, 0, size_ );
	/* Retrieve the token information in a TOKEN_USER structure. */
	DWORD dummy( 0 );
	if ( ! ::GetTokenInformation( hToken, tic_, buffer_, size_, &dummy ) ) {
		log_windows_error( "GetTokenInformation" );
		return ( nullptr );
	}

	::CloseHandle( hToken );
	PTOKEN_USER tokenUser( static_cast<PTOKEN_USER>( static_cast<void*>( buffer_ ) ) );
	if ( ! ::IsValidSid( tokenUser->User.Sid ) ) {
		log_windows_error( "IsValidSid" );
		return ( nullptr );
	}
	return ( tokenUser->User.Sid );
}

uid_t getuid( void ) {
	static int const SID_SIZE( 128 );
	static int const TOKEN_USER_SIZE( sizeof ( TOKEN_USER ) + SID_SIZE );
	char tokenUserBuffer[ TOKEN_USER_SIZE ];
	PSID sid( get_base_sid( tokenUserBuffer, TOKEN_USER_SIZE, TokenUser ) );
	int uid( sid ? *::GetSidSubAuthority( sid, *::GetSidSubAuthorityCount( sid ) - 1 ) : -1 );
	return ( uid );
}


uid_t getgid( void ) {
	static int const SID_SIZE( 128 );
	static int const TOKEN_USER_SIZE( sizeof ( TOKEN_USER ) + SID_SIZE );
	char tokenUserBuffer[ TOKEN_USER_SIZE ];
	PSID sid( get_base_sid( tokenUserBuffer, TOKEN_USER_SIZE, TokenPrimaryGroup ) );
	int uid( sid ? *::GetSidSubAuthority( sid, *::GetSidSubAuthorityCount( sid ) - 1 ) : -1 );
	return ( uid );
}

int readlink( char const* path_, char* buffer_, size_t size_ ) {
	int len( -1 );
	HANDLE h( ::CreateFile( path_, GENERIC_READ, 0, nullptr, OPEN_EXISTING, 0, nullptr ) );
	if ( h != INVALID_HANDLE_VALUE ) {
		/* Try to resolve cygwin symlink. */
		char tag[] = "!<symlink>\xFF\xFE";
		static int const tagSize( sizeof ( tag ) - 1 );
		DWORD sz( ::GetFileSize( h, nullptr ) );
		if ( sz != INVALID_FILE_SIZE ) {
			int linkSize( static_cast<int>( sz - tagSize ) / 2 );
			if ( ( sz <= ( _XOPEN_NAME_MAX * 2 + tagSize ) ) && ( linkSize < static_cast<int>( size_ ) ) ) {
				HChunk buf( static_cast<int>( sz ) );
				DWORD nRead( 0 );
				char* p( buf.get<char>() );
				if ( ::ReadFile( h, p, sz, &nRead, nullptr ) && ( nRead == sz ) && ( ::strncmp( p, tag, tagSize ) == 0 ) ) {
					len = 0;
					p += tagSize;
					while ( len < linkSize ) {
						buffer_[len] = p[len * 2];
						++ len;
					}
					buffer_[len] = 0;
				}
			} else {
				len = static_cast<int>( size_ );
			}
		}
		if ( len < 0 ) {
			len = static_cast<int>( ::GetFinalPathNameByHandle( h, buffer_, size_, VOLUME_NAME_NONE ) );
		}
		::CloseHandle( h );
	}
	return ( len );
}

bool get_system_account_name( int id_, char* buf_, int size_ ) {
	static int const SID_SIZE( 128 );
	static int const TOKEN_USER_SIZE( sizeof ( TOKEN_USER ) + SID_SIZE );
	char tokenUserBuffer[ TOKEN_USER_SIZE ];
	PSID sid( get_base_sid( tokenUserBuffer, TOKEN_USER_SIZE, TokenUser ) );
	bool fail( true );
	if ( sid ) {
		LPTSTR sidStrBuffer( nullptr );
		if ( ::ConvertSidToStringSid( sid, &sidStrBuffer ) ) {
			HString sidStr( sidStrBuffer );
			::LocalFree( sidStrBuffer );
			sidStr.erase( sidStr.find_last( '-'_ycp ) + 1 );
			sidStr += id_;
			PSID newSid( nullptr );
			HUTF8String utf8( sidStr );
			if ( ::ConvertStringSidToSid( utf8.c_str(), &newSid ) ) {
				DWORD size( size_ );
				static int const DUMMY_BUFFER_SIZE = 128;
				char dummy[DUMMY_BUFFER_SIZE];
				DWORD dummyLen( DUMMY_BUFFER_SIZE );
				SID_NAME_USE eUse = SidTypeUnknown;
				if ( ::LookupAccountSid( nullptr, newSid, buf_, &size, dummy, &dummyLen, &eUse ) )
					fail = false;
				::LocalFree( newSid );
			}
		}
	}
	return ( fail );
}

int getpwuid_r( uid_t uid_, struct passwd* p_, char* buf_, int size_, struct passwd** result_ ) {
	p_->pw_name = buf_;
	int err( -1 );
	if ( ! get_system_account_name( uid_, buf_, size_ ) ) {
		err = 0;
		*result_ = p_;
	}
	return ( err );
}

int getgrgid_r( gid_t gid_, struct group* g_, char* buf_, int size_, struct group** result_ ) {
	g_->gr_name = buf_;
	int err( -1 );
	if ( ! get_system_account_name( gid_, buf_, size_ ) ) {
		err = 0;
		*result_ = g_;
	}
	return ( err );
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

void pthread_setname_np( pthread_t, char const* name_ ) {
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
	static int* (*msvcrt_errno)( void ) = nullptr;
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
	HMODULE appHandle( ::GetModuleHandle( nullptr ) );
	typedef int ( *main_type )( int, char** );
	main_type main = bit_cast<main_type>( ::GetProcAddress( appHandle, "main" ) );
	return ( main( nArgs, argv ) );
}

int getrusage( rusage_who_t who_, struct rusage* usage_ ) {
	int err( 0 );
	if ( who_ != RUSAGE_SELF ) {
		errno = EINVAL;
		err = -1;
	} else {
		PROCESS_MEMORY_COUNTERS pmc;
		::GetProcessMemoryInfo( ::GetCurrentProcess(), &pmc, sizeof ( pmc ) );
		usage_->ru_maxrss = pmc.WorkingSetSize;
	}
	return ( err );
}

int getrlimit( rlimit_resource_t limType_, struct rlimit* rl_ ) {
	errno = ENOSYS;
	rl_->rlim_cur = rl_->rlim_max = RLIM_INFINITY;
	switch ( limType_ ) {
		case ( RLIMIT_DATA ): {
			*rl_ = limits::_data_;
		} break;
		case ( RLIMIT_STACK ): {
			*rl_ = limits::_stack_;
		} break;
		case ( RLIMIT_NOFILE ): {
			*rl_ = limits::_nofile_;
		} break;
	}
	return ( 0 );
}

int setrlimit( rlimit_resource_t limType_, struct rlimit const* rl_ ) {
	errno = ENOSYS;
	switch ( limType_ ) {
		case ( RLIMIT_DATA ): {
			limits::_data_ = *rl_;
		} break;
		case ( RLIMIT_STACK ): {
			limits::_stack_ = *rl_;
		} break;
		case ( RLIMIT_NOFILE ): {
			limits::_nofile_ = *rl_;
		} break;
	}
	return ( 0 );
}

int long sysconf( int id_ ) {
	int long result( -1 );
	static int const PASS_BUF_SIZE( 1024 );
	SYSTEM_INFO si;
	GetSystemInfo( &si );
	switch ( id_ ) {
		case ( _SC_GETGR_R_SIZE_MAX ):
		case ( _SC_GETPW_R_SIZE_MAX ): {
			result = PASS_BUF_SIZE;
		} break;
		case ( _SC_PAGESIZE ): {
			result = si.dwPageSize;
		} break;
		case ( _SC_NPROCESSORS_ONLN ): {
			result = si.dwNumberOfProcessors;
		} break;
		default: {
			errno = EINVAL;
		}
	}
	return ( result );
}

int long pathconf( char const* path_, int id_ ) {
	DWORD volSerNo( 0 );
	static int const BUF_SIZE( 1024 );
	char volName[BUF_SIZE];
	DWORD volMaxComponentLength( 0 );
	DWORD volFSFlags( 0 );
	char volFSName[BUF_SIZE];
	GetVolumeInformation( path_, volName, BUF_SIZE, &volSerNo, &volMaxComponentLength, &volFSFlags, volFSName, BUF_SIZE );
	int long result( -1 );
	switch ( id_ ) {
		case ( _PC_NAME_MAX ):
		case ( _XOPEN_NAME_MAX ): {
			result = volMaxComponentLength;
		} break;
		default: {
			errno = EINVAL;
		}
	}
	return ( result );
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
			if ( ::GetDeviceDriverBaseName( drivers[i], szDriver, yaal::size( szDriver ) ) ) {
				int c( 0 );
				while ( ( c < yaal::size( szDriver ) ) && szDriver[c] ) {
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

int statvfs( char const* path_, struct statvfs* fs_ ) {
	int code( 0 );
	if ( path_ ) {
		DWORD sectorsPerCluster( 0 );
		DWORD bytesPerSector( 0 );
		DWORD freeClusters( 0 );
		DWORD clusters( 0 );
		GetDiskFreeSpace( path_, &sectorsPerCluster, &bytesPerSector, &freeClusters, &clusters );
		fs_->f_bsize = bytesPerSector;
		fs_->f_frsize = bytesPerSector;
		DWORD bytesPerCluster( bytesPerSector * sectorsPerCluster );
		yaal::u64_t free( bytesPerCluster * freeClusters );
		yaal::u64_t total( bytesPerCluster * clusters );
		fs_->f_blocks = total / bytesPerSector;
		fs_->f_bfree = free / bytesPerSector;
		fs_->f_bavail = fs_->f_bfree;
	} else {
		code = -1;
		errno = ENOENT;
	}
	return ( code );
}

int gettimeofday( struct timeval* tv_, struct timezone* tz_ ) {
	int ret( 0 );
	if ( tz_ ) {
		memset( tz_, 0, sizeof ( *tz_ ) );
		TIME_ZONE_INFORMATION tzi;
		DWORD ret( GetTimeZoneInformation( &tzi ) );
		if ( ret != TIME_ZONE_ID_INVALID ) {
			tz_->tz_dsttime = tzi.Bias;
			tz_->tz_dsttime = ( ret == TIME_ZONE_ID_DAYLIGHT ) ? 1 : 0;
		} else {
			ret = -1;
			/* *TODO* *FIXME* Set errno accordingly. */
		}
	}
	if ( tv_ ) {
		FILETIME ft;
		ULARGE_INTEGER li;
		GetSystemTimeAsFileTime( &ft );
		static int long unsigned HECTONANOSEC_PER_SEC( 10000000 );
		static int long long WIN_TO_UNIX_EPOCH( 11644473600LL );
		li.LowPart = ft.dwLowDateTime;
		li.HighPart = ft.dwHighDateTime;
		li.QuadPart -= ( WIN_TO_UNIX_EPOCH * HECTONANOSEC_PER_SEC );
		tv_->tv_sec = static_cast<int long>( li.QuadPart / HECTONANOSEC_PER_SEC );
		tv_->tv_usec = ( li.QuadPart % HECTONANOSEC_PER_SEC ) / 10;
	}
	return ( ret );
}

void* dlopen( char const* name_, int flags_ ) {
	void* dll( nullptr );
	DWORD flags( ( flags_ & RTLD_NOW ) ? 0 : DONT_RESOLVE_DLL_REFERENCES );
	if ( ! name_ ) {
		dll = GetModuleHandle( nullptr );
	} else {
		dll = LoadLibraryEx( name_, nullptr, flags );
	}
	return ( dll );
}

int dlclose( void* dll_ ) {
	return ( FreeLibrary( static_cast<HMODULE>( dll_ ) ) ? 0 : -1 );
}

char const* dlerror( void ) {
	return ( msvcxx::windows_strerror( GetLastError() ) );
}

void* dlsym( void* dll_, char const* name_ ) {
	return ( GetProcAddress( static_cast<HMODULE>( dll_ ), name_ ) );
}

void* mmap( void* ptr_, size_t size_, int mode_, int access_, int fd_, off_t off_ ) {
	void* ptr( MAP_FAILED );
	do {
		if ( ptr_ ) {
			errno = EINVAL;
			break;
		}
		if ( off_ ) {
			errno = EINVAL;
			break;
		}
		DWORD mapFlags( 0 );
		DWORD viewFlags( 0 );
		if ( ( mode_ == ( PROT_READ | PROT_WRITE ) ) && ( access_ == MAP_PRIVATE ) ) {
			mapFlags = PAGE_READWRITE;
			viewFlags = FILE_MAP_COPY;
		} else {
			errno = EINVAL;
			break;
		}
		DWORD size( static_cast<DWORD>( yaal::min<yaal::i64_t>( _filelengthi64( fd_ ), size_ ) ) );
		HANDLE h(
			CreateFileMapping( (HANDLE)_get_osfhandle( fd_ ), nullptr, mapFlags, 0, size, nullptr )
		);
		if ( ! h ) {
			errno = EINVAL;
			break;
		}
		ptr = MapViewOfFileEx( h, viewFlags, 0, 0, size, nullptr );
		CloseHandle( h );
		if ( ! ptr ) {
			ptr = MAP_FAILED;
			errno = EINVAL;
			break;
		}
	} while ( false );
	return ( ptr );
}

int munmap( void* ptr_, size_t ) {
	return ( UnmapViewOfFile( ptr_ ) ? 0 : -1 );
}

int tcgetattr( int, struct termios* ) {
	errno = ENOSYS;
	return ( -1 );
}

int cfsetospeed( struct termios*, speed_t ) {
	errno = ENOSYS;
	return ( -1 );
}

int cfsetispeed( struct termios*, speed_t ) {
	errno = ENOSYS;
	return ( -1 );
}

int tcsetattr( int, int, struct termios const* ) {
	errno = ENOSYS;
	return ( -1 );
}

int tcflush( int, int ) {
	errno = ENOSYS;
	return ( -1 );
}

int tcdrain( int ) {
	errno = ENOSYS;
	return ( -1 );
}

int tcsendbreak( int, int ) {
	errno = ENOSYS;
	return ( -1 );
}
