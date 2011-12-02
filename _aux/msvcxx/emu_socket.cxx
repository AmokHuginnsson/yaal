#include <sys/cdefs.h>
#include <sys/socket.h>
#include <csignal>

#include "hcore/base.hxx"
#include "hcore/hexception.hxx"
#include "cleanup.hxx"
#include "emu_unistd.hxx"
#include "msio.hxx"
#include "emu_signals.hxx"

using namespace std;
using yaal::error_message;

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
	int long miliseconds( timeout ? ( ( timeout->tv_sec * 1000 ) + ( timeout->tv_usec / 1000 ) ) : 0 );
	if ( writeFds ) {
		ret = writeFds->_count;
		if ( readFds )
			readFds->_count = 0;
	} else if ( readFds ) {
		int count( ( readFds ? readFds->_count : 0 ) + ( writeFds ? writeFds->_count : 0 ) );
		M_ENSURE( count <= MAXIMUM_WAIT_OBJECTS );
		IO* ios[MAXIMUM_WAIT_OBJECTS];
		HANDLE handles[MAXIMUM_WAIT_OBJECTS];
		OsCast osCast;
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
		} else {
			std::transform( ios, ios + readFds->_count, handles, osCast );
 			int up( ::WaitForMultipleObjects( readFds->_count, handles, false, miliseconds ) );
			if ( up == WAIT_FAILED ) {
				ret = -1;
				log_windows_error( "WaitForMultipleObjects" );
			} else if ( ( up >= static_cast<int>( WAIT_OBJECT_0 ) ) && ( up < ( static_cast<int>( WAIT_OBJECT_0 ) + count ) ) ) {
				for ( int i( 0 ); i < count; ++ i ) {
					int upL( ::WaitForSingleObject( handles[ i ], 0 ) );
					if ( ( i == ( up - WAIT_OBJECT_0 ) || ( upL == WAIT_OBJECT_0 ) ) )
						++ ret;
					else
						readFds->_data[ i ] = -1;
				}
				std::remove( readFds->_data, readFds->_data + readFds->_count, -1 );
				readFds->_count = ret;
				ret += ( writeFds ? writeFds->_count : 0 );
			} else
				FD_ZERO( readFds );
		}
	} else {
		M_ASSERT( timeout );
		HANDLE h( _tlsSignalsSetup_->interrupt() );
		if ( ::WaitForSingleObject( h, miliseconds ) == WAIT_OBJECT_0 ) {
			ret = -1;
			errno = EINTR;
		}
	}
	return ( ret );
}

int make_pipe_instance( IO& io_ ) {
	io_.set_handle( CreateNamedPipe( io_.path().c_str(),
		PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED,
		PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT,
		PIPE_UNLIMITED_INSTANCES, 1024, 1024, 1000, NULL ) );
	/* Connection it self fires event so we only fake read here. */
	io_.fake_schedule_read();
	return ( io_.handle() != INVALID_HANDLE_VALUE ? 0 : -1 );
}

int bind( int fd_, const struct sockaddr* addr_, socklen_t len_ ) {
	int ret( -1 );
	SystemIO& sysIo( SystemIO::get_instance() );
	IO& io( *( sysIo.get_io( fd_ ).second ) );
	char const* path( addr_->sa_data + 2 );
	if ( addr_->sa_family == PF_UNIX ) {
		M_ASSERT( io.type() == IO::TYPE::NAMED_PIPE );
		HANDLE h = ::CreateFile( path,                // name of the write
                       GENERIC_WRITE,          // open for writing
                       0,                      // do not share
                       NULL,                   // default security
                       CREATE_NEW,          // overwrite existing
											 FILE_ATTRIBUTE_NORMAL | FILE_FLAG_NO_BUFFERING, // normal file
                       NULL);                  // no attr. template
		string n( "\\\\.\\pipe" );
		if ( h != INVALID_HANDLE_VALUE ) {
			::CloseHandle( h );
			n += path;
			std::replace( n.begin(), n.end(), '/', '\\' );
			io.set_path( n );
			ret = make_pipe_instance( io );
		}
	} else {
		M_ASSERT( io.type() == IO::TYPE::SOCKET );
		ret = ::bind( reinterpret_cast<SOCKET>( io.handle() ), addr_, len_ );
	}
	return ( ret );
}

#undef socket
int socket( int af, int type, int protocol ) {
	SOCKET s = -1;
	if ( af != PF_UNIX )
		s = ::socket( af, type, protocol );
	SystemIO& sysIo( SystemIO::get_instance() );
	SystemIO::io_t sock( sysIo.create_io( af == PF_UNIX ? IO::TYPE::NAMED_PIPE : IO::TYPE::SOCKET, reinterpret_cast<HANDLE>( s ) ) );
	return ( sock.first );
}

int listen( int fd_, int backlog_ ) {
	int ret( 0 );
	SystemIO& sysIo( SystemIO::get_instance() );
	IO& io( *( sysIo.get_io( fd_ ).second ) );
	if ( io.type() == IO::TYPE::NAMED_PIPE ) {
		if ( ::ConnectNamedPipe( io.handle(), io.overlapped() ) ) {
			log_windows_error( "ConnectNamedPipe" );
			ret = -1;
		}
	} else {
		SOCKET s( reinterpret_cast<SOCKET>( io.handle() ) );
		if ( WSAEventSelect( s, io.event(), FD_ACCEPT | FD_OOB ) )
			log_windows_error( "WSAEventSelect" );
		/* Connection it self fires eventc so we only fake read here. */
		io.fake_schedule_read();
		ret = ::listen( s, backlog_ );
	}
	return ( ret );
}

int accept( int fd_, struct sockaddr* addr_, socklen_t* len_ ) {
	int ret( 0 );
	SystemIO& sysIo( SystemIO::get_instance() );
	IO& io( *( sysIo.get_io( fd_ ).second ) );
	if ( io.type() == IO::TYPE::SOCKET ) {
		int len = *len_;
		ret = ::accept( reinterpret_cast<SOCKET>( io.handle() ), addr_, &len );
		SystemIO::io_t sock( sysIo.create_io( IO::TYPE::SOCKET, reinterpret_cast<HANDLE>( ret ) ) );
		if ( WSAEventSelect( ret, sock.second->event(), FD_ACCEPT | FD_CONNECT | FD_READ | FD_WRITE | FD_CLOSE | FD_OOB ) )
			log_windows_error( "WSAEventSelect" );
		ret = sock.first;
	} else {
		SystemIO::io_t np( sysIo.create_io( IO::TYPE::NAMED_PIPE,
			reinterpret_cast<HANDLE>( -1 ), NULL, io.path() ) );
		ret = make_pipe_instance( *(np.second) );
		np.second->swap( io );
		np.second->reset();
		if ( ::ConnectNamedPipe( io.handle(), io.overlapped() ) ) {
			log_windows_error( "ConnectNamedPipe" );
			ret = -1;
		}	
		if ( ! ret )
			ret = np.first;
	}
	return ( ret );
}

int connect( int fd_, struct sockaddr* addr_, socklen_t len_ ) {
	int ret( 0 );
	SystemIO& sysIo( SystemIO::get_instance() );
	IO& io( *( sysIo.get_io( fd_ ).second ) );
	if ( addr_->sa_family == PF_UNIX ) {
		char const* path( addr_->sa_data + 2 );
		string n( "\\\\.\\pipe" );
		n += path;
		std::replace( n.begin(), n.end(), '/', '\\' );
		HANDLE h( ::CreateFile( n.c_str(), ( GENERIC_READ | GENERIC_WRITE ) & ( ~SYNCHRONIZE ), 0, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED | FILE_FLAG_NO_BUFFERING, 0 ) );
		if ( h == INVALID_HANDLE_VALUE ) {
			log_windows_error( "CreateNamedPipe" );
			ret = -1;
		} else
			io.set_handle( h );
	} else {
		SOCKET s( reinterpret_cast<SOCKET>( io.handle() ) );
		ret = ::connect( s, const_cast<sockaddr const*>( addr_ ), static_cast<int>( len_ ) );
		if ( WSAEventSelect( s, io.event(), FD_ACCEPT | FD_CONNECT | FD_READ | FD_WRITE | FD_CLOSE | FD_OOB ) )
			log_windows_error( "WSAEventSelect" );
	}
	return ( ret );
}

int shutdown( int fd_, int how_ ) {
	SystemIO& sysIo( SystemIO::get_instance() );
	IO& io( *( sysIo.get_io( fd_ ).second ) );
	int ret( 0 );
	if ( io.type() == IO::TYPE::SOCKET )
		ret = ::shutdown( reinterpret_cast<SOCKET>( io.handle() ), how_ );
	return ( ret );
}

int setsockopt( int fd_, int level_, int optname_, void const* optval_, socklen_t optlen_ ) {
	int ret( 0 );
	SystemIO& sysIo( SystemIO::get_instance() );
	IO& io( *( sysIo.get_io( fd_ ).second ) );
	if ( io.type() == IO::TYPE::SOCKET )
		ret = ::setsockopt( reinterpret_cast<SOCKET>( io.handle() ), level_,
				optname_ == SO_REUSEADDR ? SO_EXCLUSIVEADDRUSE : optname_,
				static_cast<char const*>( optval_ ), optlen_ );
	return ( ret );
}

int getsockopt( int fd_, int level_, int optname_, void* optval_, socklen_t* optlen_ ) {
	int ret( 0 );
	SystemIO& sysIo( SystemIO::get_instance() );
	IO& io( *( sysIo.get_io( fd_ ).second ) );
	if ( io.type() == IO::TYPE::SOCKET )
		ret = ::getsockopt( reinterpret_cast<SOCKET>( io.handle() ), level_, optname_, static_cast<char*>( optval_ ), optlen_ );
	return ( ret );
}

}

int get_socket_error( void ) {
	return ( WSAGetLastError() );
}

void set_socket_error( int errno_ ) {
	WSASetLastError( errno_ );
}
