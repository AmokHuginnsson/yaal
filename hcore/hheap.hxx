/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hcore/hheap.hxx - this file is integral part of `yaal' project.

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
/*! \file hcore/hheap.hxx
 * \brief Declaration of HHeap<> class template.
 */

#ifndef YAAL_HCORE_HHEAP_HXX_INCLUDED
#define YAAL_HCORE_HHEAP_HXX_INCLUDED 1

#include "hcore/harray.hxx"

namespace yaal {

namespace hcore {

/*! \brief HHeap<> - a heap concept implementation.
 */
template<typename type_t, template <typename> class sequence_t = HArray>
class HHeap {
	typedef sequence_t<type_t> sequence_type;
public:
	typedef typename sequence_type::value_type value_type;
private:
	sequence_type _sequence;
public:
	HHeap( void ) : _sequence() {}
	HHeap( HHeap const& heap_ ) : _sequence( heap_._sequence ) {}
	template<typename iter_t>
	HHeap( iter_t first_, iter_t last_ ) : _sequence( first_, last_ ) {
		M_PROLOG
		make_heap( _sequence.begin(), _sequence.end() );
		M_EPILOG
	}
	bool is_empty( void ) const {
		M_PROLOG
		return ( _sequence.is_empty() );
		M_EPILOG
	}
	bool empty( void ) const {
		M_PROLOG
		return ( _sequence.is_empty() );
		M_EPILOG
	}
	bool get_size( void ) const {
		M_PROLOG
		return ( _sequence.get_size() );
		M_EPILOG
	}
	bool size( void ) const {
		M_PROLOG
		return ( _sequence.get_size() );
		M_EPILOG
	}
	bool clear( void ) {
		M_PROLOG
		return ( _sequence.clear() );
		M_EPILOG
	}
	void push( value_type const& value_ ) {
		M_PROLOG
		_sequence.push_back( value_ );
		push_heap( _sequence.begin(), _sequence.end() );
		M_EPILOG
	}
	void pop( void ) {
		M_PROLOG
		pop_heap( _sequence.begin(), _sequence.end() );
		_sequence.pop_back();
		M_EPILOG
	}
	value_type const& top( void ) const {
		M_PROLOG
		M_ASSERT( ! is_empty() );
		return ( *_sequence.begin() );
		M_EPILOG
	}
};

}

}

#endif /* #ifndef YAAL_HCORE_HHEAP_HXX_INCLUDED */

