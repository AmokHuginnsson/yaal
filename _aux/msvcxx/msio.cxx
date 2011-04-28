#include <sys/cdefs.h>
#include <sys/time.h>
#include <io.h>
#define _FCNTL_H 1
#include <bits/fcntl.h>

#include "hcore/base.hxx"
#include "hcore/hexception.hxx"

#include "msio.hxx"
#include "emu_unistd.hxx"

namespace msvcxx
{

IO::IO( TYPE::type_t t_, HANDLE h_, HANDLE e_, std::string const& p_ )
: _type( t_ ), _handle( h_ ), _overlapped(), _buffer( 0 ),
	_scheduled( false ), _ready( false ), _nonBlocking( false ), _path( p_ )
	{
	_overlapped.hEvent = e_ ? e_ : ::CreateEvent( NULL, false, false, NULL );
	}

IO::~IO( void )
	{
	M_PROLOG
	if ( _overlapped.hEvent )
		::CloseHandle( _overlapped.hEvent );
	M_DESTRUCTOR_EPILOG
	}

void IO::schedule_read( void )
	{
	DWORD nRead( 0 );
	if ( ! _scheduled )
		{
		BOOL status( ::ReadFile( _handle, &_buffer, 1, &nRead, &_overlapped ) );
		if ( status && ( nRead == 1 ) )
			_ready = true;
		}
	_scheduled = true;
	return;
	}

void IO::sync_read( void )
	{
	DWORD iRead( 0 );
	if ( ! ::GetOverlappedResult( _handle, &_overlapped, &iRead, true ) || ( iRead != 1 ) )
		log_windows_error( "GetOverlappedResult" );
	else
		_ready = true;
	_scheduled = false;
	return;
	}

int long IO::read( void* buf_, int long size_ )
	{
	DWORD iRead( 0 );
	int off( 0 );
	bool ok( false );
	if ( _scheduled && ! _ready )
		sync_read();
	if ( _ready )
		{
		_scheduled = false;
		_ready = false;
		static_cast<char*>( buf_ )[0] = _buffer;
		-- size_;
		++ off;
		ok = true;
		}
	if ( size_ > 0 )
		{
		ok = ::ReadFile( _handle, static_cast<char*>( buf_ ) + off, size_, &iRead, &_overlapped ) ? true : false;
		if ( ! ok && ( ::GetLastError() != ERROR_IO_PENDING ) )
			log_windows_error( "ReadFile" );
		ok = ::GetOverlappedResult( _handle, &_overlapped, &iRead, ! _nonBlocking ) ? true : false;
		if ( ! ok )
			log_windows_error( "GetOverlappedResult" );
		}
	return ( ok ? iRead + off : -1 );
	}

int long IO::write( void const* buf_, int long size_ )
	{
	DWORD iWritten( 0 );
	bool ok( ::WriteFile( _handle, buf_, size_, &iWritten, &_overlapped ) ? true : false );
	if ( ::GetLastError() == ERROR_IO_PENDING )
		{
		ok = ::GetOverlappedResult( _handle, &_overlapped, &iWritten, true ) ? true : false;
		if ( ! ok )
			log_windows_error( "GetOverlappedResult" );
		}
	return ( ok ? iWritten : -1 );
	}

int IO::close( void )
	{
	int ret( 0 );
	if ( reinterpret_cast<SOCKET>( _handle ) != -1 )
		{
		if ( ( _type == IO::TYPE::PIPE ) || ( _type == IO::TYPE::NAMED_PIPE ) )
			ret = ::CloseHandle( _handle ) ? 0 : -1;
		else if ( _type == IO::TYPE::SOCKET )
			ret = ::closesocket( reinterpret_cast<SOCKET>( _handle ) );
		else
			{
			M_ASSERT( ! "invalid HANDLE type" );
			}
		}
	return ( ret );
	}

int IO::fcntl( int cmd_, int arg_ )
	{
	int ret( 0 );
	if ( cmd_ == F_SETFL )
		{
		_nonBlocking = ( ( arg_ & O_NONBLOCK ) ? true : false );
		if ( ( _nonBlocking ) && ( ( _type == IO::TYPE::NAMED_PIPE ) || ( _type == IO::TYPE::PIPE ) ) )
			schedule_read();
		}
	else if ( cmd_ == F_GETFL )
		ret = _nonBlocking ? O_NONBLOCK : 0;
	return ( ret );
	}

HANDLE IO::event( void ) const
	{
	return ( _overlapped.hEvent );
	}

HANDLE IO::handle( void ) const
	{
	return ( _handle );
	}

void IO::set_handle( HANDLE handle_ )
	{
	_handle = handle_;
	}

IO::TYPE::type_t IO::type( void ) const
	{
	return ( _type );
	}

OVERLAPPED* IO::overlapped( void )
	{
	return ( &_overlapped );
	}

std::string IO::path( void ) const
	{
	return ( _path );
	}

void IO::set_path( std::string const& path_ )
	{
	_path = path_;
	return;
	}

void IO::swap( IO& io_ )
	{
	using std::swap;
	swap( _overlapped, io_._overlapped );
	swap( _buffer, io_._buffer );
	swap( _scheduled, io_._scheduled );
	swap( _ready, io_._ready );
	swap( _handle, io_._handle );
	swap( _nonBlocking, io_._nonBlocking );
	swap( _path, io_._path );
	swap( _type, io_._type );
	return;
	}

void IO::reset( void )
	{
	if ( ! _ready )
		_scheduled = false;
	}

void IO::fake_schedule_read( void )
	{
	_scheduled = true;
	}

bool IO::ready( void ) const
	{
	return ( _ready );
	}

SystemIO::SystemIO( void )
	: _ioTable(), _mutex(), _idPool( MANAGED_IO )
	{}

SystemIO& SystemIO::get_instance( void )
	{
	static SystemIO instance;
	return ( instance );
	}

SystemIO::io_t& SystemIO::create_io( IO::TYPE::type_t type_, HANDLE h_, HANDLE e_, std::string const& p_ )
	{
	CLock l( _mutex );
	return ( *( _ioTable.insert( std::make_pair( _idPool ++, io_ptr_t( new IO( type_, h_, e_, p_ ) ) ) ).first ) );
	}

SystemIO::io_t& SystemIO::get_io( int id_ )
	{
	CLock l( _mutex );
	io_table_t::iterator i( _ioTable.find( id_ ) );
	if ( i != _ioTable.end() )
		return ( *i );
	M_ASSERT( id_ < MANAGED_IO );
	HANDLE h( reinterpret_cast<HANDLE>( _get_osfhandle( id_ ) ) );
	return ( *( _ioTable.insert( std::make_pair( id_, io_ptr_t( new IO( IO::TYPE::TERMINAL, h, h ) ) ) ).first ) );
	}

int SystemIO::close_io( int id_ )
	{
	CLock l( _mutex );
	io_table_t::iterator i( _ioTable.find( id_ ) );
	int ret( 0 );
	if ( i != _ioTable.end() )
		ret = i->second->close();
	else
		{
		M_ASSERT( id_ < MANAGED_IO );
		}
	_ioTable.erase( i );
	return ( ret );
	}

int SystemIO::dup2_io( int id1_, int id2_ )
	{
	M_ASSERT( ( id1_ >= MANAGED_IO ) || ( id2_ >= MANAGED_IO ) );
	CLock l( _mutex );
	int fd1( id1_ < MANAGED_IO ? id1_ : _open_osfhandle( reinterpret_cast<intptr_t>( get_io( id1_ ).second->_handle ), 0 ) );
	int fd2( id2_ < MANAGED_IO ? id2_ : _open_osfhandle( reinterpret_cast<intptr_t>( get_io( id2_ ).second->_handle ), 0 ) );	int fd( _dup2( fd1, fd2 ) );
/*	HANDLE h( reinterpret_cast<HANDLE>( _get_osfhandle( fd ) ) );
	 _ioTable.insert( std::make_pair( fd, io_ptr_t( new IO( IO::TYPE::TERMINAL, h, h ) ) ) */
	return ( fd );
	}

} /* namespace msvcxx */
