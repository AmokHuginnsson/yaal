/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

  hcore/htaggedpod.hxx - this file is integral part of `yaal' project.

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
/*! \file hcore/htaggedpod.hxx
 * \brief Declaration and implementation of HTaggedPOD template class.
 */

#ifndef YAAL_HCORE_HTAGGEDPOD_HXX_INCLUDED
#define YAAL_HCORE_HTAGGEDPOD_HXX_INCLUDED 1

#include "hcore/macro.hxx"
#include "hcore/algorithm_low.hxx"

namespace yaal {

namespace hcore {

/*! \brief Type safe PODs.
 *
 * \tparam pod_t - internal POD type.
 * \tparam TAG - type tag.
 *
 * HTaggedPOD<> allows easy creation of distinguishable and type safe
 * pod look-alikes.
 */
template<typename pod_t, typename TAG>
class HTaggedPOD {
public:
	typedef HTaggedPOD<pod_t, TAG> this_type;
	typedef pod_t value_type;
private:
	pod_t _value;
public:
	HTaggedPOD( void )
		: _value() {
		return;
	}
	explicit HTaggedPOD( pod_t const& pod_ )
		: _value( pod_ ) {
		return;
	}
	HTaggedPOD( HTaggedPOD const& tp_ )
		: _value( tp_._value ) {
		return;
	}
	virtual ~HTaggedPOD( void ) {
		return;
	}
	HTaggedPOD& operator = ( HTaggedPOD const& tp_ ) {
		if ( &tp_ != this ) {
			_value = tp_._value;
		}
		return ( *this );
	}
	void swap( HTaggedPOD& tp_ ) {
		if ( &tp_ != this ) {
			using yaal::swap;
			swap( _value, tp_._value );
		}
		return;
	}
	pod_t const& raw( void ) const {
		return ( _value );
	}
	pod_t const& get( void ) const {
		return ( _value );
	}
	pod_t& ref( void ) {
		return ( _value );
	}
#define YAAL_DEFINE_OPER( OP ) \
	HTaggedPOD& operator OP ( HTaggedPOD const& tp_ ) { \
		_value OP tp_._value; \
		return ( *this ); \
	} \
	HTaggedPOD& operator OP ( pod_t const& value_ ) { \
		_value OP value_; \
		return ( *this ); \
	}
YAAL_DEFINE_OPER( += )
YAAL_DEFINE_OPER( -= )
YAAL_DEFINE_OPER( *= )
YAAL_DEFINE_OPER( /= )
YAAL_DEFINE_OPER( %= )
YAAL_DEFINE_OPER( ^= )
YAAL_DEFINE_OPER( &= )
YAAL_DEFINE_OPER( |= )
#undef YAAL_DEFINE_OPER
#define YAAL_DEFINE_OPER( OP ) \
	HTaggedPOD& operator OP ( void ) { \
		OP _value; \
		return ( *this ); \
	} \
	HTaggedPOD operator OP ( int ) const { \
		HTaggedPOD tp( *this ); \
		OP tp; \
		return ( tp ); \
	}
YAAL_DEFINE_OPER( ++ )
YAAL_DEFINE_OPER( -- )
#undef YAAL_DEFINE_OPER
#define YAAL_DEFINE_OPER( OP ) \
	HTaggedPOD operator OP ( HTaggedPOD const& tp_ ) const { \
		return ( HTaggedPOD( _value OP tp_._value ) ); \
	} \
	HTaggedPOD operator OP ( pod_t const& value_ ) const { \
		return ( HTaggedPOD( _value OP value_ ) ); \
	} \
	friend HTaggedPOD operator OP ( pod_t const& pod_, HTaggedPOD const& tp_ ) { \
		return ( HTaggedPOD( pod_ OP tp_._value ) ); \
	}
YAAL_DEFINE_OPER( + )
YAAL_DEFINE_OPER( - )
YAAL_DEFINE_OPER( * )
YAAL_DEFINE_OPER( / )
YAAL_DEFINE_OPER( % )
YAAL_DEFINE_OPER( ^ )
YAAL_DEFINE_OPER( & )
YAAL_DEFINE_OPER( | )
#undef YAAL_DEFINE_OPER
#define YAAL_DEFINE_OPER( OP ) \
	bool operator OP ( HTaggedPOD const& tp_ ) const { \
		return ( _value OP tp_._value ); \
	} \
	bool operator OP ( pod_t const& value_ ) const { \
		return ( _value OP value_ ); \
	} \
	friend bool operator OP ( pod_t const& pod_, HTaggedPOD const& tp_ ) { \
		return ( pod_ OP tp_._value ); \
	}
YAAL_DEFINE_OPER( == )
YAAL_DEFINE_OPER( != )
YAAL_DEFINE_OPER( <= )
YAAL_DEFINE_OPER( < )
YAAL_DEFINE_OPER( >= )
YAAL_DEFINE_OPER( > )
#pragma GCC diagnostic ignored "-Weffc++"
YAAL_DEFINE_OPER( && )
YAAL_DEFINE_OPER( || )
#pragma GCC diagnostic error "-Weffc++"
#undef YAAL_DEFINE_OPER
#define YAAL_DEFINE_OPER( OP ) \
	HTaggedPOD operator OP ( void ) const { \
		return ( HTaggedPOD( OP _value ) ); \
	}
YAAL_DEFINE_OPER( + )
YAAL_DEFINE_OPER( - )
YAAL_DEFINE_OPER( ~ )
#undef YAAL_DEFINE_OPER
	bool operator ! ( void ) const {
		return ( ! _value );
	}
#define YAAL_DEFINE_OPER( OP ) \
	HTaggedPOD& operator OP ( int shift_ ) { \
		_value OP shift_; \
		return ( *this ); \
	}
YAAL_DEFINE_OPER( <<= )
YAAL_DEFINE_OPER( >>= )
#undef YAAL_DEFINE_OPER
#define YAAL_DEFINE_OPER( OP ) \
	HTaggedPOD operator OP ( int shift_ ) const { \
		return ( HTaggedPOD( _value OP shift_ ) ); \
	}
YAAL_DEFINE_OPER( << )
YAAL_DEFINE_OPER( >> )
#undef YAAL_DEFINE_OPER
};

template<typename pod_t, typename TAG>
inline void swap( HTaggedPOD<pod_t, TAG>& a, HTaggedPOD<pod_t, TAG>& b ) {
	a.swap( b );
}

}

}

#endif /* #ifndef YAAL_HCORE_HTAGGEDPOD_HXX_INCLUDED */

