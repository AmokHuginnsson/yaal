/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hcore/assert.hxx - this file is integral part of `yaal' project.

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
/*! \file hcore/assert.hxx
 * \brief Declarations of assertion tesing helpers: HFailedAssertion, failed_assert.
 */

#ifndef YAAL_HCORE_ASSERT_HXX_INCLUDED
#define YAAL_HCORE_ASSERT_HXX_INCLUDED 1

namespace yaal {

namespace hcore {

/*! \brief Failed assertion exception.
 *
 * In \e DEBUG build failuers in assertions does not abort the
 * process, insead the throw instance of HFailedAssertion.
 */
class HFailedAssertion {
	char const* _what;
public:
	HFailedAssertion( char const* const what_ ) : _what( what_ ) {}
	HFailedAssertion( HFailedAssertion const& fa ) : _what( fa._what ) {}
	HFailedAssertion& operator = ( HFailedAssertion const& fa ) {
		if ( &fa != this ) {
			HFailedAssertion n( fa );
			swap( n );
		}
		return ( *this );
	}
	char const* what( void ) const { return ( _what ); }
private:
	void swap( HFailedAssertion& );
};

void failed_assert( char const* const, int, char const* const, char const* const ) __attribute__(( __noreturn__ ));

}

}

#endif /* #ifndef YAAL_HCORE_ASSERT_HXX_INCLUDED */

