/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	lambda.hxx - this file is integral part of `yaal' project.

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
/*! \file tools/lambda.hxx
 * \brief Declaration of lambda language extension.
 */

#ifndef YAAL_TOOLS_LAMBDA_HXX_INCLUDED
#define YAAL_TOOLS_LAMBDA_HXX_INCLUDED 1

#include "hcore/placeholder.hxx"
#include "hcore/hstreaminterface.hxx"
#include "tools/hstringstream.hxx"
#include "hcore/memory.hxx"

namespace yaal {

namespace tools {

template<int no>
inline yaal::hcore::higher_order::placeholder<no> operator *( yaal::hcore::higher_order::placeholder<no> arg_ ) {
	return ( arg_ );
}

template<typename T1, typename T2>
struct promote {
	typedef typename trait::ternary<
			is_integral<T1>::value != is_integral<T2>::value,
			typename trait::ternary<is_floating_point<T1>::value, T1, T2>::type,
			typename trait::ternary<sizeof ( T1 ) >= sizeof (T2 ), T1, T2>::type
		>::type type;
};

template<typename T1, typename T2>
struct return_type_binary_arithmetic {
	static bool const both_numeric = ( is_numeric<T1>::value && is_numeric<T2>::value );
	typedef typename trait::ternary<trait::same_type<T1, T2>::value, T1, typename trait::ternary<both_numeric, typename promote<T1, T2>::type, trait::no_type>::type>::type type;
};

struct LAMBDA_TYPE {
	typedef enum {
		RETURN,
		VALUE,
		PRE_INCREMENT,
		POST_INCREMENT,
		PRE_DECREMENT,
		POST_DECREMENT,
		PLUS,
		MINUS,
		MULTIPLIES,
		DIVIDES,
		NEGATE,
		BOOLEAN_NOT,
		BOOLEAN_AND,
		BOOLEAN_OR,
		BIT_NOT,
		BIT_AND,
		BIT_OR,
		BIR_XOR,
		COMA,
		STREAM
	} type_t;
};

template<typename R, typename L>
class HLambdaReturn {
	L _lambda;
public:
	HLambdaReturn( L lambda_ ) : _lambda( lambda_ ) {}
	template<typename T1>
	R operator()( T1 const& arg1_ )
		{ return ( _lambda.operator()<R>( arg1_ ) ); }
	template<typename T1, typename T2>
	R operator()( T1 const& arg1_, T2 const& arg2_ )
		{ return ( _lambda.operator()<R>( arg1_, arg2_ ) ); }
	template<typename T1, typename T2, typename T3>
	R operator()( T1 const& arg1_, T2 const& arg2_, T3 const& arg3_ )
		{ return ( _lambda.operator()<R>( arg1_, arg2_, arg3_ ) ); }
	template<typename T1, typename T2, typename T3, typename T4>
	R operator()( T1 const& arg1_, T2 const& arg2_, T3 const& arg3_, T4 const& arg4_ )
		{ return ( _lambda.operator()<R>( arg1_, arg2_, arg3_, arg4_ ) ); }
	template<typename T1, typename T2, typename T3, typename T4, typename T5>
	R operator()( T1 const& arg1_, T2 const& arg2_, T3 const& arg3_, T4 const& arg4_, T5 const& arg5_ )
		{ return ( _lambda.operator()<R>( arg1_, arg2_, arg3_, arg4_, arg5_ ) ); }
};

template<typename T, typename L>
HLambdaReturn<T, L> ret( L lambda_ ) {
	return ( HLambdaReturn<T, L>( lambda_ ) );
}

template<LAMBDA_TYPE::type_t const lambda_type, typename first_lambda = trait::no_type, typename second_lambda = trait::no_type>
class HLambda;

template<>
class HLambda<LAMBDA_TYPE::VALUE, yaal::hcore::higher_order::placeholder<1> > {
public:
	template<typename a0_t>
	a0_t const& operator()( a0_t const& a0_ ) const
		{ return ( a0_ ); }
	template<typename a0_t>
	a0_t& operator()( a0_t& a0_ ) const
		{ return ( a0_ ); }
	template<typename a0_t, typename a1_t>
	a0_t const& operator()( a0_t const& a0_, a1_t const& ) const
		{ return ( a0_ ); }
	template<typename a0_t, typename a1_t>
	a0_t& operator()( a0_t& a0_, a1_t const& ) const
		{ return ( a0_ ); }
	template<typename a0_t, typename a1_t, typename a2_t>
	a0_t const& operator()( a0_t const& a0_, a1_t const&, a2_t const& ) const
		{ return ( a0_ ); }
	template<typename a0_t, typename a1_t, typename a2_t>
	a0_t& operator()( a0_t& a0_, a1_t const&, a2_t const& ) const
		{ return ( a0_ ); }
};

template<>
class HLambda<LAMBDA_TYPE::VALUE, yaal::hcore::higher_order::placeholder<2> > {
public:
	template<typename a0_t, typename a1_t>
	a1_t const& operator()( a0_t const&, a1_t const& a1_ ) const
		{ return ( a1_ ); }
	template<typename a0_t, typename a1_t>
	a1_t& operator()( a0_t const&, a1_t& a1_ ) const
		{ return ( a1_ ); }
	template<typename a0_t, typename a1_t, typename a2_t>
	a1_t const& operator()( a0_t const&, a1_t const& a1_, a2_t const& ) const
		{ return ( a1_ ); }
	template<typename a0_t, typename a1_t, typename a2_t>
	a1_t& operator()( a0_t const&, a1_t& a1_, a2_t const& ) const
		{ return ( a1_ ); }
};

template<>
class HLambda<LAMBDA_TYPE::VALUE, yaal::hcore::higher_order::placeholder<3> > {
public:
	template<typename a0_t, typename a1_t, typename a2_t>
	a2_t const& operator()( a0_t const&, a1_t const&, a2_t const& a2_ ) const
		{ return ( a2_ ); }
	template<typename a0_t, typename a1_t, typename a2_t>
	a2_t& operator()( a0_t const&, a1_t const&, a2_t& a2_ ) const
		{ return ( a2_ ); }
};

template<typename first_lambda, typename second_lambda>
class HLambda<LAMBDA_TYPE::COMA, first_lambda, second_lambda> {
public:
	template<typename T>
	T& operator()( T& val_ ) const {
		first_lambda fl;
		second_lambda sl;
		fl( val_ );
		return ( sl( val_ ) );
	}
};

#ifdef __GNUC__
#if ( 100 * __GNUC__ + __GNUC_MINOR__ ) <= 402
#pragma GCC diagnostic warning "-Weffc++"
#endif /* #if ( 100 * __GNUC__ + __GNUC_MINOR__ ) <= 402 */
#endif /* #ifdef __GNUC__ */

template<LAMBDA_TYPE::type_t const first_lambda_type,
	typename first_lambda_first_type, typename first_lambda_second_type,
	LAMBDA_TYPE::type_t const second_lambda_type,
	typename second_lambda_first_type, typename second_lambda_second_type>
HLambda<LAMBDA_TYPE::COMA,
	HLambda<first_lambda_type, first_lambda_first_type, first_lambda_second_type>,
	HLambda<second_lambda_type, second_lambda_first_type, second_lambda_second_type> >
operator , ( HLambda<first_lambda_type, first_lambda_first_type, first_lambda_second_type> const&,
		HLambda<second_lambda_type, second_lambda_first_type, second_lambda_second_type> const& )
	{ return ( HLambda<LAMBDA_TYPE::COMA,
			HLambda<first_lambda_type, first_lambda_first_type, first_lambda_second_type>,
			HLambda<second_lambda_type, second_lambda_first_type, second_lambda_second_type> >() ); }

template<>
class HLambda<LAMBDA_TYPE::PRE_INCREMENT> {
public:
	HLambda()
		{}
	template<int const N>
	HLambda( yaal::hcore::higher_order::placeholder<N> const& )
		{}
	template<typename T>
	T& operator()( T& val_ ) const {
		return ( ++ val_ );
	}
};

template<>
class HLambda<LAMBDA_TYPE::POST_INCREMENT> {
public:
	HLambda()
		{}
	template<int const N>
	HLambda( yaal::hcore::higher_order::placeholder<N> const& )
		{}
	template<typename T>
	T operator()( T& val_ ) const {
		return ( val_ ++ );
	}
};

template<>
class HLambda<LAMBDA_TYPE::PRE_DECREMENT> {
public:
	HLambda()
		{}
	template<int const N>
	HLambda( yaal::hcore::higher_order::placeholder<N> const& )
		{}
	template<typename T>
	T& operator()( T& val_ ) const {
		return ( -- val_ );
	}
};

template<>
class HLambda<LAMBDA_TYPE::POST_DECREMENT> {
public:
	HLambda()
		{}
	template<int const N>
	HLambda( yaal::hcore::higher_order::placeholder<N> const& )
		{}
	template<typename T>
	T operator()( T& val_ ) const {
		return ( val_ -- );
	}
};

template<typename first_lambda, typename second_lambda>
class HLambda<LAMBDA_TYPE::PLUS, first_lambda, second_lambda> {
public:
	template<typename R, typename T1, typename T2>
	R operator()( T1 const& val1_, T2 const& val2_ ) {
		first_lambda fl;
		second_lambda sl;
		return ( static_cast<R>( fl( val1_, val2_ ) + sl( val1_, val2_ ) ) );
	}
	template<typename R, typename T1, typename T2, typename T3>
	R operator()( T1 const& val1_, T2 const& val2_, T3 const& val3_ ) {
		first_lambda fl;
		second_lambda sl;
		return ( static_cast<R>( fl( val1_, val2_, val3_ ) + sl( val1_, val2_, val3_ ) ) );
	}
	template<typename T1, typename T2>
	typename return_type_binary_arithmetic<T1, T2>::type operator()( T1 const& val1_, T2 const& val2_ ) {
		first_lambda fl;
		second_lambda sl;
		return ( fl( val1_, val2_ ) + sl( val1_, val2_ ) );
	}
	template<typename T1, typename T2, typename T3>
	typename return_type_binary_arithmetic<typename return_type_binary_arithmetic<T1, T2>::type, T3>::type operator()( T1 const& val1_, T2 const& val2_, T3 const& val3_ ) {
		first_lambda fl;
		second_lambda sl;
		return ( fl( val1_, val2_, val3_ ) + sl( val1_, val2_, val3_ ) );
	}
};

template<typename first_lambda, typename second_lambda>
class HLambda<LAMBDA_TYPE::MINUS, first_lambda, second_lambda> {
public:
	template<typename R, typename T1, typename T2>
	R operator()( T1 const& val1_, T2 const& val2_ ) {
		first_lambda fl;
		second_lambda sl;
		return ( static_cast<R>( fl( val1_, val2_ ) - sl( val1_, val2_ ) ) );
	}
	template<typename R, typename T1, typename T2, typename T3>
	R operator()( T1 const& val1_, T2 const& val2_, T3 const& val3_ ) {
		first_lambda fl;
		second_lambda sl;
		return ( static_cast<R>( fl( val1_, val2_, val3_ ) - sl( val1_, val2_, val3_ ) ) );
	}
	template<typename T1, typename T2>
	typename return_type_binary_arithmetic<T1, T2>::type operator()( T1 const& val1_, T2 const& val2_ ) {
		first_lambda fl;
		second_lambda sl;
		return ( fl( val1_, val2_ ) - sl( val1_, val2_ ) );
	}
	template<typename T1, typename T2, typename T3>
	typename return_type_binary_arithmetic<typename return_type_binary_arithmetic<T1, T2>::type, T3>::type operator()( T1 const& val1_, T2 const& val2_, T3 const& val3_ ) {
		first_lambda fl;
		second_lambda sl;
		return ( fl( val1_, val2_, val3_ ) - sl( val1_, val2_, val3_ ) );
	}
};

template<typename first_lambda, typename second_lambda>
class HLambda<LAMBDA_TYPE::MULTIPLIES, first_lambda, second_lambda> {
public:
	template<typename R, typename T1, typename T2>
	R operator()( T1 const& val1_, T2 const& val2_ ) {
		first_lambda fl;
		second_lambda sl;
		return ( static_cast<R>( fl( val1_, val2_ ) * sl( val1_, val2_ ) ) );
	}
	template<typename R, typename T1, typename T2, typename T3>
	R operator()( T1 const& val1_, T2 const& val2_, T3 const& val3_ ) {
		first_lambda fl;
		second_lambda sl;
		return ( static_cast<R>( fl( val1_, val2_, val3_ ) * sl( val1_, val2_, val3_ ) ) );
	}
	template<typename T1, typename T2>
	typename return_type_binary_arithmetic<T1, T2>::type operator()( T1 const& val1_, T2 const& val2_ ) {
		first_lambda fl;
		second_lambda sl;
		return ( fl( val1_, val2_ ) * sl( val1_, val2_ ) );
	}
	template<typename T1, typename T2, typename T3>
	typename return_type_binary_arithmetic<typename return_type_binary_arithmetic<T1, T2>::type, T3>::type operator()( T1 const& val1_, T2 const& val2_, T3 const& val3_ ) {
		first_lambda fl;
		second_lambda sl;
		return ( fl( val1_, val2_, val3_ ) * sl( val1_, val2_, val3_ ) );
	}
};

template<typename first_lambda, typename second_lambda>
class HLambda<LAMBDA_TYPE::DIVIDES, first_lambda, second_lambda> {
public:
	template<typename R, typename T1, typename T2>
	R operator()( T1 const& val1_, T2 const& val2_ ) {
		first_lambda fl;
		second_lambda sl;
		return ( static_cast<R>( fl( val1_, val2_ ) / sl( val1_, val2_ ) ) );
	}
	template<typename R, typename T1, typename T2, typename T3>
	R operator()( T1 const& val1_, T2 const& val2_, T3 const& val3_ ) {
		first_lambda fl;
		second_lambda sl;
		return ( static_cast<R>( fl( val1_, val2_, val3_ ) / sl( val1_, val2_, val3_ ) ) );
	}
	template<typename T1, typename T2>
	typename return_type_binary_arithmetic<T1, T2>::type operator()( T1 const& val1_, T2 const& val2_ ) {
		first_lambda fl;
		second_lambda sl;
		return ( fl( val1_, val2_ ) / sl( val1_, val2_ ) );
	}
	template<typename T1, typename T2, typename T3>
	typename return_type_binary_arithmetic<typename return_type_binary_arithmetic<T1, T2>::type, T3>::type operator()( T1 const& val1_, T2 const& val2_, T3 const& val3_ ) {
		first_lambda fl;
		second_lambda sl;
		return ( fl( val1_, val2_, val3_ ) / sl( val1_, val2_, val3_ ) );
	}
};

inline HLambda<LAMBDA_TYPE::PRE_INCREMENT> operator ++ ( HLambda<LAMBDA_TYPE::PRE_INCREMENT> ) {
	return ( HLambda<LAMBDA_TYPE::PRE_INCREMENT>() );
}

inline HLambda<LAMBDA_TYPE::POST_INCREMENT> operator ++ ( HLambda<LAMBDA_TYPE::POST_INCREMENT>, int ) {
	return ( HLambda<LAMBDA_TYPE::POST_INCREMENT>() );
}

inline HLambda<LAMBDA_TYPE::PRE_DECREMENT> operator -- ( HLambda<LAMBDA_TYPE::PRE_DECREMENT> ) {
	return ( HLambda<LAMBDA_TYPE::PRE_DECREMENT>() );
}

inline HLambda<LAMBDA_TYPE::POST_DECREMENT> operator -- ( HLambda<LAMBDA_TYPE::POST_DECREMENT>, int ) {
	return ( HLambda<LAMBDA_TYPE::POST_DECREMENT>() );
}

/***********************************************************************/
/* Lambda Plus */

template<int const first, int const second>
inline HLambda<LAMBDA_TYPE::PLUS,
			 HLambda<LAMBDA_TYPE::VALUE, yaal::hcore::higher_order::placeholder<first> >,
			 HLambda<LAMBDA_TYPE::VALUE, yaal::hcore::higher_order::placeholder<second> > >
			 operator + ( yaal::hcore::higher_order::placeholder<first>, yaal::hcore::higher_order::placeholder<second> ) {
	return ( HLambda<LAMBDA_TYPE::PLUS,
			HLambda<LAMBDA_TYPE::VALUE, yaal::hcore::higher_order::placeholder<first> >,
			HLambda<LAMBDA_TYPE::VALUE, yaal::hcore::higher_order::placeholder<second> > >() );
}

template<LAMBDA_TYPE::type_t const first_lambda_type,
	typename first_lambda_first_type, typename first_lambda_second_type, int second>
HLambda<LAMBDA_TYPE::PLUS,
	HLambda<first_lambda_type, first_lambda_first_type, first_lambda_second_type>,
	HLambda<LAMBDA_TYPE::VALUE, yaal::hcore::higher_order::placeholder<second> > >
operator + ( HLambda<first_lambda_type, first_lambda_first_type, first_lambda_second_type> const&,
		yaal::hcore::higher_order::placeholder<second> )
	{ return ( HLambda<LAMBDA_TYPE::PLUS,
			HLambda<first_lambda_type, first_lambda_first_type, first_lambda_second_type>,
			HLambda<LAMBDA_TYPE::VALUE, yaal::hcore::higher_order::placeholder<second> > >() ); }

template<int const first,
	LAMBDA_TYPE::type_t const second_lambda_type,
	typename second_lambda_first_type, typename second_lambda_second_type>
HLambda<LAMBDA_TYPE::PLUS,
	HLambda<LAMBDA_TYPE::VALUE, yaal::hcore::higher_order::placeholder<first> >,
	HLambda<second_lambda_type, second_lambda_first_type, second_lambda_second_type> >
operator + ( yaal::hcore::higher_order::placeholder<first> const&,
		HLambda<second_lambda_type, second_lambda_first_type, second_lambda_second_type> const& )
	{ return ( HLambda<LAMBDA_TYPE::PLUS,
			HLambda<LAMBDA_TYPE::VALUE, yaal::hcore::higher_order::placeholder<first> >,
			HLambda<second_lambda_type, second_lambda_first_type, second_lambda_second_type> >() ); }

template<LAMBDA_TYPE::type_t const first_lambda_type,
	typename first_lambda_first_type, typename first_lambda_second_type,
	LAMBDA_TYPE::type_t const second_lambda_type,
	typename second_lambda_first_type, typename second_lambda_second_type>
HLambda<LAMBDA_TYPE::PLUS,
	HLambda<first_lambda_type, first_lambda_first_type, first_lambda_second_type>,
	HLambda<second_lambda_type, second_lambda_first_type, second_lambda_second_type> >
operator + ( HLambda<first_lambda_type, first_lambda_first_type, first_lambda_second_type> const&,
		HLambda<second_lambda_type, second_lambda_first_type, second_lambda_second_type> const& )
	{ return ( HLambda<LAMBDA_TYPE::PLUS,
			HLambda<first_lambda_type, first_lambda_first_type, first_lambda_second_type>,
			HLambda<second_lambda_type, second_lambda_first_type, second_lambda_second_type> >() ); }

/***********************************************************************/

/***********************************************************************/
/* Lambda Minus */

template<int const first, int const second>
inline HLambda<LAMBDA_TYPE::MINUS,
			 HLambda<LAMBDA_TYPE::VALUE, yaal::hcore::higher_order::placeholder<first> >,
			 HLambda<LAMBDA_TYPE::VALUE, yaal::hcore::higher_order::placeholder<second> > >
			 operator - ( yaal::hcore::higher_order::placeholder<first>, yaal::hcore::higher_order::placeholder<second> ) {
	return ( HLambda<LAMBDA_TYPE::MINUS,
			HLambda<LAMBDA_TYPE::VALUE, yaal::hcore::higher_order::placeholder<first> >,
			HLambda<LAMBDA_TYPE::VALUE, yaal::hcore::higher_order::placeholder<second> > >() );
}

template<LAMBDA_TYPE::type_t const first_lambda_type,
	typename first_lambda_first_type, typename first_lambda_second_type, int second>
HLambda<LAMBDA_TYPE::MINUS,
	HLambda<first_lambda_type, first_lambda_first_type, first_lambda_second_type>,
	HLambda<LAMBDA_TYPE::VALUE, yaal::hcore::higher_order::placeholder<second> > >
operator - ( HLambda<first_lambda_type, first_lambda_first_type, first_lambda_second_type> const&,
		yaal::hcore::higher_order::placeholder<second> )
	{ return ( HLambda<LAMBDA_TYPE::MINUS,
			HLambda<first_lambda_type, first_lambda_first_type, first_lambda_second_type>,
			HLambda<LAMBDA_TYPE::VALUE, yaal::hcore::higher_order::placeholder<second> > >() ); }

template<int const first,
	LAMBDA_TYPE::type_t const second_lambda_type,
	typename second_lambda_first_type, typename second_lambda_second_type>
HLambda<LAMBDA_TYPE::MINUS,
	HLambda<LAMBDA_TYPE::VALUE, yaal::hcore::higher_order::placeholder<first> >,
	HLambda<second_lambda_type, second_lambda_first_type, second_lambda_second_type> >
operator - ( yaal::hcore::higher_order::placeholder<first> const&,
		HLambda<second_lambda_type, second_lambda_first_type, second_lambda_second_type> const& )
	{ return ( HLambda<LAMBDA_TYPE::MINUS,
			HLambda<LAMBDA_TYPE::VALUE, yaal::hcore::higher_order::placeholder<first> >,
			HLambda<second_lambda_type, second_lambda_first_type, second_lambda_second_type> >() ); }

template<LAMBDA_TYPE::type_t const first_lambda_type,
	typename first_lambda_first_type, typename first_lambda_second_type,
	LAMBDA_TYPE::type_t const second_lambda_type,
	typename second_lambda_first_type, typename second_lambda_second_type>
HLambda<LAMBDA_TYPE::MINUS,
	HLambda<first_lambda_type, first_lambda_first_type, first_lambda_second_type>,
	HLambda<second_lambda_type, second_lambda_first_type, second_lambda_second_type> >
operator - ( HLambda<first_lambda_type, first_lambda_first_type, first_lambda_second_type> const&,
		HLambda<second_lambda_type, second_lambda_first_type, second_lambda_second_type> const& )
	{ return ( HLambda<LAMBDA_TYPE::MINUS,
			HLambda<first_lambda_type, first_lambda_first_type, first_lambda_second_type>,
			HLambda<second_lambda_type, second_lambda_first_type, second_lambda_second_type> >() ); }

/***********************************************************************/

/***********************************************************************/
/* Lambda Multiplies */

template<int const first, int const second>
inline HLambda<LAMBDA_TYPE::MULTIPLIES,
			 HLambda<LAMBDA_TYPE::VALUE, yaal::hcore::higher_order::placeholder<first> >,
			 HLambda<LAMBDA_TYPE::VALUE, yaal::hcore::higher_order::placeholder<second> > >
			 operator * ( yaal::hcore::higher_order::placeholder<first>, yaal::hcore::higher_order::placeholder<second> ) {
	return ( HLambda<LAMBDA_TYPE::MULTIPLIES,
			HLambda<LAMBDA_TYPE::VALUE, yaal::hcore::higher_order::placeholder<first> >,
			HLambda<LAMBDA_TYPE::VALUE, yaal::hcore::higher_order::placeholder<second> > >() );
}

template<LAMBDA_TYPE::type_t const first_lambda_type,
	typename first_lambda_first_type, typename first_lambda_second_type, int second>
HLambda<LAMBDA_TYPE::MULTIPLIES,
	HLambda<first_lambda_type, first_lambda_first_type, first_lambda_second_type>,
	HLambda<LAMBDA_TYPE::VALUE, yaal::hcore::higher_order::placeholder<second> > >
operator * ( HLambda<first_lambda_type, first_lambda_first_type, first_lambda_second_type> const&,
		yaal::hcore::higher_order::placeholder<second> )
	{ return ( HLambda<LAMBDA_TYPE::MULTIPLIES,
			HLambda<first_lambda_type, first_lambda_first_type, first_lambda_second_type>,
			HLambda<LAMBDA_TYPE::VALUE, yaal::hcore::higher_order::placeholder<second> > >() ); }

template<int const first,
	LAMBDA_TYPE::type_t const second_lambda_type,
	typename second_lambda_first_type, typename second_lambda_second_type>
HLambda<LAMBDA_TYPE::MULTIPLIES,
	HLambda<LAMBDA_TYPE::VALUE, yaal::hcore::higher_order::placeholder<first> >,
	HLambda<second_lambda_type, second_lambda_first_type, second_lambda_second_type> >
operator * ( yaal::hcore::higher_order::placeholder<first> const&,
		HLambda<second_lambda_type, second_lambda_first_type, second_lambda_second_type> const& )
	{ return ( HLambda<LAMBDA_TYPE::MULTIPLIES,
			HLambda<LAMBDA_TYPE::VALUE, yaal::hcore::higher_order::placeholder<first> >,
			HLambda<second_lambda_type, second_lambda_first_type, second_lambda_second_type> >() ); }

template<LAMBDA_TYPE::type_t const first_lambda_type,
	typename first_lambda_first_type, typename first_lambda_second_type,
	LAMBDA_TYPE::type_t const second_lambda_type,
	typename second_lambda_first_type, typename second_lambda_second_type>
HLambda<LAMBDA_TYPE::MULTIPLIES,
	HLambda<first_lambda_type, first_lambda_first_type, first_lambda_second_type>,
	HLambda<second_lambda_type, second_lambda_first_type, second_lambda_second_type> >
operator * ( HLambda<first_lambda_type, first_lambda_first_type, first_lambda_second_type> const&,
		HLambda<second_lambda_type, second_lambda_first_type, second_lambda_second_type> const& )
	{ return ( HLambda<LAMBDA_TYPE::MULTIPLIES,
			HLambda<first_lambda_type, first_lambda_first_type, first_lambda_second_type>,
			HLambda<second_lambda_type, second_lambda_first_type, second_lambda_second_type> >() ); }

/***********************************************************************/

/***********************************************************************/
/* Lambda Divides */

template<int const first, int const second>
inline HLambda<LAMBDA_TYPE::DIVIDES,
			 HLambda<LAMBDA_TYPE::VALUE, yaal::hcore::higher_order::placeholder<first> >,
			 HLambda<LAMBDA_TYPE::VALUE, yaal::hcore::higher_order::placeholder<second> > >
			 operator / ( yaal::hcore::higher_order::placeholder<first>, yaal::hcore::higher_order::placeholder<second> ) {
	return ( HLambda<LAMBDA_TYPE::DIVIDES,
			HLambda<LAMBDA_TYPE::VALUE, yaal::hcore::higher_order::placeholder<first> >,
			HLambda<LAMBDA_TYPE::VALUE, yaal::hcore::higher_order::placeholder<second> > >() );
}

template<LAMBDA_TYPE::type_t const first_lambda_type,
	typename first_lambda_first_type, typename first_lambda_second_type, int second>
HLambda<LAMBDA_TYPE::DIVIDES,
	HLambda<first_lambda_type, first_lambda_first_type, first_lambda_second_type>,
	HLambda<LAMBDA_TYPE::VALUE, yaal::hcore::higher_order::placeholder<second> > >
operator / ( HLambda<first_lambda_type, first_lambda_first_type, first_lambda_second_type> const&,
		yaal::hcore::higher_order::placeholder<second> )
	{ return ( HLambda<LAMBDA_TYPE::DIVIDES,
			HLambda<first_lambda_type, first_lambda_first_type, first_lambda_second_type>,
			HLambda<LAMBDA_TYPE::VALUE, yaal::hcore::higher_order::placeholder<second> > >() ); }

template<int const first,
	LAMBDA_TYPE::type_t const second_lambda_type,
	typename second_lambda_first_type, typename second_lambda_second_type>
HLambda<LAMBDA_TYPE::DIVIDES,
	HLambda<LAMBDA_TYPE::VALUE, yaal::hcore::higher_order::placeholder<first> >,
	HLambda<second_lambda_type, second_lambda_first_type, second_lambda_second_type> >
operator / ( yaal::hcore::higher_order::placeholder<first> const&,
		HLambda<second_lambda_type, second_lambda_first_type, second_lambda_second_type> const& )
	{ return ( HLambda<LAMBDA_TYPE::DIVIDES,
			HLambda<LAMBDA_TYPE::VALUE, yaal::hcore::higher_order::placeholder<first> >,
			HLambda<second_lambda_type, second_lambda_first_type, second_lambda_second_type> >() ); }

template<LAMBDA_TYPE::type_t const first_lambda_type,
	typename first_lambda_first_type, typename first_lambda_second_type,
	LAMBDA_TYPE::type_t const second_lambda_type,
	typename second_lambda_first_type, typename second_lambda_second_type>
HLambda<LAMBDA_TYPE::DIVIDES,
	HLambda<first_lambda_type, first_lambda_first_type, first_lambda_second_type>,
	HLambda<second_lambda_type, second_lambda_first_type, second_lambda_second_type> >
operator / ( HLambda<first_lambda_type, first_lambda_first_type, first_lambda_second_type> const&,
		HLambda<second_lambda_type, second_lambda_first_type, second_lambda_second_type> const& )
	{ return ( HLambda<LAMBDA_TYPE::DIVIDES,
			HLambda<first_lambda_type, first_lambda_first_type, first_lambda_second_type>,
			HLambda<second_lambda_type, second_lambda_first_type, second_lambda_second_type> >() ); }

/***********************************************************************/

template<typename T>
inline HBinder<plus<T>, T, 0> operator + ( T const& constant_, yaal::hcore::higher_order::placeholder<1> ) {
	return ( HBinder<plus<T>, T, 0>( plus<T>(), constant_ ) );
}

template<int const first, typename T>
inline HBinder<plus<T>, T, 1> operator + ( yaal::hcore::higher_order::placeholder<first>, T const& constant_ ) {
	return ( HBinder<plus<T>, T, 1>( plus<T>(), constant_ ) );
}

template<typename T>
inline HBinder<minus<T>, T, 0> operator - ( T const& constant_, yaal::hcore::higher_order::placeholder<1> ) {
	return ( HBinder<minus<T>, T, 0>( minus<T>(), constant_ ) );
}

template<int const first, typename T>
inline HBinder<minus<T>, T, 1> operator - ( yaal::hcore::higher_order::placeholder<first>, T const& constant_ ) {
	return ( HBinder<minus<T>, T, 1>( minus<T>(), constant_ ) );
}

template<typename T>
inline HBinder<multiplies<T>, T, 0> operator * ( T const& constant_, yaal::hcore::higher_order::placeholder<1> ) {
	return ( HBinder<multiplies<T>, T, 0>( multiplies<T>(), constant_ ) );
}

template<int const first, typename T>
inline HBinder<multiplies<T>, T, 1> operator * ( yaal::hcore::higher_order::placeholder<first>, T const& constant_ ) {
	return ( HBinder<multiplies<T>, T, 1>( multiplies<T>(), constant_ ) );
}

template<typename T>
inline HBinder<divides<T>, T, 0> operator / ( T const& constant_, yaal::hcore::higher_order::placeholder<1> ) {
	return ( HBinder<divides<T>, T, 0>( divides<T>(), constant_ ) );
}

template<int const first, typename T>
inline HBinder<divides<T>, T, 1> operator / ( yaal::hcore::higher_order::placeholder<first>, T const& constant_ ) {
	return ( HBinder<divides<T>, T, 1>( divides<T>(), constant_ ) );
}

class HLambdaStream {
	yaal::hcore::HStreamInterface& _stream;
	yaal::hcore::HStreamInterface::ptr_t _buffer;
public:
	HLambdaStream( yaal::hcore::HStreamInterface& stream_ ) : _stream( stream_ ), _buffer( new ( memory::yaal ) HStringStream ) {}
	template<typename T>
	HLambdaStream& operator << ( T const& val_ ) {
		*_buffer << val_;
		return ( *this );
	}
	template<typename T>
	void operator()( T const& val_ ) {
		_stream << val_ << static_cast<HStringStream*>( _buffer.raw() )->string();
	}
};

inline HLambdaStream operator << ( yaal::hcore::HStreamInterface& stream_, yaal::hcore::higher_order::placeholder<1> ) {
	return ( HLambdaStream( stream_ ) );
}

}

}

#endif /* #ifndef YAAL_TOOLS_LAMBDA_HXX_INCLUDED */

