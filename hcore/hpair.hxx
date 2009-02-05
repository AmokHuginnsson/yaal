/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hpair.hxx - this file is integral part of `yaal' project.

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

#ifndef YAAL_HCORE_HPAIR_HXX_INCLUDED
#define YAAL_HCORE_HPAIR_HXX_INCLUDED

#line 31

#define D_VCSID_YAAL_HCORE_HPAIR_H "$Id$"

namespace yaal
{

namespace hcore
{

/*! \brief Implementation of ordered pair concept.
 *
 * \tparam first_t - type of first pair element.
 * \tparam second_t - type of second pair element.
 */
template<typename first_t, typename second_t>
class HPair
	{
public:
	first_t first;
	second_t second;
	HPair( void ) : first(), second() {}
	HPair( first_t const& key, second_t const& value ) : first( key ), second( value ) {}
	HPair( HPair const& pair ) : first( pair.first ), second( pair.second ) {}
	template<typename alt_first_t, typename alt_second_t>
	HPair( HPair<alt_first_t, alt_second_t> const& p ) : first( p.first ), second( p.second ) {}
	HPair& operator = ( HPair const& pair )
		{
		M_PROLOG
		if ( &pair != this )
			{
			first = pair.first;
			second = pair.second;
			}
		return ( *this );
		M_EPILOG
		}
	bool operator == ( HPair const& pair ) const
		{	return ( ( first == pair.first ) && ( second == pair.second ) );	}
	bool operator != ( HPair const& pair ) const
		{	return ( ! operator == ( pair ) );	}
	bool operator < ( HPair const& pair ) const
		{
		return ( ( first < pair.first )
				|| ( ! ( pair.first < first ) && ( second < pair.second ) ) );
		}
	};

template<typename first_t, typename second_t>
HPair<first_t, second_t> make_pair( first_t const& first, second_t const& second )
	{ return ( HPair<first_t, second_t>( first, second ) ); }

}

}

#endif /* not YAAL_HCORE_HPAIR_HXX_INCLUDED */

