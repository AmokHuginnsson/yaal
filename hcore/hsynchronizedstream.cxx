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

HSynchronizedStreamBase::HSynchronizedStreamBase( void )
	: _mutex( HMutex::TYPE::RECURSIVE )
	{
	return;
	}

HStreamInterface& HSynchronizedStreamBase::do_output( HString const& val_ )
	{
	M_PROLOG
	HLock l( _mutex );
	return ( HStreamInterface::do_output( val_ ) );
	M_EPILOG
	}

HStreamInterface& HSynchronizedStreamBase::do_output( char const* const& val_ )
	{
	M_PROLOG
	HLock l( _mutex );
	return ( HStreamInterface::do_output( val_ ) );
	M_EPILOG
	}

HStreamInterface& HSynchronizedStreamBase::do_output( char const& val_ )
	{
	M_PROLOG
	HLock l( _mutex );
	return ( HStreamInterface::do_output( val_ ) );
	M_EPILOG
	}

HStreamInterface& HSynchronizedStreamBase::do_output( int short const& val_ )
	{
	M_PROLOG
	HLock l( _mutex );
	return ( HStreamInterface::do_output( val_ ) );
	M_EPILOG
	}

HStreamInterface& HSynchronizedStreamBase::do_output( int short unsigned const& val_ )
	{
	M_PROLOG
	HLock l( _mutex );
	return ( HStreamInterface::do_output( val_ ) );
	M_EPILOG
	}

HStreamInterface& HSynchronizedStreamBase::do_output( int const& val_ )
	{
	M_PROLOG
	HLock l( _mutex );
	return ( HStreamInterface::do_output( val_ ) );
	M_EPILOG
	}

HStreamInterface& HSynchronizedStreamBase::do_output( int unsigned const& val_ )
	{
	M_PROLOG
	HLock l( _mutex );
	return ( HStreamInterface::do_output( val_ ) );
	M_EPILOG
	}

HStreamInterface& HSynchronizedStreamBase::do_output( int long const& val_ )
	{
	M_PROLOG
	HLock l( _mutex );
	return ( HStreamInterface::do_output( val_ ) );
	M_EPILOG
	}

HStreamInterface& HSynchronizedStreamBase::do_output( int long unsigned const& val_ )
	{
	M_PROLOG
	HLock l( _mutex );
	return ( HStreamInterface::do_output( val_ ) );
	M_EPILOG
	}

HStreamInterface& HSynchronizedStreamBase::do_output( double const& val_ )
	{
	M_PROLOG
	HLock l( _mutex );
	return ( HStreamInterface::do_output( val_ ) );
	M_EPILOG
	}

HStreamInterface& HSynchronizedStreamBase::do_output( double long const& val_ )
	{
	M_PROLOG
	HLock l( _mutex );
	return ( HStreamInterface::do_output( val_ ) );
	M_EPILOG
	}

HStreamInterface& HSynchronizedStreamBase::do_output( float const& val_ )
	{
	M_PROLOG
	HLock l( _mutex );
	return ( HStreamInterface::do_output( val_ ) );
	M_EPILOG
	}

HStreamInterface& HSynchronizedStreamBase::do_output( void const* const& val_ )
	{
	M_PROLOG
	HLock l( _mutex );
	return ( HStreamInterface::do_output( val_ ) );
	M_EPILOG
	}

HStreamInterface& HSynchronizedStreamBase::do_output( manipulator_t const& val_ )
	{
	M_PROLOG
	HLock l( _mutex );
	return ( HStreamInterface::do_output( val_ ) );
	M_EPILOG
	}

HStreamInterface& HSynchronizedStreamBase::do_output( HManipulator const& val_ )
	{
	M_PROLOG
	HLock l( _mutex );
	return ( HStreamInterface::do_output( val_ ) );
	M_EPILOG
	}

HStreamInterface& HSynchronizedStreamBase::do_input( HString& val_ )
	{
	M_PROLOG
	HLock l( _mutex );
	return ( HStreamInterface::do_input( val_ ) );
	M_EPILOG
	}

HStreamInterface& HSynchronizedStreamBase::do_input( char& val_ )
	{
	M_PROLOG
	HLock l( _mutex );
	return ( HStreamInterface::do_input( val_ ) );
	M_EPILOG
	}

HStreamInterface& HSynchronizedStreamBase::do_input( int short& val_ )
	{
	M_PROLOG
	HLock l( _mutex );
	return ( HStreamInterface::do_input( val_ ) );
	M_EPILOG
	}

HStreamInterface& HSynchronizedStreamBase::do_input( int short unsigned& val_ )
	{
	M_PROLOG
	HLock l( _mutex );
	return ( HStreamInterface::do_input( val_ ) );
	M_EPILOG
	}

HStreamInterface& HSynchronizedStreamBase::do_input( int& val_ )
	{
	M_PROLOG
	HLock l( _mutex );
	return ( HStreamInterface::do_input( val_ ) );
	M_EPILOG
	}

HStreamInterface& HSynchronizedStreamBase::do_input( int unsigned& val_ )
	{
	M_PROLOG
	HLock l( _mutex );
	return ( HStreamInterface::do_input( val_ ) );
	M_EPILOG
	}

HStreamInterface& HSynchronizedStreamBase::do_input( int long& val_ )
	{
	M_PROLOG
	HLock l( _mutex );
	return ( HStreamInterface::do_input( val_ ) );
	M_EPILOG
	}

HStreamInterface& HSynchronizedStreamBase::do_input( int long unsigned& val_ )
	{
	M_PROLOG
	HLock l( _mutex );
	return ( HStreamInterface::do_input( val_ ) );
	M_EPILOG
	}

HStreamInterface& HSynchronizedStreamBase::do_input( double& val_ )
	{
	M_PROLOG
	HLock l( _mutex );
	return ( HStreamInterface::do_input( val_ ) );
	M_EPILOG
	}

HStreamInterface& HSynchronizedStreamBase::do_input( double long& val_ )
	{
	M_PROLOG
	HLock l( _mutex );
	return ( HStreamInterface::do_input( val_ ) );
	M_EPILOG
	}

HStreamInterface& HSynchronizedStreamBase::do_input( float& val_ )
	{
	M_PROLOG
	HLock l( _mutex );
	return ( HStreamInterface::do_input( val_ ) );
	M_EPILOG
	}

HStreamInterface& HSynchronizedStreamBase::do_input( void const*& val_ )
	{
	M_PROLOG
	HLock l( _mutex );
	return ( HStreamInterface::do_input( val_ ) );
	M_EPILOG
	}

int long HSynchronizedStreamBase::do_read_until( yaal::hcore::HString& store, char const* const delim = eols, bool strip = true )
	{
	M_PROLOG
	HLock l( _mutex );
	return ( HStreamInterface::do_read_until( store, delim, strip ) );
	M_EPILOG
	}

int long HSynchronizedStreamBase::do_read_until_n( yaal::hcore::HString& store, int long const& maxcount, char const* const delim = eols, bool strip = true )
	{
	M_PROLOG
	HLock l( _mutex );
	return ( HStreamInterface::do_read_until_n( store, maxcount, delim, strip ) );
	M_EPILOG
	}

HStreamInterface& HSynchronizedStreamBase::do_set_fill( int val_ )
	{
	M_PROLOG
	HLock l( _mutex );
	return ( HStreamInterface::do_set_fill( val_ ) );
	M_EPILOG
	}

HStreamInterface& HSynchronizedStreamBase::do_set_width( int val_ )
	{
	M_PROLOG
	HLock l( _mutex );
	return ( HStreamInterface::do_set_width( val_ ) );
	M_EPILOG
	}

HStreamInterface& HSynchronizedStreamBase::do_set_base( BASES::enum_t val_ )
	{
	M_PROLOG
	HLock l( _mutex );
	return ( HStreamInterface::do_set_base( val_ ) );
	M_EPILOG
	}

}

}

