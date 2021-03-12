/* Read yaal/LICENSE.md file for copyright and licensing information. */
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
		{ return a0_; }
	template <typename a0_t>
	a0_t& operator()( a0_t& a0_ )
		{ return a0_; }
};

template<>
struct placeholder<2> : public placeholder_base {
	placeholder( void ) {}
	template <typename a0_t, typename a1_t>
	a1_t const& operator()( a0_t const&, a1_t const& a1_ ) const
		{ return a1_; }
	template <typename a0_t, typename a1_t>
	a1_t& operator()( a0_t&, a1_t& a1_ )
		{ return a1_; }
};

template<>
struct placeholder<3> : public placeholder_base {
	placeholder( void ) {}
	template <typename a0_t, typename a1_t, typename a2_t>
	a2_t const& operator()( a0_t const&, a1_t const&, a2_t const& a2_ ) const
		{ return a2_; }
	template <typename a0_t, typename a1_t, typename a2_t>
	a2_t& operator()( a0_t&, a1_t&, a2_t& a2_ )
		{ return a2_; }
};

}

namespace {

static higher_order::placeholder<1> const _1; /*!< Place holder for first anonymous argument. */
static higher_order::placeholder<2> const _2; /*!< Place holder for second anonymous argument. */
static higher_order::placeholder<3> const _3; /*!< Place holder for third anonymous argument. */
static higher_order::placeholder<4> const _4; /*!< Place holder for fourth anonymous argument. */
static higher_order::placeholder<5> const _5; /*!< Place holder for fifth anonymous argument. */
static higher_order::placeholder<6> const _6; /*!< Place holder for sixth anonymous argument. */
static higher_order::placeholder<7> const _7; /*!< Place holder for seventh anonymous argument. */
static higher_order::placeholder<8> const _8; /*!< Place holder for eighth anonymous argument. */
static higher_order::placeholder<9> const _9; /*!< Place holder for ninth anonymous argument. */
static higher_order::placeholder<10> const _10; /*!< Place holder for tenth anonymous argument. */
static higher_order::placeholder<11> const _11; /*!< Place holder for eleventh anonymous argument. */

}

}

}

#endif /* #ifndef YAAL_HCORE_PLACEHOLDER_HXX_INCLUDED */

