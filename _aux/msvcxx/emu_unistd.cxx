#include <iterator>
#include <sstream>
#include <io.h>
#include <_aux/msvcxx/sys/time.h>
#include <_aux/msvcxx/unistd.h>
#include <_aux/msvcxx/sys/stat.h>
#undef stat
#define YAAL_MSVCXX_OPENSSL_RSA_H_INCLUDED 1
#include <openssl/ssl.h>

#define access access_off
#define lseek lseek_off

#include <csignal>

#include "emu_pwd.hxx"
#include "hcore/base.hxx"
#include "hcore/hlog.hxx"
#include "cleanup.hxx"

#include "cxxabi.h"

#include "cleanup.hxx"
#include "msio.hxx"
#include "msvcxx.hxx"
#include "emu_signals.hxx"

using namespace std;
using namespace yaal;
using namespace yaal::hcore;
using namespace yaal::tools;

namespace msvcxx {

struct OsCast {
	SystemIO& _sysIO;
	OsCast() : _sysIO( SystemIO::get_instance() ) {}
	IO* operator()( int id_ ) {
		return ( _sysIO.get_io( id_ ).second.get() );
	}
	HANDLE operator()( IO* io_ ) {
		io_->schedule_read();
		return ( io_->event() );
	}
};

M_EXPORT_SYMBOL
int select( int ndfs, fd_set* readFds, fd_set* writeFds, fd_set* exceptFds, struct timeval* timeout ) {
	int ret( 0 );
	do {
		int count( ( readFds ? readFds->_count : 0 ) + ( writeFds ? writeFds->_count : 0 ) );
		M_ENSURE( ( count + 1 ) <= MAXIMUM_WAIT_OBJECTS ); /* +1 for interrupt handler */
		int long milliseconds( timeout ? ( ( timeout->tv_sec * 1000 ) + ( timeout->tv_usec / 1000 ) ) : 0 );
		IO* ios[MAXIMUM_WAIT_OBJECTS];
		HANDLE handles[MAXIMUM_WAIT_OBJECTS];
		int offset( readFds ? readFds->_count : 0 );
		OsCast osCast;
		if ( readFds && writeFds ) {
			std::sort( readFds->_data, readFds->_data + readFds->_count );
			std::sort( writeFds->_data, writeFds->_data + writeFds->_count );
			int* end( std::set_difference( readFds->_data, readFds->_data + readFds->_count,
				writeFds->_data, writeFds->_data + writeFds->_count,
				readFds->_data ) );
			readFds->_count = end - readFds->_data;
		}
		if ( readFds ) {
			std::transform( readFds->_data, readFds->_data + readFds->_count, ios, osCast );
			for ( int i( 0 ); i < readFds->_count; ++ i ) {
				if ( ios[i]->ready() )
					++ ret;
			}
			if ( ret > 0 ) {
				for ( int i( 0 ); i < readFds->_count; ++ i ) {
					if ( ! ios[i]->ready() )
						readFds->_data[ i ] = -1;
				}
				std::remove( readFds->_data, readFds->_data + readFds->_count, -1 );
				readFds->_count = ret;
				if ( writeFds )
					FD_ZERO( writeFds );
				break; /* !!! Early exit. !!! */
			}
			std::transform( ios, ios + readFds->_count, handles, osCast );
		}
		if ( writeFds ) {
			std::transform( writeFds->_data, writeFds->_data + writeFds->_count, stdext::checked_array_iterator<IO**>( ios + offset, writeFds->_count), osCast );
			for ( int i( 0 ); i < writeFds->_count; ++ i ) {
				if ( ios[i + offset]->is_connected() )
					++ ret;
			}
			if ( ret > 0 ) {
				for ( int i( 0 ); i < writeFds->_count; ++ i ) {
					if ( ! ios[i + offset]->is_connected() )
						writeFds->_data[ i ] = -1;
				}
				std::remove( writeFds->_data, writeFds->_data + writeFds->_count, -1 );
				writeFds->_count = ret;
				if ( readFds )
					FD_ZERO( readFds );
				break; /* !!! Early exit. !!! */
			}
			std::transform( ios + offset, ios + count, handles, osCast );
		}
		HANDLE interrupt( _tlsSignalsSetup_->interrupt() );
		handles[count] = interrupt;
		int up( ::WaitForMultipleObjects( count + 1, handles, false, milliseconds ) );
		if ( up == WAIT_FAILED ) {
			ret = -1;
			log_windows_error( "WaitForMultipleObjects" );
		} else if ( ( up >= static_cast<int>( WAIT_OBJECT_0 ) ) && ( up < ( static_cast<int>( WAIT_OBJECT_0 ) + count ) ) ) {
			if ( readFds ) {
				for ( int i( 0 ); i < readFds->_count; ++ i ) {
					int upL( ::WaitForSingleObject( handles[ i ], 0 ) );
					if ( ( i == ( up - WAIT_OBJECT_0 ) ) || ( upL == WAIT_OBJECT_0 ) )
						++ ret;
					else
						readFds->_data[ i ] = -1;
				}
				std::remove( readFds->_data, readFds->_data + readFds->_count, -1 );
				readFds->_count = ret;
			}
			if ( writeFds ) {
				for ( int i( 0 ); i < writeFds->_count; ++ i ) {
					int upL( ::WaitForSingleObject( handles[ i + offset ], 0 ) );
					if ( ( ( i + offset ) == ( up - WAIT_OBJECT_0 ) ) || ( upL == WAIT_OBJECT_0 ) ) {
						if ( ! ios[i + offset]->is_connected() )
							ios[i + offset]->sync();
						++ ret;
					} else
						writeFds->_data[ i ] = -1;
				}
				std::remove( writeFds->_data, writeFds->_data + writeFds->_count, -1 );
				writeFds->_count = ( ret - ( readFds ? readFds->_count : 0 ) );
			}
		} else {
			if ( readFds )
				FD_ZERO( readFds );
			if ( writeFds )
				FD_ZERO( writeFds );
			milliseconds = 0;
			if ( up != WAIT_TIMEOUT ) {
				ret = -1;
				errno = EINTR;
			}
		}
		if ( ! ret && milliseconds ) {
			if ( ::WaitForSingleObject( interrupt, milliseconds ) == WAIT_OBJECT_0 ) {
				ret = -1;
				errno = EINTR;
			}
		}
	} while ( false );
	return ( ret );
}

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
		nullptr );

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
	int ret( 0 );
	if ( fd_ >= SystemIO::MANAGED_IO ) {
		SystemIO& sysIo( SystemIO::get_instance() );
		IO& io( *( sysIo.get_io( fd_ ).second ) );
		ret = io.fcntl( cmd_, arg_ );
	}
	return ( ret );
}

M_EXPORT_SYMBOL
int dup( int fd_ ) {
	int ret( -1 );
	if ( ( fd_ < SystemIO::MANAGED_IO ) && ( fd_ < SystemIO::MANAGED_IO ) ) {
		ret = _dup( fd_ );
	} else {
		SystemIO& sysIo( SystemIO::get_instance() );
		ret = sysIo.dup_io( fd_ );
	}
	return ( ret );
}

M_EXPORT_SYMBOL
int dup2( int fd1_, int fd2_ ) {
	int ret( -1 );
	if ( ( fd1_ < SystemIO::MANAGED_IO ) && ( fd2_ < SystemIO::MANAGED_IO ) ) {
		ret = _dup2( fd1_, fd2_ );
	} else {
		SystemIO& sysIo( SystemIO::get_instance() );
		ret = sysIo.dup2_io( fd1_, fd2_ );
	}
	return ( ret );
}

M_EXPORT_SYMBOL
int pipe( int* fds_ ) {
	SystemIO& sysIo( SystemIO::get_instance() );
	HANDLE hRead( nullptr );
	HANDLE hWrite( nullptr );
	bool ok( CreatePipeEx( &hRead, &hWrite, nullptr, 4096, FILE_FLAG_OVERLAPPED , FILE_FLAG_OVERLAPPED ) ? true : false );
	if ( ok ) {
		SystemIO::io_t readIO( sysIo.create_io( IO::TYPE::PIPE, hRead ) );
		SystemIO::io_t writeIO( sysIo.create_io( IO::TYPE::PIPE, hWrite ) );
		fds_[0] = readIO.first;
		fds_[1] = writeIO.first;
	}
	return ( ok ? 0 : -1 );
}

#undef isatty /* original isatty comes from VS */
M_EXPORT_SYMBOL
int isatty( int fd_ ) {
	static bool initialized( false );
	static char const* ANSICON( nullptr );
	static bool ansiSysLoaded( false );
	static CMutex m;
	if ( ! initialized ) {
		CLock l( m );
		if ( ! initialized ) {
			ansiSysLoaded = get_drivers().count( "ansi.sys" ) > 0;
			ANSICON = ::getenv( "ANSICON" );
		}
		initialized = true;
	}
	int val( ::isatty( fd_ ) );
	do {
		if ( ( fd_ != STDIN_FILENO ) && ( fd_ != STDOUT_FILENO ) && ( fd_ != STDERR_FILENO ) ) {
			break;
		}
		if ( val && ( ANSICON || ansiSysLoaded ) ) {
			break;
		}
		HANDLE h( reinterpret_cast<HANDLE>( _get_osfhandle( fd_ ) ) );
		val = 0;
		if ( h == INVALID_HANDLE_VALUE ) {
			break;
		}
		DWORD st( 0 );
		if ( GetConsoleMode( h, &st ) ) {
			val = 1;
			break;
		}
		if ( GetFileType( h ) != FILE_TYPE_PIPE ) {
			break;
		}
		int const bufSize( MAX_PATH + static_cast<int>( sizeof ( FILE_NAME_INFO ) ) );
		HChunk c( bufSize );
		if ( ! GetFileInformationByHandleEx( h, FileNameInfo, c.raw(), bufSize ) ) {
			break;
		}
		FILE_NAME_INFO* fni( c.get<FILE_NAME_INFO>() );
		HString n;
		for ( int i( 0 ); i < static_cast<int>( fni->FileNameLength ); ++ i ) {
			n.push_back( code_point_t( fni->FileName[i] ) );
		}
		if ( ( n.find( "\\cygwin-" ) != 0 ) && ( n.find( "\\msys-" ) != 0 ) ) {
			break;
		}
		if ( n.find( "-pty" ) == HString::npos ) {
			break;
		}
		val = 1;
	} while ( false );
	return ( val );
}

int close( int fd_ ) {
	int ret( 0 );
	if ( fd_ < SystemIO::MANAGED_IO ) {
		ret = ::close( fd_ );
	} else {
		ret = SystemIO::get_instance().close_io( fd_ );
	}
	if ( ret < 0 ) {
		log_windows_error( "close" );
	}
	return ( ret );
}

M_EXPORT_SYMBOL
int long read( int fd_, void* buf_, int long size_ ) {
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
int long write( int fd_, void const* buf_, int long size_ ) {
	int long nWritten( -1 );
	if ( fd_ < SystemIO::MANAGED_IO ) {
		nWritten = ::write( fd_, buf_, size_ );
	} else {
		SystemIO& sysIo( SystemIO::get_instance() );
		IO& io( *( sysIo.get_io( fd_ ).second ) );
		nWritten = io.write( buf_, size_ );
	}
	return ( nWritten );
}

M_EXPORT_SYMBOL
int long sendto( int fd_, void const* buf_, int long size_, int flags_, sockaddr const* to_, int toLen_ ) {
	SystemIO& sysIo( SystemIO::get_instance() );
	IO& io( *( sysIo.get_io( fd_ ).second ) );
	return (
		io.sendto( static_cast<char const*>( buf_ ), static_cast<int>( size_ ), flags_, to_, toLen_ )
	);
}

M_EXPORT_SYMBOL
int long recvfrom( int fd_, void* buf_, int long size_, int flags_, sockaddr* from_, int* fromLen_ ) {
	SystemIO& sysIo( SystemIO::get_instance() );
	IO& io( *( sysIo.get_io( fd_ ).second ) );
	return (
		io.recvfrom( static_cast<char*>( buf_ ), static_cast<int>( size_ ), flags_, from_, fromLen_ )
	);
}

int unix_stat( char const* path_, struct stat* s_ ) {
	string path( path_ );
	size_t dotLnkPos( path.rfind( ".lnk" ) );
	bool isLink( ( dotLnkPos != string::npos ) && ( dotLnkPos == ( path.length() - 4 ) ) );
	string::size_type lastNonSeparator( path.find_last_not_of( "/\\" ) );
	int len( path.length() );
	if ( lastNonSeparator != string::npos ) {
		path.erase( lastNonSeparator + 1 );
	} else {
		path.erase( 1 );
	}
	int res( ::stat( path.c_str(), s_ ) );
	if ( ! res ) {
		if ( ( len != path.length() ) && ! ( S_IFDIR & s_->st_mode ) ) {
			res = -1;
			errno = ENOTDIR;
		} else {
			if ( isLink ) {
				s_->st_mode &= ~TYPE_REG;
				s_->st_mode |= TYPE_LNK;
			}
			owner_t owner( get_path_owner( path ) );
			s_->st_uid = owner.first;
			s_->st_gid = owner.second;
			s_->st_ctime = s_->st_mtime = yaal::max( s_->st_ctime, s_->st_mtime );
			OFSTRUCT of;
			HFILE hf( ::OpenFile( path_, &of, 0 ) );
			if ( hf != HFILE_ERROR ) {
				HANDLE h( reinterpret_cast<HANDLE>( hf ) );
				BY_HANDLE_FILE_INFORMATION fi;
				if ( ::GetFileInformationByHandle( h, &fi ) ) {
#if SIZE_INO_T == 8
					s_->st_ino = ( static_cast<_ino_t>( fi.nFileIndexHigh ) << 32 ) | static_cast<_ino_t>( fi.nFileIndexLow );
#else
					s_->st_ino = static_cast<_ino_t>( fi.nFileIndexLow );
#endif
				}
				::CloseHandle( h );
			}
		}
	}
	return ( res );
}

mode_t umask( mode_t umask_ ) {
	static bool initialized( false );
	static mode_t currentUmask( 0 );
	mode_t oldUmask( ::_umask( umask_ ) );
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
int lockf( int, int, int long long ) {
	return ( 0 );
}

M_EXPORT_SYMBOL
char const* windows_strerror( int code_ ) {
	static int const MAX_MSG_LEN( 1024 );
	static char msg[MAX_MSG_LEN];
	char* p( msg );

	FormatMessage(
		FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		nullptr, code_, MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ),
		p, MAX_MSG_LEN - 1, nullptr );

	return ( msg );
}

M_EXPORT_SYMBOL
void log_windows_error( char const* api_ ) {
	int err( GetLastError() );
	hcore::log << "ERROR: API = " << api_
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

