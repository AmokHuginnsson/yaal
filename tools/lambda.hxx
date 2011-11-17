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

class HLambdaPlus {
public:
	template<typename R, typename T1, typename T2>
	R operator()( T1 const& val1_, T2 const& val2_ )
		{ return ( static_cast<R>( val1_ + val2_ ) ); }
	template<typename T1, typename T2>
	typename return_type_binary_arithmetic<T1, T2>::type operator()( T1 const& val1_, T2 const& val2_ )
		{ return ( val1_ + val2_ ); }
};

class HLambdaMinus {
public:
	template<typename R, typename T1, typename T2>
	R operator()( T1 const& val1_, T2 const& val2_ )
		{ return ( static_cast<R>( val1_ - val2_ ) ); }
	template<typename T1, typename T2>
	typename return_type_binary_arithmetic<T1, T2>::type operator()( T1 const& val1_, T2 const& val2_ )
		{ return ( val1_ - val2_ ); }
};

class HLambdaMuliplies {
public:
	template<typename R, typename T1, typename T2>
	R operator()( T1 const& val1_, T2 const& val2_ )
		{ return ( static_cast<R>( val1_ * val2_ ) ); }
	template<typename T1, typename T2>
	typename return_type_binary_arithmetic<T1, T2>::type operator()( T1 const& val1_, T2 const& val2_ )
		{ return ( val1_ * val2_ ); }
};

class HLambdaDivides {
public:
	template<typename R, typename T1, typename T2>
	R operator()( T1 const& val1_, T2 const& val2_ )
		{ return ( static_cast<R>( val1_ / val2_ ) ); }
	template<typename T1, typename T2>
	typename return_type_binary_arithmetic<T1, T2>::type operator()( T1 const& val1_, T2 const& val2_ )
		{ return ( val1_ / val2_ ); }
};

inline HLambdaPlus operator + ( yaal::hcore::higher_order::placeholder<1>, yaal::hcore::higher_order::placeholder<2> ) {
	return ( HLambdaPlus() );
}

inline HLambdaMinus operator - ( yaal::hcore::higher_order::placeholder<1>, yaal::hcore::higher_order::placeholder<2> ) {
	return ( HLambdaMinus() );
}

inline HLambdaMuliplies operator * ( yaal::hcore::higher_order::placeholder<1>, yaal::hcore::higher_order::placeholder<2> ) {
	return ( HLambdaMuliplies() );
}

inline HLambdaDivides operator / ( yaal::hcore::higher_order::placeholder<1>, yaal::hcore::higher_order::placeholder<2> ) {
	return ( HLambdaDivides() );
}

template<typename T>
inline HBinder<plus<T>, T, 0> operator + ( T const& constant_, yaal::hcore::higher_order::placeholder<1> ) {
	return ( HBinder<plus<T>, T, 0>( plus<T>(), constant_ ) );
}

template<typename T>
inline HBinder<plus<T>, T, 1> operator + ( yaal::hcore::higher_order::placeholder<1>, T const& constant_ ) {
	return ( HBinder<plus<T>, T, 1>( plus<T>(), constant_ ) );
}

template<typename T>
inline HBinder<minus<T>, T, 0> operator - ( T const& constant_, yaal::hcore::higher_order::placeholder<1> ) {
	return ( HBinder<minus<T>, T, 0>( minus<T>(), constant_ ) );
}

template<typename T>
inline HBinder<minus<T>, T, 1> operator - ( yaal::hcore::higher_order::placeholder<1>, T const& constant_ ) {
	return ( HBinder<minus<T>, T, 1>( minus<T>(), constant_ ) );
}

template<typename T>
inline HBinder<multiplies<T>, T, 0> operator * ( T const& constant_, yaal::hcore::higher_order::placeholder<1> ) {
	return ( HBinder<multiplies<T>, T, 0>( multiplies<T>(), constant_ ) );
}

template<typename T>
inline HBinder<multiplies<T>, T, 1> operator * ( yaal::hcore::higher_order::placeholder<1>, T const& constant_ ) {
	return ( HBinder<multiplies<T>, T, 1>( multiplies<T>(), constant_ ) );
}

template<typename T>
inline HBinder<divides<T>, T, 0> operator / ( T const& constant_, yaal::hcore::higher_order::placeholder<1> ) {
	return ( HBinder<divides<T>, T, 0>( divides<T>(), constant_ ) );
}

template<typename T>
inline HBinder<divides<T>, T, 1> operator / ( yaal::hcore::higher_order::placeholder<1>, T const& constant_ ) {
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

