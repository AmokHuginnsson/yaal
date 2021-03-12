#include "_aux/msvcxx/sys/socket.h"

#include "hcore/base.hxx"
#include "hcore/hexception.hxx"
#include "cleanup.hxx"
#include "emu_unistd.hxx"
#include "msio.hxx"

using namespace std;
using yaal::error_message;

namespace msvcxx {

int make_pipe_instance( IO& io_ ) {
	io_.set_handle( CreateNamedPipe( io_.path().c_str(),
		PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED,
		PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT,
		PIPE_UNLIMITED_INSTANCES, 1024, 1024, 1000, nullptr ) );
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
                       nullptr,                   // default security
                       CREATE_NEW,          // overwrite existing
											 FILE_ATTRIBUTE_NORMAL | FILE_FLAG_NO_BUFFERING, // normal file
                       nullptr);                  // no attr. template
		string n( "\\\\.\\pipe" );
		if ( h != INVALID_HANDLE_VALUE ) {
			::CloseHandle( h );
			n += path;
			std::replace( n.begin(), n.end(), '/', '\\' );
			io.set_path( n );
			ret = make_pipe_instance( io );
		}
	} else {
		M_ASSERT( ( io.type() == IO::TYPE::SOCKET ) || ( io.type() == IO::TYPE::SOCKET_DGRAM ) );
		SOCKET s( reinterpret_cast<SOCKET>( io.handle() ) );
		if ( io.type() == IO::TYPE::SOCKET_DGRAM ) {
			if ( ::WSAEventSelect( s, io.event(), FD_READ | FD_OOB ) )
				log_windows_error( "WSAEventSelect" );
			int const reuseAddr( 1 );
			ret = ::setsockopt( s, SOL_SOCKET, SO_EXCLUSIVEADDRUSE,
					reinterpret_cast<char const*>( &reuseAddr ), sizeof ( reuseAddr ) );
		}
		ret = ::bind( s, addr_, len_ );
	}
	return ret;
}

#undef socket
int socket( int af, int type, int protocol ) {
	SOCKET s = -1;
	if ( af != PF_UNIX )
		s = ::socket( af, type, protocol );
	SystemIO& sysIo( SystemIO::get_instance() );
	SystemIO::io_t sock( sysIo.create_io(
		af == PF_UNIX ? IO::TYPE::NAMED_PIPE : ( type == SOCK_DGRAM ? IO::TYPE::SOCKET_DGRAM : IO::TYPE::SOCKET ),
		reinterpret_cast<HANDLE>( s ) ) );
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
		ret = ::listen( s, backlog_ );
	}
	return ret;
}

int accept( int fd_, struct sockaddr* addr_, socklen_t* len_ ) {
	int ret( 0 );
	SystemIO& sysIo( SystemIO::get_instance() );
	IO& io( *( sysIo.get_io( fd_ ).second ) );
	if ( io.type() == IO::TYPE::SOCKET ) {
		int len = *len_;
		ret = ::accept( reinterpret_cast<SOCKET>( io.handle() ), addr_, &len );
		if ( ret != -1 ) {
			SystemIO::io_t sock( sysIo.create_io( IO::TYPE::SOCKET, reinterpret_cast<HANDLE>( ret ) ) );
			if ( ::WSAEventSelect( ret, sock.second->event(), FD_ACCEPT | FD_CONNECT | FD_READ | FD_WRITE | FD_CLOSE | FD_OOB ) )
				log_windows_error( "WSAEventSelect" );
			if ( ! ::ResetEvent( io.event() ) )
				log_windows_error( "ResetEvent" );
			ret = sock.first;
			sock.second->accept();
		}
	} else {
		M_ASSERT( io.type() == IO::TYPE::NAMED_PIPE );
		SystemIO::io_t np( sysIo.create_io( IO::TYPE::NAMED_PIPE,
			reinterpret_cast<HANDLE>( -1 ), nullptr, io.path() ) );
		ret = make_pipe_instance( *(np.second) );
		np.second->swap( io );
		np.second->reset();
		if ( ::ConnectNamedPipe( io.handle(), io.overlapped() ) ) {
			log_windows_error( "ConnectNamedPipe" );
			ret = -1;
		}
		if ( ! ret )
			ret = np.first;
		np.second->accept();
	}
	return ret;
}

int connect( int fd_, struct sockaddr* addr_, socklen_t len_ ) {
	int ret( 0 );
	SystemIO& sysIo( SystemIO::get_instance() );
	IO& io( *( sysIo.get_io( fd_ ).second ) );
	do {
		if ( addr_->sa_family == PF_UNIX ) {
			char const* path( addr_->sa_data + 2 );
			string n( "\\\\.\\pipe" );
			n += path;
			std::replace( n.begin(), n.end(), '/', '\\' );
			HANDLE h( ::CreateFile( n.c_str(), ( GENERIC_READ | GENERIC_WRITE ) & ( ~SYNCHRONIZE ), 0, nullptr, OPEN_EXISTING, FILE_FLAG_OVERLAPPED | FILE_FLAG_NO_BUFFERING, 0 ) );
			if ( h == INVALID_HANDLE_VALUE ) {
				log_windows_error( "CreateNamedPipe" );
				ret = -1;
			} else {
				io.set_handle( h );
			}
		} else {
			M_ASSERT( ( io.type() == IO::TYPE::SOCKET ) || ( io.type() == IO::TYPE::SOCKET_DGRAM ) );
			SOCKET s( reinterpret_cast<SOCKET>( io.handle() ) );
			ret = ::connect( s, const_cast<sockaddr const*>( addr_ ), static_cast<int>( len_ ) );
			if ( ret != 0 ) {
				break;
			}
			if ( ::WSAEventSelect( s, io.event(), FD_ACCEPT | FD_CONNECT | FD_READ | FD_WRITE | FD_CLOSE | FD_OOB ) ) {
				log_windows_error( "WSAEventSelect" );
				ret = -1;
			}
		}
	} while ( false );
	if ( ! ret ) {
		ret = io.connect();
	}
	return ret;
}

int shutdown( int fd_, int how_ ) {
	SystemIO& sysIo( SystemIO::get_instance() );
	IO& io( *( sysIo.get_io( fd_ ).second ) );
	int ret( 0 );
	if ( ( io.type() == IO::TYPE::SOCKET ) && io.is_connected() )
		ret = ::shutdown( reinterpret_cast<SOCKET>( io.handle() ), how_ );
	return ret;
}

int setsockopt( int fd_, int level_, int optname_, void const* optval_, socklen_t optlen_ ) {
	int ret( 0 );
	SystemIO& sysIo( SystemIO::get_instance() );
	IO& io( *( sysIo.get_io( fd_ ).second ) );
	IO::TYPE::type_t t( io.type() );
	if ( ( t == IO::TYPE::SOCKET ) || ( t == IO::TYPE::SOCKET_DGRAM ) )
		ret = ::setsockopt( reinterpret_cast<SOCKET>( io.handle() ), level_,
				optname_ == SO_REUSEADDR ? SO_EXCLUSIVEADDRUSE : optname_,
				static_cast<char const*>( optval_ ), optlen_ );
	return ret;
}

int getsockopt( int fd_, int level_, int optname_, void* optval_, socklen_t* optlen_ ) {
	int ret( 0 );
	SystemIO& sysIo( SystemIO::get_instance() );
	IO& io( *( sysIo.get_io( fd_ ).second ) );
	IO::TYPE::type_t t( io.type() );
	if ( ( t == IO::TYPE::SOCKET ) || ( t == IO::TYPE::SOCKET_DGRAM ) )
		ret = ::getsockopt( reinterpret_cast<SOCKET>( io.handle() ), level_, optname_, static_cast<char*>( optval_ ), optlen_ );
	else {
		if ( optname_ == SO_ERROR ) {
			int* opt( reinterpret_cast<int*>( optval_ ) );
			*opt = 0;
		}
	}
	return ret;
}

}

int get_socket_error( void ) {
	return ( ::WSAGetLastError() );
}

void set_socket_error( int errno_ ) {
	::WSASetLastError( *_errno() = errno_ );
}

