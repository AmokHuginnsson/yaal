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
/*! \file hcore/bound_call.hxx
 * \brief Declaration and implementation of HBoundCall<> template.
 */

#ifndef YAAL_HCORE_HCALL_HXX_INCLUDED
#define YAAL_HCORE_HCALL_HXX_INCLUDED

#include "hcore/hpointer.hxx"
#include "hcore/trait.hxx"

namespace yaal
{

namespace hcore
{

namespace free_standing_call_args
{

struct arg_base
	{
	typedef trait::no_type free_standing_call_arg_type_t;
	virtual ~arg_base( void ) {}
	};

template<int arg_no>
struct arg : public arg_base
	{
	};

}

namespace
{

static const free_standing_call_args::arg<1> _1;
static const free_standing_call_args::arg<2> _2;
static const free_standing_call_args::arg<3> _3;
static const free_standing_call_args::arg<4> _4;
static const free_standing_call_args::arg<5> _5;
static const free_standing_call_args::arg<6> _6;
static const free_standing_call_args::arg<7> _7;
static const free_standing_call_args::arg<8> _8;
static const free_standing_call_args::arg<9> _9;
static const free_standing_call_args::arg<10> _10;

}

/*! \brief Interface to abstration of any-method of any-class invocation.
 */
class HBoundCallInterface
	{
public:
	typedef yaal::hcore::HPointer<HBoundCallInterface> ptr_t;
	virtual ~HBoundCallInterface( void ) {}
	void operator()( void )
		{ invoke(); }
	void operator()( void ) const
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

/*! \brief Make a functor from any (class, method) pair.
 */
template<typename CLASS_t, typename METHOD_t>
class HFunctor
	{
	CLASS_t _object;
	METHOD_t _method;
	typedef typename trait::return_type<METHOD_t>::type return_t;
public:
	HFunctor( CLASS_t object_, METHOD_t method_ ) : _object( object_ ), _method( method_ ) {}
	template<typename a0_t, typename a1_t, typename a2_t, typename a3_t,
		typename a4_t, typename a5_t, typename a6_t, typename a7_t,
		typename a8_t, typename a9_t>
	return_t operator()(
			a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4,
			a5_t a5, a6_t a6, a7_t a7, a8_t a8, a9_t a9 )
		{ (_object.*_method)( a0, a1, a2, a3, a4, a5, a6, a7, a8, a9 ); }
	template<typename a0_t, typename a1_t, typename a2_t, typename a3_t,
		typename a4_t, typename a5_t, typename a6_t, typename a7_t,
		typename a8_t, typename a9_t>
	return_t operator()(
			a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4,
			a5_t a5, a6_t a6, a7_t a7, a8_t a8, a9_t a9 ) const
		{ (_object.*_method)( a0, a1, a2, a3, a4, a5, a6, a7, a8, a9 ); }
	template<typename a0_t, typename a1_t, typename a2_t, typename a3_t,
		typename a4_t, typename a5_t, typename a6_t, typename a7_t,
		typename a8_t>
	return_t operator()(
			a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4,
			a5_t a5, a6_t a6, a7_t a7, a8_t a8 )
		{ (_object.*_method)( a0, a1, a2, a3, a4, a5, a6, a7, a8 ); }
	template<typename a0_t, typename a1_t, typename a2_t, typename a3_t,
		typename a4_t, typename a5_t, typename a6_t, typename a7_t,
		typename a8_t>
	return_t operator()(
			a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4,
			a5_t a5, a6_t a6, a7_t a7, a8_t a8 ) const
		{ (_object.*_method)( a0, a1, a2, a3, a4, a5, a6, a7, a8 ); }
	template<typename a0_t, typename a1_t, typename a2_t, typename a3_t,
		typename a4_t, typename a5_t, typename a6_t, typename a7_t>
	return_t operator()(
			a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4,
			a5_t a5, a6_t a6, a7_t a7 )
		{ (_object.*_method)( a0, a1, a2, a3, a4, a5, a6, a7 ); }
	template<typename a0_t, typename a1_t, typename a2_t, typename a3_t,
		typename a4_t, typename a5_t, typename a6_t, typename a7_t>
	return_t operator()(
			a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4,
			a5_t a5, a6_t a6, a7_t a7 ) const
		{ (_object.*_method)( a0, a1, a2, a3, a4, a5, a6, a7 ); }
	template<typename a0_t, typename a1_t, typename a2_t, typename a3_t,
		typename a4_t, typename a5_t, typename a6_t>
	return_t operator()(
			a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4,
			a5_t a5, a6_t a6 )
		{ (_object.*_method)( a0, a1, a2, a3, a4, a5, a6 ); }
	template<typename a0_t, typename a1_t, typename a2_t, typename a3_t,
		typename a4_t, typename a5_t, typename a6_t>
	return_t operator()(
			a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4,
			a5_t a5, a6_t a6 ) const
		{ (_object.*_method)( a0, a1, a2, a3, a4, a5, a6 ); }
	template<typename a0_t, typename a1_t, typename a2_t, typename a3_t,
		typename a4_t, typename a5_t>
	return_t operator()(
			a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4,
			a5_t a5 )
		{ (_object.*_method)( a0, a1, a2, a3, a4, a5 ); }
	template<typename a0_t, typename a1_t, typename a2_t, typename a3_t,
		typename a4_t, typename a5_t>
	return_t operator()(
			a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4,
			a5_t a5 ) const
		{ (_object.*_method)( a0, a1, a2, a3, a4, a5 ); }
	template<typename a0_t, typename a1_t, typename a2_t, typename a3_t,
		typename a4_t>
	return_t operator()(
			a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4 )
		{ (_object.*_method)( a0, a1, a2, a3, a4 ); }
	template<typename a0_t, typename a1_t, typename a2_t, typename a3_t,
		typename a4_t>
	return_t operator()(
			a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4 ) const
		{ (_object.*_method)( a0, a1, a2, a3, a4 ); }
	template<typename a0_t, typename a1_t, typename a2_t, typename a3_t>
	return_t operator()( a0_t a0, a1_t a1, a2_t a2, a3_t a3 )
		{ (_object.*_method)( a0, a1, a2, a3 ); }
	template<typename a0_t, typename a1_t, typename a2_t, typename a3_t>
	return_t operator()( a0_t a0, a1_t a1, a2_t a2, a3_t a3 ) const
		{ (_object.*_method)( a0, a1, a2, a3 ); }
	template<typename a0_t, typename a1_t, typename a2_t>
	return_t operator()( a0_t a0, a1_t a1, a2_t a2 )
		{ (_object.*_method)( a0, a1, a2 ); }
	template<typename a0_t, typename a1_t, typename a2_t>
	return_t operator()( a0_t a0, a1_t a1, a2_t a2 ) const
		{ (_object.*_method)( a0, a1, a2 ); }
	template<typename a0_t, typename a1_t>
	return_t operator()( a0_t a0, a1_t a1 )
		{ (_object.*_method)( a0, a1 ); }
	template<typename a0_t, typename a1_t>
	return_t operator()( a0_t a0, a1_t a1 ) const
		{ (_object.*_method)( a0, a1 ); }
	template<typename a0_t>
	return_t operator()( a0_t a0 )
		{ (_object.*_method)( a0 ); }
	template<typename a0_t>
	return_t operator()( a0_t a0 ) const
		{ (_object.*_method)( a0 ); }
	return_t operator()( void )
		{ (_object.*_method)(); }
	return_t operator()( void ) const
		{ (_object.*_method)(); }
	void const* id( void ) const
		{ return ( &_object ); }
	};

template<typename CLASS_t, typename METHOD_t>
void const* caller_id( HFunctor<CLASS_t, METHOD_t> const& functor_ )
	{ return ( functor_.id() ); }

template<typename CLASS_t>
void const* caller_id( CLASS_t const& object_ )
	{ return ( &object_ ); }

/*! \brief Base class for HBoundCall<1..10>.
 */
template<typename CALL_t>
class HBoundCallBase : public HBoundCallInterface
	{
protected:
	CALL_t _call;
public:
	HBoundCallBase( CALL_t call_ ) : _call( call_ ) {}
protected:
	virtual void const* do_id( void ) const
		{ return ( caller_id( _call ) ); }
	};

template<typename CALL_t,
	typename a0_t = trait::no_type,
	typename a1_t = trait::no_type,
	typename a2_t = trait::no_type,
	typename a3_t = trait::no_type,
	typename a4_t = trait::no_type,
	typename a5_t = trait::no_type,
	typename a6_t = trait::no_type,
	typename a7_t = trait::no_type,
	typename a8_t = trait::no_type,
	typename a9_t = trait::no_type>
class HBoundCall;

/*! \cond */
template<typename CALL_t>
class HBoundCall<CALL_t,
	trait::no_type,
	trait::no_type,
	trait::no_type,
	trait::no_type,
	trait::no_type,
	trait::no_type,
	trait::no_type,
	trait::no_type,
	trait::no_type,
	trait::no_type> : public HBoundCallBase<CALL_t>
	{
	typedef HBoundCallBase<CALL_t> base_t;
public:
	HBoundCall( CALL_t call_ )
		: base_t( call_ ) {}
protected:
	virtual void do_invoke( void )
		{ (base_t::_call)(); }
	virtual void do_invoke( void ) const
		{ (base_t::_call)(); }
	};

template<typename CALL_t,
	typename a0_t>
class HBoundCall<CALL_t,
	a0_t,
	trait::no_type,
	trait::no_type,
	trait::no_type,
	trait::no_type,
	trait::no_type,
	trait::no_type,
	trait::no_type,
	trait::no_type,
	trait::no_type> : public HBoundCallBase<CALL_t>
	{
	typedef HBoundCallBase<CALL_t> base_t;
protected:
	a0_t _a0;
public:
	HBoundCall( CALL_t call_, a0_t a0 )
		: base_t( call_ ),
		_a0( a0 ) {}
protected:
	virtual void do_invoke( void )
		{ (base_t::_call)( _a0 ); }
	virtual void do_invoke( void ) const
		{ (base_t::_call)( _a0 ); }
	};

template<typename CALL_t,
	typename a0_t, typename a1_t>
class HBoundCall<CALL_t,
	a0_t, a1_t,
	trait::no_type,
	trait::no_type,
	trait::no_type,
	trait::no_type,
	trait::no_type,
	trait::no_type,
	trait::no_type,
	trait::no_type> : public HBoundCallBase<CALL_t>
	{
	typedef HBoundCallBase<CALL_t> base_t;
protected:
	a0_t _a0;
	a1_t _a1;
public:
	HBoundCall( CALL_t call_, a0_t a0, a1_t a1 )
		: base_t( call_ ),
		_a0( a0 ), _a1( a1 ) {}
protected:
	virtual void do_invoke( void )
		{ (base_t::_call)( _a0, _a1 ); }
	virtual void do_invoke( void ) const
		{ (base_t::_call)( _a0, _a1 ); }
	};

template<typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t>
class HBoundCall<CALL_t,
	a0_t, a1_t, a2_t,
	trait::no_type,
	trait::no_type,
	trait::no_type,
	trait::no_type,
	trait::no_type,
	trait::no_type,
	trait::no_type> : public HBoundCallBase<CALL_t>
	{
	typedef HBoundCallBase<CALL_t> base_t;
protected:
	a0_t _a0;
	a1_t _a1;
	a2_t _a2;
public:
	HBoundCall( CALL_t call_, a0_t a0, a1_t a1,
			a2_t a2 )
		: base_t( call_ ),
		_a0( a0 ), _a1( a1 ), _a2( a2 ) {}
protected:
	virtual void do_invoke( void )
		{ (base_t::_call)( _a0, _a1, _a2 ); }
	virtual void do_invoke( void ) const
		{ (base_t::_call)( _a0, _a1, _a2 ); }
	};

template<typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t>
class HBoundCall<CALL_t,
	a0_t, a1_t, a2_t, a3_t,
	trait::no_type,
	trait::no_type,
	trait::no_type,
	trait::no_type,
	trait::no_type,
	trait::no_type> : public HBoundCallBase<CALL_t>
	{
	typedef HBoundCallBase<CALL_t> base_t;
protected:
	a0_t _a0;
	a1_t _a1;
	a2_t _a2;
	a3_t _a3;
public:
	HBoundCall( CALL_t call_, a0_t a0, a1_t a1,
			a2_t a2, a3_t a3 )
		: base_t( call_ ),
		_a0( a0 ), _a1( a1 ), _a2( a2 ), _a3( a3 ) {}
protected:
	virtual void do_invoke( void )
		{ (base_t::_call)( _a0, _a1, _a2, _a3 ); }
	virtual void do_invoke( void ) const
		{ (base_t::_call)( _a0, _a1, _a2, _a3 ); }
	};

template<typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t>
class HBoundCall<CALL_t,
	a0_t, a1_t, a2_t, a3_t, a4_t,
	trait::no_type,
	trait::no_type,
	trait::no_type,
	trait::no_type,
	trait::no_type> : public HBoundCallBase<CALL_t>
	{
	typedef HBoundCallBase<CALL_t> base_t;
protected:
	a0_t _a0;
	a1_t _a1;
	a2_t _a2;
	a3_t _a3;
	a4_t _a4;
public:
	HBoundCall( CALL_t call_, a0_t a0, a1_t a1,
			a2_t a2, a3_t a3, a4_t a4 )
		: base_t( call_ ),
		_a0( a0 ), _a1( a1 ), _a2( a2 ), _a3( a3 ), _a4( a4 ) {}
protected:
	virtual void do_invoke( void )
		{ (base_t::_call)( _a0, _a1, _a2, _a3, _a4 ); }
	virtual void do_invoke( void ) const
		{ (base_t::_call)( _a0, _a1, _a2, _a3, _a4 ); }
	};

template<typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t>
class HBoundCall<CALL_t,
	a0_t, a1_t, a2_t, a3_t, a4_t, a5_t,
	trait::no_type,
	trait::no_type,
	trait::no_type,
	trait::no_type> : public HBoundCallBase<CALL_t>
	{
	typedef HBoundCallBase<CALL_t> base_t;
protected:
	a0_t _a0;
	a1_t _a1;
	a2_t _a2;
	a3_t _a3;
	a4_t _a4;
	a5_t _a5;
public:
	HBoundCall( CALL_t call_, a0_t a0, a1_t a1,
			a2_t a2, a3_t a3, a4_t a4, a5_t a5 )
		: base_t( call_ ),
		_a0( a0 ), _a1( a1 ), _a2( a2 ), _a3( a3 ), _a4( a4 ),
		_a5( a5 ) {}
protected:
	virtual void do_invoke( void )
		{ (base_t::_call)( _a0, _a1, _a2, _a3, _a4, _a5 ); }
	virtual void do_invoke( void ) const
		{ (base_t::_call)( _a0, _a1, _a2, _a3, _a4, _a5 ); }
	};

template<typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t, typename a6_t>
class HBoundCall<CALL_t,
	a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t,
	trait::no_type,
	trait::no_type,
	trait::no_type> : public HBoundCallBase<CALL_t>
	{
	typedef HBoundCallBase<CALL_t> base_t;
protected:
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
		: base_t( call_ ),
		_a0( a0 ), _a1( a1 ), _a2( a2 ), _a3( a3 ), _a4( a4 ),
		_a5( a5 ), _a6( a6 ) {}
protected:
	virtual void do_invoke( void )
		{ (base_t::_call)( _a0, _a1, _a2, _a3, _a4, _a5, _a6 ); }
	virtual void do_invoke( void ) const
		{ (base_t::_call)( _a0, _a1, _a2, _a3, _a4, _a5, _a6 ); }
	};

template<typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t, typename a6_t, typename a7_t>
class HBoundCall<CALL_t,
	a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t,
	trait::no_type,
	trait::no_type> : public HBoundCallBase<CALL_t>
	{
	typedef HBoundCallBase<CALL_t> base_t;
protected:
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
		: base_t( call_ ),
		_a0( a0 ), _a1( a1 ), _a2( a2 ), _a3( a3 ), _a4( a4 ),
		_a5( a5 ), _a6( a6 ), _a7( a7 ) {}
protected:
	virtual void do_invoke( void )
		{ (base_t::_call)( _a0, _a1, _a2, _a3, _a4, _a5, _a6, _a7 ); }
	virtual void do_invoke( void ) const
		{ (base_t::_call)( _a0, _a1, _a2, _a3, _a4, _a5, _a6, _a7 ); }
	};

template<typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t, typename a6_t, typename a7_t,
	typename a8_t>
class HBoundCall<CALL_t,
	a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t,
	trait::no_type> : public HBoundCallBase<CALL_t>
	{
	typedef HBoundCallBase<CALL_t> base_t;
protected:
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
	HBoundCall( CALL_t call_, a0_t a0, a1_t a1,
			a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7,
			a8_t a8 )
		: base_t( call_ ),
		_a0( a0 ), _a1( a1 ), _a2( a2 ), _a3( a3 ), _a4( a4 ),
		_a5( a5 ), _a6( a6 ), _a7( a7 ), _a8( a8 ) {}
protected:
	virtual void do_invoke( void )
		{ (base_t::_call)( _a0, _a1, _a2, _a3, _a4, _a5, _a6, _a7, _a8 ); }
	virtual void do_invoke( void ) const
		{ (base_t::_call)( _a0, _a1, _a2, _a3, _a4, _a5, _a6, _a7, _a8 ); }
	};
/*! \endcond */

/*! \brief Implementation of abstration of any-method of any-class invocation.
 *
 * \tparam CALL_t - class on which this invocation will operate.
 * \tparam METHOD_t - method of given class CALL_t that shall be invoked.
 * \tparam a0_t - first argument to given METHOD_t of CALL_t.
 * \tparam a1_t - second argument to given METHOD_t of CALL_t.
 * \tparam aN_t - N-th argument to given METHOD_t of CALL_t.
 */
template<typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t, typename a6_t, typename a7_t,
	typename a8_t, typename a9_t>
class HBoundCall : public HBoundCallBase<CALL_t>
	{
	typedef HBoundCallBase<CALL_t> base_t;
protected:
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
	HBoundCall( CALL_t call_, a0_t a0, a1_t a1,
			a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7,
			a8_t a8, a9_t a9 )
		: base_t( call_ ),
		_a0( a0 ), _a1( a1 ), _a2( a2 ), _a3( a3 ), _a4( a4 ),
		_a5( a5 ), _a6( a6 ), _a7( a7 ), _a8( a8 ), _a9( a9 ) {}
protected:
	virtual void do_invoke( void )
		{ (base_t::_call)( _a0, _a1, _a2, _a3, _a4, _a5, _a6, _a7, _a8, _a9 ); }
	virtual void do_invoke( void ) const
		{ (base_t::_call)( _a0, _a1, _a2, _a3, _a4, _a5, _a6, _a7, _a8, _a9 ); }
	};

template<typename CLASS_t, typename METHOD_t>
HBoundCallInterface::ptr_t bound_call( CLASS_t obj, METHOD_t A_METHOD )
	{ return ( HBoundCallInterface::ptr_t(
				new HBoundCall<HFunctor<CLASS_t, METHOD_t> >(
					HFunctor<CLASS_t, METHOD_t>( obj, A_METHOD ) ) ) ); }

template<typename CLASS_t, typename METHOD_t, typename a0_t>
HBoundCallInterface::ptr_t bound_call( CLASS_t obj, METHOD_t A_METHOD,
		a0_t a0 )
	{ return ( HBoundCallInterface::ptr_t(
				new HBoundCall<HFunctor<CLASS_t, METHOD_t>, a0_t>(
					HFunctor<CLASS_t, METHOD_t>( obj, A_METHOD ), a0 ) ) ); }

template<typename CLASS_t, typename METHOD_t, typename a0_t,
	typename a1_t>
HBoundCallInterface::ptr_t bound_call( CLASS_t obj, METHOD_t A_METHOD,
		a0_t a0, a1_t a1 )
	{ return ( HBoundCallInterface::ptr_t(
				new HBoundCall<HFunctor<CLASS_t, METHOD_t>, a0_t, a1_t>(
					HFunctor<CLASS_t, METHOD_t>( obj, A_METHOD ), a0, a1 ) ) ); }

template<typename CLASS_t, typename METHOD_t, typename a0_t,
	typename a1_t,
	typename a2_t>
HBoundCallInterface::ptr_t bound_call( CLASS_t obj, METHOD_t A_METHOD,
		a0_t a0, a1_t a1, a2_t a2 )
	{ return ( HBoundCallInterface::ptr_t(
				new HBoundCall<HFunctor<CLASS_t, METHOD_t>, a0_t, a1_t, a2_t>(
					HFunctor<CLASS_t, METHOD_t>( obj, A_METHOD ), a0, a1, a2 ) ) ); }

template<typename CLASS_t, typename METHOD_t, typename a0_t,
	typename a1_t, typename a2_t, typename a3_t>
HBoundCallInterface::ptr_t bound_call( CLASS_t obj, METHOD_t A_METHOD,
		a0_t a0, a1_t a1, a2_t a2, a3_t a3 )
	{ return ( HBoundCallInterface::ptr_t(
				new HBoundCall<HFunctor<CLASS_t, METHOD_t>, a0_t, a1_t, a2_t, a3_t>(
					HFunctor<CLASS_t, METHOD_t>( obj, A_METHOD ), a0, a1, a2, a3 ) ) ); }

template<typename CLASS_t, typename METHOD_t, typename a0_t,
	typename a1_t, typename a2_t, typename a3_t, typename a4_t>
HBoundCallInterface::ptr_t bound_call( CLASS_t obj, METHOD_t A_METHOD,
		a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4 )
	{ return ( HBoundCallInterface::ptr_t(
				new HBoundCall<HFunctor<CLASS_t, METHOD_t>, a0_t, a1_t, a2_t, a3_t, a4_t>(
					HFunctor<CLASS_t, METHOD_t>( obj, A_METHOD ), a0, a1, a2, a3, a4 ) ) ); }

template<typename CLASS_t, typename METHOD_t, typename a0_t, typename a1_t,
	typename a2_t, typename a3_t, typename a4_t, typename a5_t>
HBoundCallInterface::ptr_t bound_call( CLASS_t obj, METHOD_t A_METHOD,
		a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5 )
	{ return ( HBoundCallInterface::ptr_t(
				new HBoundCall<HFunctor<CLASS_t, METHOD_t>, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t>(
					HFunctor<CLASS_t, METHOD_t>( obj, A_METHOD ), a0, a1, a2, a3, a4, a5 ) ) ); }

template<typename CLASS_t, typename METHOD_t, typename a0_t, typename a1_t,
	typename a2_t, typename a3_t, typename a4_t, typename a5_t, typename a6_t>
HBoundCallInterface::ptr_t bound_call( CLASS_t obj, METHOD_t A_METHOD,
		a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6 )
	{ return ( HBoundCallInterface::ptr_t(
				new HBoundCall<HFunctor<CLASS_t, METHOD_t>, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t>(
					HFunctor<CLASS_t, METHOD_t>( obj, A_METHOD ), a0, a1, a2, a3, a4, a5, a6 ) ) ); }

template<typename CLASS_t, typename METHOD_t, typename a0_t, typename a1_t,
	typename a2_t, typename a3_t, typename a4_t, typename a5_t, typename a6_t,
	typename a7_t>
HBoundCallInterface::ptr_t bound_call( CLASS_t obj, METHOD_t A_METHOD,
		a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7 )
	{ return ( HBoundCallInterface::ptr_t(
				new HBoundCall<HFunctor<CLASS_t, METHOD_t>, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t>(
					HFunctor<CLASS_t, METHOD_t>( obj, A_METHOD ), a0, a1, a2, a3, a4, a5, a6, a7 ) ) ); }

template<typename CLASS_t, typename METHOD_t, typename a0_t, typename a1_t,
	typename a2_t, typename a3_t, typename a4_t, typename a5_t, typename a6_t,
	typename a7_t, typename a8_t>
HBoundCallInterface::ptr_t bound_call( CLASS_t obj, METHOD_t A_METHOD,
		a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7, a8_t a8 )
	{ return ( HBoundCallInterface::ptr_t(
				new HBoundCall<HFunctor<CLASS_t, METHOD_t>, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t>(
					HFunctor<CLASS_t, METHOD_t>( obj, A_METHOD ), a0, a1, a2, a3, a4, a5, a6, a7, a8 ) ) ); }

template<typename CLASS_t, typename METHOD_t, typename a0_t, typename a1_t,
	typename a2_t, typename a3_t, typename a4_t, typename a5_t, typename a6_t,
	typename a7_t, typename a8_t, typename a9_t>
HBoundCallInterface::ptr_t bound_call( CLASS_t obj, METHOD_t A_METHOD,
		a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7, a8_t a8, a9_t a9 )
	{ return ( HBoundCallInterface::ptr_t(
				new HBoundCall<HFunctor<CLASS_t, METHOD_t>, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t>(
					HFunctor<CLASS_t, METHOD_t>( obj, A_METHOD ), a0, a1, a2, a3, a4, a5, a6, a7, a8, a9 ) ) ); }

template<typename return_t, typename CALL_t,
	typename a0_t = trait::no_type,
	typename a1_t = trait::no_type,
	typename a2_t = trait::no_type,
	typename a3_t = trait::no_type,
	typename a4_t = trait::no_type,
	typename a5_t = trait::no_type,
	typename a6_t = trait::no_type,
	typename a7_t = trait::no_type,
	typename a8_t = trait::no_type,
	typename a9_t = trait::no_type>
class HCall;

template<typename METHOD_t,
	typename a0_t = trait::no_type, typename a1_t = trait::no_type,
	typename a2_t = trait::no_type, typename a3_t = trait::no_type,
	typename a4_t = trait::no_type, typename a5_t = trait::no_type,
	typename a6_t = trait::no_type, typename a7_t = trait::no_type,
	typename a8_t = trait::no_type, typename a9_t = trait::no_type, typename a10_t = trait::no_type>
struct call_calculator
	{
	struct free_standing_args
		{
		static int const value =
			ternary<trait::is_kind_of<typename trait::ternary<trait::is_member<METHOD_t>::value,a10_t,a0_t>::type, free_standing_call_args::arg_base>::value, 1, 0>::value
			+ ternary<trait::is_kind_of<a1_t, free_standing_call_args::arg_base>::value, 1, 0>::value
			+ ternary<trait::is_kind_of<a2_t, free_standing_call_args::arg_base>::value, 1, 0>::value
			+ ternary<trait::is_kind_of<a3_t, free_standing_call_args::arg_base>::value, 1, 0>::value
			+ ternary<trait::is_kind_of<a4_t, free_standing_call_args::arg_base>::value, 1, 0>::value
			+ ternary<trait::is_kind_of<a5_t, free_standing_call_args::arg_base>::value, 1, 0>::value
			+ ternary<trait::is_kind_of<a6_t, free_standing_call_args::arg_base>::value, 1, 0>::value
			+ ternary<trait::is_kind_of<a7_t, free_standing_call_args::arg_base>::value, 1, 0>::value
			+ ternary<trait::is_kind_of<a8_t, free_standing_call_args::arg_base>::value, 1, 0>::value
			+ ternary<trait::is_kind_of<a9_t, free_standing_call_args::arg_base>::value, 1, 0>::value;
		};
	typedef typename trait::ternary<trait::is_member<METHOD_t>::value,
					HCall<typename trait::return_type<METHOD_t>::type, HFunctor<a0_t, METHOD_t>, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t, a10_t>,
					HCall<typename trait::return_type<METHOD_t>::type, METHOD_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t> >::type type;
	};

/*! \cond */
template<typename return_t, typename CALL_t>
class HCall<return_t, CALL_t,
	trait::no_type,
	trait::no_type,
	trait::no_type,
	trait::no_type,
	trait::no_type,
	trait::no_type,
	trait::no_type,
	trait::no_type,
	trait::no_type,
	trait::no_type>
	{
	CALL_t _call;
public:
	HCall( CALL_t call_ )
		: _call( call_ ) {}
	return_t operator()( void )
		{ return ( _call() ); }
	return_t operator()( void ) const
		{ return ( _call() ); }
	};

template<typename return_t, typename CALL_t,
	typename a0_t>
class HCall<return_t, CALL_t,
	a0_t,
	trait::no_type,
	trait::no_type,
	trait::no_type,
	trait::no_type,
	trait::no_type,
	trait::no_type,
	trait::no_type,
	trait::no_type,
	trait::no_type>
	{
	CALL_t _call;
	a0_t _a0;
public:
	HCall( CALL_t call_, a0_t a0 )
		: _call( call_ ),
		_a0( a0 ) {}
	return_t operator()( void )
		{ return ( _call( _a0 ) ); }
	return_t operator()( void ) const
		{ return ( _call( _a0 ) ); }
	};

template<typename return_t, typename CALL_t,
	typename a0_t, typename a1_t>
class HCall<return_t, CALL_t,
	a0_t, a1_t,
	trait::no_type,
	trait::no_type,
	trait::no_type,
	trait::no_type,
	trait::no_type,
	trait::no_type,
	trait::no_type,
	trait::no_type>
	{
protected:
	CALL_t _call;
	a0_t _a0;
	a1_t _a1;
public:
	HCall( CALL_t call_, a0_t a0, a1_t a1 )
		: _call( call_ ),
		_a0( a0 ), _a1( a1 ) {}
	return_t operator()( void )
		{ return ( _call( _a0, _a1 ) ); }
	return_t operator()( void ) const
		{ return ( _call( _a0, _a1 ) ); }
	};

template<typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t>
class HCall<return_t, CALL_t,
	a0_t, a1_t, a2_t,
	trait::no_type,
	trait::no_type,
	trait::no_type,
	trait::no_type,
	trait::no_type,
	trait::no_type,
	trait::no_type>
	{
	CALL_t _call;
	a0_t _a0;
	a1_t _a1;
	a2_t _a2;
public:
	HCall( CALL_t call_, a0_t a0, a1_t a1,
			a2_t a2 )
		: _call( call_ ),
		_a0( a0 ), _a1( a1 ), _a2( a2 ) {}
	return_t operator()( void )
		{ return ( _call( _a0, _a1, _a2 ) ); }
	return_t operator()( void ) const
		{ return ( _call( _a0, _a1, _a2 ) ); }
	};

template<typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t>
class HCall<return_t, CALL_t,
	a0_t, a1_t, a2_t, a3_t,
	trait::no_type,
	trait::no_type,
	trait::no_type,
	trait::no_type,
	trait::no_type,
	trait::no_type>
	{
	CALL_t _call;
	a0_t _a0;
	a1_t _a1;
	a2_t _a2;
	a3_t _a3;
public:
	HCall( CALL_t call_, a0_t a0, a1_t a1,
			a2_t a2, a3_t a3 )
		: _call( call_ ),
		_a0( a0 ), _a1( a1 ), _a2( a2 ), _a3( a3 ) {}
	return_t operator()( void )
		{ return ( _call( _a0, _a1, _a2, _a3 ) ); }
	return_t operator()( void ) const
		{ return ( _call( _a0, _a1, _a2, _a3 ) ); }
	};

template<typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t>
class HCall<return_t, CALL_t,
	a0_t, a1_t, a2_t, a3_t, a4_t,
	trait::no_type,
	trait::no_type,
	trait::no_type,
	trait::no_type,
	trait::no_type>
	{
	CALL_t _call;
	a0_t _a0;
	a1_t _a1;
	a2_t _a2;
	a3_t _a3;
	a4_t _a4;
public:
	HCall( CALL_t call_, a0_t a0, a1_t a1,
			a2_t a2, a3_t a3, a4_t a4 )
		: _call( call_ ),
		_a0( a0 ), _a1( a1 ), _a2( a2 ), _a3( a3 ), _a4( a4 ) {}
	return_t operator()( void )
		{ return ( _call( _a0, _a1, _a2, _a3, _a4 ) ); }
	return_t operator()( void ) const
		{ return ( _call( _a0, _a1, _a2, _a3, _a4 ) ); }
	};

template<typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t>
class HCall<return_t, CALL_t,
	a0_t, a1_t, a2_t, a3_t, a4_t, a5_t,
	trait::no_type,
	trait::no_type,
	trait::no_type,
	trait::no_type>
	{
	CALL_t _call;
	a0_t _a0;
	a1_t _a1;
	a2_t _a2;
	a3_t _a3;
	a4_t _a4;
	a5_t _a5;
public:
	HCall( CALL_t call_, a0_t a0, a1_t a1,
			a2_t a2, a3_t a3, a4_t a4, a5_t a5 )
		: _call( call_ ),
		_a0( a0 ), _a1( a1 ), _a2( a2 ), _a3( a3 ), _a4( a4 ),
		_a5( a5 ) {}
	return_t operator()( void )
		{ return ( _call( _a0, _a1, _a2, _a3, _a4, _a5 ) ); }
	return_t operator()( void ) const
		{ return ( _call( _a0, _a1, _a2, _a3, _a4, _a5 ) ); }
	};

template<typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t, typename a6_t>
class HCall<return_t, CALL_t,
	a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t,
	trait::no_type,
	trait::no_type,
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
public:
	HCall( CALL_t call_, a0_t a0, a1_t a1,
			a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6 )
		: _call( call_ ),
		_a0( a0 ), _a1( a1 ), _a2( a2 ), _a3( a3 ), _a4( a4 ),
		_a5( a5 ), _a6( a6 ) {}
	return_t operator()( void )
		{ return ( _call( _a0, _a1, _a2, _a3, _a4, _a5, _a6 ) ); }
	return_t operator()( void ) const
		{ return ( _call( _a0, _a1, _a2, _a3, _a4, _a5, _a6 ) ); }
	};

template<typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t, typename a6_t, typename a7_t>
class HCall<return_t, CALL_t,
	a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t,
	trait::no_type,
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
public:
	HCall( CALL_t call_, a0_t a0, a1_t a1,
			a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7 )
		: _call( call_ ),
		_a0( a0 ), _a1( a1 ), _a2( a2 ), _a3( a3 ), _a4( a4 ),
		_a5( a5 ), _a6( a6 ), _a7( a7 ) {}
	return_t operator()( void )
		{ return ( _call( _a0, _a1, _a2, _a3, _a4, _a5, _a6, _a7 ) ); }
	return_t operator()( void ) const
		{ return ( _call( _a0, _a1, _a2, _a3, _a4, _a5, _a6, _a7 ) ); }
	};

template<typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t, typename a6_t, typename a7_t,
	typename a8_t>
class HCall<return_t, CALL_t,
	a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t,
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
	HCall( CALL_t call_, a0_t a0, a1_t a1,
			a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7,
			a8_t a8 )
		: _call( call_ ),
		_a0( a0 ), _a1( a1 ), _a2( a2 ), _a3( a3 ), _a4( a4 ),
		_a5( a5 ), _a6( a6 ), _a7( a7 ), _a8( a8 ) {}
	return_t operator()( void )
		{ return ( _call( _a0, _a1, _a2, _a3, _a4, _a5, _a6, _a7, _a8 ) ); }
	return_t operator()( void ) const
		{ return ( _call( _a0, _a1, _a2, _a3, _a4, _a5, _a6, _a7, _a8 ) ); }
	};
/*! \endcond */

/*! \brief Implementation of abstration of any-method of any-class invocation.
 *
 * \tparam CALL_t - class on which this invocation will operate.
 * \tparam METHOD_t - method of given class CALL_t that shall be invoked.
 * \tparam a0_t - first argument to given METHOD_t of CALL_t.
 * \tparam a1_t - second argument to given METHOD_t of CALL_t.
 * \tparam aN_t - N-th argument to given METHOD_t of CALL_t.
 */
template<typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t, typename a6_t, typename a7_t,
	typename a8_t, typename a9_t>
class HCall
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
	HCall( CALL_t call_, a0_t a0, a1_t a1,
			a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7,
			a8_t a8, a9_t a9 )
		: _call( call_ ),
		_a0( a0 ), _a1( a1 ), _a2( a2 ), _a3( a3 ), _a4( a4 ),
		_a5( a5 ), _a6( a6 ), _a7( a7 ), _a8( a8 ), _a9( a9 ) {}
	return_t operator()( void )
		{ return ( _call( _a0, _a1, _a2, _a3, _a4, _a5, _a6, _a7, _a8, _a9 ) ); }
	return_t operator()( void ) const
		{ return ( _call( _a0, _a1, _a2, _a3, _a4, _a5, _a6, _a7, _a8, _a9 ) ); }
	};

template<typename METHOD_t>
typename call_calculator<METHOD_t>::type call( METHOD_t A_METHOD )
	{ return ( typename call_calculator<METHOD_t>::type( A_METHOD ) ); }

template<typename METHOD_t, typename a0_t>
typename call_calculator<METHOD_t, a0_t>::type call( METHOD_t A_METHOD, a0_t a0 )
	{ return ( typename call_calculator<METHOD_t, a0_t>::type( A_METHOD, a0 ) ); }

}

}

#endif /* not YAAL_HCORE_HCALL_HXX_INCLUDED */

