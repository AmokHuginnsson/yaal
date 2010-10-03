#include <sys/time.h>
#include <io.h>

#include "hcore/base.hxx"
#include "hcore/hexception.hxx"

#include "msio.hxx"

namespace msvcxx
{

IO::IO( TYPE::type_t t_, HANDLE h_, HANDLE e_, std::string const& p_ )
: _type( t_ ), _handle( h_ ), _overlapped(), _buffer( 0 ), _scheduled( false ), _nonBlocking( false ), _path( p_ )
	{
	_overlapped.hEvent = e_ ? e_ : ::CreateEvent( NULL, false, false, NULL );
	}

IO::~IO( void )
	{
	if ( _overlapped.hEvent )
		::CloseHandle( _overlapped.hEvent );
	}

void IO::schedule_read( void )
	{
	DWORD nRead( 0 );
	::ReadFile( _handle, &_buffer, 1, &nRead, &_overlapped );
	_scheduled = true;
	return;
	}

void IO::sync_read( void )
	{
	}

int long IO::read( void*, int long )
	{
	return ( 0 );
	}

int long IO::write( void const*, int long )
	{
	return ( 0 );
	}

int IO::close( void )
	{
	return ( 0 );
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

void SystemIO::erase_io( int id_ )
	{
	CLock l( _mutex );
	_ioTable.erase( id_ );
	}

} /* namespace msvcxx */