/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	hinfo.cxx - this file is integral part of `yaal' project.

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
     You cannot even demand cost of the carrier(CD for example).
  5. You cannot include it to any commercial enterprise(for example
     as a free add-on to paid software or newspaper).
 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE. Use it at your own risk.
*/

#include <cstdlib>

#include "hcore/base.hxx"
M_VCSID( "$Id: "__ID__" $" )
M_VCSID( "$Id: "__TID__" $" )
#include "hinfo.hxx"

using namespace yaal::hcore;

namespace yaal {

namespace hconsole {

HInfoMultiVal::HInfoMultiVal( void )
	: _type( TYPE::VOID ), _char( 0 ),
	_intShort( 0 ), _int( 0 ), _intLong( 0 ),
	_double( 0 ), _doubleLong( 0 ),
	_pointer( NULL ), _string( "" ), _time() {
	M_PROLOG
	return;
	M_EPILOG
}

HInfoMultiVal::HInfoMultiVal( int int_ )
	: _type( TYPE::VOID ), _char( 0 ),
	_intShort( 0 ), _int( 0 ), _intLong( 0 ),
	_double( 0 ), _doubleLong( 0 ),
	_pointer( NULL ), _string( "" ), _time() {
	M_PROLOG
	_type = TYPE::INT;
	_int = int_;
	return;
	M_EPILOG
}

HInfoMultiVal::HInfoMultiVal( int long longInt_ )
	: _type( TYPE::VOID ), _char( 0 ),
	_intShort( 0 ), _int( 0 ), _intLong( 0 ),
	_double( 0 ), _doubleLong( 0 ),
	_pointer( NULL ), _string( "" ), _time() {
	M_PROLOG
	_type = TYPE::INT_LONG;
	_intLong = longInt_;
	return;
	M_EPILOG
}

HInfoMultiVal::HInfoMultiVal ( double double_ )
	: _type( TYPE::VOID ), _char( 0 ),
	_intShort( 0 ), _int( 0 ), _intLong( 0 ),
	_double( 0 ), _doubleLong( 0 ),
	_pointer( NULL ), _string( "" ), _time() {
	M_PROLOG
	_type = TYPE::DOUBLE;
	_double = double_;
	return;
	M_EPILOG
}

HInfoMultiVal::HInfoMultiVal( void* const pointer_ )
	: _type( TYPE::VOID ), _char( 0 ),
	_intShort( 0 ), _int( 0 ), _intLong( 0 ),
	_double( 0 ), _doubleLong( 0 ),
	_pointer( NULL ), _string( "" ), _time() {
	M_PROLOG
	_type = TYPE::VOID_PTR;
	_pointer = pointer_;
	return;
	M_EPILOG
}

HInfoMultiVal::HInfoMultiVal( char const* const buffer_ )
	: _type( TYPE::VOID ), _char( 0 ),
	_intShort( 0 ), _int( 0 ), _intLong( 0 ),
	_double( 0 ), _doubleLong( 0 ),
	_pointer( NULL ), _string( "" ), _time() {
	M_PROLOG
	_type = TYPE::HSTRING;
	_string = buffer_;
	return;
	M_EPILOG
}

HInfoMultiVal::HInfoMultiVal( HString const& string_ )
	: _type( TYPE::VOID ), _char( 0 ),
	_intShort( 0 ), _int( 0 ), _intLong( 0 ),
	_double( 0 ), _doubleLong( 0 ),
	_pointer( NULL ), _string( "" ), _time() {
	M_PROLOG
	_type = TYPE::HSTRING;
	_string = string_;
	return;
	M_EPILOG
}

HInfoMultiVal::HInfoMultiVal( HTime const& time_ )
	: _type( TYPE::VOID ), _char( 0 ),
	_intShort( 0 ), _int( 0 ), _intLong( 0 ),
	_double( 0 ), _doubleLong( 0 ),
	_pointer( NULL ), _string( "" ), _time() {
	M_PROLOG
	_type = TYPE::HTIME;
	_time = time_;
	return;
	M_EPILOG
}

HInfoMultiVal::HInfoMultiVal( HInfoMultiVal const& info_ )
	: HInfo(), _type( TYPE::VOID ), _char( 0 ),
	_intShort( 0 ), _int( 0 ), _intLong( 0 ),
	_double( 0 ), _doubleLong( 0 ),
	_pointer( NULL ), _string( "" ), _time() {
	M_PROLOG
	( * this ) = info_;
	return;
	M_EPILOG
}

HInfoMultiVal::~HInfoMultiVal( void ) {
	M_PROLOG
	purge();
	return;
	M_DESTRUCTOR_EPILOG
}

void HInfoMultiVal::purge( void ) {
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

char HInfoMultiVal::do_get_byte ( int index_ ) const {
	M_PROLOG
	return ( _string[ index_ ] );
	M_EPILOG
}

HInfoMultiVal& HInfoMultiVal::operator = ( HInfoMultiVal const& info_ ) {
	M_PROLOG
	if ( this != & info_ ) {
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

char HInfoMultiVal::do_get_char( void ) const {
	M_PROLOG
	return ( static_cast<char>( _int ) );
	M_EPILOG
}

short HInfoMultiVal::do_get_int_short( void ) const {
	M_PROLOG
	return ( static_cast<short>( _int ) );
	M_EPILOG
}

int HInfoMultiVal::do_get_int( void ) const {
	M_PROLOG
	return ( _int );
	M_EPILOG
}

int long HInfoMultiVal::do_get_int_long( void ) const {
	M_PROLOG
	return ( _intLong );
	M_EPILOG
}

double HInfoMultiVal::do_get_double( void ) const {
	M_PROLOG
	return ( _double );
	M_EPILOG
}

double long HInfoMultiVal::do_get_double_long( void ) const {
	M_PROLOG
	return ( _double );
	M_EPILOG
}

HString const& HInfoMultiVal::do_get_string( void ) const {
	M_PROLOG
	return ( _string );
	M_EPILOG
}

HTime const& HInfoMultiVal::do_get_time( void ) const {
	M_PROLOG
	return ( _time );
	M_EPILOG
}

void* HInfoMultiVal::do_get_pointer( void ) const {
	M_PROLOG
	return ( _pointer );
	M_EPILOG
}

void HInfoMultiVal::do_set_char( char char_ ) {
	_char = char_;
}

void HInfoMultiVal::do_set_int_short( int short short_ ) {
	_intShort = short_;
}

void HInfoMultiVal::do_set_int( int int_ ) {
	_int = int_;
}

void HInfoMultiVal::do_set_int_long( int long intLong_ ) {
	_intLong = intLong_;
}

void HInfoMultiVal::do_set_double( double double_ ) {
	_double = double_;
}

void HInfoMultiVal::do_set_double_long( double long doubleLong_ ) {
	_doubleLong = doubleLong_;
}

void HInfoMultiVal::do_set_pointer( void* pointer_ ) {
	_pointer = pointer_;
}

void HInfoMultiVal::do_set_string( HString const& string_ ) {
	_string = string_;
}

void HInfoMultiVal::do_set_time( HTime const& time_ ) {
	_time = time_;
}

HInfoString::HInfoString( HString const& string_ )
	: _data( string_ )
	{ }

HInfoString::HInfoString( HInfoString const& info_ )
	: HInfo(), _data( info_._data )
	{ }

HInfoString::~HInfoString( void )
	{ }

HString const& HInfoString::do_get_string( void ) const
	{ return ( _data ); }

char HInfoString::do_get_byte( int idx_ ) const
	{ return ( _data[idx_] ); }

char HInfoString::do_get_char( void ) const
	{ return ( !_data.is_empty() ? _data[0] : static_cast<char>( 0 ) ); }
 
int short HInfoString::do_get_int_short( void ) const
	{ return ( static_cast<int short>( lexical_cast<int>( _data ) ) ); }

int HInfoString::do_get_int( void ) const
	{ return ( lexical_cast<int>( _data ) ); }

int long HInfoString::do_get_int_long( void ) const
	{ return ( lexical_cast<int long>( _data ) ); }

double HInfoString::do_get_double( void ) const
	{ return ( lexical_cast<double>( _data ) ); }

double long HInfoString::do_get_double_long( void ) const
	{ return ( lexical_cast<double long>( _data ) ); }

HTime const& HInfoString::do_get_time( void ) const {
	M_ASSERT( !"impossible inplace conversion requested" );
#ifdef __MSVCXX__
	static HTime const dummy;
	return ( dummy );
#endif /* #ifdef __MSVCXX__ */
}

void* HInfoString::do_get_pointer( void ) const
	{
	int long val( ::strtol( _data.raw(), NULL, 16 ) );
	return ( reinterpret_cast<void*>( val ) );
	}

void HInfoString::do_set_char( char ) {
	M_ASSERT( !"writting to read-only location" );
}

void HInfoString::do_set_int_short( int short ) {
	M_ASSERT( !"writting to read-only location" );
}

void HInfoString::do_set_int( int ) {
	M_ASSERT( !"writting to read-only location" );
}

void HInfoString::do_set_int_long( int long ) {
	M_ASSERT( !"writting to read-only location" );
}

void HInfoString::do_set_double( double ) {
	M_ASSERT( !"writting to read-only location" );
}

void HInfoString::do_set_double_long( double long ) {
	M_ASSERT( !"writting to read-only location" );
}

void HInfoString::do_set_pointer( void* ) {
	M_ASSERT( !"writting to read-only location" );
}

void HInfoString::do_set_string( HString const& ) {
	M_ASSERT( !"writting to read-only location" );
}

void HInfoString::do_set_time( HTime const& ) {
	M_ASSERT( !"writting to read-only location" );
}

}

}

