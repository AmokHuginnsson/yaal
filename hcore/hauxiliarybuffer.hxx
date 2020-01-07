/* Read yaal/LICENSE.md file for copyright and licensing information. */
/*! \file hcore/hauxiliarybuffer.hxx
 * \brief Implementation of HAuxiliaryBuffer<> class template.
 */

#ifndef YAAL_HCORE_HAUXILIARYBUFFER_HXX_INCLUDED
#define YAAL_HCORE_HAUXILIARYBUFFER_HXX_INCLUDED 1

#include "hcore/algorithm_low.hxx"
#include "hcore/memory.hxx"
#include "hcore/iterator.hxx"

namespace yaal {

namespace hcore {

namespace auxiliary_buffer {
i64_t available_memory( void );
}

/*! \brief Helper class for adaptive algorithms.
 */
template<typename type_t>
class HAuxiliaryBuffer {
public:
	typedef HAuxiliaryBuffer<type_t> this_type;
	typedef type_t value_type;
private:
	value_type* _data;
	i64_t _size;
	i64_t _requestedSize;
	i64_t _allocated;
	bool _initialized;
public:
	template<typename iter_t>
	HAuxiliaryBuffer( iter_t first_, iter_t last_ )
		: _data( nullptr )
		, _size( 0 )
		, _requestedSize( 0 )
		, _allocated( 0 )
		, _initialized( false ) {
		init( first_, last_ );
		return;
	}
	~HAuxiliaryBuffer( void ) {
		M_PROLOG
		clear();
		::operator delete ( _data, memory::yaal );
		return;
		M_DESTRUCTOR_EPILOG
	}
	value_type* begin( void ) {
		return ( _initialized ? _data : nullptr );
	}
	value_type* end( void ) {
		return ( _initialized ? _data + _size : nullptr );
	}
	i64_t get_size( void ) const {
		return ( _initialized ? _size : 0 );
	}
	i64_t get_requested_size( void ) const {
		return ( _initialized ? _requestedSize : 0 );
	}
	template<typename iter_t>
	void init( iter_t, iter_t );
	void clear( void ) {
		if ( ! _initialized ) {
			return;
		}
		for ( value_type* it( begin() ), * endIt( end() ); it != endIt; ++ it ) {
			M_SAFE( it->~value_type() );
		}
		_initialized = false;
	}
private:
	HAuxiliaryBuffer( HAuxiliaryBuffer const& ) = delete;
	HAuxiliaryBuffer& operator = ( HAuxiliaryBuffer const& ) = delete;
};

template<typename type_t>
template<typename iter_t>
void HAuxiliaryBuffer<type_t>::init( iter_t first_, iter_t last_ ) {
	using yaal::distance;
	_requestedSize = distance( first_, last_, typename hcore::iterator_traits<iter_t>::category_type() );
	i64_t canCopy( _allocated );
	if ( _requestedSize > _allocated ) {
		i64_t newCanCopy( auxiliary_buffer::available_memory() / static_cast<int>( sizeof ( value_type ) ) );
		if ( newCanCopy > canCopy ) {
			canCopy = newCanCopy;
		}
	}
	i64_t auxSize( min( _requestedSize, canCopy ) );
	clear();
	if ( auxSize <= 0 ) {
		return;
	}
	if ( auxSize > _allocated ) {
		if ( _allocated > 0 ) {
			::operator delete ( _data, memory::yaal );
			_data = nullptr;
			_allocated = 0;
		}
		_data = static_cast<value_type*>( ::operator new ( static_cast<size_t>( auxSize * static_cast<int>( sizeof ( value_type ) ) ), memory::yaal ) );
		_allocated = auxSize;
	}
	_size = auxSize;
	for ( value_type* it( _data ), * endIt( _data + _size ); it != endIt; ++ it, ++ first_ ) {
		new ( it ) value_type( *first_ );
	}
	_initialized = true;
	return;
}

}

}

#endif /* #ifndef YAAL_HCORE_HAUXILIARYBUFFER_HXX_INCLUDED */

