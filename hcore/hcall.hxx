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
/*! \file hcore/hcall.hxx
 * \brief Declaration and implementation of HCall<> template.
 */

#ifndef YAAL_HCORE_HCALL_HXX_INCLUDED
#define YAAL_HCORE_HCALL_HXX_INCLUDED

#include "hcore/hfunctor.hxx"
#include "hcore/functional.hxx"

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
template<int free_args = 0, typename return_t = void,
	typename a0_t = trait::no_type, typename a1_t = trait::no_type,
	typename a2_t = trait::no_type, typename a3_t = trait::no_type,
	typename a4_t = trait::no_type, typename a5_t = trait::no_type,
	typename a6_t = trait::no_type, typename a7_t = trait::no_type,
	typename a8_t = trait::no_type, typename a9_t = trait::no_type,
	typename a10_t = trait::no_type>
class HCallInterface;

/*! \brief Interface to abstration of any-method of any-class invocation.
 */
template<typename return_t>
class HCallInterface<0, return_t,
	trait::no_type, trait::no_type, trait::no_type, trait::no_type,
	trait::no_type, trait::no_type, trait::no_type, trait::no_type,
	trait::no_type, trait::no_type, trait::no_type>
	{
public:
	virtual ~HCallInterface( void ) {}
	return_t operator()( void )
		{ return ( do_invoke() ); }
	return_t operator()( void ) const
		{ return ( do_invoke() ); }
	void const* id( void ) const
		{ return ( do_id() ); }
protected:
	virtual return_t do_invoke( void ) = 0;
	virtual return_t do_invoke( void ) const = 0;
	virtual void const* do_id( void ) const = 0;
	};

template<typename return_t, typename a0_t>
class HCallInterface<1, return_t, a0_t,
	trait::no_type, trait::no_type, trait::no_type, trait::no_type,
	trait::no_type, trait::no_type, trait::no_type, trait::no_type,
	trait::no_type, trait::no_type> : public unary_function<return_t, a0_t>
	{
public:
	typedef unary_function<return_t, a0_t> base_type;
	typedef typename base_type::result_type result_type;
	typedef typename base_type::argument_type argument_type;
	virtual ~HCallInterface( void ) {}
	return_t operator()( a0_t a0 )
		{ return ( do_invoke( a0 ) ); }
	return_t operator()( a0_t a0 ) const
		{ return ( do_invoke( a0 ) ); }
	void const* id( void ) const
		{ return ( do_id() ); }
protected:
	virtual return_t do_invoke( a0_t ) = 0;
	virtual return_t do_invoke( a0_t ) const = 0;
	virtual void const* do_id( void ) const = 0;
	};

template<typename return_t, typename a0_t, typename a1_t>
class HCallInterface<2, return_t, a0_t, a1_t,
	trait::no_type, trait::no_type, trait::no_type,
	trait::no_type, trait::no_type, trait::no_type, trait::no_type,
	trait::no_type, trait::no_type>
		: public binary_function<return_t, a0_t, a1_t>
	{
public:
	typedef binary_function<return_t, a0_t, a1_t> base_type;
	typedef typename base_type::result_type result_type;
	typedef typename base_type::first_argument_type first_argument_type;
	typedef typename base_type::second_argument_type second_argument_type;
	virtual ~HCallInterface( void ) {}
	return_t operator()( a0_t a0, a1_t a1 )
		{ return ( do_invoke( a0, a1 ) ); }
	return_t operator()( a0_t a0, a1_t a1 ) const
		{ return ( do_invoke( a0, a1 ) ); }
	void const* id( void ) const
		{ return ( do_id() ); }
protected:
	virtual return_t do_invoke( a0_t, a1_t ) = 0;
	virtual return_t do_invoke( a0_t, a1_t ) const = 0;
	virtual void const* do_id( void ) const = 0;
	};

template<typename return_t, typename a0_t, typename a1_t, typename a2_t>
class HCallInterface<3, return_t, a0_t, a1_t,
	a2_t, trait::no_type, trait::no_type,
	trait::no_type, trait::no_type, trait::no_type, trait::no_type,
	trait::no_type, trait::no_type>
	{
public:
	virtual ~HCallInterface( void ) {}
	return_t operator()( a0_t a0, a1_t a1, a2_t a2 )
		{ return ( do_invoke( a0, a1, a2 ) ); }
	return_t operator()( a0_t a0, a1_t a1, a2_t a2 ) const
		{ return ( do_invoke( a0, a1, a2 ) ); }
	void const* id( void ) const
		{ return ( do_id() ); }
protected:
	virtual return_t do_invoke( a0_t, a1_t, a2_t ) = 0;
	virtual return_t do_invoke( a0_t, a1_t, a2_t ) const = 0;
	virtual void const* do_id( void ) const = 0;
	};

template<typename return_t, typename a0_t, typename a1_t, typename a2_t, typename a3_t>
class HCallInterface<4, return_t, a0_t, a1_t,
	a2_t, a3_t, trait::no_type,
	trait::no_type, trait::no_type, trait::no_type, trait::no_type,
	trait::no_type, trait::no_type>
	{
public:
	virtual ~HCallInterface( void ) {}
	return_t operator()( a0_t a0, a1_t a1, a2_t a2, a3_t a3 )
		{ return ( do_invoke( a0, a1, a2, a3 ) ); }
	return_t operator()( a0_t a0, a1_t a1, a2_t a2, a3_t a3 ) const
		{ return ( do_invoke( a0, a1, a2, a3 ) ); }
	void const* id( void ) const
		{ return ( do_id() ); }
protected:
	virtual return_t do_invoke( a0_t, a1_t, a2_t, a3_t ) = 0;
	virtual return_t do_invoke( a0_t, a1_t, a2_t, a3_t ) const = 0;
	virtual void const* do_id( void ) const = 0;
	};

template<typename return_t, typename a0_t, typename a1_t, typename a2_t, typename a3_t, typename a4_t>
class HCallInterface<5, return_t, a0_t, a1_t,
	a2_t, a3_t, a4_t,
	trait::no_type, trait::no_type, trait::no_type, trait::no_type,
	trait::no_type, trait::no_type>
	{
public:
	virtual ~HCallInterface( void ) {}
	return_t operator()( a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4 )
		{ return ( do_invoke( a0, a1, a2, a3, a4 ) ); }
	return_t operator()( a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4 ) const
		{ return ( do_invoke( a0, a1, a2, a3, a4 ) ); }
	void const* id( void ) const
		{ return ( do_id() ); }
protected:
	virtual return_t do_invoke( a0_t, a1_t, a2_t, a3_t, a4_t ) = 0;
	virtual return_t do_invoke( a0_t, a1_t, a2_t, a3_t, a4_t ) const = 0;
	virtual void const* do_id( void ) const = 0;
	};

template<typename return_t, typename a0_t, typename a1_t, typename a2_t, typename a3_t, typename a4_t, typename a5_t>
class HCallInterface<6, return_t, a0_t, a1_t,
	a2_t, a3_t, a4_t,
	a5_t, trait::no_type, trait::no_type, trait::no_type,
	trait::no_type, trait::no_type>
	{
public:
	virtual ~HCallInterface( void ) {}
	return_t operator()( a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5 )
		{ return ( do_invoke( a0, a1, a2, a3, a4, a5 ) ); }
	return_t operator()( a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5 ) const
		{ return ( do_invoke( a0, a1, a2, a3, a4, a5 ) ); }
	void const* id( void ) const
		{ return ( do_id() ); }
protected:
	virtual return_t do_invoke( a0_t, a1_t, a2_t, a3_t, a4_t, a5_t ) = 0;
	virtual return_t do_invoke( a0_t, a1_t, a2_t, a3_t, a4_t, a5_t ) const = 0;
	virtual void const* do_id( void ) const = 0;
	};

template<typename return_t, typename a0_t, typename a1_t, typename a2_t, typename a3_t, typename a4_t, typename a5_t, typename a6_t>
class HCallInterface<7, return_t, a0_t, a1_t,
	a2_t, a3_t, a4_t,
	a5_t, a6_t, trait::no_type, trait::no_type,
	trait::no_type, trait::no_type>
	{
public:
	virtual ~HCallInterface( void ) {}
	return_t operator()( a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6 )
		{ return ( do_invoke( a0, a1, a2, a3, a4, a5, a6 ) ); }
	return_t operator()( a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6 ) const
		{ return ( do_invoke( a0, a1, a2, a3, a4, a5, a6 ) ); }
	void const* id( void ) const
		{ return ( do_id() ); }
protected:
	virtual return_t do_invoke( a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t ) = 0;
	virtual return_t do_invoke( a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t ) const = 0;
	virtual void const* do_id( void ) const = 0;
	};

template<typename return_t, typename a0_t, typename a1_t, typename a2_t, typename a3_t, typename a4_t, typename a5_t, typename a6_t, typename a7_t>
class HCallInterface<8, return_t, a0_t, a1_t,
	a2_t, a3_t, a4_t,
	a5_t, a6_t, a7_t, trait::no_type,
	trait::no_type, trait::no_type>
	{
public:
	virtual ~HCallInterface( void ) {}
	return_t operator()( a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7 )
		{ return ( do_invoke( a0, a1, a2, a3, a4, a5, a6, a7 ) ); }
	return_t operator()( a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7 ) const
		{ return ( do_invoke( a0, a1, a2, a3, a4, a5, a6, a7 ) ); }
	void const* id( void ) const
		{ return ( do_id() ); }
protected:
	virtual return_t do_invoke( a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t ) = 0;
	virtual return_t do_invoke( a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t ) const = 0;
	virtual void const* do_id( void ) const = 0;
	};

template<typename return_t, typename a0_t, typename a1_t, typename a2_t, typename a3_t, typename a4_t, typename a5_t, typename a6_t, typename a7_t, typename a8_t>
class HCallInterface<9, return_t, a0_t, a1_t,
	a2_t, a3_t, a4_t,
	a5_t, a6_t, a7_t, a8_t,
	trait::no_type, trait::no_type>
	{
public:
	virtual ~HCallInterface( void ) {}
	return_t operator()( a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7, a8_t a8 )
		{ return ( do_invoke( a0, a1, a2, a3, a4, a5, a6, a7, a8 ) ); }
	return_t operator()( a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7, a8_t a8 ) const
		{ return ( do_invoke( a0, a1, a2, a3, a4, a5, a6, a7, a8 ) ); }
	void const* id( void ) const
		{ return ( do_id() ); }
protected:
	virtual return_t do_invoke( a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t ) = 0;
	virtual return_t do_invoke( a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t ) const = 0;
	virtual void const* do_id( void ) const = 0;
	};

template<typename return_t, typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t, typename a6_t, typename a7_t, typename a8_t, typename a9_t>
class HCallInterface<10, return_t, a0_t, a1_t,
	a2_t, a3_t, a4_t,
	a5_t, a6_t, a7_t, a8_t,
	a9_t, trait::no_type>
	{
public:
	virtual ~HCallInterface( void ) {}
	return_t operator()( a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7, a8_t a8, a9_t a9 )
		{ return ( do_invoke( a0, a1, a2, a3, a4, a5, a6, a7, a8, a9 ) ); }
	return_t operator()( a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7, a8_t a8, a9_t a9 ) const
		{ return ( do_invoke( a0, a1, a2, a3, a4, a5, a6, a7, a8, a9 ) ); }
	void const* id( void ) const
		{ return ( do_id() ); }
protected:
	virtual return_t do_invoke( a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t ) = 0;
	virtual return_t do_invoke( a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t ) const = 0;
	virtual void const* do_id( void ) const = 0;
	};

template<typename return_t, typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t, typename a6_t, typename a7_t, typename a8_t, typename a9_t, typename a10_t>
class HCallInterface<11, return_t, a0_t, a1_t,
	a2_t, a3_t, a4_t,
	a5_t, a6_t, a7_t, a8_t,
	a9_t, a10_t>
	{
public:
	virtual ~HCallInterface( void ) {}
	return_t operator()( a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7, a8_t a8, a9_t a9, a10_t a10 )
		{ return ( do_invoke( a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10 ) ); }
	return_t operator()( a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7, a8_t a8, a9_t a9, a10_t a10 ) const
		{ return ( do_invoke( a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10 ) ); }
	void const* id( void ) const
		{ return ( do_id() ); }
protected:
	virtual return_t do_invoke( a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t, a10_t ) = 0;
	virtual return_t do_invoke( a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t, a10_t ) const = 0;
	virtual void const* do_id( void ) const = 0;
	};

template<typename CLASS_t, typename METHOD_t>
void const* caller_id( HFunctor<CLASS_t, METHOD_t> const& functor_ )
	{ return ( functor_.id() ); }

template<typename CLASS_t>
void const* caller_id( CLASS_t const& object_ )
	{ return ( &object_ ); }

/*! \brief Implementation of abstration of any-method of any-class invocation.
 *
 * \tparam CALL_t - class on which this invocation will operate.
 * \tparam METHOD_t - method of given class CALL_t that shall be invoked.
 * \tparam a0_t - first argument to given METHOD_t of CALL_t.
 * \tparam a1_t - second argument to given METHOD_t of CALL_t.
 * \tparam aN_t - N-th argument to given METHOD_t of CALL_t.
 */
template<int free_args, typename descriptor_t, typename return_t, typename CALL_t,
	typename a0_t = trait::no_type, typename a1_t = trait::no_type,
	typename a2_t = trait::no_type, typename a3_t = trait::no_type,
	typename a4_t = trait::no_type, typename a5_t = trait::no_type,
	typename a6_t = trait::no_type, typename a7_t = trait::no_type,
	typename a8_t = trait::no_type, typename a9_t = trait::no_type,
	typename a10_t = trait::no_type>
class HCall;

template<typename METHOD_t,
	typename fa0_t = trait::no_type, typename fa1_t = trait::no_type,
	typename fa2_t = trait::no_type, typename fa3_t = trait::no_type,
	typename fa4_t = trait::no_type, typename fa5_t = trait::no_type,
	typename fa6_t = trait::no_type, typename fa7_t = trait::no_type,
	typename fa8_t = trait::no_type, typename fa9_t = trait::no_type, typename fa10_t = trait::no_type>
struct call_calculator
	{
	typedef call_calculator<METHOD_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t, fa9_t, fa10_t> descriptor;
	struct free_standing_args_count
		{
		static int const value =
			+ meta::ternary<trait::is_kind_of<fa0_t, free_standing_call_args::arg_base>::value, 1, 0>::value
			+ meta::ternary<trait::is_kind_of<fa1_t, free_standing_call_args::arg_base>::value, 1, 0>::value
			+ meta::ternary<trait::is_kind_of<fa2_t, free_standing_call_args::arg_base>::value, 1, 0>::value
			+ meta::ternary<trait::is_kind_of<fa3_t, free_standing_call_args::arg_base>::value, 1, 0>::value
			+ meta::ternary<trait::is_kind_of<fa4_t, free_standing_call_args::arg_base>::value, 1, 0>::value
			+ meta::ternary<trait::is_kind_of<fa5_t, free_standing_call_args::arg_base>::value, 1, 0>::value
			+ meta::ternary<trait::is_kind_of<fa6_t, free_standing_call_args::arg_base>::value, 1, 0>::value
			+ meta::ternary<trait::is_kind_of<fa7_t, free_standing_call_args::arg_base>::value, 1, 0>::value
			+ meta::ternary<trait::is_kind_of<fa8_t, free_standing_call_args::arg_base>::value, 1, 0>::value
			+ meta::ternary<trait::is_kind_of<fa9_t, free_standing_call_args::arg_base>::value, 1, 0>::value
			+ meta::ternary<trait::is_kind_of<fa10_t, free_standing_call_args::arg_base>::value, 1, 0>::value;
		};
	template<typename ra0_t, typename ra1_t = trait::no_type,
		typename ra2_t = trait::no_type, typename ra3_t = trait::no_type,
		typename ra4_t = trait::no_type, typename ra5_t = trait::no_type,
		typename ra6_t = trait::no_type, typename ra7_t = trait::no_type,
		typename ra8_t = trait::no_type, typename ra9_t = trait::no_type,
		typename ra10_t = trait::no_type,
		typename f0_t = trait::no_type, typename f1_t = trait::no_type,
		typename f2_t = trait::no_type, typename f3_t = trait::no_type,
		typename f4_t = trait::no_type, typename f5_t = trait::no_type,
		typename f6_t = trait::no_type, typename f7_t = trait::no_type,
		typename f8_t = trait::no_type, typename f9_t = trait::no_type,
		typename f10_t = trait::no_type>
	struct free_standing_args
		{
		/* f* are types deduced from call() arguments
		 * ra* are true argument types of function/method
		 */
		typedef typename trait::select_index<trait::find_type<free_standing_call_args::arg<1>,
			f0_t, f1_t, f2_t, f3_t, f4_t, f5_t, f6_t, f7_t, f8_t, f9_t, f10_t>::value,
			ra0_t, ra1_t, ra2_t, ra3_t, ra4_t, ra5_t, ra6_t, ra7_t, ra8_t, ra9_t, ra10_t>::type a0_t;
		typedef typename trait::select_index<trait::find_type<free_standing_call_args::arg<2>,
			f0_t, f1_t, f2_t, f3_t, f4_t, f5_t, f6_t, f7_t, f8_t, f9_t, f10_t>::value,
			ra0_t, ra1_t, ra2_t, ra3_t, ra4_t, ra5_t, ra6_t, ra7_t, ra8_t, ra9_t, ra10_t>::type a1_t;
		typedef typename trait::select_index<trait::find_type<free_standing_call_args::arg<3>,
			f0_t, f1_t, f2_t, f3_t, f4_t, f5_t, f6_t, f7_t, f8_t, f9_t, f10_t>::value,
			ra0_t, ra1_t, ra2_t, ra3_t, ra4_t, ra5_t, ra6_t, ra7_t, ra8_t, ra9_t, ra10_t>::type a2_t;
		typedef typename trait::select_index<trait::find_type<free_standing_call_args::arg<4>,
			f0_t, f1_t, f2_t, f3_t, f4_t, f5_t, f6_t, f7_t, f8_t, f9_t, f10_t>::value,
			ra0_t, ra1_t, ra2_t, ra3_t, ra4_t, ra5_t, ra6_t, ra7_t, ra8_t, ra9_t, ra10_t>::type a3_t;
		typedef typename trait::select_index<trait::find_type<free_standing_call_args::arg<5>,
			f0_t, f1_t, f2_t, f3_t, f4_t, f5_t, f6_t, f7_t, f8_t, f9_t, f10_t>::value,
			ra0_t, ra1_t, ra2_t, ra3_t, ra4_t, ra5_t, ra6_t, ra7_t, ra8_t, ra9_t, ra10_t>::type a4_t;
		typedef typename trait::select_index<trait::find_type<free_standing_call_args::arg<6>,
			f0_t, f1_t, f2_t, f3_t, f4_t, f5_t, f6_t, f7_t, f8_t, f9_t, f10_t>::value,
			ra0_t, ra1_t, ra2_t, ra3_t, ra4_t, ra5_t, ra6_t, ra7_t, ra8_t, ra9_t, ra10_t>::type a5_t;
		typedef typename trait::select_index<trait::find_type<free_standing_call_args::arg<7>,
			f0_t, f1_t, f2_t, f3_t, f4_t, f5_t, f6_t, f7_t, f8_t, f9_t, f10_t>::value,
			ra0_t, ra1_t, ra2_t, ra3_t, ra4_t, ra5_t, ra6_t, ra7_t, ra8_t, ra9_t, ra10_t>::type a6_t;
		typedef typename trait::select_index<trait::find_type<free_standing_call_args::arg<8>,
			f0_t, f1_t, f2_t, f3_t, f4_t, f5_t, f6_t, f7_t, f8_t, f9_t, f10_t>::value,
			ra0_t, ra1_t, ra2_t, ra3_t, ra4_t, ra5_t, ra6_t, ra7_t, ra8_t, ra9_t, ra10_t>::type a7_t;
		typedef typename trait::select_index<trait::find_type<free_standing_call_args::arg<9>,
			f0_t, f1_t, f2_t, f3_t, f4_t, f5_t, f6_t, f7_t, f8_t, f9_t, f10_t>::value,
			ra0_t, ra1_t, ra2_t, ra3_t, ra4_t, ra5_t, ra6_t, ra7_t, ra8_t, ra9_t, ra10_t>::type a8_t;
		typedef typename trait::select_index<trait::find_type<free_standing_call_args::arg<10>,
			f0_t, f1_t, f2_t, f3_t, f4_t, f5_t, f6_t, f7_t, f8_t, f9_t, f10_t>::value,
			ra0_t, ra1_t, ra2_t, ra3_t, ra4_t, ra5_t, ra6_t, ra7_t, ra8_t, ra9_t, ra10_t>::type a9_t;
		typedef typename trait::select_index<trait::find_type<free_standing_call_args::arg<11>,
			f0_t, f1_t, f2_t, f3_t, f4_t, f5_t, f6_t, f7_t, f8_t, f9_t, f10_t>::value,
			ra0_t, ra1_t, ra2_t, ra3_t, ra4_t, ra5_t, ra6_t, ra7_t, ra8_t, ra9_t, ra10_t>::type a10_t;
		};
	struct functor
		{
		typedef HFunctor<fa0_t, METHOD_t> functor_t;
		typedef HCall<free_standing_args_count::value, descriptor, typename trait::return_type<METHOD_t>::type,
						functor_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t, fa9_t, fa10_t> type;
		typedef free_standing_args<
			typename trait::argument_type<METHOD_t, 0>::type,
			typename trait::argument_type<METHOD_t, 1>::type,
			typename trait::argument_type<METHOD_t, 2>::type,
			typename trait::argument_type<METHOD_t, 3>::type,
			typename trait::argument_type<METHOD_t, 4>::type,
			typename trait::argument_type<METHOD_t, 5>::type,
			typename trait::argument_type<METHOD_t, 6>::type,
			typename trait::argument_type<METHOD_t, 7>::type,
			typename trait::argument_type<METHOD_t, 8>::type,
			typename trait::argument_type<METHOD_t, 9>::type,
			typename trait::argument_type<METHOD_t, 10>::type,
			fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t, fa9_t, fa10_t> sorted_real_args;
		typedef HCallInterface<free_standing_args_count::value, typename trait::return_type<METHOD_t>::type,
						typename sorted_real_args::a0_t, typename sorted_real_args::a1_t, typename sorted_real_args::a2_t, typename sorted_real_args::a3_t,
						typename sorted_real_args::a4_t, typename sorted_real_args::a5_t, typename sorted_real_args::a6_t, typename sorted_real_args::a7_t,
						typename sorted_real_args::a8_t, typename sorted_real_args::a9_t, typename sorted_real_args::a10_t> interface_type;
		inline static type make( METHOD_t m, fa0_t fa0, fa1_t fa1, fa2_t fa2, fa3_t fa3, fa4_t fa4, fa5_t fa5, fa6_t fa6, fa7_t fa7, fa8_t fa8, fa9_t fa9, fa10_t fa10 )
			{ return type( functor_t( fa0, m ), fa1, fa2, fa3, fa4, fa5, fa6, fa7, fa8, fa9, fa10 ); }
		inline static type make( METHOD_t m, fa0_t fa0, fa1_t fa1, fa2_t fa2, fa3_t fa3, fa4_t fa4, fa5_t fa5, fa6_t fa6, fa7_t fa7, fa8_t fa8, fa9_t fa9 )
			{ return type( functor_t( fa0, m ), fa1, fa2, fa3, fa4, fa5, fa6, fa7, fa8, fa9 ); }
		inline static type make( METHOD_t m, fa0_t fa0, fa1_t fa1, fa2_t fa2, fa3_t fa3, fa4_t fa4, fa5_t fa5, fa6_t fa6, fa7_t fa7, fa8_t fa8 )
			{ return type( functor_t( fa0, m ), fa1, fa2, fa3, fa4, fa5, fa6, fa7, fa8 ); }
		inline static type make( METHOD_t m, fa0_t fa0, fa1_t fa1, fa2_t fa2, fa3_t fa3, fa4_t fa4, fa5_t fa5, fa6_t fa6, fa7_t fa7 )
			{ return type( functor_t( fa0, m ), fa1, fa2, fa3, fa4, fa5, fa6, fa7 ); }
		inline static type make( METHOD_t m, fa0_t fa0, fa1_t fa1, fa2_t fa2, fa3_t fa3, fa4_t fa4, fa5_t fa5, fa6_t fa6 )
			{ return type( functor_t( fa0, m ), fa1, fa2, fa3, fa4, fa5, fa6 ); }
		inline static type make( METHOD_t m, fa0_t fa0, fa1_t fa1, fa2_t fa2, fa3_t fa3, fa4_t fa4, fa5_t fa5 )
			{ return type( functor_t( fa0, m ), fa1, fa2, fa3, fa4, fa5 ); }
		inline static type make( METHOD_t m, fa0_t fa0, fa1_t fa1, fa2_t fa2, fa3_t fa3, fa4_t fa4 )
			{ return type( functor_t( fa0, m ), fa1, fa2, fa3, fa4 ); }
		inline static type make( METHOD_t m, fa0_t fa0, fa1_t fa1, fa2_t fa2, fa3_t fa3 )
			{ return type( functor_t( fa0, m ), fa1, fa2, fa3 ); }
		inline static type make( METHOD_t m, fa0_t fa0, fa1_t fa1, fa2_t fa2 )
			{ return type( functor_t( fa0, m ), fa1, fa2 ); }
		inline static type make( METHOD_t m, fa0_t fa0, fa1_t fa1 )
			{ return type( functor_t( fa0, m ), fa1 ); }
		inline static type make( METHOD_t m, fa0_t fa0 )
			{ return type( functor_t( fa0, m ) ); }
		};
	struct functor_this
		{
		typedef HFunctor<fa0_t, METHOD_t> functor_t;
		typedef HCall<free_standing_args_count::value, descriptor, typename trait::return_type<METHOD_t>::type,
						functor_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t, fa9_t, fa10_t> type;
		typedef free_standing_args<
			typename trait::class_type<METHOD_t>::type&,
			typename trait::argument_type<METHOD_t, 0>::type,
			typename trait::argument_type<METHOD_t, 1>::type,
			typename trait::argument_type<METHOD_t, 2>::type,
			typename trait::argument_type<METHOD_t, 3>::type,
			typename trait::argument_type<METHOD_t, 4>::type,
			typename trait::argument_type<METHOD_t, 5>::type,
			typename trait::argument_type<METHOD_t, 6>::type,
			typename trait::argument_type<METHOD_t, 7>::type,
			typename trait::argument_type<METHOD_t, 8>::type,
			typename trait::argument_type<METHOD_t, 9>::type,
			fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t, fa9_t, fa10_t> sorted_real_args;
		typedef HCallInterface<free_standing_args_count::value, typename trait::return_type<METHOD_t>::type,
						typename sorted_real_args::a0_t, typename sorted_real_args::a1_t, typename sorted_real_args::a2_t, typename sorted_real_args::a3_t,
						typename sorted_real_args::a4_t, typename sorted_real_args::a5_t, typename sorted_real_args::a6_t, typename sorted_real_args::a7_t,
						typename sorted_real_args::a8_t, typename sorted_real_args::a9_t, typename sorted_real_args::a10_t> interface_type;
		inline static type make( METHOD_t m, fa0_t fa0, fa1_t fa1, fa2_t fa2, fa3_t fa3, fa4_t fa4, fa5_t fa5, fa6_t fa6, fa7_t fa7, fa8_t fa8, fa9_t fa9, fa10_t fa10 )
			{ return type( functor_t( fa0, m ), fa0, fa1, fa2, fa3, fa4, fa5, fa6, fa7, fa8, fa9, fa10 ); }
		inline static type make( METHOD_t m, fa0_t fa0, fa1_t fa1, fa2_t fa2, fa3_t fa3, fa4_t fa4, fa5_t fa5, fa6_t fa6, fa7_t fa7, fa8_t fa8, fa9_t fa9 )
			{ return type( functor_t( fa0, m ), fa0, fa1, fa2, fa3, fa4, fa5, fa6, fa7, fa8, fa9 ); }
		inline static type make( METHOD_t m, fa0_t fa0, fa1_t fa1, fa2_t fa2, fa3_t fa3, fa4_t fa4, fa5_t fa5, fa6_t fa6, fa7_t fa7, fa8_t fa8 )
			{ return type( functor_t( fa0, m ), fa0, fa1, fa2, fa3, fa4, fa5, fa6, fa7, fa8 ); }
		inline static type make( METHOD_t m, fa0_t fa0, fa1_t fa1, fa2_t fa2, fa3_t fa3, fa4_t fa4, fa5_t fa5, fa6_t fa6, fa7_t fa7 )
			{ return type( functor_t( fa0, m ), fa0, fa1, fa2, fa3, fa4, fa5, fa6, fa7 ); }
		inline static type make( METHOD_t m, fa0_t fa0, fa1_t fa1, fa2_t fa2, fa3_t fa3, fa4_t fa4, fa5_t fa5, fa6_t fa6 )
			{ return type( functor_t( fa0, m ), fa0, fa1, fa2, fa3, fa4, fa5, fa6 ); }
		inline static type make( METHOD_t m, fa0_t fa0, fa1_t fa1, fa2_t fa2, fa3_t fa3, fa4_t fa4, fa5_t fa5 )
			{ return type( functor_t( fa0, m ), fa0, fa1, fa2, fa3, fa4, fa5 ); }
		inline static type make( METHOD_t m, fa0_t fa0, fa1_t fa1, fa2_t fa2, fa3_t fa3, fa4_t fa4 )
			{ return type( functor_t( fa0, m ), fa0, fa1, fa2, fa3, fa4 ); }
		inline static type make( METHOD_t m, fa0_t fa0, fa1_t fa1, fa2_t fa2, fa3_t fa3 )
			{ return type( functor_t( fa0, m ), fa0, fa1, fa2, fa3 ); }
		inline static type make( METHOD_t m, fa0_t fa0, fa1_t fa1, fa2_t fa2 )
			{ return type( functor_t( fa0, m ), fa0, fa1, fa2 ); }
		inline static type make( METHOD_t m, fa0_t fa0, fa1_t fa1 )
			{ return type( functor_t( fa0, m ), fa0, fa1 ); }
		inline static type make( METHOD_t m, fa0_t fa0 )
			{ return type( functor_t( fa0, m ), fa0 ); }
		};
	struct function
		{
		typedef HCall<free_standing_args_count::value, descriptor, typename trait::return_type<METHOD_t>::type,
						METHOD_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t, fa9_t> type;
		typedef free_standing_args<
			typename trait::argument_type<METHOD_t, 0>::type,
			typename trait::argument_type<METHOD_t, 1>::type,
			typename trait::argument_type<METHOD_t, 2>::type,
			typename trait::argument_type<METHOD_t, 3>::type,
			typename trait::argument_type<METHOD_t, 4>::type,
			typename trait::argument_type<METHOD_t, 5>::type,
			typename trait::argument_type<METHOD_t, 6>::type,
			typename trait::argument_type<METHOD_t, 7>::type,
			typename trait::argument_type<METHOD_t, 8>::type,
			typename trait::argument_type<METHOD_t, 9>::type,
			typename trait::argument_type<METHOD_t, 10>::type,
			fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t, fa9_t, fa10_t> sorted_real_args;
		typedef HCallInterface<free_standing_args_count::value, typename trait::return_type<METHOD_t>::type,
						typename sorted_real_args::a0_t, typename sorted_real_args::a1_t, typename sorted_real_args::a2_t, typename sorted_real_args::a3_t,
						typename sorted_real_args::a4_t, typename sorted_real_args::a5_t, typename sorted_real_args::a6_t, typename sorted_real_args::a7_t,
						typename sorted_real_args::a8_t, typename sorted_real_args::a9_t, typename sorted_real_args::a10_t> interface_type;
		inline static type make( METHOD_t m, fa0_t fa0, fa1_t fa1, fa2_t fa2, fa3_t fa3, fa4_t fa4, fa5_t fa5, fa6_t fa6, fa7_t fa7, fa8_t fa8, fa9_t fa9 )
			{ return type( m, fa0, fa1, fa2, fa3, fa4, fa5, fa6, fa7, fa8, fa9 ); }
		inline static type make( METHOD_t m, fa0_t fa0, fa1_t fa1, fa2_t fa2, fa3_t fa3, fa4_t fa4, fa5_t fa5, fa6_t fa6, fa7_t fa7, fa8_t fa8 )
			{ return type( m, fa0, fa1, fa2, fa3, fa4, fa5, fa6, fa7, fa8 ); }
		inline static type make( METHOD_t m, fa0_t fa0, fa1_t fa1, fa2_t fa2, fa3_t fa3, fa4_t fa4, fa5_t fa5, fa6_t fa6, fa7_t fa7 )
			{ return type( m, fa0, fa1, fa2, fa3, fa4, fa5, fa6, fa7 ); }
		inline static type make( METHOD_t m, fa0_t fa0, fa1_t fa1, fa2_t fa2, fa3_t fa3, fa4_t fa4, fa5_t fa5, fa6_t fa6 )
			{ return type( m, fa0, fa1, fa2, fa3, fa4, fa5, fa6 ); }
		inline static type make( METHOD_t m, fa0_t fa0, fa1_t fa1, fa2_t fa2, fa3_t fa3, fa4_t fa4, fa5_t fa5 )
			{ return type( m, fa0, fa1, fa2, fa3, fa4, fa5 ); }
		inline static type make( METHOD_t m, fa0_t fa0, fa1_t fa1, fa2_t fa2, fa3_t fa3, fa4_t fa4 )
			{ return type( m, fa0, fa1, fa2, fa3, fa4 ); }
		inline static type make( METHOD_t m, fa0_t fa0, fa1_t fa1, fa2_t fa2, fa3_t fa3 )
			{ return type( m, fa0, fa1, fa2, fa3 ); }
		inline static type make( METHOD_t m, fa0_t fa0, fa1_t fa1, fa2_t fa2 )
			{ return type( m, fa0, fa1, fa2 ); }
		inline static type make( METHOD_t m, fa0_t fa0, fa1_t fa1 )
			{ return type( m, fa0, fa1 ); }
		inline static type make( METHOD_t m, fa0_t fa0 )
			{ return type( m, fa0 ); }
		inline static type make( METHOD_t m )
			{ return type( m ); }
		};
	struct field
		{
		typedef HCall<-2, descriptor, typename trait::field_type<METHOD_t>::type,
						fa0_t, METHOD_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t, fa9_t> type;
		inline static type make( METHOD_t m, fa0_t a0 )
			{ return ( type( m, a0 ) ); }
		};
	struct field_this
		{
		typedef HCall<-1, descriptor, typename trait::field_type<METHOD_t>::type,
						fa0_t, METHOD_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t, fa9_t> type;
		inline static type make( METHOD_t m, fa0_t )
			{ return ( type( m ) ); }
		};
	typedef typename trait::ternary<trait::is_member<METHOD_t>::value, /* we have method */
					typename trait::ternary<meta::greater<trait::find_type<fa0_t, /* is 'this' a free standing arg? */
					free_standing_call_args::arg<1>,
					free_standing_call_args::arg<2>,
					free_standing_call_args::arg<3>,
					free_standing_call_args::arg<4>,
					free_standing_call_args::arg<5>,
					free_standing_call_args::arg<6>,
					free_standing_call_args::arg<7>,
					free_standing_call_args::arg<8>,
					free_standing_call_args::arg<9>,
					free_standing_call_args::arg<10> >::value, -1>::value,
						functor_this, functor>::type,
						typename trait::ternary<trait::is_field<METHOD_t>::value,
							typename trait::ternary<trait::same_type<fa0_t, free_standing_call_args::arg<1> >::value,
								field_this, field>::type, function>::type>::type type;
	};

/*! \brief Base class for HCall<1..10>.
 */
template<int free_args, typename descriptor, typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t, typename a6_t, typename a7_t,
	typename a8_t, typename a9_t, typename a10_t>
class HCallBase : public descriptor::type::interface_type
	{
protected:
	CALL_t _call;
public:
	HCallBase( CALL_t call_ ) : _call( call_ ) {}
protected:
	virtual void const* do_id( void ) const
		{ return ( caller_id( _call ) ); }
	};

template<typename descriptor, typename return_t, typename class_t, typename field_t>
class HCall<-2, descriptor, return_t, class_t, field_t,
	trait::no_type, trait::no_type, trait::no_type, trait::no_type,
	trait::no_type, trait::no_type, trait::no_type, trait::no_type,
	trait::no_type, trait::no_type>
	{
	field_t _field;
	class_t _object;
public:
	HCall(  field_t field_, class_t object_ ) : _field( field_ ), _object( object_ ) {}
	return_t const& operator()( void ) const
		{
		return ( _object->*_field );
		}
	};

template<typename descriptor, typename return_t, typename class_t, typename field_t>
class HCall<-1, descriptor, return_t, class_t, field_t,
	trait::no_type, trait::no_type, trait::no_type, trait::no_type,
	trait::no_type, trait::no_type, trait::no_type, trait::no_type,
	trait::no_type, trait::no_type> : public unary_function<return_t, typename trait::class_type<field_t>::type>
	{
public:
	typedef unary_function<return_t, typename trait::class_type<field_t>::type> base_type;
	typedef typename base_type::result_type result_type;
	typedef typename base_type::argument_type argument_type;
private:
	field_t _field;
public:
	HCall( field_t field_ ) : _field( field_ ) {}
	return_t const& operator()( argument_type const& obj_ ) const
		{
		return ( obj_.*_field );
		}
	return_t& operator()( argument_type& obj_ )
		{
		return ( obj_.*_field );
		}
	};

/*! \cond */

template<typename descriptor, typename return_t, typename CALL_t>
class HCall<0, descriptor, return_t, CALL_t,
	trait::no_type, trait::no_type, trait::no_type, trait::no_type,
	trait::no_type, trait::no_type, trait::no_type, trait::no_type,
	trait::no_type, trait::no_type, trait::no_type>
		: public HCallBase<0, descriptor, return_t, CALL_t,
	trait::no_type, trait::no_type, trait::no_type, trait::no_type,
	trait::no_type, trait::no_type, trait::no_type, trait::no_type,
	trait::no_type, trait::no_type, trait::no_type>
	{
	typedef HCallBase<0, descriptor, return_t, CALL_t,
	trait::no_type, trait::no_type, trait::no_type, trait::no_type,
	trait::no_type, trait::no_type, trait::no_type, trait::no_type,
	trait::no_type, trait::no_type, trait::no_type> base_t;
public:
	HCall( CALL_t call_ )
		: base_t( call_ ) {}
	virtual return_t do_invoke( void )
		{ return ( (base_t::_call)() ); }
	virtual return_t do_invoke( void ) const
		{ return ( (base_t::_call)() ); }
	};

template<typename descriptor, typename return_t, typename CALL_t, typename a0_t>
class HCall<0, descriptor, return_t, CALL_t, a0_t,
	trait::no_type, trait::no_type, trait::no_type, trait::no_type,
	trait::no_type, trait::no_type, trait::no_type, trait::no_type,
	trait::no_type, trait::no_type>
		: public HCallBase<0, descriptor, return_t, CALL_t, a0_t,
	trait::no_type, trait::no_type, trait::no_type, trait::no_type,
	trait::no_type, trait::no_type, trait::no_type, trait::no_type,
	trait::no_type, trait::no_type>
	{
	typedef HCallBase<0, descriptor, return_t, CALL_t, a0_t,
	trait::no_type, trait::no_type, trait::no_type, trait::no_type,
	trait::no_type, trait::no_type, trait::no_type, trait::no_type,
	trait::no_type, trait::no_type> base_t;
	a0_t _a0;
public:
	HCall( CALL_t call_, a0_t a0 )
		: base_t( call_ ), _a0( a0 ) {}
	virtual return_t do_invoke( void )
		{ return ( (base_t::_call)( _a0 ) ); }
	virtual return_t do_invoke( void ) const
		{ return ( (base_t::_call)( _a0 ) ); }
	};

template<typename descriptor, typename return_t, typename CALL_t, typename a0_t, typename a1_t>
class HCall<0, descriptor, return_t, CALL_t, a0_t, a1_t,
	trait::no_type, trait::no_type, trait::no_type, trait::no_type,
	trait::no_type, trait::no_type, trait::no_type, trait::no_type, trait::no_type>
		: public HCallBase<0, descriptor, return_t, CALL_t, a0_t, a1_t,
	trait::no_type, trait::no_type, trait::no_type, trait::no_type,
	trait::no_type, trait::no_type, trait::no_type, trait::no_type, trait::no_type>
	{
	typedef HCallBase<0, descriptor, return_t, CALL_t, a0_t, a1_t,
	trait::no_type, trait::no_type, trait::no_type, trait::no_type,
	trait::no_type, trait::no_type, trait::no_type, trait::no_type, trait::no_type> base_t;
	a0_t _a0;
	a1_t _a1;
public:
	HCall( CALL_t call_, a0_t a0, a1_t a1 )
		: base_t( call_ ), _a0( a0 ), _a1( a1 ) {}
	virtual return_t do_invoke( void )
		{ return ( (base_t::_call)( _a0, _a1 ) ); }
	virtual return_t do_invoke( void ) const
		{ return ( (base_t::_call)( _a0, _a1 ) ); }
	};

template<typename descriptor, typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t>
class HCall<0, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t,
	trait::no_type, trait::no_type, trait::no_type, trait::no_type,
	trait::no_type, trait::no_type, trait::no_type, trait::no_type>
		: public HCallBase<0, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t,
	trait::no_type, trait::no_type, trait::no_type, trait::no_type,
	trait::no_type, trait::no_type, trait::no_type, trait::no_type>
	{
	typedef HCallBase<0, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t,
	trait::no_type, trait::no_type, trait::no_type, trait::no_type,
	trait::no_type, trait::no_type, trait::no_type, trait::no_type> base_t;
	a0_t _a0;
	a1_t _a1;
	a2_t _a2;
public:
	HCall( CALL_t call_, a0_t a0, a1_t a1, a2_t a2 )
		: base_t( call_ ), _a0( a0 ), _a1( a1 ), _a2( a2 ) {}
	virtual return_t do_invoke( void )
		{ return ( (base_t::_call)( _a0, _a1, _a2 ) ); }
	virtual return_t do_invoke( void ) const
		{ return ( (base_t::_call)( _a0, _a1, _a2 ) ); }
	};

template<typename descriptor, typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t>
class HCall<0, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t,
	trait::no_type, trait::no_type, trait::no_type, trait::no_type,
	trait::no_type, trait::no_type, trait::no_type>
		: public HCallBase<0, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t,
	trait::no_type, trait::no_type, trait::no_type, trait::no_type,
	trait::no_type, trait::no_type, trait::no_type>
	{
	typedef HCallBase<0, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t,
	trait::no_type, trait::no_type, trait::no_type, trait::no_type,
	trait::no_type, trait::no_type, trait::no_type> base_t;
	a0_t _a0;
	a1_t _a1;
	a2_t _a2;
	a3_t _a3;
public:
	HCall( CALL_t call_, a0_t a0, a1_t a1, a2_t a2, a3_t a3 )
		: base_t( call_ ), _a0( a0 ), _a1( a1 ), _a2( a2 ), _a3( a3 ) {}
	virtual return_t do_invoke( void )
		{ return ( (base_t::_call)( _a0, _a1, _a2, _a3 ) ); }
	virtual return_t do_invoke( void ) const
		{ return ( (base_t::_call)( _a0, _a1, _a2, _a3 ) ); }
	};

template<typename descriptor, typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t>
class HCall<0, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t,
	trait::no_type, trait::no_type, trait::no_type, trait::no_type,
	trait::no_type, trait::no_type>
		: public HCallBase<0, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t,
	trait::no_type, trait::no_type, trait::no_type, trait::no_type,
	trait::no_type, trait::no_type>
	{
	typedef HCallBase<0, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t,
	trait::no_type, trait::no_type, trait::no_type, trait::no_type,
	trait::no_type, trait::no_type> base_t;
	a0_t _a0;
	a1_t _a1;
	a2_t _a2;
	a3_t _a3;
	a4_t _a4;
public:
	HCall( CALL_t call_, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4 )
		: base_t( call_ ), _a0( a0 ), _a1( a1 ), _a2( a2 ), _a3( a3 ), _a4( a4 ) {}
	virtual return_t do_invoke( void )
		{ return ( (base_t::_call)( _a0, _a1, _a2, _a3, _a4 ) ); }
	virtual return_t do_invoke( void ) const
		{ return ( (base_t::_call)( _a0, _a1, _a2, _a3, _a4 ) ); }
	};

template<typename descriptor, typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t>
class HCall<0, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t,
	trait::no_type, trait::no_type, trait::no_type, trait::no_type, trait::no_type>
		: public HCallBase<0, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t,
	trait::no_type, trait::no_type, trait::no_type, trait::no_type, trait::no_type>
	{
	typedef HCallBase<0, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t,
	trait::no_type, trait::no_type, trait::no_type, trait::no_type, trait::no_type> base_t;
	a0_t _a0;
	a1_t _a1;
	a2_t _a2;
	a3_t _a3;
	a4_t _a4;
	a5_t _a5;
public:
	HCall( CALL_t call_, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5 )
		: base_t( call_ ), _a0( a0 ), _a1( a1 ), _a2( a2 ), _a3( a3 ), _a4( a4 ), _a5( a5 ) {}
	virtual return_t do_invoke( void )
		{ return ( (base_t::_call)( _a0, _a1, _a2, _a3, _a4, _a5 ) ); }
	virtual return_t do_invoke( void ) const
		{ return ( (base_t::_call)( _a0, _a1, _a2, _a3, _a4, _a5 ) ); }
	};

template<typename descriptor, typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t, typename a6_t>
class HCall<0, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t,
	trait::no_type, trait::no_type, trait::no_type, trait::no_type>
		: public HCallBase<0, descriptor, return_t, CALL_t,
	a0_t, a1_t, a2_t, a3_t,
	a4_t, a5_t, a6_t, trait::no_type,
	trait::no_type, trait::no_type, trait::no_type>
	{
	typedef HCallBase<0, descriptor, return_t, CALL_t,
	a0_t, a1_t, a2_t, a3_t,
	a4_t, a5_t, a6_t, trait::no_type,
	trait::no_type, trait::no_type, trait::no_type> base_t;
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
		: base_t( call_ ), _a0( a0 ), _a1( a1 ), _a2( a2 ), _a3( a3 ), _a4( a4 ), _a5( a5 ), _a6( a6 ) {}
	virtual return_t do_invoke( void )
		{ return ( (base_t::_call)( _a0, _a1, _a2, _a3, _a4, _a5, _a6 ) ); }
	virtual return_t do_invoke( void ) const
		{ return ( (base_t::_call)( _a0, _a1, _a2, _a3, _a4, _a5, _a6 ) ); }
	};

template<typename descriptor, typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t, typename a6_t, typename a7_t>
class HCall<0, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t,
	trait::no_type, trait::no_type, trait::no_type>
		: public HCallBase<0, descriptor, return_t, CALL_t,
	a0_t, a1_t, a2_t, a3_t,
	a4_t, a5_t, a6_t, a7_t,
	trait::no_type, trait::no_type, trait::no_type>
	{
	typedef HCallBase<0, descriptor, return_t, CALL_t,
	a0_t, a1_t, a2_t, a3_t,
	a4_t, a5_t, a6_t, a7_t,
	trait::no_type, trait::no_type, trait::no_type> base_t;
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
		: base_t( call_ ), _a0( a0 ), _a1( a1 ), _a2( a2 ), _a3( a3 ), _a4( a4 ), _a5( a5 ), _a6( a6 ), _a7( a7 ) {}
	virtual return_t do_invoke( void )
		{ return ( (base_t::_call)( _a0, _a1, _a2, _a3, _a4, _a5, _a6, _a7 ) ); }
	virtual return_t do_invoke( void ) const
		{ return ( (base_t::_call)( _a0, _a1, _a2, _a3, _a4, _a5, _a6, _a7 ) ); }
	};

template<typename descriptor, typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t, typename a6_t, typename a7_t,
	typename a8_t>
class HCall<0, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t,
	trait::no_type, trait::no_type>
		: public HCallBase<0, descriptor, return_t, CALL_t,
	a0_t, a1_t, a2_t, a3_t,
	a4_t, a5_t, a6_t, a7_t,
	a8_t, trait::no_type, trait::no_type>
	{
	typedef HCallBase<0, descriptor, return_t, CALL_t,
	a0_t, a1_t, a2_t, a3_t,
	a4_t, a5_t, a6_t, a7_t,
	a8_t, trait::no_type, trait::no_type> base_t;
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
	HCall( CALL_t call_, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7, a8_t a8 )
		: base_t( call_ ), _a0( a0 ), _a1( a1 ), _a2( a2 ), _a3( a3 ), _a4( a4 ), _a5( a5 ), _a6( a6 ), _a7( a7 ), _a8( a8 ) {}
	virtual return_t do_invoke( void )
		{ return ( (base_t::_call)( _a0, _a1, _a2, _a3, _a4, _a5, _a6, _a7, _a8 ) ); }
	virtual return_t do_invoke( void ) const
		{ return ( (base_t::_call)( _a0, _a1, _a2, _a3, _a4, _a5, _a6, _a7, _a8 ) ); }
	};

template<typename descriptor, typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t, typename a6_t, typename a7_t,
	typename a8_t, typename a9_t>
class HCall<0, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t, trait::no_type>
		: public HCallBase<0, descriptor, return_t, CALL_t,
	a0_t, a1_t, a2_t, a3_t,
	a4_t, a5_t, a6_t, a7_t,
	a8_t, a9_t, trait::no_type>
	{
	typedef HCallBase<0, descriptor, return_t, CALL_t,
	a0_t, a1_t, a2_t, a3_t,
	a4_t, a5_t, a6_t, a7_t,
	a8_t, a9_t, trait::no_type> base_t;
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
	HCall( CALL_t call_, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7, a8_t a8, a9_t a9 )
		: base_t( call_ ), _a0( a0 ), _a1( a1 ), _a2( a2 ), _a3( a3 ), _a4( a4 ), _a5( a5 ), _a6( a6 ), _a7( a7 ), _a8( a8 ), _a9( a9 ) {}
	virtual return_t do_invoke( void )
		{ return ( (base_t::_call)( _a0, _a1, _a2, _a3, _a4, _a5, _a6, _a7, _a8, _a9 ) ); }
	virtual return_t do_invoke( void ) const
		{ return ( (base_t::_call)( _a0, _a1, _a2, _a3, _a4, _a5, _a6, _a7, _a8, _a9 ) ); }
	};

template<typename descriptor, typename return_t, typename CALL_t, typename a0_t>
class HCall<1, descriptor, return_t, CALL_t, a0_t,
	trait::no_type, trait::no_type, trait::no_type, trait::no_type,
	trait::no_type, trait::no_type, trait::no_type, trait::no_type,
	trait::no_type>
		: public HCallBase<1, descriptor, return_t, CALL_t, a0_t,
	trait::no_type, trait::no_type, trait::no_type, trait::no_type,
	trait::no_type, trait::no_type, trait::no_type, trait::no_type,
	trait::no_type, trait::no_type>
	{
	typedef HCallBase<1, descriptor, return_t, CALL_t, a0_t,
	trait::no_type, trait::no_type, trait::no_type, trait::no_type,
	trait::no_type, trait::no_type, trait::no_type, trait::no_type,
	trait::no_type, trait::no_type> base_t;
	typedef typename descriptor::type::sorted_real_args::a0_t fa0_t;
	a0_t _a0;
public:
	HCall( CALL_t call_, a0_t a0 )
		: base_t( call_ ), _a0( a0 ) {}
	virtual return_t do_invoke( fa0_t a0 ) { return ( (base_t::_call)( free_standing_call_args::resolve_arg<1, a0_t, fa0_t>::get( _a0, a0 ) ) ); }
	virtual return_t do_invoke( fa0_t a0 ) const { return ( (base_t::_call)( free_standing_call_args::resolve_arg<1, a0_t, fa0_t>::get( _a0, a0 ) ) ); }
	};

template<typename descriptor, typename return_t, typename CALL_t, typename a0_t, typename a1_t>
class HCall<1, descriptor, return_t, CALL_t, a0_t, a1_t,
	trait::no_type, trait::no_type, trait::no_type, trait::no_type,
	trait::no_type, trait::no_type, trait::no_type, trait::no_type>
		: public HCallBase<1, descriptor, return_t, CALL_t, a0_t,
	a1_t, trait::no_type, trait::no_type, trait::no_type,
	trait::no_type, trait::no_type, trait::no_type, trait::no_type,
	trait::no_type, trait::no_type>
	{
	typedef HCallBase<1, descriptor, return_t, CALL_t, a0_t,
	a1_t, trait::no_type, trait::no_type, trait::no_type,
	trait::no_type, trait::no_type, trait::no_type, trait::no_type,
	trait::no_type, trait::no_type> base_t;
	typedef typename descriptor::type::sorted_real_args::a0_t fa0_t;
	a0_t _a0;
	a1_t _a1;
public:
	HCall( CALL_t call_, a0_t a0, a1_t a1 )
		: base_t( call_ ), _a0( a0 ), _a1( a1 ) {}
	virtual return_t do_invoke( fa0_t a0 ) { return ( (base_t::_call)(
				free_standing_call_args::resolve_arg<1, a0_t, fa0_t>::get( _a0, a0 ),
				free_standing_call_args::resolve_arg<1, a1_t, fa0_t>::get( _a1, a0 ) ) ); }
	virtual return_t do_invoke( fa0_t a0 ) const { return ( (base_t::_call)(
				free_standing_call_args::resolve_arg<1, a0_t, fa0_t>::get( _a0, a0 ),
				free_standing_call_args::resolve_arg<1, a1_t, fa0_t>::get( _a1, a0 ) ) ); }
	};

template<typename descriptor, typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t>
class HCall<1, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t,
	trait::no_type, trait::no_type, trait::no_type, trait::no_type,
	trait::no_type, trait::no_type, trait::no_type>
		: public HCallBase<1, descriptor, return_t, CALL_t, a0_t,
	a1_t, a2_t, trait::no_type, trait::no_type,
	trait::no_type, trait::no_type, trait::no_type, trait::no_type,
	trait::no_type, trait::no_type>
	{
	typedef HCallBase<1, descriptor, return_t, CALL_t, a0_t,
	a1_t, a2_t, trait::no_type, trait::no_type,
	trait::no_type, trait::no_type, trait::no_type, trait::no_type,
	trait::no_type, trait::no_type> base_t;
	typedef typename descriptor::type::sorted_real_args::a0_t fa0_t;
	a0_t _a0;
	a1_t _a1;
	a2_t _a2;
public:
	HCall( CALL_t call_, a0_t a0, a1_t a1, a2_t a2 )
		: base_t( call_ ), _a0( a0 ), _a1( a1 ), _a2( a2 ) {}
	virtual return_t do_invoke( fa0_t a0 ) { return ( (base_t::_call)(
				free_standing_call_args::resolve_arg<1, a0_t, fa0_t>::get( _a0, a0 ),
				free_standing_call_args::resolve_arg<1, a1_t, fa0_t>::get( _a1, a0 ),
				free_standing_call_args::resolve_arg<1, a2_t, fa0_t>::get( _a2, a0 ) ) ); }
	virtual return_t do_invoke( fa0_t a0 ) const { return ( (base_t::_call)(
				free_standing_call_args::resolve_arg<1, a0_t, fa0_t>::get( _a0, a0 ),
				free_standing_call_args::resolve_arg<1, a1_t, fa0_t>::get( _a1, a0 ),
				free_standing_call_args::resolve_arg<1, a2_t, fa0_t>::get( _a2, a0 ) ) ); }
	};

template<typename descriptor, typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t>
class HCall<1, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t,
	trait::no_type, trait::no_type, trait::no_type, trait::no_type,
	trait::no_type, trait::no_type>
		: public HCallBase<1, descriptor, return_t, CALL_t, a0_t,
	a1_t, a2_t, a3_t, trait::no_type,
	trait::no_type, trait::no_type, trait::no_type, trait::no_type,
	trait::no_type, trait::no_type>
	{
	typedef HCallBase<1, descriptor, return_t, CALL_t, a0_t,
	a1_t, a2_t, a3_t, trait::no_type,
	trait::no_type, trait::no_type, trait::no_type, trait::no_type,
	trait::no_type, trait::no_type> base_t;
	typedef typename descriptor::type::sorted_real_args::a0_t fa0_t;
	a0_t _a0;
	a1_t _a1;
	a2_t _a2;
	a3_t _a3;
public:
	HCall( CALL_t call_, a0_t a0, a1_t a1, a2_t a2, a3_t a3 )
		: base_t( call_ ), _a0( a0 ), _a1( a1 ), _a2( a2 ), _a3( a3 ) {}
	virtual return_t do_invoke( fa0_t a0 ) { return ( (base_t::_call)(
				free_standing_call_args::resolve_arg<1, a0_t, fa0_t>::get( _a0, a0 ),
				free_standing_call_args::resolve_arg<1, a1_t, fa0_t>::get( _a1, a0 ),
				free_standing_call_args::resolve_arg<1, a2_t, fa0_t>::get( _a2, a0 ),
				free_standing_call_args::resolve_arg<1, a3_t, fa0_t>::get( _a3, a0 ) ) ); }
	virtual return_t do_invoke( fa0_t a0 ) const { return ( (base_t::_call)(
				free_standing_call_args::resolve_arg<1, a0_t, fa0_t>::get( _a0, a0 ),
				free_standing_call_args::resolve_arg<1, a1_t, fa0_t>::get( _a1, a0 ),
				free_standing_call_args::resolve_arg<1, a2_t, fa0_t>::get( _a2, a0 ),
				free_standing_call_args::resolve_arg<1, a3_t, fa0_t>::get( _a3, a0 ) ) ); }
	};

template<typename descriptor, typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t>
class HCall<1, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t,
	trait::no_type, trait::no_type, trait::no_type, trait::no_type,
	trait::no_type>
		: public HCallBase<1, descriptor, return_t, CALL_t, a0_t,
	a1_t, a2_t, a3_t, a4_t,
	trait::no_type, trait::no_type, trait::no_type, trait::no_type,
	trait::no_type, trait::no_type>
	{
	typedef HCallBase<1, descriptor, return_t, CALL_t, a0_t,
	a1_t, a2_t, a3_t, a4_t,
	trait::no_type, trait::no_type, trait::no_type, trait::no_type,
	trait::no_type, trait::no_type> base_t;
	typedef typename descriptor::type::sorted_real_args::a0_t fa0_t;
	a0_t _a0;
	a1_t _a1;
	a2_t _a2;
	a3_t _a3;
	a4_t _a4;
public:
	HCall( CALL_t call_, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4 )
		: base_t( call_ ), _a0( a0 ), _a1( a1 ), _a2( a2 ), _a3( a3 ), _a4( a4 ) {}
	virtual return_t do_invoke( fa0_t a0 ) { return ( (base_t::_call)(
				free_standing_call_args::resolve_arg<1, a0_t, fa0_t>::get( _a0, a0 ),
				free_standing_call_args::resolve_arg<1, a1_t, fa0_t>::get( _a1, a0 ),
				free_standing_call_args::resolve_arg<1, a2_t, fa0_t>::get( _a2, a0 ),
				free_standing_call_args::resolve_arg<1, a3_t, fa0_t>::get( _a3, a0 ),
				free_standing_call_args::resolve_arg<1, a4_t, fa0_t>::get( _a4, a0 ) ) ); }
	virtual return_t do_invoke( fa0_t a0 ) const { return ( (base_t::_call)(
				free_standing_call_args::resolve_arg<1, a0_t, fa0_t>::get( _a0, a0 ),
				free_standing_call_args::resolve_arg<1, a1_t, fa0_t>::get( _a1, a0 ),
				free_standing_call_args::resolve_arg<1, a2_t, fa0_t>::get( _a2, a0 ),
				free_standing_call_args::resolve_arg<1, a3_t, fa0_t>::get( _a3, a0 ),
				free_standing_call_args::resolve_arg<1, a4_t, fa0_t>::get( _a4, a0 ) ) ); }
	};

template<typename descriptor, typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t>
class HCall<1, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t,
	trait::no_type, trait::no_type, trait::no_type, trait::no_type>
		: public HCallBase<1, descriptor, return_t, CALL_t, a0_t,
	a1_t, a2_t, a3_t, a4_t,
	a5_t, trait::no_type, trait::no_type, trait::no_type,
	trait::no_type, trait::no_type>
	{
	typedef HCallBase<1, descriptor, return_t, CALL_t, a0_t,
	a1_t, a2_t, a3_t, a4_t,
	a5_t, trait::no_type, trait::no_type, trait::no_type,
	trait::no_type, trait::no_type> base_t;
	typedef typename descriptor::type::sorted_real_args::a0_t fa0_t;
	a0_t _a0;
	a1_t _a1;
	a2_t _a2;
	a3_t _a3;
	a4_t _a4;
	a5_t _a5;
public:
	HCall( CALL_t call_, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5 )
		: base_t( call_ ), _a0( a0 ), _a1( a1 ), _a2( a2 ), _a3( a3 ), _a4( a4 ), _a5( a5 ) {}
	virtual return_t do_invoke( fa0_t a0 ) { return ( (base_t::_call)(
				free_standing_call_args::resolve_arg<1, a0_t, fa0_t>::get( _a0, a0 ),
				free_standing_call_args::resolve_arg<1, a1_t, fa0_t>::get( _a1, a0 ),
				free_standing_call_args::resolve_arg<1, a2_t, fa0_t>::get( _a2, a0 ),
				free_standing_call_args::resolve_arg<1, a3_t, fa0_t>::get( _a3, a0 ),
				free_standing_call_args::resolve_arg<1, a4_t, fa0_t>::get( _a4, a0 ),
				free_standing_call_args::resolve_arg<1, a5_t, fa0_t>::get( _a5, a0 ) ) ); }
	virtual return_t do_invoke( fa0_t a0 ) const { return ( (base_t::_call)(
				free_standing_call_args::resolve_arg<1, a0_t, fa0_t>::get( _a0, a0 ),
				free_standing_call_args::resolve_arg<1, a1_t, fa0_t>::get( _a1, a0 ),
				free_standing_call_args::resolve_arg<1, a2_t, fa0_t>::get( _a2, a0 ),
				free_standing_call_args::resolve_arg<1, a3_t, fa0_t>::get( _a3, a0 ),
				free_standing_call_args::resolve_arg<1, a4_t, fa0_t>::get( _a4, a0 ),
				free_standing_call_args::resolve_arg<1, a5_t, fa0_t>::get( _a5, a0 ) ) ); }
	};

template<typename descriptor, typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t, typename a6_t>
class HCall<1, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t,
	trait::no_type, trait::no_type, trait::no_type>
		: public HCallBase<1, descriptor, return_t, CALL_t, a0_t,
	a1_t, a2_t, a3_t, a4_t,
	a5_t, a6_t, trait::no_type, trait::no_type,
	trait::no_type, trait::no_type>
	{
	typedef HCallBase<1, descriptor, return_t, CALL_t, a0_t,
	a1_t, a2_t, a3_t, a4_t,
	a5_t, a6_t, trait::no_type, trait::no_type,
	trait::no_type, trait::no_type> base_t;
	typedef typename descriptor::type::sorted_real_args::a0_t fa0_t;
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
		: base_t( call_ ), _a0( a0 ), _a1( a1 ), _a2( a2 ), _a3( a3 ), _a4( a4 ), _a5( a5 ), _a6( a6 ) {}
	virtual return_t do_invoke( fa0_t a0 ) { return ( (base_t::_call)(
				free_standing_call_args::resolve_arg<1, a0_t, fa0_t>::get( _a0, a0 ),
				free_standing_call_args::resolve_arg<1, a1_t, fa0_t>::get( _a1, a0 ),
				free_standing_call_args::resolve_arg<1, a2_t, fa0_t>::get( _a2, a0 ),
				free_standing_call_args::resolve_arg<1, a3_t, fa0_t>::get( _a3, a0 ),
				free_standing_call_args::resolve_arg<1, a4_t, fa0_t>::get( _a4, a0 ),
				free_standing_call_args::resolve_arg<1, a5_t, fa0_t>::get( _a5, a0 ),
				free_standing_call_args::resolve_arg<1, a6_t, fa0_t>::get( _a6, a0 ) ) ); }
	virtual return_t do_invoke( fa0_t a0 ) const { return ( (base_t::_call)(
				free_standing_call_args::resolve_arg<1, a0_t, fa0_t>::get( _a0, a0 ),
				free_standing_call_args::resolve_arg<1, a1_t, fa0_t>::get( _a1, a0 ),
				free_standing_call_args::resolve_arg<1, a2_t, fa0_t>::get( _a2, a0 ),
				free_standing_call_args::resolve_arg<1, a3_t, fa0_t>::get( _a3, a0 ),
				free_standing_call_args::resolve_arg<1, a4_t, fa0_t>::get( _a4, a0 ),
				free_standing_call_args::resolve_arg<1, a5_t, fa0_t>::get( _a5, a0 ),
				free_standing_call_args::resolve_arg<1, a6_t, fa0_t>::get( _a6, a0 ) ) ); }
	};

template<typename descriptor, typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t, typename a6_t, typename a7_t>
class HCall<1, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t,
	trait::no_type, trait::no_type>
		: public HCallBase<1, descriptor, return_t, CALL_t, a0_t,
	a1_t, a2_t, a3_t, a4_t,
	a5_t, a6_t, a7_t, trait::no_type,
	trait::no_type, trait::no_type>
	{
	typedef HCallBase<1, descriptor, return_t, CALL_t, a0_t,
	a1_t, a2_t, a3_t, a4_t,
	a5_t, a6_t, a7_t, trait::no_type,
	trait::no_type, trait::no_type> base_t;
	typedef typename descriptor::type::sorted_real_args::a0_t fa0_t;
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
		: base_t( call_ ), _a0( a0 ), _a1( a1 ), _a2( a2 ), _a3( a3 ), _a4( a4 ), _a5( a5 ), _a6( a6 ), _a7( a7 ) {}
	virtual return_t do_invoke( fa0_t a0 ) { return ( (base_t::_call)(
				free_standing_call_args::resolve_arg<1, a0_t, fa0_t>::get( _a0, a0 ),
				free_standing_call_args::resolve_arg<1, a1_t, fa0_t>::get( _a1, a0 ),
				free_standing_call_args::resolve_arg<1, a2_t, fa0_t>::get( _a2, a0 ),
				free_standing_call_args::resolve_arg<1, a3_t, fa0_t>::get( _a3, a0 ),
				free_standing_call_args::resolve_arg<1, a4_t, fa0_t>::get( _a4, a0 ),
				free_standing_call_args::resolve_arg<1, a5_t, fa0_t>::get( _a5, a0 ),
				free_standing_call_args::resolve_arg<1, a6_t, fa0_t>::get( _a6, a0 ),
				free_standing_call_args::resolve_arg<1, a7_t, fa0_t>::get( _a7, a0 ) ) ); }
	virtual return_t do_invoke( fa0_t a0 ) const { return ( (base_t::_call)(
				free_standing_call_args::resolve_arg<1, a0_t, fa0_t>::get( _a0, a0 ),
				free_standing_call_args::resolve_arg<1, a1_t, fa0_t>::get( _a1, a0 ),
				free_standing_call_args::resolve_arg<1, a2_t, fa0_t>::get( _a2, a0 ),
				free_standing_call_args::resolve_arg<1, a3_t, fa0_t>::get( _a3, a0 ),
				free_standing_call_args::resolve_arg<1, a4_t, fa0_t>::get( _a4, a0 ),
				free_standing_call_args::resolve_arg<1, a5_t, fa0_t>::get( _a5, a0 ),
				free_standing_call_args::resolve_arg<1, a6_t, fa0_t>::get( _a6, a0 ),
				free_standing_call_args::resolve_arg<1, a7_t, fa0_t>::get( _a7, a0 ) ) ); }
	};

template<typename descriptor, typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t, typename a6_t, typename a7_t,
	typename a8_t>
class HCall<1, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t,
	trait::no_type>
		: public HCallBase<1, descriptor, return_t, CALL_t, a0_t,
	a1_t, a2_t, a3_t, a4_t,
	a5_t, a6_t, a7_t, a8_t,
	trait::no_type, trait::no_type>
	{
	typedef HCallBase<1, descriptor, return_t, CALL_t, a0_t,
	a1_t, a2_t, a3_t, a4_t,
	a5_t, a6_t, a7_t, a8_t,
	trait::no_type, trait::no_type> base_t;
	typedef typename descriptor::type::sorted_real_args::a0_t fa0_t;
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
	HCall( CALL_t call_, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7, a8_t a8 )
		: base_t( call_ ), _a0( a0 ), _a1( a1 ), _a2( a2 ), _a3( a3 ), _a4( a4 ), _a5( a5 ), _a6( a6 ), _a7( a7 ), _a8( a8 ) {}
	virtual return_t do_invoke( fa0_t a0 ) { return ( (base_t::_call)(
				free_standing_call_args::resolve_arg<1, a0_t, fa0_t>::get( _a0, a0 ),
				free_standing_call_args::resolve_arg<1, a1_t, fa0_t>::get( _a1, a0 ),
				free_standing_call_args::resolve_arg<1, a2_t, fa0_t>::get( _a2, a0 ),
				free_standing_call_args::resolve_arg<1, a3_t, fa0_t>::get( _a3, a0 ),
				free_standing_call_args::resolve_arg<1, a4_t, fa0_t>::get( _a4, a0 ),
				free_standing_call_args::resolve_arg<1, a5_t, fa0_t>::get( _a5, a0 ),
				free_standing_call_args::resolve_arg<1, a6_t, fa0_t>::get( _a6, a0 ),
				free_standing_call_args::resolve_arg<1, a7_t, fa0_t>::get( _a7, a0 ),
				free_standing_call_args::resolve_arg<1, a8_t, fa0_t>::get( _a8, a0 ) ) ); }
	virtual return_t do_invoke( fa0_t a0 ) const { return ( (base_t::_call)(
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

template<typename descriptor, typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t, typename a6_t, typename a7_t,
	typename a8_t, typename a9_t>
class HCall<1, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t>
		: public HCallBase<1, descriptor, return_t, CALL_t, a0_t,
	a1_t, a2_t, a3_t, a4_t,
	a5_t, a6_t, a7_t, a8_t,
	a9_t, trait::no_type>
	{
	typedef HCallBase<1, descriptor, return_t, CALL_t, a0_t,
	a1_t, a2_t, a3_t, a4_t,
	a5_t, a6_t, a7_t, a8_t,
	a9_t, trait::no_type> base_t;
	typedef typename descriptor::type::sorted_real_args::a0_t fa0_t;
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
	HCall( CALL_t call_, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7, a8_t a8, a9_t a9 )
		: base_t( call_ ), _a0( a0 ), _a1( a1 ), _a2( a2 ), _a3( a3 ), _a4( a4 ), _a5( a5 ), _a6( a6 ), _a7( a7 ), _a8( a8 ), _a9( a9 ) {}
	virtual return_t do_invoke( fa0_t a0 ) { return ( (base_t::_call)(
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
	virtual return_t do_invoke( fa0_t a0 ) const { return ( (base_t::_call)(
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

template<typename descriptor, typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t, typename a6_t, typename a7_t,
	typename a8_t, typename a9_t, typename a10_t>
class HCall<1, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t, a10_t>
		: public HCallBase<1, descriptor, return_t, CALL_t, a0_t,
	a1_t, a2_t, a3_t, a4_t,
	a5_t, a6_t, a7_t, a8_t,
	a9_t, a10_t>
	{
	typedef HCallBase<1, descriptor, return_t, CALL_t, a0_t,
	a1_t, a2_t, a3_t, a4_t,
	a5_t, a6_t, a7_t, a8_t,
	a9_t, a10_t> base_t;
	typedef typename descriptor::type::sorted_real_args::a0_t fa0_t;
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
	HCall( CALL_t call_, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7, a8_t a8, a9_t a9, a10_t a10)
		: base_t( call_ ), _a0( a0 ), _a1( a1 ), _a2( a2 ), _a3( a3 ), _a4( a4 ), _a5( a5 ), _a6( a6 ), _a7( a7 ), _a8( a8 ), _a9( a9 ), _a10( a10 ) {}
	virtual return_t do_invoke( fa0_t a0 ) { return ( (base_t::_call)(
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
	virtual return_t do_invoke( fa0_t a0 ) const { return ( (base_t::_call)(
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

template<typename descriptor, typename return_t, typename CALL_t, typename a0_t, typename a1_t>
class HCall<2, descriptor, return_t, CALL_t, a0_t, a1_t,
	trait::no_type, trait::no_type, trait::no_type, trait::no_type,
	trait::no_type, trait::no_type, trait::no_type, trait::no_type>
		: public HCallBase<2, descriptor, return_t, CALL_t, a0_t,
	a1_t, trait::no_type, trait::no_type, trait::no_type,
	trait::no_type, trait::no_type, trait::no_type, trait::no_type,
	trait::no_type, trait::no_type>
	{
	typedef HCallBase<2, descriptor, return_t, CALL_t, a0_t,
	a1_t, trait::no_type, trait::no_type, trait::no_type,
	trait::no_type, trait::no_type, trait::no_type, trait::no_type,
	trait::no_type, trait::no_type> base_t;
	typedef typename descriptor::type::sorted_real_args::a0_t fa0_t;
	typedef typename descriptor::type::sorted_real_args::a1_t fa1_t;
	a0_t _a0;
	a1_t _a1;
public:
	HCall( CALL_t call_, a0_t a0, a1_t a1 )
		: base_t( call_ ), _a0( a0 ), _a1( a1 ) {}
	virtual return_t do_invoke( fa0_t a0, fa1_t a1 ) { return ( (base_t::_call)(
				free_standing_call_args::resolve_arg<2, a0_t, fa0_t, fa1_t>::get( _a0, a0, a1 ),
				free_standing_call_args::resolve_arg<2, a1_t, fa0_t, fa1_t>::get( _a1, a0, a1 ) ) ); }
	virtual return_t do_invoke( fa0_t a0, fa1_t a1 ) const { return ( (base_t::_call)(
				free_standing_call_args::resolve_arg<2, a0_t, fa0_t, fa1_t>::get( _a0, a0, a1 ),
				free_standing_call_args::resolve_arg<2, a1_t, fa0_t, fa1_t>::get( _a1, a0, a1 ) ) ); }
	};

template<typename descriptor, typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t>
class HCall<2, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t,
	trait::no_type, trait::no_type, trait::no_type, trait::no_type,
	trait::no_type, trait::no_type, trait::no_type>
		: public HCallBase<2, descriptor, return_t, CALL_t, a0_t,
	a1_t, a2_t, trait::no_type, trait::no_type,
	trait::no_type, trait::no_type, trait::no_type, trait::no_type,
	trait::no_type, trait::no_type>
	{
	typedef HCallBase<2, descriptor, return_t, CALL_t, a0_t,
	a1_t, a2_t, trait::no_type, trait::no_type,
	trait::no_type, trait::no_type, trait::no_type, trait::no_type,
	trait::no_type, trait::no_type> base_t;
	typedef typename descriptor::type::sorted_real_args::a0_t fa0_t;
	typedef typename descriptor::type::sorted_real_args::a1_t fa1_t;
	a0_t _a0;
	a1_t _a1;
	a2_t _a2;
public:
	HCall( CALL_t call_, a0_t a0, a1_t a1, a2_t a2 )
		: base_t( call_ ), _a0( a0 ), _a1( a1 ), _a2( a2 ) {}
	virtual return_t do_invoke( fa0_t a0, fa1_t a1 ) { return ( (base_t::_call)(
				free_standing_call_args::resolve_arg<2, a0_t, fa0_t, fa1_t>::get( _a0, a0, a1 ),
				free_standing_call_args::resolve_arg<2, a1_t, fa0_t, fa1_t>::get( _a1, a0, a1 ),
				free_standing_call_args::resolve_arg<2, a2_t, fa0_t, fa1_t>::get( _a2, a0, a1 ) ) ); }
	virtual return_t do_invoke( fa0_t a0, fa1_t a1 ) const { return ( (base_t::_call)(
				free_standing_call_args::resolve_arg<2, a0_t, fa0_t, fa1_t>::get( _a0, a0, a1 ),
				free_standing_call_args::resolve_arg<2, a1_t, fa0_t, fa1_t>::get( _a1, a0, a1 ),
				free_standing_call_args::resolve_arg<2, a2_t, fa0_t, fa1_t>::get( _a2, a0, a1 ) ) ); }
	};

template<typename descriptor, typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t>
class HCall<2, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t,
	trait::no_type, trait::no_type, trait::no_type, trait::no_type,
	trait::no_type, trait::no_type>
		: public HCallBase<2, descriptor, return_t, CALL_t, a0_t,
	a1_t, a2_t, a3_t, trait::no_type,
	trait::no_type, trait::no_type, trait::no_type, trait::no_type,
	trait::no_type, trait::no_type>
	{
	typedef HCallBase<2, descriptor, return_t, CALL_t, a0_t,
	a1_t, a2_t, a3_t, trait::no_type,
	trait::no_type, trait::no_type, trait::no_type, trait::no_type,
	trait::no_type, trait::no_type> base_t;
	typedef typename descriptor::type::sorted_real_args::a0_t fa0_t;
	typedef typename descriptor::type::sorted_real_args::a1_t fa1_t;
	a0_t _a0;
	a1_t _a1;
	a2_t _a2;
	a3_t _a3;
public:
	HCall( CALL_t call_, a0_t a0, a1_t a1, a2_t a2, a3_t a3 )
		: base_t( call_ ), _a0( a0 ), _a1( a1 ), _a2( a2 ), _a3( a3 ) {}
	virtual return_t do_invoke( fa0_t a0, fa1_t a1 ) { return ( (base_t::_call)(
				free_standing_call_args::resolve_arg<2, a0_t, fa0_t, fa1_t>::get( _a0, a0, a1 ),
				free_standing_call_args::resolve_arg<2, a1_t, fa0_t, fa1_t>::get( _a1, a0, a1 ),
				free_standing_call_args::resolve_arg<2, a2_t, fa0_t, fa1_t>::get( _a2, a0, a1 ),
				free_standing_call_args::resolve_arg<2, a3_t, fa0_t, fa1_t>::get( _a3, a0, a1 ) ) ); }
	virtual return_t do_invoke( fa0_t a0, fa1_t a1 ) const { return ( (base_t::_call)(
				free_standing_call_args::resolve_arg<2, a0_t, fa0_t, fa1_t>::get( _a0, a0, a1 ),
				free_standing_call_args::resolve_arg<2, a1_t, fa0_t, fa1_t>::get( _a1, a0, a1 ),
				free_standing_call_args::resolve_arg<2, a2_t, fa0_t, fa1_t>::get( _a2, a0, a1 ),
				free_standing_call_args::resolve_arg<2, a3_t, fa0_t, fa1_t>::get( _a3, a0, a1 ) ) ); }
	};

template<typename descriptor, typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t>
class HCall<2, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t,
	trait::no_type, trait::no_type, trait::no_type, trait::no_type,
	trait::no_type>
		: public HCallBase<2, descriptor, return_t, CALL_t, a0_t,
	a1_t, a2_t, a3_t, a4_t,
	trait::no_type, trait::no_type, trait::no_type, trait::no_type,
	trait::no_type, trait::no_type>
	{
	typedef HCallBase<2, descriptor, return_t, CALL_t, a0_t,
	a1_t, a2_t, a3_t, a4_t,
	trait::no_type, trait::no_type, trait::no_type, trait::no_type,
	trait::no_type, trait::no_type> base_t;
	typedef typename descriptor::type::sorted_real_args::a0_t fa0_t;
	typedef typename descriptor::type::sorted_real_args::a1_t fa1_t;
	a0_t _a0;
	a1_t _a1;
	a2_t _a2;
	a3_t _a3;
	a4_t _a4;
public:
	HCall( CALL_t call_, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4 )
		: base_t( call_ ), _a0( a0 ), _a1( a1 ), _a2( a2 ), _a3( a3 ), _a4( a4 ) {}
	virtual return_t do_invoke( fa0_t a0, fa1_t a1 ) { return ( (base_t::_call)(
				free_standing_call_args::resolve_arg<2, a0_t, fa0_t, fa1_t>::get( _a0, a0, a1 ),
				free_standing_call_args::resolve_arg<2, a1_t, fa0_t, fa1_t>::get( _a1, a0, a1 ),
				free_standing_call_args::resolve_arg<2, a2_t, fa0_t, fa1_t>::get( _a2, a0, a1 ),
				free_standing_call_args::resolve_arg<2, a3_t, fa0_t, fa1_t>::get( _a3, a0, a1 ),
				free_standing_call_args::resolve_arg<2, a4_t, fa0_t, fa1_t>::get( _a4, a0, a1 ) ) ); }
	virtual return_t do_invoke( fa0_t a0, fa1_t a1 ) const { return ( (base_t::_call)(
				free_standing_call_args::resolve_arg<2, a0_t, fa0_t, fa1_t>::get( _a0, a0, a1 ),
				free_standing_call_args::resolve_arg<2, a1_t, fa0_t, fa1_t>::get( _a1, a0, a1 ),
				free_standing_call_args::resolve_arg<2, a2_t, fa0_t, fa1_t>::get( _a2, a0, a1 ),
				free_standing_call_args::resolve_arg<2, a3_t, fa0_t, fa1_t>::get( _a3, a0, a1 ),
				free_standing_call_args::resolve_arg<2, a4_t, fa0_t, fa1_t>::get( _a4, a0, a1 ) ) ); }
	};

template<typename descriptor, typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t>
class HCall<2, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t,
	trait::no_type, trait::no_type, trait::no_type, trait::no_type>
		: public HCallBase<2, descriptor, return_t, CALL_t, a0_t,
	a1_t, a2_t, a3_t, a4_t,
	a5_t, trait::no_type, trait::no_type, trait::no_type,
	trait::no_type, trait::no_type>
	{
	typedef HCallBase<2, descriptor, return_t, CALL_t, a0_t,
	a1_t, a2_t, a3_t, a4_t,
	a5_t, trait::no_type, trait::no_type, trait::no_type,
	trait::no_type, trait::no_type> base_t;
	typedef typename descriptor::type::sorted_real_args::a0_t fa0_t;
	typedef typename descriptor::type::sorted_real_args::a1_t fa1_t;
	a0_t _a0;
	a1_t _a1;
	a2_t _a2;
	a3_t _a3;
	a4_t _a4;
	a5_t _a5;
public:
	HCall( CALL_t call_, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5 )
		: base_t( call_ ), _a0( a0 ), _a1( a1 ), _a2( a2 ), _a3( a3 ), _a4( a4 ), _a5( a5 ) {}
	virtual return_t do_invoke( fa0_t a0, fa1_t a1 ) { return ( (base_t::_call)(
				free_standing_call_args::resolve_arg<2, a0_t, fa0_t, fa1_t>::get( _a0, a0, a1 ),
				free_standing_call_args::resolve_arg<2, a1_t, fa0_t, fa1_t>::get( _a1, a0, a1 ),
				free_standing_call_args::resolve_arg<2, a2_t, fa0_t, fa1_t>::get( _a2, a0, a1 ),
				free_standing_call_args::resolve_arg<2, a3_t, fa0_t, fa1_t>::get( _a3, a0, a1 ),
				free_standing_call_args::resolve_arg<2, a4_t, fa0_t, fa1_t>::get( _a4, a0, a1 ),
				free_standing_call_args::resolve_arg<2, a5_t, fa0_t, fa1_t>::get( _a5, a0, a1 ) ) ); }
	virtual return_t do_invoke( fa0_t a0, fa1_t a1 ) const { return ( (base_t::_call)(
				free_standing_call_args::resolve_arg<2, a0_t, fa0_t, fa1_t>::get( _a0, a0, a1 ),
				free_standing_call_args::resolve_arg<2, a1_t, fa0_t, fa1_t>::get( _a1, a0, a1 ),
				free_standing_call_args::resolve_arg<2, a2_t, fa0_t, fa1_t>::get( _a2, a0, a1 ),
				free_standing_call_args::resolve_arg<2, a3_t, fa0_t, fa1_t>::get( _a3, a0, a1 ),
				free_standing_call_args::resolve_arg<2, a4_t, fa0_t, fa1_t>::get( _a4, a0, a1 ),
				free_standing_call_args::resolve_arg<2, a5_t, fa0_t, fa1_t>::get( _a5, a0, a1 ) ) ); }
	};

template<typename descriptor, typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t, typename a6_t>
class HCall<2, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t,
	trait::no_type, trait::no_type, trait::no_type>
		: public HCallBase<2, descriptor, return_t, CALL_t, a0_t,
	a1_t, a2_t, a3_t, a4_t,
	a5_t, a6_t, trait::no_type, trait::no_type,
	trait::no_type, trait::no_type>
	{
	typedef HCallBase<2, descriptor, return_t, CALL_t, a0_t,
	a1_t, a2_t, a3_t, a4_t,
	a5_t, a6_t, trait::no_type, trait::no_type,
	trait::no_type, trait::no_type> base_t;
	typedef typename descriptor::type::sorted_real_args::a0_t fa0_t;
	typedef typename descriptor::type::sorted_real_args::a1_t fa1_t;
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
		: base_t( call_ ), _a0( a0 ), _a1( a1 ), _a2( a2 ), _a3( a3 ), _a4( a4 ), _a5( a5 ), _a6( a6 ) {}
	virtual return_t do_invoke( fa0_t a0, fa1_t a1 ) { return ( (base_t::_call)(
				free_standing_call_args::resolve_arg<2, a0_t, fa0_t, fa1_t>::get( _a0, a0, a1 ),
				free_standing_call_args::resolve_arg<2, a1_t, fa0_t, fa1_t>::get( _a1, a0, a1 ),
				free_standing_call_args::resolve_arg<2, a2_t, fa0_t, fa1_t>::get( _a2, a0, a1 ),
				free_standing_call_args::resolve_arg<2, a3_t, fa0_t, fa1_t>::get( _a3, a0, a1 ),
				free_standing_call_args::resolve_arg<2, a4_t, fa0_t, fa1_t>::get( _a4, a0, a1 ),
				free_standing_call_args::resolve_arg<2, a5_t, fa0_t, fa1_t>::get( _a5, a0, a1 ),
				free_standing_call_args::resolve_arg<2, a6_t, fa0_t, fa1_t>::get( _a6, a0, a1 ) ) ); }
	virtual return_t do_invoke( fa0_t a0, fa1_t a1 ) const { return ( (base_t::_call)(
				free_standing_call_args::resolve_arg<2, a0_t, fa0_t, fa1_t>::get( _a0, a0, a1 ),
				free_standing_call_args::resolve_arg<2, a1_t, fa0_t, fa1_t>::get( _a1, a0, a1 ),
				free_standing_call_args::resolve_arg<2, a2_t, fa0_t, fa1_t>::get( _a2, a0, a1 ),
				free_standing_call_args::resolve_arg<2, a3_t, fa0_t, fa1_t>::get( _a3, a0, a1 ),
				free_standing_call_args::resolve_arg<2, a4_t, fa0_t, fa1_t>::get( _a4, a0, a1 ),
				free_standing_call_args::resolve_arg<2, a5_t, fa0_t, fa1_t>::get( _a5, a0, a1 ),
				free_standing_call_args::resolve_arg<2, a6_t, fa0_t, fa1_t>::get( _a6, a0, a1 ) ) ); }
	};

template<typename descriptor, typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t, typename a6_t, typename a7_t>
class HCall<2, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t,
	trait::no_type, trait::no_type>
		: public HCallBase<2, descriptor, return_t, CALL_t, a0_t,
	a1_t, a2_t, a3_t, a4_t,
	a5_t, a6_t, a7_t, trait::no_type,
	trait::no_type, trait::no_type>
	{
	typedef HCallBase<2, descriptor, return_t, CALL_t, a0_t,
	a1_t, a2_t, a3_t, a4_t,
	a5_t, a6_t, a7_t, trait::no_type,
	trait::no_type, trait::no_type> base_t;
	typedef typename descriptor::type::sorted_real_args::a0_t fa0_t;
	typedef typename descriptor::type::sorted_real_args::a1_t fa1_t;
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
		: base_t( call_ ), _a0( a0 ), _a1( a1 ), _a2( a2 ), _a3( a3 ), _a4( a4 ), _a5( a5 ), _a6( a6 ), _a7( a7 ) {}
	virtual return_t do_invoke( fa0_t a0, fa1_t a1 ) { return ( (base_t::_call)(
				free_standing_call_args::resolve_arg<2, a0_t, fa0_t, fa1_t>::get( _a0, a0, a1 ),
				free_standing_call_args::resolve_arg<2, a1_t, fa0_t, fa1_t>::get( _a1, a0, a1 ),
				free_standing_call_args::resolve_arg<2, a2_t, fa0_t, fa1_t>::get( _a2, a0, a1 ),
				free_standing_call_args::resolve_arg<2, a3_t, fa0_t, fa1_t>::get( _a3, a0, a1 ),
				free_standing_call_args::resolve_arg<2, a4_t, fa0_t, fa1_t>::get( _a4, a0, a1 ),
				free_standing_call_args::resolve_arg<2, a5_t, fa0_t, fa1_t>::get( _a5, a0, a1 ),
				free_standing_call_args::resolve_arg<2, a6_t, fa0_t, fa1_t>::get( _a6, a0, a1 ),
				free_standing_call_args::resolve_arg<2, a7_t, fa0_t, fa1_t>::get( _a7, a0, a1 ) ) ); }
	virtual return_t do_invoke( fa0_t a0, fa1_t a1 ) const { return ( (base_t::_call)(
				free_standing_call_args::resolve_arg<2, a0_t, fa0_t, fa1_t>::get( _a0, a0, a1 ),
				free_standing_call_args::resolve_arg<2, a1_t, fa0_t, fa1_t>::get( _a1, a0, a1 ),
				free_standing_call_args::resolve_arg<2, a2_t, fa0_t, fa1_t>::get( _a2, a0, a1 ),
				free_standing_call_args::resolve_arg<2, a3_t, fa0_t, fa1_t>::get( _a3, a0, a1 ),
				free_standing_call_args::resolve_arg<2, a4_t, fa0_t, fa1_t>::get( _a4, a0, a1 ),
				free_standing_call_args::resolve_arg<2, a5_t, fa0_t, fa1_t>::get( _a5, a0, a1 ),
				free_standing_call_args::resolve_arg<2, a6_t, fa0_t, fa1_t>::get( _a6, a0, a1 ),
				free_standing_call_args::resolve_arg<2, a7_t, fa0_t, fa1_t>::get( _a7, a0, a1 ) ) ); }
	};

template<typename descriptor, typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t, typename a6_t, typename a7_t,
	typename a8_t>
class HCall<2, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t,
	trait::no_type>
		: public HCallBase<2, descriptor, return_t, CALL_t, a0_t,
	a1_t, a2_t, a3_t, a4_t,
	a5_t, a6_t, a7_t, a8_t,
	trait::no_type, trait::no_type>
	{
	typedef HCallBase<2, descriptor, return_t, CALL_t, a0_t,
	a1_t, a2_t, a3_t, a4_t,
	a5_t, a6_t, a7_t, a8_t,
	trait::no_type, trait::no_type> base_t;
	typedef typename descriptor::type::sorted_real_args::a0_t fa0_t;
	typedef typename descriptor::type::sorted_real_args::a1_t fa1_t;
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
	HCall( CALL_t call_, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7, a8_t a8 )
		: base_t( call_ ), _a0( a0 ), _a1( a1 ), _a2( a2 ), _a3( a3 ), _a4( a4 ), _a5( a5 ), _a6( a6 ), _a7( a7 ), _a8( a8 ) {}
	virtual return_t do_invoke( fa0_t a0, fa1_t a1 ) { return ( (base_t::_call)(
				free_standing_call_args::resolve_arg<2, a0_t, fa0_t, fa1_t>::get( _a0, a0, a1 ),
				free_standing_call_args::resolve_arg<2, a1_t, fa0_t, fa1_t>::get( _a1, a0, a1 ),
				free_standing_call_args::resolve_arg<2, a2_t, fa0_t, fa1_t>::get( _a2, a0, a1 ),
				free_standing_call_args::resolve_arg<2, a3_t, fa0_t, fa1_t>::get( _a3, a0, a1 ),
				free_standing_call_args::resolve_arg<2, a4_t, fa0_t, fa1_t>::get( _a4, a0, a1 ),
				free_standing_call_args::resolve_arg<2, a5_t, fa0_t, fa1_t>::get( _a5, a0, a1 ),
				free_standing_call_args::resolve_arg<2, a6_t, fa0_t, fa1_t>::get( _a6, a0, a1 ),
				free_standing_call_args::resolve_arg<2, a7_t, fa0_t, fa1_t>::get( _a7, a0, a1 ),
				free_standing_call_args::resolve_arg<2, a8_t, fa0_t, fa1_t>::get( _a8, a0, a1 ) ) ); }
	virtual return_t do_invoke( fa0_t a0, fa1_t a1 ) const { return ( (base_t::_call)(
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

template<typename descriptor, typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t, typename a6_t, typename a7_t,
	typename a8_t, typename a9_t>
class HCall<2, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t>
		: public HCallBase<2, descriptor, return_t, CALL_t, a0_t,
	a1_t, a2_t, a3_t, a4_t,
	a5_t, a6_t, a7_t, a8_t,
	a9_t, trait::no_type>
	{
	typedef HCallBase<2, descriptor, return_t, CALL_t, a0_t,
	a1_t, a2_t, a3_t, a4_t,
	a5_t, a6_t, a7_t, a8_t,
	a9_t, trait::no_type> base_t;
	typedef typename descriptor::type::sorted_real_args::a0_t fa0_t;
	typedef typename descriptor::type::sorted_real_args::a1_t fa1_t;
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
	HCall( CALL_t call_, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7, a8_t a8, a9_t a9 )
		: base_t( call_ ), _a0( a0 ), _a1( a1 ), _a2( a2 ), _a3( a3 ), _a4( a4 ), _a5( a5 ), _a6( a6 ), _a7( a7 ), _a8( a8 ), _a9( a9 ) {}
	virtual return_t do_invoke( fa0_t a0, fa1_t a1 ) { return ( (base_t::_call)(
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
	virtual return_t do_invoke( fa0_t a0, fa1_t a1 ) const { return ( (base_t::_call)(
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

template<typename descriptor, typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t, typename a6_t, typename a7_t,
	typename a8_t, typename a9_t, typename a10_t>
class HCall<2, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t, a10_t>
		: public HCallBase<2, descriptor, return_t, CALL_t, a0_t,
	a1_t, a2_t, a3_t, a4_t,
	a5_t, a6_t, a7_t, a8_t,
	a9_t, a10_t>
	{
	typedef HCallBase<2, descriptor, return_t, CALL_t, a0_t,
	a1_t, a2_t, a3_t, a4_t,
	a5_t, a6_t, a7_t, a8_t,
	a9_t, a10_t> base_t;
	typedef typename descriptor::type::sorted_real_args::a0_t fa0_t;
	typedef typename descriptor::type::sorted_real_args::a1_t fa1_t;
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
	HCall( CALL_t call_, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7, a8_t a8, a9_t a9, a10_t a10 )
		: base_t( call_ ), _a0( a0 ), _a1( a1 ), _a2( a2 ), _a3( a3 ), _a4( a4 ), _a5( a5 ), _a6( a6 ), _a7( a7 ), _a8( a8 ), _a9( a9 ), _a10( a10 ) {}
	virtual return_t do_invoke( fa0_t a0, fa1_t a1 ) { return ( (base_t::_call)(
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
	virtual return_t do_invoke( fa0_t a0, fa1_t a1 ) const { return ( (base_t::_call)(
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

template<typename descriptor, typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t>
class HCall<3, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t,
	trait::no_type, trait::no_type, trait::no_type, trait::no_type,
	trait::no_type, trait::no_type, trait::no_type>
		: public HCallBase<3, descriptor, return_t, CALL_t, a0_t,
	a1_t, a2_t, trait::no_type, trait::no_type,
	trait::no_type, trait::no_type, trait::no_type, trait::no_type,
	trait::no_type, trait::no_type>
	{
	typedef HCallBase<3, descriptor, return_t, CALL_t, a0_t,
	a1_t, a2_t, trait::no_type, trait::no_type,
	trait::no_type, trait::no_type, trait::no_type, trait::no_type,
	trait::no_type, trait::no_type> base_t;
	typedef typename descriptor::type::sorted_real_args::a0_t fa0_t;
	typedef typename descriptor::type::sorted_real_args::a1_t fa1_t;
	typedef typename descriptor::type::sorted_real_args::a2_t fa2_t;
	a0_t _a0;
	a1_t _a1;
	a2_t _a2;
public:
	HCall( CALL_t call_, a0_t a0, a1_t a1, a2_t a2 )
		: base_t( call_ ), _a0( a0 ), _a1( a1 ), _a2( a2 ) {}
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2 ) { return ( (base_t::_call)(
				free_standing_call_args::resolve_arg<3, a0_t, fa0_t, fa1_t, fa2_t>::get( _a0, a0, a1, a2 ),
				free_standing_call_args::resolve_arg<3, a1_t, fa0_t, fa1_t, fa2_t>::get( _a1, a0, a1, a2 ),
				free_standing_call_args::resolve_arg<3, a2_t, fa0_t, fa1_t, fa2_t>::get( _a2, a0, a1, a2 ) ) ); }
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2 ) const { return ( (base_t::_call)(
				free_standing_call_args::resolve_arg<3, a0_t, fa0_t, fa1_t, fa2_t>::get( _a0, a0, a1, a2 ),
				free_standing_call_args::resolve_arg<3, a1_t, fa0_t, fa1_t, fa2_t>::get( _a1, a0, a1, a2 ),
				free_standing_call_args::resolve_arg<3, a2_t, fa0_t, fa1_t, fa2_t>::get( _a2, a0, a1, a2 ) ) ); }
	};

template<typename descriptor, typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t>
class HCall<3, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t,
	trait::no_type, trait::no_type, trait::no_type, trait::no_type,
	trait::no_type, trait::no_type>
		: public HCallBase<3, descriptor, return_t, CALL_t, a0_t,
	a1_t, a2_t, a3_t, trait::no_type,
	trait::no_type, trait::no_type, trait::no_type, trait::no_type,
	trait::no_type, trait::no_type>
	{
	typedef HCallBase<3, descriptor, return_t, CALL_t, a0_t,
	a1_t, a2_t, a3_t, trait::no_type,
	trait::no_type, trait::no_type, trait::no_type, trait::no_type,
	trait::no_type, trait::no_type> base_t;
	typedef typename descriptor::type::sorted_real_args::a0_t fa0_t;
	typedef typename descriptor::type::sorted_real_args::a1_t fa1_t;
	typedef typename descriptor::type::sorted_real_args::a2_t fa2_t;
	a0_t _a0;
	a1_t _a1;
	a2_t _a2;
	a3_t _a3;
public:
	HCall( CALL_t call_, a0_t a0, a1_t a1, a2_t a2, a3_t a3 )
		: base_t( call_ ), _a0( a0 ), _a1( a1 ), _a2( a2 ), _a3( a3 ) {}
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2 ) { return ( (base_t::_call)(
				free_standing_call_args::resolve_arg<3, a0_t, fa0_t, fa1_t, fa2_t>::get( _a0, a0, a1, a2 ),
				free_standing_call_args::resolve_arg<3, a1_t, fa0_t, fa1_t, fa2_t>::get( _a1, a0, a1, a2 ),
				free_standing_call_args::resolve_arg<3, a2_t, fa0_t, fa1_t, fa2_t>::get( _a2, a0, a1, a2 ),
				free_standing_call_args::resolve_arg<3, a3_t, fa0_t, fa1_t, fa2_t>::get( _a3, a0, a1, a2 ) ) ); }
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2 ) const { return ( (base_t::_call)(
				free_standing_call_args::resolve_arg<3, a0_t, fa0_t, fa1_t, fa2_t>::get( _a0, a0, a1, a2 ),
				free_standing_call_args::resolve_arg<3, a1_t, fa0_t, fa1_t, fa2_t>::get( _a1, a0, a1, a2 ),
				free_standing_call_args::resolve_arg<3, a2_t, fa0_t, fa1_t, fa2_t>::get( _a2, a0, a1, a2 ),
				free_standing_call_args::resolve_arg<3, a3_t, fa0_t, fa1_t, fa2_t>::get( _a3, a0, a1, a2 ) ) ); }
	};

template<typename descriptor, typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t>
class HCall<3, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t,
	trait::no_type, trait::no_type, trait::no_type, trait::no_type,
	trait::no_type>
		: public HCallBase<3, descriptor, return_t, CALL_t, a0_t,
	a1_t, a2_t, a3_t, a4_t,
	trait::no_type, trait::no_type, trait::no_type, trait::no_type,
	trait::no_type, trait::no_type>
	{
	typedef HCallBase<3, descriptor, return_t, CALL_t, a0_t,
	a1_t, a2_t, a3_t, a4_t,
	trait::no_type, trait::no_type, trait::no_type, trait::no_type,
	trait::no_type, trait::no_type> base_t;
	typedef typename descriptor::type::sorted_real_args::a0_t fa0_t;
	typedef typename descriptor::type::sorted_real_args::a1_t fa1_t;
	typedef typename descriptor::type::sorted_real_args::a2_t fa2_t;
	a0_t _a0;
	a1_t _a1;
	a2_t _a2;
	a3_t _a3;
	a4_t _a4;
public:
	HCall( CALL_t call_, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4 )
		: base_t( call_ ), _a0( a0 ), _a1( a1 ), _a2( a2 ), _a3( a3 ), _a4( a4 ) {}
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2 ) { return ( (base_t::_call)(
				free_standing_call_args::resolve_arg<3, a0_t, fa0_t, fa1_t, fa2_t>::get( _a0, a0, a1, a2 ),
				free_standing_call_args::resolve_arg<3, a1_t, fa0_t, fa1_t, fa2_t>::get( _a1, a0, a1, a2 ),
				free_standing_call_args::resolve_arg<3, a2_t, fa0_t, fa1_t, fa2_t>::get( _a2, a0, a1, a2 ),
				free_standing_call_args::resolve_arg<3, a3_t, fa0_t, fa1_t, fa2_t>::get( _a3, a0, a1, a2 ),
				free_standing_call_args::resolve_arg<3, a4_t, fa0_t, fa1_t, fa2_t>::get( _a4, a0, a1, a2 ) ) ); }
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2 ) const { return ( (base_t::_call)(
				free_standing_call_args::resolve_arg<3, a0_t, fa0_t, fa1_t, fa2_t>::get( _a0, a0, a1, a2 ),
				free_standing_call_args::resolve_arg<3, a1_t, fa0_t, fa1_t, fa2_t>::get( _a1, a0, a1, a2 ),
				free_standing_call_args::resolve_arg<3, a2_t, fa0_t, fa1_t, fa2_t>::get( _a2, a0, a1, a2 ),
				free_standing_call_args::resolve_arg<3, a3_t, fa0_t, fa1_t, fa2_t>::get( _a3, a0, a1, a2 ),
				free_standing_call_args::resolve_arg<3, a4_t, fa0_t, fa1_t, fa2_t>::get( _a4, a0, a1, a2 ) ) ); }
	};

template<typename descriptor, typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t>
class HCall<3, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t,
	trait::no_type, trait::no_type, trait::no_type, trait::no_type>
		: public HCallBase<3, descriptor, return_t, CALL_t, a0_t,
	a1_t, a2_t, a3_t, a4_t,
	a5_t, trait::no_type, trait::no_type, trait::no_type,
	trait::no_type, trait::no_type>
	{
	typedef HCallBase<3, descriptor, return_t, CALL_t, a0_t,
	a1_t, a2_t, a3_t, a4_t,
	a5_t, trait::no_type, trait::no_type, trait::no_type,
	trait::no_type, trait::no_type> base_t;
	typedef typename descriptor::type::sorted_real_args::a0_t fa0_t;
	typedef typename descriptor::type::sorted_real_args::a1_t fa1_t;
	typedef typename descriptor::type::sorted_real_args::a2_t fa2_t;
	a0_t _a0;
	a1_t _a1;
	a2_t _a2;
	a3_t _a3;
	a4_t _a4;
	a5_t _a5;
public:
	HCall( CALL_t call_, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5 )
		: base_t( call_ ), _a0( a0 ), _a1( a1 ), _a2( a2 ), _a3( a3 ), _a4( a4 ), _a5( a5 ) {}
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2 ) { return ( (base_t::_call)(
				free_standing_call_args::resolve_arg<3, a0_t, fa0_t, fa1_t, fa2_t>::get( _a0, a0, a1, a2 ),
				free_standing_call_args::resolve_arg<3, a1_t, fa0_t, fa1_t, fa2_t>::get( _a1, a0, a1, a2 ),
				free_standing_call_args::resolve_arg<3, a2_t, fa0_t, fa1_t, fa2_t>::get( _a2, a0, a1, a2 ),
				free_standing_call_args::resolve_arg<3, a3_t, fa0_t, fa1_t, fa2_t>::get( _a3, a0, a1, a2 ),
				free_standing_call_args::resolve_arg<3, a4_t, fa0_t, fa1_t, fa2_t>::get( _a4, a0, a1, a2 ),
				free_standing_call_args::resolve_arg<3, a5_t, fa0_t, fa1_t, fa2_t>::get( _a5, a0, a1, a2 ) ) ); }
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2 ) const { return ( (base_t::_call)(
				free_standing_call_args::resolve_arg<3, a0_t, fa0_t, fa1_t, fa2_t>::get( _a0, a0, a1, a2 ),
				free_standing_call_args::resolve_arg<3, a1_t, fa0_t, fa1_t, fa2_t>::get( _a1, a0, a1, a2 ),
				free_standing_call_args::resolve_arg<3, a2_t, fa0_t, fa1_t, fa2_t>::get( _a2, a0, a1, a2 ),
				free_standing_call_args::resolve_arg<3, a3_t, fa0_t, fa1_t, fa2_t>::get( _a3, a0, a1, a2 ),
				free_standing_call_args::resolve_arg<3, a4_t, fa0_t, fa1_t, fa2_t>::get( _a4, a0, a1, a2 ),
				free_standing_call_args::resolve_arg<3, a5_t, fa0_t, fa1_t, fa2_t>::get( _a5, a0, a1, a2 ) ) ); }
	};

template<typename descriptor, typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t, typename a6_t>
class HCall<3, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t,
	trait::no_type, trait::no_type, trait::no_type>
		: public HCallBase<3, descriptor, return_t, CALL_t, a0_t,
	a1_t, a2_t, a3_t, a4_t,
	a5_t, a6_t, trait::no_type, trait::no_type,
	trait::no_type, trait::no_type>
	{
	typedef HCallBase<3, descriptor, return_t, CALL_t, a0_t,
	a1_t, a2_t, a3_t, a4_t,
	a5_t, a6_t, trait::no_type, trait::no_type,
	trait::no_type, trait::no_type> base_t;
	typedef typename descriptor::type::sorted_real_args::a0_t fa0_t;
	typedef typename descriptor::type::sorted_real_args::a1_t fa1_t;
	typedef typename descriptor::type::sorted_real_args::a2_t fa2_t;
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
		: base_t( call_ ), _a0( a0 ), _a1( a1 ), _a2( a2 ), _a3( a3 ), _a4( a4 ), _a5( a5 ), _a6( a6 ) {}
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2 ) { return ( (base_t::_call)(
				free_standing_call_args::resolve_arg<3, a0_t, fa0_t, fa1_t, fa2_t>::get( _a0, a0, a1, a2 ),
				free_standing_call_args::resolve_arg<3, a1_t, fa0_t, fa1_t, fa2_t>::get( _a1, a0, a1, a2 ),
				free_standing_call_args::resolve_arg<3, a2_t, fa0_t, fa1_t, fa2_t>::get( _a2, a0, a1, a2 ),
				free_standing_call_args::resolve_arg<3, a3_t, fa0_t, fa1_t, fa2_t>::get( _a3, a0, a1, a2 ),
				free_standing_call_args::resolve_arg<3, a4_t, fa0_t, fa1_t, fa2_t>::get( _a4, a0, a1, a2 ),
				free_standing_call_args::resolve_arg<3, a5_t, fa0_t, fa1_t, fa2_t>::get( _a5, a0, a1, a2 ),
				free_standing_call_args::resolve_arg<3, a6_t, fa0_t, fa1_t, fa2_t>::get( _a6, a0, a1, a2 ) ) ); }
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2 ) const { return ( (base_t::_call)(
				free_standing_call_args::resolve_arg<3, a0_t, fa0_t, fa1_t, fa2_t>::get( _a0, a0, a1, a2 ),
				free_standing_call_args::resolve_arg<3, a1_t, fa0_t, fa1_t, fa2_t>::get( _a1, a0, a1, a2 ),
				free_standing_call_args::resolve_arg<3, a2_t, fa0_t, fa1_t, fa2_t>::get( _a2, a0, a1, a2 ),
				free_standing_call_args::resolve_arg<3, a3_t, fa0_t, fa1_t, fa2_t>::get( _a3, a0, a1, a2 ),
				free_standing_call_args::resolve_arg<3, a4_t, fa0_t, fa1_t, fa2_t>::get( _a4, a0, a1, a2 ),
				free_standing_call_args::resolve_arg<3, a5_t, fa0_t, fa1_t, fa2_t>::get( _a5, a0, a1, a2 ),
				free_standing_call_args::resolve_arg<3, a6_t, fa0_t, fa1_t, fa2_t>::get( _a6, a0, a1, a2 ) ) ); }
	};

template<typename descriptor, typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t, typename a6_t, typename a7_t>
class HCall<3, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t,
	trait::no_type, trait::no_type>
		: public HCallBase<3, descriptor, return_t, CALL_t, a0_t,
	a1_t, a2_t, a3_t, a4_t,
	a5_t, a6_t, a7_t, trait::no_type,
	trait::no_type, trait::no_type>
	{
	typedef HCallBase<3, descriptor, return_t, CALL_t, a0_t,
	a1_t, a2_t, a3_t, a4_t,
	a5_t, a6_t, a7_t, trait::no_type,
	trait::no_type, trait::no_type> base_t;
	typedef typename descriptor::type::sorted_real_args::a0_t fa0_t;
	typedef typename descriptor::type::sorted_real_args::a1_t fa1_t;
	typedef typename descriptor::type::sorted_real_args::a2_t fa2_t;
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
		: base_t( call_ ), _a0( a0 ), _a1( a1 ), _a2( a2 ), _a3( a3 ), _a4( a4 ), _a5( a5 ), _a6( a6 ), _a7( a7 ) {}
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2 ) { return ( (base_t::_call)(
				free_standing_call_args::resolve_arg<3, a0_t, fa0_t, fa1_t, fa2_t>::get( _a0, a0, a1, a2 ),
				free_standing_call_args::resolve_arg<3, a1_t, fa0_t, fa1_t, fa2_t>::get( _a1, a0, a1, a2 ),
				free_standing_call_args::resolve_arg<3, a2_t, fa0_t, fa1_t, fa2_t>::get( _a2, a0, a1, a2 ),
				free_standing_call_args::resolve_arg<3, a3_t, fa0_t, fa1_t, fa2_t>::get( _a3, a0, a1, a2 ),
				free_standing_call_args::resolve_arg<3, a4_t, fa0_t, fa1_t, fa2_t>::get( _a4, a0, a1, a2 ),
				free_standing_call_args::resolve_arg<3, a5_t, fa0_t, fa1_t, fa2_t>::get( _a5, a0, a1, a2 ),
				free_standing_call_args::resolve_arg<3, a6_t, fa0_t, fa1_t, fa2_t>::get( _a6, a0, a1, a2 ),
				free_standing_call_args::resolve_arg<3, a7_t, fa0_t, fa1_t, fa2_t>::get( _a7, a0, a1, a2 ) ) ); }
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2 ) const { return ( (base_t::_call)(
				free_standing_call_args::resolve_arg<3, a0_t, fa0_t, fa1_t, fa2_t>::get( _a0, a0, a1, a2 ),
				free_standing_call_args::resolve_arg<3, a1_t, fa0_t, fa1_t, fa2_t>::get( _a1, a0, a1, a2 ),
				free_standing_call_args::resolve_arg<3, a2_t, fa0_t, fa1_t, fa2_t>::get( _a2, a0, a1, a2 ),
				free_standing_call_args::resolve_arg<3, a3_t, fa0_t, fa1_t, fa2_t>::get( _a3, a0, a1, a2 ),
				free_standing_call_args::resolve_arg<3, a4_t, fa0_t, fa1_t, fa2_t>::get( _a4, a0, a1, a2 ),
				free_standing_call_args::resolve_arg<3, a5_t, fa0_t, fa1_t, fa2_t>::get( _a5, a0, a1, a2 ),
				free_standing_call_args::resolve_arg<3, a6_t, fa0_t, fa1_t, fa2_t>::get( _a6, a0, a1, a2 ),
				free_standing_call_args::resolve_arg<3, a7_t, fa0_t, fa1_t, fa2_t>::get( _a7, a0, a1, a2 ) ) ); }
	};

template<typename descriptor, typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t, typename a6_t, typename a7_t,
	typename a8_t>
class HCall<3, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t,
	trait::no_type>
		: public HCallBase<3, descriptor, return_t, CALL_t, a0_t,
	a1_t, a2_t, a3_t, a4_t,
	a5_t, a6_t, a7_t, a8_t,
	trait::no_type, trait::no_type>
	{
	typedef HCallBase<3, descriptor, return_t, CALL_t, a0_t,
	a1_t, a2_t, a3_t, a4_t,
	a5_t, a6_t, a7_t, a8_t,
	trait::no_type, trait::no_type> base_t;
	typedef typename descriptor::type::sorted_real_args::a0_t fa0_t;
	typedef typename descriptor::type::sorted_real_args::a1_t fa1_t;
	typedef typename descriptor::type::sorted_real_args::a2_t fa2_t;
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
	HCall( CALL_t call_, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7, a8_t a8 )
		: base_t( call_ ), _a0( a0 ), _a1( a1 ), _a2( a2 ), _a3( a3 ), _a4( a4 ), _a5( a5 ), _a6( a6 ), _a7( a7 ), _a8( a8 ) {}
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2 ) { return ( (base_t::_call)(
				free_standing_call_args::resolve_arg<3, a0_t, fa0_t, fa1_t, fa2_t>::get( _a0, a0, a1, a2 ),
				free_standing_call_args::resolve_arg<3, a1_t, fa0_t, fa1_t, fa2_t>::get( _a1, a0, a1, a2 ),
				free_standing_call_args::resolve_arg<3, a2_t, fa0_t, fa1_t, fa2_t>::get( _a2, a0, a1, a2 ),
				free_standing_call_args::resolve_arg<3, a3_t, fa0_t, fa1_t, fa2_t>::get( _a3, a0, a1, a2 ),
				free_standing_call_args::resolve_arg<3, a4_t, fa0_t, fa1_t, fa2_t>::get( _a4, a0, a1, a2 ),
				free_standing_call_args::resolve_arg<3, a5_t, fa0_t, fa1_t, fa2_t>::get( _a5, a0, a1, a2 ),
				free_standing_call_args::resolve_arg<3, a6_t, fa0_t, fa1_t, fa2_t>::get( _a6, a0, a1, a2 ),
				free_standing_call_args::resolve_arg<3, a7_t, fa0_t, fa1_t, fa2_t>::get( _a7, a0, a1, a2 ),
				free_standing_call_args::resolve_arg<3, a8_t, fa0_t, fa1_t, fa2_t>::get( _a8, a0, a1, a2 ) ) ); }
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2 ) const { return ( (base_t::_call)(
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

template<typename descriptor, typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t, typename a6_t, typename a7_t,
	typename a8_t, typename a9_t>
class HCall<3, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t>
		: public HCallBase<3, descriptor, return_t, CALL_t, a0_t,
	a1_t, a2_t, a3_t, a4_t,
	a5_t, a6_t, a7_t, a8_t,
	a9_t, trait::no_type>
	{
	typedef HCallBase<3, descriptor, return_t, CALL_t, a0_t,
	a1_t, a2_t, a3_t, a4_t,
	a5_t, a6_t, a7_t, a8_t,
	a9_t, trait::no_type> base_t;
	typedef typename descriptor::type::sorted_real_args::a0_t fa0_t;
	typedef typename descriptor::type::sorted_real_args::a1_t fa1_t;
	typedef typename descriptor::type::sorted_real_args::a2_t fa2_t;
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
	HCall( CALL_t call_, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7, a8_t a8, a9_t a9 )
		: base_t( call_ ), _a0( a0 ), _a1( a1 ), _a2( a2 ), _a3( a3 ), _a4( a4 ), _a5( a5 ), _a6( a6 ), _a7( a7 ), _a8( a8 ), _a9( a9 ) {}
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2 ) { return ( (base_t::_call)(
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
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2 ) const { return ( (base_t::_call)(
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

template<typename descriptor, typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t, typename a6_t, typename a7_t,
	typename a8_t, typename a9_t, typename a10_t>
class HCall<3, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t, a10_t>
		: public HCallBase<3, descriptor, return_t, CALL_t, a0_t,
	a1_t, a2_t, a3_t, a4_t,
	a5_t, a6_t, a7_t, a8_t,
	a9_t, a10_t>
	{
	typedef HCallBase<3, descriptor, return_t, CALL_t, a0_t,
	a1_t, a2_t, a3_t, a4_t,
	a5_t, a6_t, a7_t, a8_t,
	a9_t, a10_t> base_t;
	typedef typename descriptor::type::sorted_real_args::a0_t fa0_t;
	typedef typename descriptor::type::sorted_real_args::a1_t fa1_t;
	typedef typename descriptor::type::sorted_real_args::a2_t fa2_t;
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
	HCall( CALL_t call_, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7, a8_t a8, a9_t a9, a10_t a10 )
		: base_t( call_ ), _a0( a0 ), _a1( a1 ), _a2( a2 ), _a3( a3 ), _a4( a4 ), _a5( a5 ), _a6( a6 ), _a7( a7 ), _a8( a8 ), _a9( a9 ), _a10( a10 ) {}
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2 ) { return ( (base_t::_call)(
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
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2 ) const { return ( (base_t::_call)(
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

template<typename descriptor, typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t>
class HCall<4, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t,
	trait::no_type, trait::no_type, trait::no_type, trait::no_type,
	trait::no_type, trait::no_type>
		: public HCallBase<4, descriptor, return_t, CALL_t, a0_t,
	a1_t, a2_t, a3_t, trait::no_type,
	trait::no_type, trait::no_type, trait::no_type, trait::no_type,
	trait::no_type, trait::no_type>
	{
	typedef HCallBase<4, descriptor, return_t, CALL_t, a0_t,
	a1_t, a2_t, a3_t, trait::no_type,
	trait::no_type, trait::no_type, trait::no_type, trait::no_type,
	trait::no_type, trait::no_type> base_t;
	typedef typename descriptor::type::sorted_real_args::a0_t fa0_t;
	typedef typename descriptor::type::sorted_real_args::a1_t fa1_t;
	typedef typename descriptor::type::sorted_real_args::a2_t fa2_t;
	typedef typename descriptor::type::sorted_real_args::a3_t fa3_t;
	a0_t _a0;
	a1_t _a1;
	a2_t _a2;
	a3_t _a3;
public:
	HCall( CALL_t call_, a0_t a0, a1_t a1, a2_t a2, a3_t a3 )
		: base_t( call_ ), _a0( a0 ), _a1( a1 ), _a2( a2 ), _a3( a3 ) {}
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3 ) { return ( (base_t::_call)(
				free_standing_call_args::resolve_arg<4, a0_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a0, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg<4, a1_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a1, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg<4, a2_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a2, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg<4, a3_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a3, a0, a1, a2, a3 ) ) ); }
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3 ) const { return ( (base_t::_call)(
				free_standing_call_args::resolve_arg<4, a0_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a0, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg<4, a1_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a1, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg<4, a2_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a2, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg<4, a3_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a3, a0, a1, a2, a3 ) ) ); }
	};

template<typename descriptor, typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t>
class HCall<4, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t,
	trait::no_type, trait::no_type, trait::no_type, trait::no_type,
	trait::no_type>
		: public HCallBase<4, descriptor, return_t, CALL_t, a0_t,
	a1_t, a2_t, a3_t, a4_t,
	trait::no_type, trait::no_type, trait::no_type, trait::no_type,
	trait::no_type, trait::no_type>
	{
	typedef HCallBase<4, descriptor, return_t, CALL_t, a0_t,
	a1_t, a2_t, a3_t, a4_t,
	trait::no_type, trait::no_type, trait::no_type, trait::no_type,
	trait::no_type, trait::no_type> base_t;
	typedef typename descriptor::type::sorted_real_args::a0_t fa0_t;
	typedef typename descriptor::type::sorted_real_args::a1_t fa1_t;
	typedef typename descriptor::type::sorted_real_args::a2_t fa2_t;
	typedef typename descriptor::type::sorted_real_args::a3_t fa3_t;
	a0_t _a0;
	a1_t _a1;
	a2_t _a2;
	a3_t _a3;
	a4_t _a4;
public:
	HCall( CALL_t call_, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4 )
		: base_t( call_ ), _a0( a0 ), _a1( a1 ), _a2( a2 ), _a3( a3 ), _a4( a4 ) {}
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3 ) { return ( (base_t::_call)(
				free_standing_call_args::resolve_arg<4, a0_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a0, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg<4, a1_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a1, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg<4, a2_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a2, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg<4, a3_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a3, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg<4, a4_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a4, a0, a1, a2, a3 ) ) ); }
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3 ) const { return ( (base_t::_call)(
				free_standing_call_args::resolve_arg<4, a0_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a0, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg<4, a1_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a1, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg<4, a2_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a2, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg<4, a3_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a3, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg<4, a4_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a4, a0, a1, a2, a3 ) ) ); }
	};

template<typename descriptor, typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t>
class HCall<4, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t,
	trait::no_type, trait::no_type, trait::no_type, trait::no_type>
		: public HCallBase<4, descriptor, return_t, CALL_t, a0_t,
	a1_t, a2_t, a3_t, a4_t,
	a5_t, trait::no_type, trait::no_type, trait::no_type,
	trait::no_type, trait::no_type>
	{
	typedef HCallBase<4, descriptor, return_t, CALL_t, a0_t,
	a1_t, a2_t, a3_t, a4_t,
	a5_t, trait::no_type, trait::no_type, trait::no_type,
	trait::no_type, trait::no_type> base_t;
	typedef typename descriptor::type::sorted_real_args::a0_t fa0_t;
	typedef typename descriptor::type::sorted_real_args::a1_t fa1_t;
	typedef typename descriptor::type::sorted_real_args::a2_t fa2_t;
	typedef typename descriptor::type::sorted_real_args::a3_t fa3_t;
	a0_t _a0;
	a1_t _a1;
	a2_t _a2;
	a3_t _a3;
	a4_t _a4;
	a5_t _a5;
public:
	HCall( CALL_t call_, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5 )
		: base_t( call_ ), _a0( a0 ), _a1( a1 ), _a2( a2 ), _a3( a3 ), _a4( a4 ), _a5( a5 ) {}
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3 ) { return ( (base_t::_call)(
				free_standing_call_args::resolve_arg<4, a0_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a0, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg<4, a1_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a1, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg<4, a2_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a2, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg<4, a3_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a3, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg<4, a4_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a4, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg<4, a5_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a5, a0, a1, a2, a3 ) ) ); }
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3 ) const { return ( (base_t::_call)(
				free_standing_call_args::resolve_arg<4, a0_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a0, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg<4, a1_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a1, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg<4, a2_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a2, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg<4, a3_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a3, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg<4, a4_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a4, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg<4, a5_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a5, a0, a1, a2, a3 ) ) ); }
	};

template<typename descriptor, typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t, typename a6_t>
class HCall<4, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t,
	trait::no_type, trait::no_type, trait::no_type>
		: public HCallBase<4, descriptor, return_t, CALL_t, a0_t,
	a1_t, a2_t, a3_t, a4_t,
	a5_t, a6_t, trait::no_type, trait::no_type,
	trait::no_type, trait::no_type>
	{
	typedef HCallBase<4, descriptor, return_t, CALL_t, a0_t,
	a1_t, a2_t, a3_t, a4_t,
	a5_t, a6_t, trait::no_type, trait::no_type,
	trait::no_type, trait::no_type> base_t;
	typedef typename descriptor::type::sorted_real_args::a0_t fa0_t;
	typedef typename descriptor::type::sorted_real_args::a1_t fa1_t;
	typedef typename descriptor::type::sorted_real_args::a2_t fa2_t;
	typedef typename descriptor::type::sorted_real_args::a3_t fa3_t;
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
		: base_t( call_ ), _a0( a0 ), _a1( a1 ), _a2( a2 ), _a3( a3 ), _a4( a4 ), _a5( a5 ), _a6( a6 ) {}
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3 ) { return ( (base_t::_call)(
				free_standing_call_args::resolve_arg<4, a0_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a0, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg<4, a1_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a1, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg<4, a2_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a2, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg<4, a3_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a3, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg<4, a4_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a4, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg<4, a5_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a5, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg<4, a6_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a6, a0, a1, a2, a3 ) ) ); }
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3 ) const { return ( (base_t::_call)(
				free_standing_call_args::resolve_arg<4, a0_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a0, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg<4, a1_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a1, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg<4, a2_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a2, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg<4, a3_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a3, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg<4, a4_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a4, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg<4, a5_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a5, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg<4, a6_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a6, a0, a1, a2, a3 ) ) ); }
	};

template<typename descriptor, typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t, typename a6_t, typename a7_t>
class HCall<4, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t,
	trait::no_type, trait::no_type>
		: public HCallBase<4, descriptor, return_t, CALL_t, a0_t,
	a1_t, a2_t, a3_t, a4_t,
	a5_t, a6_t, a7_t, trait::no_type,
	trait::no_type, trait::no_type>
	{
	typedef HCallBase<4, descriptor, return_t, CALL_t, a0_t,
	a1_t, a2_t, a3_t, a4_t,
	a5_t, a6_t, a7_t, trait::no_type,
	trait::no_type, trait::no_type> base_t;
	typedef typename descriptor::type::sorted_real_args::a0_t fa0_t;
	typedef typename descriptor::type::sorted_real_args::a1_t fa1_t;
	typedef typename descriptor::type::sorted_real_args::a2_t fa2_t;
	typedef typename descriptor::type::sorted_real_args::a3_t fa3_t;
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
		: base_t( call_ ), _a0( a0 ), _a1( a1 ), _a2( a2 ), _a3( a3 ), _a4( a4 ), _a5( a5 ), _a6( a6 ), _a7( a7 ) {}
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3 ) { return ( (base_t::_call)(
				free_standing_call_args::resolve_arg<4, a0_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a0, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg<4, a1_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a1, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg<4, a2_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a2, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg<4, a3_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a3, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg<4, a4_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a4, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg<4, a5_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a5, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg<4, a6_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a6, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg<4, a7_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a7, a0, a1, a2, a3 ) ) ); }
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3 ) const { return ( (base_t::_call)(
				free_standing_call_args::resolve_arg<4, a0_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a0, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg<4, a1_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a1, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg<4, a2_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a2, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg<4, a3_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a3, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg<4, a4_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a4, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg<4, a5_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a5, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg<4, a6_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a6, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg<4, a7_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a7, a0, a1, a2, a3 ) ) ); }
	};

template<typename descriptor, typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t, typename a6_t, typename a7_t,
	typename a8_t>
class HCall<4, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t,
	trait::no_type>
		: public HCallBase<4, descriptor, return_t, CALL_t, a0_t,
	a1_t, a2_t, a3_t, a4_t,
	a5_t, a6_t, a7_t, a8_t,
	trait::no_type, trait::no_type>
	{
	typedef HCallBase<4, descriptor, return_t, CALL_t, a0_t,
	a1_t, a2_t, a3_t, a4_t,
	a5_t, a6_t, a7_t, a8_t,
	trait::no_type, trait::no_type> base_t;
	typedef typename descriptor::type::sorted_real_args::a0_t fa0_t;
	typedef typename descriptor::type::sorted_real_args::a1_t fa1_t;
	typedef typename descriptor::type::sorted_real_args::a2_t fa2_t;
	typedef typename descriptor::type::sorted_real_args::a3_t fa3_t;
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
	HCall( CALL_t call_, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7, a8_t a8 )
		: base_t( call_ ), _a0( a0 ), _a1( a1 ), _a2( a2 ), _a3( a3 ), _a4( a4 ), _a5( a5 ), _a6( a6 ), _a7( a7 ), _a8( a8 ) {}
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3 ) { return ( (base_t::_call)(
				free_standing_call_args::resolve_arg<4, a0_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a0, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg<4, a1_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a1, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg<4, a2_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a2, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg<4, a3_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a3, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg<4, a4_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a4, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg<4, a5_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a5, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg<4, a6_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a6, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg<4, a7_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a7, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg<4, a8_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( _a8, a0, a1, a2, a3 ) ) ); }
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3 ) const { return ( (base_t::_call)(
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

template<typename descriptor, typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t, typename a6_t, typename a7_t,
	typename a8_t, typename a9_t>
class HCall<4, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t>
		: public HCallBase<4, descriptor, return_t, CALL_t, a0_t,
	a1_t, a2_t, a3_t, a4_t,
	a5_t, a6_t, a7_t, a8_t,
	a9_t, trait::no_type>
	{
	typedef HCallBase<4, descriptor, return_t, CALL_t, a0_t,
	a1_t, a2_t, a3_t, a4_t,
	a5_t, a6_t, a7_t, a8_t,
	a9_t, trait::no_type> base_t;
	typedef typename descriptor::type::sorted_real_args::a0_t fa0_t;
	typedef typename descriptor::type::sorted_real_args::a1_t fa1_t;
	typedef typename descriptor::type::sorted_real_args::a2_t fa2_t;
	typedef typename descriptor::type::sorted_real_args::a3_t fa3_t;
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
	HCall( CALL_t call_, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7, a8_t a8, a9_t a9 )
		: base_t( call_ ), _a0( a0 ), _a1( a1 ), _a2( a2 ), _a3( a3 ), _a4( a4 ), _a5( a5 ), _a6( a6 ), _a7( a7 ), _a8( a8 ), _a9( a9 ) {}
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3 ) { return ( (base_t::_call)(
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
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3 ) const { return ( (base_t::_call)(
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

template<typename descriptor, typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t, typename a6_t, typename a7_t,
	typename a8_t, typename a9_t, typename a10_t>
class HCall<4, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t, a10_t>
		: public HCallBase<4, descriptor, return_t, CALL_t, a0_t,
	a1_t, a2_t, a3_t, a4_t,
	a5_t, a6_t, a7_t, a8_t,
	a9_t, a10_t>
	{
	typedef HCallBase<4, descriptor, return_t, CALL_t, a0_t,
	a1_t, a2_t, a3_t, a4_t,
	a5_t, a6_t, a7_t, a8_t,
	a9_t, a10_t> base_t;
	typedef typename descriptor::type::sorted_real_args::a0_t fa0_t;
	typedef typename descriptor::type::sorted_real_args::a1_t fa1_t;
	typedef typename descriptor::type::sorted_real_args::a2_t fa2_t;
	typedef typename descriptor::type::sorted_real_args::a3_t fa3_t;
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
	HCall( CALL_t call_, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7, a8_t a8, a9_t a9, a10_t a10 )
		: base_t( call_ ), _a0( a0 ), _a1( a1 ), _a2( a2 ), _a3( a3 ), _a4( a4 ), _a5( a5 ), _a6( a6 ), _a7( a7 ), _a8( a8 ), _a9( a9 ), _a10( a10 ) {}
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3 ) { return ( (base_t::_call)(
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
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3 ) const { return ( (base_t::_call)(
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

template<typename descriptor, typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t>
class HCall<5, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t,
	trait::no_type, trait::no_type, trait::no_type, trait::no_type,
	trait::no_type>
		: public HCallBase<5, descriptor, return_t, CALL_t, a0_t,
	a1_t, a2_t, a3_t, a4_t,
	trait::no_type, trait::no_type, trait::no_type, trait::no_type,
	trait::no_type, trait::no_type>
	{
	typedef HCallBase<5, descriptor, return_t, CALL_t, a0_t,
	a1_t, a2_t, a3_t, a4_t,
	trait::no_type, trait::no_type, trait::no_type, trait::no_type,
	trait::no_type, trait::no_type> base_t;
	typedef typename descriptor::type::sorted_real_args::a0_t fa0_t;
	typedef typename descriptor::type::sorted_real_args::a1_t fa1_t;
	typedef typename descriptor::type::sorted_real_args::a2_t fa2_t;
	typedef typename descriptor::type::sorted_real_args::a3_t fa3_t;
	typedef typename descriptor::type::sorted_real_args::a4_t fa4_t;
	a0_t _a0;
	a1_t _a1;
	a2_t _a2;
	a3_t _a3;
	a4_t _a4;
public:
	HCall( CALL_t call_, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4 )
		: base_t( call_ ), _a0( a0 ), _a1( a1 ), _a2( a2 ), _a3( a3 ), _a4( a4 ) {}
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3, fa4_t a4 ) { return ( (base_t::_call)(
				free_standing_call_args::resolve_arg<5, a0_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t>::get( _a0, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg<5, a1_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t>::get( _a1, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg<5, a2_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t>::get( _a2, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg<5, a3_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t>::get( _a3, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg<5, a4_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t>::get( _a4, a0, a1, a2, a3, a4 ) ) ); }
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3, fa4_t a4 ) const { return ( (base_t::_call)(
				free_standing_call_args::resolve_arg<5, a0_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t>::get( _a0, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg<5, a1_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t>::get( _a1, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg<5, a2_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t>::get( _a2, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg<5, a3_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t>::get( _a3, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg<5, a4_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t>::get( _a4, a0, a1, a2, a3, a4 ) ) ); }
	};

template<typename descriptor, typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t>
class HCall<5, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t,
	trait::no_type, trait::no_type, trait::no_type, trait::no_type>
		: public HCallBase<5, descriptor, return_t, CALL_t, a0_t,
	a1_t, a2_t, a3_t, a4_t,
	a5_t, trait::no_type, trait::no_type, trait::no_type,
	trait::no_type, trait::no_type>
	{
	typedef HCallBase<5, descriptor, return_t, CALL_t, a0_t,
	a1_t, a2_t, a3_t, a4_t,
	a5_t, trait::no_type, trait::no_type, trait::no_type,
	trait::no_type, trait::no_type> base_t;
	typedef typename descriptor::type::sorted_real_args::a0_t fa0_t;
	typedef typename descriptor::type::sorted_real_args::a1_t fa1_t;
	typedef typename descriptor::type::sorted_real_args::a2_t fa2_t;
	typedef typename descriptor::type::sorted_real_args::a3_t fa3_t;
	typedef typename descriptor::type::sorted_real_args::a4_t fa4_t;
	a0_t _a0;
	a1_t _a1;
	a2_t _a2;
	a3_t _a3;
	a4_t _a4;
	a5_t _a5;
public:
	HCall( CALL_t call_, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5 )
		: base_t( call_ ), _a0( a0 ), _a1( a1 ), _a2( a2 ), _a3( a3 ), _a4( a4 ), _a5( a5 ) {}
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3, fa4_t a4 ) { return ( (base_t::_call)(
				free_standing_call_args::resolve_arg<5, a0_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t>::get( _a0, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg<5, a1_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t>::get( _a1, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg<5, a2_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t>::get( _a2, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg<5, a3_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t>::get( _a3, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg<5, a4_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t>::get( _a4, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg<5, a5_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t>::get( _a5, a0, a1, a2, a3, a4 ) ) ); }
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3, fa4_t a4 ) const { return ( (base_t::_call)(
				free_standing_call_args::resolve_arg<5, a0_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t>::get( _a0, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg<5, a1_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t>::get( _a1, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg<5, a2_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t>::get( _a2, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg<5, a3_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t>::get( _a3, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg<5, a4_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t>::get( _a4, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg<5, a5_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t>::get( _a5, a0, a1, a2, a3, a4 ) ) ); }
	};

template<typename descriptor, typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t, typename a6_t>
class HCall<5, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t,
	trait::no_type, trait::no_type, trait::no_type>
		: public HCallBase<5, descriptor, return_t, CALL_t, a0_t,
	a1_t, a2_t, a3_t, a4_t,
	a5_t, a6_t, trait::no_type, trait::no_type,
	trait::no_type, trait::no_type>
	{
	typedef HCallBase<5, descriptor, return_t, CALL_t, a0_t,
	a1_t, a2_t, a3_t, a4_t,
	a5_t, a6_t, trait::no_type, trait::no_type,
	trait::no_type, trait::no_type> base_t;
	typedef typename descriptor::type::sorted_real_args::a0_t fa0_t;
	typedef typename descriptor::type::sorted_real_args::a1_t fa1_t;
	typedef typename descriptor::type::sorted_real_args::a2_t fa2_t;
	typedef typename descriptor::type::sorted_real_args::a3_t fa3_t;
	typedef typename descriptor::type::sorted_real_args::a4_t fa4_t;
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
		: base_t( call_ ), _a0( a0 ), _a1( a1 ), _a2( a2 ), _a3( a3 ), _a4( a4 ), _a5( a5 ), _a6( a6 ) {}
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3, fa4_t a4 ) { return ( (base_t::_call)(
				free_standing_call_args::resolve_arg<5, a0_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t>::get( _a0, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg<5, a1_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t>::get( _a1, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg<5, a2_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t>::get( _a2, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg<5, a3_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t>::get( _a3, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg<5, a4_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t>::get( _a4, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg<5, a5_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t>::get( _a5, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg<5, a6_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t>::get( _a6, a0, a1, a2, a3, a4 ) ) ); }
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3, fa4_t a4 ) const { return ( (base_t::_call)(
				free_standing_call_args::resolve_arg<5, a0_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t>::get( _a0, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg<5, a1_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t>::get( _a1, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg<5, a2_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t>::get( _a2, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg<5, a3_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t>::get( _a3, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg<5, a4_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t>::get( _a4, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg<5, a5_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t>::get( _a5, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg<5, a6_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t>::get( _a6, a0, a1, a2, a3, a4 ) ) ); }
	};

template<typename descriptor, typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t, typename a6_t, typename a7_t>
class HCall<5, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t,
	trait::no_type, trait::no_type>
		: public HCallBase<5, descriptor, return_t, CALL_t, a0_t,
	a1_t, a2_t, a3_t, a4_t,
	a5_t, a6_t, a7_t, trait::no_type,
	trait::no_type, trait::no_type>
	{
	typedef HCallBase<5, descriptor, return_t, CALL_t, a0_t,
	a1_t, a2_t, a3_t, a4_t,
	a5_t, a6_t, a7_t, trait::no_type,
	trait::no_type, trait::no_type> base_t;
	typedef typename descriptor::type::sorted_real_args::a0_t fa0_t;
	typedef typename descriptor::type::sorted_real_args::a1_t fa1_t;
	typedef typename descriptor::type::sorted_real_args::a2_t fa2_t;
	typedef typename descriptor::type::sorted_real_args::a3_t fa3_t;
	typedef typename descriptor::type::sorted_real_args::a4_t fa4_t;
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
		: base_t( call_ ), _a0( a0 ), _a1( a1 ), _a2( a2 ), _a3( a3 ), _a4( a4 ), _a5( a5 ), _a6( a6 ), _a7( a7 ) {}
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3, fa4_t a4 ) { return ( (base_t::_call)(
				free_standing_call_args::resolve_arg<5, a0_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t>::get( _a0, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg<5, a1_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t>::get( _a1, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg<5, a2_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t>::get( _a2, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg<5, a3_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t>::get( _a3, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg<5, a4_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t>::get( _a4, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg<5, a5_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t>::get( _a5, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg<5, a6_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t>::get( _a6, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg<5, a7_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t>::get( _a7, a0, a1, a2, a3, a4 ) ) ); }
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3, fa4_t a4 ) const { return ( (base_t::_call)(
				free_standing_call_args::resolve_arg<5, a0_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t>::get( _a0, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg<5, a1_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t>::get( _a1, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg<5, a2_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t>::get( _a2, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg<5, a3_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t>::get( _a3, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg<5, a4_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t>::get( _a4, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg<5, a5_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t>::get( _a5, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg<5, a6_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t>::get( _a6, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg<5, a7_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t>::get( _a7, a0, a1, a2, a3, a4 ) ) ); }
	};

template<typename descriptor, typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t, typename a6_t, typename a7_t,
	typename a8_t>
class HCall<5, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t,
	trait::no_type>
		: public HCallBase<5, descriptor, return_t, CALL_t, a0_t,
	a1_t, a2_t, a3_t, a4_t,
	a5_t, a6_t, a7_t, a8_t,
	trait::no_type, trait::no_type>
	{
	typedef HCallBase<5, descriptor, return_t, CALL_t, a0_t,
	a1_t, a2_t, a3_t, a4_t,
	a5_t, a6_t, a7_t, a8_t,
	trait::no_type, trait::no_type> base_t;
	typedef typename descriptor::type::sorted_real_args::a0_t fa0_t;
	typedef typename descriptor::type::sorted_real_args::a1_t fa1_t;
	typedef typename descriptor::type::sorted_real_args::a2_t fa2_t;
	typedef typename descriptor::type::sorted_real_args::a3_t fa3_t;
	typedef typename descriptor::type::sorted_real_args::a4_t fa4_t;
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
	HCall( CALL_t call_, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7, a8_t a8 )
		: base_t( call_ ), _a0( a0 ), _a1( a1 ), _a2( a2 ), _a3( a3 ), _a4( a4 ), _a5( a5 ), _a6( a6 ), _a7( a7 ), _a8( a8 ) {}
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3, fa4_t a4 ) { return ( (base_t::_call)(
				free_standing_call_args::resolve_arg<5, a0_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t>::get( _a0, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg<5, a1_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t>::get( _a1, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg<5, a2_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t>::get( _a2, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg<5, a3_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t>::get( _a3, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg<5, a4_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t>::get( _a4, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg<5, a5_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t>::get( _a5, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg<5, a6_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t>::get( _a6, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg<5, a7_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t>::get( _a7, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg<5, a8_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t>::get( _a8, a0, a1, a2, a3, a4 ) ) ); }
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3, fa4_t a4 ) const { return ( (base_t::_call)(
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

template<typename descriptor, typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t, typename a6_t, typename a7_t,
	typename a8_t, typename a9_t>
class HCall<5, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t>
		: public HCallBase<5, descriptor, return_t, CALL_t, a0_t,
	a1_t, a2_t, a3_t, a4_t,
	a5_t, a6_t, a7_t, a8_t,
	a9_t, trait::no_type>
	{
	typedef HCallBase<5, descriptor, return_t, CALL_t, a0_t,
	a1_t, a2_t, a3_t, a4_t,
	a5_t, a6_t, a7_t, a8_t,
	a9_t, trait::no_type> base_t;
	typedef typename descriptor::type::sorted_real_args::a0_t fa0_t;
	typedef typename descriptor::type::sorted_real_args::a1_t fa1_t;
	typedef typename descriptor::type::sorted_real_args::a2_t fa2_t;
	typedef typename descriptor::type::sorted_real_args::a3_t fa3_t;
	typedef typename descriptor::type::sorted_real_args::a4_t fa4_t;
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
	HCall( CALL_t call_, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7, a8_t a8, a9_t a9 )
		: base_t( call_ ), _a0( a0 ), _a1( a1 ), _a2( a2 ), _a3( a3 ), _a4( a4 ), _a5( a5 ), _a6( a6 ), _a7( a7 ), _a8( a8 ), _a9( a9 ) {}
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3, fa4_t a4 ) { return ( (base_t::_call)(
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
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3, fa4_t a4 ) const { return ( (base_t::_call)(
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

template<typename descriptor, typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t, typename a6_t, typename a7_t,
	typename a8_t, typename a9_t, typename a10_t>
class HCall<5, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t, a10_t>
		: public HCallBase<5, descriptor, return_t, CALL_t, a0_t,
	a1_t, a2_t, a3_t, a4_t,
	a5_t, a6_t, a7_t, a8_t,
	a9_t, a10_t>
	{
	typedef HCallBase<5, descriptor, return_t, CALL_t, a0_t,
	a1_t, a2_t, a3_t, a4_t,
	a5_t, a6_t, a7_t, a8_t,
	a9_t, a10_t> base_t;
	typedef typename descriptor::type::sorted_real_args::a0_t fa0_t;
	typedef typename descriptor::type::sorted_real_args::a1_t fa1_t;
	typedef typename descriptor::type::sorted_real_args::a2_t fa2_t;
	typedef typename descriptor::type::sorted_real_args::a3_t fa3_t;
	typedef typename descriptor::type::sorted_real_args::a4_t fa4_t;
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
	HCall( CALL_t call_, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7, a8_t a8, a9_t a9, a10_t a10 )
		: base_t( call_ ), _a0( a0 ), _a1( a1 ), _a2( a2 ), _a3( a3 ), _a4( a4 ), _a5( a5 ), _a6( a6 ), _a7( a7 ), _a8( a8 ), _a9( a9 ), _a10( a10 ) {}
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3, fa4_t a4 ) { return ( (base_t::_call)(
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
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3, fa4_t a4 ) const { return ( (base_t::_call)(
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

template<typename descriptor, typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t>
class HCall<6, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t,
	trait::no_type, trait::no_type, trait::no_type, trait::no_type>
		: public HCallBase<6, descriptor, return_t, CALL_t, a0_t,
	a1_t, a2_t, a3_t, a4_t,
	a5_t, trait::no_type, trait::no_type, trait::no_type,
	trait::no_type, trait::no_type>
	{
	typedef HCallBase<6, descriptor, return_t, CALL_t, a0_t,
	a1_t, a2_t, a3_t, a4_t,
	a5_t, trait::no_type, trait::no_type, trait::no_type,
	trait::no_type, trait::no_type> base_t;
	typedef typename descriptor::type::sorted_real_args::a0_t fa0_t;
	typedef typename descriptor::type::sorted_real_args::a1_t fa1_t;
	typedef typename descriptor::type::sorted_real_args::a2_t fa2_t;
	typedef typename descriptor::type::sorted_real_args::a3_t fa3_t;
	typedef typename descriptor::type::sorted_real_args::a4_t fa4_t;
	typedef typename descriptor::type::sorted_real_args::a5_t fa5_t;
	a0_t _a0;
	a1_t _a1;
	a2_t _a2;
	a3_t _a3;
	a4_t _a4;
	a5_t _a5;
public:
	HCall( CALL_t call_, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5 )
		: base_t( call_ ), _a0( a0 ), _a1( a1 ), _a2( a2 ), _a3( a3 ), _a4( a4 ), _a5( a5 ) {}
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3, fa4_t a4, fa5_t a5 ) { return ( (base_t::_call)(
				free_standing_call_args::resolve_arg<6, a0_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t>::get( _a0, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg<6, a1_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t>::get( _a1, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg<6, a2_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t>::get( _a2, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg<6, a3_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t>::get( _a3, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg<6, a4_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t>::get( _a4, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg<6, a5_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t>::get( _a5, a0, a1, a2, a3, a4, a5 ) ) ); }
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3, fa4_t a4, fa5_t a5 ) const { return ( (base_t::_call)(
				free_standing_call_args::resolve_arg<6, a0_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t>::get( _a0, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg<6, a1_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t>::get( _a1, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg<6, a2_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t>::get( _a2, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg<6, a3_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t>::get( _a3, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg<6, a4_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t>::get( _a4, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg<6, a5_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t>::get( _a5, a0, a1, a2, a3, a4, a5 ) ) ); }
	};

template<typename descriptor, typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t, typename a6_t>
class HCall<6, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t,
	trait::no_type, trait::no_type, trait::no_type>
		: public HCallBase<6, descriptor, return_t, CALL_t, a0_t,
	a1_t, a2_t, a3_t, a4_t,
	a5_t, a6_t, trait::no_type, trait::no_type,
	trait::no_type, trait::no_type>
	{
	typedef HCallBase<6, descriptor, return_t, CALL_t, a0_t,
	a1_t, a2_t, a3_t, a4_t,
	a5_t, a6_t, trait::no_type, trait::no_type,
	trait::no_type, trait::no_type> base_t;
	typedef typename descriptor::type::sorted_real_args::a0_t fa0_t;
	typedef typename descriptor::type::sorted_real_args::a1_t fa1_t;
	typedef typename descriptor::type::sorted_real_args::a2_t fa2_t;
	typedef typename descriptor::type::sorted_real_args::a3_t fa3_t;
	typedef typename descriptor::type::sorted_real_args::a4_t fa4_t;
	typedef typename descriptor::type::sorted_real_args::a5_t fa5_t;
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
		: base_t( call_ ), _a0( a0 ), _a1( a1 ), _a2( a2 ), _a3( a3 ), _a4( a4 ), _a5( a5 ), _a6( a6 ) {}
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3, fa4_t a4, fa5_t a5 ) { return ( (base_t::_call)(
				free_standing_call_args::resolve_arg<6, a0_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t>::get( _a0, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg<6, a1_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t>::get( _a1, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg<6, a2_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t>::get( _a2, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg<6, a3_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t>::get( _a3, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg<6, a4_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t>::get( _a4, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg<6, a5_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t>::get( _a5, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg<6, a6_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t>::get( _a6, a0, a1, a2, a3, a4, a5 ) ) ); }
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3, fa4_t a4, fa5_t a5 ) const { return ( (base_t::_call)(
				free_standing_call_args::resolve_arg<6, a0_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t>::get( _a0, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg<6, a1_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t>::get( _a1, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg<6, a2_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t>::get( _a2, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg<6, a3_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t>::get( _a3, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg<6, a4_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t>::get( _a4, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg<6, a5_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t>::get( _a5, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg<6, a6_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t>::get( _a6, a0, a1, a2, a3, a4, a5 ) ) ); }
	};

template<typename descriptor, typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t, typename a6_t, typename a7_t>
class HCall<6, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t,
	trait::no_type, trait::no_type>
		: public HCallBase<6, descriptor, return_t, CALL_t, a0_t,
	a1_t, a2_t, a3_t, a4_t,
	a5_t, a6_t, a7_t, trait::no_type,
	trait::no_type, trait::no_type>
	{
	typedef HCallBase<6, descriptor, return_t, CALL_t, a0_t,
	a1_t, a2_t, a3_t, a4_t,
	a5_t, a6_t, a7_t, trait::no_type,
	trait::no_type, trait::no_type> base_t;
	typedef typename descriptor::type::sorted_real_args::a0_t fa0_t;
	typedef typename descriptor::type::sorted_real_args::a1_t fa1_t;
	typedef typename descriptor::type::sorted_real_args::a2_t fa2_t;
	typedef typename descriptor::type::sorted_real_args::a3_t fa3_t;
	typedef typename descriptor::type::sorted_real_args::a4_t fa4_t;
	typedef typename descriptor::type::sorted_real_args::a5_t fa5_t;
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
		: base_t( call_ ), _a0( a0 ), _a1( a1 ), _a2( a2 ), _a3( a3 ), _a4( a4 ), _a5( a5 ), _a6( a6 ), _a7( a7 ) {}
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3, fa4_t a4, fa5_t a5 ) { return ( (base_t::_call)(
				free_standing_call_args::resolve_arg<6, a0_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t>::get( _a0, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg<6, a1_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t>::get( _a1, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg<6, a2_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t>::get( _a2, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg<6, a3_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t>::get( _a3, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg<6, a4_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t>::get( _a4, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg<6, a5_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t>::get( _a5, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg<6, a6_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t>::get( _a6, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg<6, a7_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t>::get( _a7, a0, a1, a2, a3, a4, a5 ) ) ); }
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3, fa4_t a4, fa5_t a5 ) const { return ( (base_t::_call)(
				free_standing_call_args::resolve_arg<6, a0_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t>::get( _a0, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg<6, a1_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t>::get( _a1, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg<6, a2_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t>::get( _a2, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg<6, a3_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t>::get( _a3, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg<6, a4_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t>::get( _a4, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg<6, a5_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t>::get( _a5, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg<6, a6_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t>::get( _a6, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg<6, a7_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t>::get( _a7, a0, a1, a2, a3, a4, a5 ) ) ); }
	};

template<typename descriptor, typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t, typename a6_t, typename a7_t,
	typename a8_t>
class HCall<6, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t,
	trait::no_type>
		: public HCallBase<6, descriptor, return_t, CALL_t, a0_t,
	a1_t, a2_t, a3_t, a4_t,
	a5_t, a6_t, a7_t, a8_t,
	trait::no_type, trait::no_type>
	{
	typedef HCallBase<6, descriptor, return_t, CALL_t, a0_t,
	a1_t, a2_t, a3_t, a4_t,
	a5_t, a6_t, a7_t, a8_t,
	trait::no_type, trait::no_type> base_t;
	typedef typename descriptor::type::sorted_real_args::a0_t fa0_t;
	typedef typename descriptor::type::sorted_real_args::a1_t fa1_t;
	typedef typename descriptor::type::sorted_real_args::a2_t fa2_t;
	typedef typename descriptor::type::sorted_real_args::a3_t fa3_t;
	typedef typename descriptor::type::sorted_real_args::a4_t fa4_t;
	typedef typename descriptor::type::sorted_real_args::a5_t fa5_t;
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
	HCall( CALL_t call_, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7, a8_t a8 )
		: base_t( call_ ), _a0( a0 ), _a1( a1 ), _a2( a2 ), _a3( a3 ), _a4( a4 ), _a5( a5 ), _a6( a6 ), _a7( a7 ), _a8( a8 ) {}
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3, fa4_t a4, fa5_t a5 ) { return ( (base_t::_call)(
				free_standing_call_args::resolve_arg<6, a0_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t>::get( _a0, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg<6, a1_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t>::get( _a1, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg<6, a2_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t>::get( _a2, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg<6, a3_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t>::get( _a3, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg<6, a4_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t>::get( _a4, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg<6, a5_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t>::get( _a5, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg<6, a6_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t>::get( _a6, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg<6, a7_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t>::get( _a7, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg<6, a8_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t>::get( _a8, a0, a1, a2, a3, a4, a5 ) ) ); }
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3, fa4_t a4, fa5_t a5 ) const { return ( (base_t::_call)(
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

template<typename descriptor, typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t, typename a6_t, typename a7_t,
	typename a8_t, typename a9_t>
class HCall<6, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t>
		: public HCallBase<6, descriptor, return_t, CALL_t, a0_t,
	a1_t, a2_t, a3_t, a4_t,
	a5_t, a6_t, a7_t, a8_t,
	a9_t, trait::no_type>
	{
	typedef HCallBase<6, descriptor, return_t, CALL_t, a0_t,
	a1_t, a2_t, a3_t, a4_t,
	a5_t, a6_t, a7_t, a8_t,
	a9_t, trait::no_type> base_t;
	typedef typename descriptor::type::sorted_real_args::a0_t fa0_t;
	typedef typename descriptor::type::sorted_real_args::a1_t fa1_t;
	typedef typename descriptor::type::sorted_real_args::a2_t fa2_t;
	typedef typename descriptor::type::sorted_real_args::a3_t fa3_t;
	typedef typename descriptor::type::sorted_real_args::a4_t fa4_t;
	typedef typename descriptor::type::sorted_real_args::a5_t fa5_t;
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
	HCall( CALL_t call_, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7, a8_t a8, a9_t a9 )
		: base_t( call_ ), _a0( a0 ), _a1( a1 ), _a2( a2 ), _a3( a3 ), _a4( a4 ), _a5( a5 ), _a6( a6 ), _a7( a7 ), _a8( a8 ), _a9( a9 ) {}
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3, fa4_t a4, fa5_t a5 ) { return ( (base_t::_call)(
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
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3, fa4_t a4, fa5_t a5 ) const { return ( (base_t::_call)(
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

template<typename descriptor, typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t, typename a6_t, typename a7_t,
	typename a8_t, typename a9_t, typename a10_t>
class HCall<6, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t, a10_t>
		: public HCallBase<6, descriptor, return_t, CALL_t, a0_t,
	a1_t, a2_t, a3_t, a4_t,
	a5_t, a6_t, a7_t, a8_t,
	a9_t, a10_t>
	{
	typedef HCallBase<6, descriptor, return_t, CALL_t, a0_t,
	a1_t, a2_t, a3_t, a4_t,
	a5_t, a6_t, a7_t, a8_t,
	a9_t, a10_t> base_t;
	typedef typename descriptor::type::sorted_real_args::a0_t fa0_t;
	typedef typename descriptor::type::sorted_real_args::a1_t fa1_t;
	typedef typename descriptor::type::sorted_real_args::a2_t fa2_t;
	typedef typename descriptor::type::sorted_real_args::a3_t fa3_t;
	typedef typename descriptor::type::sorted_real_args::a4_t fa4_t;
	typedef typename descriptor::type::sorted_real_args::a5_t fa5_t;
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
	HCall( CALL_t call_, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7, a8_t a8, a9_t a9, a10_t a10 )
		: base_t( call_ ), _a0( a0 ), _a1( a1 ), _a2( a2 ), _a3( a3 ), _a4( a4 ), _a5( a5 ), _a6( a6 ), _a7( a7 ), _a8( a8 ), _a9( a9 ), _a10( a10 ) {}
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3, fa4_t a4, fa5_t a5 ) { return ( (base_t::_call)(
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
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3, fa4_t a4, fa5_t a5 ) const { return ( (base_t::_call)(
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

template<typename descriptor, typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t, typename a6_t>
class HCall<7, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t,
	trait::no_type, trait::no_type, trait::no_type>
		: public HCallBase<7, descriptor, return_t, CALL_t, a0_t,
	a1_t, a2_t, a3_t, a4_t,
	a5_t, a6_t, trait::no_type, trait::no_type,
	trait::no_type, trait::no_type>
	{
	typedef HCallBase<7, descriptor, return_t, CALL_t, a0_t,
	a1_t, a2_t, a3_t, a4_t,
	a5_t, a6_t, trait::no_type, trait::no_type,
	trait::no_type, trait::no_type> base_t;
	typedef typename descriptor::type::sorted_real_args::a0_t fa0_t;
	typedef typename descriptor::type::sorted_real_args::a1_t fa1_t;
	typedef typename descriptor::type::sorted_real_args::a2_t fa2_t;
	typedef typename descriptor::type::sorted_real_args::a3_t fa3_t;
	typedef typename descriptor::type::sorted_real_args::a4_t fa4_t;
	typedef typename descriptor::type::sorted_real_args::a5_t fa5_t;
	typedef typename descriptor::type::sorted_real_args::a6_t fa6_t;
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
		: base_t( call_ ), _a0( a0 ), _a1( a1 ), _a2( a2 ), _a3( a3 ), _a4( a4 ), _a5( a5 ), _a6( a6 ) {}
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3, fa4_t a4, fa5_t a5, fa6_t a6 ) { return ( (base_t::_call)(
				free_standing_call_args::resolve_arg<7, a0_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t>::get( _a0, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg<7, a1_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t>::get( _a1, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg<7, a2_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t>::get( _a2, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg<7, a3_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t>::get( _a3, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg<7, a4_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t>::get( _a4, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg<7, a5_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t>::get( _a5, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg<7, a6_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t>::get( _a6, a0, a1, a2, a3, a4, a5, a6 ) ) ); }
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3, fa4_t a4, fa5_t a5, fa6_t a6 ) const { return ( (base_t::_call)(
				free_standing_call_args::resolve_arg<7, a0_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t>::get( _a0, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg<7, a1_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t>::get( _a1, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg<7, a2_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t>::get( _a2, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg<7, a3_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t>::get( _a3, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg<7, a4_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t>::get( _a4, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg<7, a5_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t>::get( _a5, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg<7, a6_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t>::get( _a6, a0, a1, a2, a3, a4, a5, a6 ) ) ); }
	};

template<typename descriptor, typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t, typename a6_t, typename a7_t>
class HCall<7, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t,
	trait::no_type, trait::no_type>
		: public HCallBase<7, descriptor, return_t, CALL_t, a0_t,
	a1_t, a2_t, a3_t, a4_t,
	a5_t, a6_t, a7_t, trait::no_type,
	trait::no_type, trait::no_type>
	{
	typedef HCallBase<7, descriptor, return_t, CALL_t, a0_t,
	a1_t, a2_t, a3_t, a4_t,
	a5_t, a6_t, a7_t, trait::no_type,
	trait::no_type, trait::no_type> base_t;
	typedef typename descriptor::type::sorted_real_args::a0_t fa0_t;
	typedef typename descriptor::type::sorted_real_args::a1_t fa1_t;
	typedef typename descriptor::type::sorted_real_args::a2_t fa2_t;
	typedef typename descriptor::type::sorted_real_args::a3_t fa3_t;
	typedef typename descriptor::type::sorted_real_args::a4_t fa4_t;
	typedef typename descriptor::type::sorted_real_args::a5_t fa5_t;
	typedef typename descriptor::type::sorted_real_args::a6_t fa6_t;
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
		: base_t( call_ ), _a0( a0 ), _a1( a1 ), _a2( a2 ), _a3( a3 ), _a4( a4 ), _a5( a5 ), _a6( a6 ), _a7( a7 ) {}
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3, fa4_t a4, fa5_t a5, fa6_t a6 ) { return ( (base_t::_call)(
				free_standing_call_args::resolve_arg<7, a0_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t>::get( _a0, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg<7, a1_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t>::get( _a1, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg<7, a2_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t>::get( _a2, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg<7, a3_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t>::get( _a3, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg<7, a4_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t>::get( _a4, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg<7, a5_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t>::get( _a5, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg<7, a6_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t>::get( _a6, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg<7, a7_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t>::get( _a7, a0, a1, a2, a3, a4, a5, a6 ) ) ); }
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3, fa4_t a4, fa5_t a5, fa6_t a6 ) const { return ( (base_t::_call)(
				free_standing_call_args::resolve_arg<7, a0_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t>::get( _a0, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg<7, a1_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t>::get( _a1, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg<7, a2_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t>::get( _a2, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg<7, a3_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t>::get( _a3, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg<7, a4_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t>::get( _a4, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg<7, a5_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t>::get( _a5, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg<7, a6_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t>::get( _a6, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg<7, a7_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t>::get( _a7, a0, a1, a2, a3, a4, a5, a6 ) ) ); }
	};

template<typename descriptor, typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t, typename a6_t, typename a7_t,
	typename a8_t>
class HCall<7, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t,
	trait::no_type>
		: public HCallBase<7, descriptor, return_t, CALL_t, a0_t,
	a1_t, a2_t, a3_t, a4_t,
	a5_t, a6_t, a7_t, a8_t,
	trait::no_type, trait::no_type>
	{
	typedef HCallBase<7, descriptor, return_t, CALL_t, a0_t,
	a1_t, a2_t, a3_t, a4_t,
	a5_t, a6_t, a7_t, a8_t,
	trait::no_type, trait::no_type> base_t;
	typedef typename descriptor::type::sorted_real_args::a0_t fa0_t;
	typedef typename descriptor::type::sorted_real_args::a1_t fa1_t;
	typedef typename descriptor::type::sorted_real_args::a2_t fa2_t;
	typedef typename descriptor::type::sorted_real_args::a3_t fa3_t;
	typedef typename descriptor::type::sorted_real_args::a4_t fa4_t;
	typedef typename descriptor::type::sorted_real_args::a5_t fa5_t;
	typedef typename descriptor::type::sorted_real_args::a6_t fa6_t;
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
	HCall( CALL_t call_, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7, a8_t a8 )
		: base_t( call_ ), _a0( a0 ), _a1( a1 ), _a2( a2 ), _a3( a3 ), _a4( a4 ), _a5( a5 ), _a6( a6 ), _a7( a7 ), _a8( a8 ) {}
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3, fa4_t a4, fa5_t a5, fa6_t a6 ) { return ( (base_t::_call)(
				free_standing_call_args::resolve_arg<7, a0_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t>::get( _a0, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg<7, a1_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t>::get( _a1, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg<7, a2_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t>::get( _a2, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg<7, a3_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t>::get( _a3, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg<7, a4_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t>::get( _a4, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg<7, a5_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t>::get( _a5, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg<7, a6_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t>::get( _a6, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg<7, a7_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t>::get( _a7, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg<7, a8_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t>::get( _a8, a0, a1, a2, a3, a4, a5, a6 ) ) ); }
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3, fa4_t a4, fa5_t a5, fa6_t a6 ) const { return ( (base_t::_call)(
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

template<typename descriptor, typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t, typename a6_t, typename a7_t,
	typename a8_t, typename a9_t>
class HCall<7, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t>
		: public HCallBase<7, descriptor, return_t, CALL_t, a0_t,
	a1_t, a2_t, a3_t, a4_t,
	a5_t, a6_t, a7_t, a8_t,
	a9_t, trait::no_type>
	{
	typedef HCallBase<7, descriptor, return_t, CALL_t, a0_t,
	a1_t, a2_t, a3_t, a4_t,
	a5_t, a6_t, a7_t, a8_t,
	a9_t, trait::no_type> base_t;
	typedef typename descriptor::type::sorted_real_args::a0_t fa0_t;
	typedef typename descriptor::type::sorted_real_args::a1_t fa1_t;
	typedef typename descriptor::type::sorted_real_args::a2_t fa2_t;
	typedef typename descriptor::type::sorted_real_args::a3_t fa3_t;
	typedef typename descriptor::type::sorted_real_args::a4_t fa4_t;
	typedef typename descriptor::type::sorted_real_args::a5_t fa5_t;
	typedef typename descriptor::type::sorted_real_args::a6_t fa6_t;
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
	HCall( CALL_t call_, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7, a8_t a8, a9_t a9 )
		: base_t( call_ ), _a0( a0 ), _a1( a1 ), _a2( a2 ), _a3( a3 ), _a4( a4 ), _a5( a5 ), _a6( a6 ), _a7( a7 ), _a8( a8 ), _a9( a9 ) {}
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3, fa4_t a4, fa5_t a5, fa6_t a6 ) { return ( (base_t::_call)(
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
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3, fa4_t a4, fa5_t a5, fa6_t a6 ) const { return ( (base_t::_call)(
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

template<typename descriptor, typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t, typename a6_t, typename a7_t,
	typename a8_t, typename a9_t, typename a10_t>
class HCall<7, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t, a10_t>
		: public HCallBase<7, descriptor, return_t, CALL_t, a0_t,
	a1_t, a2_t, a3_t, a4_t,
	a5_t, a6_t, a7_t, a8_t,
	a9_t, a10_t>
	{
	typedef HCallBase<7, descriptor, return_t, CALL_t, a0_t,
	a1_t, a2_t, a3_t, a4_t,
	a5_t, a6_t, a7_t, a8_t,
	a9_t, a10_t> base_t;
	typedef typename descriptor::type::sorted_real_args::a0_t fa0_t;
	typedef typename descriptor::type::sorted_real_args::a1_t fa1_t;
	typedef typename descriptor::type::sorted_real_args::a2_t fa2_t;
	typedef typename descriptor::type::sorted_real_args::a3_t fa3_t;
	typedef typename descriptor::type::sorted_real_args::a4_t fa4_t;
	typedef typename descriptor::type::sorted_real_args::a5_t fa5_t;
	typedef typename descriptor::type::sorted_real_args::a6_t fa6_t;
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
	HCall( CALL_t call_, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7, a8_t a8, a9_t a9, a10_t a10 )
		: base_t( call_ ), _a0( a0 ), _a1( a1 ), _a2( a2 ), _a3( a3 ), _a4( a4 ), _a5( a5 ), _a6( a6 ), _a7( a7 ), _a8( a8 ), _a9( a9 ), _a10( a10 ) {}
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3, fa4_t a4, fa5_t a5, fa6_t a6 ) { return ( (base_t::_call)(
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
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3, fa4_t a4, fa5_t a5, fa6_t a6 ) const { return ( (base_t::_call)(
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

template<typename descriptor, typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t, typename a6_t, typename a7_t>
class HCall<8, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t,
	trait::no_type, trait::no_type>
		: public HCallBase<8, descriptor, return_t, CALL_t, a0_t,
	a1_t, a2_t, a3_t, a4_t,
	a5_t, a6_t, a7_t, trait::no_type,
	trait::no_type, trait::no_type>
	{
	typedef HCallBase<8, descriptor, return_t, CALL_t, a0_t,
	a1_t, a2_t, a3_t, a4_t,
	a5_t, a6_t, a7_t, trait::no_type,
	trait::no_type, trait::no_type> base_t;
	typedef typename descriptor::type::sorted_real_args::a0_t fa0_t;
	typedef typename descriptor::type::sorted_real_args::a1_t fa1_t;
	typedef typename descriptor::type::sorted_real_args::a2_t fa2_t;
	typedef typename descriptor::type::sorted_real_args::a3_t fa3_t;
	typedef typename descriptor::type::sorted_real_args::a4_t fa4_t;
	typedef typename descriptor::type::sorted_real_args::a5_t fa5_t;
	typedef typename descriptor::type::sorted_real_args::a6_t fa6_t;
	typedef typename descriptor::type::sorted_real_args::a7_t fa7_t;
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
		: base_t( call_ ), _a0( a0 ), _a1( a1 ), _a2( a2 ), _a3( a3 ), _a4( a4 ), _a5( a5 ), _a6( a6 ), _a7( a7 ) {}
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3, fa4_t a4, fa5_t a5, fa6_t a6, fa7_t a7 ) { return ( (base_t::_call)(
				free_standing_call_args::resolve_arg<8, a0_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t>::get( _a0, a0, a1, a2, a3, a4, a5, a6, a7 ),
				free_standing_call_args::resolve_arg<8, a1_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t>::get( _a1, a0, a1, a2, a3, a4, a5, a6, a7 ),
				free_standing_call_args::resolve_arg<8, a2_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t>::get( _a2, a0, a1, a2, a3, a4, a5, a6, a7 ),
				free_standing_call_args::resolve_arg<8, a3_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t>::get( _a3, a0, a1, a2, a3, a4, a5, a6, a7 ),
				free_standing_call_args::resolve_arg<8, a4_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t>::get( _a4, a0, a1, a2, a3, a4, a5, a6, a7 ),
				free_standing_call_args::resolve_arg<8, a5_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t>::get( _a5, a0, a1, a2, a3, a4, a5, a6, a7 ),
				free_standing_call_args::resolve_arg<8, a6_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t>::get( _a6, a0, a1, a2, a3, a4, a5, a6, a7 ),
				free_standing_call_args::resolve_arg<8, a7_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t>::get( _a7, a0, a1, a2, a3, a4, a5, a6, a7 ) ) ); }
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3, fa4_t a4, fa5_t a5, fa6_t a6, fa7_t a7 ) const { return ( (base_t::_call)(
				free_standing_call_args::resolve_arg<8, a0_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t>::get( _a0, a0, a1, a2, a3, a4, a5, a6, a7 ),
				free_standing_call_args::resolve_arg<8, a1_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t>::get( _a1, a0, a1, a2, a3, a4, a5, a6, a7 ),
				free_standing_call_args::resolve_arg<8, a2_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t>::get( _a2, a0, a1, a2, a3, a4, a5, a6, a7 ),
				free_standing_call_args::resolve_arg<8, a3_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t>::get( _a3, a0, a1, a2, a3, a4, a5, a6, a7 ),
				free_standing_call_args::resolve_arg<8, a4_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t>::get( _a4, a0, a1, a2, a3, a4, a5, a6, a7 ),
				free_standing_call_args::resolve_arg<8, a5_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t>::get( _a5, a0, a1, a2, a3, a4, a5, a6, a7 ),
				free_standing_call_args::resolve_arg<8, a6_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t>::get( _a6, a0, a1, a2, a3, a4, a5, a6, a7 ),
				free_standing_call_args::resolve_arg<8, a7_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t>::get( _a7, a0, a1, a2, a3, a4, a5, a6, a7 ) ) ); }
	};

template<typename descriptor, typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t, typename a6_t, typename a7_t,
	typename a8_t>
class HCall<8, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t,
	trait::no_type>
		: public HCallBase<8, descriptor, return_t, CALL_t, a0_t,
	a1_t, a2_t, a3_t, a4_t,
	a5_t, a6_t, a7_t, a8_t,
	trait::no_type, trait::no_type>
	{
	typedef HCallBase<8, descriptor, return_t, CALL_t, a0_t,
	a1_t, a2_t, a3_t, a4_t,
	a5_t, a6_t, a7_t, a8_t,
	trait::no_type, trait::no_type> base_t;
	typedef typename descriptor::type::sorted_real_args::a0_t fa0_t;
	typedef typename descriptor::type::sorted_real_args::a1_t fa1_t;
	typedef typename descriptor::type::sorted_real_args::a2_t fa2_t;
	typedef typename descriptor::type::sorted_real_args::a3_t fa3_t;
	typedef typename descriptor::type::sorted_real_args::a4_t fa4_t;
	typedef typename descriptor::type::sorted_real_args::a5_t fa5_t;
	typedef typename descriptor::type::sorted_real_args::a6_t fa6_t;
	typedef typename descriptor::type::sorted_real_args::a7_t fa7_t;
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
	HCall( CALL_t call_, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7, a8_t a8 )
		: base_t( call_ ), _a0( a0 ), _a1( a1 ), _a2( a2 ), _a3( a3 ), _a4( a4 ), _a5( a5 ), _a6( a6 ), _a7( a7 ), _a8( a8 ) {}
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3, fa4_t a4, fa5_t a5, fa6_t a6, fa7_t a7 ) { return ( (base_t::_call)(
				free_standing_call_args::resolve_arg<8, a0_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t>::get( _a0, a0, a1, a2, a3, a4, a5, a6, a7 ),
				free_standing_call_args::resolve_arg<8, a1_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t>::get( _a1, a0, a1, a2, a3, a4, a5, a6, a7 ),
				free_standing_call_args::resolve_arg<8, a2_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t>::get( _a2, a0, a1, a2, a3, a4, a5, a6, a7 ),
				free_standing_call_args::resolve_arg<8, a3_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t>::get( _a3, a0, a1, a2, a3, a4, a5, a6, a7 ),
				free_standing_call_args::resolve_arg<8, a4_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t>::get( _a4, a0, a1, a2, a3, a4, a5, a6, a7 ),
				free_standing_call_args::resolve_arg<8, a5_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t>::get( _a5, a0, a1, a2, a3, a4, a5, a6, a7 ),
				free_standing_call_args::resolve_arg<8, a6_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t>::get( _a6, a0, a1, a2, a3, a4, a5, a6, a7 ),
				free_standing_call_args::resolve_arg<8, a7_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t>::get( _a7, a0, a1, a2, a3, a4, a5, a6, a7 ),
				free_standing_call_args::resolve_arg<8, a8_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t>::get( _a8, a0, a1, a2, a3, a4, a5, a6, a7 ) ) ); }
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3, fa4_t a4, fa5_t a5, fa6_t a6, fa7_t a7 ) const { return ( (base_t::_call)(
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

template<typename descriptor, typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t, typename a6_t, typename a7_t,
	typename a8_t, typename a9_t>
class HCall<8, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t>
		: public HCallBase<8, descriptor, return_t, CALL_t, a0_t,
	a1_t, a2_t, a3_t, a4_t,
	a5_t, a6_t, a7_t, a8_t,
	a9_t, trait::no_type>
	{
	typedef HCallBase<8, descriptor, return_t, CALL_t, a0_t,
	a1_t, a2_t, a3_t, a4_t,
	a5_t, a6_t, a7_t, a8_t,
	a9_t, trait::no_type> base_t;
	typedef typename descriptor::type::sorted_real_args::a0_t fa0_t;
	typedef typename descriptor::type::sorted_real_args::a1_t fa1_t;
	typedef typename descriptor::type::sorted_real_args::a2_t fa2_t;
	typedef typename descriptor::type::sorted_real_args::a3_t fa3_t;
	typedef typename descriptor::type::sorted_real_args::a4_t fa4_t;
	typedef typename descriptor::type::sorted_real_args::a5_t fa5_t;
	typedef typename descriptor::type::sorted_real_args::a6_t fa6_t;
	typedef typename descriptor::type::sorted_real_args::a7_t fa7_t;
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
	HCall( CALL_t call_, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7, a8_t a8, a9_t a9 )
		: base_t( call_ ), _a0( a0 ), _a1( a1 ), _a2( a2 ), _a3( a3 ), _a4( a4 ), _a5( a5 ), _a6( a6 ), _a7( a7 ), _a8( a8 ), _a9( a9 ) {}
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3, fa4_t a4, fa5_t a5, fa6_t a6, fa7_t a7 ) { return ( (base_t::_call)(
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
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3, fa4_t a4, fa5_t a5, fa6_t a6, fa7_t a7 ) const { return ( (base_t::_call)(
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

template<typename descriptor, typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t, typename a6_t, typename a7_t,
	typename a8_t, typename a9_t, typename a10_t>
class HCall<8, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t, a10_t>
		: public HCallBase<8, descriptor, return_t, CALL_t, a0_t,
	a1_t, a2_t, a3_t, a4_t,
	a5_t, a6_t, a7_t, a8_t,
	a9_t, a10_t>
	{
	typedef HCallBase<8, descriptor, return_t, CALL_t, a0_t,
	a1_t, a2_t, a3_t, a4_t,
	a5_t, a6_t, a7_t, a8_t,
	a9_t, a10_t> base_t;
	typedef typename descriptor::type::sorted_real_args::a0_t fa0_t;
	typedef typename descriptor::type::sorted_real_args::a1_t fa1_t;
	typedef typename descriptor::type::sorted_real_args::a2_t fa2_t;
	typedef typename descriptor::type::sorted_real_args::a3_t fa3_t;
	typedef typename descriptor::type::sorted_real_args::a4_t fa4_t;
	typedef typename descriptor::type::sorted_real_args::a5_t fa5_t;
	typedef typename descriptor::type::sorted_real_args::a6_t fa6_t;
	typedef typename descriptor::type::sorted_real_args::a7_t fa7_t;
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
	HCall( CALL_t call_, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7, a8_t a8, a9_t a9, a10_t a10 )
		: base_t( call_ ), _a0( a0 ), _a1( a1 ), _a2( a2 ), _a3( a3 ), _a4( a4 ), _a5( a5 ), _a6( a6 ), _a7( a7 ), _a8( a8 ), _a9( a9 ), _a10( a10 ) {}
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3, fa4_t a4, fa5_t a5, fa6_t a6, fa7_t a7 ) { return ( (base_t::_call)(
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
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3, fa4_t a4, fa5_t a5, fa6_t a6, fa7_t a7 ) const { return ( (base_t::_call)(
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

template<typename descriptor, typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t, typename a6_t, typename a7_t,
	typename a8_t>
class HCall<9, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t,
	trait::no_type>
		: public HCallBase<9, descriptor, return_t, CALL_t, a0_t,
	a1_t, a2_t, a3_t, a4_t,
	a5_t, a6_t, a7_t, a8_t,
	trait::no_type, trait::no_type>
	{
	typedef HCallBase<9, descriptor, return_t, CALL_t, a0_t,
	a1_t, a2_t, a3_t, a4_t,
	a5_t, a6_t, a7_t, a8_t,
	trait::no_type, trait::no_type> base_t;
	typedef typename descriptor::type::sorted_real_args::a0_t fa0_t;
	typedef typename descriptor::type::sorted_real_args::a1_t fa1_t;
	typedef typename descriptor::type::sorted_real_args::a2_t fa2_t;
	typedef typename descriptor::type::sorted_real_args::a3_t fa3_t;
	typedef typename descriptor::type::sorted_real_args::a4_t fa4_t;
	typedef typename descriptor::type::sorted_real_args::a5_t fa5_t;
	typedef typename descriptor::type::sorted_real_args::a6_t fa6_t;
	typedef typename descriptor::type::sorted_real_args::a7_t fa7_t;
	typedef typename descriptor::type::sorted_real_args::a8_t fa8_t;
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
	HCall( CALL_t call_, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7, a8_t a8 )
		: base_t( call_ ), _a0( a0 ), _a1( a1 ), _a2( a2 ), _a3( a3 ), _a4( a4 ), _a5( a5 ), _a6( a6 ), _a7( a7 ), _a8( a8 ) {}
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3, fa4_t a4, fa5_t a5, fa6_t a6, fa7_t a7, fa8_t a8 ) { return ( (base_t::_call)(
				free_standing_call_args::resolve_arg<9, a0_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t>::get( _a0, a0, a1, a2, a3, a4, a5, a6, a7, a8 ),
				free_standing_call_args::resolve_arg<9, a1_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t>::get( _a1, a0, a1, a2, a3, a4, a5, a6, a7, a8 ),
				free_standing_call_args::resolve_arg<9, a2_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t>::get( _a2, a0, a1, a2, a3, a4, a5, a6, a7, a8 ),
				free_standing_call_args::resolve_arg<9, a3_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t>::get( _a3, a0, a1, a2, a3, a4, a5, a6, a7, a8 ),
				free_standing_call_args::resolve_arg<9, a4_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t>::get( _a4, a0, a1, a2, a3, a4, a5, a6, a7, a8 ),
				free_standing_call_args::resolve_arg<9, a5_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t>::get( _a5, a0, a1, a2, a3, a4, a5, a6, a7, a8 ),
				free_standing_call_args::resolve_arg<9, a6_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t>::get( _a6, a0, a1, a2, a3, a4, a5, a6, a7, a8 ),
				free_standing_call_args::resolve_arg<9, a7_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t>::get( _a7, a0, a1, a2, a3, a4, a5, a6, a7, a8 ),
				free_standing_call_args::resolve_arg<9, a8_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t>::get( _a8, a0, a1, a2, a3, a4, a5, a6, a7, a8 ) ) ); }
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3, fa4_t a4, fa5_t a5, fa6_t a6, fa7_t a7, fa8_t a8 ) const { return ( (base_t::_call)(
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

template<typename descriptor, typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t, typename a6_t, typename a7_t,
	typename a8_t, typename a9_t>
class HCall<9, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t>
		: public HCallBase<9, descriptor, return_t, CALL_t, a0_t,
	a1_t, a2_t, a3_t, a4_t,
	a5_t, a6_t, a7_t, a8_t,
	a9_t, trait::no_type>
	{
	typedef HCallBase<9, descriptor, return_t, CALL_t, a0_t,
	a1_t, a2_t, a3_t, a4_t,
	a5_t, a6_t, a7_t, a8_t,
	a9_t, trait::no_type> base_t;
	typedef typename descriptor::type::sorted_real_args::a0_t fa0_t;
	typedef typename descriptor::type::sorted_real_args::a1_t fa1_t;
	typedef typename descriptor::type::sorted_real_args::a2_t fa2_t;
	typedef typename descriptor::type::sorted_real_args::a3_t fa3_t;
	typedef typename descriptor::type::sorted_real_args::a4_t fa4_t;
	typedef typename descriptor::type::sorted_real_args::a5_t fa5_t;
	typedef typename descriptor::type::sorted_real_args::a6_t fa6_t;
	typedef typename descriptor::type::sorted_real_args::a7_t fa7_t;
	typedef typename descriptor::type::sorted_real_args::a8_t fa8_t;
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
	HCall( CALL_t call_, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7, a8_t a8, a9_t a9 )
		: base_t( call_ ), _a0( a0 ), _a1( a1 ), _a2( a2 ), _a3( a3 ), _a4( a4 ), _a5( a5 ), _a6( a6 ), _a7( a7 ), _a8( a8 ), _a9( a9 ) {}
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3, fa4_t a4, fa5_t a5, fa6_t a6, fa7_t a7, fa8_t a8 ) { return ( (base_t::_call)(
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
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3, fa4_t a4, fa5_t a5, fa6_t a6, fa7_t a7, fa8_t a8 ) const { return ( (base_t::_call)(
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

template<typename descriptor, typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t, typename a6_t, typename a7_t,
	typename a8_t, typename a9_t, typename a10_t>
class HCall<9, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t, a10_t>
		: public HCallBase<9, descriptor, return_t, CALL_t, a0_t,
	a1_t, a2_t, a3_t, a4_t,
	a5_t, a6_t, a7_t, a8_t,
	a9_t, a10_t>
	{
	typedef HCallBase<9, descriptor, return_t, CALL_t, a0_t,
	a1_t, a2_t, a3_t, a4_t,
	a5_t, a6_t, a7_t, a8_t,
	a9_t, a10_t> base_t;
	typedef typename descriptor::type::sorted_real_args::a0_t fa0_t;
	typedef typename descriptor::type::sorted_real_args::a1_t fa1_t;
	typedef typename descriptor::type::sorted_real_args::a2_t fa2_t;
	typedef typename descriptor::type::sorted_real_args::a3_t fa3_t;
	typedef typename descriptor::type::sorted_real_args::a4_t fa4_t;
	typedef typename descriptor::type::sorted_real_args::a5_t fa5_t;
	typedef typename descriptor::type::sorted_real_args::a6_t fa6_t;
	typedef typename descriptor::type::sorted_real_args::a7_t fa7_t;
	typedef typename descriptor::type::sorted_real_args::a8_t fa8_t;
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
	HCall( CALL_t call_, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7, a8_t a8, a9_t a9, a10_t a10 )
		: base_t( call_ ), _a0( a0 ), _a1( a1 ), _a2( a2 ), _a3( a3 ), _a4( a4 ), _a5( a5 ), _a6( a6 ), _a7( a7 ), _a8( a8 ), _a9( a9 ), _a10( a10 ) {}
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3, fa4_t a4, fa5_t a5, fa6_t a6, fa7_t a7, fa8_t a8 ) { return ( (base_t::_call)(
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
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3, fa4_t a4, fa5_t a5, fa6_t a6, fa7_t a7, fa8_t a8 ) const { return ( (base_t::_call)(
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

template<typename descriptor, typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t, typename a6_t, typename a7_t,
	typename a8_t, typename a9_t>
class HCall<10, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t>
		: public HCallBase<10, descriptor, return_t, CALL_t, a0_t,
	a1_t, a2_t, a3_t, a4_t,
	a5_t, a6_t, a7_t, a8_t,
	a9_t, trait::no_type>
	{
	typedef HCallBase<10, descriptor, return_t, CALL_t, a0_t,
	a1_t, a2_t, a3_t, a4_t,
	a5_t, a6_t, a7_t, a8_t,
	a9_t, trait::no_type> base_t;
	typedef typename descriptor::type::sorted_real_args::a0_t fa0_t;
	typedef typename descriptor::type::sorted_real_args::a1_t fa1_t;
	typedef typename descriptor::type::sorted_real_args::a2_t fa2_t;
	typedef typename descriptor::type::sorted_real_args::a3_t fa3_t;
	typedef typename descriptor::type::sorted_real_args::a4_t fa4_t;
	typedef typename descriptor::type::sorted_real_args::a5_t fa5_t;
	typedef typename descriptor::type::sorted_real_args::a6_t fa6_t;
	typedef typename descriptor::type::sorted_real_args::a7_t fa7_t;
	typedef typename descriptor::type::sorted_real_args::a8_t fa8_t;
	typedef typename descriptor::type::sorted_real_args::a9_t fa9_t;
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
	HCall( CALL_t call_, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7, a8_t a8, a9_t a9 )
		: base_t( call_ ), _a0( a0 ), _a1( a1 ), _a2( a2 ), _a3( a3 ), _a4( a4 ), _a5( a5 ), _a6( a6 ), _a7( a7 ), _a8( a8 ), _a9( a9 ) {}
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3, fa4_t a4, fa5_t a5, fa6_t a6, fa7_t a7, fa8_t a8, fa9_t a9 ) { return ( (base_t::_call)(
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
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3, fa4_t a4, fa5_t a5, fa6_t a6, fa7_t a7, fa8_t a8, fa9_t a9 ) const { return ( (base_t::_call)(
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

template<typename descriptor, typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t, typename a6_t, typename a7_t,
	typename a8_t, typename a9_t, typename a10_t>
class HCall<10, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t, a10_t>
		: public HCallBase<10, descriptor, return_t, CALL_t, a0_t,
	a1_t, a2_t, a3_t, a4_t,
	a5_t, a6_t, a7_t, a8_t,
	a9_t, a10_t>
	{
	typedef HCallBase<10, descriptor, return_t, CALL_t, a0_t,
	a1_t, a2_t, a3_t, a4_t,
	a5_t, a6_t, a7_t, a8_t,
	a9_t, a10_t> base_t;
	typedef typename descriptor::type::sorted_real_args::a0_t fa0_t;
	typedef typename descriptor::type::sorted_real_args::a1_t fa1_t;
	typedef typename descriptor::type::sorted_real_args::a2_t fa2_t;
	typedef typename descriptor::type::sorted_real_args::a3_t fa3_t;
	typedef typename descriptor::type::sorted_real_args::a4_t fa4_t;
	typedef typename descriptor::type::sorted_real_args::a5_t fa5_t;
	typedef typename descriptor::type::sorted_real_args::a6_t fa6_t;
	typedef typename descriptor::type::sorted_real_args::a7_t fa7_t;
	typedef typename descriptor::type::sorted_real_args::a8_t fa8_t;
	typedef typename descriptor::type::sorted_real_args::a9_t fa9_t;
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
	HCall( CALL_t call_, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7, a8_t a8, a9_t a9, a10_t a10 )
		: base_t( call_ ), _a0( a0 ), _a1( a1 ), _a2( a2 ), _a3( a3 ), _a4( a4 ), _a5( a5 ), _a6( a6 ), _a7( a7 ), _a8( a8 ), _a9( a9 ), _a10( a10 ) {}
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3, fa4_t a4, fa5_t a5, fa6_t a6, fa7_t a7, fa8_t a8, fa9_t a9 ) { return ( (base_t::_call)(
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
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3, fa4_t a4, fa5_t a5, fa6_t a6, fa7_t a7, fa8_t a8, fa9_t a9 ) const { return ( (base_t::_call)(
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

template<typename descriptor, typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t, typename a6_t, typename a7_t,
	typename a8_t, typename a9_t, typename a10_t>
class HCall<11, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t, a10_t>
		: public HCallBase<11, descriptor, return_t, CALL_t, a0_t,
	a1_t, a2_t, a3_t, a4_t,
	a5_t, a6_t, a7_t, a8_t,
	a9_t, a10_t>
	{
	typedef HCallBase<11, descriptor, return_t, CALL_t, a0_t,
	a1_t, a2_t, a3_t, a4_t,
	a5_t, a6_t, a7_t, a8_t,
	a9_t, a10_t> base_t;
	typedef typename descriptor::type::sorted_real_args::a0_t fa0_t;
	typedef typename descriptor::type::sorted_real_args::a1_t fa1_t;
	typedef typename descriptor::type::sorted_real_args::a2_t fa2_t;
	typedef typename descriptor::type::sorted_real_args::a3_t fa3_t;
	typedef typename descriptor::type::sorted_real_args::a4_t fa4_t;
	typedef typename descriptor::type::sorted_real_args::a5_t fa5_t;
	typedef typename descriptor::type::sorted_real_args::a6_t fa6_t;
	typedef typename descriptor::type::sorted_real_args::a7_t fa7_t;
	typedef typename descriptor::type::sorted_real_args::a8_t fa8_t;
	typedef typename descriptor::type::sorted_real_args::a9_t fa9_t;
	typedef typename descriptor::type::sorted_real_args::a10_t fa10_t;
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
	HCall( CALL_t call_, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7, a8_t a8, a9_t a9, a10_t a10 )
		: base_t( call_ ), _a0( a0 ), _a1( a1 ), _a2( a2 ), _a3( a3 ), _a4( a4 ), _a5( a5 ), _a6( a6 ), _a7( a7 ), _a8( a8 ), _a9( a9 ), _a10( a10 ) {}
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3, fa4_t a4, fa5_t a5, fa6_t a6, fa7_t a7, fa8_t a8, fa9_t a9, fa10_t a10 ) { return ( (base_t::_call)(
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
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3, fa4_t a4, fa5_t a5, fa6_t a6, fa7_t a7, fa8_t a8, fa9_t a9, fa10_t a10 ) const { return ( (base_t::_call)(
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

template<typename METHOD_t>
typename call_calculator<METHOD_t>::type::type call( METHOD_t A_METHOD )
	{ return ( call_calculator<METHOD_t>::type::make( A_METHOD ) ); }

template<typename METHOD_t, typename a0_t>
typename call_calculator<METHOD_t, a0_t>::type::type call( METHOD_t A_METHOD, a0_t a0 )
	{ return ( call_calculator<METHOD_t, a0_t>::type::make( A_METHOD, a0 ) ); }

template<typename METHOD_t, typename a0_t, typename a1_t>
typename call_calculator<METHOD_t, a0_t, a1_t>::type::type call( METHOD_t A_METHOD, a0_t a0, a1_t a1 )
	{ return ( call_calculator<METHOD_t, a0_t, a1_t>::type::make( A_METHOD, a0, a1 ) ); }

template<typename METHOD_t, typename a0_t, typename a1_t, typename a2_t>
typename call_calculator<METHOD_t, a0_t, a1_t, a2_t>::type::type call( METHOD_t A_METHOD, a0_t a0, a1_t a1, a2_t a2 )
	{ return ( call_calculator<METHOD_t, a0_t, a1_t, a2_t>::type::make( A_METHOD, a0, a1, a2 ) ); }

template<typename METHOD_t, typename a0_t, typename a1_t, typename a2_t,
	typename a3_t>
typename call_calculator<METHOD_t, a0_t, a1_t, a2_t, a3_t>::type::type call( METHOD_t A_METHOD, a0_t a0, a1_t a1, a2_t a2, a3_t a3 )
	{ return ( call_calculator<METHOD_t, a0_t, a1_t, a2_t, a3_t>::type::make( A_METHOD, a0, a1, a2, a3 ) ); }

template<typename METHOD_t, typename a0_t, typename a1_t, typename a2_t,
	typename a3_t, typename a4_t>
typename call_calculator<METHOD_t, a0_t, a1_t, a2_t, a3_t, a4_t>::type::type call( METHOD_t A_METHOD, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4 )
	{ return ( call_calculator<METHOD_t, a0_t, a1_t, a2_t, a3_t, a4_t>::type::make( A_METHOD, a0, a1, a2, a3, a4 ) ); }

template<typename METHOD_t, typename a0_t, typename a1_t, typename a2_t,
	typename a3_t, typename a4_t, typename a5_t>
typename call_calculator<METHOD_t,
				 a0_t, a1_t, a2_t, a3_t, a4_t, a5_t>::type::type call(
						 METHOD_t A_METHOD, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5 )
	{ return ( call_calculator<METHOD_t,
			a0_t, a1_t, a2_t, a3_t, a4_t, a5_t>::type::make( A_METHOD, a0, a1, a2, a3, a4, a5 ) ); }

template<typename METHOD_t, typename a0_t, typename a1_t, typename a2_t,
	typename a3_t, typename a4_t, typename a5_t, typename a6_t>
typename call_calculator<METHOD_t,
				 a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t>::type::type call(
						 METHOD_t A_METHOD, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6 )
	{ return ( call_calculator<METHOD_t,
			a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t>::type::make( A_METHOD, a0, a1, a2, a3, a4, a5, a6 ) ); }

template<typename METHOD_t, typename a0_t, typename a1_t, typename a2_t,
	typename a3_t, typename a4_t, typename a5_t, typename a6_t, typename a7_t>
typename call_calculator<METHOD_t,
				 a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t>::type::type call(
						 METHOD_t A_METHOD, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7 )
	{ return ( call_calculator<METHOD_t,
			a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t>::type::make( A_METHOD, a0, a1, a2, a3, a4, a5, a6, a7 ) ); }

template<typename METHOD_t, typename a0_t, typename a1_t, typename a2_t,
	typename a3_t, typename a4_t, typename a5_t, typename a6_t, typename a7_t,
	typename a8_t>
typename call_calculator<METHOD_t,
				 a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t>::type::type call(
						 METHOD_t A_METHOD, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7, a8_t a8 )
	{ return ( call_calculator<METHOD_t,
			a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t>::type::make( A_METHOD, a0, a1, a2, a3, a4, a5, a6, a7, a8 ) ); }

template<typename METHOD_t, typename a0_t, typename a1_t, typename a2_t,
	typename a3_t, typename a4_t, typename a5_t, typename a6_t, typename a7_t,
	typename a8_t, typename a9_t>
typename call_calculator<METHOD_t,
				 a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t>::type::type call(
						 METHOD_t A_METHOD, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7, a8_t a8, a9_t a9 )
	{ return ( call_calculator<METHOD_t,
			a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t>::type::make( A_METHOD, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9 ) ); }

template<typename METHOD_t, typename a0_t, typename a1_t, typename a2_t,
	typename a3_t, typename a4_t, typename a5_t, typename a6_t, typename a7_t,
	typename a8_t, typename a9_t, typename a10_t>
typename call_calculator<METHOD_t,
				 a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t, a10_t>::type::type call(
						 METHOD_t A_METHOD, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7, a8_t a8, a9_t a9, a10_t a10 )
	{ return ( call_calculator<METHOD_t,
			a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t, a10_t>::type::make( A_METHOD, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10 ) ); }

}

}

#endif /* not YAAL_HCORE_HCALL_HXX_INCLUDED */

