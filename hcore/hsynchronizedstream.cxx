/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hcore/hsynchronizedstream.cxx - this file is integral part of `yaal' project.

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

#include "hcore/base.hxx"
M_VCSID( "$Id: "__ID__" $" )
#include "hsynchronizedstream.hxx"

namespace yaal
{

namespace hcore
{

HSynchronizedStream::HSynchronizedStream( void )
	: _mutex( HMutex::TYPE::RECURSIVE ), _streamOwned(), _streamRef( NULL )
	{
	return;
	}

HSynchronizedStream::HSynchronizedStream( owned_stream_t stream_ )
	: _mutex( HMutex::TYPE::RECURSIVE ), _streamOwned( stream_ ), _streamRef( _streamOwned.raw() )
	{
	M_PROLOG
	return;
	M_EPILOG
	}

HSynchronizedStream::HSynchronizedStream( ref_stream_t stream_ )
	: _mutex( HMutex::TYPE::RECURSIVE ), _streamOwned(), _streamRef( &stream_ )
	{
	M_PROLOG
	return;
	M_EPILOG
	}

void HSynchronizedStream::reset( owned_stream_t stream_ )
	{
	M_PROLOG
	_streamOwned = stream_;
	_streamRef = _streamOwned.raw();
	M_EPILOG
	}

void HSynchronizedStream::reset( ref_stream_t stream_ )
	{
	M_PROLOG
	_streamOwned.reset();
	_streamRef = &stream_;
	M_EPILOG
	}

HStreamInterface& HSynchronizedStream::do_output( HString const& val_ )
	{
	M_PROLOG
	HLock l( _mutex );
	return ( HStreamInterface::do_output( val_ ) );
	M_EPILOG
	}

HStreamInterface& HSynchronizedStream::do_output( char const* val_ )
	{
	M_PROLOG
	HLock l( _mutex );
	return ( HStreamInterface::do_output( val_ ) );
	M_EPILOG
	}

HStreamInterface& HSynchronizedStream::do_output( bool val_ )
	{
	M_PROLOG
	HLock l( _mutex );
	return ( HStreamInterface::do_output( val_ ) );
	M_EPILOG
	}

HStreamInterface& HSynchronizedStream::do_output( char val_ )
	{
	M_PROLOG
	HLock l( _mutex );
	return ( HStreamInterface::do_output( val_ ) );
	M_EPILOG
	}

HStreamInterface& HSynchronizedStream::do_output( char unsigned val_ )
	{
	M_PROLOG
	HLock l( _mutex );
	return ( HStreamInterface::do_output( val_ ) );
	M_EPILOG
	}

HStreamInterface& HSynchronizedStream::do_output( int short val_ )
	{
	M_PROLOG
	HLock l( _mutex );
	return ( HStreamInterface::do_output( val_ ) );
	M_EPILOG
	}

HStreamInterface& HSynchronizedStream::do_output( int short unsigned val_ )
	{
	M_PROLOG
	HLock l( _mutex );
	return ( HStreamInterface::do_output( val_ ) );
	M_EPILOG
	}

HStreamInterface& HSynchronizedStream::do_output( int val_ )
	{
	M_PROLOG
	HLock l( _mutex );
	return ( HStreamInterface::do_output( val_ ) );
	M_EPILOG
	}

HStreamInterface& HSynchronizedStream::do_output( int unsigned val_ )
	{
	M_PROLOG
	HLock l( _mutex );
	return ( HStreamInterface::do_output( val_ ) );
	M_EPILOG
	}

HStreamInterface& HSynchronizedStream::do_output( int long val_ )
	{
	M_PROLOG
	HLock l( _mutex );
	return ( HStreamInterface::do_output( val_ ) );
	M_EPILOG
	}

HStreamInterface& HSynchronizedStream::do_output( int long unsigned val_ )
	{
	M_PROLOG
	HLock l( _mutex );
	return ( HStreamInterface::do_output( val_ ) );
	M_EPILOG
	}

HStreamInterface& HSynchronizedStream::do_output( int long long val_ )
	{
	M_PROLOG
	HLock l( _mutex );
	return ( HStreamInterface::do_output( val_ ) );
	M_EPILOG
	}

HStreamInterface& HSynchronizedStream::do_output( int long long unsigned val_ )
	{
	M_PROLOG
	HLock l( _mutex );
	return ( HStreamInterface::do_output( val_ ) );
	M_EPILOG
	}

HStreamInterface& HSynchronizedStream::do_output( double val_ )
	{
	M_PROLOG
	HLock l( _mutex );
	return ( HStreamInterface::do_output( val_ ) );
	M_EPILOG
	}

HStreamInterface& HSynchronizedStream::do_output( double long val_ )
	{
	M_PROLOG
	HLock l( _mutex );
	return ( HStreamInterface::do_output( val_ ) );
	M_EPILOG
	}

HStreamInterface& HSynchronizedStream::do_output( float val_ )
	{
	M_PROLOG
	HLock l( _mutex );
	return ( HStreamInterface::do_output( val_ ) );
	M_EPILOG
	}

HStreamInterface& HSynchronizedStream::do_output( void const* val_ )
	{
	M_PROLOG
	HLock l( _mutex );
	return ( HStreamInterface::do_output( val_ ) );
	M_EPILOG
	}

HStreamInterface& HSynchronizedStream::do_output( manipulator_t const& val_ )
	{
	M_PROLOG
	HLock l( _mutex );
	return ( HStreamInterface::do_output( val_ ) );
	M_EPILOG
	}

HStreamInterface& HSynchronizedStream::do_output( HManipulator const& val_ )
	{
	M_PROLOG
	HLock l( _mutex );
	return ( HStreamInterface::do_output( val_ ) );
	M_EPILOG
	}

HStreamInterface& HSynchronizedStream::do_input( HString& val_ )
	{
	M_PROLOG
	HLock l( _mutex );
	return ( HStreamInterface::do_input( val_ ) );
	M_EPILOG
	}

HStreamInterface& HSynchronizedStream::do_input( bool& val_ )
	{
	M_PROLOG
	HLock l( _mutex );
	return ( HStreamInterface::do_input( val_ ) );
	M_EPILOG
	}

HStreamInterface& HSynchronizedStream::do_input( char& val_ )
	{
	M_PROLOG
	HLock l( _mutex );
	return ( HStreamInterface::do_input( val_ ) );
	M_EPILOG
	}

HStreamInterface& HSynchronizedStream::do_input( char unsigned& val_ )
	{
	M_PROLOG
	HLock l( _mutex );
	return ( HStreamInterface::do_input( val_ ) );
	M_EPILOG
	}

HStreamInterface& HSynchronizedStream::do_input( int short& val_ )
	{
	M_PROLOG
	HLock l( _mutex );
	return ( HStreamInterface::do_input( val_ ) );
	M_EPILOG
	}

HStreamInterface& HSynchronizedStream::do_input( int short unsigned& val_ )
	{
	M_PROLOG
	HLock l( _mutex );
	return ( HStreamInterface::do_input( val_ ) );
	M_EPILOG
	}

HStreamInterface& HSynchronizedStream::do_input( int& val_ )
	{
	M_PROLOG
	HLock l( _mutex );
	return ( HStreamInterface::do_input( val_ ) );
	M_EPILOG
	}

HStreamInterface& HSynchronizedStream::do_input( int unsigned& val_ )
	{
	M_PROLOG
	HLock l( _mutex );
	return ( HStreamInterface::do_input( val_ ) );
	M_EPILOG
	}

HStreamInterface& HSynchronizedStream::do_input( int long& val_ )
	{
	M_PROLOG
	HLock l( _mutex );
	return ( HStreamInterface::do_input( val_ ) );
	M_EPILOG
	}

HStreamInterface& HSynchronizedStream::do_input( int long unsigned& val_ )
	{
	M_PROLOG
	HLock l( _mutex );
	return ( HStreamInterface::do_input( val_ ) );
	M_EPILOG
	}

HStreamInterface& HSynchronizedStream::do_input( int long long& val_ )
	{
	M_PROLOG
	HLock l( _mutex );
	return ( HStreamInterface::do_input( val_ ) );
	M_EPILOG
	}

HStreamInterface& HSynchronizedStream::do_input( int long long unsigned& val_ )
	{
	M_PROLOG
	HLock l( _mutex );
	return ( HStreamInterface::do_input( val_ ) );
	M_EPILOG
	}

HStreamInterface& HSynchronizedStream::do_input( double& val_ )
	{
	M_PROLOG
	HLock l( _mutex );
	return ( HStreamInterface::do_input( val_ ) );
	M_EPILOG
	}

HStreamInterface& HSynchronizedStream::do_input( double long& val_ )
	{
	M_PROLOG
	HLock l( _mutex );
	return ( HStreamInterface::do_input( val_ ) );
	M_EPILOG
	}

HStreamInterface& HSynchronizedStream::do_input( float& val_ )
	{
	M_PROLOG
	HLock l( _mutex );
	return ( HStreamInterface::do_input( val_ ) );
	M_EPILOG
	}

HStreamInterface& HSynchronizedStream::do_input( void const*& val_ )
	{
	M_PROLOG
	HLock l( _mutex );
	return ( HStreamInterface::do_input( val_ ) );
	M_EPILOG
	}

HStreamInterface& HSynchronizedStream::do_input( manipulator_t const& val_ )
	{
	M_PROLOG
	HLock l( _mutex );
	return ( HStreamInterface::do_input( val_ ) );
	M_EPILOG
	}

int long HSynchronizedStream::do_read_until( yaal::hcore::HString& store, char const* const delim, bool strip )
	{
	M_PROLOG
	HLock l( _mutex );
	return ( HStreamInterface::do_read_until( store, delim, strip ) );
	M_EPILOG
	}

int long HSynchronizedStream::do_read_until_n( yaal::hcore::HString& store, int long maxcount, char const* const delim, bool strip )
	{
	M_PROLOG
	HLock l( _mutex );
	return ( HStreamInterface::do_read_until_n( store, maxcount, delim, strip ) );
	M_EPILOG
	}

int long HSynchronizedStream::do_read_while( yaal::hcore::HString& store, char const* const acquire, bool strip )
	{
	M_PROLOG
	HLock l( _mutex );
	return ( HStreamInterface::do_read_while( store, acquire, strip ) );
	M_EPILOG
	}

int long HSynchronizedStream::do_read_while_n( yaal::hcore::HString& store, int long maxcount, char const* const acquire, bool strip )
	{
	M_PROLOG
	HLock l( _mutex );
	return ( HStreamInterface::do_read_while_n( store, maxcount, acquire, strip ) );
	M_EPILOG
	}

int HSynchronizedStream::do_peek( void )
	{
	M_PROLOG
	HLock l( _mutex );
	return ( HStreamInterface::do_peek() );
	M_EPILOG
	}

HStreamInterface& HSynchronizedStream::do_set_fill( int val_ )
	{
	M_PROLOG
	HLock l( _mutex );
	return ( HStreamInterface::do_set_fill( val_ ) );
	M_EPILOG
	}

HStreamInterface& HSynchronizedStream::do_set_width( int val_ )
	{
	M_PROLOG
	HLock l( _mutex );
	return ( HStreamInterface::do_set_width( val_ ) );
	M_EPILOG
	}

HStreamInterface& HSynchronizedStream::do_set_base( BASES::enum_t val_ )
	{
	M_PROLOG
	HLock l( _mutex );
	return ( HStreamInterface::do_set_base( val_ ) );
	M_EPILOG
	}

HStreamInterface& HSynchronizedStream::do_set_skipws( bool val_ )
	{
	M_PROLOG
	HLock l( _mutex );
	return ( HStreamInterface::do_set_skipws( val_ ) );
	M_EPILOG
	}

int long HSynchronizedStream::do_write( void const* const buf_, int long size_ )
	{
	M_PROLOG
	return ( _streamRef ? _streamRef->write( buf_, size_ ) : 0 );
	M_EPILOG
	}

int long HSynchronizedStream::do_read( void* const buf_, int long size_ )
	{
	M_PROLOG
	return ( _streamRef ? _streamRef->read( buf_, size_ ) : 0 );
	M_EPILOG
	}

void HSynchronizedStream::do_flush( void ) const
	{
	M_PROLOG
	if ( _streamRef )
		_streamRef->flush();
	return;
	M_EPILOG
	}

bool HSynchronizedStream::do_is_valid( void ) const
	{
	M_PROLOG
	return ( _streamRef ? _streamRef->is_valid() : false );
	M_EPILOG
	}

}

}

