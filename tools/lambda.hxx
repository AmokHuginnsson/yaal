/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	lambda.hxx - this file is integral part of `yaal' project.

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
/*! \file tools/lambda.hxx
 * \brief Declaration of labda language extension.
 */

#ifndef YAAL_TOOLS_LAMBDA_HXX_INCLUDED
#define YAAL_TOOLS_LAMBDA_HXX_INCLUDED 1

#include "hcore/placeholder.hxx"
#include "hcore/hstreaminterface.hxx"
#include "tools/hstringstream.hxx"

namespace yaal
{

namespace tools
{

template<int no>
inline yaal::hcore::higher_order::placeholder<no> operator *( yaal::hcore::higher_order::placeholder<no> arg_ )
	{
	return ( arg_ );
	}

template<typename T1, typename T2>
struct promote
	{
	typedef typename trait::ternary<
			is_integral<T1>::value != is_integral<T2>::value,
			typename trait::ternary<is_floating_point<T1>::value, T1, T2>::type,
			typename trait::ternary<sizeof ( T1 ) >= sizeof (T2 ), T1, T2>::type
		>::type type;
	};

template<typename T1, typename T2>
struct return_type_binary_arithmetic
	{
	static bool const both_numeric = ( is_numeric<T1>::value && is_numeric<T2>::value );
	typedef typename trait::ternary<trait::same_type<T1, T2>::value, T1, typename trait::ternary<both_numeric, typename promote<T1, T2>::type, trait::no_type>::type>::type type;
	};

class HLambdaPlus
	{
public:
	template<typename T1, typename T2>
	typename return_type_binary_arithmetic<T1, T2>::type operator()( T1 const& val1_, T2 const& val2_ )
		{
		return ( val1_ + val2_ );
		}
	};

class HLambdaMinus
	{
public:
	template<typename T1, typename T2>
	typename return_type_binary_arithmetic<T1, T2>::type operator()( T1 const& val1_, T2 const& val2_ )
		{
		return ( val1_ - val2_ );
		}
	};

class HLambdaMuliplies
	{
public:
	template<typename T1, typename T2>
	typename return_type_binary_arithmetic<T1, T2>::type operator()( T1 const& val1_, T2 const& val2_ )
		{
		return ( val1_ * val2_ );
		}
	};

class HLambdaDivides
	{
public:
	template<typename T1, typename T2>
	typename return_type_binary_arithmetic<T1, T2>::type operator()( T1 const& val1_, T2 const& val2_ )
		{
		return ( val1_ / val2_ );
		}
	};

inline HLambdaPlus operator + ( yaal::hcore::higher_order::placeholder<1>, yaal::hcore::higher_order::placeholder<2> )
	{
	return ( HLambdaPlus() );
	}

inline HLambdaMinus operator - ( yaal::hcore::higher_order::placeholder<1>, yaal::hcore::higher_order::placeholder<2> )
	{
	return ( HLambdaMinus() );
	}

inline HLambdaMuliplies operator * ( yaal::hcore::higher_order::placeholder<1>, yaal::hcore::higher_order::placeholder<2> )
	{
	return ( HLambdaMuliplies() );
	}

inline HLambdaDivides operator / ( yaal::hcore::higher_order::placeholder<1>, yaal::hcore::higher_order::placeholder<2> )
	{
	return ( HLambdaDivides() );
	}

template<typename T>
inline HBinder<plus<T>, T, 0> operator + ( T const& constant_, yaal::hcore::higher_order::placeholder<1> )
	{
	return ( HBinder<plus<T>, T, 0>( plus<T>(), constant_ ) );
	}

template<typename T>
inline HBinder<plus<T>, T, 1> operator + ( yaal::hcore::higher_order::placeholder<1>, T const& constant_ )
	{
	return ( HBinder<plus<T>, T, 1>( plus<T>(), constant_ ) );
	}

template<typename T>
inline HBinder<minus<T>, T, 0> operator - ( T const& constant_, yaal::hcore::higher_order::placeholder<1> )
	{
	return ( HBinder<minus<T>, T, 0>( minus<T>(), constant_ ) );
	}

template<typename T>
inline HBinder<minus<T>, T, 1> operator - ( yaal::hcore::higher_order::placeholder<1>, T const& constant_ )
	{
	return ( HBinder<minus<T>, T, 1>( minus<T>(), constant_ ) );
	}

template<typename T>
inline HBinder<multiplies<T>, T, 0> operator * ( T const& constant_, yaal::hcore::higher_order::placeholder<1> )
	{
	return ( HBinder<multiplies<T>, T, 0>( multiplies<T>(), constant_ ) );
	}

template<typename T>
inline HBinder<multiplies<T>, T, 1> operator * ( yaal::hcore::higher_order::placeholder<1>, T const& constant_ )
	{
	return ( HBinder<multiplies<T>, T, 1>( multiplies<T>(), constant_ ) );
	}

template<typename T>
inline HBinder<divides<T>, T, 0> operator / ( T const& constant_, yaal::hcore::higher_order::placeholder<1> )
	{
	return ( HBinder<divides<T>, T, 0>( divides<T>(), constant_ ) );
	}

template<typename T>
inline HBinder<divides<T>, T, 1> operator / ( yaal::hcore::higher_order::placeholder<1>, T const& constant_ )
	{
	return ( HBinder<divides<T>, T, 1>( divides<T>(), constant_ ) );
	}

class HLambdaStream
	{
	yaal::hcore::HStreamInterface& _stream;
	yaal::hcore::HStreamInterface::ptr_t _buffer;
public:
	HLambdaStream( yaal::hcore::HStreamInterface& stream_ ) : _stream( stream_ ), _buffer( new HStringStream ) {}
	template<typename T>
	HLambdaStream& operator << ( T const& val_ )
		{
		*_buffer << val_;
		return ( *this );
		}
	template<typename T>
	void operator()( T const& val_ )
		{
		_stream << val_ << static_cast<HStringStream*>( _buffer.raw() )->string();
		}
	};

inline HLambdaStream operator << ( yaal::hcore::HStreamInterface& stream_, yaal::hcore::higher_order::placeholder<1> )
	{
	return ( HLambdaStream( stream_ ) );
	}

}

}

#endif /* #ifndef YAAL_TOOLS_LAMBDA_HXX_INCLUDED */

