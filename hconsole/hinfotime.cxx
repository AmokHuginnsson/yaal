/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

  hinfotime.cxx - this file is integral part of `yaal' project.

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

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "hinfotime.hxx"

using namespace yaal;
using namespace yaal::hcore;

namespace yaal {

namespace hconsole {

HInfoTime::HInfoTime( HTime& string_ )
	: _data( string_ ) {
}

HInfoTime::HInfoTime( HInfoTime const& info_ )
	: HInfo(), _data( info_._data ) {
}

HInfoTime::~HInfoTime( void ) {
	return;
}

HString const& HInfoTime::do_get_string( void ) const {
	M_ASSERT( 0 && "impossible inplace conversion requested" );
#if defined( NDEBUG ) || defined( __MSVCXX__ )
	static HString const dummy;
	return ( dummy );
#endif /* #if defined( NDEBUG ) || defined( __MSVCXX__ ) */
}

int long long HInfoTime::do_get_integer( void ) const {
	return ( _data.raw() );
}

double long HInfoTime::do_get_real( void ) const {
	M_ASSERT( 0 && "data lost in conversion" );
#if defined( NDEBUG ) || defined( __MSVCXX__ )
	return ( 0 );
#endif /* #if defined( NDEBUG ) || defined( __MSVCXX__ ) */
}

HTime const& HInfoTime::do_get_time( void ) const {
	return ( _data );
}

void HInfoTime::do_set_integer( int long long intLong_ ) {
	_data = HTime( intLong_, _iso8601DateTimeFormat_ );
}

void HInfoTime::do_set_real( double long ) {
	M_ASSERT( 0 && "invalid API called" );
}

void HInfoTime::do_set_string( HString const& str_ ) {
	_data.from_string( str_ );
}

void HInfoTime::do_set_time( HTime const& time_ ) {
	_data = time_;
}

}

}

