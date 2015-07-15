/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

  hbitflag.hxx - this file is integral part of `yaal' project.

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
/*! \file hcore/hbitflag.hxx
 * \brief Declaration and implementation of HBitFlag class.
 */

#ifndef YAAL_HCORE_HBITFLAG_HXX_INCLUDED
#define YAAL_HCORE_HBITFLAG_HXX_INCLUDED 1

#include "hcore/hexception.hxx"
#include "hcore/pod.hxx"
#include "hcore/bit.hxx"

namespace yaal {

namespace hcore {

template<typename TAG>
class HBitFlag final {
	class Unspecified {
		Unspecified& unspecified( Unspecified& ) { return ( *this ); }
		friend class HBitFlag;
	};
	typedef Unspecified& ( Unspecified::* unspecified_t )( Unspecified& );
public:
	typedef HBitFlag<TAG> this_type;
private:
	static u64_t _nextFlag;
	u64_t _flag;
public:
	HBitFlag( HBitFlag const& bf_ )
		: _flag( bf_._flag ) {
	}
#define YAAL_DEFINE_OPER( OP ) \
	HBitFlag& operator OP ( HBitFlag const& bf_ ) { \
		_flag OP bf_._flag; \
		return ( *this ); \
	}
YAAL_DEFINE_OPER( |= )
YAAL_DEFINE_OPER( &= )
YAAL_DEFINE_OPER( ^= )
#undef YAAL_DEFINE_OPER
#define YAAL_DEFINE_OPER( OP ) \
	HBitFlag operator OP ( HBitFlag const& bf_ ) const { \
		HBitFlag bf( *this ); \
		bf OP##= bf_; \
		return ( bf ); \
	}
YAAL_DEFINE_OPER( | )
YAAL_DEFINE_OPER( & )
YAAL_DEFINE_OPER( ^ )
#undef YAAL_DEFINE_OPER
	HBitFlag operator ~ ( void ) const {
		HBitFlag bf( *this );
		bf._flag = ~bf._flag;
		return ( bf );
	}
	bool operator == ( HBitFlag const& bf_ ) const {
		return ( _flag == bf_._flag );
	}
	bool operator != ( HBitFlag const& bf_ ) const {
		return ( _flag != bf_._flag );
	}
	operator unspecified_t ( void ) const {
		return ( _flag != 0 ? &Unspecified::unspecified : NULL );
	}
	static HBitFlag new_flag( void ) {
		return ( HBitFlag() );
	}
	u64_t value( void ) const {
		return ( _flag );
	}
	/*! \brief Get flag index.
	 *
	 * WARNING: This function does not return bit index!!
	 *
	 * \return Flag index.
	 */
	int index( void ) const {
		M_ASSERT( bit::count( _flag ) <= 1 );
		return ( bit::least_significant( _flag ) + 1 );
	}
private:
	HBitFlag( void )
		: _flag( _nextFlag ) {
		M_ASSERT( _nextFlag != ( 1ull << 63 ) );
		if ( ! _nextFlag ) {
			_nextFlag = 1;
		} else {
			_nextFlag <<= 1;
		}
	}
};

template<typename TAG>
u64_t HBitFlag<TAG>::_nextFlag = 0;

}

}

#endif /* #ifndef YAAL_HCORE_HBITFLAG_HXX_INCLUDED */

