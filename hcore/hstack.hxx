/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hcore/hstack.hxx - this file is integral part of `yaal' project.

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
/*! \file hcore/hstack.hxx
 * \brief HStack<> class template implementataion.
 */

#ifndef YAAL_HCORE_HSTACK_HXX_INCLUDED
#define YAAL_HCORE_HSTACK_HXX_INCLUDED 1

#include "hcore/hdeque.hxx"

namespace yaal {

namespace hcore {

/*! \brief HStack<> - a stack container adaptor.
 */
template<typename type_t, typename sequence_t = HDeque<type_t> >
class HStack {
public:
	typedef HStack<type_t, sequence_t> this_type;
private:
	typedef sequence_t sequence_type;
public:
	typedef typename sequence_type::value_type value_type;
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
	int long get_size( void ) const {
		return ( _sequence.get_size() );
	}
	int long size( void ) const {
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
	void pop( void ) {
		M_PROLOG
		_sequence.pop_back();
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

