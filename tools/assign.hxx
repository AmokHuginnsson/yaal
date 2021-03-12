/* Read yaal/LICENSE.md file for copyright and licensing information. */
/*! \file tools/assign.hxx
 * \brief Container assignment helper functions.
 */

#ifndef YAAL_TOOLS_ASSIGN_HXX_INCLUDED
#define YAAL_TOOLS_ASSIGN_HXX_INCLUDED 1

#include "hcore/harray.hxx"

namespace yaal {

namespace tools {

template<typename T>
class HAssign {
public:
	typedef HAssign<T> this_type;
	typedef yaal::hcore::HArray<T> data_t;
private:
	data_t _data;
public:
	HAssign( void )
		: _data()
		{}
	HAssign& operator()( T const& val_ ) {
		M_PROLOG
		_data.push_back( val_ );
		return ( *this );
		M_EPILOG
	}
	template<typename U>
	operator U ( void ) const {
		M_PROLOG
		U container( _data.begin(), _data.end() );
		return container;
		M_EPILOG
	}
	template<typename U>
	operator yaal::hcore::HArray<U> ( void ) const {
		M_PROLOG
		yaal::hcore::HArray<U> arr;
		arr.reserve( _data.size() );
		copy( _data.begin(), _data.end(), hcore::back_insert_iterator<yaal::hcore::HArray<U> >( arr ) );
		return arr;
		M_EPILOG
	}
};

template<typename T, typename S>
class HAssignPair {
public:
	typedef HAssignPair<T, S> this_type;
	typedef yaal::hcore::HPair<T, S> pair_t;
	typedef yaal::hcore::HArray<pair_t> data_t;
private:
	data_t _data;
public:
	HAssignPair( void )
		: _data()
		{}
	HAssignPair& operator()( T const& first_, S const& second_ ) {
		M_PROLOG
		_data.push_back( hcore::make_pair( first_, second_ ) );
		return ( *this );
		M_EPILOG
	}
	template<typename U>
	operator U ( void ) const {
		M_PROLOG
		U container( _data.begin(), _data.end() );
		return container;
		M_EPILOG
	}
	template<typename U>
	operator yaal::hcore::HArray<U> ( void ) const {
		M_PROLOG
		yaal::hcore::HArray<U> arr;
		arr.reserve( _data.size() );
		copy( _data.begin(), _data.end(), hcore::back_insert_iterator<yaal::hcore::HArray<U> >( arr ) );
		return arr;
		M_EPILOG
	}
};

template<typename T>
HAssign<T> sequence( T const& val_ ) {
	M_PROLOG
	HAssign<T> assign;
	assign( val_ );
	return assign;
	M_EPILOG
}

template<typename T, typename S>
HAssignPair<T, S> sequence( T const& first_, S const& second_ ) {
	M_PROLOG
	HAssignPair<T, S> assign;
	assign( first_, second_ );
	return assign;
	M_EPILOG
}

template<typename T>
yaal::hcore::HArray<T> array( T const& a0_ ) {
	M_PROLOG
	yaal::hcore::HArray<T> arr( 1, a0_ );
	return arr;
	M_EPILOG
}

template<typename T>
yaal::hcore::HArray<T> array( T const& a0_, T const& a1_ ) {
	M_PROLOG
	yaal::hcore::HArray<T> arr;
	arr.reserve( 2 );
	arr.push_back( a0_ );
	arr.push_back( a1_ );
	return arr;
	M_EPILOG
}

template<typename T>
yaal::hcore::HArray<T> array( T const& a0_, T const& a1_, T const& a2_ ) {
	M_PROLOG
	yaal::hcore::HArray<T> arr;
	arr.reserve( 3 );
	arr.push_back( a0_ );
	arr.push_back( a1_ );
	arr.push_back( a2_ );
	return arr;
	M_EPILOG
}

template<typename T>
yaal::hcore::HArray<T> array( T const& a0_, T const& a1_, T const& a2_, T const& a3_ ) {
	M_PROLOG
	yaal::hcore::HArray<T> arr;
	arr.reserve( 4 );
	arr.push_back( a0_ );
	arr.push_back( a1_ );
	arr.push_back( a2_ );
	arr.push_back( a3_ );
	return arr;
	M_EPILOG
}

template<typename T>
yaal::hcore::HArray<T> array( T const& a0_, T const& a1_, T const& a2_, T const& a3_, T const& a4_ ) {
	M_PROLOG
	yaal::hcore::HArray<T> arr;
	arr.reserve( 5 );
	arr.push_back( a0_ );
	arr.push_back( a1_ );
	arr.push_back( a2_ );
	arr.push_back( a3_ );
	arr.push_back( a4_ );
	return arr;
	M_EPILOG
}

template<typename T>
yaal::hcore::HArray<T> array( T const& a0_, T const& a1_, T const& a2_,
		T const& a3_, T const& a4_, T const& a5_ ) {
	M_PROLOG
	yaal::hcore::HArray<T> arr;
	arr.reserve( 6 );
	arr.push_back( a0_ );
	arr.push_back( a1_ );
	arr.push_back( a2_ );
	arr.push_back( a3_ );
	arr.push_back( a4_ );
	arr.push_back( a5_ );
	return arr;
	M_EPILOG
}

template<typename T>
yaal::hcore::HArray<T> array( T const& a0_, T const& a1_, T const& a2_,
		T const& a3_, T const& a4_, T const& a5_, T const& a6_ ) {
	M_PROLOG
	yaal::hcore::HArray<T> arr;
	arr.reserve( 7 );
	arr.push_back( a0_ );
	arr.push_back( a1_ );
	arr.push_back( a2_ );
	arr.push_back( a3_ );
	arr.push_back( a4_ );
	arr.push_back( a5_ );
	arr.push_back( a6_ );
	return arr;
	M_EPILOG
}

template<typename T>
yaal::hcore::HArray<T> array( T const& a0_, T const& a1_, T const& a2_,
		T const& a3_, T const& a4_, T const& a5_, T const& a6_, T const& a7_ ) {
	M_PROLOG
	yaal::hcore::HArray<T> arr;
	arr.reserve( 8 );
	arr.push_back( a0_ );
	arr.push_back( a1_ );
	arr.push_back( a2_ );
	arr.push_back( a3_ );
	arr.push_back( a4_ );
	arr.push_back( a5_ );
	arr.push_back( a6_ );
	arr.push_back( a7_ );
	return arr;
	M_EPILOG
}

template<typename T>
yaal::hcore::HArray<T> array( T const& a0_, T const& a1_, T const& a2_,
		T const& a3_, T const& a4_, T const& a5_, T const& a6_, T const& a7_, T const& a8_ ) {
	M_PROLOG
	yaal::hcore::HArray<T> arr;
	arr.reserve( 9 );
	arr.push_back( a0_ );
	arr.push_back( a1_ );
	arr.push_back( a2_ );
	arr.push_back( a3_ );
	arr.push_back( a4_ );
	arr.push_back( a5_ );
	arr.push_back( a6_ );
	arr.push_back( a7_ );
	arr.push_back( a8_ );
	return arr;
	M_EPILOG
}

template<typename T>
yaal::hcore::HArray<T> array( T const& a0_, T const& a1_, T const& a2_,
		T const& a3_, T const& a4_, T const& a5_, T const& a6_, T const& a7_,
		T const& a8_, T const& a9_ ) {
	M_PROLOG
	yaal::hcore::HArray<T> arr;
	arr.reserve( 10 );
	arr.push_back( a0_ );
	arr.push_back( a1_ );
	arr.push_back( a2_ );
	arr.push_back( a3_ );
	arr.push_back( a4_ );
	arr.push_back( a5_ );
	arr.push_back( a6_ );
	arr.push_back( a7_ );
	arr.push_back( a8_ );
	arr.push_back( a9_ );
	return arr;
	M_EPILOG
}

template<typename T>
yaal::hcore::HArray<T> array( T const& a0_, T const& a1_, T const& a2_,
		T const& a3_, T const& a4_, T const& a5_, T const& a6_, T const& a7_,
		T const& a8_, T const& a9_, T const& a10_ ) {
	M_PROLOG
	yaal::hcore::HArray<T> arr;
	arr.reserve( 11 );
	arr.push_back( a0_ );
	arr.push_back( a1_ );
	arr.push_back( a2_ );
	arr.push_back( a3_ );
	arr.push_back( a4_ );
	arr.push_back( a5_ );
	arr.push_back( a6_ );
	arr.push_back( a7_ );
	arr.push_back( a8_ );
	arr.push_back( a9_ );
	arr.push_back( a10_ );
	return arr;
	M_EPILOG
}

}

}

#endif /* #ifndef YAAL_TOOLS_ASSIGN_HXX_INCLUDED */

