/* Read yaal/LICENSE.md file for copyright and licensing information. */
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
template<typename type_t, typename sequence_t = HArray<type_t> >
class HHeap {
public:
	typedef HHeap<type_t, sequence_t> this_type;
private:
	typedef sequence_t sequence_type;
public:
	typedef typename sequence_type::value_type value_type;
	typedef typename sequence_type::size_type size_type;
private:
	sequence_type _sequence;
public:
	HHeap( void ) : _sequence() {}
	HHeap( HHeap const& heap_ ) : _sequence( heap_._sequence ) {}
	template<typename iter_t>
	HHeap( iter_t first_, iter_t last_ ) : _sequence( first_, last_ ) {
		M_PROLOG
		make_heap( _sequence.begin(), _sequence.end() );
		return;
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
	size_type get_size( void ) const {
		M_PROLOG
		return ( _sequence.get_size() );
		M_EPILOG
	}
	size_type size( void ) const {
		M_PROLOG
		return ( _sequence.get_size() );
		M_EPILOG
	}
	void clear( void ) {
		M_PROLOG
		_sequence.clear();
		return;
		M_EPILOG
	}
	void push( value_type const& value_ ) {
		M_PROLOG
		_sequence.push_back( value_ );
		push_heap( _sequence.begin(), _sequence.end() );
		return;
		M_EPILOG
	}
	void pop( void ) {
		M_PROLOG
		pop_heap( _sequence.begin(), _sequence.end() );
		_sequence.pop_back();
		return;
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

