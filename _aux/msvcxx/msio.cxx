#include <sys/time.h>
#include <io.h>

#include "hcore/base.hxx"
#include "hcore/hexception.hxx"

#include "msio.hxx"

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
