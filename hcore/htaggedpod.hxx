/* Read yaal/LICENSE.md file for copyright and licensing information. */
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
class HTaggedPOD final {
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
	explicit constexpr HTaggedPOD( pod_t const& pod_ )
		: _value( pod_ ) {
	}
	constexpr HTaggedPOD( HTaggedPOD const& ) = default;
	constexpr HTaggedPOD( HTaggedPOD&& other_ ) noexcept
		: _value( yaal::move( other_._value ) ) {
	}
	HTaggedPOD& operator = ( HTaggedPOD const& ) = default;
	HTaggedPOD& operator = ( HTaggedPOD&& ) noexcept = default;
	void swap( HTaggedPOD& tp_ ) {
		using yaal::swap;
		swap( _value, tp_._value );
		return;
	}
	constexpr pod_t const& raw( void ) const {
		return ( _value );
	}
	constexpr pod_t const& get( void ) const {
		return ( _value );
	}
	constexpr pod_t& ref( void ) {
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
		return tp; \
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

