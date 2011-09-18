/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hcore/hqueue.hxx - this file is integral part of `yaal' project.

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
/*! \file hcore/hqueue.hxx
 * \brief HQueue<> class template implementataion.
 */

#ifndef YAAL_HCORE_HQUEUE_HXX_INCLUDED
#define YAAL_HCORE_HQUEUE_HXX_INCLUDED 1

#include "hcore/hdeque.hxx"

namespace yaal {

namespace hcore {

/*! \brief HQueue<> - a queue container adaptor.
 */
template<typename type_t, template <typename> class sequence_t = HDeque>
class HQueue {
	typedef sequence_t<type_t> sequence_type;
public:
	typedef typename sequence_type::value_type value_type;
private:
	sequence_type _sequence;
public:
	HQueue( void ) : _sequence() {}
	HQueue( HQueue const& stack_ ) : _sequence( stack_._sequence ) {}
	HQueue& operator = ( HQueue const& stack_ ) {
		M_PROLOG
		if ( &stack_ != this ) {
			HQueue tmp( stack_ );
			swap( tmp );
		}
		return ( *this );
		M_EPILOG
	}
	void swap( HQueue& stack_ ) {
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
	int long get_size( void ) const {
		return ( _sequence.get_size() );
	}
	int long size( void ) const {
		return ( _sequence.get_size() );
	}
	value_type const& front( void ) const {
		M_PROLOG
		return ( _sequence.front() );
		M_EPILOG
	}
	value_type& front( void ) {
		M_PROLOG
		return ( _sequence.front() );
		M_EPILOG
	}
	value_type const& back( void ) const {
		M_PROLOG
		return ( _sequence.back() );
		M_EPILOG
	}
	value_type& back( void ) {
		M_PROLOG
		return ( _sequence.back() );
		M_EPILOG
	}
	void push( value_type const& value_ ) {
		M_PROLOG
		_sequence.push_front( value_ );
		return;
		M_EPILOG
	}
	void pop( void ) {
		M_PROLOG
		_sequence.pop_back();
		return;
		M_EPILOG
	}
	bool operator == ( HQueue const& s_ ) const {
		M_PROLOG
		return ( ( &s_ == this ) || safe_equal( _sequence.begin(), _sequence.end(), s_._sequence.begin(), s_._sequence.end() ) );
		M_EPILOG
	}
	bool operator < ( HQueue const& s_ ) const {
		M_PROLOG
		return ( ( &s_ != this ) && lexicographical_compare( _sequence.begin(), _sequence.end(), s_._sequence.begin(), s_._sequence.end() ) );
		M_EPILOG
	}
};

}

}

#endif /* #ifndef YAAL_HCORE_HQUEUE_HXX_INCLUDED */

