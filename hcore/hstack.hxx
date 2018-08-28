/* Read yaal/LICENSE.md file for copyright and licensing information. */
/*! \file hcore/hstack.hxx
 * \brief HStack<> class template implementation.
 */

#ifndef YAAL_HCORE_HSTACK_HXX_INCLUDED
#define YAAL_HCORE_HSTACK_HXX_INCLUDED 1

#include "hcore/harray.hxx"

namespace yaal {

namespace hcore {

/*! \brief HStack<> - a stack container adaptor.
 */
template<typename type_t, typename sequence_t = HArray<type_t> >
class HStack final {
public:
	typedef HStack<type_t, sequence_t> this_type;
private:
	typedef sequence_t sequence_type;
public:
	typedef typename sequence_type::value_type value_type;
	typedef typename sequence_type::size_type size_type;
private:
	sequence_type _sequence;
public:
	HStack( void ) : _sequence() {}
	HStack( HStack const& stack_ ) : _sequence( stack_._sequence ) {}
	HStack& operator = ( HStack const& stack_ ) {
		M_PROLOG
		if ( &stack_ != this ) {
			HStack tmp( stack_ );
			swap( tmp );
		}
		return ( *this );
		M_EPILOG
	}
	void swap( HStack& stack_ ) {
		if ( &stack_ != this ) {
			using yaal::swap;
			swap( _sequence, stack_._sequence );
		}
		return;
	}
	bool is_empty( void ) const {
		return ( _sequence.is_empty() );
	}
	bool empty( void ) const {
		return ( _sequence.is_empty() );
	}
	size_type get_size( void ) const {
		return ( _sequence.get_size() );
	}
	size_type size( void ) const {
		return ( _sequence.get_size() );
	}
	value_type const& top( void ) const {
		M_PROLOG
		return ( _sequence.back() );
		M_EPILOG
	}
	value_type& top( void ) {
		M_PROLOG
		return ( _sequence.back() );
		M_EPILOG
	}
	void push( value_type const& value_ ) {
		M_PROLOG
		_sequence.push_back( value_ );
		return;
		M_EPILOG
	}
	void push( value_type&& value_ ) {
		M_PROLOG
		_sequence.push_back( yaal::move( value_ ) );
		return;
		M_EPILOG
	}
	template<typename... arg_t>
	void emplace( arg_t&&... arg_ ) {
		M_PROLOG
		_sequence.emplace_back( yaal::forward<arg_t>( arg_ )... );
		return;
		M_EPILOG
	}
	void pop( void ) {
		M_PROLOG
		_sequence.pop_back();
		return;
		M_EPILOG
	}
	void clear( void ) {
		M_PROLOG
		_sequence.clear();
		return;
		M_EPILOG
	}
	bool operator == ( HStack const& s_ ) const {
		M_PROLOG
		return ( ( &s_ == this ) || safe_equal( _sequence.begin(), _sequence.end(), s_._sequence.begin(), s_._sequence.end() ) );
		M_EPILOG
	}
	bool operator < ( HStack const& s_ ) const {
		M_PROLOG
		return ( ( &s_ != this ) && lexicographical_compare( _sequence.begin(), _sequence.end(), s_._sequence.begin(), s_._sequence.end() ) );
		M_EPILOG
	}
};

}

}

#endif /* #ifndef YAAL_HCORE_HSTACK_HXX_INCLUDED */

