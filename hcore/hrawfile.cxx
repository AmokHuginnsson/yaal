/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	hrawfile.cxx - this file is integral part of `yaal' project.

	i.  You may not make any changes in Copyright information.
	ii. You must attach Copyright information to any part of every copy
	    of this software.

Copyright:

 You are free to use this program as is, you can redistribute binary
 package freely but:
  1. You can not use any part of sources of this software.
  2. You can not redistribute any part of sources of this software.
  3. No reverse engineering is allowed.
  4. If you want redistribute binary package you can not demand any fees
     for this software.
     You can not even demand cost of the carrier (CD for example).
  5. You can not include it to any commercial enterprise (for example 
     as a free add-on to payed software or payed newspaper).
 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE. Use it at your own risk.
*/

#include <unistd.h>
#include <poll.h>
#include <cstdlib>
#include <libintl.h>

#include "hexception.h"
M_VCSID ( "$Id$" )
#include "hrawfile.h"

namespace yaal
{

namespace hcore
{

char const * const n_pcError = _( "file is not opened" );

HRawFile::HRawFile ( TYPE::raw_file_type_t a_eType )
	: f_eType( a_eType ), f_iFileDescriptor( -1 ), f_iTimeOut( 0 ), f_oSSL(),
	reader( ( ( a_eType & TYPE::D_SSL_SERVER )
				|| ( a_eType & TYPE::D_SSL_CLIENT ) ) ? &HRawFile::read_ssl_loader : &HRawFile::read_plain ),
	writer( ( ( a_eType & TYPE::D_SSL_SERVER )
				|| ( a_eType & TYPE::D_SSL_CLIENT ) ) ? &HRawFile::write_ssl_loader : &HRawFile::write_plain ),
	closer( &HRawFile::close_plain )
	{
	M_PROLOG
	M_ASSERT( ! ( ( a_eType == TYPE::D_DEFAULT ) && ( a_eType & TYPE::D_SSL_SERVER ) ) );
	M_ASSERT( ! ( ( a_eType == TYPE::D_DEFAULT ) && ( a_eType & TYPE::D_SSL_CLIENT ) ) );
	M_ASSERT( ! ( ( a_eType & TYPE::D_PLAIN ) && ( a_eType & TYPE::D_SSL_SERVER ) ) );
	M_ASSERT( ! ( ( a_eType & TYPE::D_PLAIN ) && ( a_eType & TYPE::D_SSL_CLIENT ) ) );
	M_ASSERT( ! ( ( a_eType & TYPE::D_SSL_CLIENT ) && ( a_eType & TYPE::D_SSL_SERVER ) ) );
	return;
	M_EPILOG
	}

HRawFile::~HRawFile( void )
	{
	M_PROLOG
	if ( f_iFileDescriptor >= 0 )
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
	if ( is_write_ready( f_iFileDescriptor ) )
		f_oSSL->shutdown();
	return ( do_close() );
	M_EPILOG
	}

int HRawFile::do_close ( void )
	{
	M_PROLOG
	int l_iError = 0;
	if ( f_iFileDescriptor < 0 )
		M_THROW( "file is not opened", errno );
	l_iError = ::close( f_iFileDescriptor );
	f_iFileDescriptor = -1;
	return ( l_iError );
	M_EPILOG
	}

file_descriptor_t HRawFile::get_file_descriptor ( void ) const
	{
	M_PROLOG
	return ( f_iFileDescriptor );
	M_EPILOG
	}

int HRawFile::read_plain( void* const a_pcBuffer, int const a_iSize )
	{
	M_PROLOG
	if ( f_iFileDescriptor < 0 )
		M_THROW ( n_pcError, errno );
	return ( ::read( f_iFileDescriptor, a_pcBuffer, a_iSize ) );
	M_EPILOG
	}

int HRawFile::read_ssl( void* const a_pcBuffer, int const a_iSize )
	{
	M_PROLOG
	if ( f_iFileDescriptor < 0 )
		M_THROW ( n_pcError, errno );
	return ( f_oSSL->read( a_pcBuffer, a_iSize ) );
	M_EPILOG
	}

int HRawFile::read_ssl_loader( void* const a_pcBuffer, int const a_iSize )
	{
	M_PROLOG
	f_oSSL = HOpenSSL::ptr_t( new HOpenSSL( f_iFileDescriptor, f_eType & TYPE::D_SSL_SERVER ? HOpenSSL::TYPE::D_SERVER : HOpenSSL::TYPE::D_CLIENT ) );
	reader = &HRawFile::read_ssl;
	writer = &HRawFile::write_ssl;
	closer = &HRawFile::close_ssl;
	return ( read_ssl( a_pcBuffer, a_iSize ) );
	M_EPILOG
	}

bool HRawFile::is_write_ready( int a_iFileDescriptor )
	{
	pollfd l_sWriter;
	::memset( &l_sWriter, 0, sizeof ( l_sWriter ) );
	l_sWriter.fd = a_iFileDescriptor;
	l_sWriter.events = POLLOUT;
	return ( ( poll( &l_sWriter, 1, 0 ) == 1 ) && ( l_sWriter.revents & POLLOUT ) && ! ( l_sWriter.revents & POLLHUP ) );
	}

int HRawFile::do_write( void const* const a_pcBuffer, int const a_iSize )
	{
	M_PROLOG
	if ( is_write_ready( f_iFileDescriptor ) )
		return ( (this->*writer)( a_pcBuffer, a_iSize ) ); 
	return ( -1 );
	M_EPILOG
	}

bool HRawFile::wait_for( ACTION::action_t const& a_eAction, void* a_pvTime )
	{
	int l_iError = - 1;
	fd_set l_xFdSet;
	timeval* l_pxWait = static_cast<timeval*>( a_pvTime );
	while ( l_pxWait->tv_sec || l_pxWait->tv_usec )
		{
		FD_ZERO( & l_xFdSet );
		FD_SET( f_iFileDescriptor, &l_xFdSet );
		l_iError = TEMP_FAILURE_RETRY( ::select( FD_SETSIZE,
					( a_eAction == ACTION::D_READ ) ? &l_xFdSet : NULL,
					( a_eAction == ACTION::D_WRITE ) ? &l_xFdSet : NULL,
					NULL, l_pxWait ) );
		if ( l_iError < 0 )
			break;
		else if ( ( l_iError > 0 )
				&& FD_ISSET( f_iFileDescriptor, &l_xFdSet ) )
			break;
		else if ( ! l_iError )
			break;
		}
	return ( l_iError <= 0 );
	}

int HRawFile::write_plain( void const* const a_pcBuffer, int const a_iSize )
	{
	M_PROLOG
	if ( f_iFileDescriptor < 0 )
		M_THROW ( n_pcError, errno );
	int iWritten = 0;
	timeval tv = { f_iTimeOut, 0 };
	do
		{
		if ( ( f_iTimeOut > 0 ) && wait_for( ACTION::D_WRITE, &tv ) )
			throw HStreamInterfaceException( _( "timeout on write" ) );
		int ret = TEMP_FAILURE_RETRY( ::write( f_iFileDescriptor,
					static_cast<char const* const>( a_pcBuffer ) + iWritten,
					a_iSize - iWritten ) );
		if ( ret < 0 )
			{
			iWritten = ret;
			break;
			}
		else
			iWritten += ret;
		}
	while ( iWritten < a_iSize );
	return ( iWritten );
	M_EPILOG
	}

int HRawFile::write_ssl( void const* const a_pcBuffer, int const a_iSize )
	{
	M_PROLOG
	if ( f_iFileDescriptor < 0 )
		M_THROW ( n_pcError, errno );
	return ( f_oSSL->write( a_pcBuffer, a_iSize ) );
	M_EPILOG
	}

int HRawFile::write_ssl_loader( void const* const a_pcBuffer, int const a_iSize )
	{
	M_PROLOG
	f_oSSL = HOpenSSL::ptr_t( new HOpenSSL( f_iFileDescriptor, f_eType & TYPE::D_SSL_SERVER ? HOpenSSL::TYPE::D_SERVER : HOpenSSL::TYPE::D_CLIENT ) );
	reader = &HRawFile::read_ssl;
	writer = &HRawFile::write_ssl;
	closer = &HRawFile::close_ssl;
	return ( write_ssl( a_pcBuffer, a_iSize ) );
	M_EPILOG
	}

int HRawFile::do_read( void* const a_pcBuffer, int const a_iSize )
	{
	M_PROLOG
	return ( (this->*reader)( a_pcBuffer, a_iSize ) );
	M_EPILOG
	}

void HRawFile::do_flush( void ) const
	{
	}

void HRawFile::set_timeout( int a_iTimeout )
	{
	f_iTimeOut = a_iTimeout;
	return;
	}

}

}

