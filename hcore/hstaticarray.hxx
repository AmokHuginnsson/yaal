/* Read yaal/LICENSE.md file for copyright and licensing information. */
/*! \file hcore/hstaticarray.hxx
 * \brief Declaration and implementation of HStaticArray<> class template.
 */

#ifndef YAAL_HCORE_HSTATICARRAY_HXX_INCLUDED
#define YAAL_HCORE_HSTATICARRAY_HXX_INCLUDED 1

#include <initializer_list>

#include "hcore/hexception.hxx"
#include "hcore/iterator.hxx"
#include "hcore/algorithm_low.hxx"

namespace yaal {

namespace hcore {

extern M_YAAL_HCORE_PUBLIC_API char const* const _errMsgHStaticArray_[];

/*! \brief Smarter version of C arrays.
 * Arrays provisioned with boundary checking and copy constructible.
 */
template<typename type_t, int const N>
class HStaticArray final {
public:
	typedef HStaticArray<type_t, N> this_type;
	typedef type_t value_type;
	typedef int size_type;
	struct ERROR {
		/*! \brief Error codes for HStaticArray<> operations.
		 */
		typedef enum {
			OK = 0,            /*!< No error. */
			BAD_INDEX,         /*!< Index of of bounds. */
			BAD_SEQUENCE_SIZE, /*!< Paired sequence size is invalid. */
			COUNT_NOT_MATCH    /*!< Specified count does not match with container size. */
		} error_t;
	};
	typedef value_type* iterator;
	typedef value_type const* const_iterator;
	typedef HReverseIterator<iterator> reverse_iterator;
	typedef HReverseIterator<const_iterator> const_reverse_iterator;
private:
	type_t _data[trait::to_unsigned<int, N>::value];
public:
	HStaticArray( void )
		: _data() {
		return;
	}
	HStaticArray( std::initializer_list<value_type> constants_ )
		: _data() {
		M_PROLOG
		assign( constants_.begin(), constants_.end() );
		return;
		M_EPILOG
	}
	HStaticArray( value_type const& filler_ )
		: _data() {
		M_PROLOG
		fill( filler_ );
		return;
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
	HStaticArray( iterator_t first_, size_type count_ )
		: _data() {
		M_PROLOG
		assign( first_, count_ );
		return;
		M_EPILOG
	}
	size_type size( void ) const {
		return ( N );
	}
	size_type get_size( void ) const {
		return ( N );
	}
	bool is_empty( void ) const {
		return ( N == 0 );
	}
	bool empty( void ) const {
		return ( N == 0 );
	}
	type_t const& operator[] ( size_type idx_ ) const {
		if ( ( idx_ >= N ) || ( idx_ < 0 ) ) {
			M_THROW( _errMsgHStaticArray_[ ERROR::BAD_INDEX ], idx_ );
		}
		return ( _data[idx_] );
	}
	type_t& operator[] ( size_type idx_ ) {
		if ( ( idx_ >= N ) || ( idx_ < 0 ) ) {
			M_THROW( _errMsgHStaticArray_[ ERROR::BAD_INDEX ], idx_ );
		}
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
		for ( size_type i( 0 ); i < N; ++ i ) {
			_data[i] = filler_;
		}
		return;
		M_EPILOG
	}
	void swap( HStaticArray& sa_ ) {
		M_PROLOG
		if ( &sa_ != this ) {
			using yaal::swap;
			for ( size_type i( 0 ); i < N; ++ i ) {
				swap( _data[i], sa_._data[i] );
			}
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
		if ( count != N ) {
			M_THROW( _errMsgHStaticArray_[ ERROR::BAD_SEQUENCE_SIZE ], count );
		}
		for ( size_type i( 0 ); first_ != last_; ++ first_, ++ i ) {
			_data[i] = *first_;
		}
		return;
		M_EPILOG
	}
	template<typename iterator_t>
	void assign( iterator_t first_, size_type count_ ) {
		M_PROLOG
		if ( count_ != N ) {
			M_THROW( _errMsgHStaticArray_[ ERROR::COUNT_NOT_MATCH ], count_ );
		}
		for ( size_type i( 0 ); i < count_; ++ first_, ++ i ) {
			_data[i] = *first_;
		}
		return;
		M_EPILOG
	}
	bool operator == ( HStaticArray const& sa_ ) const {
		M_PROLOG
		size_type i( 0 );
		for ( ; i < N && ( _data[i] == sa_._data[i] ); ++ i ) {
			;
		}
		return ( i == N );
		M_EPILOG
	}
};

template<typename type_t, int const N>
inline void swap( yaal::hcore::HStaticArray<type_t, N>& a, yaal::hcore::HStaticArray<type_t, N>& b )
	{ a.swap( b ); }

}

}

#endif /* #ifndef YAAL_HCORE_HSTATICARRAY_HXX_INCLUDED */

