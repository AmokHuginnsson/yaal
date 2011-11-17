/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hcore/hauxiliarybuffer.hxx - this file is integral part of `yaal' project.

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
/*! \file hcore/hauxiliarybuffer.hxx
 * \brief Implementation of HAuxiliaryBuffer<> class template.
 */

#ifndef YAAL_HCORE_HAUXILIARYBUFFER_HXX_INCLUDED
#define YAAL_HCORE_HAUXILIARYBUFFER_HXX_INCLUDED 1

#include <new>

#include "hcore/algorithm_low.hxx"
#include "hcore/iterator.hxx"
#include "hcore/system.hxx"

namespace yaal {

namespace hcore {

/*! \brief Helper class for adaptive algorithms.
 */
template<typename type_t>
class HAuxiliaryBuffer {
public:
	typedef HAuxiliaryBuffer<type_t> this_type;
	typedef type_t value_type;
private:
	value_type* _data;
	int long _size;
	int long _requestedSize;
	int long _allocated;
	bool _initialized;
public:
	template<typename iter_t>
	HAuxiliaryBuffer( iter_t, iter_t );
	~HAuxiliaryBuffer( void );
	value_type* begin( void );
	value_type* end( void );
	int long get_size( void ) const;
	int long get_requested_size( void ) const;
	template<typename iter_t>
	void init( iter_t, iter_t );
	void clear( void );
};

template<typename type_t>
template<typename iter_t>
HAuxiliaryBuffer<type_t>::HAuxiliaryBuffer( iter_t first_, iter_t last_ )
	: _data( NULL ), _size( 0 ), _requestedSize( 0 ), _allocated( 0 ), _initialized( false ) {
	init( first_, last_ );
	return;
}

template<typename type_t>
template<typename iter_t>
void HAuxiliaryBuffer<type_t>::init( iter_t first_, iter_t last_ ) {
	using yaal::distance;
	_requestedSize = distance( first_, last_, typename hcore::iterator_traits<iter_t>::category_type() );
	int long canCopy( _allocated );
	if ( _requestedSize > _allocated ) {
		int long newCanCopy( static_cast<int long>( hcore::system::get_memory_size_info().available() ) / sizeof ( value_type ) );
		if ( newCanCopy > canCopy )
			canCopy = newCanCopy;
	}
	int long auxSize( min( _requestedSize, canCopy ) );
	/* Only 1 element in auxiliary buffer is equivalent
	 * to pure inplace implementation, so it does not make sense to go with aux. */
	clear();
	if ( auxSize > 1 ) {
		if ( auxSize > _allocated ) {
			if ( _allocated > 0 ) {
				delete _data;
				_data = NULL;
				_allocated = 0;
			}
			_data = static_cast<value_type*>( operator new ( auxSize * sizeof ( value_type ) ) );
			_allocated = auxSize;
		}
		_size = auxSize;
		for ( value_type* it( _data ), * endIt( _data + _size ); it != endIt; ++ it, ++ first_ )
			new ( it ) value_type( *first_ );
		_initialized = true;
	}
	return;
}

template<typename type_t>
HAuxiliaryBuffer<type_t>::~HAuxiliaryBuffer( void ) {
	M_PROLOG
	clear();
	delete _data;
	return;
	M_DESTRUCTOR_EPILOG
}

template<typename type_t>
void HAuxiliaryBuffer<type_t>::clear( void ) {
	if ( _initialized ) {
		for ( value_type* it( begin() ), * endIt( end() ); it != endIt; ++ it )
			M_SAFE( it->~value_type() );
		_initialized = false;
	}
}

template<typename type_t>
typename HAuxiliaryBuffer<type_t>::value_type* HAuxiliaryBuffer<type_t>::begin( void ) {
	return ( _initialized ? _data : NULL );
}

template<typename type_t>
typename HAuxiliaryBuffer<type_t>::value_type* HAuxiliaryBuffer<type_t>::end( void ) {
	return ( _initialized ? _data + _size : NULL );
}

template<typename type_t>
int long HAuxiliaryBuffer<type_t>::get_size( void ) const {
	return ( _initialized ? _size : 0 );
}

template<typename type_t>
int long HAuxiliaryBuffer<type_t>::get_requested_size( void ) const {
	return ( _initialized ? _requestedSize : 0 );
}

}

}

#endif /* #ifndef YAAL_HCORE_HAUXILIARYBUFFER_HXX_INCLUDED */

