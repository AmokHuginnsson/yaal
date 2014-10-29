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

/*! \brief Implementation of signals and slot system.
 */
template<typename signature_t,
	typename result_agregator_t = result_agregator::HLast<typename trait::return_type<signature_t>::type> >
class HSignal {
public:
	typedef HSignal<signature_t> this_type;
	typedef result_agregator_t result_agregator;
	typedef typename trait::return_type<signature_t>::type result_type;
	typedef typename result_agregator_t::result_type agregated_result_type;
	typedef yaal::hcore::HBoundCall<signature_t> call_t;
	typedef yaal::hcore::HBoundCall<result_type ( call_t& )> callback_t;
	class HSlot;
	class HConnection;
	typedef yaal::hcore::HPointer<HSlot> slot_t;
	typedef yaal::hcore::HArray<slot_t> slots_t;
private:
	template<typename callback_t>
	class HIterator;
	slots_t _slots;
public:
	virtual ~HSignal( void ) {
		for ( slot_t& slot : _slots ) {
			slot->disown();
		}
	}
	HConnection connect( typename HSlot::call_t slot_ ) {
		M_PROLOG
		slot_t slot( hcore::make_pointer<HSlot>( slot_, this ) );
		_slots.push_back( slot );
		return ( HConnection( slot ) );
		M_EPILOG
	}
	template<typename... arg_t>
	result_type operator()( arg_t&&... arg_ ) {
		M_PROLOG
		callback_t c( call( static_cast<result_type ( call_t::* )( arg_t&&... )>( &HSlot::call_t::operator() ), hcore::_1, arg_... ) );
		return ( result_agregator()( HIterator<callback_t>( _slots.begin(), c, this ), HIterator<callback_t>( _slots.end(), c, this ) ) );
		M_EPILOG
	}
	void swap( HSignal& signal_ ) {
		if ( &signal_ != this ) {
			using yaal::swap;
			swap( _slots, signal_._slots );
		}
		return;
	}
private:
	void disconnect( HSlot* slot_ ) {
		M_PROLOG
		typename slots_t::iterator it( find( _slots.begin(), _slots.end(), slot_ ) );
		M_ENSURE( it != _slots.end() );
		(*it)->disown();
		_slots.erase( it );
		return;
		M_EPILOG
	}
	friend class HSlot;
};

template<typename signature_t, typename result_agregator_t>
class HSignal<signature_t, result_agregator_t>::HSlot {
public:
	typedef HSignal<signature_t, result_agregator_t> signal_t;
	typedef typename signal_t::call_t call_t;
	typedef typename signal_t::callback_t callback_t;
	typedef typename signal_t::result_type result_type;
private:
	call_t _call;
	signal_t* _owner;
	bool _enabled;
public:
	HSlot( call_t call_, signal_t* owner_ )
		: _call( yaal::move( call_ ) ), _owner( owner_ ), _enabled( true ) {
		return;
	}
	result_type call( callback_t callback_ ) {
		M_PROLOG
		return ( callback_( _call ) );
		M_EPILOG
	}
	void disable( void ) {
		M_PROLOG
		_enabled = false;
		return;
		M_EPILOG
	}
	void enable( void ) {
		M_PROLOG
		_enabled = true;
		return;
		M_EPILOG
	}
	void disconnect( void ) {
		M_PROLOG
		if ( _owner != nullptr ) {
			_owner->disconnect( this );
		}
		return;
		M_EPILOG
	}
	bool is_enabled( void ) const {
		return ( ( _owner != nullptr ) && _enabled );
	}
	void disown( void ) {
		M_PROLOG
		_owner = nullptr;
		return;
		M_EPILOG
	}
private:
	template<typename callback_t>
	friend class HSignal<signature_t, result_agregator_t>::HIterator;
};

template<typename signature_t, typename result_agregator_t>
class HSignal<signature_t, result_agregator_t>::HConnection {
public:
	typedef HSignal<signature_t, result_agregator_t>::HConnection this_type;
	typedef typename HSignal<signature_t, result_agregator_t>::slot_t slot_t;
private:
	slot_t _slot;
public:
	HConnection( slot_t slot_ )
		: _slot( slot_ ) {
	}
	void disable( void ) {
		M_PROLOG
		_slot->disable();
		return;
		M_EPILOG
	}
	void enable( void ) {
		M_PROLOG
		_slot->enable();
		return;
		M_EPILOG
	}
	void disconnect( void ) {
		M_PROLOG
		_slot->disconnect();
		return;
		M_EPILOG
	}
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
		return ( (*_it)->call( _callback ) );
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
		do {
			++ _it;
		} while ( ( _it != _owner->_slots.end() ) && ! (*_it)->is_enabled() );
		return ( *this );
	}
private:
	HIterator( typename slots_t::iterator it_, callback_t callback_, owner_t const* owner_  )
		: _it( it_ ), _callback( callback_ ), _owner( owner_ ) {
	}
	friend class HSignal<signature_t, result_agregator_t>;
};

template<typename signature_t, typename result_agregator_t>
inline void swap( HSignal<signature_t, result_agregator_t>& a, HSignal<signature_t, result_agregator_t>& b ) {
	a.swap( b );
}

}

}

#endif /* #ifndef YAAL_TOOLS_HSIGNAL_HXX_INCLUDED */

