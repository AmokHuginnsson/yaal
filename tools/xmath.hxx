/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	xmath.hxx - this file is integral part of `yaal' project.

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

#ifndef YAAL_TOOLS_XMATH_HXX_INCLUDED
#define YAAL_TOOLS_XMATH_HXX_INCLUDED

#line 31

#define D_VCSID_XMATH_H "$Id$"

namespace yaal
{

namespace tools
{

/*! \brief Meta-function (template) implementation of various math functions.
 */
namespace xmath
{

template<typename number_t>
number_t factorial( number_t a_iNumber )
	{
	number_t n = 1;
	for ( number_t i = 2; i <= a_iNumber; ++ i )
		n *= i;
	return ( n );
	}

template<typename number_t>
number_t binomial_coefficient( number_t a_iCardinal, number_t a_iSubCardinal )
	{
	return ( factorial( a_iCardinal ) / ( factorial( a_iSubCardinal ) * factorial( a_iCardinal - a_iSubCardinal ) ) );
	}

}

}

}


#endif /* not YAAL_TOOLS_XMATH_HXX_INCLUDED */

