/* Read yaal/LICENSE.md file for copyright and licensing information. */
/*! \file tools/hsignal.hxx
 * \brief Declaration and implementation of HSignal<> class template.
 */

#ifndef YAAL_TOOLS_HSIGNAL_HXX_INCLUDED
#define YAAL_TOOLS_HSIGNAL_HXX_INCLUDED 1

#include "hcore/hboundcall.hxx"
#include "hcore/hlist.hxx"
#include "hcore/hmultimap.hxx"

namespace yaal {

namespace tools {

namespace signal {

namespace result_aggregator {

template<typename return_t>
class HLast {
public:
	typedef return_t result_type;
	template<typename iterator>
	result_type operator()( iterator first_, iterator last_ ) {
		if ( first_ == last_ ) {
			return ( result_type() );
		}
		result_type result = result_type();
		for ( ; first_ != last_; ++ first_ ) {
			result = *first_;
		}
		return result;
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

enum class POSITION {
	AT_FRONT,
	AT_BACK
};

}

/*! \brief Implementation of signals and slot system.
 */
template<typename signature_t,
	typename result_aggregator_t = signal::result_aggregator::HLast<typename trait::return_type<signature_t>::type>,
	typename group_by = int>
class HSignal final {
public:
	typedef HSignal<signature_t> this_type;
	typedef result_aggregator_t result_aggregator;
	typedef typename trait::return_type<signature_t>::type result_type;
	typedef typename result_aggregator_t::result_type aggregated_result_type;
	typedef yaal::hcore::HBoundCall<signature_t> call_t;
	typedef yaal::hcore::HBoundCall<result_type ( call_t& )> callback_t;
	class HSlot;
	class HConnection;
	typedef yaal::hcore::HPointer<HSlot> slot_t;
	typedef yaal::hcore::HList<slot_t> slots_t;
	typedef yaal::hcore::HMultiMap<group_by, slot_t> ordered_slots_t;
private:
	template<typename callback_t>
	class HIterator;
	slots_t _slotsPre;
	ordered_slots_t _slotsPrio;
	slots_t _slotsPost;
public:
	HSignal( void )
		: _slotsPre(), _slotsPrio(), _slotsPost() {
		return;
	}
	~HSignal( void ) {
		for ( slot_t& slot : _slotsPre ) {
			slot->disown();
		}
		for ( typename ordered_slots_t::value_type& slot : _slotsPrio ) {
			slot.second->disown();
		}
		for ( slot_t& slot : _slotsPost ) {
			slot->disown();
		}
	}
	HConnection connect( typename HSlot::call_t slot_, signal::POSITION position_ = signal::POSITION::AT_BACK ) {
		M_PROLOG
		slot_t slot( hcore::make_pointer<HSlot>( slot_, this ) );
		if ( position_ == signal::POSITION::AT_BACK ) {
			_slotsPost.push_back( slot );
		} else {
			_slotsPre.push_front( slot );
		}
		return ( HConnection( slot ) );
		M_EPILOG
	}
	HConnection connect( group_by groupBy_, typename HSlot::call_t slot_, signal::POSITION position_ = signal::POSITION::AT_BACK ) {
		M_PROLOG
		slot_t slot( hcore::make_pointer<HSlot>( slot_, this ) );
		if ( position_ == signal::POSITION::AT_BACK ) {
			_slotsPrio.push_back( make_pair( groupBy_, slot ) );
		} else {
			_slotsPrio.push_front( make_pair( groupBy_, slot ) );
		}
		return ( HConnection( slot ) );
		M_EPILOG
	}
	template<typename... arg_t>
	result_type operator()( arg_t&&... arg_ ) {
		M_PROLOG
		callback_t c( call( static_cast<result_type ( call_t::* )( arg_t&&... )>( &HSlot::call_t::operator() ), hcore::_1, yaal::forward<arg_t>( arg_ )... ) );
		return ( result_aggregator()(
					HIterator<callback_t>( _slotsPre.begin(), c, this, HIterator<callback_t>::STATE::PRE ),
					HIterator<callback_t>( _slotsPost.end(), c, this, HIterator<callback_t>::STATE::POST ) ) );
		M_EPILOG
	}
	void swap( HSignal& signal_ ) {
		if ( &signal_ != this ) {
			using yaal::swap;
			swap( _slotsPre, signal_._slotsPre );
			swap( _slotsPrio, signal_._slotsPrio );
			swap( _slotsPost, signal_._slotsPost );
		}
		return;
	}
private:
	void disconnect( HSlot* slot_ ) {
		M_PROLOG
		typename slots_t::iterator it( find( _slotsPre.begin(), _slotsPre.end(), slot_ ) );
		if ( it != _slotsPre.end() ) {
			(*it)->disown();
			_slotsPre.erase( it );
		} else {
			it = find( _slotsPost.begin(), _slotsPost.end(), slot_ );
			if ( it != _slotsPost.end() ) {
				_slotsPost.erase( it );
			} else {
				typename ordered_slots_t::iterator itPrio(
						find_if( _slotsPrio.begin(), _slotsPrio.end(),
							[slot_]( typename ordered_slots_t::value_type const& val_ ) {
								return ( val_.second == slot_ );
							}
						)
				);
				M_ENSURE( itPrio != _slotsPrio.end() );
				_slotsPrio.erase( itPrio );
			}
		}
		return;
		M_EPILOG
	}
	friend class HSlot;
};

template<typename signature_t, typename result_aggregator_t, typename group_by>
class HSignal<signature_t, result_aggregator_t, group_by>::HSlot {
public:
	typedef HSignal<signature_t, result_aggregator_t, group_by> signal_t;
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
	bool is_valid( void ) const {
		return ( _owner != nullptr );
	}
	bool is_enabled( void ) const {
		return ( is_valid() && _enabled );
	}
	void disown( void ) {
		M_PROLOG
		_owner = nullptr;
		return;
		M_EPILOG
	}
private:
	HSlot( HSlot const& ) = delete;
	HSlot& operator = ( HSlot const& ) = delete;
	template<typename callback_t>
	friend class HSignal<signature_t, result_aggregator_t, group_by>::HIterator;
};

template<typename signature_t, typename result_aggregator_t, typename group_by>
class HSignal<signature_t, result_aggregator_t, group_by>::HConnection {
public:
	typedef typename HSignal<signature_t, result_aggregator_t, group_by>::HConnection this_type;
	typedef typename HSignal<signature_t, result_aggregator_t, group_by>::slot_t slot_t;
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

template<typename signature_t, typename result_aggregator_t, typename group_by>
template<typename callback_t>
class HSignal<signature_t, result_aggregator_t, group_by>::HIterator
	: public yaal::hcore::iterator_interface<typename trait::return_type<result_aggregator_t>::type, yaal::hcore::iterator_category::forward> {
public:
	typedef typename HSignal<signature_t, result_aggregator_t, group_by>::template HIterator<callback_t> this_type;
	typedef yaal::hcore::iterator_interface<typename trait::return_type<result_aggregator_t>::type, yaal::hcore::iterator_category::forward> base_type;
	typedef typename trait::return_type<signature_t>::type result_type;
	typedef typename HSignal<signature_t, result_aggregator_t, group_by>::slots_t slots_t;
	typedef typename HSignal<signature_t, result_aggregator_t, group_by>::ordered_slots_t ordered_slots_t;
	enum class STATE {
		PRE,
		PRIO,
		POST
	};
private:
	typedef HSignal<signature_t, result_aggregator_t, group_by> owner_t;
	typename slots_t::iterator _it;
	typename ordered_slots_t::iterator _itPrio;
	callback_t _callback;
	owner_t* _owner;
	STATE _state;
public:
	HIterator( HIterator const& ) = default;
	HIterator& operator = ( HIterator const& ) = default;
	result_type operator* ( void ) {
		if ( ( _state == STATE::PRE ) || ( _state == STATE::POST ) ) {
			return ( (*_it)->call( _callback ) );
		}
		return ( _itPrio->second->call( _callback ) );
	}
	bool operator == ( HIterator const it_ ) const {
		M_ASSERT( it_._owner == _owner );
		return ( ! operator != ( it_ ) );
	}
	bool operator != ( HIterator const it_ ) const {
		M_ASSERT( it_._owner == _owner );
		return ( ( it_._state != _state ) || ( ( _state == STATE::PRIO ) && ( it_._itPrio != _itPrio ) ) || ( ( _state != STATE::PRIO ) && ( it_._it != _it ) ) );
	}
	HIterator& operator ++ ( void ) {
		switch ( _state ) {
			case ( STATE::PRE ): {
				++ _it;
			} break;
			case ( STATE::PRIO ): {
				++ _itPrio;
			} break;
			case ( STATE::POST ): {
				++ _it;
			} break;
		}
		validate();
		return ( *this );
	}
private:
	void validate( void ) {
		M_PROLOG
		switch ( _state ) {
			case ( STATE::PRE ): {
				while ( ( _it != _owner->_slotsPre.end() ) && ! (*_it)->is_enabled() ) {
					++ _it;
				}
				if ( _it == _owner->_slotsPre.end() ) {
					_state = STATE::PRIO;
					_itPrio = _owner->_slotsPrio.begin();
					validate();
				}
			} break;
			case ( STATE::PRIO ): {
				while ( ( _itPrio != _owner->_slotsPrio.end() ) && ! _itPrio->second->is_enabled() ) {
					++ _itPrio;
				}
				if ( _itPrio == _owner->_slotsPrio.end() ) {
					_state = STATE::POST;
					_it = _owner->_slotsPost.begin();
					validate();
				}
			} break;
			case ( STATE::POST ): {
				while ( ( _it != _owner->_slotsPost.end() ) && ! (*_it)->is_enabled() ) {
					++ _it;
				}
			} break;
		}
		return;
		M_EPILOG
	}
	HIterator( typename slots_t::iterator it_, callback_t callback_, owner_t* owner_, STATE state_ )
		: _it( it_ ), _itPrio(), _callback( callback_ ), _owner( owner_ ), _state( state_ ) {
		validate();
		return;
	}
	friend class HSignal<signature_t, result_aggregator_t, group_by>;
};

template<typename signature_t, typename result_aggregator_t, typename group_by>
inline void swap( HSignal<signature_t, result_aggregator_t, group_by>& a, HSignal<signature_t, result_aggregator_t, group_by>& b ) {
	a.swap( b );
}

}

}

#endif /* #ifndef YAAL_TOOLS_HSIGNAL_HXX_INCLUDED */

