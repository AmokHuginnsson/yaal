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
#define YAAL_HCORE_HBOUNDCALL_HXX_INCLUDED

#include "hcore/hfunctor.hxx"
#include "hcore/hpointer.hxx"

namespace yaal
{

namespace hcore
{

/*! \brief Implementation of abstration of any-method of any-class invocation.
 *
 * \tparam free_args - how many free standing args this call have.
 * \tparam return_t - call result type.
 * \tparam CALL_t - method or function to be invoked.
 * \tparam a0_t - first argument to given METHOD_t of CALL_t.
 * \tparam a1_t - second argument to given METHOD_t of CALL_t.
 * \tparam aN_t - N-th argument to given METHOD_t of CALL_t.
 */
template<int free_args, typename return_t,
	typename a0_t = trait::no_type,	typename a1_t = trait::no_type,
	typename a2_t = trait::no_type,	typename a3_t = trait::no_type,
	typename a4_t = trait::no_type,	typename a5_t = trait::no_type,
	typename a6_t = trait::no_type,	typename a7_t = trait::no_type,
	typename a8_t = trait::no_type,	typename a9_t = trait::no_type,
	typename a10_t = trait::no_type>
class HBoundCallInterface;

/*! \brief Interface to abstration of any-method of any-class invocation.
 */
template<typename return_t>
class HBoundCallInterface<0, return_t,
	trait::no_type, trait::no_type, trait::no_type, trait::no_type,
	trait::no_type, trait::no_type, trait::no_type, trait::no_type,
	trait::no_type,	trait::no_type>
	{
public:
	typedef yaal::hcore::HPointer<HBoundCallInterface> ptr_t;
	virtual ~HBoundCallInterface( void ) {}
	return_t operator()( void )
		{ return ( do_invoke() ); }
	return_t operator()( void ) const
		{ return ( do_invoke() ); }
	return_t invoke( void )
		{ return ( do_invoke() ); }
	return_t invoke( void ) const
		{ return ( do_invoke() ); }
	void const* id( void ) const
		{ return ( do_id() ); }
protected:
	virtual return_t do_invoke( void ) = 0;
	virtual return_t do_invoke( void ) const = 0;
	virtual void const* do_id( void ) const = 0;
	};

template<typename return_t, typename a0_t>
class HBoundCallInterface<1, return_t,
	a0_t, trait::no_type, trait::no_type, trait::no_type,
	trait::no_type, trait::no_type, trait::no_type, trait::no_type,
	trait::no_type,	trait::no_type>
	{
public:
	typedef yaal::hcore::HPointer<HBoundCallInterface> ptr_t;
	virtual ~HBoundCallInterface( void ) {}
	template<typename fa0_t>
	return_t operator()( fa0_t fa0 )
		{ invoke( fa0 ); }
	template<typename fa0_t>
	return_t operator()( fa0_t fa0 ) const
		{ invoke( fa0 ); }
	template<typename fa0_t>
	return_t invoke( fa0_t fa0 )
		{ do_invoke( fa0 ); }
	template<typename fa0_t>
	return_t invoke( fa0_t fa0 ) const
		{ do_invoke( fa0 ); }
	void const* id( void ) const
		{ return ( do_id() ); }
protected:
	virtual return_t do_invoke( void ) = 0;
	virtual return_t do_invoke( void ) const = 0;
	virtual void const* do_id( void ) const = 0;
	};

template<typename CLASS_t, typename METHOD_t>
void const* caller_id( HFunctor<CLASS_t, METHOD_t> const& functor_ )
	{ return ( functor_.id() ); }

template<typename CLASS_t>
void const* caller_id( CLASS_t const& object_ )
	{ return ( &object_ ); }

/*! \brief Base class for HBoundCall<1..10>.
 */
template<int free_args, typename return_t, typename CALL_t,
	typename a0_t,	typename a1_t, typename a2_t, typename a3_t,
	typename a4_t,	typename a5_t, typename a6_t, typename a7_t,
	typename a8_t,	typename a9_t, typename a10_t>
class HBoundCallBase : public HBoundCallInterface<free_args, return_t,
	 a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t, a10_t>
	{
protected:
	CALL_t _call;
public:
	HBoundCallBase( CALL_t call_ ) : _call( call_ ) {}
protected:
	virtual void const* do_id( void ) const
		{ return ( caller_id( _call ) ); }
	};

/*! \brief Implementation of abstration of any-method of any-class invocation.
 *
 * \tparam CALL_t - class on which this invocation will operate.
 * \tparam METHOD_t - method of given class CALL_t that shall be invoked.
 * \tparam a0_t - first argument to given METHOD_t of CALL_t.
 * \tparam a1_t - second argument to given METHOD_t of CALL_t.
 * \tparam aN_t - N-th argument to given METHOD_t of CALL_t.
 */
template<int free_args, typename return_t, typename CALL_t,
	typename a0_t = trait::no_type,	typename a1_t = trait::no_type,
	typename a2_t = trait::no_type,	typename a3_t = trait::no_type,
	typename a4_t = trait::no_type,	typename a5_t = trait::no_type,
	typename a6_t = trait::no_type,	typename a7_t = trait::no_type,
	typename a8_t = trait::no_type,	typename a9_t = trait::no_type,
	typename a10_t = trait::no_type>
class HBoundCall;

template<typename return_t, typename CALL_t>
class HBoundCall<0, return_t, CALL_t,
	trait::no_type, trait::no_type, trait::no_type, trait::no_type,
	trait::no_type, trait::no_type, trait::no_type, trait::no_type,
	trait::no_type,	trait::no_type>
		: public HBoundCallBase<0, return_t, CALL_t,
	trait::no_type, trait::no_type, trait::no_type, trait::no_type,
	trait::no_type, trait::no_type, trait::no_type, trait::no_type,
	trait::no_type,	trait::no_type, trait::no_type>
	{
	typedef HBoundCallBase<0, return_t, CALL_t,
	trait::no_type, trait::no_type, trait::no_type, trait::no_type,
	trait::no_type, trait::no_type, trait::no_type, trait::no_type,
	trait::no_type,	trait::no_type, trait::no_type> base_t;
public:
	HBoundCall( CALL_t call_ )
		: base_t( call_ ) {}
	virtual return_t do_invoke( void )
		{ return ( (base_t::_call)() ); }
	virtual return_t do_invoke( void ) const
		{ return ( (base_t::_call)() ); }
	};

template<typename return_t, typename CALL_t, typename a0_t>
class HBoundCall<0, return_t, CALL_t, a0_t,
	trait::no_type,	trait::no_type, trait::no_type, trait::no_type,
	trait::no_type,	trait::no_type, trait::no_type, trait::no_type,
	trait::no_type>
		: public HBoundCallBase<0, return_t, CALL_t,
	a0_t, trait::no_type, trait::no_type, trait::no_type,
	trait::no_type, trait::no_type, trait::no_type, trait::no_type,
	trait::no_type,	trait::no_type, trait::no_type>
	{
	typedef HBoundCallBase<0, return_t, CALL_t,
	a0_t, trait::no_type, trait::no_type, trait::no_type,
	trait::no_type, trait::no_type, trait::no_type, trait::no_type,
	trait::no_type,	trait::no_type, trait::no_type> base_t;
	a0_t _a0;
public:
	HBoundCall( CALL_t call_, a0_t a0 )
		: base_t( call_ ), _a0( a0 ) {}
	virtual return_t do_invoke( void )
		{ return ( (base_t::_call)( _a0 ) ); }
	virtual return_t do_invoke( void ) const
		{ return ( (base_t::_call)( _a0 ) ); }
	};

template<typename return_t, typename CALL_t, typename a0_t, typename a1_t>
class HBoundCall<0, return_t, CALL_t, a0_t, a1_t,
	trait::no_type,	trait::no_type, trait::no_type, trait::no_type,
	trait::no_type,	trait::no_type, trait::no_type, trait::no_type>
		: public HBoundCallBase<0, return_t, CALL_t,
	a0_t, a1_t, trait::no_type, trait::no_type,
	trait::no_type, trait::no_type, trait::no_type, trait::no_type,
	trait::no_type,	trait::no_type, trait::no_type>
	{
	typedef HBoundCallBase<0, return_t, CALL_t,
	a0_t, a1_t, trait::no_type, trait::no_type,
	trait::no_type, trait::no_type, trait::no_type, trait::no_type,
	trait::no_type,	trait::no_type, trait::no_type> base_t;
	a0_t _a0;
	a1_t _a1;
public:
	HBoundCall( CALL_t call_, a0_t a0, a1_t a1 )
		: base_t( call_ ), _a0( a0 ), _a1( a1 ) {}
	virtual return_t do_invoke( void )
		{ return ( (base_t::_call)( _a0, _a1 ) ); }
	virtual return_t do_invoke( void ) const
		{ return ( (base_t::_call)( _a0, _a1 ) ); }
	};

template<typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t>
class HBoundCall<0, return_t, CALL_t, a0_t, a1_t, a2_t,
	trait::no_type,	trait::no_type, trait::no_type, trait::no_type,
	trait::no_type,	trait::no_type, trait::no_type>
		: public HBoundCallBase<0, return_t, CALL_t,
	a0_t, a1_t, a2_t, trait::no_type,
	trait::no_type, trait::no_type, trait::no_type, trait::no_type,
	trait::no_type,	trait::no_type, trait::no_type>
	{
	typedef HBoundCallBase<0, return_t, CALL_t,
	a0_t, a1_t, a2_t, trait::no_type,
	trait::no_type, trait::no_type, trait::no_type, trait::no_type,
	trait::no_type,	trait::no_type, trait::no_type> base_t;
	a0_t _a0;
	a1_t _a1;
	a2_t _a2;
public:
	HBoundCall( CALL_t call_, a0_t a0, a1_t a1, a2_t a2 )
		: base_t( call_ ), _a0( a0 ), _a1( a1 ), _a2( a2 ) {}
	virtual return_t do_invoke( void )
		{ return ( (base_t::_call)( _a0, _a1, _a2 ) ); }
	virtual return_t do_invoke( void ) const
		{ return ( (base_t::_call)( _a0, _a1, _a2 ) ); }
	};

template<typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t>
class HBoundCall<0, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t,
	trait::no_type,	trait::no_type, trait::no_type, trait::no_type,
	trait::no_type,	trait::no_type>
		: public HBoundCallBase<0, return_t, CALL_t,
	a0_t, a1_t, a2_t, a3_t,
	trait::no_type, trait::no_type, trait::no_type, trait::no_type,
	trait::no_type,	trait::no_type, trait::no_type>
	{
	typedef HBoundCallBase<0, return_t, CALL_t,
	a0_t, a1_t, a2_t, a3_t,
	trait::no_type, trait::no_type, trait::no_type, trait::no_type,
	trait::no_type,	trait::no_type, trait::no_type> base_t;
	a0_t _a0;
	a1_t _a1;
	a2_t _a2;
	a3_t _a3;
public:
	HBoundCall( CALL_t call_, a0_t a0, a1_t a1, a2_t a2, a3_t a3 )
		: base_t( call_ ), _a0( a0 ), _a1( a1 ), _a2( a2 ), _a3( a3 ) {}
	virtual return_t do_invoke( void )
		{ return ( (base_t::_call)( _a0, _a1, _a2, _a3 ) ); }
	virtual return_t do_invoke( void ) const
		{ return ( (base_t::_call)( _a0, _a1, _a2, _a3 ) ); }
	};

template<typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t>
class HBoundCall<0, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t,
	trait::no_type,	trait::no_type, trait::no_type, trait::no_type,
	trait::no_type>
		: public HBoundCallBase<0, return_t, CALL_t,
	a0_t, a1_t, a2_t, a3_t,
	a4_t, trait::no_type, trait::no_type, trait::no_type,
	trait::no_type,	trait::no_type, trait::no_type>
	{
	typedef HBoundCallBase<0, return_t, CALL_t,
	a0_t, a1_t, a2_t, a3_t,
	a4_t, trait::no_type, trait::no_type, trait::no_type,
	trait::no_type,	trait::no_type, trait::no_type> base_t;
	a0_t _a0;
	a1_t _a1;
	a2_t _a2;
	a3_t _a3;
	a4_t _a4;
public:
	HBoundCall( CALL_t call_, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4 )
		: base_t( call_ ), _a0( a0 ), _a1( a1 ), _a2( a2 ), _a3( a3 ), _a4( a4 ) {}
	virtual return_t do_invoke( void )
		{ return ( (base_t::_call)( _a0, _a1, _a2, _a3, _a4 ) ); }
	virtual return_t do_invoke( void ) const
		{ return ( (base_t::_call)( _a0, _a1, _a2, _a3, _a4 ) ); }
	};

template<typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t>
class HBoundCall<0, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t,
	trait::no_type,	trait::no_type, trait::no_type, trait::no_type>
		: public HBoundCallBase<0, return_t, CALL_t,
	a0_t, a1_t, a2_t, a3_t,
	a4_t, a5_t, trait::no_type, trait::no_type,
	trait::no_type,	trait::no_type, trait::no_type>
	{
	typedef HBoundCallBase<0, return_t, CALL_t,
	a0_t, a1_t, a2_t, a3_t,
	a4_t, a5_t, trait::no_type, trait::no_type,
	trait::no_type,	trait::no_type, trait::no_type> base_t;
	a0_t _a0;
	a1_t _a1;
	a2_t _a2;
	a3_t _a3;
	a4_t _a4;
	a5_t _a5;
public:
	HBoundCall( CALL_t call_, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5 )
		: base_t( call_ ), _a0( a0 ), _a1( a1 ), _a2( a2 ), _a3( a3 ), _a4( a4 ), _a5( a5 ) {}
	virtual return_t do_invoke( void )
		{ return ( (base_t::_call)( _a0, _a1, _a2, _a3, _a4, _a5 ) ); }
	virtual return_t do_invoke( void ) const
		{ return ( (base_t::_call)( _a0, _a1, _a2, _a3, _a4, _a5 ) ); }
	};

template<typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t, typename a6_t>
class HBoundCall<0, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t,
	trait::no_type,	trait::no_type, trait::no_type>
		: public HBoundCallBase<0, return_t, CALL_t,
	a0_t, a1_t, a2_t, a3_t,
	a4_t, a5_t, a6_t, trait::no_type,
	trait::no_type,	trait::no_type, trait::no_type>
	{
	typedef HBoundCallBase<0, return_t, CALL_t,
	a0_t, a1_t, a2_t, a3_t,
	a4_t, a5_t, a6_t, trait::no_type,
	trait::no_type,	trait::no_type, trait::no_type> base_t;
	a0_t _a0;
	a1_t _a1;
	a2_t _a2;
	a3_t _a3;
	a4_t _a4;
	a5_t _a5;
	a6_t _a6;
public:
	HBoundCall( CALL_t call_, a0_t a0, a1_t a1,
			a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6 )
		: base_t( call_ ), _a0( a0 ), _a1( a1 ), _a2( a2 ), _a3( a3 ), _a4( a4 ), _a5( a5 ), _a6( a6 ) {}
	virtual return_t do_invoke( void )
		{ return ( (base_t::_call)( _a0, _a1, _a2, _a3, _a4, _a5, _a6 ) ); }
	virtual return_t do_invoke( void ) const
		{ return ( (base_t::_call)( _a0, _a1, _a2, _a3, _a4, _a5, _a6 ) ); }
	};

template<typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t, typename a6_t, typename a7_t>
class HBoundCall<0, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t,
	trait::no_type, trait::no_type>
		: public HBoundCallBase<0, return_t, CALL_t,
	a0_t, a1_t, a2_t, a3_t,
	a4_t, a5_t, a6_t, a7_t,
	trait::no_type,	trait::no_type, trait::no_type>
	{
	typedef HBoundCallBase<0, return_t, CALL_t,
	a0_t, a1_t, a2_t, a3_t,
	a4_t, a5_t, a6_t, a7_t,
	trait::no_type,	trait::no_type, trait::no_type> base_t;
	a0_t _a0;
	a1_t _a1;
	a2_t _a2;
	a3_t _a3;
	a4_t _a4;
	a5_t _a5;
	a6_t _a6;
	a7_t _a7;
public:
	HBoundCall( CALL_t call_, a0_t a0, a1_t a1,
			a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7 )
		: base_t( call_ ), _a0( a0 ), _a1( a1 ), _a2( a2 ), _a3( a3 ), _a4( a4 ), _a5( a5 ), _a6( a6 ), _a7( a7 ) {}
	virtual return_t do_invoke( void )
		{ return ( (base_t::_call)( _a0, _a1, _a2, _a3, _a4, _a5, _a6, _a7 ) ); }
	virtual return_t do_invoke( void ) const
		{ return ( (base_t::_call)( _a0, _a1, _a2, _a3, _a4, _a5, _a6, _a7 ) ); }
	};

template<typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t, typename a6_t, typename a7_t,
	typename a8_t>
class HBoundCall<0, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t,
	trait::no_type>
		: public HBoundCallBase<0, return_t, CALL_t,
	a0_t, a1_t, a2_t, a3_t,
	a4_t, a5_t, a6_t, a7_t,
	a8_t,	trait::no_type, trait::no_type>
	{
	typedef HBoundCallBase<0, return_t, CALL_t,
	a0_t, a1_t, a2_t, a3_t,
	a4_t, a5_t, a6_t, a7_t,
	a8_t,	trait::no_type, trait::no_type> base_t;
	a0_t _a0;
	a1_t _a1;
	a2_t _a2;
	a3_t _a3;
	a4_t _a4;
	a5_t _a5;
	a6_t _a6;
	a7_t _a7;
	a8_t _a8;
public:
	HBoundCall( CALL_t call_, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7, a8_t a8 )
		: base_t( call_ ), _a0( a0 ), _a1( a1 ), _a2( a2 ), _a3( a3 ), _a4( a4 ), _a5( a5 ), _a6( a6 ), _a7( a7 ), _a8( a8 ) {}
	virtual return_t do_invoke( void )
		{ return ( (base_t::_call)( _a0, _a1, _a2, _a3, _a4, _a5, _a6, _a7, _a8 ) ); }
	virtual return_t do_invoke( void ) const
		{ return ( (base_t::_call)( _a0, _a1, _a2, _a3, _a4, _a5, _a6, _a7, _a8 ) ); }
	};

template<typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t, typename a6_t, typename a7_t,
	typename a8_t, typename a9_t>
class HBoundCall<0, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t>
		: public HBoundCallBase<0, return_t, CALL_t,
	a0_t, a1_t, a2_t, a3_t,
	a4_t, a5_t, a6_t, a7_t,
	a8_t,	a9_t, trait::no_type>
	{
	typedef HBoundCallBase<0, return_t, CALL_t,
	a0_t, a1_t, a2_t, a3_t,
	a4_t, a5_t, a6_t, a7_t,
	a8_t,	a9_t, trait::no_type> base_t;
	a0_t _a0;
	a1_t _a1;
	a2_t _a2;
	a3_t _a3;
	a4_t _a4;
	a5_t _a5;
	a6_t _a6;
	a7_t _a7;
	a8_t _a8;
	a9_t _a9;
public:
	HBoundCall( CALL_t call_, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7, a8_t a8, a9_t a9 )
		: base_t( call_ ), _a0( a0 ), _a1( a1 ), _a2( a2 ), _a3( a3 ), _a4( a4 ), _a5( a5 ), _a6( a6 ), _a7( a7 ), _a8( a8 ), _a9( a9 ) {}
	virtual return_t do_invoke( void )
		{ return ( (base_t::_call)( _a0, _a1, _a2, _a3, _a4, _a5, _a6, _a7, _a8, _a9 ) ); }
	virtual return_t do_invoke( void ) const
		{ return ( (base_t::_call)( _a0, _a1, _a2, _a3, _a4, _a5, _a6, _a7, _a8, _a9 ) ); }
	};

template<typename METHOD_t,
	typename a0_t = trait::no_type, typename a1_t = trait::no_type,
	typename a2_t = trait::no_type, typename a3_t = trait::no_type,
	typename a4_t = trait::no_type, typename a5_t = trait::no_type,
	typename a6_t = trait::no_type, typename a7_t = trait::no_type,
	typename a8_t = trait::no_type, typename a9_t = trait::no_type, typename a10_t = trait::no_type>
struct bound_call_calculator
	{
	struct free_standing_args
		{
		static int const value =
			+ meta::ternary<trait::is_kind_of<a0_t, free_standing_call_args::arg_base>::value, 1, 0>::value
			+ meta::ternary<trait::is_kind_of<a1_t, free_standing_call_args::arg_base>::value, 1, 0>::value
			+ meta::ternary<trait::is_kind_of<a2_t, free_standing_call_args::arg_base>::value, 1, 0>::value
			+ meta::ternary<trait::is_kind_of<a3_t, free_standing_call_args::arg_base>::value, 1, 0>::value
			+ meta::ternary<trait::is_kind_of<a4_t, free_standing_call_args::arg_base>::value, 1, 0>::value
			+ meta::ternary<trait::is_kind_of<a5_t, free_standing_call_args::arg_base>::value, 1, 0>::value
			+ meta::ternary<trait::is_kind_of<a6_t, free_standing_call_args::arg_base>::value, 1, 0>::value
			+ meta::ternary<trait::is_kind_of<a7_t, free_standing_call_args::arg_base>::value, 1, 0>::value
			+ meta::ternary<trait::is_kind_of<a8_t, free_standing_call_args::arg_base>::value, 1, 0>::value
			+ meta::ternary<trait::is_kind_of<a9_t, free_standing_call_args::arg_base>::value, 1, 0>::value
			+ meta::ternary<trait::is_kind_of<a10_t, free_standing_call_args::arg_base>::value, 1, 0>::value;
		};
	struct functor
		{
		typedef HFunctor<a0_t, METHOD_t> functor_t;
		typedef HBoundCall<free_standing_args::value, typename trait::return_type<METHOD_t>::type,
						functor_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t, a10_t> type;
		typedef typename HBoundCallInterface<free_standing_args::value, typename trait::return_type<METHOD_t>::type,
						a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t, a10_t>::ptr_t holder_type;
		inline static holder_type make( METHOD_t m, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7, a8_t a8, a9_t a9, a10_t a10 )
			{ return holder_type( new type( functor_t( a0, m ), a1, a2, a3, a4, a5, a6, a7, a8, a9, a10 ) ); }
		inline static holder_type make( METHOD_t m, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7, a8_t a8, a9_t a9 )
			{ return holder_type( new type( functor_t( a0, m ), a1, a2, a3, a4, a5, a6, a7, a8, a9 ) ); }
		inline static holder_type make( METHOD_t m, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7, a8_t a8 )
			{ return holder_type( new type( functor_t( a0, m ), a1, a2, a3, a4, a5, a6, a7, a8 ) ); }
		inline static holder_type make( METHOD_t m, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7 )
			{ return holder_type( new type( functor_t( a0, m ), a1, a2, a3, a4, a5, a6, a7 ) ); }
		inline static holder_type make( METHOD_t m, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6 )
			{ return holder_type( new type( functor_t( a0, m ), a1, a2, a3, a4, a5, a6 ) ); }
		inline static holder_type make( METHOD_t m, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5 )
			{ return holder_type( new type( functor_t( a0, m ), a1, a2, a3, a4, a5 ) ); }
		inline static holder_type make( METHOD_t m, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4 )
			{ return holder_type( new type( functor_t( a0, m ), a1, a2, a3, a4 ) ); }
		inline static holder_type make( METHOD_t m, a0_t a0, a1_t a1, a2_t a2, a3_t a3 )
			{ return holder_type( new type( functor_t( a0, m ), a1, a2, a3 ) ); }
		inline static holder_type make( METHOD_t m, a0_t a0, a1_t a1, a2_t a2 )
			{ return holder_type( new type( functor_t( a0, m ), a1, a2 ) ); }
		inline static holder_type make( METHOD_t m, a0_t a0, a1_t a1 )
			{ return holder_type( new type( functor_t( a0, m ), a1 ) ); }
		inline static holder_type make( METHOD_t m, a0_t a0 )
			{ return holder_type( new type( functor_t( a0, m ) ) ); }
		};
	struct functor_this
		{
		typedef HFunctor<a0_t, METHOD_t> functor_t;
		typedef HBoundCall<free_standing_args::value, typename trait::return_type<METHOD_t>::type,
						functor_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t, a10_t> type;
		typedef HBoundCallInterface<free_standing_args::value, typename trait::return_type<METHOD_t>::type,
						a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t, a10_t> holder_type;
		inline static holder_type make( METHOD_t m, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7, a8_t a8, a9_t a9, a10_t a10 )
			{ return holder_type( new type( functor_t( a0, m ), a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10 ) ); }
		inline static holder_type make( METHOD_t m, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7, a8_t a8, a9_t a9 )
			{ return holder_type( new type( functor_t( a0, m ), a0, a1, a2, a3, a4, a5, a6, a7, a8, a9 ) ); }
		inline static holder_type make( METHOD_t m, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7, a8_t a8 )
			{ return holder_type( new type( functor_t( a0, m ), a0, a1, a2, a3, a4, a5, a6, a7, a8 ) ); }
		inline static holder_type make( METHOD_t m, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7 )
			{ return holder_type( new type( functor_t( a0, m ), a0, a1, a2, a3, a4, a5, a6, a7 ) ); }
		inline static holder_type make( METHOD_t m, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6 )
			{ return holder_type( new type( functor_t( a0, m ), a0, a1, a2, a3, a4, a5, a6 ) ); }
		inline static holder_type make( METHOD_t m, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5 )
			{ return holder_type( new type( functor_t( a0, m ), a0, a1, a2, a3, a4, a5 ) ); }
		inline static holder_type make( METHOD_t m, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4 )
			{ return holder_type( new type( functor_t( a0, m ), a0, a1, a2, a3, a4 ) ); }
		inline static holder_type make( METHOD_t m, a0_t a0, a1_t a1, a2_t a2, a3_t a3 )
			{ return holder_type( new type( functor_t( a0, m ), a0, a1, a2, a3 ) ); }
		inline static holder_type make( METHOD_t m, a0_t a0, a1_t a1, a2_t a2 )
			{ return holder_type( new type( functor_t( a0, m ), a0, a1, a2 ) ); }
		inline static holder_type make( METHOD_t m, a0_t a0, a1_t a1 )
			{ return holder_type( new type( functor_t( a0, m ), a0, a1 ) ); }
		inline static holder_type make( METHOD_t m, a0_t a0 )
			{ return holder_type( new type( functor_t( a0, m ), a0 ) ); }
		};
	struct function
		{
		typedef HBoundCall<free_standing_args::value, typename trait::return_type<METHOD_t>::type,
						METHOD_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t> type;
		typedef HBoundCall<free_standing_args::value, typename trait::return_type<METHOD_t>::type,
						a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t> holder_type;
		inline static holder_type make( METHOD_t m, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7, a8_t a8, a9_t a9 )
			{ return holder_type( new type( m, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9 ) ); }
		inline static holder_type make( METHOD_t m, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7, a8_t a8 )
			{ return holder_type( new type( m, a0, a1, a2, a3, a4, a5, a6, a7, a8 ) ); }
		inline static holder_type make( METHOD_t m, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7 )
			{ return holder_type( new type( m, a0, a1, a2, a3, a4, a5, a6, a7 ) ); }
		inline static holder_type make( METHOD_t m, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6 )
			{ return holder_type( new type( m, a0, a1, a2, a3, a4, a5, a6 ) ); }
		inline static holder_type make( METHOD_t m, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5 )
			{ return holder_type( new type( m, a0, a1, a2, a3, a4, a5 ) ); }
		inline static holder_type make( METHOD_t m, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4 )
			{ return holder_type( new type( m, a0, a1, a2, a3, a4 ) ); }
		inline static holder_type make( METHOD_t m, a0_t a0, a1_t a1, a2_t a2, a3_t a3 )
			{ return holder_type( new type( m, a0, a1, a2, a3 ) ); }
		inline static holder_type make( METHOD_t m, a0_t a0, a1_t a1, a2_t a2 )
			{ return holder_type( new type( m, a0, a1, a2 ) ); }
		inline static holder_type make( METHOD_t m, a0_t a0, a1_t a1 )
			{ return holder_type( new type( m, a0, a1 ) ); }
		inline static holder_type make( METHOD_t m, a0_t a0 )
			{ return holder_type( new type( m, a0 ) ); }
		inline static holder_type make( METHOD_t m )
			{ return holder_type( new type( m ) ); }
		};
	typedef typename trait::ternary<trait::is_member<METHOD_t>::value,
					typename trait::ternary<meta::greater<trait::find_type<a0_t,
					free_standing_call_args::arg<1>,
					free_standing_call_args::arg<2>,
					free_standing_call_args::arg<3>,
					free_standing_call_args::arg<4>,
					free_standing_call_args::arg<5>,
					free_standing_call_args::arg<6>,
					free_standing_call_args::arg<7>,
					free_standing_call_args::arg<8>,
					free_standing_call_args::arg<9>,
					free_standing_call_args::arg<10> >::value, -1>::value, functor_this, functor>::type,
					function>::type type;
	};

#if 0

/*! \cond */

template<typename return_t, typename CALL_t, typename a0_t>
class HBoundCall<1, return_t, CALL_t, a0_t,
	trait::no_type,	trait::no_type, trait::no_type, trait::no_type,
	trait::no_type,	trait::no_type, trait::no_type, trait::no_type,
	trait::no_type>
	{
	CALL_t _call;
	a0_t _a0;
public:
	HBoundCall( CALL_t call_, a0_t a0 )
		: _call( call_ ), _a0( a0 ) {}
	template<typename fa0_t>
	return_t operator()( fa0_t a0 ) { return ( _call( free_standing_call_args::resolve_arg<1, a0_t, fa0_t>::get( _a0, a0 ) ) ); }
	template<typename fa0_t>
	return_t operator()( fa0_t a0 ) const { return ( _call( free_standing_call_args::resolve_arg<1, a0_t, fa0_t>::get( _a0, a0 ) ) ); }
	};

template<typename return_t, typename CALL_t, typename a0_t, typename a1_t>
class HBoundCall<1, return_t, CALL_t, a0_t, a1_t,
	trait::no_type,	trait::no_type, trait::no_type, trait::no_type,
	trait::no_type,	trait::no_type, trait::no_type, trait::no_type>
	{
	CALL_t _call;
	a0_t _a0;
	a1_t _a1;
public:
	HBoundCall( CALL_t call_, a0_t a0, a1_t a1 )
		: _call( call_ ), _a0( a0 ), _a1( a1 ) {}
	template<typename fa0_t>
	return_t operator()( fa0_t a0 ) { return ( _call(
				free_standing_call_args::resolve_arg<1, a0_t, fa0_t>::get( _a0, a0 ),
				free_standing_call_args::resolve_arg<1, a1_t, fa0_t>::get( _a1, a0 ) ) ); }
	template<typename fa0_t>
	return_t operator()( fa0_t a0 ) const { return ( _call(
				free_standing_call_args::resolve_arg<1, a0_t, fa0_t>::get( _a0, a0 ),
				free_standing_call_args::resolve_arg<1, a1_t, fa0_t>::get( _a1, a0 ) ) ); }
	};

template<typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t>
class HBoundCall<1, return_t, CALL_t, a0_t, a1_t, a2_t,
	trait::no_type,	trait::no_type, trait::no_type, trait::no_type,
	trait::no_type,	trait::no_type, trait::no_type>
	{
	CALL_t _call;
	a0_t _a0;
	a1_t _a1;
	a2_t _a2;
public:
	HBoundCall( CALL_t call_, a0_t a0, a1_t a1, a2_t a2 )
		: _call( call_ ), _a0( a0 ), _a1( a1 ), _a2( a2 ) {}
	template<typename fa0_t>
	return_t operator()( fa0_t a0 ) { return ( _call(
				free_standing_call_args::resolve_arg<1, a0_t, fa0_t>::get( _a0, a0 ),
				free_standing_call_args::resolve_arg<1, a1_t, fa0_t>::get( _a1, a0 ),
				free_standing_call_args::resolve_arg<1, a2_t, fa0_t>::get( _a2, a0 ) ) ); }
	template<typename fa0_t>
	return_t operator()( fa0_t a0 ) const { return ( _call(
				free_standing_call_args::resolve_arg<1, a0_t, fa0_t>::get( _a0, a0 ),
				free_standing_call_args::resolve_arg<1, a1_t, fa0_t>::get( _a1, a0 ),
				free_standing_call_args::resolve_arg<1, a2_t, fa0_t>::get( _a2, a0 ) ) ); }
	};

template<typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t>
class HBoundCall<1, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t,
	trait::no_type,	trait::no_type, trait::no_type, trait::no_type,
	trait::no_type,	trait::no_type>
	{
	CALL_t _call;
	a0_t _a0;
	a1_t _a1;
	a2_t _a2;
	a3_t _a3;
public:
	HBoundCall( CALL_t call_, a0_t a0, a1_t a1, a2_t a2, a3_t a3 )
		: _call( call_ ), _a0( a0 ), _a1( a1 ), _a2( a2 ), _a3( a3 ) {}
	template<typename fa0_t>
	return_t operator()( fa0_t a0 ) { return ( _call(
				free_standing_call_args::resolve_arg<1, a0_t, fa0_t>::get( _a0, a0 ),
				free_standing_call_args::resolve_arg<1, a1_t, fa0_t>::get( _a1, a0 ),
				free_standing_call_args::resolve_arg<1, a2_t, fa0_t>::get( _a2, a0 ),
				free_standing_call_args::resolve_arg<1, a3_t, fa0_t>::get( _a3, a0 ) ) ); }
	template<typename fa0_t>
	return_t operator()( fa0_t a0 ) const { return ( _call(
				free_standing_call_args::resolve_arg<1, a0_t, fa0_t>::get( _a0, a0 ),
				free_standing_call_args::resolve_arg<1, a1_t, fa0_t>::get( _a1, a0 ),
				free_standing_call_args::resolve_arg<1, a2_t, fa0_t>::get( _a2, a0 ),
				free_standing_call_args::resolve_arg<1, a3_t, fa0_t>::get( _a3, a0 ) ) ); }
	};

template<typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t>
class HBoundCall<1, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t,
	trait::no_type,	trait::no_type, trait::no_type, trait::no_type,
	trait::no_type>
	{
	CALL_t _call;
	a0_t _a0;
	a1_t _a1;
	a2_t _a2;
	a3_t _a3;
	a4_t _a4;
public:
	HBoundCall( CALL_t call_, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4 )
		: _call( call_ ), _a0( a0 ), _a1( a1 ), _a2( a2 ), _a3( a3 ), _a4( a4 ) {}
	template<typename fa0_t>
	return_t operator()( fa0_t a0 ) { return ( _call(
				free_standing_call_args::resolve_arg<1, a0_t, fa0_t>::get( _a0, a0 ),
				free_standing_call_args::resolve_arg<1, a1_t, fa0_t>::get( _a1, a0 ),
				free_standing_call_args::resolve_arg<1, a2_t, fa0_t>::get( _a2, a0 ),
				free_standing_call_args::resolve_arg<1, a3_t, fa0_t>::get( _a3, a0 ),
				free_standing_call_args::resolve_arg<1, a4_t, fa0_t>::get( _a4, a0 ) ) ); }
	template<typename fa0_t>
	return_t operator()( fa0_t a0 ) const { return ( _call(
				free_standing_call_args::resolve_arg<1, a0_t, fa0_t>::get( _a0, a0 ),
				free_standing_call_args::resolve_arg<1, a1_t, fa0_t>::get( _a1, a0 ),
				free_standing_call_args::resolve_arg<1, a2_t, fa0_t>::get( _a2, a0 ),
				free_standing_call_args::resolve_arg<1, a3_t, fa0_t>::get( _a3, a0 ),
				free_standing_call_args::resolve_arg<1, a4_t, fa0_t>::get( _a4, a0 ) ) ); }
	};

template<typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t>
class HBoundCall<1, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t,
	trait::no_type,	trait::no_type, trait::no_type, trait::no_type>
	{
	CALL_t _call;
	a0_t _a0;
	a1_t _a1;
	a2_t _a2;
	a3_t _a3;
	a4_t _a4;
	a5_t _a5;
public:
	HBoundCall( CALL_t call_, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5 )
		: _call( call_ ), _a0( a0 ), _a1( a1 ), _a2( a2 ), _a3( a3 ), _a4( a4 ), _a5( a5 ) {}
	template<typename fa0_t>
	return_t operator()( fa0_t a0 ) { return ( _call(
				free_standing_call_args::resolve_arg<1, a0_t, fa0_t>::get( _a0, a0 ),
				free_standing_call_args::resolve_arg<1, a1_t, fa0_t>::get( _a1, a0 ),
				free_standing_call_args::resolve_arg<1, a2_t, fa0_t>::get( _a2, a0 ),
				free_standing_call_args::resolve_arg<1, a3_t, fa0_t>::get( _a3, a0 ),
				free_standing_call_args::resolve_arg<1, a4_t, fa0_t>::get( _a4, a0 ),
				free_standing_call_args::resolve_arg<1, a5_t, fa0_t>::get( _a5, a0 ) ) ); }
	template<typename fa0_t>
	return_t operator()( fa0_t a0 ) const { return ( _call(
				free_standing_call_args::resolve_arg<1, a0_t, fa0_t>::get( _a0, a0 ),
				free_standing_call_args::resolve_arg<1, a1_t, fa0_t>::get( _a1, a0 ),
				free_standing_call_args::resolve_arg<1, a2_t, fa0_t>::get( _a2, a0 ),
				free_standing_call_args::resolve_arg<1, a3_t, fa0_t>::get( _a3, a0 ),
				free_standing_call_args::resolve_arg<1, a4_t, fa0_t>::get( _a4, a0 ),
				free_standing_call_args::resolve_arg<1, a5_t, fa0_t>::get( _a5, a0 ) ) ); }
	};

template<typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t, typename a6_t>
class HBoundCall<1, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t,
	trait::no_type,	trait::no_type, trait::no_type>
	{
	CALL_t _call;
	a0_t _a0;
	a1_t _a1;
	a2_t _a2;
	a3_t _a3;
	a4_t _a4;
	a5_t _a5;
	a6_t _a6;
public:
	HBoundCall( CALL_t call_, a0_t a0, a1_t a1,
			a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6 )
		: _call( call_ ), _a0( a0 ), _a1( a1 ), _a2( a2 ), _a3( a3 ), _a4( a4 ), _a5( a5 ), _a6( a6 ) {}
	template<typename fa0_t>
	return_t operator()( fa0_t a0 ) { return ( _call(
				free_standing_call_args::resolve_arg<1, a0_t, fa0_t>::get( _a0, a0 ),
				free_standing_call_args::resolve_arg<1, a1_t, fa0_t>::get( _a1, a0 ),
				free_standing_call_args::resolve_arg<1, a2_t, fa0_t>::get( _a2, a0 ),
				free_standing_call_args::resolve_arg<1, a3_t, fa0_t>::get( _a3, a0 ),
				free_standing_call_args::resolve_arg<1, a4_t, fa0_t>::get( _a4, a0 ),
				free_standing_call_args::resolve_arg<1, a5_t, fa0_t>::get( _a5, a0 ),
				free_standing_call_args::resolve_arg<1, a6_t, fa0_t>::get( _a6, a0 ) ) ); }
	template<typename fa0_t>
	return_t operator()( fa0_t a0 ) const { return ( _call(
				free_standing_call_args::resolve_arg<1, a0_t, fa0_t>::get( _a0, a0 ),
				free_standing_call_args::resolve_arg<1, a1_t, fa0_t>::get( _a1, a0 ),
				free_standing_call_args::resolve_arg<1, a2_t, fa0_t>::get( _a2, a0 ),
				free_standing_call_args::resolve_arg<1, a3_t, fa0_t>::get( _a3, a0 ),
				free_standing_call_args::resolve_arg<1, a4_t, fa0_t>::get( _a4, a0 ),
				free_standing_call_args::resolve_arg<1, a5_t, fa0_t>::get( _a5, a0 ),
				free_standing_call_args::resolve_arg<1, a6_t, fa0_t>::get( _a6, a0 ) ) ); }
	};

template<typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t, typename a6_t, typename a7_t>
class HBoundCall<1, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t,
	trait::no_type, trait::no_type>
	{
	CALL_t _call;
	a0_t _a0;
	a1_t _a1;
	a2_t _a2;
	a3_t _a3;
	a4_t _a4;
	a5_t _a5;
	a6_t _a6;
	a7_t _a7;
public:
	HBoundCall( CALL_t call_, a0_t a0, a1_t a1,
			a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7 )
		: _call( call_ ), _a0( a0 ), _a1( a1 ), _a2( a2 ), _a3( a3 ), _a4( a4 ), _a5( a5 ), _a6( a6 ), _a7( a7 ) {}
	template<typename fa0_t>
	return_t operator()( fa0_t a0 ) { return ( _call(
				free_standing_call_args::resolve_arg<1, a0_t, fa0_t>::get( _a0, a0 ),
				free_standing_call_args::resolve_arg<1, a1_t, fa0_t>::get( _a1, a0 ),
				free_standing_call_args::resolve_arg<1, a2_t, fa0_t>::get( _a2, a0 ),
				free_standing_call_args::resolve_arg<1, a3_t, fa0_t>::get( _a3, a0 ),
				free_standing_call_args::resolve_arg<1, a4_t, fa0_t>::get( _a4, a0 ),
				free_standing_call_args::resolve_arg<1, a5_t, fa0_t>::get( _a5, a0 ),
				free_standing_call_args::resolve_arg<1, a6_t, fa0_t>::get( _a6, a0 ),
				free_standing_call_args::resolve_arg<1, a7_t, fa0_t>::get( _a7, a0 ) ) ); }
	template<typename fa0_t>
	return_t operator()( fa0_t a0 ) const { return ( _call(
				free_standing_call_args::resolve_arg<1, a0_t, fa0_t>::get( _a0, a0 ),
				free_standing_call_args::resolve_arg<1, a1_t, fa0_t>::get( _a1, a0 ),
				free_standing_call_args::resolve_arg<1, a2_t, fa0_t>::get( _a2, a0 ),
				free_standing_call_args::resolve_arg<1, a3_t, fa0_t>::get( _a3, a0 ),
				free_standing_call_args::resolve_arg<1, a4_t, fa0_t>::get( _a4, a0 ),
				free_standing_call_args::resolve_arg<1, a5_t, fa0_t>::get( _a5, a0 ),
				free_standing_call_args::resolve_arg<1, a6_t, fa0_t>::get( _a6, a0 ),
				free_standing_call_args::resolve_arg<1, a7_t, fa0_t>::get( _a7, a0 ) ) ); }
	};

template<typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t, typename a6_t, typename a7_t,
	typename a8_t>
class HBoundCall<1, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t,
	trait::no_type>
	{
	CALL_t _call;
	a0_t _a0;
	a1_t _a1;
	a2_t _a2;
	a3_t _a3;
	a4_t _a4;
	a5_t _a5;
	a6_t _a6;
	a7_t _a7;
	a8_t _a8;
public:
	HBoundCall( CALL_t call_, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7, a8_t a8 )
		: _call( call_ ), _a0( a0 ), _a1( a1 ), _a2( a2 ), _a3( a3 ), _a4( a4 ), _a5( a5 ), _a6( a6 ), _a7( a7 ), _a8( a8 ) {}
	template<typename fa0_t>
	return_t operator()( fa0_t a0 ) { return ( _call(
				free_standing_call_args::resolve_arg<1, a0_t, fa0_t>::get( _a0, a0 ),
				free_standing_call_args::resolve_arg<1, a1_t, fa0_t>::get( _a1, a0 ),
				free_standing_call_args::resolve_arg<1, a2_t, fa0_t>::get( _a2, a0 ),
				free_standing_call_args::resolve_arg<1, a3_t, fa0_t>::get( _a3, a0 ),
				free_standing_call_args::resolve_arg<1, a4_t, fa0_t>::get( _a4, a0 ),
				free_standing_call_args::resolve_arg<1, a5_t, fa0_t>::get( _a5, a0 ),
				free_standing_call_args::resolve_arg<1, a6_t, fa0_t>::get( _a6, a0 ),
				free_standing_call_args::resolve_arg<1, a7_t, fa0_t>::get( _a7, a0 ),
				free_standing_call_args::resolve_arg<1, a8_t, fa0_t>::get( _a8, a0 ) ) ); }
	template<typename fa0_t>
	return_t operator()( fa0_t a0 ) const { return ( _call(
				free_standing_call_args::resolve_arg<1, a0_t, fa0_t>::get( _a0, a0 ),
				free_standing_call_args::resolve_arg<1, a1_t, fa0_t>::get( _a1, a0 ),
				free_standing_call_args::resolve_arg<1, a2_t, fa0_t>::get( _a2, a0 ),
				free_standing_call_args::resolve_arg<1, a3_t, fa0_t>::get( _a3, a0 ),
				free_standing_call_args::resolve_arg<1, a4_t, fa0_t>::get( _a4, a0 ),
				free_standing_call_args::resolve_arg<1, a5_t, fa0_t>::get( _a5, a0 ),
				free_standing_call_args::resolve_arg<1, a6_t, fa0_t>::get( _a6, a0 ),
				free_standing_call_args::resolve_arg<1, a7_t, fa0_t>::get( _a7, a0 ),
				free_standing_call_args::resolve_arg<1, a8_t, fa0_t>::get( _a8, a0 ) ) ); }
	};

template<typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t, typename a6_t, typename a7_t,
	typename a8_t, typename a9_t>
class HBoundCall<1, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t>
	{
	CALL_t _call;
	a0_t _a0;
	a1_t _a1;
	a2_t _a2;
	a3_t _a3;
	a4_t _a4;
	a5_t _a5;
	a6_t _a6;
	a7_t _a7;
	a8_t _a8;
	a9_t _a9;
public:
	HBoundCall( CALL_t call_, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7, a8_t a8, a9_t a9 )
		: _call( call_ ), _a0( a0 ), _a1( a1 ), _a2( a2 ), _a3( a3 ), _a4( a4 ), _a5( a5 ), _a6( a6 ), _a7( a7 ), _a8( a8 ), _a9( a9 ) {}
	template<typename fa0_t>
	return_t operator()( fa0_t a0 ) { return ( _call(
				free_standing_call_args::resolve_arg<1, a0_t, fa0_t>::get( _a0, a0 ),
				free_standing_call_args::resolve_arg<1, a1_t, fa0_t>::get( _a1, a0 ),
				free_standing_call_args::resolve_arg<1, a2_t, fa0_t>::get( _a2, a0 ),
				free_standing_call_args::resolve_arg<1, a3_t, fa0_t>::get( _a3, a0 ),
				free_standing_call_args::resolve_arg<1, a4_t, fa0_t>::get( _a4, a0 ),
				free_standing_call_args::resolve_arg<1, a5_t, fa0_t>::get( _a5, a0 ),
				free_standing_call_args::resolve_arg<1, a6_t, fa0_t>::get( _a6, a0 ),
				free_standing_call_args::resolve_arg<1, a7_t, fa0_t>::get( _a7, a0 ),
				free_standing_call_args::resolve_arg<1, a8_t, fa0_t>::get( _a8, a0 ),
				free_standing_call_args::resolve_arg<1, a9_t, fa0_t>::get( _a9, a0 ) ) ); }
	template<typename fa0_t>
	return_t operator()( fa0_t a0 ) const { return ( _call(
				free_standing_call_args::resolve_arg<1, a0_t, fa0_t>::get( _a0, a0 ),
				free_standing_call_args::resolve_arg<1, a1_t, fa0_t>::get( _a1, a0 ),
				free_standing_call_args::resolve_arg<1, a2_t, fa0_t>::get( _a2, a0 ),
				free_standing_call_args::resolve_arg<1, a3_t, fa0_t>::get( _a3, a0 ),
				free_standing_call_args::resolve_arg<1, a4_t, fa0_t>::get( _a4, a0 ),
				free_standing_call_args::resolve_arg<1, a5_t, fa0_t>::get( _a5, a0 ),
				free_standing_call_args::resolve_arg<1, a6_t, fa0_t>::get( _a6, a0 ),
				free_standing_call_args::resolve_arg<1, a7_t, fa0_t>::get( _a7, a0 ),
				free_standing_call_args::resolve_arg<1, a8_t, fa0_t>::get( _a8, a0 ),
				free_standing_call_args::resolve_arg<1, a9_t, fa0_t>::get( _a9, a0 ) ) ); }
	};

template<typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t, typename a6_t, typename a7_t,
	typename a8_t, typename a9_t, typename a10_t>
class HBoundCall<1, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t, a10_t>
	{
	CALL_t _call;
	a0_t _a0;
	a1_t _a1;
	a2_t _a2;
	a3_t _a3;
	a4_t _a4;
	a5_t _a5;
	a6_t _a6;
	a7_t _a7;
	a8_t _a8;
	a9_t _a9;
	a10_t _a10;
public:
	HBoundCall( CALL_t call_, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7, a8_t a8, a9_t a9, a10_t a10)
		: _call( call_ ), _a0( a0 ), _a1( a1 ), _a2( a2 ), _a3( a3 ), _a4( a4 ), _a5( a5 ), _a6( a6 ), _a7( a7 ), _a8( a8 ), _a9( a9 ), _a10( a10 ) {}
	template<typename fa0_t>
	return_t operator()( fa0_t a0 ) { return ( _call(
				free_standing_call_args::resolve_arg<1, a0_t, fa0_t>::get( _a0, a0 ),
				free_standing_call_args::resolve_arg<1, a1_t, fa0_t>::get( _a1, a0 ),
				free_standing_call_args::resolve_arg<1, a2_t, fa0_t>::get( _a2, a0 ),
				free_standing_call_args::resolve_arg<1, a3_t, fa0_t>::get( _a3, a0 ),
				free_standing_call_args::resolve_arg<1, a4_t, fa0_t>::get( _a4, a0 ),
				free_standing_call_args::resolve_arg<1, a5_t, fa0_t>::get( _a5, a0 ),
				free_standing_call_args::resolve_arg<1, a6_t, fa0_t>::get( _a6, a0 ),
				free_standing_call_args::resolve_arg<1, a7_t, fa0_t>::get( _a7, a0 ),
				free_standing_call_args::resolve_arg<1, a8_t, fa0_t>::get( _a8, a0 ),
				free_standing_call_args::resolve_arg<1, a9_t, fa0_t>::get( _a9, a0 ),
				free_standing_call_args::resolve_arg<1, a10_t, fa0_t>::get( _a10, a0 ) ) ); }
	template<typename fa0_t>
	return_t operator()( fa0_t a0 ) const { return ( _call(
				free_standing_call_args::resolve_arg<1, a0_t, fa0_t>::get( _a0, a0 ),
				free_standing_call_args::resolve_arg<1, a1_t, fa0_t>::get( _a1, a0 ),
				free_standing_call_args::resolve_arg<1, a2_t, fa0_t>::get( _a2, a0 ),
				free_standing_call_args::resolve_arg<1, a3_t, fa0_t>::get( _a3, a0 ),
				free_standing_call_args::resolve_arg<1, a4_t, fa0_t>::get( _a4, a0 ),
				free_standing_call_args::resolve_arg<1, a5_t, fa0_t>::get( _a5, a0 ),
				free_standing_call_args::resolve_arg<1, a6_t, fa0_t>::get( _a6, a0 ),
				free_standing_call_args::resolve_arg<1, a7_t, fa0_t>::get( _a7, a0 ),
				free_standing_call_args::resolve_arg<1, a8_t, fa0_t>::get( _a8, a0 ),
				free_standing_call_args::resolve_arg<1, a9_t, fa0_t>::get( _a9, a0 ),
				free_standing_call_args::resolve_arg<1, a10_t, fa0_t>::get( _a10, a0 ) ) ); }
	};

template<typename return_t, typename CALL_t, typename a0_t, typename a1_t>
class HBoundCall<2, return_t, CALL_t, a0_t, a1_t,
	trait::no_type,	trait::no_type, trait::no_type, trait::no_type,
	trait::no_type,	trait::no_type, trait::no_type, trait::no_type>
	{
	CALL_t _call;
	a0_t _a0;
	a1_t _a1;
public:
	HBoundCall( CALL_t call_, a0_t a0, a1_t a1 )
		: _call( call_ ), _a0( a0 ), _a1( a1 ) {}
	template<typename fa0_t, typename fa1_t>
	return_t operator()( fa0_t a0, fa1_t a1 ) { return ( _call(
				free_standing_call_args::resolve_arg<2, a0_t, fa0_t, fa1_t>::get( _a0, a0, a1 ),
				free_standing_call_args::resolve_arg<2, a1_t, fa0_t, fa1_t>::get( _a1, a0, a1 ) ) ); }
	template<typename fa0_t, typename fa1_t>
	return_t operator()( fa0_t a0, fa1_t a1 ) const { return ( _call(
				free_standing_call_args::resolve_arg<2, a0_t, fa0_t, fa1_t>::get( _a0, a0, a1 ),
				free_standing_call_args::resolve_arg<2, a1_t, fa0_t, fa1_t>::get( _a1, a0, a1 ) ) ); }
	};

template<typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t>
class HBoundCall<2, return_t, CALL_t, a0_t, a1_t, a2_t,
	trait::no_type,	trait::no_type, trait::no_type, trait::no_type,
	trait::no_type,	trait::no_type, trait::no_type>
	{
	CALL_t _call;
	a0_t _a0;
	a1_t _a1;
	a2_t _a2;
public:
	HBoundCall( CALL_t call_, a0_t a0, a1_t a1, a2_t a2 )
		: _call( call_ ), _a0( a0 ), _a1( a1 ), _a2( a2 ) {}
	template<typename fa0_t, typename fa1_t>
	return_t operator()( fa0_t a0, fa1_t a1 ) { return ( _call(
				free_standing_call_args::resolve_arg<2, a0_t, fa0_t, fa1_t>::get( _a0, a0, a1 ),
				free_standing_call_args::resolve_arg<2, a1_t, fa0_t, fa1_t>::get( _a1, a0, a1 ),
				free_standing_call_args::resolve_arg<2, a2_t, fa0_t, fa1_t>::get( _a2, a0, a1 ) ) ); }
	template<typename fa0_t, typename fa1_t>
	return_t operator()( fa0_t a0, fa1_t a1 ) const { return ( _call(
				free_standing_call_args::resolve_arg<2, a0_t, fa0_t, fa1_t>::get( _a0, a0, a1 ),
				free_standing_call_args::resolve_arg<2, a1_t, fa0_t, fa1_t>::get( _a1, a0, a1 ),
				free_standing_call_args::resolve_arg<2, a2_t, fa0_t, fa1_t>::get( _a2, a0, a1 ) ) ); }
	};

template<typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t>
class HBoundCall<2, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t,
	trait::no_type,	trait::no_type, trait::no_type, trait::no_type,
	trait::no_type,	trait::no_type>
	{
	CALL_t _call;
	a0_t _a0;
	a1_t _a1;
	a2_t _a2;
	a3_t _a3;
public:
	HBoundCall( CALL_t call_, a0_t a0, a1_t a1, a2_t a2, a3_t a3 )
		: _call( call_ ), _a0( a0 ), _a1( a1 ), _a2( a2 ), _a3( a3 ) {}
	template<typename fa0_t, typename fa1_t>
	return_t operator()( fa0_t a0, fa1_t a1 ) { return ( _call(
				free_standing_call_args::resolve_arg<2, a0_t, fa0_t, fa1_t>::get( _a0, a0, a1 ),
				free_standing_call_args::resolve_arg<2, a1_t, fa0_t, fa1_t>::get( _a1, a0, a1 ),
				free_standing_call_args::resolve_arg<2, a2_t, fa0_t, fa1_t>::get( _a2, a0, a1 ),
				free_standing_call_args::resolve_arg<2, a3_t, fa0_t, fa1_t>::get( _a3, a0, a1 ) ) ); }
	template<typename fa0_t, typename fa1_t>
	return_t operator()( fa0_t a0, fa1_t a1 ) const { return ( _call(
				free_standing_call_args::resolve_arg<2, a0_t, fa0_t, fa1_t>::get( _a0, a0, a1 ),
				free_standing_call_args::resolve_arg<2, a1_t, fa0_t, fa1_t>::get( _a1, a0, a1 ),
				free_standing_call_args::resolve_arg<2, a2_t, fa0_t, fa1_t>::get( _a2, a0, a1 ),
				free_standing_call_args::resolve_arg<2, a3_t, fa0_t, fa1_t>::get( _a3, a0, a1 ) ) ); }
	};

template<typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t>
class HBoundCall<2, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t,
	trait::no_type,	trait::no_type, trait::no_type, trait::no_type,
	trait::no_type>
	{
	CALL_t _call;
	a0_t _a0;
	a1_t _a1;
	a2_t _a2;
	a3_t _a3;
	a4_t _a4;
public:
	HBoundCall( CALL_t call_, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4 )
		: _call( call_ ), _a0( a0 ), _a1( a1 ), _a2( a2 ), _a3( a3 ), _a4( a4 ) {}
	template<typename fa0_t, typename fa1_t>
	return_t operator()( fa0_t a0, fa1_t a1 ) { return ( _call(
				free_standing_call_args::resolve_arg<2, a0_t, fa0_t, fa1_t>::get( _a0, a0, a1 ),
				free_standing_call_args::resolve_arg<2, a1_t, fa0_t, fa1_t>::get( _a1, a0, a1 ),
				free_standing_call_args::resolve_arg<2, a2_t, fa0_t, fa1_t>::get( _a2, a0, a1 ),
				free_standing_call_args::resolve_arg<2, a3_t, fa0_t, fa1_t>::get( _a3, a0, a1 ),
				free_standing_call_args::resolve_arg<2, a4_t, fa0_t, fa1_t>::get( _a4, a0, a1 ) ) ); }
	template<typename fa0_t, typename fa1_t>
	return_t operator()( fa0_t a0, fa1_t a1 ) const { return ( _call(
				free_standing_call_args::resolve_arg<2, a0_t, fa0_t, fa1_t>::get( _a0, a0, a1 ),
				free_standing_call_args::resolve_arg<2, a1_t, fa0_t, fa1_t>::get( _a1, a0, a1 ),
				free_standing_call_args::resolve_arg<2, a2_t, fa0_t, fa1_t>::get( _a2, a0, a1 ),
				free_standing_call_args::resolve_arg<2, a3_t, fa0_t, fa1_t>::get( _a3, a0, a1 ),
				free_standing_call_args::resolve_arg<2, a4_t, fa0_t, fa1_t>::get( _a4, a0, a1 ) ) ); }
	};

template<typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t>
class HBoundCall<2, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t,
	trait::no_type,	trait::no_type, trait::no_type, trait::no_type>
	{
	CALL_t _call;
	a0_t _a0;
	a1_t _a1;
	a2_t _a2;
	a3_t _a3;
	a4_t _a4;
	a5_t _a5;
public:
	HBoundCall( CALL_t call_, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5 )
		: _call( call_ ), _a0( a0 ), _a1( a1 ), _a2( a2 ), _a3( a3 ), _a4( a4 ), _a5( a5 ) {}
	template<typename fa0_t, typename fa1_t>
	return_t operator()( fa0_t a0, fa1_t a1 ) { return ( _call(
				free_standing_call_args::resolve_arg<2, a0_t, fa0_t, fa1_t>::get( _a0, a0, a1 ),
				free_standing_call_args::resolve_arg<2, a1_t, fa0_t, fa1_t>::get( _a1, a0, a1 ),
				free_standing_call_args::resolve_arg<2, a2_t, fa0_t, fa1_t>::get( _a2, a0, a1 ),
				free_standing_call_args::resolve_arg<2, a3_t, fa0_t, fa1_t>::get( _a3, a0, a1 ),
				free_standing_call_args::resolve_arg<2, a4_t, fa0_t, fa1_t>::get( _a4, a0, a1 ),
				free_standing_call_args::resolve_arg<2, a5_t, fa0_t, fa1_t>::get( _a5, a0, a1 ) ) ); }
	template<typename fa0_t, typename fa1_t>
	return_t operator()( fa0_t a0, fa1_t a1 ) const { return ( _call(
				free_standing_call_args::resolve_arg<2, a0_t, fa0_t, fa1_t>::get( _a0, a0, a1 ),
				free_standing_call_args::resolve_arg<2, a1_t, fa0_t, fa1_t>::get( _a1, a0, a1 ),
				free_standing_call_args::resolve_arg<2, a2_t, fa0_t, fa1_t>::get( _a2, a0, a1 ),
				free_standing_call_args::resolve_arg<2, a3_t, fa0_t, fa1_t>::get( _a3, a0, a1 ),
				free_standing_call_args::resolve_arg<2, a4_t, fa0_t, fa1_t>::get( _a4, a0, a1 ),
				free_standing_call_args::resolve_arg<2, a5_t, fa0_t, fa1_t>::get( _a5, a0, a1 ) ) ); }
	};

template<typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t, typename a6_t>
class HBoundCall<2, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t,
	trait::no_type,	trait::no_type, trait::no_type>
	{
	CALL_t _call;
	a0_t _a0;
	a1_t _a1;
	a2_t _a2;
	a3_t _a3;
	a4_t _a4;
	a5_t _a5;
	a6_t _a6;
public:
	HBoundCall( CALL_t call_, a0_t a0, a1_t a1,
			a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6 )
		: _call( call_ ), _a0( a0 ), _a1( a1 ), _a2( a2 ), _a3( a3 ), _a4( a4 ), _a5( a5 ), _a6( a6 ) {}
	template<typename fa0_t, typename fa1_t>
	return_t operator()( fa0_t a0, fa1_t a1 ) { return ( _call(
				free_standing_call_args::resolve_arg<2, a0_t, fa0_t, fa1_t>::get( _a0, a0, a1 ),
				free_standing_call_args::resolve_arg<2, a1_t, fa0_t, fa1_t>::get( _a1, a0, a1 ),
				free_standing_call_args::resolve_arg<2, a2_t, fa0_t, fa1_t>::get( _a2, a0, a1 ),
				free_standing_call_args::resolve_arg<2, a3_t, fa0_t, fa1_t>::get( _a3, a0, a1 ),
				free_standing_call_args::resolve_arg<2, a4_t, fa0_t, fa1_t>::get( _a4, a0, a1 ),
				free_standing_call_args::resolve_arg<2, a5_t, fa0_t, fa1_t>::get( _a5, a0, a1 ),
				free_standing_call_args::resolve_arg<2, a6_t, fa0_t, fa1_t>::get( _a6, a0, a1 ) ) ); }
	template<typename fa0_t, typename fa1_t>
	return_t operator()( fa0_t a0, fa1_t a1 ) const { return ( _call(
				free_standing_call_args::resolve_arg<2, a0_t, fa0_t, fa1_t>::get( _a0, a0, a1 ),
				free_standing_call_args::resolve_arg<2, a1_t, fa0_t, fa1_t>::get( _a1, a0, a1 ),
				free_standing_call_args::resolve_arg<2, a2_t, fa0_t, fa1_t>::get( _a2, a0, a1 ),
				free_standing_call_args::resolve_arg<2, a3_t, fa0_t, fa1_t>::get( _a3, a0, a1 ),
				free_standing_call_args::resolve_arg<2, a4_t, fa0_t, fa1_t>::get( _a4, a0, a1 ),
				free_standing_call_args::resolve_arg<2, a5_t, fa0_t, fa1_t>::get( _a5, a0, a1 ),
				free_standing_call_args::resolve_arg<2, a6_t, fa0_t, fa1_t>::get( _a6, a0, a1 ) ) ); }
	};

template<typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t, typename a6_t, typename a7_t>
class HBoundCall<2, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t,
	trait::no_type, trait::no_type>
	{
	CALL_t _call;
	a0_t _a0;
	a1_t _a1;
	a2_t _a2;
	a3_t _a3;
	a4_t _a4;
	a5_t _a5;
	a6_t _a6;
	a7_t _a7;
public:
	HBoundCall( CALL_t call_, a0_t a0, a1_t a1,
			a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7 )
		: _call( call_ ), _a0( a0 ), _a1( a1 ), _a2( a2 ), _a3( a3 ), _a4( a4 ), _a5( a5 ), _a6( a6 ), _a7( a7 ) {}
	template<typename fa0_t, typename fa1_t>
	return_t operator()( fa0_t a0, fa1_t a1 ) { return ( _call(
				free_standing_call_args::resolve_arg<2, a0_t, fa0_t, fa1_t>::get( _a0, a0, a1 ),
				free_standing_call_args::resolve_arg<2, a1_t, fa0_t, fa1_t>::get( _a1, a0, a1 ),
				free_standing_call_args::resolve_arg<2, a2_t, fa0_t, fa1_t>::get( _a2, a0, a1 ),
				free_standing_call_args::resolve_arg<2, a3_t, fa0_t, fa1_t>::get( _a3, a0, a1 ),
				free_standing_call_args::resolve_arg<2, a4_t, fa0_t, fa1_t>::get( _a4, a0, a1 ),
				free_standing_call_args::resolve_arg<2, a5_t, fa0_t, fa1_t>::get( _a5, a0, a1 ),
				free_standing_call_args::resolve_arg<2, a6_t, fa0_t, fa1_t>::get( _a6, a0, a1 ),
				free_standing_call_args::resolve_arg<2, a7_t, fa0_t, fa1_t>::get( _a7, a0, a1 ) ) ); }
	template<typename fa0_t, typename fa1_t>
	return_t operator()( fa0_t a0, fa1_t a1 ) const { return ( _call(
				free_standing_call_args::resolve_arg<2, a0_t, fa0_t, fa1_t>::get( _a0, a0, a1 ),
				free_standing_call_args::resolve_arg<2, a1_t, fa0_t, fa1_t>::get( _a1, a0, a1 ),
				free_standing_call_args::resolve_arg<2, a2_t, fa0_t, fa1_t>::get( _a2, a0, a1 ),
				free_standing_call_args::resolve_arg<2, a3_t, fa0_t, fa1_t>::get( _a3, a0, a1 ),
				free_standing_call_args::resolve_arg<2, a4_t, fa0_t, fa1_t>::get( _a4, a0, a1 ),
				free_standing_call_args::resolve_arg<2, a5_t, fa0_t, fa1_t>::get( _a5, a0, a1 ),
				free_standing_call_args::resolve_arg<2, a6_t, fa0_t, fa1_t>::get( _a6, a0, a1 ),
				free_standing_call_args::resolve_arg<2, a7_t, fa0_t, fa1_t>::get( _a7, a0, a1 ) ) ); }
	};

template<typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t, typename a6_t, typename a7_t,
	typename a8_t>
class HBoundCall<2, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t,
	trait::no_type>
	{
	CALL_t _call;
	a0_t _a0;
	a1_t _a1;
	a2_t _a2;
	a3_t _a3;
	a4_t _a4;
	a5_t _a5;
	a6_t _a6;
	a7_t _a7;
	a8_t _a8;
public:
	HBoundCall( CALL_t call_, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7, a8_t a8 )
		: _call( call_ ), _a0( a0 ), _a1( a1 ), _a2( a2 ), _a3( a3 ), _a4( a4 ), _a5( a5 ), _a6( a6 ), _a7( a7 ), _a8( a8 ) {}
	template<typename fa0_t, typename fa1_t>
	return_t operator()( fa0_t a0, fa1_t a1 ) { return ( _call(
				free_standing_call_args::resolve_arg<2, a0_t, fa0_t, fa1_t>::get( _a0, a0, a1 ),
				free_standing_call_args::resolve_arg<2, a1_t, fa0_t, fa1_t>::get( _a1, a0, a1 ),
				free_standing_call_args::resolve_arg<2, a2_t, fa0_t, fa1_t>::get( _a2, a0, a1 ),
				free_standing_call_args::resolve_arg<2, a3_t, fa0_t, fa1_t>::get( _a3, a0, a1 ),
				free_standing_call_args::resolve_arg<2, a4_t, fa0_t, fa1_t>::get( _a4, a0, a1 ),
				free_standing_call_args::resolve_arg<2, a5_t, fa0_t, fa1_t>::get( _a5, a0, a1 ),
				free_standing_call_args::resolve_arg<2, a6_t, fa0_t, fa1_t>::get( _a6, a0, a1 ),
				free_standing_call_args::resolve_arg<2, a7_t, fa0_t, fa1_t>::get( _a7, a0, a1 ),
				free_standing_call_args::resolve_arg<2, a8_t, fa0_t, fa1_t>::get( _a8, a0, a1 ) ) ); }
	template<typename fa0_t, typename fa1_t>
	return_t operator()( fa0_t a0, fa1_t a1 ) const { return ( _call(
				free_standing_call_args::resolve_arg<2, a0_t, fa0_t, fa1_t>::get( _a0, a0, a1 ),
				free_standing_call_args::resolve_arg<2, a1_t, fa0_t, fa1_t>::get( _a1, a0, a1 ),
				free_standing_call_args::resolve_arg<2, a2_t, fa0_t, fa1_t>::get( _a2, a0, a1 ),
				free_standing_call_args::resolve_arg<2, a3_t, fa0_t, fa1_t>::get( _a3, a0, a1 ),
				free_standing_call_args::resolve_arg<2, a4_t, fa0_t, fa1_t>::get( _a4, a0, a1 ),
				free_standing_call_args::resolve_arg<2, a5_t, fa0_t, fa1_t>::get( _a5, a0, a1 ),
				free_standing_call_args::resolve_arg<2, a6_t, fa0_t, fa1_t>::get( _a6, a0, a1 ),
				free_standing_call_args::resolve_arg<2, a7_t, fa0_t, fa1_t>::get( _a7, a0, a1 ),
				free_standing_call_args::resolve_arg<2, a8_t, fa0_t, fa1_t>::get( _a8, a0, a1 ) ) ); }
	};

template<typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t, typename a6_t, typename a7_t,
	typename a8_t, typename a9_t>
class HBoundCall<2, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t>
	{
	CALL_t _call;
	a0_t _a0;
	a1_t _a1;
	a2_t _a2;
	a3_t _a3;
	a4_t _a4;
	a5_t _a5;
	a6_t _a6;
	a7_t _a7;
	a8_t _a8;
	a9_t _a9;
public:
	HBoundCall( CALL_t call_, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7, a8_t a8, a9_t a9 )
		: _call( call_ ), _a0( a0 ), _a1( a1 ), _a2( a2 ), _a3( a3 ), _a4( a4 ), _a5( a5 ), _a6( a6 ), _a7( a7 ), _a8( a8 ), _a9( a9 ) {}
	template<typename fa0_t, typename fa1_t>
	return_t operator()( fa0_t a0, fa1_t a1 ) { return ( _call(
				free_standing_call_args::resolve_arg<2, a0_t, fa0_t, fa1_t>::get( _a0, a0, a1 ),
				free_standing_call_args::resolve_arg<2, a1_t, fa0_t, fa1_t>::get( _a1, a0, a1 ),
				free_standing_call_args::resolve_arg<2, a2_t, fa0_t, fa1_t>::get( _a2, a0, a1 ),
				free_standing_call_args::resolve_arg<2, a3_t, fa0_t, fa1_t>::get( _a3, a0, a1 ),
				free_standing_call_args::resolve_arg<2, a4_t, fa0_t, fa1_t>::get( _a4, a0, a1 ),
				free_standing_call_args::resolve_arg<2, a5_t, fa0_t, fa1_t>::get( _a5, a0, a1 ),
				free_standing_call_args::resolve_arg<2, a6_t, fa0_t, fa1_t>::get( _a6, a0, a1 ),
				free_standing_call_args::resolve_arg<2, a7_t, fa0_t, fa1_t>::get( _a7, a0, a1 ),
				free_standing_call_args::resolve_arg<2, a8_t, fa0_t, fa1_t>::get( _a8, a0, a1 ),
				free_standing_call_args::resolve_arg<2, a9_t, fa0_t, fa1_t>::get( _a9, a0, a1 ) ) ); }
	template<typename fa0_t, typename fa1_t>
	return_t operator()( fa0_t a0, fa1_t a1 ) const { return ( _call(
				free_standing_call_args::resolve_arg<2, a0_t, fa0_t, fa1_t>::get( _a0, a0, a1 ),
				free_standing_call_args::resolve_arg<2, a1_t, fa0_t, fa1_t>::get( _a1, a0, a1 ),
				free_standing_call_args::resolve_arg<2, a2_t, fa0_t, fa1_t>::get( _a2, a0, a1 ),
				free_standing_call_args::resolve_arg<2, a3_t, fa0_t, fa1_t>::get( _a3, a0, a1 ),
				free_standing_call_args::resolve_arg<2, a4_t, fa0_t, fa1_t>::get( _a4, a0, a1 ),
				free_standing_call_args::resolve_arg<2, a5_t, fa0_t, fa1_t>::get( _a5, a0, a1 ),
				free_standing_call_args::resolve_arg<2, a6_t, fa0_t, fa1_t>::get( _a6, a0, a1 ),
				free_standing_call_args::resolve_arg<2, a7_t, fa0_t, fa1_t>::get( _a7, a0, a1 ),
				free_standing_call_args::resolve_arg<2, a8_t, fa0_t, fa1_t>::get( _a8, a0, a1 ),
				free_standing_call_args::resolve_arg<2, a9_t, fa0_t, fa1_t>::get( _a9, a0, a1 ) ) ); }
	};

template<typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t, typename a6_t, typename a7_t,
	typename a8_t, typename a9_t, typename a10_t>
class HBoundCall<2, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t, a10_t>
	{
	CALL_t _call;
	a0_t _a0;
	a1_t _a1;
	a2_t _a2;
	a3_t _a3;
	a4_t _a4;
	a5_t _a5;
	a6_t _a6;
	a7_t _a7;
	a8_t _a8;
	a9_t _a9;
	a10_t _a10;
public:
	HBoundCall( CALL_t call_, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7, a8_t a8, a9_t a9, a10_t a10 )
		: _call( call_ ), _a0( a0 ), _a1( a1 ), _a2( a2 ), _a3( a3 ), _a4( a4 ), _a5( a5 ), _a6( a6 ), _a7( a7 ), _a8( a8 ), _a9( a9 ), _a10( a10 ) {}
	template<typename fa0_t, typename fa1_t>
	return_t operator()( fa0_t a0, fa1_t a1 ) { return ( _call(
				free_standing_call_args::resolve_arg<2, a0_t, fa0_t, fa1_t>::get( _a0, a0, a1 ),
				free_standing_call_args::resolve_arg<2, a1_t, fa0_t, fa1_t>::get( _a1, a0, a1 ),
				free_standing_call_args::resolve_arg<2, a2_t, fa0_t, fa1_t>::get( _a2, a0, a1 ),
				free_standing_call_args::resolve_arg<2, a3_t, fa0_t, fa1_t>::get( _a3, a0, a1 ),
				free_standing_call_args::resolve_arg<2, a4_t, fa0_t, fa1_t>::get( _a4, a0, a1 ),
				free_standing_call_args::resolve_arg<2, a5_t, fa0_t, fa1_t>::get( _a5, a0, a1 ),
				free_standing_call_args::resolve_arg<2, a6_t, fa0_t, fa1_t>::get( _a6, a0, a1 ),
				free_standing_call_args::resolve_arg<2, a7_t, fa0_t, fa1_t>::get( _a7, a0, a1 ),
				free_standing_call_args::resolve_arg<2, a8_t, fa0_t, fa1_t>::get( _a8, a0, a1 ),
				free_standing_call_args::resolve_arg<2, a9_t, fa0_t, fa1_t>::get( _a9, a0, a1 ),
				free_standing_call_args::resolve_arg<2, a10_t, fa0_t, fa1_t>::get( _a10, a0, a1 ) ) ); }
	template<typename fa0_t, typename fa1_t>
	return_t operator()( fa0_t a0, fa1_t a1 ) const { return ( _call(
				free_standing_call_args::resolve_arg<2, a0_t, fa0_t, fa1_t>::get( _a0, a0, a1 ),
				free_standing_call_args::resolve_arg<2, a1_t, fa0_t, fa1_t>::get( _a1, a0, a1 ),
				free_standing_call_args::resolve_arg<2, a2_t, fa0_t, fa1_t>::get( _a2, a0, a1 ),
				free_standing_call_args::resolve_arg<2, a3_t, fa0_t, fa1_t>::get( _a3, a0, a1 ),
				free_standing_call_args::resolve_arg<2, a4_t, fa0_t, fa1_t>::get( _a4, a0, a1 ),
				free_standing_call_args::resolve_arg<2, a5_t, fa0_t, fa1_t>::get( _a5, a0, a1 ),
				free_standing_call_args::resolve_arg<2, a6_t, fa0_t, fa1_t>::get( _a6, a0, a1 ),
				free_standing_call_args::resolve_arg<2, a7_t, fa0_t, fa1_t>::get( _a7, a0, a1 ),
				free_standing_call_args::resolve_arg<2, a8_t, fa0_t, fa1_t>::get( _a8, a0, a1 ),
				free_standing_call_args::resolve_arg<2, a9_t, fa0_t, fa1_t>::get( _a9, a0, a1 ),
				free_standing_call_args::resolve_arg<2, a10_t, fa0_t, fa1_t>::get( _a10, a0, a1 ) ) ); }
	};

template<typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t>
class HBoundCall<3, return_t, CALL_t, a0_t, a1_t, a2_t,
	trait::no_type,	trait::no_type, trait::no_type, trait::no_type,
	trait::no_type,	trait::no_type, trait::no_type>
	{
	CALL_t _call;
	a0_t _a0;
	a1_t _a1;
	a2_t _a2;
public:
	HBoundCall( CALL_t call_, a0_t a0, a1_t a1, a2_t a2 )
		: _call( call_ ), _a0( a0 ), _a1( a1 ), _a2( a2 ) {}
	template<typename fa0_t, typename fa1_t, typename fa2_t>
	return_t operator()( fa0_t a0, fa1_t a1, fa2_t a2 ) { return ( _call(
				free_standing_call_args::resolve_arg<3, a0_t, fa0_t, fa1_t, fa2_t>::get( _a0, a0, a1, a2 ),
				free_standing_call_args::resolve_arg<3, a1_t, fa0_t, fa1_t, fa2_t>::get( _a1, a0, a1, a2 ),
				free_standing_call_args::resolve_arg<3, a2_t, fa0_t, fa1_t, fa2_t>::get( _a2, a0, a1, a2 ) ) ); }
	template<typename fa0_t, typename fa1_t, typename fa2_t>
	return_t operator()( fa0_t a0, fa1_t a1, fa2_t a2 ) const { return ( _call(
				free_standing_call_args::resolve_arg<3, a0_t, fa0_t, fa1_t, fa2_t>::get( _a0, a0, a1, a2 ),
				free_standing_call_args::resolve_arg<3, a1_t, fa0_t, fa1_t, fa2_t>::get( _a1, a0, a1, a2 ),
				free_standing_call_args::resolve_arg<3, a2_t, fa0_t, fa1_t, fa2_t>::get( _a2, a0, a1, a2 ) ) ); }
	};

template<typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t>
class HBoundCall<3, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t,
	trait::no_type,	trait::no_type, trait::no_type, trait::no_type,
	trait::no_type,	trait::no_type>
	{
	CALL_t _call;
	a0_t _a0;
	a1_t _a1;
	a2_t _a2;
	a3_t _a3;
public:
	HBoundCall( CALL_t call_, a0_t a0, a1_t a1, a2_t a2, a3_t a3 )
		: _call( call_ ), _a0( a0 ), _a1( a1 ), _a2( a2 ), _a3( a3 ) {}
	template<typename fa0_t, typename fa1_t, typename fa2_t>
	return_t operator()( fa0_t a0, fa1_t a1, fa2_t a2 ) { return ( _call(
				free_standing_call_args::resolve_arg<3, a0_t, fa0_t, fa1_t, fa2_t>::get( _a0, a0, a1, a2 ),
				free_standing_call_args::resolve_arg<3, a1_t, fa0_t, fa1_t, fa2_t>::get( _a1, a0, a1, a2 ),
				free_standing_call_args::resolve_arg<3, a2_t, fa0_t, fa1_t, fa2_t>::get( _a2, a0, a1, a2 ),
				free_standing_call_args::resolve_arg<3, a3_t, fa0_t, fa1_t, fa2_t>::get( _a3, a0, a1, a2 ) ) ); }
	template<typename fa0_t, typename fa1_t, typename fa2_t>
	return_t operator()( fa0_t a0, fa1_t a1, fa2_t a2 ) const { return ( _call(
				free_standing_call_args::resolve_arg<3, a0_t, fa0_t, fa1_t, fa2_t>::get( _a0, a0, a1, a2 ),
				free_standing_call_args::resolve_arg<3, a1_t, fa0_t, fa1_t, fa2_t>::get( _a1, a0, a1, a2 ),
				free_standing_call_args::resolve_arg<3, a2_t, fa0_t, fa1_t, fa2_t>::get( _a2, a0, a1, a2 ),
				free_standing_call_args::resolve_arg<3, a3_t, fa0_t, fa1_t, fa2_t>::get( _a3, a0, a1, a2 ) ) ); }
	};

template<typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t>
class HBoundCall<3, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t,
	trait::no_type,	trait::no_type, trait::no_type, trait::no_type,
	trait::no_type>
	{
	CALL_t _call;
	a0_t _a0;
	a1_t _a1;
	a2_t _a2;
	a3_t _a3;
	a4_t _a4;
public:
	HBoundCall( CALL_t call_, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4 )
		: _call( call_ ), _a0( a0 ), _a1( a1 ), _a2( a2 ), _a3( a3 ), _a4( a4 ) {}
	template<typename fa0_t, typename fa1_t, typename fa2_t>
	return_t operator()( fa0_t a0, fa1_t a1, fa2_t a2 ) { return ( _call(
				free_standing_call_args::resolve_arg<3, a0_t, fa0_t, fa1_t, fa2_t>::get( _a0, a0, a1, a2 ),
				free_standing_call_args::resolve_arg<3, a1_t, fa0_t, fa1_t, fa2_t>::get( _a1, a0, a1, a2 ),
				free_standing_call_args::resolve_arg<3, a2_t, fa0_t, fa1_t, fa2_t>::get( _a2, a0, a1, a2 ),
				free_standing_call_args::resolve_arg<3, a3_t, fa0_t, fa1_t, fa2_t>::get( _a3, a0, a1, a2 ),
				free_standing_call_args::resolve_arg<3, a4_t, fa0_t, fa1_t, fa2_t>::get( _a4, a0, a1, a2 ) ) ); }
	template<typename fa0_t, typename fa1_t, typename fa2_t>
	return_t operator()( fa0_t a0, fa1_t a1, fa2_t a2 ) const { return ( _call(
				free_standing_call_args::resolve_arg<3, a0_t, fa0_t, fa1_t, fa2_t>::get( _a0, a0, a1, a2 ),
				free_standing_call_args::resolve_arg<3, a1_t, fa0_t, fa1_t, fa2_t>::get( _a1, a0, a1, a2 ),
				free_standing_call_args::resolve_arg<3, a2_t, fa0_t, fa1_t, fa2_t>::get( _a2, a0, a1, a2 ),
				free_standing_call_args::resolve_arg<3, a3_t, fa0_t, fa1_t, fa2_t>::get( _a3, a0, a1, a2 ),
				free_standing_call_args::resolve_arg<3, a4_t, fa0_t, fa1_t, fa2_t>::get( _a4, a0, a1, a2 ) ) ); }
	};

template<typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t>
class HBoundCall<3, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t,
	trait::no_type,	trait::no_type, trait::no_type, trait::no_type>
	{
	CALL_t _call;
	a0_t _a0;
	a1_t _a1;
	a2_t _a2;
	a3_t _a3;
	a4_t _a4;
	a5_t _a5;
public:
	HBoundCall( CALL_t call_, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5 )
		: _call( call_ ), _a0( a0 ), _a1( a1 ), _a2( a2 ), _a3( a3 ), _a4( a4 ), _a5( a5 ) {}
	template<typename fa0_t, typename fa1_t, typename fa2_t>
	return_t operator()( fa0_t a0, fa1_t a1, fa2_t a2 ) { return ( _call(
				free_standing_call_args::resolve_arg<3, a0_t, fa0_t, fa1_t, fa2_t>::get( _a0, a0, a1, a2 ),
				free_standing_call_args::resolve_arg<3, a1_t, fa0_t, fa1_t, fa2_t>::get( _a1, a0, a1, a2 ),
				free_standing_call_args::resolve_arg<3, a2_t, fa0_t, fa1_t, fa2_t>::get( _a2, a0, a1, a2 ),
				free_standing_call_args::resolve_arg<3, a3_t, fa0_t, fa1_t, fa2_t>::get( _a3, a0, a1, a2 ),
				free_standing_call_args::resolve_arg<3, a4_t, fa0_t, fa1_t, fa2_t>::get( _a4, a0, a1, a2 ),
				free_standing_call_args::resolve_arg<3, a5_t, fa0_t, fa1_t, fa2_t>::get( _a5, a0, a1, a2 ) ) ); }
	template<typename fa0_t, typename fa1_t, typename fa2_t>
	return_t operator()( fa0_t a0, fa1_t a1, fa2_t a2 ) const { return ( _call(
				free_standing_call_args::resolve_arg<3, a0_t, fa0_t, fa1_t, fa2_t>::get( _a0, a0, a1, a2 ),
				free_standing_call_args::resolve_arg<3, a1_t, fa0_t, fa1_t, fa2_t>::get( _a1, a0, a1, a2 ),
				free_standing_call_args::resolve_arg<3, a2_t, fa0_t, fa1_t, fa2_t>::get( _a2, a0, a1, a2 ),
				free_standing_call_args::resolve_arg<3, a3_t, fa0_t, fa1_t, fa2_t>::get( _a3, a0, a1, a2 ),
				free_standing_call_args::resolve_arg<3, a4_t, fa0_t, fa1_t, fa2_t>::get( _a4, a0, a1, a2 ),
				free_standing_call_args::resolve_arg<3, a5_t, fa0_t, fa1_t, fa2_t>::get( _a5, a0, a1, a2 ) ) ); }
	};

template<typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t, typename a6_t>
class HBoundCall<3, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t,
	trait::no_type,	trait::no_type, trait::no_type>
	{
	CALL_t _call;
	a0_t _a0;
	a1_t _a1;
	a2_t _a2;
	a3_t _a3;
	a4_t _a4;
	a5_t _a5;
	a6_t _a6;
public:
	HBoundCall( CALL_t call_, a0_t a0, a1_t a1,
			a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6 )
		: _call( call_ ), _a0( a0 ), _a1( a1 ), _a2( a2 ), _a3( a3 ), _a4( a4 ), _a5( a5 ), _a6( a6 ) {}
	template<typename fa0_t, typename fa1_t, typename fa2_t>
	return_t operator()( fa0_t a0, fa1_t a1, fa2_t a2 ) { return ( _call(
				free_standing_call_args::resolve_arg<3, a0_t, fa0_t, fa1_t, fa2_t>::get( _a0, a0, a1, a2 ),
				free_standing_call_args::resolve_arg<3, a1_t, fa0_t, fa1_t, fa2_t>::get( _a1, a0, a1, a2 ),
				free_standing_call_args::resolve_arg<3, a2_t, fa0_t, fa1_t, fa2_t>::get( _a2, a0, a1, a2 ),
				free_standing_call_args::resolve_arg<3, a3_t, fa0_t, fa1_t, fa2_t>::get( _a3, a0, a1, a2 ),
				free_standing_call_args::resolve_arg<3, a4_t, fa0_t, fa1_t, fa2_t>::get( _a4, a0, a1, a2 ),
				free_standing_call_args::resolve_arg<3, a5_t, fa0_t, fa1_t, fa2_t>::get( _a5, a0, a1, a2 ),
				free_standing_call_args::resolve_arg<3, a6_t, fa0_t, fa1_t, fa2_t>::get( _a6, a0, a1, a2 ) ) ); }
	template<typename fa0_t, typename fa1_t, typename fa2_t>
	return_t operator()( fa0_t a0, fa1_t a1, fa2_t a2 ) const { return ( _call(
				free_standing_call_args::resolve_arg<3, a0_t, fa0_t, fa1_t, fa2_t>::get( _a0, a0, a1, a2 ),
				free_standing_call_args::resolve_arg<3, a1_t, fa0_t, fa1_t, fa2_t>::get( _a1, a0, a1, a2 ),
				free_standing_call_args::resolve_arg<3, a2_t, fa0_t, fa1_t, fa2_t>::get( _a2, a0, a1, a2 ),
				free_standing_call_args::resolve_arg<3, a3_t, fa0_t, fa1_t, fa2_t>::get( _a3, a0, a1, a2 ),
				free_standing_call_args::resolve_arg<3, a4_t, fa0_t, fa1_t, fa2_t>::get( _a4, a0, a1, a2 ),
				free_standing_call_args::resolve_arg<3, a5_t, fa0_t, fa1_t, fa2_t>::get( _a5, a0, a1, a2 ),
				free_standing_call_args::resolve_arg<3, a6_t, fa0_t, fa1_t, fa2_t>::get( _a6, a0, a1, a2 ) ) ); }
	};

template<typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t, typename a6_t, typename a7_t>
class HBoundCall<3, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t,
	trait::no_type, trait::no_type>
	{
	CALL_t _call;
	a0_t _a0;
	a1_t _a1;
	a2_t _a2;
	a3_t _a3;
	a4_t _a4;
	a5_t _a5;
	a6_t _a6;
	a7_t _a7;
public:
	HBoundCall( CALL_t call_, a0_t a0, a1_t a1,
			a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7 )
		: _call( call_ ), _a0( a0 ), _a1( a1 ), _a2( a2 ), _a3( a3 ), _a4( a4 ), _a5( a5 ), _a6( a6 ), _a7( a7 ) {}
	template<typename fa0_t, typename fa1_t, typename fa2_t>
	return_t operator()( fa0_t a0, fa1_t a1, fa2_t a2 ) { return ( _call(
				free_standing_call_args::resolve_arg<3, a0_t, fa0_t, fa1_t, fa2_t>::get( _a0, a0, a1, a2 ),
				free_standing_call_args::resolve_arg<3, a1_t, fa0_t, fa1_t, fa2_t>::get( _a1, a0, a1, a2 ),
				free_standing_call_args::resolve_arg<3, a2_t, fa0_t, fa1_t, fa2_t>::get( _a2, a0, a1, a2 ),
				free_standing_call_args::resolve_arg<3, a3_t, fa0_t, fa1_t, fa2_t>::get( _a3, a0, a1, a2 ),
				free_standing_call_args::resolve_arg<3, a4_t, fa0_t, fa1_t, fa2_t>::get( _a4, a0, a1, a2 ),
				free_standing_call_args::resolve_arg<3, a5_t, fa0_t, fa1_t, fa2_t>::get( _a5, a0, a1, a2 ),
				free_standing_call_args::resolve_arg<3, a6_t, fa0_t, fa1_t, fa2_t>::get( _a6, a0, a1, a2 ),
				free_standing_call_args::resolve_arg<3, a7_t, fa0_t, fa1_t, fa2_t>::get( _a7, a0, a1, a2 ) ) ); }
	template<typename fa0_t, typename fa1_t, typename fa2_t>
	return_t operator()( fa0_t a0, fa1_t a1, fa2_t a2 ) const { return ( _call(
				free_standing_call_args::resolve_arg<3, a0_t, fa0_t, fa1_t, fa2_t>::get( _a0, a0, a1, a2 ),
				free_standing_call_args::resolve_arg<3, a1_t, fa0_t, fa1_t, fa2_t>::get( _a1, a0, a1, a2 ),
				free_standing_call_args::resolve_arg<3, a2_t, fa0_t, fa1_t, fa2_t>::get( _a2, a0, a1, a2 ),
				free_standing_call_args::resolve_arg<3, a3_t, fa0_t, fa1_t, fa2_t>::get( _a3, a0, a1, a2 ),
				free_standing_call_args::resolve_arg<3, a4_t, fa0_t, fa1_t, fa2_t>::get( _a4, a0, a1, a2 ),
				free_standing_call_args::resolve_arg<3, a5_t, fa0_t, fa1_t, fa2_t>::get( _a5, a0, a1, a2 ),
				free_standing_call_args::resolve_arg<3, a6_t, fa0_t, fa1_t, fa2_t>::get( _a6, a0, a1, a2 ),
				free_standing_call_args::resolve_arg<3, a7_t, fa0_t, fa1_t, fa2_t>::get( _a7, a0, a1, a2 ) ) ); }
	};

template<typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t, typename a6_t, typename a7_t,
	typename a8_t>
class HBoundCall<3, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t,
	trait::no_type>
	{
	CALL_t _call;
	a0_t _a0;
	a1_t _a1;
	a2_t _a2;
	a3_t _a3;
	a4_t _a4;
	a5_t _a5;
	a6_t _a6;
	a7_t _a7;
	a8_t _a8;
public:
	HBoundCall( CALL_t call_, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7, a8_t a8 )
		: _call( call_ ), _a0( a0 ), _a1( a1 ), _a2( a2 ), _a3( a3 ), _a4( a4 ), _a5( a5 ), _a6( a6 ), _a7( a7 ), _a8( a8 ) {}
	template<typename fa0_t, typename fa1_t, typename fa2_t>
	return_t operator()( fa0_t a0, fa1_t a1, fa2_t a2 ) { return ( _call(
				free_standing_call_args::resolve_arg<3, a0_t, fa0_t, fa1_t, fa2_t>::get( _a0, a0, a1, a2 ),
				free_standing_call_args::resolve_arg<3, a1_t, fa0_t, fa1_t, fa2_t>::get( _a1, a0, a1, a2 ),
				free_standing_call_args::resolve_arg<3, a2_t, fa0_t, fa1_t, fa2_t>::get( _a2, a0, a1, a2 ),
				free_standing_call_args::resolve_arg<3, a3_t, fa0_t, fa1_t, fa2_t>::get( _a3, a0, a1, a2 ),
				free_standing_call_args::resolve_arg<3, a4_t, fa0_t, fa1_t, fa2_t>::get( _a4, a0, a1, a2 ),
				free_standing_call_args::resolve_arg<3, a5_t, fa0_t, fa1_t, fa2_t>::get( _a5, a0, a1, a2 ),
				free_standing_call_args::resolve_arg<3, a6_t, fa0_t, fa1_t, fa2_t>::get( _a6, a0, a1, a2 ),
				free_standing_call_args::resolve_arg<3, a7_t, fa0_t, fa1_t, fa2_t>::get( _a7, a0, a1, a2 ),
				free_standing_call_args::resolve_arg<3, a8_t, fa0_t, fa1_t, fa2_t>::get( _a8, a0, a1, a2 ) ) ); }
	template<typename fa0_t, typename fa1_t, typename fa2_t>
	return_t operator()( fa0_t a0, fa1_t a1, fa2_t a2 ) const { return ( _call(
				free_standing_call_args::resolve_arg<3, a0_t, fa0_t, fa1_t, fa2_t>::get( _a0, a0, a1, a2 ),
				free_standing_call_args::resolve_arg<3, a1_t, fa0_t, fa1_t, fa2_t>::get( _a1, a0, a1, a2 ),
				free_standing_call_args::resolve_arg<3, a2_t, fa0_t, fa1_t, fa2_t>::get( _a2, a0, a1, a2 ),
				free_standing_call_args::resolve_arg<3, a3_t, fa0_t, fa1_t, fa2_t>::get( _a3, a0, a1, a2 ),
				free_standing_call_args::resolve_arg<3, a4_t, fa0_t, fa1_t, fa2_t>::get( _a4, a0, a1, a2 ),
				free_standing_call_args::resolve_arg<3, a5_t, fa0_t, fa1_t, fa2_t>::get( _a5, a0, a1, a2 ),
				free_standing_call_args::resolve_arg<3, a6_t, fa0_t, fa1_t, fa2_t>::get( _a6, a0, a1, a2 ),
				free_standing_call_args::resolve_arg<3, a7_t, fa0_t, fa1_t, fa2_t>::get( _a7, a0, a1, a2 ),
				free_standing_call_args::resolve_arg<3, a8_t, fa0_t, fa1_t, fa2_t>::get( _a8, a0, a1, a2 ) ) ); }
	};

template<typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t, typename a6_t, typename a7_t,
	typename a8_t, typename a9_t>
class HBoundCall<3, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t>
	{
	CALL_t _call;
	a0_t _a0;
	a1_t _a1;
	a2_t _a2;
	a3_t _a3;
	a4_t _a4;
	a5_t _a5;
	a6_t _a6;
	a7_t _a7;
	a8_t _a8;
	a9_t _a9;
public:
	HBoundCall( CALL_t call_, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7, a8_t a8, a9_t a9 )
		: _call( call_ ), _a0( a0 ), _a1( a1 ), _a2( a2 ), _a3( a3 ), _a4( a4 ), _a5( a5 ), _a6( a6 ), _a7( a7 ), _a8( a8 ), _a9( a9 ) {}
	template<typename fa0_t, typename fa1_t, typename fa2_t>
	return_t operator()( fa0_t a0, fa1_t a1, fa2_t a2 ) { return ( _call(
				free_standing_call_args::resolve_arg<3, a0_t, fa0_t, fa1_t, fa2_t>::get( _a0, a0, a1, a2 ),
				free_standing_call_args::resolve_arg<3, a1_t, fa0_t, fa1_t, fa2_t>::get( _a1, a0, a1, a2 ),
				free_standing_call_args::resolve_arg<3, a2_t, fa0_t, fa1_t, fa2_t>::get( _a2, a0, a1, a2 ),
				free_standing_call_args::resolve_arg<3, a3_t, fa0_t, fa1_t, fa2_t>::get( _a3, a0, a1, a2 ),
				free_standing_call_args::resolve_arg<3, a4_t, fa0_t, fa1_t, fa2_t>::get( _a4, a0, a1, a2 ),
				free_standing_call_args::resolve_arg<3, a5_t, fa0_t, fa1_t, fa2_t>::get( _a5, a0, a1, a2 ),
				free_standing_call_args::resolve_arg<3, a6_t, fa0_t, fa1_t, fa2_t>::get( _a6, a0, a1, a2 ),
				free_standing_call_args::resolve_arg<3, a7_t, fa0_t, fa1_t, fa2_t>::get( _a7, a0, a1, a2 ),
				free_standing_call_args::resolve_arg<3, a8_t, fa0_t, fa1_t, fa2_t>::get( _a8, a0, a1, a2 ),
				free_standing_call_args::resolve_arg<3, a9_t, fa0_t, fa1_t, fa2_t>::get( _a9, a0, a1, a2 ) ) ); }
	template<typename fa0_t, typename fa1_t, typename fa2_t>
	return_t operator()( fa0_t a0, fa1_t a1, fa2_t a2 ) const { return ( _call(
				free_standing_call_args::resolve_arg<3, a0_t, fa0_t, fa1_t, fa2_t>::get( _a0, a0, a1, a2 ),
				free_standing_call_args::resolve_arg<3, a1_t, fa0_t, fa1_t, fa2_t>::get( _a1, a0, a1, a2 ),
				free_standing_call_args::resolve_arg<3, a2_t, fa0_t, fa1_t, fa2_t>::get( _a2, a0, a1, a2 ),
				free_standing_call_args::resolve_arg<3, a3_t, fa0_t, fa1_t, fa2_t>::get( _a3, a0, a1, a2 ),
				free_standing_call_args::resolve_arg<3, a4_t, fa0_t, fa1_t, fa2_t>::get( _a4, a0, a1, a2 ),
				free_standing_call_args::resolve_arg<3, a5_t, fa0_t, fa1_t, fa2_t>::get( _a5, a0, a1, a2 ),
				free_standing_call_args::resolve_arg<3, a6_t, fa0_t, fa1_t, fa2_t>::get( _a6, a0, a1, a2 ),
				free_standing_call_args::resolve_arg<3, a7_t, fa0_t, fa1_t, fa2_t>::get( _a7, a0, a1, a2 ),
				free_standing_call_args::resolve_arg<3, a8_t, fa0_t, fa1_t, fa2_t>::get( _a8, a0, a1, a2 ),
				free_standing_call_args::resolve_arg<3, a9_t, fa0_t, fa1_t, fa2_t>::get( _a9, a0, a1, a2 ) ) ); }
	};

template<typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t, typename a6_t, typename a7_t,
	typename a8_t, typename a9_t, typename a10_t>
class HBoundCall<3, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t, a10_t>
	{
	CALL_t _call;
	a0_t _a0;
	a1_t _a1;
	a2_t _a2;
	a3_t _a3;
	a4_t _a4;
	a5_t _a5;
	a6_t _a6;
	a7_t _a7;
	a8_t _a8;
	a9_t _a9;
	a10_t _a10;
public:
	HBoundCall( CALL_t call_, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7, a8_t a8, a9_t a9, a10_t a10 )
		: _call( call_ ), _a0( a0 ), _a1( a1 ), _a2( a2 ), _a3( a3 ), _a4( a4 ), _a5( a5 ), _a6( a6 ), _a7( a7 ), _a8( a8 ), _a9( a9 ), _a10( a10 ) {}
	template<typename fa0_t, typename fa1_t, typename fa2_t>
	return_t operator()( fa0_t a0, fa1_t a1, fa2_t a2 ) { return ( _call(
				free_standing_call_args::resolve_arg<3, a0_t, fa0_t, fa1_t, fa2_t>::get( _a0, a0, a1, a2 ),
				free_standing_call_args::resolve_arg<3, a1_t, fa0_t, fa1_t, fa2_t>::get( _a1, a0, a1, a2 ),
				free_standing_call_args::resolve_arg<3, a2_t, fa0_t, fa1_t, fa2_t>::get( _a2, a0, a1, a2 ),
				free_standing_call_args::resolve_arg<3, a3_t, fa0_t, fa1_t, fa2_t>::get( _a3, a0, a1, a2 ),
				free_standing_call_args::resolve_arg<3, a4_t, fa0_t, fa1_t, fa2_t>::get( _a4, a0, a1, a2 ),
				free_standing_call_args::resolve_arg<3, a5_t, fa0_t, fa1_t, fa2_t>::get( _a5, a0, a1, a2 ),
				free_standing_call_args::resolve_arg<3, a6_t, fa0_t, fa1_t, fa2_t>::get( _a6, a0, a1, a2 ),
				free_standing_call_args::resolve_arg<3, a7_t, fa0_t, fa1_t, fa2_t>::get( _a7, a0, a1, a2 ),
				free_standing_call_args::resolve_arg<3, a8_t, fa0_t, fa1_t, fa2_t>::get( _a8, a0, a1, a2 ),
				free_standing_call_args::resolve_arg<3, a9_t, fa0_t, fa1_t, fa2_t>::get( _a9, a0, a1, a2 ),
				free_standing_call_args::resolve_arg<3, a10_t, fa0_t, fa1_t, fa2_t>::get( _a10, a0, a1, a2 ) ) ); }
	template<typename fa0_t, typename fa1_t, typename fa2_t>
	return_t operator()( fa0_t a0, fa1_t a1, fa2_t a2 ) const { return ( _call(
				free_standing_call_args::resolve_arg<3, a0_t, fa0_t, fa1_t, fa2_t>::get( _a0, a0, a1, a2 ),
				free_standing_call_args::resolve_arg<3, a1_t, fa0_t, fa1_t, fa2_t>::get( _a1, a0, a1, a2 ),
				free_standing_call_args::resolve_arg<3, a2_t, fa0_t, fa1_t, fa2_t>::get( _a2, a0, a1, a2 ),
				free_standing_call_args::resolve_arg<3, a3_t, fa0_t, fa1_t, fa2_t>::get( _a3, a0, a1, a2 ),
				free_standing_call_args::resolve_arg<3, a4_t, fa0_t, fa1_t, fa2_t>::get( _a4, a0, a1, a2 ),
				free_standing_call_args::resolve_arg<3, a5_t, fa0_t, fa1_t, fa2_t>::get( _a5, a0, a1, a2 ),
				free_standing_call_args::resolve_arg<3, a6_t, fa0_t, fa1_t, fa2_t>::get( _a6, a0, a1, a2 ),
				free_standing_call_args::resolve_arg<3, a7_t, fa0_t, fa1_t, fa2_t>::get( _a7, a0, a1, a2 ),
				free_standing_call_args::resolve_arg<3, a8_t, fa0_t, fa1_t, fa2_t>::get( _a8, a0, a1, a2 ),
				free_standing_call_args::resolve_arg<3, a9_t, fa0_t, fa1_t, fa2_t>::get( _a9, a0, a1, a2 ),
				free_standing_call_args::resolve_arg<3, a10_t, fa0_t, fa1_t, fa2_t>::get( _a10, a0, a1, a2 ) ) ); }
	};

template<typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t>
class HBoundCall<4, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t,
	trait::no_type,	trait::no_type, trait::no_type, trait::no_type,
	trait::no_type,	trait::no_type>
	{
	CALL_t _call;
	a0_t _a0;
	a1_t _a1;
	a2_t _a2;
	a3_t _a3;
public:
	HBoundCall( CALL_t call_, a0_t a0, a1_t a1, a2_t a2, a3_t a3 )
		: _call( call_ ), _a0( a0 ), _a1( a1 ), _a2( a2 ), _a3( a3 ) {}
	template<typename fa0_t, typename fa1_t, typename fa2_t, typename fa3_t>
	return_t operator()( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3 ) { return ( _call(
				free_standing_call_args::resolve_arg<4, a0_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a0, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg<4, a1_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a1, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg<4, a2_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a2, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg<4, a3_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a3, a0, a1, a2, a3 ) ) ); }
	template<typename fa0_t, typename fa1_t, typename fa2_t, typename fa3_t>
	return_t operator()( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3 ) const { return ( _call(
				free_standing_call_args::resolve_arg<4, a0_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a0, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg<4, a1_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a1, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg<4, a2_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a2, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg<4, a3_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a3, a0, a1, a2, a3 ) ) ); }
	};

template<typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t>
class HBoundCall<4, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t,
	trait::no_type,	trait::no_type, trait::no_type, trait::no_type,
	trait::no_type>
	{
	CALL_t _call;
	a0_t _a0;
	a1_t _a1;
	a2_t _a2;
	a3_t _a3;
	a4_t _a4;
public:
	HBoundCall( CALL_t call_, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4 )
		: _call( call_ ), _a0( a0 ), _a1( a1 ), _a2( a2 ), _a3( a3 ), _a4( a4 ) {}
	template<typename fa0_t, typename fa1_t, typename fa2_t, typename fa3_t>
	return_t operator()( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3 ) { return ( _call(
				free_standing_call_args::resolve_arg<4, a0_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a0, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg<4, a1_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a1, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg<4, a2_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a2, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg<4, a3_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a3, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg<4, a4_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a4, a0, a1, a2, a3 ) ) ); }
	template<typename fa0_t, typename fa1_t, typename fa2_t, typename fa3_t>
	return_t operator()( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3 ) const { return ( _call(
				free_standing_call_args::resolve_arg<4, a0_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a0, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg<4, a1_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a1, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg<4, a2_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a2, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg<4, a3_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a3, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg<4, a4_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a4, a0, a1, a2, a3 ) ) ); }
	};

template<typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t>
class HBoundCall<4, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t,
	trait::no_type,	trait::no_type, trait::no_type, trait::no_type>
	{
	CALL_t _call;
	a0_t _a0;
	a1_t _a1;
	a2_t _a2;
	a3_t _a3;
	a4_t _a4;
	a5_t _a5;
public:
	HBoundCall( CALL_t call_, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5 )
		: _call( call_ ), _a0( a0 ), _a1( a1 ), _a2( a2 ), _a3( a3 ), _a4( a4 ), _a5( a5 ) {}
	template<typename fa0_t, typename fa1_t, typename fa2_t, typename fa3_t>
	return_t operator()( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3 ) { return ( _call(
				free_standing_call_args::resolve_arg<4, a0_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a0, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg<4, a1_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a1, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg<4, a2_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a2, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg<4, a3_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a3, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg<4, a4_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a4, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg<4, a5_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a5, a0, a1, a2, a3 ) ) ); }
	template<typename fa0_t, typename fa1_t, typename fa2_t, typename fa3_t>
	return_t operator()( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3 ) const { return ( _call(
				free_standing_call_args::resolve_arg<4, a0_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a0, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg<4, a1_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a1, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg<4, a2_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a2, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg<4, a3_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a3, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg<4, a4_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a4, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg<4, a5_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a5, a0, a1, a2, a3 ) ) ); }
	};

template<typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t, typename a6_t>
class HBoundCall<4, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t,
	trait::no_type,	trait::no_type, trait::no_type>
	{
	CALL_t _call;
	a0_t _a0;
	a1_t _a1;
	a2_t _a2;
	a3_t _a3;
	a4_t _a4;
	a5_t _a5;
	a6_t _a6;
public:
	HBoundCall( CALL_t call_, a0_t a0, a1_t a1,
			a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6 )
		: _call( call_ ), _a0( a0 ), _a1( a1 ), _a2( a2 ), _a3( a3 ), _a4( a4 ), _a5( a5 ), _a6( a6 ) {}
	template<typename fa0_t, typename fa1_t, typename fa2_t, typename fa3_t>
	return_t operator()( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3 ) { return ( _call(
				free_standing_call_args::resolve_arg<4, a0_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a0, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg<4, a1_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a1, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg<4, a2_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a2, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg<4, a3_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a3, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg<4, a4_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a4, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg<4, a5_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a5, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg<4, a6_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a6, a0, a1, a2, a3 ) ) ); }
	template<typename fa0_t, typename fa1_t, typename fa2_t, typename fa3_t>
	return_t operator()( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3 ) const { return ( _call(
				free_standing_call_args::resolve_arg<4, a0_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a0, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg<4, a1_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a1, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg<4, a2_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a2, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg<4, a3_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a3, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg<4, a4_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a4, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg<4, a5_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a5, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg<4, a6_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a6, a0, a1, a2, a3 ) ) ); }
	};

template<typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t, typename a6_t, typename a7_t>
class HBoundCall<4, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t,
	trait::no_type, trait::no_type>
	{
	CALL_t _call;
	a0_t _a0;
	a1_t _a1;
	a2_t _a2;
	a3_t _a3;
	a4_t _a4;
	a5_t _a5;
	a6_t _a6;
	a7_t _a7;
public:
	HBoundCall( CALL_t call_, a0_t a0, a1_t a1,
			a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7 )
		: _call( call_ ), _a0( a0 ), _a1( a1 ), _a2( a2 ), _a3( a3 ), _a4( a4 ), _a5( a5 ), _a6( a6 ), _a7( a7 ) {}
	template<typename fa0_t, typename fa1_t, typename fa2_t, typename fa3_t>
	return_t operator()( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3 ) { return ( _call(
				free_standing_call_args::resolve_arg<4, a0_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a0, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg<4, a1_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a1, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg<4, a2_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a2, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg<4, a3_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a3, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg<4, a4_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a4, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg<4, a5_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a5, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg<4, a6_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a6, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg<4, a7_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a7, a0, a1, a2, a3 ) ) ); }
	template<typename fa0_t, typename fa1_t, typename fa2_t, typename fa3_t>
	return_t operator()( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3 ) const { return ( _call(
				free_standing_call_args::resolve_arg<4, a0_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a0, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg<4, a1_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a1, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg<4, a2_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a2, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg<4, a3_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a3, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg<4, a4_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a4, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg<4, a5_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a5, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg<4, a6_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a6, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg<4, a7_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a7, a0, a1, a2, a3 ) ) ); }
	};

template<typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t, typename a6_t, typename a7_t,
	typename a8_t>
class HBoundCall<4, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t,
	trait::no_type>
	{
	CALL_t _call;
	a0_t _a0;
	a1_t _a1;
	a2_t _a2;
	a3_t _a3;
	a4_t _a4;
	a5_t _a5;
	a6_t _a6;
	a7_t _a7;
	a8_t _a8;
public:
	HBoundCall( CALL_t call_, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7, a8_t a8 )
		: _call( call_ ), _a0( a0 ), _a1( a1 ), _a2( a2 ), _a3( a3 ), _a4( a4 ), _a5( a5 ), _a6( a6 ), _a7( a7 ), _a8( a8 ) {}
	template<typename fa0_t, typename fa1_t, typename fa2_t, typename fa3_t>
	return_t operator()( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3 ) { return ( _call(
				free_standing_call_args::resolve_arg<4, a0_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a0, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg<4, a1_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a1, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg<4, a2_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a2, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg<4, a3_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a3, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg<4, a4_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a4, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg<4, a5_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a5, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg<4, a6_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a6, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg<4, a7_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a7, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg<4, a8_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a8, a0, a1, a2, a3 ) ) ); }
	template<typename fa0_t, typename fa1_t, typename fa2_t, typename fa3_t>
	return_t operator()( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3 ) const { return ( _call(
				free_standing_call_args::resolve_arg<4, a0_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a0, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg<4, a1_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a1, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg<4, a2_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a2, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg<4, a3_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a3, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg<4, a4_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a4, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg<4, a5_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a5, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg<4, a6_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a6, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg<4, a7_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a7, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg<4, a8_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a8, a0, a1, a2, a3 ) ) ); }
	};

template<typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t, typename a6_t, typename a7_t,
	typename a8_t, typename a9_t>
class HBoundCall<4, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t>
	{
	CALL_t _call;
	a0_t _a0;
	a1_t _a1;
	a2_t _a2;
	a3_t _a3;
	a4_t _a4;
	a5_t _a5;
	a6_t _a6;
	a7_t _a7;
	a8_t _a8;
	a9_t _a9;
public:
	HBoundCall( CALL_t call_, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7, a8_t a8, a9_t a9 )
		: _call( call_ ), _a0( a0 ), _a1( a1 ), _a2( a2 ), _a3( a3 ), _a4( a4 ), _a5( a5 ), _a6( a6 ), _a7( a7 ), _a8( a8 ), _a9( a9 ) {}
	template<typename fa0_t, typename fa1_t, typename fa2_t, typename fa3_t>
	return_t operator()( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3 ) { return ( _call(
				free_standing_call_args::resolve_arg<4, a0_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a0, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg<4, a1_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a1, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg<4, a2_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a2, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg<4, a3_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a3, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg<4, a4_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a4, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg<4, a5_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a5, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg<4, a6_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a6, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg<4, a7_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a7, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg<4, a8_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a8, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg<4, a9_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a9, a0, a1, a2, a3 ) ) ); }
	template<typename fa0_t, typename fa1_t, typename fa2_t, typename fa3_t>
	return_t operator()( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3 ) const { return ( _call(
				free_standing_call_args::resolve_arg<4, a0_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a0, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg<4, a1_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a1, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg<4, a2_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a2, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg<4, a3_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a3, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg<4, a4_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a4, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg<4, a5_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a5, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg<4, a6_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a6, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg<4, a7_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a7, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg<4, a8_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a8, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg<4, a9_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a9, a0, a1, a2, a3 ) ) ); }
	};

template<typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t, typename a6_t, typename a7_t,
	typename a8_t, typename a9_t, typename a10_t>
class HBoundCall<4, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t, a10_t>
	{
	CALL_t _call;
	a0_t _a0;
	a1_t _a1;
	a2_t _a2;
	a3_t _a3;
	a4_t _a4;
	a5_t _a5;
	a6_t _a6;
	a7_t _a7;
	a8_t _a8;
	a9_t _a9;
	a10_t _a10;
public:
	HBoundCall( CALL_t call_, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7, a8_t a8, a9_t a9, a10_t a10 )
		: _call( call_ ), _a0( a0 ), _a1( a1 ), _a2( a2 ), _a3( a3 ), _a4( a4 ), _a5( a5 ), _a6( a6 ), _a7( a7 ), _a8( a8 ), _a9( a9 ), _a10( a10 ) {}
	template<typename fa0_t, typename fa1_t, typename fa2_t, typename fa3_t>
	return_t operator()( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3 ) { return ( _call(
				free_standing_call_args::resolve_arg<4, a0_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a0, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg<4, a1_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a1, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg<4, a2_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a2, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg<4, a3_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a3, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg<4, a4_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a4, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg<4, a5_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a5, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg<4, a6_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a6, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg<4, a7_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a7, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg<4, a8_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a8, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg<4, a9_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a9, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg<4, a10_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a10, a0, a1, a2, a3 ) ) ); }
	template<typename fa0_t, typename fa1_t, typename fa2_t, typename fa3_t>
	return_t operator()( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3 ) const { return ( _call(
				free_standing_call_args::resolve_arg<4, a0_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a0, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg<4, a1_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a1, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg<4, a2_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a2, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg<4, a3_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a3, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg<4, a4_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a4, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg<4, a5_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a5, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg<4, a6_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a6, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg<4, a7_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a7, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg<4, a8_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a8, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg<4, a9_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a9, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg<4, a10_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a10, a0, a1, a2, a3 ) ) ); }
	};

template<typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t>
class HBoundCall<5, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t,
	trait::no_type,	trait::no_type, trait::no_type, trait::no_type,
	trait::no_type>
	{
	CALL_t _call;
	a0_t _a0;
	a1_t _a1;
	a2_t _a2;
	a3_t _a3;
	a4_t _a4;
public:
	HBoundCall( CALL_t call_, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4 )
		: _call( call_ ), _a0( a0 ), _a1( a1 ), _a2( a2 ), _a3( a3 ), _a4( a4 ) {}
	template<typename fa0_t, typename fa1_t, typename fa2_t, typename fa3_t, typename fa4_t>
	return_t operator()( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3, fa4_t a4 ) { return ( _call(
				free_standing_call_args::resolve_arg<5, a0_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t>::get( _a0, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg<5, a1_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t>::get( _a1, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg<5, a2_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t>::get( _a2, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg<5, a3_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t>::get( _a3, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg<5, a4_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t>::get( _a4, a0, a1, a2, a3, a4 ) ) ); }
	template<typename fa0_t, typename fa1_t, typename fa2_t, typename fa3_t, typename fa4_t>
	return_t operator()( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3, fa4_t a4 ) const { return ( _call(
				free_standing_call_args::resolve_arg<5, a0_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t>::get( _a0, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg<5, a1_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t>::get( _a1, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg<5, a2_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t>::get( _a2, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg<5, a3_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t>::get( _a3, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg<5, a4_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t>::get( _a4, a0, a1, a2, a3, a4 ) ) ); }
	};

template<typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t>
class HBoundCall<5, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t,
	trait::no_type,	trait::no_type, trait::no_type, trait::no_type>
	{
	CALL_t _call;
	a0_t _a0;
	a1_t _a1;
	a2_t _a2;
	a3_t _a3;
	a4_t _a4;
	a5_t _a5;
public:
	HBoundCall( CALL_t call_, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5 )
		: _call( call_ ), _a0( a0 ), _a1( a1 ), _a2( a2 ), _a3( a3 ), _a4( a4 ), _a5( a5 ) {}
	template<typename fa0_t, typename fa1_t, typename fa2_t, typename fa3_t, typename fa4_t>
	return_t operator()( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3, fa4_t a4 ) { return ( _call(
				free_standing_call_args::resolve_arg<5, a0_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t>::get( _a0, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg<5, a1_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t>::get( _a1, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg<5, a2_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t>::get( _a2, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg<5, a3_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t>::get( _a3, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg<5, a4_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t>::get( _a4, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg<5, a5_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t>::get( _a5, a0, a1, a2, a3, a4 ) ) ); }
	template<typename fa0_t, typename fa1_t, typename fa2_t, typename fa3_t, typename fa4_t>
	return_t operator()( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3, fa4_t a4 ) const { return ( _call(
				free_standing_call_args::resolve_arg<5, a0_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t>::get( _a0, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg<5, a1_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t>::get( _a1, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg<5, a2_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t>::get( _a2, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg<5, a3_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t>::get( _a3, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg<5, a4_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t>::get( _a4, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg<5, a5_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t>::get( _a5, a0, a1, a2, a3, a4 ) ) ); }
	};

template<typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t, typename a6_t>
class HBoundCall<5, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t,
	trait::no_type,	trait::no_type, trait::no_type>
	{
	CALL_t _call;
	a0_t _a0;
	a1_t _a1;
	a2_t _a2;
	a3_t _a3;
	a4_t _a4;
	a5_t _a5;
	a6_t _a6;
public:
	HBoundCall( CALL_t call_, a0_t a0, a1_t a1,
			a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6 )
		: _call( call_ ), _a0( a0 ), _a1( a1 ), _a2( a2 ), _a3( a3 ), _a4( a4 ), _a5( a5 ), _a6( a6 ) {}
	template<typename fa0_t, typename fa1_t, typename fa2_t, typename fa3_t, typename fa4_t>
	return_t operator()( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3, fa4_t a4 ) { return ( _call(
				free_standing_call_args::resolve_arg<5, a0_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t>::get( _a0, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg<5, a1_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t>::get( _a1, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg<5, a2_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t>::get( _a2, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg<5, a3_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t>::get( _a3, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg<5, a4_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t>::get( _a4, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg<5, a5_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t>::get( _a5, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg<5, a6_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t>::get( _a6, a0, a1, a2, a3, a4 ) ) ); }
	template<typename fa0_t, typename fa1_t, typename fa2_t, typename fa3_t, typename fa4_t>
	return_t operator()( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3, fa4_t a4 ) const { return ( _call(
				free_standing_call_args::resolve_arg<5, a0_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t>::get( _a0, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg<5, a1_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t>::get( _a1, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg<5, a2_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t>::get( _a2, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg<5, a3_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t>::get( _a3, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg<5, a4_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t>::get( _a4, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg<5, a5_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t>::get( _a5, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg<5, a6_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t>::get( _a6, a0, a1, a2, a3, a4 ) ) ); }
	};

template<typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t, typename a6_t, typename a7_t>
class HBoundCall<5, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t,
	trait::no_type, trait::no_type>
	{
	CALL_t _call;
	a0_t _a0;
	a1_t _a1;
	a2_t _a2;
	a3_t _a3;
	a4_t _a4;
	a5_t _a5;
	a6_t _a6;
	a7_t _a7;
public:
	HBoundCall( CALL_t call_, a0_t a0, a1_t a1,
			a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7 )
		: _call( call_ ), _a0( a0 ), _a1( a1 ), _a2( a2 ), _a3( a3 ), _a4( a4 ), _a5( a5 ), _a6( a6 ), _a7( a7 ) {}
	template<typename fa0_t, typename fa1_t, typename fa2_t, typename fa3_t, typename fa4_t>
	return_t operator()( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3, fa4_t a4 ) { return ( _call(
				free_standing_call_args::resolve_arg<5, a0_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t>::get( _a0, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg<5, a1_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t>::get( _a1, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg<5, a2_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t>::get( _a2, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg<5, a3_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t>::get( _a3, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg<5, a4_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t>::get( _a4, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg<5, a5_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t>::get( _a5, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg<5, a6_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t>::get( _a6, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg<5, a7_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t>::get( _a7, a0, a1, a2, a3, a4 ) ) ); }
	template<typename fa0_t, typename fa1_t, typename fa2_t, typename fa3_t, typename fa4_t>
	return_t operator()( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3, fa4_t a4 ) const { return ( _call(
				free_standing_call_args::resolve_arg<5, a0_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t>::get( _a0, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg<5, a1_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t>::get( _a1, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg<5, a2_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t>::get( _a2, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg<5, a3_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t>::get( _a3, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg<5, a4_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t>::get( _a4, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg<5, a5_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t>::get( _a5, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg<5, a6_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t>::get( _a6, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg<5, a7_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t>::get( _a7, a0, a1, a2, a3, a4 ) ) ); }
	};

template<typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t, typename a6_t, typename a7_t,
	typename a8_t>
class HBoundCall<5, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t,
	trait::no_type>
	{
	CALL_t _call;
	a0_t _a0;
	a1_t _a1;
	a2_t _a2;
	a3_t _a3;
	a4_t _a4;
	a5_t _a5;
	a6_t _a6;
	a7_t _a7;
	a8_t _a8;
public:
	HBoundCall( CALL_t call_, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7, a8_t a8 )
		: _call( call_ ), _a0( a0 ), _a1( a1 ), _a2( a2 ), _a3( a3 ), _a4( a4 ), _a5( a5 ), _a6( a6 ), _a7( a7 ), _a8( a8 ) {}
	template<typename fa0_t, typename fa1_t, typename fa2_t, typename fa3_t, typename fa4_t>
	return_t operator()( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3, fa4_t a4 ) { return ( _call(
				free_standing_call_args::resolve_arg<5, a0_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t>::get( _a0, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg<5, a1_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t>::get( _a1, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg<5, a2_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t>::get( _a2, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg<5, a3_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t>::get( _a3, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg<5, a4_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t>::get( _a4, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg<5, a5_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t>::get( _a5, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg<5, a6_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t>::get( _a6, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg<5, a7_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t>::get( _a7, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg<5, a8_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t>::get( _a8, a0, a1, a2, a3, a4 ) ) ); }
	template<typename fa0_t, typename fa1_t, typename fa2_t, typename fa3_t, typename fa4_t>
	return_t operator()( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3, fa4_t a4 ) const { return ( _call(
				free_standing_call_args::resolve_arg<5, a0_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t>::get( _a0, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg<5, a1_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t>::get( _a1, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg<5, a2_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t>::get( _a2, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg<5, a3_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t>::get( _a3, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg<5, a4_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t>::get( _a4, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg<5, a5_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t>::get( _a5, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg<5, a6_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t>::get( _a6, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg<5, a7_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t>::get( _a7, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg<5, a8_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t>::get( _a8, a0, a1, a2, a3, a4 ) ) ); }
	};

template<typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t, typename a6_t, typename a7_t,
	typename a8_t, typename a9_t>
class HBoundCall<5, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t>
	{
	CALL_t _call;
	a0_t _a0;
	a1_t _a1;
	a2_t _a2;
	a3_t _a3;
	a4_t _a4;
	a5_t _a5;
	a6_t _a6;
	a7_t _a7;
	a8_t _a8;
	a9_t _a9;
public:
	HBoundCall( CALL_t call_, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7, a8_t a8, a9_t a9 )
		: _call( call_ ), _a0( a0 ), _a1( a1 ), _a2( a2 ), _a3( a3 ), _a4( a4 ), _a5( a5 ), _a6( a6 ), _a7( a7 ), _a8( a8 ), _a9( a9 ) {}
	template<typename fa0_t, typename fa1_t, typename fa2_t, typename fa3_t, typename fa4_t>
	return_t operator()( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3, fa4_t a4 ) { return ( _call(
				free_standing_call_args::resolve_arg<5, a0_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t>::get( _a0, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg<5, a1_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t>::get( _a1, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg<5, a2_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t>::get( _a2, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg<5, a3_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t>::get( _a3, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg<5, a4_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t>::get( _a4, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg<5, a5_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t>::get( _a5, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg<5, a6_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t>::get( _a6, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg<5, a7_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t>::get( _a7, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg<5, a8_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t>::get( _a8, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg<5, a9_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t>::get( _a9, a0, a1, a2, a3, a4 ) ) ); }
	template<typename fa0_t, typename fa1_t, typename fa2_t, typename fa3_t, typename fa4_t>
	return_t operator()( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3, fa4_t a4 ) const { return ( _call(
				free_standing_call_args::resolve_arg<5, a0_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t>::get( _a0, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg<5, a1_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t>::get( _a1, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg<5, a2_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t>::get( _a2, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg<5, a3_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t>::get( _a3, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg<5, a4_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t>::get( _a4, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg<5, a5_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t>::get( _a5, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg<5, a6_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t>::get( _a6, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg<5, a7_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t>::get( _a7, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg<5, a8_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t>::get( _a8, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg<5, a9_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t>::get( _a9, a0, a1, a2, a3, a4 ) ) ); }
	};

template<typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t, typename a6_t, typename a7_t,
	typename a8_t, typename a9_t, typename a10_t>
class HBoundCall<5, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t, a10_t>
	{
	CALL_t _call;
	a0_t _a0;
	a1_t _a1;
	a2_t _a2;
	a3_t _a3;
	a4_t _a4;
	a5_t _a5;
	a6_t _a6;
	a7_t _a7;
	a8_t _a8;
	a9_t _a9;
	a10_t _a10;
public:
	HBoundCall( CALL_t call_, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7, a8_t a8, a9_t a9, a10_t a10 )
		: _call( call_ ), _a0( a0 ), _a1( a1 ), _a2( a2 ), _a3( a3 ), _a4( a4 ), _a5( a5 ), _a6( a6 ), _a7( a7 ), _a8( a8 ), _a9( a9 ), _a10( a10 ) {}
	template<typename fa0_t, typename fa1_t, typename fa2_t, typename fa3_t, typename fa4_t>
	return_t operator()( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3, fa4_t a4 ) { return ( _call(
				free_standing_call_args::resolve_arg<5, a0_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t>::get( _a0, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg<5, a1_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t>::get( _a1, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg<5, a2_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t>::get( _a2, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg<5, a3_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t>::get( _a3, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg<5, a4_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t>::get( _a4, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg<5, a5_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t>::get( _a5, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg<5, a6_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t>::get( _a6, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg<5, a7_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t>::get( _a7, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg<5, a8_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t>::get( _a8, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg<5, a9_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t>::get( _a9, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg<5, a10_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t>::get( _a10, a0, a1, a2, a3, a4 ) ) ); }
	template<typename fa0_t, typename fa1_t, typename fa2_t, typename fa3_t, typename fa4_t>
	return_t operator()( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3, fa4_t a4 ) const { return ( _call(
				free_standing_call_args::resolve_arg<5, a0_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t>::get( _a0, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg<5, a1_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t>::get( _a1, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg<5, a2_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t>::get( _a2, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg<5, a3_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t>::get( _a3, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg<5, a4_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t>::get( _a4, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg<5, a5_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t>::get( _a5, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg<5, a6_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t>::get( _a6, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg<5, a7_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t>::get( _a7, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg<5, a8_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t>::get( _a8, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg<5, a9_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t>::get( _a9, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg<5, a10_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t>::get( _a10, a0, a1, a2, a3, a4 ) ) ); }
	};

template<typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t>
class HBoundCall<6, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t,
	trait::no_type,	trait::no_type, trait::no_type, trait::no_type>
	{
	CALL_t _call;
	a0_t _a0;
	a1_t _a1;
	a2_t _a2;
	a3_t _a3;
	a4_t _a4;
	a5_t _a5;
public:
	HBoundCall( CALL_t call_, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5 )
		: _call( call_ ), _a0( a0 ), _a1( a1 ), _a2( a2 ), _a3( a3 ), _a4( a4 ), _a5( a5 ) {}
	template<typename fa0_t, typename fa1_t, typename fa2_t, typename fa3_t, typename fa4_t, typename fa5_t>
	return_t operator()( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3, fa4_t a4, fa5_t a5 ) { return ( _call(
				free_standing_call_args::resolve_arg<6, a0_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t>::get( _a0, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg<6, a1_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t>::get( _a1, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg<6, a2_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t>::get( _a2, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg<6, a3_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t>::get( _a3, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg<6, a4_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t>::get( _a4, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg<6, a5_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t>::get( _a5, a0, a1, a2, a3, a4, a5 ) ) ); }
	template<typename fa0_t, typename fa1_t, typename fa2_t, typename fa3_t, typename fa4_t, typename fa5_t>
	return_t operator()( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3, fa4_t a4, fa5_t a5 ) const { return ( _call(
				free_standing_call_args::resolve_arg<6, a0_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t>::get( _a0, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg<6, a1_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t>::get( _a1, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg<6, a2_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t>::get( _a2, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg<6, a3_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t>::get( _a3, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg<6, a4_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t>::get( _a4, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg<6, a5_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t>::get( _a5, a0, a1, a2, a3, a4, a5 ) ) ); }
	};

template<typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t, typename a6_t>
class HBoundCall<6, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t,
	trait::no_type,	trait::no_type, trait::no_type>
	{
	CALL_t _call;
	a0_t _a0;
	a1_t _a1;
	a2_t _a2;
	a3_t _a3;
	a4_t _a4;
	a5_t _a5;
	a6_t _a6;
public:
	HBoundCall( CALL_t call_, a0_t a0, a1_t a1,
			a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6 )
		: _call( call_ ), _a0( a0 ), _a1( a1 ), _a2( a2 ), _a3( a3 ), _a4( a4 ), _a5( a5 ), _a6( a6 ) {}
	template<typename fa0_t, typename fa1_t, typename fa2_t, typename fa3_t, typename fa4_t, typename fa5_t>
	return_t operator()( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3, fa4_t a4, fa5_t a5 ) { return ( _call(
				free_standing_call_args::resolve_arg<6, a0_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t>::get( _a0, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg<6, a1_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t>::get( _a1, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg<6, a2_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t>::get( _a2, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg<6, a3_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t>::get( _a3, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg<6, a4_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t>::get( _a4, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg<6, a5_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t>::get( _a5, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg<6, a6_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t>::get( _a6, a0, a1, a2, a3, a4, a5 ) ) ); }
	template<typename fa0_t, typename fa1_t, typename fa2_t, typename fa3_t, typename fa4_t, typename fa5_t>
	return_t operator()( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3, fa4_t a4, fa5_t a5 ) const { return ( _call(
				free_standing_call_args::resolve_arg<6, a0_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t>::get( _a0, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg<6, a1_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t>::get( _a1, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg<6, a2_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t>::get( _a2, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg<6, a3_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t>::get( _a3, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg<6, a4_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t>::get( _a4, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg<6, a5_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t>::get( _a5, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg<6, a6_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t>::get( _a6, a0, a1, a2, a3, a4, a5 ) ) ); }
	};

template<typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t, typename a6_t, typename a7_t>
class HBoundCall<6, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t,
	trait::no_type, trait::no_type>
	{
	CALL_t _call;
	a0_t _a0;
	a1_t _a1;
	a2_t _a2;
	a3_t _a3;
	a4_t _a4;
	a5_t _a5;
	a6_t _a6;
	a7_t _a7;
public:
	HBoundCall( CALL_t call_, a0_t a0, a1_t a1,
			a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7 )
		: _call( call_ ), _a0( a0 ), _a1( a1 ), _a2( a2 ), _a3( a3 ), _a4( a4 ), _a5( a5 ), _a6( a6 ), _a7( a7 ) {}
	template<typename fa0_t, typename fa1_t, typename fa2_t, typename fa3_t, typename fa4_t, typename fa5_t>
	return_t operator()( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3, fa4_t a4, fa5_t a5 ) { return ( _call(
				free_standing_call_args::resolve_arg<6, a0_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t>::get( _a0, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg<6, a1_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t>::get( _a1, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg<6, a2_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t>::get( _a2, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg<6, a3_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t>::get( _a3, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg<6, a4_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t>::get( _a4, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg<6, a5_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t>::get( _a5, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg<6, a6_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t>::get( _a6, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg<6, a7_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t>::get( _a7, a0, a1, a2, a3, a4, a5 ) ) ); }
	template<typename fa0_t, typename fa1_t, typename fa2_t, typename fa3_t, typename fa4_t, typename fa5_t>
	return_t operator()( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3, fa4_t a4, fa5_t a5 ) const { return ( _call(
				free_standing_call_args::resolve_arg<6, a0_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t>::get( _a0, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg<6, a1_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t>::get( _a1, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg<6, a2_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t>::get( _a2, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg<6, a3_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t>::get( _a3, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg<6, a4_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t>::get( _a4, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg<6, a5_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t>::get( _a5, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg<6, a6_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t>::get( _a6, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg<6, a7_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t>::get( _a7, a0, a1, a2, a3, a4, a5 ) ) ); }
	};

template<typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t, typename a6_t, typename a7_t,
	typename a8_t>
class HBoundCall<6, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t,
	trait::no_type>
	{
	CALL_t _call;
	a0_t _a0;
	a1_t _a1;
	a2_t _a2;
	a3_t _a3;
	a4_t _a4;
	a5_t _a5;
	a6_t _a6;
	a7_t _a7;
	a8_t _a8;
public:
	HBoundCall( CALL_t call_, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7, a8_t a8 )
		: _call( call_ ), _a0( a0 ), _a1( a1 ), _a2( a2 ), _a3( a3 ), _a4( a4 ), _a5( a5 ), _a6( a6 ), _a7( a7 ), _a8( a8 ) {}
	template<typename fa0_t, typename fa1_t, typename fa2_t, typename fa3_t, typename fa4_t, typename fa5_t>
	return_t operator()( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3, fa4_t a4, fa5_t a5 ) { return ( _call(
				free_standing_call_args::resolve_arg<6, a0_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t>::get( _a0, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg<6, a1_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t>::get( _a1, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg<6, a2_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t>::get( _a2, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg<6, a3_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t>::get( _a3, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg<6, a4_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t>::get( _a4, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg<6, a5_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t>::get( _a5, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg<6, a6_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t>::get( _a6, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg<6, a7_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t>::get( _a7, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg<6, a8_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t>::get( _a8, a0, a1, a2, a3, a4, a5 ) ) ); }
	template<typename fa0_t, typename fa1_t, typename fa2_t, typename fa3_t, typename fa4_t, typename fa5_t>
	return_t operator()( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3, fa4_t a4, fa5_t a5 ) const { return ( _call(
				free_standing_call_args::resolve_arg<6, a0_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t>::get( _a0, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg<6, a1_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t>::get( _a1, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg<6, a2_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t>::get( _a2, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg<6, a3_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t>::get( _a3, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg<6, a4_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t>::get( _a4, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg<6, a5_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t>::get( _a5, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg<6, a6_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t>::get( _a6, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg<6, a7_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t>::get( _a7, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg<6, a8_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t>::get( _a8, a0, a1, a2, a3, a4, a5 ) ) ); }
	};

template<typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t, typename a6_t, typename a7_t,
	typename a8_t, typename a9_t>
class HBoundCall<6, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t>
	{
	CALL_t _call;
	a0_t _a0;
	a1_t _a1;
	a2_t _a2;
	a3_t _a3;
	a4_t _a4;
	a5_t _a5;
	a6_t _a6;
	a7_t _a7;
	a8_t _a8;
	a9_t _a9;
public:
	HBoundCall( CALL_t call_, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7, a8_t a8, a9_t a9 )
		: _call( call_ ), _a0( a0 ), _a1( a1 ), _a2( a2 ), _a3( a3 ), _a4( a4 ), _a5( a5 ), _a6( a6 ), _a7( a7 ), _a8( a8 ), _a9( a9 ) {}
	template<typename fa0_t, typename fa1_t, typename fa2_t, typename fa3_t, typename fa4_t, typename fa5_t>
	return_t operator()( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3, fa4_t a4, fa5_t a5 ) { return ( _call(
				free_standing_call_args::resolve_arg<6, a0_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t>::get( _a0, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg<6, a1_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t>::get( _a1, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg<6, a2_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t>::get( _a2, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg<6, a3_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t>::get( _a3, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg<6, a4_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t>::get( _a4, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg<6, a5_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t>::get( _a5, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg<6, a6_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t>::get( _a6, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg<6, a7_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t>::get( _a7, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg<6, a8_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t>::get( _a8, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg<6, a9_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t>::get( _a9, a0, a1, a2, a3, a4, a5 ) ) ); }
	template<typename fa0_t, typename fa1_t, typename fa2_t, typename fa3_t, typename fa4_t, typename fa5_t>
	return_t operator()( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3, fa4_t a4, fa5_t a5 ) const { return ( _call(
				free_standing_call_args::resolve_arg<6, a0_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t>::get( _a0, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg<6, a1_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t>::get( _a1, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg<6, a2_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t>::get( _a2, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg<6, a3_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t>::get( _a3, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg<6, a4_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t>::get( _a4, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg<6, a5_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t>::get( _a5, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg<6, a6_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t>::get( _a6, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg<6, a7_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t>::get( _a7, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg<6, a8_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t>::get( _a8, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg<6, a9_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t>::get( _a9, a0, a1, a2, a3, a4, a5 ) ) ); }
	};

template<typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t, typename a6_t, typename a7_t,
	typename a8_t, typename a9_t, typename a10_t>
class HBoundCall<6, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t, a10_t>
	{
	CALL_t _call;
	a0_t _a0;
	a1_t _a1;
	a2_t _a2;
	a3_t _a3;
	a4_t _a4;
	a5_t _a5;
	a6_t _a6;
	a7_t _a7;
	a8_t _a8;
	a9_t _a9;
	a10_t _a10;
public:
	HBoundCall( CALL_t call_, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7, a8_t a8, a9_t a9, a10_t a10 )
		: _call( call_ ), _a0( a0 ), _a1( a1 ), _a2( a2 ), _a3( a3 ), _a4( a4 ), _a5( a5 ), _a6( a6 ), _a7( a7 ), _a8( a8 ), _a9( a9 ), _a10( a10 ) {}
	template<typename fa0_t, typename fa1_t, typename fa2_t, typename fa3_t, typename fa4_t, typename fa5_t>
	return_t operator()( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3, fa4_t a4, fa5_t a5 ) { return ( _call(
				free_standing_call_args::resolve_arg<6, a0_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t>::get( _a0, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg<6, a1_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t>::get( _a1, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg<6, a2_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t>::get( _a2, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg<6, a3_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t>::get( _a3, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg<6, a4_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t>::get( _a4, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg<6, a5_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t>::get( _a5, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg<6, a6_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t>::get( _a6, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg<6, a7_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t>::get( _a7, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg<6, a8_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t>::get( _a8, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg<6, a9_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t>::get( _a9, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg<6, a10_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t>::get( _a10, a0, a1, a2, a3, a4, a5 ) ) ); }
	template<typename fa0_t, typename fa1_t, typename fa2_t, typename fa3_t, typename fa4_t, typename fa5_t>
	return_t operator()( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3, fa4_t a4, fa5_t a5 ) const { return ( _call(
				free_standing_call_args::resolve_arg<6, a0_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t>::get( _a0, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg<6, a1_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t>::get( _a1, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg<6, a2_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t>::get( _a2, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg<6, a3_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t>::get( _a3, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg<6, a4_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t>::get( _a4, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg<6, a5_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t>::get( _a5, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg<6, a6_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t>::get( _a6, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg<6, a7_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t>::get( _a7, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg<6, a8_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t>::get( _a8, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg<6, a9_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t>::get( _a9, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg<6, a10_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t>::get( _a10, a0, a1, a2, a3, a4, a5 ) ) ); }
	};

template<typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t, typename a6_t>
class HBoundCall<7, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t,
	trait::no_type,	trait::no_type, trait::no_type>
	{
	CALL_t _call;
	a0_t _a0;
	a1_t _a1;
	a2_t _a2;
	a3_t _a3;
	a4_t _a4;
	a5_t _a5;
	a6_t _a6;
public:
	HBoundCall( CALL_t call_, a0_t a0, a1_t a1,
			a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6 )
		: _call( call_ ), _a0( a0 ), _a1( a1 ), _a2( a2 ), _a3( a3 ), _a4( a4 ), _a5( a5 ), _a6( a6 ) {}
	template<typename fa0_t, typename fa1_t, typename fa2_t, typename fa3_t, typename fa4_t, typename fa5_t, typename fa6_t>
	return_t operator()( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3, fa4_t a4, fa5_t a5, fa6_t a6 ) { return ( _call(
				free_standing_call_args::resolve_arg<7, a0_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t>::get( _a0, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg<7, a1_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t>::get( _a1, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg<7, a2_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t>::get( _a2, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg<7, a3_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t>::get( _a3, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg<7, a4_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t>::get( _a4, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg<7, a5_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t>::get( _a5, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg<7, a6_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t>::get( _a6, a0, a1, a2, a3, a4, a5, a6 ) ) ); }
	template<typename fa0_t, typename fa1_t, typename fa2_t, typename fa3_t, typename fa4_t, typename fa5_t, typename fa6_t>
	return_t operator()( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3, fa4_t a4, fa5_t a5, fa6_t a6 ) const { return ( _call(
				free_standing_call_args::resolve_arg<7, a0_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t>::get( _a0, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg<7, a1_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t>::get( _a1, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg<7, a2_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t>::get( _a2, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg<7, a3_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t>::get( _a3, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg<7, a4_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t>::get( _a4, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg<7, a5_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t>::get( _a5, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg<7, a6_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t>::get( _a6, a0, a1, a2, a3, a4, a5, a6 ) ) ); }
	};

template<typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t, typename a6_t, typename a7_t>
class HBoundCall<7, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t,
	trait::no_type, trait::no_type>
	{
	CALL_t _call;
	a0_t _a0;
	a1_t _a1;
	a2_t _a2;
	a3_t _a3;
	a4_t _a4;
	a5_t _a5;
	a6_t _a6;
	a7_t _a7;
public:
	HBoundCall( CALL_t call_, a0_t a0, a1_t a1,
			a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7 )
		: _call( call_ ), _a0( a0 ), _a1( a1 ), _a2( a2 ), _a3( a3 ), _a4( a4 ), _a5( a5 ), _a6( a6 ), _a7( a7 ) {}
	template<typename fa0_t, typename fa1_t, typename fa2_t, typename fa3_t, typename fa4_t, typename fa5_t, typename fa6_t>
	return_t operator()( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3, fa4_t a4, fa5_t a5, fa6_t a6 ) { return ( _call(
				free_standing_call_args::resolve_arg<7, a0_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t>::get( _a0, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg<7, a1_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t>::get( _a1, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg<7, a2_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t>::get( _a2, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg<7, a3_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t>::get( _a3, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg<7, a4_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t>::get( _a4, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg<7, a5_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t>::get( _a5, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg<7, a6_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t>::get( _a6, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg<7, a7_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t>::get( _a7, a0, a1, a2, a3, a4, a5, a6 ) ) ); }
	template<typename fa0_t, typename fa1_t, typename fa2_t, typename fa3_t, typename fa4_t, typename fa5_t, typename fa6_t>
	return_t operator()( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3, fa4_t a4, fa5_t a5, fa6_t a6 ) const { return ( _call(
				free_standing_call_args::resolve_arg<7, a0_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t>::get( _a0, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg<7, a1_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t>::get( _a1, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg<7, a2_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t>::get( _a2, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg<7, a3_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t>::get( _a3, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg<7, a4_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t>::get( _a4, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg<7, a5_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t>::get( _a5, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg<7, a6_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t>::get( _a6, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg<7, a7_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t>::get( _a7, a0, a1, a2, a3, a4, a5, a6 ) ) ); }
	};

template<typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t, typename a6_t, typename a7_t,
	typename a8_t>
class HBoundCall<7, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t,
	trait::no_type>
	{
	CALL_t _call;
	a0_t _a0;
	a1_t _a1;
	a2_t _a2;
	a3_t _a3;
	a4_t _a4;
	a5_t _a5;
	a6_t _a6;
	a7_t _a7;
	a8_t _a8;
public:
	HBoundCall( CALL_t call_, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7, a8_t a8 )
		: _call( call_ ), _a0( a0 ), _a1( a1 ), _a2( a2 ), _a3( a3 ), _a4( a4 ), _a5( a5 ), _a6( a6 ), _a7( a7 ), _a8( a8 ) {}
	template<typename fa0_t, typename fa1_t, typename fa2_t, typename fa3_t, typename fa4_t, typename fa5_t, typename fa6_t>
	return_t operator()( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3, fa4_t a4, fa5_t a5, fa6_t a6 ) { return ( _call(
				free_standing_call_args::resolve_arg<7, a0_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t>::get( _a0, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg<7, a1_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t>::get( _a1, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg<7, a2_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t>::get( _a2, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg<7, a3_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t>::get( _a3, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg<7, a4_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t>::get( _a4, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg<7, a5_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t>::get( _a5, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg<7, a6_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t>::get( _a6, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg<7, a7_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t>::get( _a7, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg<7, a8_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t>::get( _a8, a0, a1, a2, a3, a4, a5, a6 ) ) ); }
	template<typename fa0_t, typename fa1_t, typename fa2_t, typename fa3_t, typename fa4_t, typename fa5_t, typename fa6_t>
	return_t operator()( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3, fa4_t a4, fa5_t a5, fa6_t a6 ) const { return ( _call(
				free_standing_call_args::resolve_arg<7, a0_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t>::get( _a0, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg<7, a1_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t>::get( _a1, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg<7, a2_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t>::get( _a2, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg<7, a3_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t>::get( _a3, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg<7, a4_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t>::get( _a4, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg<7, a5_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t>::get( _a5, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg<7, a6_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t>::get( _a6, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg<7, a7_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t>::get( _a7, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg<7, a8_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t>::get( _a8, a0, a1, a2, a3, a4, a5, a6 ) ) ); }
	};

template<typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t, typename a6_t, typename a7_t,
	typename a8_t, typename a9_t>
class HBoundCall<7, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t>
	{
	CALL_t _call;
	a0_t _a0;
	a1_t _a1;
	a2_t _a2;
	a3_t _a3;
	a4_t _a4;
	a5_t _a5;
	a6_t _a6;
	a7_t _a7;
	a8_t _a8;
	a9_t _a9;
public:
	HBoundCall( CALL_t call_, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7, a8_t a8, a9_t a9 )
		: _call( call_ ), _a0( a0 ), _a1( a1 ), _a2( a2 ), _a3( a3 ), _a4( a4 ), _a5( a5 ), _a6( a6 ), _a7( a7 ), _a8( a8 ), _a9( a9 ) {}
	template<typename fa0_t, typename fa1_t, typename fa2_t, typename fa3_t, typename fa4_t, typename fa5_t, typename fa6_t>
	return_t operator()( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3, fa4_t a4, fa5_t a5, fa6_t a6 ) { return ( _call(
				free_standing_call_args::resolve_arg<7, a0_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t>::get( _a0, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg<7, a1_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t>::get( _a1, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg<7, a2_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t>::get( _a2, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg<7, a3_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t>::get( _a3, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg<7, a4_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t>::get( _a4, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg<7, a5_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t>::get( _a5, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg<7, a6_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t>::get( _a6, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg<7, a7_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t>::get( _a7, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg<7, a8_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t>::get( _a8, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg<7, a9_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t>::get( _a9, a0, a1, a2, a3, a4, a5, a6 ) ) ); }
	template<typename fa0_t, typename fa1_t, typename fa2_t, typename fa3_t, typename fa4_t, typename fa5_t, typename fa6_t>
	return_t operator()( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3, fa4_t a4, fa5_t a5, fa6_t a6 ) const { return ( _call(
				free_standing_call_args::resolve_arg<7, a0_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t>::get( _a0, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg<7, a1_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t>::get( _a1, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg<7, a2_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t>::get( _a2, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg<7, a3_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t>::get( _a3, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg<7, a4_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t>::get( _a4, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg<7, a5_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t>::get( _a5, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg<7, a6_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t>::get( _a6, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg<7, a7_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t>::get( _a7, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg<7, a8_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t>::get( _a8, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg<7, a9_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t>::get( _a9, a0, a1, a2, a3, a4, a5, a6 ) ) ); }
	};

template<typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t, typename a6_t, typename a7_t,
	typename a8_t, typename a9_t, typename a10_t>
class HBoundCall<7, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t, a10_t>
	{
	CALL_t _call;
	a0_t _a0;
	a1_t _a1;
	a2_t _a2;
	a3_t _a3;
	a4_t _a4;
	a5_t _a5;
	a6_t _a6;
	a7_t _a7;
	a8_t _a8;
	a9_t _a9;
	a10_t _a10;
public:
	HBoundCall( CALL_t call_, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7, a8_t a8, a9_t a9, a10_t a10 )
		: _call( call_ ), _a0( a0 ), _a1( a1 ), _a2( a2 ), _a3( a3 ), _a4( a4 ), _a5( a5 ), _a6( a6 ), _a7( a7 ), _a8( a8 ), _a9( a9 ), _a10( a10 ) {}
	template<typename fa0_t, typename fa1_t, typename fa2_t, typename fa3_t, typename fa4_t, typename fa5_t, typename fa6_t>
	return_t operator()( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3, fa4_t a4, fa5_t a5, fa6_t a6 ) { return ( _call(
				free_standing_call_args::resolve_arg<7, a0_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t>::get( _a0, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg<7, a1_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t>::get( _a1, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg<7, a2_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t>::get( _a2, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg<7, a3_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t>::get( _a3, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg<7, a4_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t>::get( _a4, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg<7, a5_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t>::get( _a5, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg<7, a6_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t>::get( _a6, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg<7, a7_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t>::get( _a7, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg<7, a8_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t>::get( _a8, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg<7, a9_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t>::get( _a9, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg<7, a10_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t>::get( _a10, a0, a1, a2, a3, a4, a5, a6 ) ) ); }
	template<typename fa0_t, typename fa1_t, typename fa2_t, typename fa3_t, typename fa4_t, typename fa5_t, typename fa6_t>
	return_t operator()( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3, fa4_t a4, fa5_t a5, fa6_t a6 ) const { return ( _call(
				free_standing_call_args::resolve_arg<7, a0_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t>::get( _a0, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg<7, a1_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t>::get( _a1, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg<7, a2_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t>::get( _a2, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg<7, a3_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t>::get( _a3, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg<7, a4_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t>::get( _a4, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg<7, a5_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t>::get( _a5, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg<7, a6_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t>::get( _a6, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg<7, a7_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t>::get( _a7, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg<7, a8_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t>::get( _a8, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg<7, a9_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t>::get( _a9, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg<7, a10_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t>::get( _a10, a0, a1, a2, a3, a4, a5, a6 ) ) ); }
	};

template<typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t, typename a6_t, typename a7_t>
class HBoundCall<8, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t,
	trait::no_type, trait::no_type>
	{
	CALL_t _call;
	a0_t _a0;
	a1_t _a1;
	a2_t _a2;
	a3_t _a3;
	a4_t _a4;
	a5_t _a5;
	a6_t _a6;
	a7_t _a7;
public:
	HBoundCall( CALL_t call_, a0_t a0, a1_t a1,
			a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7 )
		: _call( call_ ), _a0( a0 ), _a1( a1 ), _a2( a2 ), _a3( a3 ), _a4( a4 ), _a5( a5 ), _a6( a6 ), _a7( a7 ) {}
	template<typename fa0_t, typename fa1_t, typename fa2_t, typename fa3_t, typename fa4_t, typename fa5_t, typename fa6_t, typename fa7_t>
	return_t operator()( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3, fa4_t a4, fa5_t a5, fa6_t a6, fa7_t a7 ) { return ( _call(
				free_standing_call_args::resolve_arg<8, a0_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t>::get( _a0, a0, a1, a2, a3, a4, a5, a6, a7 ),
				free_standing_call_args::resolve_arg<8, a1_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t>::get( _a1, a0, a1, a2, a3, a4, a5, a6, a7 ),
				free_standing_call_args::resolve_arg<8, a2_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t>::get( _a2, a0, a1, a2, a3, a4, a5, a6, a7 ),
				free_standing_call_args::resolve_arg<8, a3_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t>::get( _a3, a0, a1, a2, a3, a4, a5, a6, a7 ),
				free_standing_call_args::resolve_arg<8, a4_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t>::get( _a4, a0, a1, a2, a3, a4, a5, a6, a7 ),
				free_standing_call_args::resolve_arg<8, a5_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t>::get( _a5, a0, a1, a2, a3, a4, a5, a6, a7 ),
				free_standing_call_args::resolve_arg<8, a6_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t>::get( _a6, a0, a1, a2, a3, a4, a5, a6, a7 ),
				free_standing_call_args::resolve_arg<8, a7_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t>::get( _a7, a0, a1, a2, a3, a4, a5, a6, a7 ) ) ); }
	template<typename fa0_t, typename fa1_t, typename fa2_t, typename fa3_t, typename fa4_t, typename fa5_t, typename fa6_t, typename fa7_t>
	return_t operator()( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3, fa4_t a4, fa5_t a5, fa6_t a6, fa7_t a7 ) const { return ( _call(
				free_standing_call_args::resolve_arg<8, a0_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t>::get( _a0, a0, a1, a2, a3, a4, a5, a6, a7 ),
				free_standing_call_args::resolve_arg<8, a1_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t>::get( _a1, a0, a1, a2, a3, a4, a5, a6, a7 ),
				free_standing_call_args::resolve_arg<8, a2_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t>::get( _a2, a0, a1, a2, a3, a4, a5, a6, a7 ),
				free_standing_call_args::resolve_arg<8, a3_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t>::get( _a3, a0, a1, a2, a3, a4, a5, a6, a7 ),
				free_standing_call_args::resolve_arg<8, a4_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t>::get( _a4, a0, a1, a2, a3, a4, a5, a6, a7 ),
				free_standing_call_args::resolve_arg<8, a5_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t>::get( _a5, a0, a1, a2, a3, a4, a5, a6, a7 ),
				free_standing_call_args::resolve_arg<8, a6_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t>::get( _a6, a0, a1, a2, a3, a4, a5, a6, a7 ),
				free_standing_call_args::resolve_arg<8, a7_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t>::get( _a7, a0, a1, a2, a3, a4, a5, a6, a7 ) ) ); }
	};

template<typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t, typename a6_t, typename a7_t,
	typename a8_t>
class HBoundCall<8, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t,
	trait::no_type>
	{
	CALL_t _call;
	a0_t _a0;
	a1_t _a1;
	a2_t _a2;
	a3_t _a3;
	a4_t _a4;
	a5_t _a5;
	a6_t _a6;
	a7_t _a7;
	a8_t _a8;
public:
	HBoundCall( CALL_t call_, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7, a8_t a8 )
		: _call( call_ ), _a0( a0 ), _a1( a1 ), _a2( a2 ), _a3( a3 ), _a4( a4 ), _a5( a5 ), _a6( a6 ), _a7( a7 ), _a8( a8 ) {}
	template<typename fa0_t, typename fa1_t, typename fa2_t, typename fa3_t, typename fa4_t, typename fa5_t, typename fa6_t, typename fa7_t>
	return_t operator()( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3, fa4_t a4, fa5_t a5, fa6_t a6, fa7_t a7 ) { return ( _call(
				free_standing_call_args::resolve_arg<8, a0_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t>::get( _a0, a0, a1, a2, a3, a4, a5, a6, a7 ),
				free_standing_call_args::resolve_arg<8, a1_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t>::get( _a1, a0, a1, a2, a3, a4, a5, a6, a7 ),
				free_standing_call_args::resolve_arg<8, a2_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t>::get( _a2, a0, a1, a2, a3, a4, a5, a6, a7 ),
				free_standing_call_args::resolve_arg<8, a3_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t>::get( _a3, a0, a1, a2, a3, a4, a5, a6, a7 ),
				free_standing_call_args::resolve_arg<8, a4_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t>::get( _a4, a0, a1, a2, a3, a4, a5, a6, a7 ),
				free_standing_call_args::resolve_arg<8, a5_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t>::get( _a5, a0, a1, a2, a3, a4, a5, a6, a7 ),
				free_standing_call_args::resolve_arg<8, a6_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t>::get( _a6, a0, a1, a2, a3, a4, a5, a6, a7 ),
				free_standing_call_args::resolve_arg<8, a7_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t>::get( _a7, a0, a1, a2, a3, a4, a5, a6, a7 ),
				free_standing_call_args::resolve_arg<8, a8_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t>::get( _a8, a0, a1, a2, a3, a4, a5, a6, a7 ) ) ); }
	template<typename fa0_t, typename fa1_t, typename fa2_t, typename fa3_t, typename fa4_t, typename fa5_t, typename fa6_t, typename fa7_t>
	return_t operator()( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3, fa4_t a4, fa5_t a5, fa6_t a6, fa7_t a7 ) const { return ( _call(
				free_standing_call_args::resolve_arg<8, a0_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t>::get( _a0, a0, a1, a2, a3, a4, a5, a6, a7 ),
				free_standing_call_args::resolve_arg<8, a1_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t>::get( _a1, a0, a1, a2, a3, a4, a5, a6, a7 ),
				free_standing_call_args::resolve_arg<8, a2_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t>::get( _a2, a0, a1, a2, a3, a4, a5, a6, a7 ),
				free_standing_call_args::resolve_arg<8, a3_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t>::get( _a3, a0, a1, a2, a3, a4, a5, a6, a7 ),
				free_standing_call_args::resolve_arg<8, a4_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t>::get( _a4, a0, a1, a2, a3, a4, a5, a6, a7 ),
				free_standing_call_args::resolve_arg<8, a5_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t>::get( _a5, a0, a1, a2, a3, a4, a5, a6, a7 ),
				free_standing_call_args::resolve_arg<8, a6_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t>::get( _a6, a0, a1, a2, a3, a4, a5, a6, a7 ),
				free_standing_call_args::resolve_arg<8, a7_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t>::get( _a7, a0, a1, a2, a3, a4, a5, a6, a7 ),
				free_standing_call_args::resolve_arg<8, a8_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t>::get( _a8, a0, a1, a2, a3, a4, a5, a6, a7 ) ) ); }
	};

template<typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t, typename a6_t, typename a7_t,
	typename a8_t, typename a9_t>
class HBoundCall<8, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t>
	{
	CALL_t _call;
	a0_t _a0;
	a1_t _a1;
	a2_t _a2;
	a3_t _a3;
	a4_t _a4;
	a5_t _a5;
	a6_t _a6;
	a7_t _a7;
	a8_t _a8;
	a9_t _a9;
public:
	HBoundCall( CALL_t call_, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7, a8_t a8, a9_t a9 )
		: _call( call_ ), _a0( a0 ), _a1( a1 ), _a2( a2 ), _a3( a3 ), _a4( a4 ), _a5( a5 ), _a6( a6 ), _a7( a7 ), _a8( a8 ), _a9( a9 ) {}
	template<typename fa0_t, typename fa1_t, typename fa2_t, typename fa3_t, typename fa4_t, typename fa5_t, typename fa6_t, typename fa7_t>
	return_t operator()( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3, fa4_t a4, fa5_t a5, fa6_t a6, fa7_t a7 ) { return ( _call(
				free_standing_call_args::resolve_arg<8, a0_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t>::get( _a0, a0, a1, a2, a3, a4, a5, a6, a7 ),
				free_standing_call_args::resolve_arg<8, a1_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t>::get( _a1, a0, a1, a2, a3, a4, a5, a6, a7 ),
				free_standing_call_args::resolve_arg<8, a2_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t>::get( _a2, a0, a1, a2, a3, a4, a5, a6, a7 ),
				free_standing_call_args::resolve_arg<8, a3_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t>::get( _a3, a0, a1, a2, a3, a4, a5, a6, a7 ),
				free_standing_call_args::resolve_arg<8, a4_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t>::get( _a4, a0, a1, a2, a3, a4, a5, a6, a7 ),
				free_standing_call_args::resolve_arg<8, a5_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t>::get( _a5, a0, a1, a2, a3, a4, a5, a6, a7 ),
				free_standing_call_args::resolve_arg<8, a6_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t>::get( _a6, a0, a1, a2, a3, a4, a5, a6, a7 ),
				free_standing_call_args::resolve_arg<8, a7_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t>::get( _a7, a0, a1, a2, a3, a4, a5, a6, a7 ),
				free_standing_call_args::resolve_arg<8, a8_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t>::get( _a8, a0, a1, a2, a3, a4, a5, a6, a7 ),
				free_standing_call_args::resolve_arg<8, a9_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t>::get( _a9, a0, a1, a2, a3, a4, a5, a6, a7 ) ) ); }
	template<typename fa0_t, typename fa1_t, typename fa2_t, typename fa3_t, typename fa4_t, typename fa5_t, typename fa6_t, typename fa7_t>
	return_t operator()( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3, fa4_t a4, fa5_t a5, fa6_t a6, fa7_t a7 ) const { return ( _call(
				free_standing_call_args::resolve_arg<8, a0_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t>::get( _a0, a0, a1, a2, a3, a4, a5, a6, a7 ),
				free_standing_call_args::resolve_arg<8, a1_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t>::get( _a1, a0, a1, a2, a3, a4, a5, a6, a7 ),
				free_standing_call_args::resolve_arg<8, a2_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t>::get( _a2, a0, a1, a2, a3, a4, a5, a6, a7 ),
				free_standing_call_args::resolve_arg<8, a3_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t>::get( _a3, a0, a1, a2, a3, a4, a5, a6, a7 ),
				free_standing_call_args::resolve_arg<8, a4_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t>::get( _a4, a0, a1, a2, a3, a4, a5, a6, a7 ),
				free_standing_call_args::resolve_arg<8, a5_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t>::get( _a5, a0, a1, a2, a3, a4, a5, a6, a7 ),
				free_standing_call_args::resolve_arg<8, a6_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t>::get( _a6, a0, a1, a2, a3, a4, a5, a6, a7 ),
				free_standing_call_args::resolve_arg<8, a7_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t>::get( _a7, a0, a1, a2, a3, a4, a5, a6, a7 ),
				free_standing_call_args::resolve_arg<8, a8_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t>::get( _a8, a0, a1, a2, a3, a4, a5, a6, a7 ),
				free_standing_call_args::resolve_arg<8, a9_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t>::get( _a9, a0, a1, a2, a3, a4, a5, a6, a7 ) ) ); }
	};

template<typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t, typename a6_t, typename a7_t,
	typename a8_t, typename a9_t, typename a10_t>
class HBoundCall<8, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t, a10_t>
	{
	CALL_t _call;
	a0_t _a0;
	a1_t _a1;
	a2_t _a2;
	a3_t _a3;
	a4_t _a4;
	a5_t _a5;
	a6_t _a6;
	a7_t _a7;
	a8_t _a8;
	a9_t _a9;
	a10_t _a10;
public:
	HBoundCall( CALL_t call_, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7, a8_t a8, a9_t a9, a10_t a10 )
		: _call( call_ ), _a0( a0 ), _a1( a1 ), _a2( a2 ), _a3( a3 ), _a4( a4 ), _a5( a5 ), _a6( a6 ), _a7( a7 ), _a8( a8 ), _a9( a9 ), _a10( a10 ) {}
	template<typename fa0_t, typename fa1_t, typename fa2_t, typename fa3_t, typename fa4_t, typename fa5_t, typename fa6_t, typename fa7_t>
	return_t operator()( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3, fa4_t a4, fa5_t a5, fa6_t a6, fa7_t a7 ) { return ( _call(
				free_standing_call_args::resolve_arg<8, a0_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t>::get( _a0, a0, a1, a2, a3, a4, a5, a6, a7 ),
				free_standing_call_args::resolve_arg<8, a1_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t>::get( _a1, a0, a1, a2, a3, a4, a5, a6, a7 ),
				free_standing_call_args::resolve_arg<8, a2_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t>::get( _a2, a0, a1, a2, a3, a4, a5, a6, a7 ),
				free_standing_call_args::resolve_arg<8, a3_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t>::get( _a3, a0, a1, a2, a3, a4, a5, a6, a7 ),
				free_standing_call_args::resolve_arg<8, a4_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t>::get( _a4, a0, a1, a2, a3, a4, a5, a6, a7 ),
				free_standing_call_args::resolve_arg<8, a5_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t>::get( _a5, a0, a1, a2, a3, a4, a5, a6, a7 ),
				free_standing_call_args::resolve_arg<8, a6_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t>::get( _a6, a0, a1, a2, a3, a4, a5, a6, a7 ),
				free_standing_call_args::resolve_arg<8, a7_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t>::get( _a7, a0, a1, a2, a3, a4, a5, a6, a7 ),
				free_standing_call_args::resolve_arg<8, a8_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t>::get( _a8, a0, a1, a2, a3, a4, a5, a6, a7 ),
				free_standing_call_args::resolve_arg<8, a9_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t>::get( _a9, a0, a1, a2, a3, a4, a5, a6, a7 ),
				free_standing_call_args::resolve_arg<8, a10_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t>::get( _a10, a0, a1, a2, a3, a4, a5, a6, a7 ) ) ); }
	template<typename fa0_t, typename fa1_t, typename fa2_t, typename fa3_t, typename fa4_t, typename fa5_t, typename fa6_t, typename fa7_t>
	return_t operator()( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3, fa4_t a4, fa5_t a5, fa6_t a6, fa7_t a7 ) const { return ( _call(
				free_standing_call_args::resolve_arg<8, a0_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t>::get( _a0, a0, a1, a2, a3, a4, a5, a6, a7 ),
				free_standing_call_args::resolve_arg<8, a1_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t>::get( _a1, a0, a1, a2, a3, a4, a5, a6, a7 ),
				free_standing_call_args::resolve_arg<8, a2_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t>::get( _a2, a0, a1, a2, a3, a4, a5, a6, a7 ),
				free_standing_call_args::resolve_arg<8, a3_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t>::get( _a3, a0, a1, a2, a3, a4, a5, a6, a7 ),
				free_standing_call_args::resolve_arg<8, a4_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t>::get( _a4, a0, a1, a2, a3, a4, a5, a6, a7 ),
				free_standing_call_args::resolve_arg<8, a5_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t>::get( _a5, a0, a1, a2, a3, a4, a5, a6, a7 ),
				free_standing_call_args::resolve_arg<8, a6_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t>::get( _a6, a0, a1, a2, a3, a4, a5, a6, a7 ),
				free_standing_call_args::resolve_arg<8, a7_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t>::get( _a7, a0, a1, a2, a3, a4, a5, a6, a7 ),
				free_standing_call_args::resolve_arg<8, a8_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t>::get( _a8, a0, a1, a2, a3, a4, a5, a6, a7 ),
				free_standing_call_args::resolve_arg<8, a9_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t>::get( _a9, a0, a1, a2, a3, a4, a5, a6, a7 ),
				free_standing_call_args::resolve_arg<8, a10_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t>::get( _a10, a0, a1, a2, a3, a4, a5, a6, a7 ) ) ); }
	};

template<typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t, typename a6_t, typename a7_t,
	typename a8_t>
class HBoundCall<9, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t,
	trait::no_type>
	{
	CALL_t _call;
	a0_t _a0;
	a1_t _a1;
	a2_t _a2;
	a3_t _a3;
	a4_t _a4;
	a5_t _a5;
	a6_t _a6;
	a7_t _a7;
	a8_t _a8;
public:
	HBoundCall( CALL_t call_, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7, a8_t a8 )
		: _call( call_ ), _a0( a0 ), _a1( a1 ), _a2( a2 ), _a3( a3 ), _a4( a4 ), _a5( a5 ), _a6( a6 ), _a7( a7 ), _a8( a8 ) {}
	template<typename fa0_t, typename fa1_t, typename fa2_t, typename fa3_t, typename fa4_t, typename fa5_t, typename fa6_t, typename fa7_t, typename fa8_t>
	return_t operator()( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3, fa4_t a4, fa5_t a5, fa6_t a6, fa7_t a7, fa8_t a8 ) { return ( _call(
				free_standing_call_args::resolve_arg<9, a0_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t>::get( _a0, a0, a1, a2, a3, a4, a5, a6, a7, a8 ),
				free_standing_call_args::resolve_arg<9, a1_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t>::get( _a1, a0, a1, a2, a3, a4, a5, a6, a7, a8 ),
				free_standing_call_args::resolve_arg<9, a2_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t>::get( _a2, a0, a1, a2, a3, a4, a5, a6, a7, a8 ),
				free_standing_call_args::resolve_arg<9, a3_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t>::get( _a3, a0, a1, a2, a3, a4, a5, a6, a7, a8 ),
				free_standing_call_args::resolve_arg<9, a4_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t>::get( _a4, a0, a1, a2, a3, a4, a5, a6, a7, a8 ),
				free_standing_call_args::resolve_arg<9, a5_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t>::get( _a5, a0, a1, a2, a3, a4, a5, a6, a7, a8 ),
				free_standing_call_args::resolve_arg<9, a6_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t>::get( _a6, a0, a1, a2, a3, a4, a5, a6, a7, a8 ),
				free_standing_call_args::resolve_arg<9, a7_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t>::get( _a7, a0, a1, a2, a3, a4, a5, a6, a7, a8 ),
				free_standing_call_args::resolve_arg<9, a8_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t>::get( _a8, a0, a1, a2, a3, a4, a5, a6, a7, a8 ) ) ); }
	template<typename fa0_t, typename fa1_t, typename fa2_t, typename fa3_t, typename fa4_t, typename fa5_t, typename fa6_t, typename fa7_t, typename fa8_t>
	return_t operator()( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3, fa4_t a4, fa5_t a5, fa6_t a6, fa7_t a7, fa8_t a8 ) const { return ( _call(
				free_standing_call_args::resolve_arg<9, a0_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t>::get( _a0, a0, a1, a2, a3, a4, a5, a6, a7, a8 ),
				free_standing_call_args::resolve_arg<9, a1_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t>::get( _a1, a0, a1, a2, a3, a4, a5, a6, a7, a8 ),
				free_standing_call_args::resolve_arg<9, a2_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t>::get( _a2, a0, a1, a2, a3, a4, a5, a6, a7, a8 ),
				free_standing_call_args::resolve_arg<9, a3_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t>::get( _a3, a0, a1, a2, a3, a4, a5, a6, a7, a8 ),
				free_standing_call_args::resolve_arg<9, a4_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t>::get( _a4, a0, a1, a2, a3, a4, a5, a6, a7, a8 ),
				free_standing_call_args::resolve_arg<9, a5_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t>::get( _a5, a0, a1, a2, a3, a4, a5, a6, a7, a8 ),
				free_standing_call_args::resolve_arg<9, a6_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t>::get( _a6, a0, a1, a2, a3, a4, a5, a6, a7, a8 ),
				free_standing_call_args::resolve_arg<9, a7_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t>::get( _a7, a0, a1, a2, a3, a4, a5, a6, a7, a8 ),
				free_standing_call_args::resolve_arg<9, a8_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t>::get( _a8, a0, a1, a2, a3, a4, a5, a6, a7, a8 ) ) ); }
	};

template<typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t, typename a6_t, typename a7_t,
	typename a8_t, typename a9_t>
class HBoundCall<9, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t>
	{
	CALL_t _call;
	a0_t _a0;
	a1_t _a1;
	a2_t _a2;
	a3_t _a3;
	a4_t _a4;
	a5_t _a5;
	a6_t _a6;
	a7_t _a7;
	a8_t _a8;
	a9_t _a9;
public:
	HBoundCall( CALL_t call_, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7, a8_t a8, a9_t a9 )
		: _call( call_ ), _a0( a0 ), _a1( a1 ), _a2( a2 ), _a3( a3 ), _a4( a4 ), _a5( a5 ), _a6( a6 ), _a7( a7 ), _a8( a8 ), _a9( a9 ) {}
	template<typename fa0_t, typename fa1_t, typename fa2_t, typename fa3_t, typename fa4_t, typename fa5_t, typename fa6_t, typename fa7_t, typename fa8_t>
	return_t operator()( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3, fa4_t a4, fa5_t a5, fa6_t a6, fa7_t a7, fa8_t a8 ) { return ( _call(
				free_standing_call_args::resolve_arg<9, a0_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t>::get( _a0, a0, a1, a2, a3, a4, a5, a6, a7, a8 ),
				free_standing_call_args::resolve_arg<9, a1_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t>::get( _a1, a0, a1, a2, a3, a4, a5, a6, a7, a8 ),
				free_standing_call_args::resolve_arg<9, a2_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t>::get( _a2, a0, a1, a2, a3, a4, a5, a6, a7, a8 ),
				free_standing_call_args::resolve_arg<9, a3_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t>::get( _a3, a0, a1, a2, a3, a4, a5, a6, a7, a8 ),
				free_standing_call_args::resolve_arg<9, a4_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t>::get( _a4, a0, a1, a2, a3, a4, a5, a6, a7, a8 ),
				free_standing_call_args::resolve_arg<9, a5_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t>::get( _a5, a0, a1, a2, a3, a4, a5, a6, a7, a8 ),
				free_standing_call_args::resolve_arg<9, a6_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t>::get( _a6, a0, a1, a2, a3, a4, a5, a6, a7, a8 ),
				free_standing_call_args::resolve_arg<9, a7_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t>::get( _a7, a0, a1, a2, a3, a4, a5, a6, a7, a8 ),
				free_standing_call_args::resolve_arg<9, a8_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t>::get( _a8, a0, a1, a2, a3, a4, a5, a6, a7, a8 ),
				free_standing_call_args::resolve_arg<9, a9_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t>::get( _a9, a0, a1, a2, a3, a4, a5, a6, a7, a8 ) ) ); }
	template<typename fa0_t, typename fa1_t, typename fa2_t, typename fa3_t, typename fa4_t, typename fa5_t, typename fa6_t, typename fa7_t, typename fa8_t>
	return_t operator()( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3, fa4_t a4, fa5_t a5, fa6_t a6, fa7_t a7, fa8_t a8 ) const { return ( _call(
				free_standing_call_args::resolve_arg<9, a0_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t>::get( _a0, a0, a1, a2, a3, a4, a5, a6, a7, a8 ),
				free_standing_call_args::resolve_arg<9, a1_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t>::get( _a1, a0, a1, a2, a3, a4, a5, a6, a7, a8 ),
				free_standing_call_args::resolve_arg<9, a2_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t>::get( _a2, a0, a1, a2, a3, a4, a5, a6, a7, a8 ),
				free_standing_call_args::resolve_arg<9, a3_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t>::get( _a3, a0, a1, a2, a3, a4, a5, a6, a7, a8 ),
				free_standing_call_args::resolve_arg<9, a4_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t>::get( _a4, a0, a1, a2, a3, a4, a5, a6, a7, a8 ),
				free_standing_call_args::resolve_arg<9, a5_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t>::get( _a5, a0, a1, a2, a3, a4, a5, a6, a7, a8 ),
				free_standing_call_args::resolve_arg<9, a6_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t>::get( _a6, a0, a1, a2, a3, a4, a5, a6, a7, a8 ),
				free_standing_call_args::resolve_arg<9, a7_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t>::get( _a7, a0, a1, a2, a3, a4, a5, a6, a7, a8 ),
				free_standing_call_args::resolve_arg<9, a8_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t>::get( _a8, a0, a1, a2, a3, a4, a5, a6, a7, a8 ),
				free_standing_call_args::resolve_arg<9, a9_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t>::get( _a9, a0, a1, a2, a3, a4, a5, a6, a7, a8 ) ) ); }
	};

template<typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t, typename a6_t, typename a7_t,
	typename a8_t, typename a9_t, typename a10_t>
class HBoundCall<9, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t, a10_t>
	{
	CALL_t _call;
	a0_t _a0;
	a1_t _a1;
	a2_t _a2;
	a3_t _a3;
	a4_t _a4;
	a5_t _a5;
	a6_t _a6;
	a7_t _a7;
	a8_t _a8;
	a9_t _a9;
	a10_t _a10;
public:
	HBoundCall( CALL_t call_, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7, a8_t a8, a9_t a9, a10_t a10 )
		: _call( call_ ), _a0( a0 ), _a1( a1 ), _a2( a2 ), _a3( a3 ), _a4( a4 ), _a5( a5 ), _a6( a6 ), _a7( a7 ), _a8( a8 ), _a9( a9 ), _a10( a10 ) {}
	template<typename fa0_t, typename fa1_t, typename fa2_t, typename fa3_t, typename fa4_t, typename fa5_t, typename fa6_t, typename fa7_t, typename fa8_t>
	return_t operator()( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3, fa4_t a4, fa5_t a5, fa6_t a6, fa7_t a7, fa8_t a8 ) { return ( _call(
				free_standing_call_args::resolve_arg<9, a0_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t>::get( _a0, a0, a1, a2, a3, a4, a5, a6, a7, a8 ),
				free_standing_call_args::resolve_arg<9, a1_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t>::get( _a1, a0, a1, a2, a3, a4, a5, a6, a7, a8 ),
				free_standing_call_args::resolve_arg<9, a2_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t>::get( _a2, a0, a1, a2, a3, a4, a5, a6, a7, a8 ),
				free_standing_call_args::resolve_arg<9, a3_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t>::get( _a3, a0, a1, a2, a3, a4, a5, a6, a7, a8 ),
				free_standing_call_args::resolve_arg<9, a4_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t>::get( _a4, a0, a1, a2, a3, a4, a5, a6, a7, a8 ),
				free_standing_call_args::resolve_arg<9, a5_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t>::get( _a5, a0, a1, a2, a3, a4, a5, a6, a7, a8 ),
				free_standing_call_args::resolve_arg<9, a6_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t>::get( _a6, a0, a1, a2, a3, a4, a5, a6, a7, a8 ),
				free_standing_call_args::resolve_arg<9, a7_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t>::get( _a7, a0, a1, a2, a3, a4, a5, a6, a7, a8 ),
				free_standing_call_args::resolve_arg<9, a8_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t>::get( _a8, a0, a1, a2, a3, a4, a5, a6, a7, a8 ),
				free_standing_call_args::resolve_arg<9, a9_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t>::get( _a9, a0, a1, a2, a3, a4, a5, a6, a7, a8 ),
				free_standing_call_args::resolve_arg<9, a10_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t>::get( _a10, a0, a1, a2, a3, a4, a5, a6, a7, a8 ) ) ); }
	template<typename fa0_t, typename fa1_t, typename fa2_t, typename fa3_t, typename fa4_t, typename fa5_t, typename fa6_t, typename fa7_t, typename fa8_t>
	return_t operator()( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3, fa4_t a4, fa5_t a5, fa6_t a6, fa7_t a7, fa8_t a8 ) const { return ( _call(
				free_standing_call_args::resolve_arg<9, a0_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t>::get( _a0, a0, a1, a2, a3, a4, a5, a6, a7, a8 ),
				free_standing_call_args::resolve_arg<9, a1_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t>::get( _a1, a0, a1, a2, a3, a4, a5, a6, a7, a8 ),
				free_standing_call_args::resolve_arg<9, a2_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t>::get( _a2, a0, a1, a2, a3, a4, a5, a6, a7, a8 ),
				free_standing_call_args::resolve_arg<9, a3_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t>::get( _a3, a0, a1, a2, a3, a4, a5, a6, a7, a8 ),
				free_standing_call_args::resolve_arg<9, a4_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t>::get( _a4, a0, a1, a2, a3, a4, a5, a6, a7, a8 ),
				free_standing_call_args::resolve_arg<9, a5_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t>::get( _a5, a0, a1, a2, a3, a4, a5, a6, a7, a8 ),
				free_standing_call_args::resolve_arg<9, a6_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t>::get( _a6, a0, a1, a2, a3, a4, a5, a6, a7, a8 ),
				free_standing_call_args::resolve_arg<9, a7_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t>::get( _a7, a0, a1, a2, a3, a4, a5, a6, a7, a8 ),
				free_standing_call_args::resolve_arg<9, a8_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t>::get( _a8, a0, a1, a2, a3, a4, a5, a6, a7, a8 ),
				free_standing_call_args::resolve_arg<9, a9_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t>::get( _a9, a0, a1, a2, a3, a4, a5, a6, a7, a8 ),
				free_standing_call_args::resolve_arg<9, a10_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t>::get( _a10, a0, a1, a2, a3, a4, a5, a6, a7, a8 ) ) ); }
	};

template<typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t, typename a6_t, typename a7_t,
	typename a8_t, typename a9_t>
class HBoundCall<10, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t>
	{
	CALL_t _call;
	a0_t _a0;
	a1_t _a1;
	a2_t _a2;
	a3_t _a3;
	a4_t _a4;
	a5_t _a5;
	a6_t _a6;
	a7_t _a7;
	a8_t _a8;
	a9_t _a9;
public:
	HBoundCall( CALL_t call_, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7, a8_t a8, a9_t a9 )
		: _call( call_ ), _a0( a0 ), _a1( a1 ), _a2( a2 ), _a3( a3 ), _a4( a4 ), _a5( a5 ), _a6( a6 ), _a7( a7 ), _a8( a8 ), _a9( a9 ) {}
	template<typename fa0_t, typename fa1_t, typename fa2_t, typename fa3_t, typename fa4_t, typename fa5_t, typename fa6_t, typename fa7_t, typename fa8_t, typename fa9_t>
	return_t operator()( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3, fa4_t a4, fa5_t a5, fa6_t a6, fa7_t a7, fa8_t a8, fa9_t a9 ) { return ( _call(
				free_standing_call_args::resolve_arg<10, a0_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t, fa9_t>::get( _a0, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9 ),
				free_standing_call_args::resolve_arg<10, a1_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t, fa9_t>::get( _a1, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9 ),
				free_standing_call_args::resolve_arg<10, a2_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t, fa9_t>::get( _a2, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9 ),
				free_standing_call_args::resolve_arg<10, a3_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t, fa9_t>::get( _a3, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9 ),
				free_standing_call_args::resolve_arg<10, a4_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t, fa9_t>::get( _a4, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9 ),
				free_standing_call_args::resolve_arg<10, a5_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t, fa9_t>::get( _a5, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9 ),
				free_standing_call_args::resolve_arg<10, a6_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t, fa9_t>::get( _a6, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9 ),
				free_standing_call_args::resolve_arg<10, a7_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t, fa9_t>::get( _a7, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9 ),
				free_standing_call_args::resolve_arg<10, a8_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t, fa9_t>::get( _a8, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9 ),
				free_standing_call_args::resolve_arg<10, a9_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t, fa9_t>::get( _a9, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9 ) ) ); }
	template<typename fa0_t, typename fa1_t, typename fa2_t, typename fa3_t, typename fa4_t, typename fa5_t, typename fa6_t, typename fa7_t, typename fa8_t, typename fa9_t>
	return_t operator()( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3, fa4_t a4, fa5_t a5, fa6_t a6, fa7_t a7, fa8_t a8, fa9_t a9 ) const { return ( _call(
				free_standing_call_args::resolve_arg<10, a0_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t, fa9_t>::get( _a0, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9 ),
				free_standing_call_args::resolve_arg<10, a1_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t, fa9_t>::get( _a1, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9 ),
				free_standing_call_args::resolve_arg<10, a2_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t, fa9_t>::get( _a2, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9 ),
				free_standing_call_args::resolve_arg<10, a3_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t, fa9_t>::get( _a3, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9 ),
				free_standing_call_args::resolve_arg<10, a4_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t, fa9_t>::get( _a4, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9 ),
				free_standing_call_args::resolve_arg<10, a5_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t, fa9_t>::get( _a5, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9 ),
				free_standing_call_args::resolve_arg<10, a6_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t, fa9_t>::get( _a6, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9 ),
				free_standing_call_args::resolve_arg<10, a7_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t, fa9_t>::get( _a7, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9 ),
				free_standing_call_args::resolve_arg<10, a8_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t, fa9_t>::get( _a8, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9 ),
				free_standing_call_args::resolve_arg<10, a9_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t, fa9_t>::get( _a9, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9 ) ) ); }
	};

template<typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t, typename a6_t, typename a7_t,
	typename a8_t, typename a9_t, typename a10_t>
class HBoundCall<10, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t, a10_t>
	{
	CALL_t _call;
	a0_t _a0;
	a1_t _a1;
	a2_t _a2;
	a3_t _a3;
	a4_t _a4;
	a5_t _a5;
	a6_t _a6;
	a7_t _a7;
	a8_t _a8;
	a9_t _a9;
	a10_t _a10;
public:
	HBoundCall( CALL_t call_, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7, a8_t a8, a9_t a9, a10_t a10 )
		: _call( call_ ), _a0( a0 ), _a1( a1 ), _a2( a2 ), _a3( a3 ), _a4( a4 ), _a5( a5 ), _a6( a6 ), _a7( a7 ), _a8( a8 ), _a9( a9 ), _a10( a10 ) {}
	template<typename fa0_t, typename fa1_t, typename fa2_t, typename fa3_t, typename fa4_t, typename fa5_t, typename fa6_t, typename fa7_t, typename fa8_t, typename fa9_t>
	return_t operator()( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3, fa4_t a4, fa5_t a5, fa6_t a6, fa7_t a7, fa8_t a8, fa9_t a9 ) { return ( _call(
				free_standing_call_args::resolve_arg<10, a0_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t, fa9_t>::get( _a0, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9 ),
				free_standing_call_args::resolve_arg<10, a1_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t, fa9_t>::get( _a1, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9 ),
				free_standing_call_args::resolve_arg<10, a2_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t, fa9_t>::get( _a2, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9 ),
				free_standing_call_args::resolve_arg<10, a3_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t, fa9_t>::get( _a3, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9 ),
				free_standing_call_args::resolve_arg<10, a4_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t, fa9_t>::get( _a4, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9 ),
				free_standing_call_args::resolve_arg<10, a5_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t, fa9_t>::get( _a5, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9 ),
				free_standing_call_args::resolve_arg<10, a6_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t, fa9_t>::get( _a6, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9 ),
				free_standing_call_args::resolve_arg<10, a7_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t, fa9_t>::get( _a7, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9 ),
				free_standing_call_args::resolve_arg<10, a8_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t, fa9_t>::get( _a8, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9 ),
				free_standing_call_args::resolve_arg<10, a9_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t, fa9_t>::get( _a9, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9 ),
				free_standing_call_args::resolve_arg<10, a10_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t, fa9_t>::get( _a10, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9 ) ) ); }
	template<typename fa0_t, typename fa1_t, typename fa2_t, typename fa3_t, typename fa4_t, typename fa5_t, typename fa6_t, typename fa7_t, typename fa8_t, typename fa9_t>
	return_t operator()( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3, fa4_t a4, fa5_t a5, fa6_t a6, fa7_t a7, fa8_t a8, fa9_t a9 ) const { return ( _call(
				free_standing_call_args::resolve_arg<10, a0_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t, fa9_t>::get( _a0, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9 ),
				free_standing_call_args::resolve_arg<10, a1_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t, fa9_t>::get( _a1, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9 ),
				free_standing_call_args::resolve_arg<10, a2_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t, fa9_t>::get( _a2, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9 ),
				free_standing_call_args::resolve_arg<10, a3_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t, fa9_t>::get( _a3, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9 ),
				free_standing_call_args::resolve_arg<10, a4_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t, fa9_t>::get( _a4, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9 ),
				free_standing_call_args::resolve_arg<10, a5_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t, fa9_t>::get( _a5, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9 ),
				free_standing_call_args::resolve_arg<10, a6_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t, fa9_t>::get( _a6, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9 ),
				free_standing_call_args::resolve_arg<10, a7_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t, fa9_t>::get( _a7, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9 ),
				free_standing_call_args::resolve_arg<10, a8_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t, fa9_t>::get( _a8, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9 ),
				free_standing_call_args::resolve_arg<10, a9_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t, fa9_t>::get( _a9, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9 ),
				free_standing_call_args::resolve_arg<10, a10_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t, fa9_t>::get( _a10, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9 ) ) ); }
	};

template<typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t, typename a6_t, typename a7_t,
	typename a8_t, typename a9_t, typename a10_t>
class HBoundCall<11, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t, a10_t>
	{
	CALL_t _call;
	a0_t _a0;
	a1_t _a1;
	a2_t _a2;
	a3_t _a3;
	a4_t _a4;
	a5_t _a5;
	a6_t _a6;
	a7_t _a7;
	a8_t _a8;
	a9_t _a9;
	a10_t _a10;
public:
	HBoundCall( CALL_t call_, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7, a8_t a8, a9_t a9, a10_t a10 )
		: _call( call_ ), _a0( a0 ), _a1( a1 ), _a2( a2 ), _a3( a3 ), _a4( a4 ), _a5( a5 ), _a6( a6 ), _a7( a7 ), _a8( a8 ), _a9( a9 ), _a10( a10 ) {}
	template<typename fa0_t, typename fa1_t, typename fa2_t, typename fa3_t, typename fa4_t, typename fa5_t, typename fa6_t, typename fa7_t, typename fa8_t, typename fa9_t, typename fa10_t>
	return_t operator()( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3, fa4_t a4, fa5_t a5, fa6_t a6, fa7_t a7, fa8_t a8, fa9_t a9, fa10_t a10 ) { return ( _call(
				free_standing_call_args::resolve_arg<11, a0_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t, fa9_t, fa10_t>::get( _a0, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10 ),
				free_standing_call_args::resolve_arg<11, a1_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t, fa9_t, fa10_t>::get( _a1, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10 ),
				free_standing_call_args::resolve_arg<11, a2_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t, fa9_t, fa10_t>::get( _a2, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10 ),
				free_standing_call_args::resolve_arg<11, a3_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t, fa9_t, fa10_t>::get( _a3, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10 ),
				free_standing_call_args::resolve_arg<11, a4_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t, fa9_t, fa10_t>::get( _a4, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10 ),
				free_standing_call_args::resolve_arg<11, a5_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t, fa9_t, fa10_t>::get( _a5, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10 ),
				free_standing_call_args::resolve_arg<11, a6_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t, fa9_t, fa10_t>::get( _a6, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10 ),
				free_standing_call_args::resolve_arg<11, a7_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t, fa9_t, fa10_t>::get( _a7, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10 ),
				free_standing_call_args::resolve_arg<11, a8_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t, fa9_t, fa10_t>::get( _a8, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10 ),
				free_standing_call_args::resolve_arg<11, a9_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t, fa9_t, fa10_t>::get( _a9, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10 ),
				free_standing_call_args::resolve_arg<11, a10_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t, fa9_t, fa10_t>::get( _a10, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10 ) ) ); }
	template<typename fa0_t, typename fa1_t, typename fa2_t, typename fa3_t, typename fa4_t, typename fa5_t, typename fa6_t, typename fa7_t, typename fa8_t, typename fa9_t, typename fa10_t>
	return_t operator()( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3, fa4_t a4, fa5_t a5, fa6_t a6, fa7_t a7, fa8_t a8, fa9_t a9, fa10_t a10 ) const { return ( _call(
				free_standing_call_args::resolve_arg<11, a0_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t, fa9_t, fa10_t>::get( _a0, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10 ),
				free_standing_call_args::resolve_arg<11, a1_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t, fa9_t, fa10_t>::get( _a1, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10 ),
				free_standing_call_args::resolve_arg<11, a2_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t, fa9_t, fa10_t>::get( _a2, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10 ),
				free_standing_call_args::resolve_arg<11, a3_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t, fa9_t, fa10_t>::get( _a3, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10 ),
				free_standing_call_args::resolve_arg<11, a4_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t, fa9_t, fa10_t>::get( _a4, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10 ),
				free_standing_call_args::resolve_arg<11, a5_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t, fa9_t, fa10_t>::get( _a5, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10 ),
				free_standing_call_args::resolve_arg<11, a6_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t, fa9_t, fa10_t>::get( _a6, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10 ),
				free_standing_call_args::resolve_arg<11, a7_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t, fa9_t, fa10_t>::get( _a7, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10 ),
				free_standing_call_args::resolve_arg<11, a8_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t, fa9_t, fa10_t>::get( _a8, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10 ),
				free_standing_call_args::resolve_arg<11, a9_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t, fa9_t, fa10_t>::get( _a9, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10 ),
				free_standing_call_args::resolve_arg<11, a10_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t, fa9_t, fa10_t>::get( _a10, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10 ) ) ); }
	};

/*! \endcond */

#endif

template<typename METHOD_t>
typename bound_call_calculator<METHOD_t>::type::holder_type bound_call( METHOD_t A_METHOD )
	{ return ( bound_call_calculator<METHOD_t>::type::make( A_METHOD ) ); }

template<typename METHOD_t, typename a0_t>
typename bound_call_calculator<METHOD_t, a0_t>::type::holder_type bound_call( METHOD_t A_METHOD, a0_t a0 )
	{ return ( bound_call_calculator<METHOD_t, a0_t>::type::make( A_METHOD, a0 ) ); }

template<typename METHOD_t, typename a0_t, typename a1_t>
typename bound_call_calculator<METHOD_t, a0_t, a1_t>::type::holder_type bound_call( METHOD_t A_METHOD, a0_t a0, a1_t a1 )
	{ return ( bound_call_calculator<METHOD_t, a0_t, a1_t>::type::make( A_METHOD, a0, a1 ) ); }

template<typename METHOD_t, typename a0_t, typename a1_t, typename a2_t>
typename bound_call_calculator<METHOD_t, a0_t, a1_t, a2_t>::type::holder_type bound_call( METHOD_t A_METHOD, a0_t a0, a1_t a1, a2_t a2 )
	{ return ( bound_call_calculator<METHOD_t, a0_t, a1_t, a2_t>::type::make( A_METHOD, a0, a1, a2 ) ); }

template<typename METHOD_t, typename a0_t, typename a1_t, typename a2_t,
	typename a3_t>
typename bound_call_calculator<METHOD_t, a0_t, a1_t, a2_t, a3_t>::type::holder_type bound_call( METHOD_t A_METHOD, a0_t a0, a1_t a1, a2_t a2, a3_t a3 )
	{ return ( bound_call_calculator<METHOD_t, a0_t, a1_t, a2_t, a3_t>::type::make( A_METHOD, a0, a1, a2, a3 ) ); }

template<typename METHOD_t, typename a0_t, typename a1_t, typename a2_t,
	typename a3_t, typename a4_t>
typename bound_call_calculator<METHOD_t, a0_t, a1_t, a2_t, a3_t, a4_t>::type::holder_type bound_call( METHOD_t A_METHOD, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4 )
	{ return ( bound_call_calculator<METHOD_t, a0_t, a1_t, a2_t, a3_t, a4_t>::type::make( A_METHOD, a0, a1, a2, a3, a4 ) ); }

template<typename METHOD_t, typename a0_t, typename a1_t, typename a2_t,
	typename a3_t, typename a4_t, typename a5_t>
typename bound_call_calculator<METHOD_t,
				 a0_t, a1_t, a2_t, a3_t, a4_t, a5_t>::type::holder_type bound_call(
						 METHOD_t A_METHOD, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5 )
	{ return ( bound_call_calculator<METHOD_t,
			a0_t, a1_t, a2_t, a3_t, a4_t, a5_t>::type::make( A_METHOD, a0, a1, a2, a3, a4, a5 ) ); }

template<typename METHOD_t, typename a0_t, typename a1_t, typename a2_t,
	typename a3_t, typename a4_t, typename a5_t, typename a6_t>
typename bound_call_calculator<METHOD_t,
				 a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t>::type::holder_type bound_call(
						 METHOD_t A_METHOD, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6 )
	{ return ( bound_call_calculator<METHOD_t,
			a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t>::type::make( A_METHOD, a0, a1, a2, a3, a4, a5, a6 ) ); }

template<typename METHOD_t, typename a0_t, typename a1_t, typename a2_t,
	typename a3_t, typename a4_t, typename a5_t, typename a6_t, typename a7_t>
typename bound_call_calculator<METHOD_t,
				 a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t>::type::holder_type bound_call(
						 METHOD_t A_METHOD, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7 )
	{ return ( bound_call_calculator<METHOD_t,
			a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t>::type::make( A_METHOD, a0, a1, a2, a3, a4, a5, a6, a7 ) ); }

template<typename METHOD_t, typename a0_t, typename a1_t, typename a2_t,
	typename a3_t, typename a4_t, typename a5_t, typename a6_t, typename a7_t,
	typename a8_t>
typename bound_call_calculator<METHOD_t,
				 a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t>::type::holder_type bound_call(
						 METHOD_t A_METHOD, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7, a8_t a8 )
	{ return ( bound_call_calculator<METHOD_t,
			a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t>::type::make( A_METHOD, a0, a1, a2, a3, a4, a5, a6, a7, a8 ) ); }

template<typename METHOD_t, typename a0_t, typename a1_t, typename a2_t,
	typename a3_t, typename a4_t, typename a5_t, typename a6_t, typename a7_t,
	typename a8_t, typename a9_t>
typename bound_call_calculator<METHOD_t,
				 a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t>::type::holder_type bound_call(
						 METHOD_t A_METHOD, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7, a8_t a8, a9_t a9 )
	{ return ( bound_call_calculator<METHOD_t,
			a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t>::type::make( A_METHOD, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9 ) ); }

template<typename METHOD_t, typename a0_t, typename a1_t, typename a2_t,
	typename a3_t, typename a4_t, typename a5_t, typename a6_t, typename a7_t,
	typename a8_t, typename a9_t, typename a10_t>
typename bound_call_calculator<METHOD_t,
				 a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t, a10_t>::type::holder_type bound_call(
						 METHOD_t A_METHOD, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7, a8_t a8, a9_t a9, a10_t a10 )
	{ return ( bound_call_calculator<METHOD_t,
			a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t, a10_t>::type::make( A_METHOD, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10 ) ); }

}

}

#endif /* not YAAL_HCORE_HBOUNDCALL_HXX_INCLUDED */

