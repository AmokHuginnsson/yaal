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
template<typename type_t, typename sequence_t = HArray<type_t>, typename compare_t = less<type_t>>
class HHeap {
public:
	typedef HHeap<type_t, sequence_t, compare_t> this_type;
	typedef sequence_t sequence_type;
	typedef compare_t compare_type;
	typedef typename sequence_type::value_type value_type;
	typedef typename sequence_type::size_type size_type;
private:
	sequence_type _sequence;
	compare_type _compare;
public:
	HHeap( void )
		: _sequence()
		, _compare() {
	}
	HHeap( HHeap const& ) = default;
	HHeap( HHeap&& ) = default;
	explicit HHeap( compare_type const& compare_ )
		: _sequence()
		, _compare( compare_ ) {
		return;
	}
	template<typename iter_t>
	HHeap( iter_t first_, iter_t last_ )
		: _sequence( first_, last_ )
		, _compare() {
		M_PROLOG
		make_heap( _sequence.begin(), _sequence.end(), _compare );
		return;
		M_EPILOG
	}
	HHeap& operator = ( HHeap const& ) = default;
	HHeap& operator = ( HHeap&& ) = default;
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
		push_heap<typename sequence_type::iterator, compare_type>( _sequence.begin(), _sequence.end(), _compare );
		return;
		M_EPILOG
	}
	void pop( void ) {
		M_PROLOG
		pop_heap<typename sequence_type::iterator, compare_type>( _sequence.begin(), _sequence.end(), _compare );
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
	void swap( HHeap& other_ ) {
		M_PROLOG
		using yaal::swap;
		swap( _sequence, other_._sequence );
		swap( _compare, other_._compare );
		return;
		M_EPILOG
	}
};

}

}

#endif /* #ifndef YAAL_HCORE_HHEAP_HXX_INCLUDED */

