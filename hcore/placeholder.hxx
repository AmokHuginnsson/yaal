/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	placeholder.hxx - this file is integral part of `yaal' project.

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
/*! \file hcore/placeholder.hxx
 * \brief Definition of placeholders used for call abstractions and lambda expressions.
 */

#ifndef YAAL_HCORE_PLACEHOLDER_HXX_INCLUDED
#define YAAL_HCORE_PLACEHOLDER_HXX_INCLUDED 1

namespace yaal {

namespace hcore {

namespace higher_order {

struct placeholder_base {
	virtual ~placeholder_base( void ) {}
};

template<int arg_no>
struct placeholder : public placeholder_base {
	placeholder( void ) {}
};

template<>
struct placeholder<1> : public placeholder_base {
	placeholder( void ) {}
	template <typename a0_t>
	a0_t const& operator()( a0_t const& a0_ ) const
		{ return ( a0_ ); }
	template <typename a0_t>
	a0_t& operator()( a0_t& a0_ )
		{ return ( a0_ ); }
};

template<>
struct placeholder<2> : public placeholder_base {
	placeholder( void ) {}
	template <typename a0_t, typename a1_t>
	a1_t const& operator()( a0_t const& a0_, a1_t const& a1_ ) const
		{ return ( a1_ ); }
	template <typename a0_t, typename a1_t>
	a1_t& operator()( a0_t& a0_, a1_t& a1_ )
		{ return ( a1_ ); }
};

template<>
struct placeholder<3> : public placeholder_base {
	placeholder( void ) {}
	template <typename a0_t, typename a1_t, typename a2_t>
	a2_t const& operator()( a0_t const& a0_, a1_t const& a1_, a2_t const& a2_ ) const
		{ return ( a2_ ); }
	template <typename a0_t, typename a1_t, typename a2_t>
	a2_t& operator()( a0_t& a0_, a1_t& a1_, a2_t& a2_ )
		{ return ( a2_ ); }
};

}

namespace {

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

