/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	hinfo.cxx - this file is integral part of `yaal' project.

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
     You cannot even demand cost of the carrier(CD for example).
  5. You cannot include it to any commercial enterprise(for example 
     as a free add-on to payed software or payed newspaper).
 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE. Use it at your own risk.
*/

#include "base.hxx"
M_VCSID( "$Id: "__ID__" $" )
M_VCSID( "$Id: "__TID__" $" )
#include "hinfo.hxx"

namespace yaal
{

namespace hcore
{

HInfo::HInfo( void )
	: _type( TYPE::VOID ), _int( 0 ), _intLong( 0 ), _double( 0 ), _pointer( NULL ), _string( "" ), _time( )
	{
	M_PROLOG
	return;
	M_EPILOG
	}

HInfo::HInfo( int const& int_ )
	: _type( TYPE::VOID ), _int( 0 ), _intLong( 0 ), _double( 0 ), _pointer( NULL ), _string( "" ), _time( )
	{
	M_PROLOG
	_type = TYPE::INT;
	_int = int_;
	return;
	M_EPILOG
	}

HInfo::HInfo( int long const& longInt_ )
	: _type( TYPE::VOID ), _int( 0 ), _intLong( 0 ), _double( 0 ), _pointer( NULL ), _string( "" ), _time( )
	{
	M_PROLOG
	_type = TYPE::INT_LONG;
	_intLong = longInt_;
	return;
	M_EPILOG
	}

HInfo::HInfo ( double const& double_ )
	: _type( TYPE::VOID ), _int( 0 ), _intLong( 0 ), _double( 0 ), _pointer( NULL ), _string( "" ), _time( )
	{
	M_PROLOG
	_type = TYPE::DOUBLE;
	_double = double_;
	return;
	M_EPILOG
	}

HInfo::HInfo( void* const pointer_ )
	: _type( TYPE::VOID ), _int( 0 ), _intLong( 0 ), _double( 0 ), _pointer( NULL ), _string( "" ), _time( )
	{
	M_PROLOG
	_type = TYPE::VOID_PTR;
	_pointer = pointer_;
	return;
	M_EPILOG
	}

HInfo::HInfo( char const* const buffer_ )
	: _type( TYPE::VOID ), _int( 0 ), _intLong( 0 ), _double( 0 ), _pointer( NULL ), _string( "" ), _time( )
	{
	M_PROLOG
	_type = TYPE::HSTRING;
	_string = buffer_;
	return;
	M_EPILOG
	}

HInfo::HInfo( HString const& string_ )
	: _type( TYPE::VOID ), _int( 0 ), _intLong( 0 ), _double( 0 ), _pointer( NULL ), _string( "" ), _time( )
	{
	M_PROLOG
	_type = TYPE::HSTRING;
	_string = string_;
	return;
	M_EPILOG
	}

HInfo::HInfo( HTime const& time_ )
	: _type( TYPE::VOID ), _int( 0 ), _intLong( 0 ), _double( 0 ), _pointer( NULL ), _string( "" ), _time( )
	{
	M_PROLOG
	_type = TYPE::HTIME;
	_time = time_;
	return;
	M_EPILOG
	}

HInfo::HInfo( HInfo const& info_ )
	: _type( TYPE::VOID ), _int( 0 ), _intLong( 0 ), _double( 0 ), _pointer( NULL ), _string( "" ), _time( )
	{
	M_PROLOG
	( * this ) = info_;
	return;
	M_EPILOG
	}

HInfo::~HInfo( void )
	{
	M_PROLOG
	purge( );
	return;
	M_EPILOG
	}

void HInfo::purge( void )
	{
	M_PROLOG
	_type = TYPE::VOID;
	_int = 0;
	_intLong = 0;
	_double = 0;
	_string = "";
	_time = HTime( );
	_pointer = NULL;
	return;
	M_EPILOG
	}

char HInfo::operator [] ( int const index_ )
	{
	M_PROLOG
	return ( _string[ index_ ] );
	M_EPILOG
	}

HInfo& HInfo::operator = ( HInfo const& info_ )
	{
	M_PROLOG
	if ( this != & info_ )
		{
		_type = info_._type;
		_int = info_._int;
		_intLong = info_._intLong;
		_double = info_._double;
		_string = info_._string;
		_time = info_._time;
		_pointer = info_._pointer;
		}
	return ( * this );
	M_EPILOG
	}

HInfo& HInfo::operator () ( HInfo const& info_ )
	{
	M_PROLOG
	if ( !!( info_._type & TYPE::INT ) )
		_int = info_._int;
	if ( !!( info_._type & TYPE::INT_LONG ) )
		_intLong = info_._intLong;
	if ( !!( info_._type & TYPE::DOUBLE ) )
		_double = info_._double;
	if ( !!( info_._type & TYPE::HSTRING ) )
		_string = info_._string;
	if ( !!( info_._type & TYPE::HTIME ) )
		_time = info_._time;
	if ( !!( info_._type & TYPE::VOID_PTR ) )
		_pointer = info_._pointer;
	_type |= info_._type;
	return ( * this );
	M_EPILOG
	}

template<>
int HInfo::get<int>( void ) const
	{
	M_PROLOG
	return ( _int );
	M_EPILOG
	}

template<>
int long HInfo::get<int long>( void ) const
	{
	M_PROLOG
	return ( _intLong );
	M_EPILOG
	}

template<>
double HInfo::get<double>( void ) const
	{
	M_PROLOG
	return ( _double );
	M_EPILOG
	}

template<>
char const* HInfo::get<char const*>( void ) const
	{
	M_PROLOG
	return ( _string.raw() );
	M_EPILOG
	}

template<>
HString const& HInfo::get<HString const&>( void ) const
	{
	M_PROLOG
	return ( _string );
	M_EPILOG
	}

template<>
HTime const& HInfo::get<HTime const&>( void ) const
	{
	M_PROLOG
	return ( _time );
	M_EPILOG
	}

template<>
void* HInfo::get<void*>( void ) const
	{
	M_PROLOG
	void* tmp = _pointer;
	return ( tmp );
	M_EPILOG
	}

bool HInfo::operator ! ( void ) const
	{
	M_PROLOG
	return ( ! ( _intLong || _double || ! _string.empty() ) );
	M_EPILOG
	}

}

}

