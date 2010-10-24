/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	placeholder.hxx - this file is integral part of `yaal' project.

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
/*! \file hcore/placeholder.hxx
 * \brief Definition of placeholders used for call abstractions and lambda expressions.
 */

#ifndef YAAL_HCORE_PLACEHOLDER_HXX_INCLUDED
#define YAAL_HCORE_PLACEHOLDER_HXX_INCLUDED 1

namespace yaal
{

namespace hcore
{

namespace higher_order
{

struct placeholder_base
	{
	virtual ~placeholder_base( void ) {}
	};

template<int arg_no>
struct placeholder : public placeholder_base
	{
	placeholder( void ) {}
	};

}

namespace
{

static const higher_order::placeholder<1> _1; /*!< Place holder for first anonymous argument. */
static const higher_order::placeholder<2> _2; /*!< Place holder for second anonymous argument. */
static const higher_order::placeholder<3> _3; /*!< Place holder for third anonymous argument. */
static const higher_order::placeholder<4> _4; /*!< Place holder for fourth anonymous argument. */
static const higher_order::placeholder<5> _5; /*!< Place holder for fifth anonymous argument. */
static const higher_order::placeholder<6> _6; /*!< Place holder for sixth anonymous argument. */
static const higher_order::placeholder<7> _7; /*!< Place holder for seventh anonymous argument. */
static const higher_order::placeholder<8> _8; /*!< Place holder for eighth anonymous argument. */
static const higher_order::placeholder<9> _9; /*!< Place holder for ninth anonymous argument. */
static const higher_order::placeholder<10> _10; /*!< Place holder for tenth anonymous argument. */
static const higher_order::placeholder<11> _11; /*!< Place holder for eleventh anonymous argument. */

}

}

}

#endif /* #ifndef YAAL_HCORE_PLACEHOLDER_HXX_INCLUDED */

