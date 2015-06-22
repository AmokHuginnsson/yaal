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
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "hinfo.hxx"

using namespace yaal::hcore;

namespace yaal {

namespace hconsole {

HInfoMultiVal::HInfoMultiVal( void )
	: _type( static_cast<type_mask_t>( TYPE::UNKNOWN ) ),
	_integer( 0 ), _real( 0 ),
	_string(), _time( HTime::TZ::LOCAL ) {
	M_PROLOG
	return;
	M_EPILOG
}

HInfoMultiVal::HInfoMultiVal( int long long data_ )
	: _type( static_cast<type_mask_t>( TYPE::INT_LONG_LONG ) ),
	_integer( data_ ), _real( 0 ),
	_string(), _time( HTime::TZ::LOCAL ) {
	M_PROLOG
	return;
	M_EPILOG
}

HInfoMultiVal::HInfoMultiVal( double long data_ )
	: _type( static_cast<type_mask_t>( TYPE::DOUBLE_LONG ) ),
	_integer( 0 ), _real( data_ ),
	_string(), _time( HTime::TZ::LOCAL ) {
	M_PROLOG
	return;
	M_EPILOG
}

HInfoMultiVal::HInfoMultiVal( HString const& string_ )
	: _type( static_cast<type_mask_t>( TYPE::HSTRING ) ),
	_integer( 0 ), _real( 0 ),
	_string( string_ ), _time( HTime::TZ::LOCAL ) {
	M_PROLOG
	return;
	M_EPILOG
}

HInfoMultiVal::HInfoMultiVal( HTime const& time_ )
	: _type( static_cast<type_mask_t>( TYPE::HTIME ) ),
	_integer( 0 ), _real( 0 ),
	_string(), _time( time_ ) {
	M_PROLOG
	return;
	M_EPILOG
}

HInfoMultiVal::HInfoMultiVal( HInfoMultiVal const& info_ )
	: HInfo(), _type( info_._type ),
	_integer( info_._integer ),
	_real( info_._real ),
	_string( info_._string ),
	_time( info_._time ) {
	M_PROLOG
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
	_type = static_cast<type_mask_t>( TYPE::UNKNOWN );
	_integer = 0;
	_real = 0;
	_string.clear();
	_time.set_now();
	return;
	M_EPILOG
}

HInfoMultiVal& HInfoMultiVal::operator = ( HInfoMultiVal const& info_ ) {
	M_PROLOG
	if ( &info_ != this ) {
		HInfoMultiVal tmp( info_ );
		swap( tmp );
	}
	return ( * this );
	M_EPILOG
}

void HInfoMultiVal::swap( HInfoMultiVal& info_ ) {
	if ( &info_ != this ) {
		using yaal::swap;
		swap( _type, info_._type );
		swap( _integer, info_._integer );
		swap( _real, info_._real );
		swap( _string, info_._string );
		swap( _time, info_._time );
	}
	return;
}

int long long HInfoMultiVal::do_get_integer( void ) const {
	M_PROLOG
	int long long val( _integer );
	if ( ! ( _type & static_cast<type_mask_t>( TYPE::INT_LONG_LONG ) ) ) {
		if ( _type & static_cast<type_mask_t>( TYPE::DOUBLE_LONG ) ) {
			val = static_cast<int long long>( _real );
		} else if ( _type & static_cast<type_mask_t>( TYPE::HSTRING ) ) {
			val = lexical_cast<int long long>( _string );
		} else if ( _type & static_cast<type_mask_t>( TYPE::HTIME ) ) {
			val = _time.raw();
		} else {
			M_ASSERT( ! _integer && ( _type == static_cast<type_mask_t>( TYPE::UNKNOWN ) ) );
		}
	}
	return ( val );
	M_EPILOG
}

double long HInfoMultiVal::do_get_real( void ) const {
	M_PROLOG
	double long val( _real );
	if ( ! ( _type & static_cast<type_mask_t>( TYPE::DOUBLE_LONG ) ) ) {
		if ( _type & static_cast<type_mask_t>( TYPE::INT_LONG_LONG ) ) {
			val = _integer;
		} else if ( _type & static_cast<type_mask_t>( TYPE::HSTRING ) ) {
			val = lexical_cast<double long>( _string );
		} else if ( _type & static_cast<type_mask_t>( TYPE::HTIME ) ) {
			val = _time.raw();
		} else {
			M_ASSERT( ( _real == 0. ) && ( _type == static_cast<type_mask_t>( TYPE::UNKNOWN ) ) );
		}
	}
	return ( val );
	M_EPILOG
}

HString const& HInfoMultiVal::do_get_string( void ) const {
	M_PROLOG
	if ( ! ( _type & static_cast<type_mask_t>( TYPE::HSTRING ) ) ) {
		if ( _type & static_cast<type_mask_t>( TYPE::DOUBLE_LONG ) ) {
			_string = _real;
		} else if ( _type & static_cast<type_mask_t>( TYPE::INT_LONG_LONG ) ) {
			_string = _integer;
		} else if ( _type & static_cast<type_mask_t>( TYPE::HTIME ) ) {
			_string = _time.string();
		} else {
			M_ASSERT( _string.is_empty() && ( _type == static_cast<type_mask_t>( TYPE::UNKNOWN ) ) );
		}
	}
	return ( _string );
	M_EPILOG
}

HTime const& HInfoMultiVal::do_get_time( void ) const {
	M_PROLOG
	return ( _time );
	M_EPILOG
}

void HInfoMultiVal::do_set_integer( int long long data_ ) {
	_integer = data_;
	_type |= static_cast<type_mask_t>( TYPE::INT_LONG_LONG );
	return;
}

void HInfoMultiVal::do_set_real( double long data_ ) {
	_real = data_;
	_type |= static_cast<type_mask_t>( TYPE::DOUBLE_LONG );
	return;
}

void HInfoMultiVal::do_set_string( HString const& string_ ) {
	_string = string_;
	_type |= static_cast<type_mask_t>( TYPE::HSTRING );
	return;
}

void HInfoMultiVal::do_set_time( HTime const& time_ ) {
	_time = time_;
	_type |= static_cast<type_mask_t>( TYPE::HTIME );
	return;
}

}

}

