/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hcall.hxx - this file is integral part of `yaal' project.

	i.  You may not make any changes in Copyright information.
	ii. You must attach Copyright information to any part of every copy
	    of this software.

Copyright:

 You are free to use this program as is, you can redistribute binary
 package freely but:
  1. You can not use any part of sources of this software.
  2. You can not redistribute any part of sources of this software.
  3. No reverse engineering is allowed.
  4. If you want redistribute binary package you can not demand any fees
     for this software.
     You can not even demand cost of the carrier (CD for example).
  5. You can not include it to any commercial enterprise (for example 
     as a free add-on to payed software or payed newspaper).
 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE. Use it at your own risk.
*/

#ifndef YAAL_HCORE_HCALL_HXX_INCLUDED
#define YAAL_HCORE_HCALL_HXX_INCLUDED

#line 31

#define D_VCSID_YAAL_HCORE_HCALL_H "$Id$"

#include "hcore/hpointer.hxx"

namespace yaal
{

namespace hcore
{

/*! \brief Interface to abstration of any-method of any-class invocation.
 */
class HCallInterface
	{
public:
	/*! \brief A dummy class.
	 *
	 * Dummy used for template metaprogramming trick in HFunctor and HCall implementation.
	 */
	class HPseudoType {};
	typedef yaal::hcore::HPointer<HCallInterface> ptr_t;
	virtual ~HCallInterface( void ) {}
	virtual void invoke( void ) = 0;
	virtual void invoke( void ) const = 0;
	virtual void operator()( void ) = 0;
	virtual void const* id( void ) = 0;
	};

/*! \brief Make a functor from any standalone function.
 */
template<typename call_t,
	typename a0_t = HCallInterface::HPseudoType,
	typename a1_t = HCallInterface::HPseudoType,
	typename a2_t = HCallInterface::HPseudoType,
	typename a3_t = HCallInterface::HPseudoType,
	typename a4_t = HCallInterface::HPseudoType,
	typename a5_t = HCallInterface::HPseudoType,
	typename a6_t = HCallInterface::HPseudoType,
	typename a7_t = HCallInterface::HPseudoType,
	typename a8_t = HCallInterface::HPseudoType,
	typename a9_t = HCallInterface::HPseudoType>
class HFunctor
	{
	call_t& call;
public:
	HFunctor( call_t& a_call ) : call( a_call ) {}
	void method(
			a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4,
			a5_t a5, a6_t a6, a7_t a7, a8_t a8, a9_t a9 )
		{ call( a0, a1, a2, a3, a4, a5, a6, a7, a8, a9 ); }
	void method(
			a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4,
			a5_t a5, a6_t a6, a7_t a7, a8_t a8 )
		{ call( a0, a1, a2, a3, a4, a5, a6, a7, a8 ); }
	void method(
			a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4,
			a5_t a5, a6_t a6, a7_t a7 )
		{ call( a0, a1, a2, a3, a4, a5, a6, a7 ); }
	void method(
			a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4,
			a5_t a5, a6_t a6 )
		{ call( a0, a1, a2, a3, a4, a5, a6 ); }
	void method(
			a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4,
			a5_t a5 )
		{ call( a0, a1, a2, a3, a4, a5 ); }
	void method(
			a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4 )
		{ call( a0, a1, a2, a3, a4 ); }
	void method( a0_t a0, a1_t a1, a2_t a2, a3_t a3 )
		{ call( a0, a1, a2, a3 ); }
	void method( a0_t a0, a1_t a1, a2_t a2 )
		{ call( a0, a1, a2 ); }
	void method( a0_t a0, a1_t a1 )
		{ call( a0, a1 ); }
	void method( a0_t a0 )
		{ call( a0 ); }
	void method( void )
		{ call(); }
	void method( void ) const
		{ call(); }
	};

/*! \brief Implementation of abstration of any-method of any-class invocation.
 *
 * \tparam CLASS_t - class on which this invocation will operate.
 * \tparam METHOD_t - method of given class CLASS_t that shall be invoked.
 * \tparam a0_t - first argument to given METHOD_t of CLASS_t.
 * \tparam a1_t - second argument to given METHOD_t of CLASS_t.
 * \tparam aN_t - N-th argument to given METHOD_t of CLASS_t.
 */
template<typename CLASS_t, typename METHOD_t,
	typename a0_t = HCallInterface::HPseudoType,
	typename a1_t = HCallInterface::HPseudoType,
	typename a2_t = HCallInterface::HPseudoType,
	typename a3_t = HCallInterface::HPseudoType,
	typename a4_t = HCallInterface::HPseudoType,
	typename a5_t = HCallInterface::HPseudoType,
	typename a6_t = HCallInterface::HPseudoType,
	typename a7_t = HCallInterface::HPseudoType,
	typename a8_t = HCallInterface::HPseudoType,
	typename a9_t = HCallInterface::HPseudoType>
class HCall : public HCallInterface
	{
	typedef HCallInterface::HPseudoType* __;
	CLASS_t f_oObiect;
	METHOD_t METHOD;
	a0_t f_xArg0;
	a1_t f_xArg1;
	a2_t f_xArg2;
	a3_t f_xArg3;
	a4_t f_xArg4;
	a5_t f_xArg5;
	a6_t f_xArg6;
	a7_t f_xArg7;
	a8_t f_xArg8;
	a9_t f_xArg9;
public:
	HCall(
			CLASS_t, METHOD_t,
			a0_t = a0_t(), a1_t = a1_t(), a2_t = a2_t(), a3_t = a3_t(),
			a4_t = a4_t(), a5_t = a5_t(), a6_t = a6_t(), a7_t = a7_t(),
			a8_t = a8_t(), a9_t = a9_t()
			);
	void invoke( 
			a0_t const*, a1_t const*, a2_t const*, a3_t const*, a4_t const*,
			a5_t const*, a6_t const*, a7_t const*, a8_t const*, a9_t const* )
		{ (f_oObiect.*METHOD)( f_xArg0, f_xArg1, f_xArg2, f_xArg3, f_xArg4,
				f_xArg5, f_xArg6, f_xArg7, f_xArg8, f_xArg9 ); }
	void invoke( 
			a0_t const*, a1_t const*, a2_t const*, a3_t const*, a4_t const*,
			a5_t const*, a6_t const*, a7_t const*, a8_t const*, a9_t const* ) const
		{ (f_oObiect.*METHOD)( f_xArg0, f_xArg1, f_xArg2, f_xArg3, f_xArg4,
				f_xArg5, f_xArg6, f_xArg7, f_xArg8, f_xArg9 ); }
	void invoke( 
			a0_t const*, a1_t const*, a2_t const*, a3_t const*, a4_t const*,
			a5_t const*, a6_t const*, a7_t const*, a8_t const*, __ )
		{ (f_oObiect.*METHOD)( f_xArg0, f_xArg1, f_xArg2, f_xArg3, f_xArg4,
				f_xArg5, f_xArg6, f_xArg7, f_xArg8 ); }
	void invoke( 
			a0_t const*, a1_t const*, a2_t const*, a3_t const*, a4_t const*,
			a5_t const*, a6_t const*, a7_t const*, a8_t const*, __ ) const
		{ (f_oObiect.*METHOD)( f_xArg0, f_xArg1, f_xArg2, f_xArg3, f_xArg4,
				f_xArg5, f_xArg6, f_xArg7, f_xArg8 ); }
	void invoke( 
			a0_t const*, a1_t const*, a2_t const*, a3_t const*, a4_t const*,
			a5_t const*, a6_t const*, a7_t const*, __, __ )
		{ (f_oObiect.*METHOD)( f_xArg0, f_xArg1, f_xArg2, f_xArg3, f_xArg4,
				f_xArg5, f_xArg6, f_xArg7 ); }
	void invoke( 
			a0_t const*, a1_t const*, a2_t const*, a3_t const*, a4_t const*,
			a5_t const*, a6_t const*, a7_t const*, __, __ ) const
		{ (f_oObiect.*METHOD)( f_xArg0, f_xArg1, f_xArg2, f_xArg3, f_xArg4,
				f_xArg5, f_xArg6, f_xArg7 ); }
	void invoke( 
			a0_t const*, a1_t const*, a2_t const*, a3_t const*, a4_t const*,
			a5_t const*, a6_t const*, __, __, __ )
		{ (f_oObiect.*METHOD)( f_xArg0, f_xArg1, f_xArg2, f_xArg3, f_xArg4,
				f_xArg5, f_xArg6 ); }
	void invoke( 
			a0_t const*, a1_t const*, a2_t const*, a3_t const*, a4_t const*,
			a5_t const*, a6_t const*, __, __, __ ) const
		{ (f_oObiect.*METHOD)( f_xArg0, f_xArg1, f_xArg2, f_xArg3, f_xArg4,
				f_xArg5, f_xArg6 ); }
	void invoke( 
			a0_t const*, a1_t const*, a2_t const*, a3_t const*, a4_t const*,
			a5_t const*, __, __, __, __ )
		{ (f_oObiect.*METHOD)( f_xArg0, f_xArg1, f_xArg2, f_xArg3, f_xArg4,
				f_xArg5 ); }
	void invoke( 
			a0_t const*, a1_t const*, a2_t const*, a3_t const*, a4_t const*,
			a5_t const*, __, __, __, __ ) const
		{ (f_oObiect.*METHOD)( f_xArg0, f_xArg1, f_xArg2, f_xArg3, f_xArg4,
				f_xArg5 ); }
	void invoke( 
			a0_t const*, a1_t const*, a2_t const*, a3_t const*, a4_t const*,
			__, __, __, __, __ )
		{ (f_oObiect.*METHOD)( f_xArg0, f_xArg1, f_xArg2, f_xArg3, f_xArg4 ); }
	void invoke( 
			a0_t const*, a1_t const*, a2_t const*, a3_t const*, a4_t const*,
			__, __, __, __, __ ) const
		{ (f_oObiect.*METHOD)( f_xArg0, f_xArg1, f_xArg2, f_xArg3, f_xArg4 ); }
	void invoke( a0_t const*, a1_t const*, a2_t const*, a3_t const*, __,
			__, __, __, __, __ )
		{ (f_oObiect.*METHOD)( f_xArg0, f_xArg1, f_xArg2, f_xArg3 ); }
	void invoke( a0_t const*, a1_t const*, a2_t const*, a3_t const*, __,
			__, __, __, __, __ ) const
		{ (f_oObiect.*METHOD)( f_xArg0, f_xArg1, f_xArg2, f_xArg3 ); }
	void invoke( a0_t const*, a1_t const*, a2_t const*, __, __, __, __,
			__, __, __ )
		{ (f_oObiect.*METHOD)( f_xArg0, f_xArg1, f_xArg2 ); }
	void invoke( a0_t const*, a1_t const*, a2_t const*, __, __, __, __,
			__, __, __ ) const
		{ (f_oObiect.*METHOD)( f_xArg0, f_xArg1, f_xArg2 ); }
	void invoke( a0_t const*, a1_t const*, __, __, __, __, __, __, __, __ )
		{ (f_oObiect.*METHOD)( f_xArg0, f_xArg1 ); }
	void invoke( a0_t const*, a1_t const*, __, __, __, __, __, __, __, __ ) const
		{ (f_oObiect.*METHOD)( f_xArg0, f_xArg1 ); }
	void invoke( a0_t const*, __, __, __, __, __, __, __, __, __ )
		{ (f_oObiect.*METHOD)( f_xArg0 ); }
	void invoke( a0_t const*, __, __, __, __, __, __, __, __, __ ) const
		{ (f_oObiect.*METHOD)( f_xArg0 ); }
	void invoke( __, __, __, __, __, __, __, __, __, __ )
		{ (f_oObiect.*METHOD)(); }
	void invoke( __, __, __, __, __, __, __, __, __, __ ) const
		{ (f_oObiect.*METHOD)(); }
	virtual void invoke( void )
		{
		invoke(
				static_cast<a0_t*>( NULL ),
				static_cast<a1_t*>( NULL ),
				static_cast<a2_t*>( NULL ),
				static_cast<a3_t*>( NULL ),
				static_cast<a4_t*>( NULL ),
				static_cast<a5_t*>( NULL ),
				static_cast<a6_t*>( NULL ),
				static_cast<a7_t*>( NULL ),
				static_cast<a8_t*>( NULL ),
				static_cast<a9_t*>( NULL ) );
		}
	virtual void invoke( void ) const
		{
		invoke(
				static_cast<a0_t*>( NULL ),
				static_cast<a1_t*>( NULL ),
				static_cast<a2_t*>( NULL ),
				static_cast<a3_t*>( NULL ),
				static_cast<a4_t*>( NULL ),
				static_cast<a5_t*>( NULL ),
				static_cast<a6_t*>( NULL ),
				static_cast<a7_t*>( NULL ),
				static_cast<a8_t*>( NULL ),
				static_cast<a9_t*>( NULL ) );
		}
	virtual void operator()( void )
		{ invoke(); }
	virtual void const* id( void )
		{ return ( &f_oObiect ); }
	};

template<typename CLASS_t, typename METHOD_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t, typename a6_t, typename a7_t,
	typename a8_t, typename a9_t>
HCall<CLASS_t, METHOD_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t>::HCall(
			CLASS_t a_oObject, METHOD_t A_METHOD,
			a0_t a0, a1_t a1,
			a2_t a2, a3_t a3,
			a4_t a4, a5_t a5,
			a6_t a6, a7_t a7,
			a8_t a8, a9_t a9
			) : f_oObiect( a_oObject ), METHOD( A_METHOD ),
	f_xArg0( a0 ), f_xArg1( a1 ),
	f_xArg2( a2 ), f_xArg3( a3 ),
	f_xArg4( a4 ), f_xArg5( a5 ),
	f_xArg6( a6 ), f_xArg7( a7 ),
	f_xArg8( a8 ), f_xArg9( a9 ) {}

}

}

#endif /* not YAAL_HCORE_HCALL_HXX_INCLUDED */

