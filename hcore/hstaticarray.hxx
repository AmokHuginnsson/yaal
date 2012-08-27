/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hcore/hstaticarray.hxx - this file is integral part of `yaal' project.

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
/*! \file hcore/hstaticarray.hxx
 * \brief Declaration and implementation of HStaticArray<> class template.
 */

#ifndef YAAL_HCORE_HSTATICARRAY_HXX_INCLUDED
#define YAAL_HCORE_HSTATICARRAY_HXX_INCLUDED 1

#include "hcore/hexception.hxx"
#include "hcore/iterator.hxx"

namespace yaal {

namespace hcore {

extern M_YAAL_HCORE_PUBLIC_API char const* const _errMsgHStaticArray_[];

/*! \brief Smarter version of C arrays.
 * Arrays provisioned with boundary checking and copy constructible.
 */
template<typename type_t, int const N>
class HStaticArray {
public:
	typedef HStaticArray<type_t, N> this_type;
	typedef type_t value_type;
	struct ERROR {
		/*! \brief Error codes for HStaticArray<> operations.
		 */
		typedef enum {
			OK = 0,            /*!< No error. */
			BAD_INDEX,         /*!< Index of of bounds. */
			BAD_SEQUENCE_SIZE, /*!< Paired sequence size is invalid. */
			COUNT_NOT_MATCH    /*!< Specifed count does not match with container size. */
		} error_t;
	};
	typedef value_type* iterator;
	typedef value_type const* const_iterator;
	typedef HReverseIterator<iterator> reverse_iterator;
	typedef HReverseIterator<const_iterator> const_reverse_iterator;
private:
	type_t _data[N];
public:
	HStaticArray( void )
		: _data()
		{	}
	HStaticArray( value_type const& filler_ )
		: _data() {
		M_PROLOG
		fill( filler_ );
		M_EPILOG
	}
	template<typename iterator_t>
	HStaticArray( iterator_t first_, iterator_t last_ )
		: _data() {
		M_PROLOG
		assign( first_, last_ );
		return;
		M_EPILOG
	}
	template<typename iterator_t>
	HStaticArray( iterator_t first_, int count_ )
		: _data() {
		M_PROLOG
		assign( first_, count_ );
		return;
		M_EPILOG
	}
	int size( void ) const {
		return ( N );
	}
	int get_size( void ) const {
		return ( N );
	}
	bool is_empty( void ) const {
		return ( N == 0 );
	}
	bool empty( void ) const {
		return ( N == 0 );
	}
	type_t const& operator[] ( int long idx_ ) const {
		if ( ( idx_ >= N ) || ( idx_ < 0 ) )
			M_THROW( _errMsgHStaticArray_[ ERROR::BAD_INDEX ], idx_ );
		return ( _data[idx_] );
	}
	type_t& operator[] ( int long idx_ ) {
		if ( ( idx_ >= N ) || ( idx_ < 0 ) )
			M_THROW( _errMsgHStaticArray_[ ERROR::BAD_INDEX ], idx_ );
		return ( _data[idx_] );
	}
	type_t const* data( void ) const {
		return ( _data );
	}
	type_t* data( void ) {
		return ( _data );
	}
	void fill( value_type const& filler_ ) {
		M_PROLOG
		for ( int i( 0 ); i < N; ++ i )
			_data[i] = filler_;
		return;
		M_EPILOG
	}
	void swap( HStaticArray& sa_ ) {
		M_PROLOG
		if ( &sa_ != this ) {
			using yaal::swap;
			for ( int i( 0 ); i < N; ++ i )
				swap( _data[i], sa_._data[i] );
		}
		return;
		M_EPILOG
	}
	const_iterator begin( void ) const {
		return ( _data );
	}
	const_iterator cbegin( void ) const {
		return ( _data );
	}
	iterator begin( void ) {
		return ( _data );
	}
	const_iterator end( void ) const {
		return ( _data + N );
	}
	const_iterator cend( void ) const {
		return ( _data + N );
	}
	iterator end( void ) {
		return ( _data + N );
	}
	const_reverse_iterator rbegin( void ) const {
		return ( const_reverse_iterator( end() ) );
	}
	const_reverse_iterator crbegin( void ) const {
		return ( const_reverse_iterator( cend() ) );
	}
	reverse_iterator rbegin( void ) {
		return ( reverse_iterator( end() ) );
	}
	const_reverse_iterator rend( void ) const {
		return ( const_reverse_iterator( begin() ) );
	}
	const_reverse_iterator crend( void ) const {
		return ( const_reverse_iterator( cbegin() ) );
	}
	reverse_iterator rend( void ) {
		return ( reverse_iterator( begin() ) );
	}
	value_type const& front( void ) const {
		return ( _data[0] );
	}
	value_type& front( void ) {
		return ( _data[0] );
	}
	value_type const& back( void ) const {
		return ( _data[N - 1] );
	}
	value_type& back( void ) {
		return ( _data[N - 1] );
	}
	template<typename iterator_t>
	void assign( iterator_t first_, iterator_t last_ ) {
		M_PROLOG
		using yaal::distance;
		int long count( distance( first_, last_ ) );
		if ( count != N )
			M_THROW( _errMsgHStaticArray_[ ERROR::BAD_SEQUENCE_SIZE ], count );
		for ( int i( 0 ); first_ != last_; ++ first_, ++ i ) {
			_data[i] = *first_;
		}
		return;
		M_EPILOG
	}
	template<typename iterator_t>
	void assign( iterator_t first_, int count_ ) {
		M_PROLOG
		if ( count_ != N )
			M_THROW( _errMsgHStaticArray_[ ERROR::COUNT_NOT_MATCH ], count_ );
		for ( int i( 0 ); i < count_; ++ first_, ++ i ) {
			_data[i] = *first_;
		}
		return;
		M_EPILOG
	}
	bool operator == ( HStaticArray const& sa_ ) const {
		M_PROLOG
		int i( 0 );
		for ( ; i < N && ( _data[i] == sa_._data[i] ); ++ i )
			;
		return ( i == N );
		M_EPILOG
	}
};

}

template<typename type_t, int const N>
inline void swap( yaal::hcore::HStaticArray<type_t, N>& a, yaal::hcore::HStaticArray<type_t, N>& b )
	{ a.swap( b ); }

}

#endif /* #ifndef YAAL_HCORE_HSTATICARRAY_HXX_INCLUDED */

