#include <sys/cdefs.h>
#include <hash_map>
#include <sstream>
#define fd_set fd_set_win_off
#include <ws2tcpip.h>
#include <io.h>
#include <../include/fcntl.h>

#define timeval timeval_off
#define dup dup_off
#define dup2 dup2_off

#undef fd_set
#undef FD_SET
#undef FD_CLR
#undef FD_ISSET
#undef FD_ZERO
#undef FD_SETSIZE
#undef timerclear
#undef timercmp
#undef O_RDONLY
#undef O_WRONLY
#undef O_RDWR
#undef O_CREAT
#undef O_EXCL
#undef O_TRUNC
#undef O_APPEND
#undef fd_set
#undef FD_SET

#include <sys/time.h>
#define _FCNTL_H 1
#include <bits/fcntl.h>

#undef timeval

#include "hcore/base.hxx"
#include "hcore/hexception.hxx"
#include "cleanup.hxx"
#include "crit.hxx"
#include "emu_unistd.hxx"

using namespace std;
using namespace yaal;

int get_socket_error( void )
	{
	return ( WSAGetLastError() );
	}

void set_socket_error( int errno_ )
	{
	WSASetLastError( errno_ );
	}

int APIENTRY CreatePipeEx( LPHANDLE lpReadPipe,
		LPHANDLE lpWritePipe,
		LPSECURITY_ATTRIBUTES lpPipeAttributes,
		DWORD nSize,
		DWORD dwReadMode,
		DWORD dwWriteMode )
	{
	HANDLE ReadPipeHandle, WritePipeHandle;
	DWORD dwError;

	// Only one valid OpenMode flag - FILE_FLAG_OVERLAPPED
	if ( ( dwReadMode | dwWriteMode ) & ( ~FILE_FLAG_OVERLAPPED ) )
		{
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

	if ( INVALID_HANDLE_VALUE == WritePipeHandle )
		{
		dwError = GetLastError();
		CloseHandle( ReadPipeHandle );
		SetLastError( dwError );
		return FALSE;
		}

	*lpReadPipe = ReadPipeHandle;
	*lpWritePipe = WritePipeHandle;
	return ( 1 );
	}

struct IO
	{
	struct TYPE
		{
		typedef enum
			{
			TERMINAL,
			PIPE,
			NAMED_PIPE,
			SOCKET
			} type_t;
		};
	TYPE::type_t _type;
	HANDLE _handle;
	OVERLAPPED _overlapped;
	char _buffer;
	bool _scheduled; /* io has been scheduled */
	bool _nonBlocking;
	string _path;
	IO( TYPE::type_t t_, HANDLE h_, HANDLE e_ = NULL, string const& p_ = string() )
		: _type( t_ ), _handle( h_ ), _overlapped(), _buffer( 0 ), _scheduled( false ), _nonBlocking( false ), _path( p_ )
		{
		_overlapped.hEvent = e_ ? e_ : ::CreateEvent( NULL, false, false, NULL );
		}
	~IO( void )
		{
		if ( _overlapped.hEvent )
			::CloseHandle( _overlapped.hEvent );
		}
private:
	IO( IO const& );
	IO& operator = ( IO const& );
	};

typedef tr1::shared_ptr<IO> io_ptr_t;

class SystemIO
	{
public:
	static int const MANAGED_IO = 0x10000;
private:
	typedef stdext::hash_map<int, io_ptr_t> io_table_t;
	io_table_t _ioTable;
	CMutex _mutex;
	int _idPool;
public:
	typedef io_table_t::value_type io_t;
	io_t& create_io( IO::TYPE::type_t type_, HANDLE h_, HANDLE e_ = NULL, string const& p_ = string() )
		{
		CLock l( _mutex );
		HANDLE e(  );
		return ( *( _ioTable.insert( std::make_pair( _idPool ++, io_ptr_t( new IO( type_, h_, e_, p_ ) ) ) ).first ) );
		}
	io_t& get_io( int id_ )
		{
		CLock l( _mutex );
		io_table_t::iterator i( _ioTable.find( id_ ) );
		if ( i != _ioTable.end() )
			return ( *i );
		M_ASSERT( id_ < MANAGED_IO );
		HANDLE h( reinterpret_cast<HANDLE>( _get_osfhandle( id_ ) ) );
		return ( *( _ioTable.insert( std::make_pair( id_, io_ptr_t( new IO( IO::TYPE::TERMINAL, h, h ) ) ) ).first ) );
		}
	void erase_io( int id_ )
		{
		CLock l( _mutex );
		_ioTable.erase( id_ );
		}
	static SystemIO& get_instance( void )
		{
		static SystemIO instance;
		return ( instance );
		}
private:
	SystemIO( void ) : _ioTable(), _mutex(), _idPool( MANAGED_IO ) {}
	};

extern "C" int fcntl( int fd_, int cmd_, ... );

int do_unix_fcntl( int fd_, int cmd_, void* arg_ )
	{
	int ret( -1 );
	if ( fd_ < SystemIO::MANAGED_IO )
		ret = fcntl( fd_, cmd_, arg_ );
	else if ( cmd_ == F_SETFL )
		{
		int arg( reinterpret_cast<int>( arg_ ) );
		SystemIO& sysIo( SystemIO::get_instance() );
		IO& io( *( sysIo.get_io( fd_ ).second ) );
		io._nonBlocking = ( ( arg & O_NONBLOCK ) ? true : false );
		}
	else if ( cmd_ == F_GETFL )
		{
		int* arg( static_cast<int*>( arg_ ) );
		SystemIO& sysIo( SystemIO::get_instance() );
		IO& io( *( sysIo.get_io( fd_ ).second ) );
		*arg = io._nonBlocking ? O_NONBLOCK : 0;
		}
	return ( ret );
	}

M_EXPORT_SYMBOL
int unix_fcntl( int fd_, int cmd_, ... )
	{
  va_list ap;
  void *arg;

  va_start( ap, cmd_ );
  arg = va_arg( ap, void* );
  va_end( ap );

	if ( fd_ < 0 )
		{
		SetLastError( EBADF );
		return -1;
		}
	return do_unix_fcntl( fd_, cmd_, arg );
	}


void sched_read( IO& io_ )
	{
	DWORD nRead( 0 );
	::ReadFile( io_._handle, &io_._buffer, 1, &nRead, &io_._overlapped );
	io_._scheduled = true;
	return;
	}

M_EXPORT_SYMBOL
int unix_pipe( int* fds_ )
	{
	SystemIO& sysIo( SystemIO::get_instance() );
	HANDLE hRead( NULL );
	HANDLE hWrite( NULL );
	bool ok( ::CreatePipeEx( &hRead, &hWrite, NULL, 4096, FILE_FLAG_OVERLAPPED , FILE_FLAG_OVERLAPPED ) ? true : false );
	if ( ok )
		{
		SystemIO::io_t readIO( sysIo.create_io( IO::TYPE::PIPE, hRead ) );
		SystemIO::io_t writeIO( sysIo.create_io( IO::TYPE::PIPE, hWrite ) );
		fds_[0] = readIO.first;
		fds_[1] = writeIO.first;
		sched_read( *(readIO.second) );
		}
	return ( ok ? 0 : -1 );
	}

int unix_close( int const& fd_ )
	{
	SystemIO& sysIo( SystemIO::get_instance() );
	int ret( 0 );
	IO& io( *( sysIo.get_io( fd_ ).second ) );
	if ( ( io._type == IO::TYPE::PIPE ) || ( io._type == IO::TYPE::NAMED_PIPE ) )
		ret = ( ::CloseHandle( io._handle ) == S_OK ) ? 0 : -1;
	else if ( io._type == IO::TYPE::SOCKET )
		ret = ::closesocket( reinterpret_cast<SOCKET>( io._handle ) );
	else
		{
		M_ASSERT( ! "invalid HANDLE" );
		}
	sysIo.erase_io( fd_ );
	return ( ret );
	}

M_EXPORT_SYMBOL
int long unix_read( int const& fd_, void* buf_, int long size_ )
	{
	SystemIO& sysIo( SystemIO::get_instance() );
	DWORD nRead( 0 );
	IO& io( *( sysIo.get_io( fd_ ).second ) );
	int off( 0 );
	bool ok( false );
	if ( io._scheduled )
		{
		static_cast<char*>( buf_ )[0] = io._buffer;
		io._scheduled = false;
		-- size_;
		++ off;
		ok = true;
		}
	if ( size_ > 0 )
		{
		ok = ::ReadFile( io._handle, static_cast<char*>( buf_ ) + off, size_ - off, &nRead, &io._overlapped ) ? true : false;
		if ( ! ok )
			log_windows_error( "ReadFile" );
		ok = ::GetOverlappedResult( io._handle, &io._overlapped, &nRead, false ) ? true : false;
		if ( ! ok )
			log_windows_error( "GetOverlappedResult" );
		}
	if ( ! ( nRead + off ) && ( io._type != IO::TYPE::SOCKET ) )
		sched_read( io );
	return ( ok ? nRead + off : -1 );
	}

M_EXPORT_SYMBOL
int long unix_write( int const& fd_, void const* buf_, int long size_ )
	{
	SystemIO& sysIo( SystemIO::get_instance() );
	DWORD nWritten( 0 );
	IO& io( *( sysIo.get_io( fd_ ).second ) );
	bool ok( ::WriteFile( io._handle, buf_, size_, &nWritten, NULL ) ? true : false );
	return ( ok ? nWritten : -1 );
	}

struct OsCast
	{
	SystemIO& _sysIO;
	OsCast() : _sysIO( SystemIO::get_instance() ) {}
	IO* operator()( int id_ )
		{
		return ( _sysIO.get_io( id_ ).second.get() );
		}
	HANDLE operator()( IO* io_ )
		{
		return ( io_->_overlapped.hEvent );
		}
	};

M_EXPORT_SYMBOL
int unix_select( int ndfs, fd_set* readFds, fd_set* writeFds, fd_set* exceptFds, struct timeval* timeout )
	{
	int ret( 0 );
	int long miliseconds( timeout ? ( ( timeout->tv_sec * 1000 ) + ( timeout->tv_usec / 1000 ) ) : 0 );
	if ( readFds )
		{
		int count( ( readFds ? readFds->_count : 0 ) + ( writeFds ? writeFds->_count : 0 ) );
		M_ENSURE( count <= MAXIMUM_WAIT_OBJECTS );
		IO* ios[MAXIMUM_WAIT_OBJECTS];
		HANDLE handles[MAXIMUM_WAIT_OBJECTS];
		OsCast osCast;
		std::transform( readFds->_data, readFds->_data + readFds->_count, ios, osCast );
		std::transform( ios, ios + readFds->_count, handles, osCast );
 		int up( ::WaitForMultipleObjects( readFds->_count, handles, false, miliseconds ) );
		if ( up == WAIT_FAILED )
			{
			ret = -1;
			log_windows_error( "WaitForMultipleObjects" );
			}
		else if ( ( up >= WAIT_OBJECT_0 ) && ( up < ( WAIT_OBJECT_0 + count ) ) )
			{
			for ( int i( 0 ); i < count; ++ i )
				{
				int upL( ::WaitForSingleObject( handles[ i ], 0 ) );
				if ( ( i == ( up - WAIT_OBJECT_0 ) || ( upL == WAIT_OBJECT_0 ) ) )
					{
					++ ret;
					DWORD nRead( 0 );
					if ( ios[i]->_scheduled )
						{
						if ( ! ( ::GetOverlappedResult( ios[i]->_handle, &ios[i]->_overlapped, &nRead, true ) || nRead ) )
							log_windows_error( "GetOverlappedResult (scheduled)" );
						}
					}
				else
					readFds->_data[ i ] = -1;
				}
			std::remove( readFds->_data, readFds->_data + readFds->_count, -1 );
			readFds->_count = ret;
			ret += ( writeFds ? writeFds->_count : 0 );
			}
		else
			FD_ZERO( readFds );
		}
	else
		{
		M_ASSERT( timeout );
		::Sleep( miliseconds );
		}
	return ( ret );
	}

namespace msvcxx	
{

int unix_getaddrinfo( char const* node_,
								char const* service_,
								struct addrinfo const* hints_,
								struct addrinfo** res_ )
	{
	return ( getaddrinfo( node_, service_, hints_, res_ ) );
	}

void unix_freeaddrinfo( struct addrinfo* res_ )
	{
	freeaddrinfo( res_ );
	}

int unix_getnameinfo( struct sockaddr const* sa_,
								 socklen_t salen_,
								 char* host_, size_t hostlen_,
                 char* serv_, size_t servlen_,
                 int flags_ )
	{
	return ( getnameinfo( sa_, salen_, host_, hostlen_, serv_, servlen_, flags_ ) );
	}

int make_pipe_instance( IO& io_ )
	{
	io_._handle = CreateNamedPipe( io_._path.c_str(),
		PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED,
		PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT,
		PIPE_UNLIMITED_INSTANCES, 1024, 1024, 1000, NULL );
	return ( io_._handle != INVALID_HANDLE_VALUE ? 0 : -1 );
	}

int unix_bind( int fd_, const struct sockaddr* addr_, socklen_t len_ )
	{
	int ret( -1 );
	SystemIO& sysIo( SystemIO::get_instance() );
	IO& io( *( sysIo.get_io( fd_ ).second ) );
	char const* path( addr_->sa_data + 2 );
	if ( addr_->sa_family == PF_UNIX )
		{
		M_ASSERT( io._type == IO::TYPE::NAMED_PIPE );
		HANDLE h = ::CreateFile( path,                // name of the write
                       GENERIC_WRITE,          // open for writing
                       0,                      // do not share
                       NULL,                   // default security
                       CREATE_NEW,          // overwrite existing
                       FILE_ATTRIBUTE_NORMAL,  // normal file
                       NULL);                  // no attr. template
		string n( "\\\\.\\pipe" );
		if ( h != INVALID_HANDLE_VALUE )
			{
			::CloseHandle( h );
			n += path;
			replace( n.begin(), n.end(), '/', '\\' );
			io._path = n;
			ret = make_pipe_instance( io );
			}
		}
	else
		{
		M_ASSERT( io._type == IO::TYPE::SOCKET );
		ret = ::bind( reinterpret_cast<SOCKET>( io._handle ), addr_, len_ );
		}
	return ( ret );
	}

int unix_socket( int af, int type, int protocol )
	{
	SOCKET s = -1;
	if ( af != PF_UNIX )
		s = ::socket( af, type, protocol );
	SystemIO& sysIo( SystemIO::get_instance() );
	SystemIO::io_t sock( sysIo.create_io( af == PF_UNIX ? IO::TYPE::NAMED_PIPE : IO::TYPE::SOCKET, reinterpret_cast<HANDLE>( s ) ) );
	return ( sock.first );
	}

int unix_listen( int const& fd_, int const& backlog_ )
	{
	int ret( 0 );
	SystemIO& sysIo( SystemIO::get_instance() );
	IO& io( *( sysIo.get_io( fd_ ).second ) );
	io._nonBlocking = true;
	if ( io._type == IO::TYPE::NAMED_PIPE )
		{
		if ( ::ConnectNamedPipe( io._handle, &io._overlapped ) )
			{
			log_windows_error( "ConnectNamedPipe" );
			ret = -1;
			}		
		}
	else
		{
		SOCKET s( reinterpret_cast<SOCKET>( io._handle ) );
		if ( WSAEventSelect( s, io._overlapped.hEvent, FD_ACCEPT ) )
			log_windows_error( "WSAEventSelect" );
		ret = listen( s, backlog_ );
		}
	return ( ret );
	}

int unix_accept( int fd_, struct sockaddr* addr_, socklen_t* len_ )
	{
	int ret( 0 );
	SystemIO& sysIo( SystemIO::get_instance() );
	IO& io( *( sysIo.get_io( fd_ ).second ) );
	if ( io._type == IO::TYPE::SOCKET )
		{
		int len = *len_;
		ret = ::accept( reinterpret_cast<SOCKET>( io._handle ), addr_, &len );
		SystemIO::io_t sock( sysIo.create_io( IO::TYPE::SOCKET, reinterpret_cast<HANDLE>( ret ) ) );
		if ( WSAEventSelect( ret, sock.second->_overlapped.hEvent, FD_ACCEPT | FD_CONNECT | FD_READ | FD_WRITE | FD_CLOSE ) )
			log_windows_error( "WSAEventSelect" );
		ret = sock.first;
		}
	else
		{
		SystemIO::io_t np( sysIo.create_io( IO::TYPE::NAMED_PIPE, io._handle ) );
		std::swap( io._overlapped, np.second->_overlapped );
		ret = make_pipe_instance( io );
		if ( ! ret )
			{
			ret = np.first;
			sched_read( *( np.second ) );
			}
		}
	return ( ret );
	}

int unix_connect( int fd_, struct sockaddr* addr_, socklen_t len_ )
	{
	int ret( 0 );
	SystemIO& sysIo( SystemIO::get_instance() );
	IO& io( *( sysIo.get_io( fd_ ).second ) );
	if ( addr_->sa_family == PF_UNIX )
		{
		char const* path( addr_->sa_data + 2 );
		string n( "\\\\.\\pipe" );
		n += path;
		replace( n.begin(), n.end(), '/', '\\' );
		HANDLE h( ::CreateFile( n.c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, 0 ) );
		if ( h == INVALID_HANDLE_VALUE )
			{
			log_windows_error( "CreateNamedPipe" );
			ret = -1;
			}
		else
			io._handle = h;
		}
	else
		{
		SOCKET s( reinterpret_cast<SOCKET>( io._handle ) );
		ret = ::connect( s, addr_, len_ );
		if ( WSAEventSelect( s, io._overlapped.hEvent, FD_ACCEPT | FD_CONNECT | FD_READ | FD_WRITE | FD_CLOSE ) )
			log_windows_error( "WSAEventSelect" );
		}
	return ( ret );
	}

int unix_shutdown( int fd_, int how_ )
	{
	return ( 0 );
	}

int unix_setsockopt( int fd_, int level_, int optname_, void const* optval_, socklen_t optlen_ )
	{
	int ret( 0 );
	SystemIO& sysIo( SystemIO::get_instance() );
	IO& io( *( sysIo.get_io( fd_ ).second ) );
	if ( io._type == IO::TYPE::SOCKET )
		ret = setsockopt( reinterpret_cast<SOCKET>( io._handle ), level_, optname_, static_cast<char const*>( optval_ ), optlen_ );
	return ( ret );
	}

}

