/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	tools/hsignal.hxx - this file is integral part of `yaal' project.

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
/*! \file tools/hsignal.hxx
 * \brief Declaration and implementation of HSignal<> class template.
 */

#ifndef YAAL_TOOLS_HSIGNAL_HXX_INCLUDED
#define YAAL_TOOLS_HSIGNAL_HXX_INCLUDED 1

#include "hcore/hboundcall.hxx"
#include "hcore/harray.hxx"

namespace yaal {

namespace tools {

namespace result_agregator {

template<typename return_t>
class HLast {
public:
	typedef return_t result_type;
	template<typename iterator>
	result_type operator()( iterator first_, iterator last_ ) {
		if ( first_ == last_ ) {
			return ( result_type() );
		}
		result_type result;
		for ( ; first_ != last_; ++ first_ ) {
			result = *first_;
		}
		return ( result );
	}
};

template<>
class HLast<void> {
public:
	typedef void result_type;
	template<typename iterator>
	void operator()( iterator first_, iterator last_ ) {
		for ( ; first_ != last_; ++ first_ ) {
			*first_;
		}
	}
};

}

template<typename R, typename call_t, typename... arg_t>
R callback( call_t call_, arg_t... arg_ ) {
	return ( call_( arg_... ) );
}

/*! \brief Implementation of signals and slot system.
 */
template<typename signature_t,
	typename result_agregator_t = result_agregator::HLast<typename trait::return_type<signature_t>::type> >
class HSignal {
public:
	typedef HSignal<signature_t> this_type;
	typedef yaal::hcore::HBoundCall<signature_t> call_t;
	class HSlot;
	typedef yaal::hcore::HArray<HSlot> slots_t;
	typedef result_agregator_t result_agregator;
	typedef typename trait::return_type<signature_t>::type result_type;
	typedef typename result_agregator_t::result_type agregated_result_type;
private:
	template<typename callback_t>
	class HIterator;
	slots_t _slots;
public:
	void connect( typename HSlot::call_t slot_ ) {
		_slots.push_back( slot_ );
	}
	template<typename... arg_t>
	result_type operator()( arg_t&&... arg_ ) {
		M_PROLOG
		typedef yaal::hcore::HBoundCall<result_type ( call_t& )> callback_t;
		callback_t c( call( static_cast<result_type ( call_t::* )( arg_t&&... )>( &HSlot::call_t::operator() ), hcore::_1, arg_... ) );
		return ( result_agregator()( HIterator<callback_t>( _slots.begin(), c, this ), HIterator<callback_t>( _slots.end(), c, this ) ) );
		M_EPILOG
	}
};

template<typename signature_t, typename result_agregator_t>
class HSignal<signature_t, result_agregator_t>::HSlot {
public:
	typedef typename HSignal<signature_t, result_agregator_t>::call_t call_t;
private:
	call_t _call;
public:
	HSlot( call_t call_ )
		: _call( yaal::move( call_ ) ) {
		return;
	}
private:
	template<typename callback_t>
	friend class HSignal<signature_t, result_agregator_t>::HIterator;
};

template<typename signature_t, typename result_agregator_t>
template<typename callback_t>
class HSignal<signature_t, result_agregator_t>::HIterator : public yaal::hcore::iterator_interface<typename trait::return_type<result_agregator_t>::type, yaal::hcore::iterator_category::forward> {
public:
	typedef HSignal<signature_t, result_agregator_t>::HIterator<callback_t> this_type;
	typedef yaal::hcore::iterator_interface<typename trait::return_type<result_agregator_t>::type, yaal::hcore::iterator_category::forward> base_type;
	typedef typename trait::return_type<signature_t>::type result_type;
	typedef typename HSignal<signature_t, result_agregator_t>::slots_t slots_t;
private:
	typedef HSignal<signature_t, result_agregator_t> owner_t;
	typename slots_t::iterator _it;
	callback_t _callback;
	owner_t const* _owner;
public:
	result_type operator* ( void ) {
		return ( _callback( _it->_call ) );
	}
	bool operator == ( HIterator const it_ ) const {
		M_ASSERT( it_._owner == _owner );
		return ( it_._it == _it );
	}
	bool operator != ( HIterator const it_ ) const {
		M_ASSERT( it_._owner == _owner );
		return ( it_._it != _it );
	}
	HIterator& operator ++ ( void ) {
		++ _it;
		return ( *this );
	}
private:
	HIterator( typename slots_t::iterator it_, callback_t callback_, owner_t const* owner_  )
		: _it( it_ ), _callback( callback_ ), _owner( owner_ ) {
	}
	friend class HSignal<signature_t, result_agregator_t>;
};

}

}

#endif /* #ifndef YAAL_TOOLS_HSIGNAL_HXX_INCLUDED */

