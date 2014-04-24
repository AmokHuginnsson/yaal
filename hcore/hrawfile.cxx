/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	hrawfile.cxx - this file is integral part of `yaal' project.

  i.  You may not make any changes in Copyright information.
  ii. You must attach Copyright information to any part of every copy
      of this software.

Copyright:

 You can use this software free of charge and you can redistribute its binary
 package freely but:
  1. You are not allowed to use any part of sources of this software.
  2. You are not allowed to redistribute any part of sources of this software.
  3. You are not allowed to reverse engineer this software.
  4. If you want to distribute a binary package of this software you cannot
     demand any fees for it. You cannot even demand
     a return of cost of the media or distribution (CD for example).
  5. You cannot involve this software in any commercial activity (for example
     as a free add-on to paid software or newspaper).
 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE. Use it at your own risk.
*/

#include <cstring>
#include <unistd.h>
#include <libintl.h>
#include <sys/time.h> /* timeval */

#include "base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "hrawfile.hxx"
#include "hcore/memory.hxx"
#include "system.hxx"
#include "hclock.hxx"
#include "hlog.hxx"

namespace yaal {

namespace hcore {

char const* const _error_ = _( "file is not opened" );
int long _writeTimeout_ = 4 * 1000; /* 4 seconds */

HRawFile::HRawFile( TYPE::raw_file_type_t type_ )
	: _type( type_ ), _fileDescriptor( -1 ), _timeout( _writeTimeout_ ), _ssl(),
	reader( ( ( type_ & TYPE::SSL_SERVER )
				|| ( type_ & TYPE::SSL_CLIENT ) ) ? &HRawFile::read_ssl_loader : &HRawFile::read_plain ),
	writer( ( ( type_ & TYPE::SSL_SERVER )
				|| ( type_ & TYPE::SSL_CLIENT ) ) ? &HRawFile::write_ssl_loader : &HRawFile::write_plain ),
	closer( &HRawFile::close_plain ) {
	M_PROLOG
	M_ASSERT( ! ( ( type_ == TYPE::DEFAULT ) && ( type_ & TYPE::SSL_SERVER ) ) );
	M_ASSERT( ! ( ( type_ == TYPE::DEFAULT ) && ( type_ & TYPE::SSL_CLIENT ) ) );
	M_ASSERT( ! ( ( type_ & TYPE::PLAIN ) && ( type_ & TYPE::SSL_SERVER ) ) );
	M_ASSERT( ! ( ( type_ & TYPE::PLAIN ) && ( type_ & TYPE::SSL_CLIENT ) ) );
	M_ASSERT( ! ( ( type_ & TYPE::SSL_CLIENT ) && ( type_ & TYPE::SSL_SERVER ) ) );
	return;
	M_EPILOG
}

HRawFile::HRawFile( file_descriptor_t fd_ )
	: _type( TYPE::DEFAULT ), _fileDescriptor( fd_ ), _timeout( _writeTimeout_ ), _ssl(),
	reader( &HRawFile::read_plain ), writer( &HRawFile::write_plain ), closer( &HRawFile::close_plain ) {
	M_PROLOG
	return;
	M_EPILOG
}

HRawFile::~HRawFile( void ) {
	M_PROLOG
	if ( _fileDescriptor >= 0 )
		HRawFile::close();
	M_DESTRUCTOR_EPILOG
}

int HRawFile::close( void ) {
	M_PROLOG
	return ( (this->*closer)() );
	M_EPILOG
}

int HRawFile::close_plain( void ) {
	M_PROLOG
	return ( do_close() );
	M_EPILOG
}

int HRawFile::close_ssl( void ) {
	M_PROLOG
	_ssl->shutdown();
	return ( do_close() );
	M_EPILOG
}

int HRawFile::do_close( void ) {
	M_PROLOG
	int error( 0 );
	if ( _fileDescriptor < 0 )
		M_THROW( "file is not opened", errno );
	error = static_cast<int>( M_TEMP_FAILURE_RETRY( system::close( _fileDescriptor ) ) );
	_fileDescriptor = -1;
	return ( error );
	M_EPILOG
}

file_descriptor_t HRawFile::get_file_descriptor( void ) const {
	M_PROLOG
	return ( _fileDescriptor );
	M_EPILOG
}

int long HRawFile::read_plain( void* const buffer_, int long size_ ) {
	M_PROLOG
	if ( _fileDescriptor < 0 )
		M_THROW( _error_, errno );
	return ( ::read( _fileDescriptor, buffer_, static_cast<size_t>( size_ ) ) );
	M_EPILOG
}

int long HRawFile::read_ssl( void* const buffer_, int long size_ ) {
	M_PROLOG
	if ( _fileDescriptor < 0 )
		M_THROW( _error_, errno );
	return ( _ssl->read( buffer_, size_ ) );
	M_EPILOG
}

int long HRawFile::read_ssl_loader( void* const buffer_, int long size_ ) {
	M_PROLOG
	_ssl = HOpenSSL::ptr_t( make_pointer<HOpenSSL>( _fileDescriptor, _type & TYPE::SSL_SERVER ? HOpenSSL::TYPE::SERVER : HOpenSSL::TYPE::CLIENT ) );
	reader = &HRawFile::read_ssl;
	writer = &HRawFile::write_ssl;
	closer = &HRawFile::close_ssl;
	return ( read_ssl( buffer_, size_ ) );
	M_EPILOG
}

int long HRawFile::do_write( void const* const buffer_, int long size_ ) {
	M_PROLOG
	return ( (this->*writer)( buffer_, size_ ) );
	M_EPILOG
}

bool HRawFile::wait_for( ACTION::action_t const& action_, int long* time_ ) {
	int fdR( action_ & ACTION::READ ? _fileDescriptor : -1 );
	int fdW( action_ & ACTION::WRITE ? _fileDescriptor : -1 );
	int error( system::wait_for_io( action_ & ACTION::READ ? &fdR : NULL,
				action_ & ACTION::READ ? 1 : 0,
				action_ & ACTION::WRITE ? &fdW : NULL,
				action_ & ACTION::WRITE ? 1 : 0, time_ ) );
	return ( ( error <= 0 ) || ( ( fdR == -1 ) && ( fdW == -1 ) ) );
}

int long HRawFile::write_plain( void const* const buffer_, int long size_ ) {
	M_PROLOG
	if ( _fileDescriptor < 0 )
		M_THROW( _error_, errno );
	int long iWritten = 0;
	int long timeOut( _timeout );
	do {
		if ( ( _timeout > 0 ) && wait_for( ACTION::WRITE, &timeOut ) )
			throw HStreamInterfaceException( _( "timeout on write" ) );
		int long ret = M_TEMP_FAILURE_RETRY( ::write( _fileDescriptor,
					static_cast<char const* const>( buffer_ ) + iWritten,
					static_cast<size_t>( size_ - iWritten ) ) );
		if ( ret < 0 ) {
			iWritten = ret;
			break;
		} else
			iWritten += ret;
	} while ( iWritten < size_ );
	return ( iWritten );
	M_EPILOG
}

int long HRawFile::write_ssl( void const* const buffer_, int long size_ ) {
	M_PROLOG
	if ( _fileDescriptor < 0 )
		M_THROW( _error_, errno );
	HClock clk;
	int long nWritten = 0;
	int long timeOut( _timeout );
	do {
		if ( ( _timeout > 0 ) && wait_for( ACTION::BOTH, &timeOut ) )
			throw HStreamInterfaceException( _( "timeout on write" ) );
		int long ret = _ssl->write( static_cast<char const* const>( buffer_ ) + nWritten, size_ );
		if ( ! ret ) {
			nWritten = ret;
			break;
		} else if ( ret > 0 )
			nWritten += ret;
	} while ( nWritten < size_ );
	return ( nWritten );
	M_EPILOG
}

int long HRawFile::write_ssl_loader( void const* const buffer_, int long size_ ) {
	M_PROLOG
	_ssl = HOpenSSL::ptr_t( make_pointer<HOpenSSL>( _fileDescriptor, _type & TYPE::SSL_SERVER ? HOpenSSL::TYPE::SERVER : HOpenSSL::TYPE::CLIENT ) );
	reader = &HRawFile::read_ssl;
	writer = &HRawFile::write_ssl;
	closer = &HRawFile::close_ssl;
	return ( write_ssl( buffer_, size_ ) );
	M_EPILOG
}

int long HRawFile::do_read( void* const buffer_, int long size_ ) {
	M_PROLOG
	return ( (this->*reader)( buffer_, size_ ) );
	M_EPILOG
}

void HRawFile::do_flush( void ) {
}

void HRawFile::set_timeout( int long timeout_ ) {
	_timeout = timeout_;
	if ( _timeout < LOW_TIMEOUT_WARNING )
		log( LOG_TYPE::WARNING ) << "Low timout for write operations!" << endl;
	return;
}

bool HRawFile::do_is_valid( void ) const {
	M_PROLOG
	return ( _fileDescriptor >= 0 );
	M_EPILOG
}

}

}

