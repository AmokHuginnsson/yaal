/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hboundcall.hxx - this file is integral part of `yaal' project.

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
/*! \file hcore/boundcall.hxx
 * \brief Declaration and implementation of HBoundCall<> template.
 */

#ifndef YAAL_HCORE_HBOUNDCALL_HXX_INCLUDED
#define YAAL_HCORE_HBOUNDCALL_HXX_INCLUDED 1

#include "hcore/hcall.hxx"
#include "hcore/hpointer.hxx"
#include "hcore/memory.hxx"

namespace yaal
{

namespace hcore
{

namespace defaults
{

typedef void simple_call( void );

}

template<typename signature_t = defaults::simple_call>
class HBoundCall
	{
	typedef typename trait::return_type<signature_t>::type return_t;
	typedef HCallInterface<trait::argument_count<signature_t>::value,
					return_t,
					typename trait::argument_type<signature_t, 0>::type,
					typename trait::argument_type<signature_t, 1>::type,
					typename trait::argument_type<signature_t, 2>::type,
					typename trait::argument_type<signature_t, 3>::type,
					typename trait::argument_type<signature_t, 4>::type,
					typename trait::argument_type<signature_t, 5>::type,
					typename trait::argument_type<signature_t, 6>::type,
					typename trait::argument_type<signature_t, 7>::type,
					typename trait::argument_type<signature_t, 8>::type,
					typename trait::argument_type<signature_t, 9>::type,
					typename trait::argument_type<signature_t, 10>::type> call_t;
	typedef HPointer<call_t> call_ptr_t;
	call_ptr_t _call;
public:
	HBoundCall( void ) : _call() {}
	template<int free_args_count, typename return_type, typename CALL_type,
		typename b0_t, typename b1_t,
		typename b2_t, typename b3_t,
		typename b4_t, typename b5_t,
		typename b6_t, typename b7_t,
		typename b8_t, typename b9_t,
		typename b10_t>
	HBoundCall( HCall<free_args_count, return_type, CALL_type, b0_t, b1_t, b2_t, b3_t, b4_t, b5_t, b6_t, b7_t, b8_t, b9_t, b10_t> const& call_ )
	 : _call( make_pointer<HCall<free_args_count, return_type, CALL_type, b0_t, b1_t, b2_t, b3_t, b4_t, b5_t, b6_t, b7_t, b8_t, b9_t, b10_t> >( call_ ) )
		{	}
	return_t operator()( void ) const
		{ return ( (*_call)() ); }
	return_t operator()( void )
		{ return ( (*_call)() ); }
	template<typename fa0_t>
	return_t operator()( fa0_t fa0 ) const
		{ return ( (*_call)( fa0 ) ); }
	template<typename fa0_t>
	return_t operator()( fa0_t fa0 )
		{ return ( (*_call)( fa0 ) ); }
	template<typename fa0_t, typename fa1_t>
	return_t operator()( fa0_t fa0, fa1_t fa1 ) const
		{ return ( (*_call)( fa0, fa1 ) ); }
	template<typename fa0_t, typename fa1_t>
	return_t operator()( fa0_t fa0, fa1_t fa1 )
		{ return ( (*_call)( fa0, fa1 ) ); }
	template<typename fa0_t, typename fa1_t, typename fa2_t>
	return_t operator()( fa0_t fa0, fa1_t fa1, fa2_t fa2 ) const
		{ return ( (*_call)( fa0, fa1, fa2 ) ); }
	template<typename fa0_t, typename fa1_t, typename fa2_t>
	return_t operator()( fa0_t fa0, fa1_t fa1, fa2_t fa2 )
		{ return ( (*_call)( fa0, fa1, fa2 ) ); }
	template<typename fa0_t, typename fa1_t, typename fa2_t, typename fa3_t>
	return_t operator()( fa0_t fa0, fa1_t fa1, fa2_t fa2, fa3_t fa3 ) const
		{ return ( (*_call)( fa0, fa1, fa2, fa3 ) ); }
	template<typename fa0_t, typename fa1_t, typename fa2_t, typename fa3_t>
	return_t operator()( fa0_t fa0, fa1_t fa1, fa2_t fa2, fa3_t fa3 )
		{ return ( (*_call)( fa0, fa1, fa2, fa3 ) ); }
	template<typename fa0_t, typename fa1_t, typename fa2_t, typename fa3_t, typename fa4_t>
	return_t operator()( fa0_t fa0, fa1_t fa1, fa2_t fa2, fa3_t fa3, fa4_t fa4 ) const
		{ return ( (*_call)( fa0, fa1, fa2, fa3, fa4 ) ); }
	template<typename fa0_t, typename fa1_t, typename fa2_t, typename fa3_t, typename fa4_t>
	return_t operator()( fa0_t fa0, fa1_t fa1, fa2_t fa2, fa3_t fa3, fa4_t fa4 )
		{ return ( (*_call)( fa0, fa1, fa2, fa3, fa4 ) ); }
	template<typename fa0_t, typename fa1_t, typename fa2_t, typename fa3_t, typename fa4_t, typename fa5_t>
	return_t operator()( fa0_t fa0, fa1_t fa1, fa2_t fa2, fa3_t fa3, fa4_t fa4, fa5_t fa5 ) const
		{ return ( (*_call)( fa0, fa1, fa2, fa3, fa4, fa5 ) ); }
	template<typename fa0_t, typename fa1_t, typename fa2_t, typename fa3_t, typename fa4_t, typename fa5_t>
	return_t operator()( fa0_t fa0, fa1_t fa1, fa2_t fa2, fa3_t fa3, fa4_t fa4, fa5_t fa5 )
		{ return ( (*_call)( fa0, fa1, fa2, fa3, fa4, fa5 ) ); }
	template<typename fa0_t, typename fa1_t, typename fa2_t, typename fa3_t, typename fa4_t, typename fa5_t, typename fa6_t>
	return_t operator()( fa0_t fa0, fa1_t fa1, fa2_t fa2, fa3_t fa3, fa4_t fa4, fa5_t fa5, fa6_t fa6 ) const
		{ return ( (*_call)( fa0, fa1, fa2, fa3, fa4, fa5, fa6 ) ); }
	template<typename fa0_t, typename fa1_t, typename fa2_t, typename fa3_t, typename fa4_t, typename fa5_t, typename fa6_t>
	return_t operator()( fa0_t fa0, fa1_t fa1, fa2_t fa2, fa3_t fa3, fa4_t fa4, fa5_t fa5, fa6_t fa6 )
		{ return ( (*_call)( fa0, fa1, fa2, fa3, fa4, fa5, fa6 ) ); }
	template<typename fa0_t, typename fa1_t, typename fa2_t, typename fa3_t, typename fa4_t, typename fa5_t, typename fa6_t, typename fa7_t>
	return_t operator()( fa0_t fa0, fa1_t fa1, fa2_t fa2, fa3_t fa3, fa4_t fa4, fa5_t fa5, fa6_t fa6, fa7_t fa7 ) const
		{ return ( (*_call)( fa0, fa1, fa2, fa3, fa4, fa5, fa6, fa7 ) ); }
	template<typename fa0_t, typename fa1_t, typename fa2_t, typename fa3_t, typename fa4_t, typename fa5_t, typename fa6_t, typename fa7_t>
	return_t operator()( fa0_t fa0, fa1_t fa1, fa2_t fa2, fa3_t fa3, fa4_t fa4, fa5_t fa5, fa6_t fa6, fa7_t fa7 )
		{ return ( (*_call)( fa0, fa1, fa2, fa3, fa4, fa5, fa6, fa7 ) ); }
	template<typename fa0_t, typename fa1_t, typename fa2_t, typename fa3_t, typename fa4_t, typename fa5_t, typename fa6_t, typename fa7_t, typename fa8_t>
	return_t operator()( fa0_t fa0, fa1_t fa1, fa2_t fa2, fa3_t fa3, fa4_t fa4, fa5_t fa5, fa6_t fa6, fa7_t fa7, fa8_t fa8 ) const
		{ return ( (*_call)( fa0, fa1, fa2, fa3, fa4, fa5, fa6, fa7, fa8 ) ); }
	template<typename fa0_t, typename fa1_t, typename fa2_t, typename fa3_t, typename fa4_t, typename fa5_t, typename fa6_t, typename fa7_t, typename fa8_t>
	return_t operator()( fa0_t fa0, fa1_t fa1, fa2_t fa2, fa3_t fa3, fa4_t fa4, fa5_t fa5, fa6_t fa6, fa7_t fa7, fa8_t fa8 )
		{ return ( (*_call)( fa0, fa1, fa2, fa3, fa4, fa5, fa6, fa7, fa8 ) ); }
	template<typename fa0_t, typename fa1_t, typename fa2_t, typename fa3_t,
		typename fa4_t, typename fa5_t, typename fa6_t, typename fa7_t,
		typename fa8_t, typename fa9_t>
	return_t operator()( fa0_t fa0, fa1_t fa1, fa2_t fa2, fa3_t fa3, fa4_t fa4, fa5_t fa5, fa6_t fa6, fa7_t fa7, fa8_t fa8, fa9_t fa9 ) const
		{ return ( (*_call)( fa0, fa1, fa2, fa3, fa4, fa5, fa6, fa7, fa8, fa9 ) ); }
	template<typename fa0_t, typename fa1_t, typename fa2_t, typename fa3_t,
		typename fa4_t, typename fa5_t, typename fa6_t, typename fa7_t,
		typename fa8_t, typename fa9_t>
	return_t operator()( fa0_t fa0, fa1_t fa1, fa2_t fa2, fa3_t fa3, fa4_t fa4, fa5_t fa5, fa6_t fa6, fa7_t fa7, fa8_t fa8, fa9_t fa9 )
		{ return ( (*_call)( fa0, fa1, fa2, fa3, fa4, fa5, fa6, fa7, fa8, fa9 ) ); }
	void const* id( void ) const
		{ return ( _call->id() ); }
	void reset( void )
		{
		_call.reset();
		}
	bool operator ! ( void ) const
		{ return ( ! _call ); }
	};

}

}

#endif /* #ifndef YAAL_HCORE_HBOUNDCALL_HXX_INCLUDED */
