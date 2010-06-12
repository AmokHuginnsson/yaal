/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	hrawfile.cxx - this file is integral part of `yaal' project.

	i.  You may not make any changes in Copyright information.
	ii. You must attach Copyright information to any part of every copy
	    of this software.

Copyright:

 You are free to use this program as is, you can redistribute binary
 package freely but:
  1. You cannot use any part of sources of this software.
  2. You cannot redistribute any part of sources of this software.
  3. No reverse engineering is allowed.
  4. If you want redistribute binary package you cannot demand any fees
     for this software.
     You cannot even demand cost of the carrier (CD for example).
  5. You cannot include it to any commercial enterprise (for example 
     as a free add-on to payed software or payed newspaper).
 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE. Use it at your own risk.
*/

#include <cstring>
#include <unistd.h>
#include <poll.h>
#include <libintl.h>
#include <sys/time.h> /* timeval */

#include "base.hxx"
M_VCSID( "$Id: "__ID__" $" )
M_VCSID( "$Id: "__TID__" $" )
#include "hrawfile.hxx"
#include "hclock.hxx"

namespace yaal
{

namespace hcore
{

char const* const _error_ = _( "file is not opened" );

HRawFile::HRawFile ( TYPE::raw_file_type_t type_ )
	: _type( type_ ), _fileDescriptor( -1 ), _timeOut( 0 ), _sSL(),
	reader( ( ( type_ & TYPE::SSL_SERVER )
				|| ( type_ & TYPE::SSL_CLIENT ) ) ? &HRawFile::read_ssl_loader : &HRawFile::read_plain ),
	writer( ( ( type_ & TYPE::SSL_SERVER )
				|| ( type_ & TYPE::SSL_CLIENT ) ) ? &HRawFile::write_ssl_loader : &HRawFile::write_plain ),
	closer( &HRawFile::close_plain )
	{
	M_PROLOG
	M_ASSERT( ! ( ( type_ == TYPE::DEFAULT ) && ( type_ & TYPE::SSL_SERVER ) ) );
	M_ASSERT( ! ( ( type_ == TYPE::DEFAULT ) && ( type_ & TYPE::SSL_CLIENT ) ) );
	M_ASSERT( ! ( ( type_ & TYPE::PLAIN ) && ( type_ & TYPE::SSL_SERVER ) ) );
	M_ASSERT( ! ( ( type_ & TYPE::PLAIN ) && ( type_ & TYPE::SSL_CLIENT ) ) );
	M_ASSERT( ! ( ( type_ & TYPE::SSL_CLIENT ) && ( type_ & TYPE::SSL_SERVER ) ) );
	return;
	M_EPILOG
	}

HRawFile::~HRawFile( void )
	{
	M_PROLOG
	if ( _fileDescriptor >= 0 )
		HRawFile::close();
	M_EPILOG
	}

int HRawFile::close( void )
	{
	M_PROLOG
	return ( (this->*closer)() );
	M_EPILOG
	}

int HRawFile::close_plain( void )
	{
	M_PROLOG
	return ( do_close() );
	M_EPILOG
	}

int HRawFile::close_ssl( void )
	{
	M_PROLOG
	if ( is_write_ready( _fileDescriptor ) )
		_sSL->shutdown();
	return ( do_close() );
	M_EPILOG
	}

int HRawFile::do_close ( void )
	{
	M_PROLOG
	int error = 0;
	if ( _fileDescriptor < 0 )
		M_THROW( "file is not opened", errno );
	error = static_cast<int>( TEMP_FAILURE_RETRY( ::close( _fileDescriptor ) ) );
	_fileDescriptor = -1;
	return ( error );
	M_EPILOG
	}

file_descriptor_t HRawFile::get_file_descriptor ( void ) const
	{
	M_PROLOG
	return ( _fileDescriptor );
	M_EPILOG
	}

int long HRawFile::read_plain( void* const buffer_, int long const& size_ )
	{
	M_PROLOG
	if ( _fileDescriptor < 0 )
		M_THROW( _error_, errno );
	return ( ::read( _fileDescriptor, buffer_, size_ ) );
	M_EPILOG
	}

int long HRawFile::read_ssl( void* const buffer_, int long const& size_ )
	{
	M_PROLOG
	if ( _fileDescriptor < 0 )
		M_THROW( _error_, errno );
	return ( _sSL->read( buffer_, size_ ) );
	M_EPILOG
	}

int long HRawFile::read_ssl_loader( void* const buffer_, int long const& size_ )
	{
	M_PROLOG
	_sSL = HOpenSSL::ptr_t( new HOpenSSL( _fileDescriptor, _type & TYPE::SSL_SERVER ? HOpenSSL::TYPE::SERVER : HOpenSSL::TYPE::CLIENT ) );
	reader = &HRawFile::read_ssl;
	writer = &HRawFile::write_ssl;
	closer = &HRawFile::close_ssl;
	return ( read_ssl( buffer_, size_ ) );
	M_EPILOG
	}

bool HRawFile::is_write_ready( int fileDescriptor_ )
	{
	pollfd writer;
	::memset( &writer, 0, sizeof ( writer ) );
	writer.fd = fileDescriptor_;
	writer.events = POLLOUT;
	return ( ( poll( &writer, 1, 0 ) == 1 ) && ( writer.revents & POLLOUT ) && ! ( writer.revents & POLLHUP ) );
	}

int long HRawFile::do_write( void const* const buffer_, int long const& size_ )
	{
	M_PROLOG
	int long done = ( _timeOut || is_write_ready( _fileDescriptor ) ) ? (this->*writer)( buffer_, size_ ) : -1;
	return ( done );
	M_EPILOG
	}

bool HRawFile::wait_for( ACTION::action_t const& action_, void* time_ )
	{
	int error = - 1;
	fd_set fdSet;
	timeval* wait = static_cast<timeval*>( time_ );
	while ( wait->tv_sec || wait->tv_usec )
		{
		FD_ZERO( & fdSet );
		FD_SET( _fileDescriptor, &fdSet );
		error = static_cast<int>( TEMP_FAILURE_RETRY( ::select( FD_SETSIZE,
						( action_ == ACTION::READ ) ? &fdSet : NULL,
						( action_ == ACTION::WRITE ) ? &fdSet : NULL,
						NULL, wait ) ) );
		if ( error < 0 )
			break;
		else if ( ( error > 0 )
				&& FD_ISSET( _fileDescriptor, &fdSet ) )
			break;
		else if ( ! error )
			break;
		}
	return ( error <= 0 );
	}

int long HRawFile::write_plain( void const* const buffer_, int long const& size_ )
	{
	M_PROLOG
	if ( _fileDescriptor < 0 )
		M_THROW( _error_, errno );
	int long iWritten = 0;
	timeval tv = { _timeOut, 0 };
	do
		{
		if ( ( _timeOut > 0 ) && wait_for( ACTION::WRITE, &tv ) )
			throw HStreamInterfaceException( _( "timeout on write" ) );
		int long ret = TEMP_FAILURE_RETRY( ::write( _fileDescriptor,
					static_cast<char const* const>( buffer_ ) + iWritten,
					size_ - iWritten ) );
		if ( ret < 0 )
			{
			iWritten = ret;
			break;
			}
		else
			iWritten += ret;
		}
	while ( iWritten < size_ );
	return ( iWritten );
	M_EPILOG
	}

int long HRawFile::write_ssl( void const* const buffer_, int long const& size_ )
	{
	M_PROLOG
	if ( _fileDescriptor < 0 )
		M_THROW( _error_, errno );
	HClock clk;
	int long nWritten = 0;
	do
		{
		if ( _timeOut  && ( clk.get_time_elapsed() > _timeOut ) )
			throw HStreamInterfaceException( _( "timeout on write" ) );
		int long ret = _sSL->write( static_cast<char const* const>( buffer_ ) + nWritten, size_ );
		if ( ! ret )
			{
			nWritten = ret;
			break;
			}
		else if ( ret > 0 )
			nWritten += ret;
		}
	while ( nWritten < size_ );
	return ( nWritten );
	M_EPILOG
	}

int long HRawFile::write_ssl_loader( void const* const buffer_, int long const& size_ )
	{
	M_PROLOG
	_sSL = HOpenSSL::ptr_t( new HOpenSSL( _fileDescriptor, _type & TYPE::SSL_SERVER ? HOpenSSL::TYPE::SERVER : HOpenSSL::TYPE::CLIENT ) );
	reader = &HRawFile::read_ssl;
	writer = &HRawFile::write_ssl;
	closer = &HRawFile::close_ssl;
	return ( write_ssl( buffer_, size_ ) );
	M_EPILOG
	}

int long HRawFile::do_read( void* const buffer_, int long const& size_ )
	{
	M_PROLOG
	return ( (this->*reader)( buffer_, size_ ) );
	M_EPILOG
	}

void HRawFile::do_flush( void ) const
	{
	}

void HRawFile::set_timeout( int timeout_ )
	{
	_timeOut = timeout_;
	return;
	}

bool HRawFile::do_is_valid( void ) const
	{
	M_PROLOG
	return ( _fileDescriptor >= 0 );
	M_EPILOG
	}

}

}

