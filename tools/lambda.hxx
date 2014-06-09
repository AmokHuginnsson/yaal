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
		VARIABLE,
		CONST,
		DEREFERENCE,
		ADDRESS_OF,
		PRE_INCREMENT,
		POST_INCREMENT,
		PRE_DECREMENT,
		POST_DECREMENT,
		PLUS,
		MINUS,
		MULTIPLIES,
		DIVIDES,
		MODULO,
		PLUS_ASSIGN,
		MINUS_ASSIGN,
		MULTIPLIES_ASSIGN,
		DIVIDES_ASSIGN,
		MODULO_ASSIGN,
		NEGATE,
		LOGICAL_NOT,
		LOGICAL_AND,
		LOGICAL_OR,
		BIT_NOT,
		BIT_AND,
		BIT_OR,
		BIT_XOR,
		BIT_AND_ASSIGN,
		BIT_OR_ASSIGN,
		ASSIGN,
		EQUAL,
		NOT_EQUAL,
		LESS,
		GREATER,
		LESS_EQUAL,
		GREATER_EQUAL,
		COMMA,
		STREAM
	} type_t;
};

#ifdef __MSVCXX__
# define TEMPLATE /**/
#else /* #ifdef __MSVCXX__ */
# define TEMPLATE template
#endif /* #else #ifdef __MSVCXX__ */

template<typename R, typename L>
class HLambdaReturn {
	L _lambda;
public:
	HLambdaReturn( L lambda_ ) : _lambda( lambda_ ) {}
	template<typename T1>
	R operator()( T1 const& arg1_ ) const
		{ return ( _lambda.TEMPLATE operator()<R>( arg1_ ) ); }
	template<typename T1, typename T2>
	R operator()( T1 const& arg1_, T2 const& arg2_ ) const
		{ return ( _lambda.TEMPLATE operator()<R>( arg1_, arg2_ ) ); }
	template<typename T1, typename T2, typename T3>
	R operator()( T1 const& arg1_, T2 const& arg2_, T3 const& arg3_ ) const
		{ return ( _lambda.TEMPLATE operator()<R>( arg1_, arg2_, arg3_ ) ); }
	template<typename T1, typename T2, typename T3, typename T4>
	R operator()( T1 const& arg1_, T2 const& arg2_, T3 const& arg3_, T4 const& arg4_ ) const
		{ return ( _lambda.TEMPLATE operator()<R>( arg1_, arg2_, arg3_, arg4_ ) ); }
	template<typename T1, typename T2, typename T3, typename T4, typename T5>
	R operator()( T1 const& arg1_, T2 const& arg2_, T3 const& arg3_, T4 const& arg4_, T5 const& arg5_ ) const
		{ return ( _lambda.TEMPLATE operator()<R>( arg1_, arg2_, arg3_, arg4_, arg5_ ) ); }
};

#undef TEMPLATE

template<typename T, typename L>
HLambdaReturn<T, L> ret( L lambda_ ) {
	return ( HLambdaReturn<T, L>( lambda_ ) );
}

template<LAMBDA_TYPE::type_t const lambda_type, typename first_lambda = trait::no_type, typename second_lambda = trait::no_type>
class HLambda;

template<typename type_t>
class HLambda<LAMBDA_TYPE::CONST, type_t> {
	type_t _const;
public:
	HLambda( type_t const& const_ )
		: _const( const_ )
		{}
	type_t const& operator()( void ) const
		{ return ( _const ); }
	template<typename a0_t>
	type_t const& operator()( a0_t const& ) const
		{ return ( _const ); }
	template<typename a0_t>
	type_t const& operator()( a0_t& ) const
		{ return ( _const ); }
	template<typename a0_t, typename a1_t>
	type_t const& operator()( a0_t const&, a1_t const& ) const
		{ return ( _const ); }
	template<typename a0_t, typename a1_t>
	type_t const& operator()( a0_t&, a1_t const& ) const
		{ return ( _const ); }
	template<typename a0_t, typename a1_t, typename a2_t>
	type_t const& operator()( a0_t const&, a1_t const&, a2_t const& ) const
		{ return ( _const ); }
	template<typename a0_t, typename a1_t, typename a2_t>
	type_t const& operator()( a0_t&, a1_t const&, a2_t const& ) const
		{ return ( _const ); }
};

template<>
class HLambda<LAMBDA_TYPE::VARIABLE, yaal::hcore::higher_order::placeholder<1> > {
public:
	HLambda( void ) {}
	HLambda( yaal::trait::no_type const& ) {}
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
class HLambda<LAMBDA_TYPE::VARIABLE, yaal::hcore::higher_order::placeholder<2> > {
public:
	HLambda( void ) {}
	HLambda( yaal::trait::no_type const& ) {}
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
class HLambda<LAMBDA_TYPE::VARIABLE, yaal::hcore::higher_order::placeholder<3> > {
public:
	HLambda( void ) {}
	HLambda( yaal::trait::no_type const& ) {}
	template<typename a0_t, typename a1_t, typename a2_t>
	a2_t const& operator()( a0_t const&, a1_t const&, a2_t const& a2_ ) const
		{ return ( a2_ ); }
	template<typename a0_t, typename a1_t, typename a2_t>
	a2_t& operator()( a0_t const&, a1_t const&, a2_t& a2_ ) const
		{ return ( a2_ ); }
};

template<>
class HLambda<LAMBDA_TYPE::DEREFERENCE, yaal::hcore::higher_order::placeholder<1> > {
public:
	HLambda( void ) {}
	HLambda( yaal::trait::no_type const& ) {}
	template<typename a0_t>
	a0_t const& operator()( a0_t const* a0_ ) const
		{ return ( *a0_ ); }
	template<typename a0_t>
	a0_t& operator()( a0_t* a0_ ) const
		{ return ( *a0_ ); }
	template<typename a0_t, typename a1_t>
	a0_t const& operator()( a0_t const* a0_, a1_t const& ) const
		{ return ( *a0_ ); }
	template<typename a0_t, typename a1_t>
	a0_t& operator()( a0_t* a0_, a1_t const& ) const
		{ return ( *a0_ ); }
	template<typename a0_t, typename a1_t, typename a2_t>
	a0_t const& operator()( a0_t const* a0_, a1_t const&, a2_t const& ) const
		{ return ( *a0_ ); }
	template<typename a0_t, typename a1_t, typename a2_t>
	a0_t& operator()( a0_t* a0_, a1_t const&, a2_t const& ) const
		{ return ( *a0_ ); }
};

template<>
class HLambda<LAMBDA_TYPE::DEREFERENCE, yaal::hcore::higher_order::placeholder<2> > {
public:
	HLambda( void ) {}
	HLambda( yaal::trait::no_type const& ) {}
	template<typename a0_t, typename a1_t>
	a1_t const& operator()( a0_t const&, a1_t const* a1_ ) const
		{ return ( *a1_ ); }
	template<typename a0_t, typename a1_t>
	a1_t& operator()( a0_t const&, a1_t* a1_ ) const
		{ return ( *a1_ ); }
	template<typename a0_t, typename a1_t, typename a2_t>
	a1_t const& operator()( a0_t const&, a1_t const* a1_, a2_t const& ) const
		{ return ( *a1_ ); }
	template<typename a0_t, typename a1_t, typename a2_t>
	a1_t& operator()( a0_t const&, a1_t* a1_, a2_t const& ) const
		{ return ( *a1_ ); }
};

template<>
class HLambda<LAMBDA_TYPE::DEREFERENCE, yaal::hcore::higher_order::placeholder<3> > {
public:
	HLambda( void ) {}
	HLambda( yaal::trait::no_type const& ) {}
	template<typename a0_t, typename a1_t, typename a2_t>
	a2_t const& operator()( a0_t const&, a1_t const&, a2_t const* a2_ ) const
		{ return ( *a2_ ); }
	template<typename a0_t, typename a1_t, typename a2_t>
	a2_t& operator()( a0_t const&, a1_t const&, a2_t* a2_ ) const
		{ return ( *a2_ ); }
};

template<int const no>
inline HLambda<LAMBDA_TYPE::DEREFERENCE, yaal::hcore::higher_order::placeholder<no> > operator * ( yaal::hcore::higher_order::placeholder<no> arg_ ) {
	return ( HLambda<LAMBDA_TYPE::DEREFERENCE, yaal::hcore::higher_order::placeholder<no> >() );
}

template<typename first_lambda, typename second_lambda>
class HLambda<LAMBDA_TYPE::COMMA, first_lambda, second_lambda> {
	first_lambda _firstLambda;
	second_lambda _secondLambda;
public:
	HLambda( first_lambda const& firstLambda_, second_lambda const& secondLambda_ )
		: _firstLambda( firstLambda_ ), _secondLambda( secondLambda_ )
		{}
	template<typename a0_t>
	void operator()( a0_t& a0_ ) const {
		_firstLambda( a0_ );
		_secondLambda( a0_ );
	}
	template<typename a0_t>
	void operator()( a0_t const& a0_ ) const {
		_firstLambda( a0_ );
		_secondLambda( a0_ );
	}
	template<typename a0_t, typename a1_t>
	void operator()( a0_t& a0_, a1_t& a1_ ) const {
		_firstLambda( a0_, a1_ );
		_secondLambda( a0_, a1_ );
	}
	template<typename a0_t, typename a1_t>
	void operator()( a0_t const& a0_, a1_t const& a1_ ) const {
		_firstLambda( a0_, a1_ );
		_secondLambda( a0_, a1_ );
	}
	template<typename a0_t, typename a1_t, typename a2_t>
	void operator()( a0_t& a0_, a1_t& a1_, a2_t& a2_ ) const {
		_firstLambda( a0_, a1_, a2_ );
		_secondLambda( a0_, a1_, a2_ );
	}
	template<typename a0_t, typename a1_t, typename a2_t>
	void operator()( a0_t const& a0_, a1_t const& a1_, a2_t const& a2_ ) const {
		_firstLambda( a0_, a1_, a2_ );
		_secondLambda( a0_, a1_, a2_ );
	}
	template<typename R, typename a0_t>
	R operator()( a0_t& a0_ ) const {
		_firstLambda( a0_ );
		return ( static_cast<R>( _secondLambda( a0_ ) ) );
	}
	template<typename R, typename a0_t>
	R operator()( a0_t const& a0_ ) const {
		_firstLambda( a0_ );
		return ( static_cast<R>( _secondLambda( a0_ ) ) );
	}
	template<typename R, typename a0_t, typename a1_t>
	R operator()( a0_t& a0_, a1_t& a1_ ) const {
		_firstLambda( a0_, a1_ );
		return ( static_cast<R>( _secondLambda( a0_, a1_ ) ) );
	}
	template<typename R, typename a0_t, typename a1_t>
	R operator()( a0_t const& a0_, a1_t const& a1_ ) const {
		_firstLambda( a0_, a1_ );
		return ( static_cast<R>( _secondLambda( a0_, a1_ ) ) );
	}
	template<typename R, typename a0_t, typename a1_t, typename a2_t>
	R operator()( a0_t& a0_, a1_t& a1_, a2_t& a2_ ) const {
		_firstLambda( a0_, a1_, a2_ );
		return ( static_cast<R>( _secondLambda( a0_, a1_, a2_ ) ) );
	}
	template<typename R, typename a0_t, typename a1_t, typename a2_t>
	R operator()( a0_t const& a0_, a1_t const& a1_, a2_t const& a2_ ) const {
		_firstLambda( a0_, a1_, a2_ );
		return ( static_cast<R>( _secondLambda( a0_, a1_, a2_ ) ) );
	}
};

#ifdef __GNUC__
#if ( 100 * __GNUC__ + __GNUC_MINOR__ ) <= 402
#pragma GCC diagnostic warning "-Weffc++"
#endif /* #if ( 100 * __GNUC__ + __GNUC_MINOR__ ) <= 402 */
#endif /* #ifdef __GNUC__ */


#if defined ( YAAL_DEFINE_LAMBDA )
#	error Yaal redefines YAAL_DEFINE_LAMBDA macro.
#endif /* #if defined ( YAAL_DEFINE_LAMBDA ) */

#define YAAL_DEFINE_LAMBDA( OP, NAME ) \
template<typename first_lambda, typename second_lambda> \
class HLambda<LAMBDA_TYPE::NAME, first_lambda, second_lambda> { \
	first_lambda _firstLambda; \
	second_lambda _secondLambda; \
public: \
	HLambda( first_lambda const& firstLambda_, second_lambda const& secondLambda_ ) \
		: _firstLambda( firstLambda_ ), _secondLambda( secondLambda_ ) \
		{} \
	template<typename a0_t> \
	bool operator()( a0_t const& a0_ ) const { \
		return ( _firstLambda( a0_ ) OP _secondLambda( a0_ ) ); \
	} \
	template<typename a0_t, typename a1_t> \
	bool operator()( a0_t const& a0_, a1_t const& a1_ ) const { \
		return ( _firstLambda( a0_, a1_ ) OP _secondLambda( a0_, a1_ ) ); \
	} \
	template<typename a0_t, typename a1_t, typename a2_t> \
	bool operator()( a0_t const& a0_, a1_t const& a1_, a2_t const& a2_ ) const { \
		return ( _firstLambda( a0_, a1_, a2_ ) OP _secondLambda( a0_, a1_, a2_ ) ); \
	} \
}; \
 \
/* _1 OP 0 */ \
template<int const no, typename T> \
HLambda<LAMBDA_TYPE::NAME, HLambda<LAMBDA_TYPE::VARIABLE, yaal::hcore::higher_order::placeholder<no> >, HLambda<LAMBDA_TYPE::CONST, T> > \
operator OP ( yaal::hcore::higher_order::placeholder<no> const&, T const& const_ ) { \
	return ( HLambda<LAMBDA_TYPE::NAME, HLambda<LAMBDA_TYPE::VARIABLE, yaal::hcore::higher_order::placeholder<no> >, HLambda<LAMBDA_TYPE::CONST, T> >( HLambda<LAMBDA_TYPE::VARIABLE, yaal::hcore::higher_order::placeholder<no> >(), HLambda<LAMBDA_TYPE::CONST, T>( const_ ) ) ); \
} \
 \
/* _1 OP _2 */ \
template<int const first, int const second> \
HLambda<LAMBDA_TYPE::NAME, HLambda<LAMBDA_TYPE::VARIABLE, yaal::hcore::higher_order::placeholder<first> >, HLambda<LAMBDA_TYPE::VARIABLE, yaal::hcore::higher_order::placeholder<second> > > \
operator OP ( yaal::hcore::higher_order::placeholder<first> const&, yaal::hcore::higher_order::placeholder<second> const& ) { \
	return ( HLambda<LAMBDA_TYPE::NAME, HLambda<LAMBDA_TYPE::VARIABLE, yaal::hcore::higher_order::placeholder<first> >, HLambda<LAMBDA_TYPE::VARIABLE, yaal::hcore::higher_order::placeholder<second> > >( HLambda<LAMBDA_TYPE::VARIABLE, yaal::hcore::higher_order::placeholder<first> >(), HLambda<LAMBDA_TYPE::VARIABLE, yaal::hcore::higher_order::placeholder<second> >() ) ); \
} \
 \
/* 0 OP _1 */ \
template<typename T, int const no> \
HLambda<LAMBDA_TYPE::NAME, HLambda<LAMBDA_TYPE::CONST, T>, HLambda<LAMBDA_TYPE::VARIABLE, yaal::hcore::higher_order::placeholder<no> > > \
operator OP ( T const& const_, yaal::hcore::higher_order::placeholder<no> const& ) { \
	return ( HLambda<LAMBDA_TYPE::NAME, HLambda<LAMBDA_TYPE::CONST, T>, HLambda<LAMBDA_TYPE::VARIABLE, yaal::hcore::higher_order::placeholder<no> > >( HLambda<LAMBDA_TYPE::CONST, T>( const_ ), HLambda<LAMBDA_TYPE::VARIABLE, yaal::hcore::higher_order::placeholder<no> >() ) ); \
} \
 \
/* _1 OP ( _2 * 2 ) */ \
template<int const no, LAMBDA_TYPE::type_t const lambda_type, typename lambda_first_type, typename lambda_second_type> \
HLambda<LAMBDA_TYPE::NAME, HLambda<LAMBDA_TYPE::VARIABLE, yaal::hcore::higher_order::placeholder<no> >, HLambda<lambda_type, lambda_first_type, lambda_second_type> > \
operator OP ( yaal::hcore::higher_order::placeholder<no> const&, HLambda<lambda_type, lambda_first_type, lambda_second_type> const& lambda_ ) { \
	return ( HLambda<LAMBDA_TYPE::NAME, HLambda<LAMBDA_TYPE::VARIABLE, yaal::hcore::higher_order::placeholder<no> >, HLambda<lambda_type, lambda_first_type, lambda_second_type> >( HLambda<LAMBDA_TYPE::VARIABLE, yaal::hcore::higher_order::placeholder<no> >(), lambda_ ) ); \
} \
 \
/* ( _2 * 2 ) OP _1 */ \
template<LAMBDA_TYPE::type_t const lambda_type, typename lambda_first_type, typename lambda_second_type, int const no> \
HLambda<LAMBDA_TYPE::NAME, HLambda<lambda_type, lambda_first_type, lambda_second_type>, HLambda<LAMBDA_TYPE::VARIABLE, yaal::hcore::higher_order::placeholder<no> > > \
operator OP ( HLambda<lambda_type, lambda_first_type, lambda_second_type> const& lambda_, yaal::hcore::higher_order::placeholder<no> const& ) { \
	return ( HLambda<LAMBDA_TYPE::NAME, HLambda<lambda_type, lambda_first_type, lambda_second_type>, HLambda<LAMBDA_TYPE::VARIABLE, yaal::hcore::higher_order::placeholder<no> > >( lambda_, HLambda<LAMBDA_TYPE::VARIABLE, yaal::hcore::higher_order::placeholder<no> >() ) ); \
} \
 \
/* 0 OP ( _2 * 2 ) */ \
template<typename T, LAMBDA_TYPE::type_t const lambda_type, typename lambda_first_type, typename lambda_second_type> \
HLambda<LAMBDA_TYPE::NAME, HLambda<LAMBDA_TYPE::CONST, T>, HLambda<lambda_type, lambda_first_type, lambda_second_type> > \
operator OP ( T const& const_, HLambda<lambda_type, lambda_first_type, lambda_second_type> const& lambda_ ) { \
	return ( HLambda<LAMBDA_TYPE::NAME, HLambda<LAMBDA_TYPE::CONST, T>, HLambda<lambda_type, lambda_first_type, lambda_second_type> >( HLambda<LAMBDA_TYPE::CONST, T>( const_ ), lambda_ ) ); \
} \
 \
/* ( _2 * 2 ) OP 0 */ \
template<LAMBDA_TYPE::type_t const lambda_type, typename lambda_first_type, typename lambda_second_type, typename T> \
HLambda<LAMBDA_TYPE::NAME, HLambda<lambda_type, lambda_first_type, lambda_second_type>, HLambda<LAMBDA_TYPE::CONST, T> > \
operator OP ( HLambda<lambda_type, lambda_first_type, lambda_second_type> const& lambda_, T const& const_ ) { \
	return ( HLambda<LAMBDA_TYPE::NAME, HLambda<lambda_type, lambda_first_type, lambda_second_type>, HLambda<LAMBDA_TYPE::CONST, T> >( lambda_, HLambda<LAMBDA_TYPE::CONST, T >( const_ ) ) ); \
} \
 \
/* ( _1 + 1 ) OP ( _2 * 2 ) */ \
template<LAMBDA_TYPE::type_t const first_lambda_type, typename lambda_first_type, typename lambda_second_type, LAMBDA_TYPE::type_t const second_lambda_type, typename second_lambda_first_type, typename second_lambda_second_type> \
HLambda<LAMBDA_TYPE::NAME, HLambda<first_lambda_type, lambda_first_type, lambda_second_type>, HLambda<second_lambda_type, second_lambda_first_type, second_lambda_second_type> > \
operator OP ( HLambda<first_lambda_type, lambda_first_type, lambda_second_type> const& firstLambda_, HLambda<second_lambda_type, second_lambda_first_type, second_lambda_second_type> const& secondLambda_ ) { \
	return ( HLambda<LAMBDA_TYPE::NAME, HLambda<first_lambda_type, lambda_first_type, lambda_second_type>, HLambda<second_lambda_type, second_lambda_first_type, second_lambda_second_type> >( firstLambda_, secondLambda_ ) ); \
}

YAAL_DEFINE_LAMBDA( ==, EQUAL )
YAAL_DEFINE_LAMBDA( !=, NOT_EQUAL )
YAAL_DEFINE_LAMBDA( <, LESS )
YAAL_DEFINE_LAMBDA( >, GREATER )
YAAL_DEFINE_LAMBDA( <=, LESS_EQUAL )
YAAL_DEFINE_LAMBDA( >=, GREATER_EQUAL )

#undef YAAL_DEFINE_LAMBDA

/* !_1 */
template<typename lambda>
class HLambda<LAMBDA_TYPE::LOGICAL_NOT, lambda> {
	lambda _lambda;
public:
	HLambda( lambda const& lambda_ )
		: _lambda( lambda_ )
		{}
	template<typename a0_t>
	bool operator()( a0_t const& a0_ ) const {
		return ( !_lambda( a0_ ) );
	}
	template<typename a0_t, typename a1_t>
	bool operator()( a0_t const& a0_, a1_t const& a1_ ) const {
		return ( !_lambda( a0_, a1_ ) );
	}
	template<typename a0_t, typename a1_t, typename a2_t>
	bool operator()( a0_t const& a0_, a1_t const& a1_, a2_t const& a2_ ) const {
		return ( !_lambda( a0_, a1_, a2_ ) );
	}
};

template<int const no>
HLambda<LAMBDA_TYPE::LOGICAL_NOT, HLambda<LAMBDA_TYPE::VARIABLE, yaal::hcore::higher_order::placeholder<no> > >
operator ! ( yaal::hcore::higher_order::placeholder<no> const& ) {
	return ( HLambda<LAMBDA_TYPE::LOGICAL_NOT, HLambda<LAMBDA_TYPE::VARIABLE, yaal::hcore::higher_order::placeholder<no> > >( HLambda<LAMBDA_TYPE::VARIABLE, yaal::hcore::higher_order::placeholder<no> >() ) );
}

template<LAMBDA_TYPE::type_t const lambda_type, typename lambda_first_type, typename lambda_second_type>
HLambda<LAMBDA_TYPE::LOGICAL_NOT, HLambda<lambda_type, lambda_first_type, lambda_second_type > >
operator ! ( HLambda<lambda_type, lambda_first_type, lambda_second_type > const& lambda_ ) {
	return ( HLambda<LAMBDA_TYPE::LOGICAL_NOT, HLambda<lambda_type, lambda_first_type, lambda_second_type > >( lambda_ ) );
}

/* -_1 */
#define YAAL_DEFINE_LAMBDA( OP, NAME ) \
template<typename lambda> \
class HLambda<LAMBDA_TYPE::NAME, lambda> { \
	lambda _lambda; \
public: \
	HLambda( lambda const& lambda_ ) \
		: _lambda( lambda_ ) \
		{} \
	template<typename R, typename a0_t> \
	R operator()( a0_t const& a0_ ) const { \
		return ( OP _lambda( a0_ ) ); \
	} \
	template<typename R, typename a0_t, typename a1_t> \
	R operator()( a0_t const& a0_, a1_t const& a1_ ) const { \
		return ( OP _lambda( a0_, a1_ ) ); \
	} \
	template<typename R, typename a0_t, typename a1_t, typename a2_t> \
	R  operator()( a0_t const& a0_, a1_t const& a1_, a2_t const& a2_ ) const { \
		return ( OP _lambda( a0_, a1_, a2_ ) ); \
	} \
	template<typename a0_t> \
	a0_t operator()( a0_t const& a0_ ) const { \
		return ( OP _lambda( a0_ ) ); \
	} \
	template<typename a0_t, typename a1_t> \
	typename return_type_binary_arithmetic<a0_t, a1_t>::type operator()( a0_t const& a0_, a1_t const& a1_ ) const { \
		return ( OP _lambda( a0_, a1_ ) ); \
	} \
	template<typename a0_t, typename a1_t, typename a2_t> \
	typename return_type_binary_arithmetic<typename return_type_binary_arithmetic<a0_t, a1_t>::type, a2_t>::type operator()( a0_t const& a0_, a1_t const& a1_, a2_t const& a2_ ) const { \
		return ( OP _lambda( a0_, a1_, a2_ ) ); \
	} \
}; \
 \
template<int const no> \
HLambda<LAMBDA_TYPE::NAME, HLambda<LAMBDA_TYPE::VARIABLE, yaal::hcore::higher_order::placeholder<no> > > \
operator OP ( yaal::hcore::higher_order::placeholder<no> const& ) { \
	return ( HLambda<LAMBDA_TYPE::NAME, HLambda<LAMBDA_TYPE::VARIABLE, yaal::hcore::higher_order::placeholder<no> > >( HLambda<LAMBDA_TYPE::VARIABLE, yaal::hcore::higher_order::placeholder<no> >() ) ); \
} \
 \
template<LAMBDA_TYPE::type_t const lambda_type, typename lambda_first_type, typename lambda_second_type> \
HLambda<LAMBDA_TYPE::NAME, HLambda<lambda_type, lambda_first_type, lambda_second_type > > \
operator OP ( HLambda<lambda_type, lambda_first_type, lambda_second_type > const& lambda_ ) { \
	return ( HLambda<LAMBDA_TYPE::NAME, HLambda<lambda_type, lambda_first_type, lambda_second_type > >( lambda_ ) ); \
}

YAAL_DEFINE_LAMBDA( -, NEGATE )
YAAL_DEFINE_LAMBDA( ~, BIT_NOT )

#undef YAAL_DEFINE_LAMBDA

#define YAAL_DEFINE_LAMBDA( OP, NAME ) \
template<typename first_lambda, typename second_lambda> \
class HLambda<LAMBDA_TYPE::NAME, first_lambda, second_lambda> { \
	first_lambda _firstLambda; \
	second_lambda _secondLambda; \
public: \
	HLambda( first_lambda const& firstLambda_, second_lambda const& secondLambda_ ) \
		: _firstLambda( firstLambda_ ), _secondLambda( secondLambda_ ) \
		{} \
	template<typename a0_t> \
	bool operator()( a0_t const& a0_ ) const { \
		return ( _firstLambda( a0_ ) OP _secondLambda( a0_ ) ); \
	} \
	template<typename a0_t, typename a1_t> \
	bool operator()( a0_t const& a0_, a1_t const& a1_ ) const { \
		return ( _firstLambda( a0_, a1_ ) OP _secondLambda( a0_, a1_ ) ); \
	} \
	template<typename a0_t, typename a1_t, typename a2_t> \
	bool operator()( a0_t const& a0_, a1_t const& a1_, a2_t const& a2_ ) const { \
		return ( _firstLambda( a0_, a1_, a2_ ) OP _secondLambda( a0_, a1_, a2_ ) ); \
	} \
}; \
 \
template<int const no, typename T> \
HLambda<LAMBDA_TYPE::NAME, HLambda<LAMBDA_TYPE::VARIABLE, yaal::hcore::higher_order::placeholder<no> >, HLambda<LAMBDA_TYPE::CONST, T> > \
operator OP ( yaal::hcore::higher_order::placeholder<no> const&, T const& const_ ) { \
	return ( HLambda<LAMBDA_TYPE::NAME, HLambda<LAMBDA_TYPE::VARIABLE, yaal::hcore::higher_order::placeholder<no> >, HLambda<LAMBDA_TYPE::CONST, T> >( HLambda<LAMBDA_TYPE::VARIABLE, yaal::hcore::higher_order::placeholder<no> >(), HLambda<LAMBDA_TYPE::CONST, T>( const_ ) ) ); \
} \
 \
template<int const no, typename T> \
HLambda<LAMBDA_TYPE::NAME, HLambda<LAMBDA_TYPE::CONST, T>, HLambda<LAMBDA_TYPE::VARIABLE, yaal::hcore::higher_order::placeholder<no> > > \
operator OP ( T const& const_, yaal::hcore::higher_order::placeholder<no> const& ) { \
	return ( HLambda<LAMBDA_TYPE::NAME, HLambda<LAMBDA_TYPE::CONST, T>, HLambda<LAMBDA_TYPE::VARIABLE, yaal::hcore::higher_order::placeholder<no> > >( HLambda<LAMBDA_TYPE::CONST, T>( const_ ), HLambda<LAMBDA_TYPE::VARIABLE, yaal::hcore::higher_order::placeholder<no> >() ) ); \
} \
 \
template<LAMBDA_TYPE::type_t const lambda_type, typename lambda_first_type, typename lambda_second_type, typename T> \
HLambda<LAMBDA_TYPE::NAME, HLambda<lambda_type, lambda_first_type, lambda_second_type>, HLambda<LAMBDA_TYPE::CONST, T> > \
operator OP ( HLambda<lambda_type, lambda_first_type, lambda_second_type> const& lambda_, T const& const_ ) { \
	return ( HLambda<LAMBDA_TYPE::NAME, HLambda<lambda_type, lambda_first_type, lambda_second_type>, HLambda<LAMBDA_TYPE::CONST, T> >( lambda_, HLambda<LAMBDA_TYPE::CONST, T>( const_ ) ) ); \
} \
 \
template<typename T, LAMBDA_TYPE::type_t const lambda_type, typename lambda_first_type, typename lambda_second_type> \
HLambda<LAMBDA_TYPE::NAME, HLambda<LAMBDA_TYPE::CONST, T>, HLambda<lambda_type, lambda_first_type, lambda_second_type> > \
operator OP ( T const& const_, HLambda<lambda_type, lambda_first_type, lambda_second_type> const& lambda_ ) { \
	return ( HLambda<LAMBDA_TYPE::NAME, HLambda<LAMBDA_TYPE::CONST, T>, HLambda<lambda_type, lambda_first_type, lambda_second_type> >( HLambda<LAMBDA_TYPE::CONST, T>( const_ ), lambda_ ) ); \
} \
 \
template<int const first, int const second> \
HLambda<LAMBDA_TYPE::NAME, HLambda<LAMBDA_TYPE::VARIABLE, yaal::hcore::higher_order::placeholder<first> >, HLambda<LAMBDA_TYPE::VARIABLE, yaal::hcore::higher_order::placeholder<second> > > \
operator OP ( yaal::hcore::higher_order::placeholder<first> const&, yaal::hcore::higher_order::placeholder<second> const& ) { \
	return ( HLambda<LAMBDA_TYPE::NAME, HLambda<LAMBDA_TYPE::VARIABLE, yaal::hcore::higher_order::placeholder<first> >, HLambda<LAMBDA_TYPE::VARIABLE, yaal::hcore::higher_order::placeholder<second> > >( HLambda<LAMBDA_TYPE::VARIABLE, yaal::hcore::higher_order::placeholder<first> >(), HLambda<LAMBDA_TYPE::VARIABLE, yaal::hcore::higher_order::placeholder<second> >() ) ); \
} \
 \
template<int const no, LAMBDA_TYPE::type_t const lambda_type, typename lambda_first_type, typename lambda_second_type> \
HLambda<LAMBDA_TYPE::NAME, HLambda<LAMBDA_TYPE::VARIABLE, yaal::hcore::higher_order::placeholder<no> >, HLambda<lambda_type, lambda_first_type, lambda_second_type> > \
operator OP ( yaal::hcore::higher_order::placeholder<no> const&, HLambda<lambda_type, lambda_first_type, lambda_second_type> const& lambda_ ) { \
	return ( HLambda<LAMBDA_TYPE::NAME, HLambda<LAMBDA_TYPE::VARIABLE, yaal::hcore::higher_order::placeholder<no> >, HLambda<lambda_type, lambda_first_type, lambda_second_type> >( HLambda<LAMBDA_TYPE::VARIABLE, yaal::hcore::higher_order::placeholder<no> >(), lambda_ ) ); \
} \
 \
template<LAMBDA_TYPE::type_t const lambda_type, typename lambda_first_type, typename lambda_second_type, int const no> \
HLambda<LAMBDA_TYPE::NAME, HLambda<lambda_type, lambda_first_type, lambda_second_type>, HLambda<LAMBDA_TYPE::VARIABLE, yaal::hcore::higher_order::placeholder<no> > > \
operator OP ( HLambda<lambda_type, lambda_first_type, lambda_second_type> const& lambda_, yaal::hcore::higher_order::placeholder<no> const& ) { \
	return ( HLambda<LAMBDA_TYPE::NAME, HLambda<lambda_type, lambda_first_type, lambda_second_type>, HLambda<LAMBDA_TYPE::VARIABLE, yaal::hcore::higher_order::placeholder<no> > >( lambda_, HLambda<LAMBDA_TYPE::VARIABLE, yaal::hcore::higher_order::placeholder<no> >() ) ); \
} \
 \
template<LAMBDA_TYPE::type_t const first_lambda_type, \
	typename first_lambda_first_type, typename first_lambda_second_type, \
	LAMBDA_TYPE::type_t const second_lambda_type, \
	typename second_lambda_first_type, typename second_lambda_second_type> \
HLambda<LAMBDA_TYPE::NAME, HLambda<first_lambda_type, first_lambda_first_type, first_lambda_second_type>, HLambda<second_lambda_type, second_lambda_first_type, second_lambda_second_type> > \
operator OP ( HLambda<first_lambda_type, first_lambda_first_type, first_lambda_second_type> const& firstLambda_, HLambda<second_lambda_type, second_lambda_first_type, second_lambda_second_type> const& secondLambda_ ) { \
	return ( HLambda<LAMBDA_TYPE::NAME, HLambda<first_lambda_type, first_lambda_first_type, first_lambda_second_type>, HLambda<second_lambda_type, second_lambda_first_type, second_lambda_second_type> >( firstLambda_, secondLambda_ ) ); \
}

YAAL_DEFINE_LAMBDA( &&, LOGICAL_AND )
YAAL_DEFINE_LAMBDA( ||, LOGICAL_OR )

#undef YAAL_DEFINE_LAMBDA

template<LAMBDA_TYPE::type_t const first_lambda_type,
	typename first_lambda_first_type, typename first_lambda_second_type,
	LAMBDA_TYPE::type_t const second_lambda_type,
	typename second_lambda_first_type, typename second_lambda_second_type>
HLambda<LAMBDA_TYPE::COMMA,
	HLambda<first_lambda_type, first_lambda_first_type, first_lambda_second_type>,
	HLambda<second_lambda_type, second_lambda_first_type, second_lambda_second_type> >
operator , ( HLambda<first_lambda_type, first_lambda_first_type, first_lambda_second_type> const& firstLambda_,
		HLambda<second_lambda_type, second_lambda_first_type, second_lambda_second_type> const& secondLambda_ )
	{ return ( HLambda<LAMBDA_TYPE::COMMA,
			HLambda<first_lambda_type, first_lambda_first_type, first_lambda_second_type>,
			HLambda<second_lambda_type, second_lambda_first_type, second_lambda_second_type> >( firstLambda_, secondLambda_ ) ); }

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

inline HLambda<LAMBDA_TYPE::PRE_INCREMENT> operator ++ ( HLambda<LAMBDA_TYPE::PRE_INCREMENT> ) {
	return ( HLambda<LAMBDA_TYPE::PRE_INCREMENT>() );
}

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

inline HLambda<LAMBDA_TYPE::POST_INCREMENT> operator ++ ( HLambda<LAMBDA_TYPE::POST_INCREMENT>, int ) {
	return ( HLambda<LAMBDA_TYPE::POST_INCREMENT>() );
}

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

inline HLambda<LAMBDA_TYPE::PRE_DECREMENT> operator -- ( HLambda<LAMBDA_TYPE::PRE_DECREMENT> ) {
	return ( HLambda<LAMBDA_TYPE::PRE_DECREMENT>() );
}

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

inline HLambda<LAMBDA_TYPE::POST_DECREMENT> operator -- ( HLambda<LAMBDA_TYPE::POST_DECREMENT>, int ) {
	return ( HLambda<LAMBDA_TYPE::POST_DECREMENT>() );
}

#define YAAL_DEFINE_LAMBDA( OP, NAME ) \
template<typename first_lambda, typename second_lambda> \
class HLambda<LAMBDA_TYPE::NAME, first_lambda, second_lambda> { \
	first_lambda _firstLambda; \
	second_lambda _secondLambda; \
public: \
	HLambda( first_lambda const& firstLambda_, second_lambda const& secondLambda_ ) \
		: _firstLambda( firstLambda_ ), _secondLambda( secondLambda_ ) \
		{} \
	template<typename R, typename T1> \
	R operator()( T1 const& val1_ ) const { \
		return ( static_cast<R>( _firstLambda( val1_ ) OP _secondLambda( val1_ ) ) ); \
	} \
	template<typename R, typename T1, typename T2> \
	R operator()( T1 const& val1_, T2 const& val2_ ) const { \
		return ( static_cast<R>( _firstLambda( val1_, val2_ ) OP _secondLambda( val1_, val2_ ) ) ); \
	} \
	template<typename R, typename T1, typename T2, typename T3> \
	R operator()( T1 const& val1_, T2 const& val2_, T3 const& val3_ ) const { \
		return ( static_cast<R>( _firstLambda( val1_, val2_, val3_ ) OP _secondLambda( val1_, val2_, val3_ ) ) ); \
	} \
	template<typename T1> \
	T1 operator()( T1 const& val1_ ) const { \
		return ( _firstLambda( val1_ ) OP _secondLambda( val1_ ) ); \
	} \
	template<typename T1, typename T2> \
	typename return_type_binary_arithmetic<T1, T2>::type operator()( T1 const& val1_, T2 const& val2_ ) const { \
		return ( _firstLambda( val1_, val2_ ) OP _secondLambda( val1_, val2_ ) ); \
	} \
	template<typename T1, typename T2, typename T3> \
	typename return_type_binary_arithmetic<typename return_type_binary_arithmetic<T1, T2>::type, T3>::type operator()( T1 const& val1_, T2 const& val2_, T3 const& val3_ ) const { \
		return ( _firstLambda( val1_, val2_, val3_ ) OP _secondLambda( val1_, val2_, val3_ ) ); \
	} \
}; \
 \
template<typename T, int const second> \
inline HLambda<LAMBDA_TYPE::NAME, HLambda<LAMBDA_TYPE::CONST, T>, HLambda<LAMBDA_TYPE::VARIABLE, yaal::hcore::higher_order::placeholder<second> > > \
operator OP ( T const& constant_, yaal::hcore::higher_order::placeholder<second> ) { \
	return ( HLambda<LAMBDA_TYPE::NAME, HLambda<LAMBDA_TYPE::CONST, T>, HLambda<LAMBDA_TYPE::VARIABLE, yaal::hcore::higher_order::placeholder<second> > >( HLambda<LAMBDA_TYPE::CONST, T>( constant_ ), HLambda<LAMBDA_TYPE::VARIABLE, yaal::hcore::higher_order::placeholder<second> >() ) ); \
} \
 \
template<int const first, typename T> \
inline HLambda<LAMBDA_TYPE::NAME, HLambda<LAMBDA_TYPE::VARIABLE, yaal::hcore::higher_order::placeholder<first> >, HLambda<LAMBDA_TYPE::CONST, T> > \
operator OP ( yaal::hcore::higher_order::placeholder<first> const&, T const& constant_ ) { \
	return ( HLambda<LAMBDA_TYPE::NAME, HLambda<LAMBDA_TYPE::VARIABLE, yaal::hcore::higher_order::placeholder<first> >, HLambda<LAMBDA_TYPE::CONST, T> >( HLambda<LAMBDA_TYPE::VARIABLE, yaal::hcore::higher_order::placeholder<first> >(), HLambda<LAMBDA_TYPE::CONST, T>( constant_ ) ) ); \
} \
 \
template<LAMBDA_TYPE::type_t const first_lambda_type, \
	typename first_lambda_first_type, typename first_lambda_second_type, typename T> \
inline HLambda<LAMBDA_TYPE::NAME, HLambda<first_lambda_type, first_lambda_first_type, first_lambda_second_type>, HLambda<LAMBDA_TYPE::CONST, T> > \
operator OP ( HLambda<first_lambda_type, first_lambda_first_type, first_lambda_second_type> const& lambda_, T const& constant_ ) { \
	return ( HLambda<LAMBDA_TYPE::NAME, HLambda<first_lambda_type, first_lambda_first_type, first_lambda_second_type>, HLambda<LAMBDA_TYPE::CONST, T> >( lambda_, HLambda<LAMBDA_TYPE::CONST, T>( constant_ ) ) ); \
} \
 \
template<typename T, LAMBDA_TYPE::type_t const first_lambda_type, \
	typename first_lambda_first_type, typename first_lambda_second_type> \
inline HLambda<LAMBDA_TYPE::NAME, HLambda<LAMBDA_TYPE::CONST, T>, HLambda<first_lambda_type, first_lambda_first_type, first_lambda_second_type> > \
operator OP ( T const& constant_, HLambda<first_lambda_type, first_lambda_first_type, first_lambda_second_type> const& lambda_ ) { \
	return ( HLambda<LAMBDA_TYPE::NAME, HLambda<LAMBDA_TYPE::CONST, T>, HLambda<first_lambda_type, first_lambda_first_type, first_lambda_second_type> >( HLambda<LAMBDA_TYPE::CONST, T>( constant_ ), lambda_ ) ); \
} \
\
template<int const first, int const second> \
inline HLambda<LAMBDA_TYPE::NAME, \
			 HLambda<LAMBDA_TYPE::VARIABLE, yaal::hcore::higher_order::placeholder<first> >, \
			 HLambda<LAMBDA_TYPE::VARIABLE, yaal::hcore::higher_order::placeholder<second> > > \
			 operator OP ( yaal::hcore::higher_order::placeholder<first>, yaal::hcore::higher_order::placeholder<second> ) { \
	return ( HLambda<LAMBDA_TYPE::NAME, \
			HLambda<LAMBDA_TYPE::VARIABLE, yaal::hcore::higher_order::placeholder<first> >, \
			HLambda<LAMBDA_TYPE::VARIABLE, yaal::hcore::higher_order::placeholder<second> > >( HLambda<LAMBDA_TYPE::VARIABLE, yaal::hcore::higher_order::placeholder<first> >(), HLambda<LAMBDA_TYPE::VARIABLE, yaal::hcore::higher_order::placeholder<second> >() ) ); \
} \
 \
template<LAMBDA_TYPE::type_t const first_lambda_type, \
	typename first_lambda_first_type, typename first_lambda_second_type, int second> \
HLambda<LAMBDA_TYPE::NAME, \
	HLambda<first_lambda_type, first_lambda_first_type, first_lambda_second_type>, \
	HLambda<LAMBDA_TYPE::VARIABLE, yaal::hcore::higher_order::placeholder<second> > > \
operator OP ( HLambda<first_lambda_type, first_lambda_first_type, first_lambda_second_type> const& firstLambda_, \
		yaal::hcore::higher_order::placeholder<second> ) \
	{ return ( HLambda<LAMBDA_TYPE::NAME, \
			HLambda<first_lambda_type, first_lambda_first_type, first_lambda_second_type>, \
			HLambda<LAMBDA_TYPE::VARIABLE, yaal::hcore::higher_order::placeholder<second> > >( firstLambda_, HLambda<LAMBDA_TYPE::VARIABLE, yaal::hcore::higher_order::placeholder<second> >() ) ); } \
 \
template<int const first, \
	LAMBDA_TYPE::type_t const second_lambda_type, \
	typename second_lambda_first_type, typename second_lambda_second_type> \
HLambda<LAMBDA_TYPE::NAME, \
	HLambda<LAMBDA_TYPE::VARIABLE, yaal::hcore::higher_order::placeholder<first> >, \
	HLambda<second_lambda_type, second_lambda_first_type, second_lambda_second_type> > \
operator OP ( yaal::hcore::higher_order::placeholder<first> const&, \
		HLambda<second_lambda_type, second_lambda_first_type, second_lambda_second_type> const& lambda_ ) \
	{ return ( HLambda<LAMBDA_TYPE::NAME, \
			HLambda<LAMBDA_TYPE::VARIABLE, yaal::hcore::higher_order::placeholder<first> >, \
			HLambda<second_lambda_type, second_lambda_first_type, second_lambda_second_type> >( yaal::trait::no_type(), lambda_ ) ); } \
 \
template<LAMBDA_TYPE::type_t const first_lambda_type, \
	typename first_lambda_first_type, typename first_lambda_second_type, \
	LAMBDA_TYPE::type_t const second_lambda_type, \
	typename second_lambda_first_type, typename second_lambda_second_type> \
HLambda<LAMBDA_TYPE::NAME, \
	HLambda<first_lambda_type, first_lambda_first_type, first_lambda_second_type>, \
	HLambda<second_lambda_type, second_lambda_first_type, second_lambda_second_type> > \
operator OP ( HLambda<first_lambda_type, first_lambda_first_type, first_lambda_second_type> const& firstLambda_, \
		HLambda<second_lambda_type, second_lambda_first_type, second_lambda_second_type> const& secondLambda_ ) \
	{ return ( HLambda<LAMBDA_TYPE::NAME, \
			HLambda<first_lambda_type, first_lambda_first_type, first_lambda_second_type>, \
			HLambda<second_lambda_type, second_lambda_first_type, second_lambda_second_type> >( firstLambda_, secondLambda_ ) ); }

YAAL_DEFINE_LAMBDA( +, PLUS )
YAAL_DEFINE_LAMBDA( -, MINUS )
YAAL_DEFINE_LAMBDA( *, MULTIPLIES )
YAAL_DEFINE_LAMBDA( /, DIVIDES )
YAAL_DEFINE_LAMBDA( %, MODULO )
YAAL_DEFINE_LAMBDA( &, BIT_AND )
YAAL_DEFINE_LAMBDA( |, BIT_OR )
YAAL_DEFINE_LAMBDA( ^, BIT_XOR )

#undef YAAL_DEFINE_LAMBDA

template<typename first_lambda, typename second_lambda>
class HLambda<LAMBDA_TYPE::ASSIGN, first_lambda, second_lambda> {
	first_lambda _firstLambda;
	second_lambda _secondLambda;
public:
	HLambda( first_lambda const& firstLambda_, second_lambda const& secondLambda_ )
		: _firstLambda( firstLambda_ ), _secondLambda( secondLambda_ )
		{}
	template<typename R, typename T1>
	R operator()( T1 const& val1_ ) const {
		return ( static_cast<R>( _firstLambda( val1_ ) = _secondLambda( val1_ ) ) );
	}
	template<typename R, typename T1>
	R operator()( T1& val1_ ) const {
		return ( static_cast<R>( _firstLambda( val1_ ) = _secondLambda( val1_ ) ) );
	}
	template<typename R, typename T1, typename T2>
	R operator()( T1& val1_, T2 const& val2_ ) const {
		return ( static_cast<R>( _firstLambda( val1_, val2_ ) = _secondLambda( val1_, val2_ ) ) );
	}
	template<typename R, typename T1, typename T2>
	R operator()( T1 const& val1_, T2& val2_ ) const {
		return ( static_cast<R>( _firstLambda( val1_, val2_ ) = _secondLambda( val1_, val2_ ) ) );
	}
	template<typename R, typename T1, typename T2>
	R operator()( T1& val1_, T2& val2_ ) const {
		return ( static_cast<R>( _firstLambda( val1_, val2_ ) = _secondLambda( val1_, val2_ ) ) );
	}
	template<typename R, typename T1, typename T2, typename T3>
	R operator()( T1& val1_, T2 const& val2_, T3 const& val3_ ) const {
		return ( static_cast<R>( _firstLambda( val1_, val2_, val3_ ) = _secondLambda( val1_, val2_, val3_ ) ) );
	}
	template<typename R, typename T1, typename T2, typename T3>
	R operator()( T1& val1_, T2& val2_, T3 const& val3_ ) const {
		return ( static_cast<R>( _firstLambda( val1_, val2_, val3_ ) = _secondLambda( val1_, val2_, val3_ ) ) );
	}
	template<typename R, typename T1, typename T2, typename T3>
	R operator()( T1 const& val1_, T2& val2_, T3 const& val3_ ) const {
		return ( static_cast<R>( _firstLambda( val1_, val2_, val3_ ) = _secondLambda( val1_, val2_, val3_ ) ) );
	}
	template<typename R, typename T1, typename T2, typename T3>
	R operator()( T1 const& val1_, T2& val2_, T3& val3_ ) const {
		return ( static_cast<R>( _firstLambda( val1_, val2_, val3_ ) = _secondLambda( val1_, val2_, val3_ ) ) );
	}
	template<typename R, typename T1, typename T2, typename T3>
	R operator()( T1& val1_, T2 const& val2_, T3& val3_ ) const {
		return ( static_cast<R>( _firstLambda( val1_, val2_, val3_ ) = _secondLambda( val1_, val2_, val3_ ) ) );
	}
	template<typename R, typename T1, typename T2, typename T3>
	R operator()( T1 const& val1_, T2 const& val2_, T3& val3_ ) const {
		return ( static_cast<R>( _firstLambda( val1_, val2_, val3_ ) = _secondLambda( val1_, val2_, val3_ ) ) );
	}
	template<typename R, typename T1, typename T2, typename T3>
	R operator()( T1& val1_, T2& val2_, T3& val3_ ) const {
		return ( static_cast<R>( _firstLambda( val1_, val2_, val3_ ) = _secondLambda( val1_, val2_, val3_ ) ) );
	}
	template<typename T1>
	T1 operator()( T1 const& val1_ ) const {
		return ( _firstLambda( val1_ ) = _secondLambda( val1_ ) );
	}
	template<typename T1>
	T1 operator()( T1& val1_ ) const {
		return ( _firstLambda( val1_ ) = _secondLambda( val1_ ) );
	}
	template<typename T1, typename T2>
	typename return_type_binary_arithmetic<T1, T2>::type operator()( T1& val1_, T2 const& val2_ ) const {
		return ( _firstLambda( val1_, val2_ ) = _secondLambda( val1_, val2_ ) );
	}
	template<typename T1, typename T2>
	typename return_type_binary_arithmetic<T1, T2>::type operator()( T1 const& val1_, T2& val2_ ) const {
		return ( _firstLambda( val1_, val2_ ) = _secondLambda( val1_, val2_ ) );
	}
	template<typename T1, typename T2>
	typename return_type_binary_arithmetic<T1, T2>::type operator()( T1& val1_, T2& val2_ ) const {
		return ( _firstLambda( val1_, val2_ ) = _secondLambda( val1_, val2_ ) );
	}
	template<typename T1, typename T2, typename T3>
	typename return_type_binary_arithmetic<typename return_type_binary_arithmetic<T1, T2>::type, T3>::type operator()( T1& val1_, T2 const& val2_, T3 const& val3_ ) const {
		return ( _firstLambda( val1_, val2_, val3_ ) = _secondLambda( val1_, val2_, val3_ ) );
	}
	template<typename T1, typename T2, typename T3>
	typename return_type_binary_arithmetic<typename return_type_binary_arithmetic<T1, T2>::type, T3>::type operator()( T1& val1_, T2& val2_, T3 const& val3_ ) const {
		return ( _firstLambda( val1_, val2_, val3_ ) = _secondLambda( val1_, val2_, val3_ ) );
	}
	template<typename T1, typename T2, typename T3>
	typename return_type_binary_arithmetic<typename return_type_binary_arithmetic<T1, T2>::type, T3>::type operator()( T1 const& val1_, T2& val2_, T3 const& val3_ ) const {
		return ( _firstLambda( val1_, val2_, val3_ ) = _secondLambda( val1_, val2_, val3_ ) );
	}
	template<typename T1, typename T2, typename T3>
	typename return_type_binary_arithmetic<typename return_type_binary_arithmetic<T1, T2>::type, T3>::type operator()( T1 const& val1_, T2& val2_, T3& val3_ ) const {
		return ( _firstLambda( val1_, val2_, val3_ ) = _secondLambda( val1_, val2_, val3_ ) );
	}
	template<typename T1, typename T2, typename T3>
	typename return_type_binary_arithmetic<typename return_type_binary_arithmetic<T1, T2>::type, T3>::type operator()( T1 const& val1_, T2 const& val2_, T3& val3_ ) const {
		return ( _firstLambda( val1_, val2_, val3_ ) = _secondLambda( val1_, val2_, val3_ ) );
	}
	template<typename T1, typename T2, typename T3>
	typename return_type_binary_arithmetic<typename return_type_binary_arithmetic<T1, T2>::type, T3>::type operator()( T1& val1_, T2 const& val2_, T3& val3_ ) const {
		return ( _firstLambda( val1_, val2_, val3_ ) = _secondLambda( val1_, val2_, val3_ ) );
	}
	template<typename T1, typename T2, typename T3>
	typename return_type_binary_arithmetic<typename return_type_binary_arithmetic<T1, T2>::type, T3>::type operator()( T1& val1_, T2& val2_, T3& val3_ ) const {
		return ( _firstLambda( val1_, val2_, val3_ ) = _secondLambda( val1_, val2_, val3_ ) );
	}
};

template<int const no>
struct lambda_placeholder : public yaal::hcore::higher_order::placeholder<no> {
	lambda_placeholder( void )
		{}
/* _1 = 0 */
	template<typename T>
	HLambda<LAMBDA_TYPE::ASSIGN, HLambda<LAMBDA_TYPE::VARIABLE, yaal::hcore::higher_order::placeholder<no> >, HLambda<LAMBDA_TYPE::CONST, T> >
		operator = ( T const& const_ ) const {
		return ( HLambda<LAMBDA_TYPE::ASSIGN, HLambda<LAMBDA_TYPE::VARIABLE, yaal::hcore::higher_order::placeholder<no> >, HLambda<LAMBDA_TYPE::CONST, T> >( HLambda<LAMBDA_TYPE::VARIABLE, yaal::hcore::higher_order::placeholder<no> >(), HLambda<LAMBDA_TYPE::CONST, T>( const_ ) ) );
	}
/* _1 = _2 */
	template<int const arg_no>
	HLambda<LAMBDA_TYPE::ASSIGN, HLambda<LAMBDA_TYPE::VARIABLE, yaal::hcore::higher_order::placeholder<no> >, HLambda<LAMBDA_TYPE::VARIABLE, yaal::hcore::higher_order::placeholder<arg_no> > >
		operator = ( yaal::hcore::higher_order::placeholder<arg_no> const& ) const {
		return ( HLambda<LAMBDA_TYPE::ASSIGN, HLambda<LAMBDA_TYPE::VARIABLE, yaal::hcore::higher_order::placeholder<no> >, HLambda<LAMBDA_TYPE::VARIABLE, yaal::hcore::higher_order::placeholder<arg_no> > >( HLambda<LAMBDA_TYPE::VARIABLE, yaal::hcore::higher_order::placeholder<no> >(), HLambda<LAMBDA_TYPE::VARIABLE, yaal::hcore::higher_order::placeholder<arg_no> >() ) );
	}
/* _1 = _2 * 2 */
	template<LAMBDA_TYPE::type_t const lambda_type,
		typename lambda_first_type, typename lambda_second_type>
	HLambda<LAMBDA_TYPE::ASSIGN, HLambda<LAMBDA_TYPE::VARIABLE, yaal::hcore::higher_order::placeholder<no> >, HLambda<lambda_type, lambda_first_type, lambda_second_type> >
		operator = ( HLambda<lambda_type, lambda_first_type, lambda_second_type> const& lambda_ ) const {
		return ( HLambda<LAMBDA_TYPE::ASSIGN, HLambda<LAMBDA_TYPE::VARIABLE, yaal::hcore::higher_order::placeholder<no> >, HLambda<lambda_type, lambda_first_type, lambda_second_type> >( HLambda<LAMBDA_TYPE::VARIABLE, yaal::hcore::higher_order::placeholder<no> >(), lambda_ ) );
	}
};

namespace _ {

static const lambda_placeholder<1> _1; /*!< Place holder for first formal lambda argument. */
static const lambda_placeholder<2> _2; /*!< Place holder for second formal lambda argument. */
static const lambda_placeholder<3> _3; /*!< Place holder for third formal lambda argument. */

}

#define YAAL_DEFINE_LAMBDA( OP, NAME ) \
template<typename first_lambda, typename second_lambda> \
class HLambda<LAMBDA_TYPE::NAME, first_lambda, second_lambda> { \
	first_lambda _firstLambda; \
	second_lambda _secondLambda; \
public: \
	HLambda( first_lambda const& firstLambda_, second_lambda const& secondLambda_ ) \
		: _firstLambda( firstLambda_ ), _secondLambda( secondLambda_ ) \
		{} \
	template<typename R, typename T1> \
	R operator()( T1 const& val1_ ) const { \
		return ( static_cast<R>( _firstLambda( val1_ ) OP _secondLambda( val1_ ) ) ); \
	} \
	template<typename R, typename T1> \
	R operator()( T1& val1_ ) const { \
		return ( static_cast<R>( _firstLambda( val1_ ) OP _secondLambda( val1_ ) ) ); \
	} \
	template<typename R, typename T1, typename T2> \
	R operator()( T1& val1_, T2 const& val2_ ) const { \
		return ( static_cast<R>( _firstLambda( val1_, val2_ ) OP _secondLambda( val1_, val2_ ) ) ); \
	} \
	template<typename R, typename T1, typename T2> \
	R operator()( T1 const& val1_, T2& val2_ ) const { \
		return ( static_cast<R>( _firstLambda( val1_, val2_ ) OP _secondLambda( val1_, val2_ ) ) ); \
	} \
	template<typename R, typename T1, typename T2> \
	R operator()( T1& val1_, T2& val2_ ) const { \
		return ( static_cast<R>( _firstLambda( val1_, val2_ ) OP _secondLambda( val1_, val2_ ) ) ); \
	} \
	template<typename R, typename T1, typename T2, typename T3> \
	R operator()( T1& val1_, T2 const& val2_, T3 const& val3_ ) const { \
		return ( static_cast<R>( _firstLambda( val1_, val2_, val3_ ) OP _secondLambda( val1_, val2_, val3_ ) ) ); \
	} \
	template<typename R, typename T1, typename T2, typename T3> \
	R operator()( T1& val1_, T2& val2_, T3 const& val3_ ) const { \
		return ( static_cast<R>( _firstLambda( val1_, val2_, val3_ ) OP _secondLambda( val1_, val2_, val3_ ) ) ); \
	} \
	template<typename R, typename T1, typename T2, typename T3> \
	R operator()( T1 const& val1_, T2& val2_, T3 const& val3_ ) const { \
		return ( static_cast<R>( _firstLambda( val1_, val2_, val3_ ) OP _secondLambda( val1_, val2_, val3_ ) ) ); \
	} \
	template<typename R, typename T1, typename T2, typename T3> \
	R operator()( T1 const& val1_, T2& val2_, T3& val3_ ) const { \
		return ( static_cast<R>( _firstLambda( val1_, val2_, val3_ ) OP _secondLambda( val1_, val2_, val3_ ) ) ); \
	} \
	template<typename R, typename T1, typename T2, typename T3> \
	R operator()( T1& val1_, T2 const& val2_, T3& val3_ ) const { \
		return ( static_cast<R>( _firstLambda( val1_, val2_, val3_ ) OP _secondLambda( val1_, val2_, val3_ ) ) ); \
	} \
	template<typename R, typename T1, typename T2, typename T3> \
	R operator()( T1 const& val1_, T2 const& val2_, T3& val3_ ) const { \
		return ( static_cast<R>( _firstLambda( val1_, val2_, val3_ ) OP _secondLambda( val1_, val2_, val3_ ) ) ); \
	} \
	template<typename R, typename T1, typename T2, typename T3> \
	R operator()( T1& val1_, T2& val2_, T3& val3_ ) const { \
		return ( static_cast<R>( _firstLambda( val1_, val2_, val3_ ) OP _secondLambda( val1_, val2_, val3_ ) ) ); \
	} \
	template<typename T1> \
	T1 operator()( T1 const& val1_ ) const { \
		return ( _firstLambda( val1_ ) OP _secondLambda( val1_ ) ); \
	} \
	template<typename T1> \
	T1 operator()( T1& val1_ ) const { \
		return ( _firstLambda( val1_ ) OP _secondLambda( val1_ ) ); \
	} \
	template<typename T1, typename T2> \
	typename return_type_binary_arithmetic<T1, T2>::type operator()( T1& val1_, T2 const& val2_ ) const { \
		return ( _firstLambda( val1_, val2_ ) OP _secondLambda( val1_, val2_ ) ); \
	} \
	template<typename T1, typename T2> \
	typename return_type_binary_arithmetic<T1, T2>::type operator()( T1 const& val1_, T2& val2_ ) const { \
		return ( _firstLambda( val1_, val2_ ) OP _secondLambda( val1_, val2_ ) ); \
	} \
	template<typename T1, typename T2> \
	typename return_type_binary_arithmetic<T1, T2>::type operator()( T1& val1_, T2& val2_ ) const { \
		return ( _firstLambda( val1_, val2_ ) OP _secondLambda( val1_, val2_ ) ); \
	} \
	template<typename T1, typename T2, typename T3> \
	typename return_type_binary_arithmetic<typename return_type_binary_arithmetic<T1, T2>::type, T3>::type operator()( T1& val1_, T2 const& val2_, T3 const& val3_ ) const { \
		return ( _firstLambda( val1_, val2_, val3_ ) OP _secondLambda( val1_, val2_, val3_ ) ); \
	} \
	template<typename T1, typename T2, typename T3> \
	typename return_type_binary_arithmetic<typename return_type_binary_arithmetic<T1, T2>::type, T3>::type operator()( T1& val1_, T2& val2_, T3 const& val3_ ) const { \
		return ( _firstLambda( val1_, val2_, val3_ ) OP _secondLambda( val1_, val2_, val3_ ) ); \
	} \
	template<typename T1, typename T2, typename T3> \
	typename return_type_binary_arithmetic<typename return_type_binary_arithmetic<T1, T2>::type, T3>::type operator()( T1 const& val1_, T2& val2_, T3 const& val3_ ) const { \
		return ( _firstLambda( val1_, val2_, val3_ ) OP _secondLambda( val1_, val2_, val3_ ) ); \
	} \
	template<typename T1, typename T2, typename T3> \
	typename return_type_binary_arithmetic<typename return_type_binary_arithmetic<T1, T2>::type, T3>::type operator()( T1 const& val1_, T2& val2_, T3& val3_ ) const { \
		return ( _firstLambda( val1_, val2_, val3_ ) OP _secondLambda( val1_, val2_, val3_ ) ); \
	} \
	template<typename T1, typename T2, typename T3> \
	typename return_type_binary_arithmetic<typename return_type_binary_arithmetic<T1, T2>::type, T3>::type operator()( T1 const& val1_, T2 const& val2_, T3& val3_ ) const { \
		return ( _firstLambda( val1_, val2_, val3_ ) OP _secondLambda( val1_, val2_, val3_ ) ); \
	} \
	template<typename T1, typename T2, typename T3> \
	typename return_type_binary_arithmetic<typename return_type_binary_arithmetic<T1, T2>::type, T3>::type operator()( T1& val1_, T2 const& val2_, T3& val3_ ) const { \
		return ( _firstLambda( val1_, val2_, val3_ ) OP _secondLambda( val1_, val2_, val3_ ) ); \
	} \
	template<typename T1, typename T2, typename T3> \
	typename return_type_binary_arithmetic<typename return_type_binary_arithmetic<T1, T2>::type, T3>::type operator()( T1& val1_, T2& val2_, T3& val3_ ) const { \
		return ( _firstLambda( val1_, val2_, val3_ ) OP _secondLambda( val1_, val2_, val3_ ) ); \
	} \
}; \
 \
/* _1 OP 0 */ \
template<int const no, typename T> \
HLambda<LAMBDA_TYPE::NAME, HLambda<LAMBDA_TYPE::VARIABLE, yaal::hcore::higher_order::placeholder<no> >, HLambda<LAMBDA_TYPE::CONST, T> > \
	operator OP ( yaal::hcore::higher_order::placeholder<no> const&, T const& const_ ) { \
	return ( HLambda<LAMBDA_TYPE::NAME, HLambda<LAMBDA_TYPE::VARIABLE, yaal::hcore::higher_order::placeholder<no> >, HLambda<LAMBDA_TYPE::CONST, T> >( HLambda<LAMBDA_TYPE::VARIABLE, yaal::hcore::higher_order::placeholder<no> >(), HLambda<LAMBDA_TYPE::CONST, T>( const_ ) ) ); \
} \
/* _1 OP _2 */ \
template<int const no, int const arg_no> \
HLambda<LAMBDA_TYPE::NAME, HLambda<LAMBDA_TYPE::VARIABLE, yaal::hcore::higher_order::placeholder<no> >, HLambda<LAMBDA_TYPE::VARIABLE, yaal::hcore::higher_order::placeholder<arg_no> > > \
	operator OP ( yaal::hcore::higher_order::placeholder<no> const&, yaal::hcore::higher_order::placeholder<arg_no> const& ) { \
	return ( HLambda<LAMBDA_TYPE::NAME, HLambda<LAMBDA_TYPE::VARIABLE, yaal::hcore::higher_order::placeholder<no> >, HLambda<LAMBDA_TYPE::VARIABLE, yaal::hcore::higher_order::placeholder<arg_no> > >( HLambda<LAMBDA_TYPE::VARIABLE, yaal::hcore::higher_order::placeholder<no> >(), HLambda<LAMBDA_TYPE::VARIABLE, yaal::hcore::higher_order::placeholder<arg_no> >() ) ); \
} \
/* _1 OP ( _2 * 2 ) */ \
template<int const no, LAMBDA_TYPE::type_t const lambda_type, \
	typename lambda_first_type, typename lambda_second_type> \
HLambda<LAMBDA_TYPE::NAME, HLambda<LAMBDA_TYPE::VARIABLE, yaal::hcore::higher_order::placeholder<no> >, HLambda<lambda_type, lambda_first_type, lambda_second_type> > \
	operator OP ( yaal::hcore::higher_order::placeholder<no> const&, HLambda<lambda_type, lambda_first_type, lambda_second_type> const& lambda_ ) { \
	return ( HLambda<LAMBDA_TYPE::NAME, HLambda<LAMBDA_TYPE::VARIABLE, yaal::hcore::higher_order::placeholder<no> >, HLambda<lambda_type, lambda_first_type, lambda_second_type> >( HLambda<LAMBDA_TYPE::VARIABLE, yaal::hcore::higher_order::placeholder<no> >(), lambda_ ) ); \
}

YAAL_DEFINE_LAMBDA( +=, PLUS_ASSIGN )
YAAL_DEFINE_LAMBDA( -=, MINUS_ASSIGN )
YAAL_DEFINE_LAMBDA( *=, MULTIPLIES_ASSIGN )
YAAL_DEFINE_LAMBDA( /=, DIVIDES_ASSIGN )
YAAL_DEFINE_LAMBDA( %=, MODULO_ASSIGN )
YAAL_DEFINE_LAMBDA( &=, BIT_AND_ASSIGN )
YAAL_DEFINE_LAMBDA( |=, BIT_OR_ASSIGN )

#undef YAAL_DEFINE_LAMBDA

template<typename lambda_type>
class HLambda<LAMBDA_TYPE::STREAM, lambda_type> {
	yaal::hcore::HStreamInterface& _stream;
	lambda_type _lambda;
public:
	HLambda( yaal::hcore::HStreamInterface& stream_, lambda_type const& lambda_ )
		: _stream( stream_ ), _lambda( lambda_ )
		{}
	template<typename a0_t>
	void operator()( a0_t& a0_ ) const {
		_stream << _lambda( a0_ );
	}
	template<typename a0_t>
	void operator()( a0_t const& a0_ ) const {
		_stream << _lambda( a0_ );
	}
	template<typename a0_t, typename a1_t>
	void operator()( a0_t& a0_, a1_t& a1_ ) const {
		_stream << _lambda( a0_, a1_ );
	}
	template<typename a0_t, typename a1_t>
	void operator()( a0_t const& a0_, a1_t const& a1_ ) const {
		_stream << _lambda( a0_, a1_ );
	}
	template<typename a0_t, typename a1_t, typename a2_t>
	void operator()( a0_t& a0_, a1_t& a1_, a2_t& a2_ ) const {
		_stream << _lambda( a0_, a1_, a2_ );
	}
	template<typename a0_t, typename a1_t, typename a2_t>
	void operator()( a0_t const& a0_, a1_t const& a1_, a2_t const& a2_ ) const {
		_stream << _lambda( a0_, a1_, a2_ );
	}
	yaal::hcore::HStreamInterface& stream( void ) const {
		return ( _stream );
	}
};

template<typename first_lambda, typename second_lambda>
class HLambda<LAMBDA_TYPE::STREAM, first_lambda, second_lambda> {
	yaal::hcore::HStreamInterface& _stream;
	first_lambda _firstLambda;
	second_lambda _secondLambda;
public:
	HLambda( yaal::hcore::HStreamInterface& stream_, first_lambda const& firstLambda_, second_lambda const& secondLambda_ )
		: _stream( stream_ ), _firstLambda( firstLambda_ ), _secondLambda( secondLambda_ )
		{}
	template<typename a0_t>
	void operator()( a0_t& a0_ ) const {
		_firstLambda( a0_ );
		_stream << _secondLambda( a0_ );
	}
	template<typename a0_t>
	void operator()( a0_t const& a0_ ) const {
		_firstLambda( a0_ );
		_stream << _secondLambda( a0_ );
	}
	template<typename a0_t, typename a1_t>
	void operator()( a0_t& a0_, a1_t& a1_ ) const {
		_firstLambda( a0_, a1_ );
		_stream << _secondLambda( a0_, a1_ );
	}
	template<typename a0_t, typename a1_t>
	void operator()( a0_t const& a0_, a1_t const& a1_ ) const {
		_firstLambda( a0_, a1_ );
		_stream << _secondLambda( a0_, a1_ );
	}
	template<typename a0_t, typename a1_t, typename a2_t>
	void operator()( a0_t& a0_, a1_t& a1_, a2_t& a2_ ) const {
		_firstLambda( a0_, a1_, a2_ );
		_stream << _secondLambda( a0_, a1_, a2_ );
	}
	template<typename a0_t, typename a1_t, typename a2_t>
	void operator()( a0_t const& a0_, a1_t const& a1_, a2_t const& a2_ ) const {
		_firstLambda( a0_, a1_, a2_ );
		_stream << _secondLambda( a0_, a1_, a2_ );
	}
};

template<int const no>
HLambda<LAMBDA_TYPE::STREAM, HLambda<LAMBDA_TYPE::VARIABLE, yaal::hcore::higher_order::placeholder<no> > >
operator << ( yaal::hcore::HStreamInterface& stream_, yaal::hcore::higher_order::placeholder<no> ) {
	return ( HLambda<LAMBDA_TYPE::STREAM, HLambda<LAMBDA_TYPE::VARIABLE, yaal::hcore::higher_order::placeholder<no> > >( stream_, HLambda<LAMBDA_TYPE::VARIABLE, yaal::hcore::higher_order::placeholder<no> >() )  );
}

template<LAMBDA_TYPE::type_t const lambda_type, typename lambda_type_first_type, typename lambda_type_second_type>
HLambda<LAMBDA_TYPE::STREAM, HLambda<lambda_type, lambda_type_first_type, lambda_type_second_type> >
operator << ( yaal::hcore::HStreamInterface& stream_, HLambda<lambda_type, lambda_type_first_type, lambda_type_second_type> const& lambda_ ) {
	return ( HLambda<LAMBDA_TYPE::STREAM, HLambda<lambda_type, lambda_type_first_type, lambda_type_second_type> >( stream_, lambda_ )  );
}

template<typename stream_lambda_first_type, typename stream_lambda_second_type,
	LAMBDA_TYPE::type_t const lambda_type, typename lambda_type_first_type, typename lambda_type_second_type>
HLambda<LAMBDA_TYPE::STREAM, HLambda<LAMBDA_TYPE::STREAM, stream_lambda_first_type, stream_lambda_second_type>, HLambda<lambda_type, lambda_type_first_type, lambda_type_second_type> >
operator << ( HLambda<LAMBDA_TYPE::STREAM, stream_lambda_first_type, stream_lambda_second_type> const& streamLambda_, HLambda<lambda_type, lambda_type_first_type, lambda_type_second_type> const& lambda_ ) {
	return ( HLambda<LAMBDA_TYPE::STREAM, HLambda<LAMBDA_TYPE::STREAM, stream_lambda_first_type, stream_lambda_second_type>, HLambda<lambda_type, lambda_type_first_type, lambda_type_second_type> >( streamLambda_.stream(), streamLambda_, lambda_ )  );
}

template<typename stream_lambda_first_type, typename stream_lambda_second_type>
HLambda<LAMBDA_TYPE::STREAM, HLambda<LAMBDA_TYPE::STREAM, stream_lambda_first_type, stream_lambda_second_type>, HLambda<LAMBDA_TYPE::CONST, char const* const> >
operator << ( HLambda<LAMBDA_TYPE::STREAM, stream_lambda_first_type, stream_lambda_second_type> const& streamLambda_, char const* str_ ) {
	return ( HLambda<LAMBDA_TYPE::STREAM, HLambda<LAMBDA_TYPE::STREAM, stream_lambda_first_type, stream_lambda_second_type>, HLambda<LAMBDA_TYPE::CONST, char const* const> >( streamLambda_.stream(), streamLambda_, HLambda<LAMBDA_TYPE::CONST, char const* const>( str_ ) )  );
}

template<typename stream_lambda_first_type, typename stream_lambda_second_type, typename T>
HLambda<LAMBDA_TYPE::STREAM, HLambda<LAMBDA_TYPE::STREAM, stream_lambda_first_type, stream_lambda_second_type>, HLambda<LAMBDA_TYPE::CONST, T> >
operator << ( HLambda<LAMBDA_TYPE::STREAM, stream_lambda_first_type, stream_lambda_second_type> const& streamLambda_, T const& const_ ) {
	return ( HLambda<LAMBDA_TYPE::STREAM, HLambda<LAMBDA_TYPE::STREAM, stream_lambda_first_type, stream_lambda_second_type>, HLambda<LAMBDA_TYPE::CONST, T> >( streamLambda_.stream(), streamLambda_, HLambda<LAMBDA_TYPE::CONST, T>( const_ ) ) );
}

}

}

#endif /* #ifndef YAAL_TOOLS_LAMBDA_HXX_INCLUDED */

