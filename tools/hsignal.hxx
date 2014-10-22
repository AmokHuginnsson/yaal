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
		result_type result();
		for ( ; first_ != last_; ++ first_ ) {
			result = *first_;
		}
		return ( result );
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
	class HSlot;
	typedef yaal::hcore::HArray<HSlot> slots_t;
	typedef result_agregator_t result_agregator;
	typedef typename result_agregator_t::result_type result_type;
private:
	class HIterator;
	slots_t _slots;
public:
	void connect( typename HSlot::call_t slot_ ) {
		_slots.push_back( slot_ );
	}
	result_type operator()( void ) const {
		M_PROLOG
		return ( result_agregator()( HIterator( _slots.begin() ), HIterator( _slots.end() ) ) );
		M_EPILOG
	}
	template<typename fa0_t>
	result_type operator()( fa0_t fa0 ) const {
		M_PROLOG
		return ( result_agregator()( HIterator( _slots.begin(), fa0 ),
					HIterator( _slots.end(), fa0 ) ) );
		M_EPILOG
	}
	template<typename fa0_t, typename fa1_t>
	result_type operator()( fa0_t fa0, fa1_t fa1 ) const {
		M_PROLOG
		return ( result_agregator()( HIterator( _slots.begin(), fa0, fa1 ),
					HIterator( _slots.end(), fa0, fa1 ) ) );
		M_EPILOG
	}
	template<typename fa0_t, typename fa1_t, typename fa2_t>
	result_type operator()( fa0_t fa0, fa1_t fa1, fa2_t fa2 ) const {
		M_PROLOG
		return ( result_agregator()( HIterator( _slots.begin(), fa0, fa1, fa2 ),
					HIterator( _slots.end(), fa0, fa1, fa2 ) ) );
		M_EPILOG
	}
	template<typename fa0_t, typename fa1_t, typename fa2_t, typename fa3_t>
	result_type operator()( fa0_t fa0, fa1_t fa1, fa2_t fa2, fa3_t fa3 ) const {
		M_PROLOG
		return ( result_agregator()( HIterator( _slots.begin(), fa0, fa1, fa2, fa3 ),
					HIterator( _slots.end(), fa0, fa1, fa2, fa3 ) ) );
		M_EPILOG
	}
	template<typename fa0_t, typename fa1_t, typename fa2_t, typename fa3_t, typename fa4_t>
	result_type operator()( fa0_t fa0, fa1_t fa1, fa2_t fa2, fa3_t fa3, fa4_t fa4 ) const {
		M_PROLOG
		return ( result_agregator()( HIterator( _slots.begin(), fa0, fa1, fa2, fa3, fa4 ),
					HIterator( _slots.end(), fa0, fa1, fa2, fa3, fa4 ) ) );
		M_EPILOG
	}
	template<typename fa0_t, typename fa1_t, typename fa2_t, typename fa3_t, typename fa4_t, typename fa5_t>
	result_type operator()( fa0_t fa0, fa1_t fa1, fa2_t fa2, fa3_t fa3, fa4_t fa4, fa5_t fa5 ) const {
		M_PROLOG
		return ( result_agregator()( HIterator( _slots.begin(), fa0, fa1, fa2, fa3, fa4, fa5 ),
					HIterator( _slots.end(), fa0, fa1, fa2, fa3, fa4, fa5 ) ) );
		M_EPILOG
	}
	template<typename fa0_t, typename fa1_t, typename fa2_t, typename fa3_t, typename fa4_t, typename fa5_t, typename fa6_t>
	result_type operator()( fa0_t fa0, fa1_t fa1, fa2_t fa2, fa3_t fa3, fa4_t fa4, fa5_t fa5, fa6_t fa6 ) const {
		M_PROLOG
		return ( result_agregator()( HIterator( _slots.begin(), fa0, fa1, fa2, fa3, fa4, fa5, fa6 ),
					HIterator( _slots.end(), fa0, fa1, fa2, fa3, fa4, fa5, fa6 ) ) );
		M_EPILOG
	}
	template<typename fa0_t, typename fa1_t, typename fa2_t, typename fa3_t, typename fa4_t, typename fa5_t, typename fa6_t, typename fa7_t>
	result_type operator()( fa0_t fa0, fa1_t fa1, fa2_t fa2, fa3_t fa3, fa4_t fa4, fa5_t fa5, fa6_t fa6, fa7_t fa7 ) const {
		M_PROLOG
		return ( result_agregator()( HIterator( _slots.begin(), fa0, fa1, fa2, fa3, fa4, fa5, fa6, fa7 ),
					HIterator( _slots.end(), fa0, fa1, fa2, fa3, fa4, fa5, fa6, fa7 ) ) );
		M_EPILOG
	}
	template<typename fa0_t, typename fa1_t, typename fa2_t, typename fa3_t,
		typename fa4_t, typename fa5_t, typename fa6_t, typename fa7_t, typename fa8_t>
	result_type operator()( fa0_t fa0, fa1_t fa1, fa2_t fa2, fa3_t fa3, fa4_t fa4, fa5_t fa5, fa6_t fa6, fa7_t fa7, fa8_t fa8 ) const {
		M_PROLOG
		return ( result_agregator()( HIterator( _slots.begin(), fa0, fa1, fa2, fa3, fa4, fa5, fa6, fa7, fa8 ),
					HIterator( _slots.end(), fa0, fa1, fa2, fa3, fa4, fa5, fa6, fa7, fa8 ) ) );
		M_EPILOG
	}
	template<typename fa0_t, typename fa1_t, typename fa2_t, typename fa3_t,
		typename fa4_t, typename fa5_t, typename fa6_t, typename fa7_t,
		typename fa8_t, typename fa9_t>
	result_type operator()( fa0_t fa0, fa1_t fa1, fa2_t fa2, fa3_t fa3, fa4_t fa4, fa5_t fa5, fa6_t fa6, fa7_t fa7, fa8_t fa8, fa9_t fa9 ) const {
		M_PROLOG
		return ( result_agregator()( HIterator( _slots.begin(), fa0, fa1, fa2, fa3, fa4, fa5, fa6, fa7, fa8, fa9 ),
					HIterator( _slots.end(), fa0, fa1, fa2, fa3, fa4, fa5, fa6, fa7, fa8, fa9 ) ) );
		M_EPILOG
	}
};

template<typename signature_t, typename result_agregator_t>
class HSignal<signature_t, result_agregator_t>::HSlot {
public:
	typedef yaal::hcore::HBoundCall<signature_t> call_t;
private:
	call_t _call;
};

template<typename signature_t, typename result_agregator_t>
class HSignal<signature_t, result_agregator_t>::HIterator : public yaal::hcore::iterator_interface<typename trait::return_type<result_agregator_t>::type, yaal::hcore::iterator_category::forward> {
	typedef HSignal<signature_t, result_agregator_t> owner_t;

};

}

}

#endif /* #ifndef YAAL_TOOLS_HSIGNAL_HXX_INCLUDED */

