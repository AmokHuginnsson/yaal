#include <sys/cdefs.h>
#include <sstream>
#include <io.h>
#include <../include/fcntl.h>

#undef timerclear
#undef timercmp
#undef O_RDONLY
#undef O_WRONLY
#undef O_RDWR
#undef O_CREAT
#undef O_EXCL
#undef O_TRUNC
#undef O_APPEND

#include <sys/socket.h>
#define _FCNTL_H 1
#include <bits/fcntl.h>

#include "hcore/base.hxx"
#include "hcore/hexception.hxx"
#include "cleanup.hxx"
#include "emu_unistd.hxx"
#include "msio.hxx"

using namespace std;
using namespace yaal;

extern "C" int fcntl( int fd_, int cmd_, ... );

int get_socket_error( void )
	{
	return ( WSAGetLastError() );
	}

void set_socket_error( int errno_ )
	{
	WSASetLastError( errno_ );
	}

namespace msvcxx	
{

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

void sched_read( IO& io_ )
	{
	DWORD nRead( 0 );
	::ReadFile( io_._handle, &io_._buffer, 1, &nRead, &io_._overlapped );
	io_._scheduled = true;
	return;
	}

M_EXPORT_SYMBOL
int fcntl( int fd_, int cmd_, int arg_ )
	{
#undef fcntl
	int ret( 0 );
	if ( fd_ < SystemIO::MANAGED_IO )
		ret = fcntl( fd_, cmd_, arg_ );
	else if ( cmd_ == F_SETFL )
		{
		SystemIO& sysIo( SystemIO::get_instance() );
		IO& io( *( sysIo.get_io( fd_ ).second ) );
		io._nonBlocking = ( ( arg_ & O_NONBLOCK ) ? true : false );
		if ( ( io._nonBlocking ) && ( ( io._type == IO::TYPE::NAMED_PIPE ) || ( io._type == IO::TYPE::PIPE ) ) )
			sched_read( io );
		}
	else if ( cmd_ == F_GETFL )
		{
		SystemIO& sysIo( SystemIO::get_instance() );
		IO& io( *( sysIo.get_io( fd_ ).second ) );
		ret = io._nonBlocking ? O_NONBLOCK : 0;
		}
	return ( ret );
	}

M_EXPORT_SYMBOL
int dup2( int fd1_, int fd2_ )
	{
	int ret( -1 );
	if ( fd1_ < SystemIO::MANAGED_IO )
		ret = _dup2( fd1_, fd2_ );
	else
		{
		SystemIO& sysIo( SystemIO::get_instance() );
		IO& io( *( sysIo.get_io( fd1_ ).second ) );
		ret = _dup2( _open_osfhandle( reinterpret_cast<intptr_t>( io._handle ), 0 ), fd2_ );
		}
	return ( ret );
	}

M_EXPORT_SYMBOL
int pipe( int* fds_ )
	{
	SystemIO& sysIo( SystemIO::get_instance() );
	HANDLE hRead( NULL );
	HANDLE hWrite( NULL );
	bool ok( CreatePipeEx( &hRead, &hWrite, NULL, 4096, FILE_FLAG_OVERLAPPED , FILE_FLAG_OVERLAPPED ) ? true : false );
	if ( ok )
		{
		SystemIO::io_t readIO( sysIo.create_io( IO::TYPE::PIPE, hRead ) );
		SystemIO::io_t writeIO( sysIo.create_io( IO::TYPE::PIPE, hWrite ) );
		fds_[0] = readIO.first;
		fds_[1] = writeIO.first;
		}
	return ( ok ? 0 : -1 );
	}

int close( int const& fd_ )
	{
	SystemIO& sysIo( SystemIO::get_instance() );
	int ret( 0 );
	if ( fd_ < SystemIO::MANAGED_IO )
		ret = ::close( fd_ );
	else
		{
		IO& io( *( sysIo.get_io( fd_ ).second ) );
		if ( ( io._type == IO::TYPE::PIPE ) || ( io._type == IO::TYPE::NAMED_PIPE ) )
			ret = ::CloseHandle( io._handle ) ? 0 : -1;
		else if ( io._type == IO::TYPE::SOCKET )
			ret = ::closesocket( reinterpret_cast<SOCKET>( io._handle ) );
		else
			{
			M_ASSERT( ! "invalid HANDLE" );
			}
		}
	if ( ret < 0 )
		log_windows_error( "close" );
	sysIo.erase_io( fd_ );
	return ( ret );
	}

M_EXPORT_SYMBOL
int long read( int const& fd_, void* buf_, int long size_ )
	{
	int long nRead( -1 );
	if ( fd_ < SystemIO::MANAGED_IO )
		nRead = ::read( fd_, buf_, size_ ); 
	else
		{
		SystemIO& sysIo( SystemIO::get_instance() );
		IO& io( *( sysIo.get_io( fd_ ).second ) );
		DWORD iRead( 0 );
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
			ok = ::ReadFile( io._handle, static_cast<char*>( buf_ ) + off, size_ - off, &iRead, &io._overlapped ) ? true : false;
			if ( ! ok )
				log_windows_error( "ReadFile" );
			ok = ::GetOverlappedResult( io._handle, &io._overlapped, &iRead, ! io._nonBlocking ) ? true : false;
			if ( ! ok )
				log_windows_error( "GetOverlappedResult" );
			}
		if ( ! ( nRead + off ) && ( io._type != IO::TYPE::SOCKET ) )
			sched_read( io );
		nRead = ok ? iRead + off : -1;
		}
	return ( nRead );
	}

M_EXPORT_SYMBOL
int long write( int const& fd_, void const* buf_, int long size_ )
	{
	int long nWritten( -1 );
	if ( fd_ < SystemIO::MANAGED_IO )
		nWritten = ::write( fd_, buf_, size_ );
	else
		{
		SystemIO& sysIo( SystemIO::get_instance() );
		DWORD iWritten( 0 );
		IO& io( *( sysIo.get_io( fd_ ).second ) );
		bool ok( ::WriteFile( io._handle, buf_, size_, &iWritten, &io._overlapped ) ? true : false );
		ok = ::GetOverlappedResult( io._handle, &io._overlapped, &iWritten, true ) ? true : false;
		if ( ! ok )
			log_windows_error( "GetOverlappedResult" );
		nWritten = ok ? iWritten : -1;
		}
	return ( nWritten );
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
int select( int ndfs, fd_set* readFds, fd_set* writeFds, fd_set* exceptFds, struct timeval* timeout )
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
		else if ( ( up >= static_cast<int>( WAIT_OBJECT_0 ) ) && ( up < ( static_cast<int>( WAIT_OBJECT_0 ) + count ) ) )
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
	else if ( writeFds )
		ret = writeFds->_count;
	else
		{
		M_ASSERT( timeout );
		::Sleep( miliseconds );
		}
	return ( ret );
	}

int make_pipe_instance( IO& io_ )
	{
	io_._handle = CreateNamedPipe( io_._path.c_str(),
		PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED,
		PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT,
		PIPE_UNLIMITED_INSTANCES, 1024, 1024, 1000, NULL );
	return ( io_._handle != INVALID_HANDLE_VALUE ? 0 : -1 );
	}

int bind( int fd_, const struct sockaddr* addr_, socklen_t len_ )
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

#undef socket
int socket( int af, int type, int protocol )
	{
	SOCKET s = -1;
	if ( af != PF_UNIX )
		s = ::socket( af, type, protocol );
	SystemIO& sysIo( SystemIO::get_instance() );
	SystemIO::io_t sock( sysIo.create_io( af == PF_UNIX ? IO::TYPE::NAMED_PIPE : IO::TYPE::SOCKET, reinterpret_cast<HANDLE>( s ) ) );
	return ( sock.first );
	}

int listen( int fd_, int backlog_ )
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
		ret = ::listen( s, backlog_ );
		}
	return ( ret );
	}

int accept( int fd_, struct sockaddr* addr_, socklen_t* len_ )
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

int connect( int fd_, struct sockaddr* addr_, socklen_t len_ )
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
		ret = ::connect( s, const_cast<sockaddr const*>( addr_ ), static_cast<int>( len_ ) );
		if ( WSAEventSelect( s, io._overlapped.hEvent, FD_ACCEPT | FD_CONNECT | FD_READ | FD_WRITE | FD_CLOSE ) )
			log_windows_error( "WSAEventSelect" );
		}
	return ( ret );
	}

int shutdown( int fd_, int how_ )
	{
	SystemIO& sysIo( SystemIO::get_instance() );
	IO& io( *( sysIo.get_io( fd_ ).second ) );
	int ret( 0 );
	if ( io._type == IO::TYPE::SOCKET )
		ret = ::shutdown( reinterpret_cast<SOCKET>( io._handle ), how_ );
	return ( ret );
	}

int setsockopt( int fd_, int level_, int optname_, void const* optval_, socklen_t optlen_ )
	{
	int ret( 0 );
	SystemIO& sysIo( SystemIO::get_instance() );
	IO& io( *( sysIo.get_io( fd_ ).second ) );
	if ( io._type == IO::TYPE::SOCKET )
		ret = ::setsockopt( reinterpret_cast<SOCKET>( io._handle ), level_, optname_, static_cast<char const*>( optval_ ), optlen_ );
	return ( ret );
	}

int getsockopt( int fd_, int level_, int optname_, void* optval_, socklen_t* optlen_ )
	{
	int ret( 0 );
	SystemIO& sysIo( SystemIO::get_instance() );
	IO& io( *( sysIo.get_io( fd_ ).second ) );
	if ( io._type == IO::TYPE::SOCKET )
		ret = ::getsockopt( reinterpret_cast<SOCKET>( io._handle ), level_, optname_, static_cast<char*>( optval_ ), optlen_ );
	return ( ret );
	}

}

