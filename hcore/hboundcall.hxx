/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hboundcall.hxx - this file is integral part of `yaal' project.

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
/*! \file hcore/boundcall.hxx
 * \brief Declaration and implementation of HBoundCall<> template.
 */

#ifndef YAAL_HCORE_HBOUNDCALL_HXX_INCLUDED
#define YAAL_HCORE_HBOUNDCALL_HXX_INCLUDED 1

#include "hcore/hcall.hxx"
#include "hcore/hpointer.hxx"
#include "hcore/memory.hxx"

namespace yaal {

namespace hcore {

namespace defaults {

typedef void simple_call( void );

}

template<typename signature_t = defaults::simple_call>
class HBoundCall {
public:
	typedef HBoundCall<signature_t> this_type;
	typedef typename trait::return_type<signature_t>::type result_type;
	typedef typename trait::argument_type<signature_t, 0>::type argument_type;
	typedef typename trait::argument_type<signature_t, 0>::type first_argument_type;
	typedef typename trait::argument_type<signature_t, 1>::type second_argument_type;
	typedef typename trait::argument_type<signature_t, 2>::type third_argument_type;
	typedef typename trait::argument_type<signature_t, 3>::type fourth_argument_type;
	typedef typename trait::argument_type<signature_t, 4>::type fifth_argument_type;
	typedef typename trait::argument_type<signature_t, 5>::type sixth_argument_type;
	typedef typename trait::argument_type<signature_t, 6>::type seventh_argument_type;
	typedef typename trait::argument_type<signature_t, 7>::type eighth_argument_type;
	typedef typename trait::argument_type<signature_t, 8>::type ninth_argument_type;
	typedef typename trait::argument_type<signature_t, 9>::type tenth_argument_type;
	typedef typename trait::argument_type<signature_t, 10>::type eleventh_argument_type;
private:
	typedef HCallInterface<trait::argument_count<signature_t>::value,
					result_type,
					first_argument_type,
					second_argument_type,
					third_argument_type,
					fourth_argument_type,
					fifth_argument_type,
					sixth_argument_type,
					seventh_argument_type,
					eighth_argument_type,
					ninth_argument_type,
					tenth_argument_type,
					eleventh_argument_type
					> call_t;
	typedef HPointer<call_t> call_ptr_t;
	call_ptr_t _call;
public:
	HBoundCall( void )
		: _call() {
		return;
	}
	template<int free_args_count, typename return_t, typename CALL_type,
		typename b0_t, typename b1_t,
		typename b2_t, typename b3_t,
		typename b4_t, typename b5_t,
		typename b6_t, typename b7_t,
		typename b8_t, typename b9_t,
		typename b10_t>
	HBoundCall( HCall<free_args_count, return_t, CALL_type, b0_t, b1_t, b2_t, b3_t, b4_t, b5_t, b6_t, b7_t, b8_t, b9_t, b10_t> const& boundCall_ )
		: _call( make_pointer<HCall<free_args_count, return_t, CALL_type,
				 b0_t, b1_t, b2_t, b3_t, b4_t, b5_t, b6_t, b7_t, b8_t, b9_t, b10_t> >( boundCall_ ) ) {
		return;
	}
	result_type operator()( void ) const
		{ return ( (*_call)() ); }
	result_type operator()( void )
		{ return ( (*_call)() ); }
	template<typename fa0_t>
	result_type operator()( fa0_t fa0 ) const
		{ return ( (*_call)( fa0 ) ); }
	template<typename fa0_t>
	result_type operator()( fa0_t fa0 )
		{ return ( (*_call)( fa0 ) ); }
	template<typename fa0_t, typename fa1_t>
	result_type operator()( fa0_t fa0, fa1_t fa1 ) const
		{ return ( (*_call)( fa0, fa1 ) ); }
	template<typename fa0_t, typename fa1_t>
	result_type operator()( fa0_t fa0, fa1_t fa1 )
		{ return ( (*_call)( fa0, fa1 ) ); }
	template<typename fa0_t, typename fa1_t, typename fa2_t>
	result_type operator()( fa0_t fa0, fa1_t fa1, fa2_t fa2 ) const
		{ return ( (*_call)( fa0, fa1, fa2 ) ); }
	template<typename fa0_t, typename fa1_t, typename fa2_t>
	result_type operator()( fa0_t fa0, fa1_t fa1, fa2_t fa2 )
		{ return ( (*_call)( fa0, fa1, fa2 ) ); }
	template<typename fa0_t, typename fa1_t, typename fa2_t, typename fa3_t>
	result_type operator()( fa0_t fa0, fa1_t fa1, fa2_t fa2, fa3_t fa3 ) const
		{ return ( (*_call)( fa0, fa1, fa2, fa3 ) ); }
	template<typename fa0_t, typename fa1_t, typename fa2_t, typename fa3_t>
	result_type operator()( fa0_t fa0, fa1_t fa1, fa2_t fa2, fa3_t fa3 )
		{ return ( (*_call)( fa0, fa1, fa2, fa3 ) ); }
	template<typename fa0_t, typename fa1_t, typename fa2_t, typename fa3_t, typename fa4_t>
	result_type operator()( fa0_t fa0, fa1_t fa1, fa2_t fa2, fa3_t fa3, fa4_t fa4 ) const
		{ return ( (*_call)( fa0, fa1, fa2, fa3, fa4 ) ); }
	template<typename fa0_t, typename fa1_t, typename fa2_t, typename fa3_t, typename fa4_t>
	result_type operator()( fa0_t fa0, fa1_t fa1, fa2_t fa2, fa3_t fa3, fa4_t fa4 )
		{ return ( (*_call)( fa0, fa1, fa2, fa3, fa4 ) ); }
	template<typename fa0_t, typename fa1_t, typename fa2_t, typename fa3_t, typename fa4_t, typename fa5_t>
	result_type operator()( fa0_t fa0, fa1_t fa1, fa2_t fa2, fa3_t fa3, fa4_t fa4, fa5_t fa5 ) const
		{ return ( (*_call)( fa0, fa1, fa2, fa3, fa4, fa5 ) ); }
	template<typename fa0_t, typename fa1_t, typename fa2_t, typename fa3_t, typename fa4_t, typename fa5_t>
	result_type operator()( fa0_t fa0, fa1_t fa1, fa2_t fa2, fa3_t fa3, fa4_t fa4, fa5_t fa5 )
		{ return ( (*_call)( fa0, fa1, fa2, fa3, fa4, fa5 ) ); }
	template<typename fa0_t, typename fa1_t, typename fa2_t, typename fa3_t, typename fa4_t, typename fa5_t, typename fa6_t>
	result_type operator()( fa0_t fa0, fa1_t fa1, fa2_t fa2, fa3_t fa3, fa4_t fa4, fa5_t fa5, fa6_t fa6 ) const
		{ return ( (*_call)( fa0, fa1, fa2, fa3, fa4, fa5, fa6 ) ); }
	template<typename fa0_t, typename fa1_t, typename fa2_t, typename fa3_t, typename fa4_t, typename fa5_t, typename fa6_t>
	result_type operator()( fa0_t fa0, fa1_t fa1, fa2_t fa2, fa3_t fa3, fa4_t fa4, fa5_t fa5, fa6_t fa6 )
		{ return ( (*_call)( fa0, fa1, fa2, fa3, fa4, fa5, fa6 ) ); }
	template<typename fa0_t, typename fa1_t, typename fa2_t, typename fa3_t, typename fa4_t, typename fa5_t, typename fa6_t, typename fa7_t>
	result_type operator()( fa0_t fa0, fa1_t fa1, fa2_t fa2, fa3_t fa3, fa4_t fa4, fa5_t fa5, fa6_t fa6, fa7_t fa7 ) const
		{ return ( (*_call)( fa0, fa1, fa2, fa3, fa4, fa5, fa6, fa7 ) ); }
	template<typename fa0_t, typename fa1_t, typename fa2_t, typename fa3_t, typename fa4_t, typename fa5_t, typename fa6_t, typename fa7_t>
	result_type operator()( fa0_t fa0, fa1_t fa1, fa2_t fa2, fa3_t fa3, fa4_t fa4, fa5_t fa5, fa6_t fa6, fa7_t fa7 )
		{ return ( (*_call)( fa0, fa1, fa2, fa3, fa4, fa5, fa6, fa7 ) ); }
	template<typename fa0_t, typename fa1_t, typename fa2_t, typename fa3_t, typename fa4_t, typename fa5_t, typename fa6_t, typename fa7_t, typename fa8_t>
	result_type operator()( fa0_t fa0, fa1_t fa1, fa2_t fa2, fa3_t fa3, fa4_t fa4, fa5_t fa5, fa6_t fa6, fa7_t fa7, fa8_t fa8 ) const
		{ return ( (*_call)( fa0, fa1, fa2, fa3, fa4, fa5, fa6, fa7, fa8 ) ); }
	template<typename fa0_t, typename fa1_t, typename fa2_t, typename fa3_t, typename fa4_t, typename fa5_t, typename fa6_t, typename fa7_t, typename fa8_t>
	result_type operator()( fa0_t fa0, fa1_t fa1, fa2_t fa2, fa3_t fa3, fa4_t fa4, fa5_t fa5, fa6_t fa6, fa7_t fa7, fa8_t fa8 )
		{ return ( (*_call)( fa0, fa1, fa2, fa3, fa4, fa5, fa6, fa7, fa8 ) ); }
	template<typename fa0_t, typename fa1_t, typename fa2_t, typename fa3_t,
		typename fa4_t, typename fa5_t, typename fa6_t, typename fa7_t,
		typename fa8_t, typename fa9_t>
	result_type operator()( fa0_t fa0, fa1_t fa1, fa2_t fa2, fa3_t fa3, fa4_t fa4, fa5_t fa5, fa6_t fa6, fa7_t fa7, fa8_t fa8, fa9_t fa9 ) const
		{ return ( (*_call)( fa0, fa1, fa2, fa3, fa4, fa5, fa6, fa7, fa8, fa9 ) ); }
	template<typename fa0_t, typename fa1_t, typename fa2_t, typename fa3_t,
		typename fa4_t, typename fa5_t, typename fa6_t, typename fa7_t,
		typename fa8_t, typename fa9_t>
	result_type operator()( fa0_t fa0, fa1_t fa1, fa2_t fa2, fa3_t fa3, fa4_t fa4, fa5_t fa5, fa6_t fa6, fa7_t fa7, fa8_t fa8, fa9_t fa9 )
		{ return ( (*_call)( fa0, fa1, fa2, fa3, fa4, fa5, fa6, fa7, fa8, fa9 ) ); }
	void const* id( void ) const
		{ return ( _call->id() ); }
	void reset( void ) {
		_call.reset();
	}
	bool operator ! ( void ) const
		{ return ( ! _call ); }
	void swap( HBoundCall& boundCall_ ) {
		if ( &boundCall_ != this ) {
			using yaal::swap;
			swap( boundCall_._call, _call );
		}
		return;
	}
};

template<typename signature_t>
inline void swap( HBoundCall<signature_t>& a, HBoundCall<signature_t>& b )
	{ a.swap( b ); }

}

}

#endif /* #ifndef YAAL_HCORE_HBOUNDCALL_HXX_INCLUDED */

