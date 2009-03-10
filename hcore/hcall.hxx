/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hcall.hxx - this file is integral part of `yaal' project.

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
/*! \file hcore/call.hxx
 * \brief Declaration and implementation of HCall<> template.
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
	void operator()( void )
		{ invoke(); }
	void invoke( void )
		{ do_invoke(); }
	void invoke( void ) const
		{ do_invoke(); }
	void const* id( void ) const
		{ return ( do_id() ); }
protected:
	virtual void do_invoke( void ) = 0;
	virtual void do_invoke( void ) const = 0;
	virtual void const* do_id( void ) const = 0;
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

/*! \brief Base class for HCall<1..10>.
 */
template<typename CLASS_t, typename METHOD_t>
class HCallBase : public HCallInterface
	{
protected:
	CLASS_t f_oObject;
	METHOD_t METHOD;
public:
	HCallBase( CLASS_t obj, METHOD_t A_METHOD ) : f_oObject( obj ), METHOD( A_METHOD ) {}
protected:
	virtual void const* do_id( void ) const
		{ return ( &f_oObject ); }
	};

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
class HCall;

/*! \cond */
template<typename CLASS_t, typename METHOD_t>
class HCall<CLASS_t, METHOD_t,
	HCallInterface::HPseudoType,
	HCallInterface::HPseudoType,
	HCallInterface::HPseudoType,
	HCallInterface::HPseudoType,
	HCallInterface::HPseudoType,
	HCallInterface::HPseudoType,
	HCallInterface::HPseudoType,
	HCallInterface::HPseudoType,
	HCallInterface::HPseudoType,
	HCallInterface::HPseudoType> : public HCallBase<CLASS_t, METHOD_t>
	{
	typedef HCallBase<CLASS_t, METHOD_t> base_t;
public:
	HCall( CLASS_t obj, METHOD_t A_METHOD )
		: base_t( obj, A_METHOD ) {}
protected:
	virtual void do_invoke( void )
		{ (base_t::f_oObject.*base_t::METHOD)(); }
	virtual void do_invoke( void ) const
		{ (base_t::f_oObject.*base_t::METHOD)(); }
	};

template<typename CLASS_t, typename METHOD_t,
	typename a0_t>
class HCall<CLASS_t, METHOD_t,
	a0_t,
	HCallInterface::HPseudoType,
	HCallInterface::HPseudoType,
	HCallInterface::HPseudoType,
	HCallInterface::HPseudoType,
	HCallInterface::HPseudoType,
	HCallInterface::HPseudoType,
	HCallInterface::HPseudoType,
	HCallInterface::HPseudoType,
	HCallInterface::HPseudoType> : public HCallBase<CLASS_t, METHOD_t>
	{
	typedef HCallBase<CLASS_t, METHOD_t> base_t;
protected:
	a0_t f_xArg0;
public:
	HCall( CLASS_t obj, METHOD_t A_METHOD, a0_t a0 )
		: base_t( obj, A_METHOD ),
		f_xArg0( a0 ) {}
protected:
	virtual void do_invoke( void )
		{ (base_t::f_oObject.*base_t::METHOD)( f_xArg0 ); }
	virtual void do_invoke( void ) const
		{ (base_t::f_oObject.*base_t::METHOD)( f_xArg0 ); }
	};

template<typename CLASS_t, typename METHOD_t,
	typename a0_t, typename a1_t>
class HCall<CLASS_t, METHOD_t,
	a0_t, a1_t,
	HCallInterface::HPseudoType,
	HCallInterface::HPseudoType,
	HCallInterface::HPseudoType,
	HCallInterface::HPseudoType,
	HCallInterface::HPseudoType,
	HCallInterface::HPseudoType,
	HCallInterface::HPseudoType,
	HCallInterface::HPseudoType> : public HCallBase<CLASS_t, METHOD_t>
	{
	typedef HCallBase<CLASS_t, METHOD_t> base_t;
protected:
	a0_t f_xArg0;
	a1_t f_xArg1;
public:
	HCall( CLASS_t obj, METHOD_t A_METHOD, a0_t a0, a1_t a1 )
		: base_t( obj, A_METHOD ),
		f_xArg0( a0 ), f_xArg1( a1 ) {}
protected:
	virtual void do_invoke( void )
		{ (base_t::f_oObject.*base_t::METHOD)( f_xArg0, f_xArg1 ); }
	virtual void do_invoke( void ) const
		{ (base_t::f_oObject.*base_t::METHOD)( f_xArg0, f_xArg1 ); }
	};

template<typename CLASS_t, typename METHOD_t,
	typename a0_t, typename a1_t, typename a2_t>
class HCall<CLASS_t, METHOD_t,
	a0_t, a1_t, a2_t,
	HCallInterface::HPseudoType,
	HCallInterface::HPseudoType,
	HCallInterface::HPseudoType,
	HCallInterface::HPseudoType,
	HCallInterface::HPseudoType,
	HCallInterface::HPseudoType,
	HCallInterface::HPseudoType> : public HCallBase<CLASS_t, METHOD_t>
	{
	typedef HCallBase<CLASS_t, METHOD_t> base_t;
protected:
	a0_t f_xArg0;
	a1_t f_xArg1;
	a2_t f_xArg2;
public:
	HCall( CLASS_t obj, METHOD_t A_METHOD, a0_t a0, a1_t a1,
			a2_t a2 )
		: base_t( obj, A_METHOD ),
		f_xArg0( a0 ), f_xArg1( a1 ), f_xArg2( a2 ) {}
protected:
	virtual void do_invoke( void )
		{ (base_t::f_oObject.*base_t::METHOD)( f_xArg0, f_xArg1, f_xArg2 ); }
	virtual void do_invoke( void ) const
		{ (base_t::f_oObject.*base_t::METHOD)( f_xArg0, f_xArg1, f_xArg2 ); }
	};

template<typename CLASS_t, typename METHOD_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t>
class HCall<CLASS_t, METHOD_t,
	a0_t, a1_t, a2_t, a3_t,
	HCallInterface::HPseudoType,
	HCallInterface::HPseudoType,
	HCallInterface::HPseudoType,
	HCallInterface::HPseudoType,
	HCallInterface::HPseudoType,
	HCallInterface::HPseudoType> : public HCallBase<CLASS_t, METHOD_t>
	{
	typedef HCallBase<CLASS_t, METHOD_t> base_t;
protected:
	a0_t f_xArg0;
	a1_t f_xArg1;
	a2_t f_xArg2;
	a3_t f_xArg3;
public:
	HCall( CLASS_t obj, METHOD_t A_METHOD, a0_t a0, a1_t a1,
			a2_t a2, a3_t a3 )
		: base_t( obj, A_METHOD ),
		f_xArg0( a0 ), f_xArg1( a1 ), f_xArg2( a2 ), f_xArg3( a3 ) {}
protected:
	virtual void do_invoke( void )
		{ (base_t::f_oObject.*base_t::METHOD)( f_xArg0, f_xArg1, f_xArg2, f_xArg3 ); }
	virtual void do_invoke( void ) const
		{ (base_t::f_oObject.*base_t::METHOD)( f_xArg0, f_xArg1, f_xArg2, f_xArg3 ); }
	};

template<typename CLASS_t, typename METHOD_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t>
class HCall<CLASS_t, METHOD_t,
	a0_t, a1_t, a2_t, a3_t, a4_t,
	HCallInterface::HPseudoType,
	HCallInterface::HPseudoType,
	HCallInterface::HPseudoType,
	HCallInterface::HPseudoType,
	HCallInterface::HPseudoType> : public HCallBase<CLASS_t, METHOD_t>
	{
	typedef HCallBase<CLASS_t, METHOD_t> base_t;
protected:
	a0_t f_xArg0;
	a1_t f_xArg1;
	a2_t f_xArg2;
	a3_t f_xArg3;
	a4_t f_xArg4;
public:
	HCall( CLASS_t obj, METHOD_t A_METHOD, a0_t a0, a1_t a1,
			a2_t a2, a3_t a3, a4_t a4 )
		: base_t( obj, A_METHOD ),
		f_xArg0( a0 ), f_xArg1( a1 ), f_xArg2( a2 ), f_xArg3( a3 ), f_xArg4( a4 ) {}
protected:
	virtual void do_invoke( void )
		{ (base_t::f_oObject.*base_t::METHOD)( f_xArg0, f_xArg1, f_xArg2, f_xArg3, f_xArg4 ); }
	virtual void do_invoke( void ) const
		{ (base_t::f_oObject.*base_t::METHOD)( f_xArg0, f_xArg1, f_xArg2, f_xArg3, f_xArg4 ); }
	};

template<typename CLASS_t, typename METHOD_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t>
class HCall<CLASS_t, METHOD_t,
	a0_t, a1_t, a2_t, a3_t, a4_t, a5_t,
	HCallInterface::HPseudoType,
	HCallInterface::HPseudoType,
	HCallInterface::HPseudoType,
	HCallInterface::HPseudoType> : public HCallBase<CLASS_t, METHOD_t>
	{
	typedef HCallBase<CLASS_t, METHOD_t> base_t;
protected:
	a0_t f_xArg0;
	a1_t f_xArg1;
	a2_t f_xArg2;
	a3_t f_xArg3;
	a4_t f_xArg4;
	a5_t f_xArg5;
public:
	HCall( CLASS_t obj, METHOD_t A_METHOD, a0_t a0, a1_t a1,
			a2_t a2, a3_t a3, a4_t a4, a5_t a5 )
		: base_t( obj, A_METHOD ),
		f_xArg0( a0 ), f_xArg1( a1 ), f_xArg2( a2 ), f_xArg3( a3 ), f_xArg4( a4 ),
		f_xArg5( a5 ) {}
protected:
	virtual void do_invoke( void )
		{ (base_t::f_oObject.*base_t::METHOD)( f_xArg0, f_xArg1, f_xArg2, f_xArg3, f_xArg4, f_xArg5 ); }
	virtual void do_invoke( void ) const
		{ (base_t::f_oObject.*base_t::METHOD)( f_xArg0, f_xArg1, f_xArg2, f_xArg3, f_xArg4, f_xArg5 ); }
	};

template<typename CLASS_t, typename METHOD_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t, typename a6_t>
class HCall<CLASS_t, METHOD_t,
	a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t,
	HCallInterface::HPseudoType,
	HCallInterface::HPseudoType,
	HCallInterface::HPseudoType> : public HCallBase<CLASS_t, METHOD_t>
	{
	typedef HCallBase<CLASS_t, METHOD_t> base_t;
protected:
	a0_t f_xArg0;
	a1_t f_xArg1;
	a2_t f_xArg2;
	a3_t f_xArg3;
	a4_t f_xArg4;
	a5_t f_xArg5;
	a6_t f_xArg6;
public:
	HCall( CLASS_t obj, METHOD_t A_METHOD, a0_t a0, a1_t a1,
			a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6 )
		: base_t( obj, A_METHOD ),
		f_xArg0( a0 ), f_xArg1( a1 ), f_xArg2( a2 ), f_xArg3( a3 ), f_xArg4( a4 ),
		f_xArg5( a5 ), f_xArg6( a6 ) {}
protected:
	virtual void do_invoke( void )
		{ (base_t::f_oObject.*base_t::METHOD)( f_xArg0, f_xArg1, f_xArg2, f_xArg3, f_xArg4, f_xArg5, f_xArg6 ); }
	virtual void do_invoke( void ) const
		{ (base_t::f_oObject.*base_t::METHOD)( f_xArg0, f_xArg1, f_xArg2, f_xArg3, f_xArg4, f_xArg5, f_xArg6 ); }
	};

template<typename CLASS_t, typename METHOD_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t, typename a6_t, typename a7_t>
class HCall<CLASS_t, METHOD_t,
	a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t,
	HCallInterface::HPseudoType,
	HCallInterface::HPseudoType> : public HCallBase<CLASS_t, METHOD_t>
	{
	typedef HCallBase<CLASS_t, METHOD_t> base_t;
protected:
	a0_t f_xArg0;
	a1_t f_xArg1;
	a2_t f_xArg2;
	a3_t f_xArg3;
	a4_t f_xArg4;
	a5_t f_xArg5;
	a6_t f_xArg6;
	a7_t f_xArg7;
public:
	HCall( CLASS_t obj, METHOD_t A_METHOD, a0_t a0, a1_t a1,
			a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7 )
		: base_t( obj, A_METHOD ),
		f_xArg0( a0 ), f_xArg1( a1 ), f_xArg2( a2 ), f_xArg3( a3 ), f_xArg4( a4 ),
		f_xArg5( a5 ), f_xArg6( a6 ), f_xArg7( a7 ) {}
protected:
	virtual void do_invoke( void )
		{ (base_t::f_oObject.*base_t::METHOD)( f_xArg0, f_xArg1, f_xArg2, f_xArg3, f_xArg4, f_xArg5, f_xArg6, f_xArg7 ); }
	virtual void do_invoke( void ) const
		{ (base_t::f_oObject.*base_t::METHOD)( f_xArg0, f_xArg1, f_xArg2, f_xArg3, f_xArg4, f_xArg5, f_xArg6, f_xArg7 ); }
	};

template<typename CLASS_t, typename METHOD_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t, typename a6_t, typename a7_t,
	typename a8_t>
class HCall<CLASS_t, METHOD_t,
	a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t,
	HCallInterface::HPseudoType> : public HCallBase<CLASS_t, METHOD_t>
	{
	typedef HCallBase<CLASS_t, METHOD_t> base_t;
protected:
	a0_t f_xArg0;
	a1_t f_xArg1;
	a2_t f_xArg2;
	a3_t f_xArg3;
	a4_t f_xArg4;
	a5_t f_xArg5;
	a6_t f_xArg6;
	a7_t f_xArg7;
	a8_t f_xArg8;
public:
	HCall( CLASS_t obj, METHOD_t A_METHOD, a0_t a0, a1_t a1,
			a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7,
			a8_t a8 )
		: base_t( obj, A_METHOD ),
		f_xArg0( a0 ), f_xArg1( a1 ), f_xArg2( a2 ), f_xArg3( a3 ), f_xArg4( a4 ),
		f_xArg5( a5 ), f_xArg6( a6 ), f_xArg7( a7 ), f_xArg8( a8 ) {}
protected:
	virtual void do_invoke( void )
		{ (base_t::f_oObject.*base_t::METHOD)( f_xArg0, f_xArg1, f_xArg2, f_xArg3, f_xArg4, f_xArg5, f_xArg6, f_xArg7, f_xArg8 ); }
	virtual void do_invoke( void ) const
		{ (base_t::f_oObject.*base_t::METHOD)( f_xArg0, f_xArg1, f_xArg2, f_xArg3, f_xArg4, f_xArg5, f_xArg6, f_xArg7, f_xArg8 ); }
	};
/*! \endcond */

/*! \brief Implementation of abstration of any-method of any-class invocation.
 *
 * \tparam CLASS_t - class on which this invocation will operate.
 * \tparam METHOD_t - method of given class CLASS_t that shall be invoked.
 * \tparam a0_t - first argument to given METHOD_t of CLASS_t.
 * \tparam a1_t - second argument to given METHOD_t of CLASS_t.
 * \tparam aN_t - N-th argument to given METHOD_t of CLASS_t.
 */
template<typename CLASS_t, typename METHOD_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t, typename a6_t, typename a7_t,
	typename a8_t, typename a9_t>
class HCall : public HCallBase<CLASS_t, METHOD_t>
	{
	typedef HCallBase<CLASS_t, METHOD_t> base_t;
protected:
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
	HCall( CLASS_t obj, METHOD_t A_METHOD, a0_t a0, a1_t a1,
			a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7,
			a8_t a8, a9_t a9 )
		: base_t( obj, A_METHOD ),
		f_xArg0( a0 ), f_xArg1( a1 ), f_xArg2( a2 ), f_xArg3( a3 ), f_xArg4( a4 ),
		f_xArg5( a5 ), f_xArg6( a6 ), f_xArg7( a7 ), f_xArg8( a8 ), f_xArg9( a9 ) {}
protected:
	virtual void do_invoke( void )
		{ (base_t::f_oObject.*base_t::METHOD)( f_xArg0, f_xArg1, f_xArg2, f_xArg3, f_xArg4, f_xArg5, f_xArg6, f_xArg7, f_xArg8, f_xArg9 ); }
	virtual void do_invoke( void ) const
		{ (base_t::f_oObject.*base_t::METHOD)( f_xArg0, f_xArg1, f_xArg2, f_xArg3, f_xArg4, f_xArg5, f_xArg6, f_xArg7, f_xArg8, f_xArg9 ); }
	};

}

}

#endif /* not YAAL_HCORE_HCALL_HXX_INCLUDED */

