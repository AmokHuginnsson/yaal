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

template<int, int, typename, typename, typename = trait::no_type, typename = trait::no_type,
	typename = trait::no_type, typename = trait::no_type,
	typename = trait::no_type, typename = trait::no_type,
	typename = trait::no_type, typename = trait::no_type,
	typename = trait::no_type, typename = trait::no_type>
struct getter;

template<int N, typename arg_t, typename fa0_t, typename fa1_t = trait::no_type,
	typename fa2_t = trait::no_type, typename fa3_t = trait::no_type,
	typename fa4_t = trait::no_type, typename fa5_t = trait::no_type,
	typename fa6_t = trait::no_type, typename fa7_t = trait::no_type,
	typename fa8_t = trait::no_type, typename fa9_t = trait::no_type>
struct resolve_arg;

template<typename arg_t, typename fa0_t>
struct resolve_arg<1, arg_t, fa0_t, trait::no_type, trait::no_type, trait::no_type, trait::no_type, trait::no_type, trait::no_type, trait::no_type, trait::no_type, trait::no_type>
	{
	typedef typename trait::ternary<trait::same_type<arg<1>, arg_t>::value, fa0_t, arg_t>::type return_t;
	inline static return_t get( arg_t a, fa0_t fa0 )
		{
		return ( getter<1, trait::find_type<arg_t, arg<1> >::value, arg_t, fa0_t>::get( a, fa0 ) );
		}
	};

template<typename arg_t, typename fa0_t, typename fa1_t>
struct resolve_arg<2, arg_t, fa0_t, fa1_t, trait::no_type, trait::no_type, trait::no_type, trait::no_type, trait::no_type, trait::no_type, trait::no_type, trait::no_type>
	{
	typedef typename trait::ternary<trait::same_type<arg<1>, arg_t>::value, fa0_t,
					typename trait::ternary<trait::same_type<arg<2>, arg_t>::value, fa1_t, arg_t>::type>::type return_t;
	inline static return_t get( arg_t a, fa0_t fa0, fa1_t fa1 )
		{
		return ( getter<2, trait::find_type<arg_t, arg<1>, arg<2> >::value, arg_t, fa0_t, fa1_t>::get( a, fa0, fa1 ) );
		}
	};

template<typename arg_t, typename fa0_t, typename fa1_t, typename fa2_t>
struct resolve_arg<3, arg_t, fa0_t, fa1_t, fa2_t, trait::no_type, trait::no_type, trait::no_type, trait::no_type, trait::no_type, trait::no_type, trait::no_type>
	{
	typedef typename trait::ternary<trait::same_type<arg<1>, arg_t>::value, fa0_t,
					typename trait::ternary<trait::same_type<arg<2>, arg_t>::value, fa1_t,
					typename trait::ternary<trait::same_type<arg<3>, arg_t>::value, fa2_t, arg_t>::type>::type>::type return_t;
	inline static return_t get( arg_t a, fa0_t fa0, fa1_t fa1, fa2_t fa2 )
		{
		return ( getter<3, trait::find_type<arg_t, arg<1>, arg<2>, arg<3> >::value, arg_t, fa0_t, fa1_t, fa2_t>::get( a, fa0, fa1, fa2 ) );
		}
	};

template<typename arg_t, typename fa0_t, typename fa1_t, typename fa2_t, typename fa3_t>
struct resolve_arg<4, arg_t, fa0_t, fa1_t, fa2_t, fa3_t, trait::no_type, trait::no_type, trait::no_type, trait::no_type, trait::no_type, trait::no_type>
	{
	typedef typename trait::ternary<trait::same_type<arg<1>, arg_t>::value, fa0_t,
					typename trait::ternary<trait::same_type<arg<2>, arg_t>::value, fa1_t,
					typename trait::ternary<trait::same_type<arg<3>, arg_t>::value, fa2_t,
					typename trait::ternary<trait::same_type<arg<4>, arg_t>::value, fa3_t,
					arg_t>::type>::type>::type>::type return_t;
	inline static return_t get( arg_t a, fa0_t fa0, fa1_t fa1, fa2_t fa2, fa3_t fa3 )
		{
		return ( getter<4, trait::find_type<arg_t, arg<1>, arg<2>, arg<3>, arg<4> >::value,
				arg_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( a, fa0, fa1, fa2, fa3 ) );
		}
	};

template<typename arg_t, typename fa0_t, typename fa1_t, typename fa2_t, typename fa3_t, typename fa4_t>
struct resolve_arg<5, arg_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, trait::no_type, trait::no_type, trait::no_type, trait::no_type, trait::no_type>
	{
	typedef typename trait::ternary<trait::same_type<arg<1>, arg_t>::value, fa0_t,
					typename trait::ternary<trait::same_type<arg<2>, arg_t>::value, fa1_t,
					typename trait::ternary<trait::same_type<arg<3>, arg_t>::value, fa2_t,
					typename trait::ternary<trait::same_type<arg<4>, arg_t>::value, fa3_t,
					typename trait::ternary<trait::same_type<arg<5>, arg_t>::value, fa4_t,
					arg_t>::type>::type>::type>::type>::type return_t;
	inline static return_t get( arg_t a, fa0_t fa0, fa1_t fa1, fa2_t fa2, fa3_t fa3, fa4_t fa4 )
		{
		return ( getter<5, trait::find_type<arg_t, arg<1>, arg<2>, arg<3>, arg<4>, arg<5> >::value,
				arg_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t>::get( a, fa0, fa1, fa2, fa3, fa4 ) );
		}
	};

template<typename arg_t, typename fa0_t, typename fa1_t, typename fa2_t, typename fa3_t, typename fa4_t, typename fa5_t>
struct resolve_arg<6, arg_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, trait::no_type, trait::no_type, trait::no_type, trait::no_type>
	{
	typedef typename trait::ternary<trait::same_type<arg<1>, arg_t>::value, fa0_t,
					typename trait::ternary<trait::same_type<arg<2>, arg_t>::value, fa1_t,
					typename trait::ternary<trait::same_type<arg<3>, arg_t>::value, fa2_t,
					typename trait::ternary<trait::same_type<arg<4>, arg_t>::value, fa3_t,
					typename trait::ternary<trait::same_type<arg<5>, arg_t>::value, fa4_t,
					typename trait::ternary<trait::same_type<arg<6>, arg_t>::value, fa5_t,
					arg_t>::type>::type>::type>::type>::type>::type return_t;
	inline static return_t get( arg_t a, fa0_t fa0, fa1_t fa1, fa2_t fa2, fa3_t fa3, fa4_t fa4, fa5_t fa5 )
		{
		return ( getter<6, trait::find_type<arg_t, arg<1>, arg<2>, arg<3>, arg<4>, arg<5>, arg<6> >::value,
				arg_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t>::get( a, fa0, fa1, fa2, fa3, fa4, fa5 ) );
		}
	};

template<typename arg_t, typename fa0_t, typename fa1_t, typename fa2_t, typename fa3_t, typename fa4_t, typename fa5_t, typename fa6_t>
struct resolve_arg<7, arg_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, trait::no_type, trait::no_type, trait::no_type>
	{
	typedef typename trait::ternary<trait::same_type<arg<1>, arg_t>::value, fa0_t,
					typename trait::ternary<trait::same_type<arg<2>, arg_t>::value, fa1_t,
					typename trait::ternary<trait::same_type<arg<3>, arg_t>::value, fa2_t,
					typename trait::ternary<trait::same_type<arg<4>, arg_t>::value, fa3_t,
					typename trait::ternary<trait::same_type<arg<5>, arg_t>::value, fa4_t,
					typename trait::ternary<trait::same_type<arg<6>, arg_t>::value, fa5_t,
					typename trait::ternary<trait::same_type<arg<7>, arg_t>::value, fa6_t,
					arg_t>::type>::type>::type>::type>::type>::type>::type return_t;
	inline static return_t get( arg_t a, fa0_t fa0, fa1_t fa1, fa2_t fa2, fa3_t fa3, fa4_t fa4, fa5_t fa5, fa6_t fa6 )
		{
		return ( getter<7, trait::find_type<arg_t, arg<1>, arg<2>, arg<3>, arg<4>, arg<5>, arg<6>, arg<7> >::value,
				arg_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t>::get( a, fa0, fa1, fa2, fa3, fa4, fa5, fa6 ) );
		}
	};

template<typename arg_t, typename fa0_t, typename fa1_t, typename fa2_t, typename fa3_t, typename fa4_t, typename fa5_t, typename fa6_t, typename fa7_t>
struct resolve_arg<8, arg_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, trait::no_type, trait::no_type>
	{
	typedef typename trait::ternary<trait::same_type<arg<1>, arg_t>::value, fa0_t,
					typename trait::ternary<trait::same_type<arg<2>, arg_t>::value, fa1_t,
					typename trait::ternary<trait::same_type<arg<3>, arg_t>::value, fa2_t,
					typename trait::ternary<trait::same_type<arg<4>, arg_t>::value, fa3_t,
					typename trait::ternary<trait::same_type<arg<5>, arg_t>::value, fa4_t,
					typename trait::ternary<trait::same_type<arg<6>, arg_t>::value, fa5_t,
					typename trait::ternary<trait::same_type<arg<7>, arg_t>::value, fa6_t,
					typename trait::ternary<trait::same_type<arg<8>, arg_t>::value, fa7_t,
					arg_t>::type>::type>::type>::type>::type>::type>::type>::type return_t;
	inline static return_t get( arg_t a, fa0_t fa0, fa1_t fa1, fa2_t fa2, fa3_t fa3, fa4_t fa4, fa5_t fa5, fa6_t fa6, fa7_t fa7 )
		{
		return ( getter<8, trait::find_type<arg_t, arg<1>, arg<2>, arg<3>, arg<4>, arg<5>, arg<6>, arg<7>, arg<8> >::value,
				arg_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t>::get( a, fa0, fa1, fa2, fa3, fa4, fa5, fa6, fa7 ) );
		}
	};

template<typename arg_t, typename fa0_t, typename fa1_t, typename fa2_t, typename fa3_t, typename fa4_t, typename fa5_t, typename fa6_t, typename fa7_t, typename fa8_t>
struct resolve_arg<9, arg_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t, trait::no_type>
	{
	typedef typename trait::ternary<trait::same_type<arg<1>, arg_t>::value, fa0_t,
					typename trait::ternary<trait::same_type<arg<2>, arg_t>::value, fa1_t,
					typename trait::ternary<trait::same_type<arg<3>, arg_t>::value, fa2_t,
					typename trait::ternary<trait::same_type<arg<4>, arg_t>::value, fa3_t,
					typename trait::ternary<trait::same_type<arg<5>, arg_t>::value, fa4_t,
					typename trait::ternary<trait::same_type<arg<6>, arg_t>::value, fa5_t,
					typename trait::ternary<trait::same_type<arg<7>, arg_t>::value, fa6_t,
					typename trait::ternary<trait::same_type<arg<8>, arg_t>::value, fa7_t,
					typename trait::ternary<trait::same_type<arg<9>, arg_t>::value, fa8_t,
					arg_t>::type>::type>::type>::type>::type>::type>::type>::type>::type return_t;
	inline static return_t get( arg_t a, fa0_t fa0, fa1_t fa1, fa2_t fa2, fa3_t fa3, fa4_t fa4, fa5_t fa5, fa6_t fa6, fa7_t fa7, fa8_t fa8 )
		{
		return ( getter<9, trait::find_type<arg_t, arg<1>, arg<2>, arg<3>, arg<4>, arg<5>, arg<6>, arg<7>, arg<8>, arg<9> >::value,
				arg_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t>::get( a, fa0, fa1, fa2, fa3, fa4, fa5, fa6, fa7, fa8 ) );
		}
	};

template<typename arg_t, typename fa0_t, typename fa1_t, typename fa2_t, typename fa3_t, typename fa4_t, typename fa5_t, typename fa6_t, typename fa7_t, typename fa8_t, typename fa9_t>
struct resolve_arg<10, arg_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t, fa9_t>
	{
	typedef typename trait::ternary<trait::same_type<arg<1>, arg_t>::value, fa0_t,
					typename trait::ternary<trait::same_type<arg<2>, arg_t>::value, fa1_t,
					typename trait::ternary<trait::same_type<arg<3>, arg_t>::value, fa2_t,
					typename trait::ternary<trait::same_type<arg<4>, arg_t>::value, fa3_t,
					typename trait::ternary<trait::same_type<arg<5>, arg_t>::value, fa4_t,
					typename trait::ternary<trait::same_type<arg<6>, arg_t>::value, fa5_t,
					typename trait::ternary<trait::same_type<arg<7>, arg_t>::value, fa6_t,
					typename trait::ternary<trait::same_type<arg<8>, arg_t>::value, fa7_t,
					typename trait::ternary<trait::same_type<arg<9>, arg_t>::value, fa8_t,
					typename trait::ternary<trait::same_type<arg<-1>, arg_t>::value, fa9_t,
					arg_t>::type>::type>::type>::type>::type>::type>::type>::type>::type>::type return_t;
	inline static return_t get( arg_t a, fa0_t fa0, fa1_t fa1, fa3_t fa3, fa4_t fa4, fa5_t fa5, fa6_t fa6, fa7_t fa7, fa8_t fa8, fa9_t fa9 )
		{
		return ( getter<10, trait::find_type<arg_t, arg<1>, arg<2>, arg<3>, arg<4>, arg<5>, arg<6>, arg<7>, arg<8>, arg<9>, arg<10> >::value,
				arg_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t, fa9_t>::get( a, fa0, fa1, fa3, fa4, fa5, fa6, fa7, fa8, fa9 ) );
		}
	};

template<int N, typename arg_t, typename fa0_t, typename fa1_t, typename fa2_t, typename fa3_t, typename fa4_t, typename fa5_t, typename fa6_t, typename fa7_t, typename fa8_t, typename fa9_t>
struct getter<N, 0, arg_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t, fa9_t>
	{
	inline static typename resolve_arg<N, arg_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t, fa9_t>::return_t get( arg_t, fa0_t fa0, fa1_t = fa1_t(), fa2_t = fa2_t(), fa3_t = fa3_t(), fa4_t = fa4_t(), fa5_t = fa5_t(), fa6_t = fa6_t(), fa7_t = fa7_t(), fa8_t = fa8_t(), fa9_t = fa9_t() )
		{ return ( fa0 ); }
	};

template<int N, typename arg_t, typename fa0_t, typename fa1_t, typename fa2_t, typename fa3_t, typename fa4_t, typename fa5_t, typename fa6_t, typename fa7_t, typename fa8_t, typename fa9_t>
struct getter<N, 1, arg_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t, fa9_t>
	{
	inline static typename resolve_arg<N, arg_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t, fa9_t>::return_t get( arg_t, fa0_t, fa1_t fa1, fa2_t = fa2_t(), fa3_t = fa3_t(), fa4_t = fa4_t(), fa5_t = fa5_t(), fa6_t = fa6_t(), fa7_t = fa7_t(), fa8_t = fa8_t(), fa9_t = fa9_t() )
		{ return ( fa1 ); }
	};

template<int N, typename arg_t, typename fa0_t, typename fa1_t, typename fa2_t, typename fa3_t, typename fa4_t, typename fa5_t, typename fa6_t, typename fa7_t, typename fa8_t, typename fa9_t>
struct getter<N, 2, arg_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t, fa9_t>
	{
	inline static typename resolve_arg<N, arg_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t, fa9_t>::return_t get( arg_t, fa0_t, fa1_t, fa2_t fa2, fa3_t = fa3_t(), fa4_t = fa4_t(), fa5_t = fa5_t(), fa6_t = fa6_t(), fa7_t = fa7_t(), fa8_t = fa8_t(), fa9_t = fa9_t() )
		{ return ( fa2 ); }
	};

template<int N, typename arg_t, typename fa0_t, typename fa1_t, typename fa2_t, typename fa3_t, typename fa4_t, typename fa5_t, typename fa6_t, typename fa7_t, typename fa8_t, typename fa9_t>
struct getter<N, 3, arg_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t, fa9_t>
	{
	inline static typename resolve_arg<N, arg_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t, fa9_t>::return_t get( arg_t, fa0_t, fa1_t, fa2_t, fa3_t fa3, fa4_t = fa4_t(), fa5_t = fa5_t(), fa6_t = fa6_t(), fa7_t = fa7_t(), fa8_t = fa8_t(), fa9_t = fa9_t() )
		{ return ( fa3 ); }
	};

template<int N, typename arg_t, typename fa0_t, typename fa1_t, typename fa2_t, typename fa3_t, typename fa4_t, typename fa5_t, typename fa6_t, typename fa7_t, typename fa8_t, typename fa9_t>
struct getter<N, 4, arg_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t, fa9_t>
	{
	inline static typename resolve_arg<N, arg_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t, fa9_t>::return_t get( arg_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t fa4, fa5_t = fa5_t(), fa6_t = fa6_t(), fa7_t = fa7_t(), fa8_t = fa8_t(), fa9_t = fa9_t() )
		{ return ( fa4 ); }
	};

template<int N, typename arg_t, typename fa0_t, typename fa1_t, typename fa2_t, typename fa3_t, typename fa4_t, typename fa5_t, typename fa6_t, typename fa7_t, typename fa8_t, typename fa9_t>
struct getter<N, 5, arg_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t, fa9_t>
	{
	inline static typename resolve_arg<N, arg_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t, fa9_t>::return_t get( arg_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t fa5, fa6_t = fa6_t(), fa7_t = fa7_t(), fa8_t = fa8_t(), fa9_t = fa9_t() )
		{ return ( fa5 ); }
	};

template<int N, typename arg_t, typename fa0_t, typename fa1_t, typename fa2_t, typename fa3_t, typename fa4_t, typename fa5_t, typename fa6_t, typename fa7_t, typename fa8_t, typename fa9_t>
struct getter<N, 6, arg_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t, fa9_t>
	{
	inline static typename resolve_arg<N, arg_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t, fa9_t>::return_t get( arg_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t fa6, fa7_t = fa7_t(), fa8_t = fa8_t(), fa9_t = fa9_t() )
		{ return ( fa6 ); }
	};

template<int N, typename arg_t, typename fa0_t, typename fa1_t, typename fa2_t, typename fa3_t, typename fa4_t, typename fa5_t, typename fa6_t, typename fa7_t, typename fa8_t, typename fa9_t>
struct getter<N, 7, arg_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t, fa9_t>
	{
	inline static typename resolve_arg<N, arg_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t, fa9_t>::return_t get( arg_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t fa7, fa8_t = fa8_t(), fa9_t = fa9_t() )
		{ return ( fa7 ); }
	};

template<int N, typename arg_t, typename fa0_t, typename fa1_t, typename fa2_t, typename fa3_t, typename fa4_t, typename fa5_t, typename fa6_t, typename fa7_t, typename fa8_t, typename fa9_t>
struct getter<N, 8, arg_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t, fa9_t>
	{
	inline static typename resolve_arg<N, arg_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t, fa9_t>::return_t get( arg_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t fa8, fa9_t = fa9_t() )
		{ return ( fa8 ); }
	};

template<int N, typename arg_t, typename fa0_t, typename fa1_t, typename fa2_t, typename fa3_t, typename fa4_t, typename fa5_t, typename fa6_t, typename fa7_t, typename fa8_t, typename fa9_t>
struct getter<N, 9, arg_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t, fa9_t>
	{
	inline static typename resolve_arg<N, arg_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t, fa9_t>::return_t get( arg_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t, fa9_t fa9 )
		{ return ( fa9 ); }
	};

template<int N, typename arg_t, typename fa0_t, typename fa1_t, typename fa2_t, typename fa3_t, typename fa4_t, typename fa5_t, typename fa6_t, typename fa7_t, typename fa8_t, typename fa9_t>
struct getter<N, -1, arg_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t, fa9_t>
	{
	inline static typename resolve_arg<N, arg_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t, fa9_t>::return_t get( arg_t a, fa0_t, fa1_t = fa1_t(), fa2_t = fa2_t(), fa3_t = fa3_t(), fa4_t = fa4_t(), fa5_t = fa5_t(), fa6_t = fa6_t(), fa7_t = fa7_t(), fa8_t = fa8_t(), fa9_t = fa9_t() )
		{ return ( a ); }
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

struct object_resolver
	{
	typedef enum { REF, PTR, FREE } object_type_t;
	template<typename CLASS_t>
	struct object_type
		{
		static object_type_t const value = static_cast<object_type_t>( meta::ternary<trait::same_type<CLASS_t, free_standing_call_args::arg<1> >::value,
				FREE, meta::ternary<trait::is_pointer<CLASS_t>::value, PTR, REF>::value>::value );
		};
	template<typename return_t, object_type_t>
	struct invoke;

	template<typename return_t>
	struct invoke<return_t, REF>
		{
		template<typename METHOD_t, typename CLASS_t,
			typename a0_t, typename a1_t, typename a2_t, typename a3_t,
			typename a4_t, typename a5_t, typename a6_t, typename a7_t,
			typename a8_t, typename a9_t>
		static return_t go( METHOD_t method_, CLASS_t object_,
			a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4,
			a5_t a5, a6_t a6, a7_t a7, a8_t a8, a9_t a9 )
			{ return ( (object_.*method_)( a0, a1, a2, a3, a4, a5, a6, a7, a8, a9 ) ); }
		template<typename METHOD_t, typename CLASS_t,
			typename a0_t, typename a1_t, typename a2_t, typename a3_t,
			typename a4_t, typename a5_t, typename a6_t, typename a7_t,
			typename a8_t>
		static return_t go( METHOD_t method_, CLASS_t object_,
			a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4,
			a5_t a5, a6_t a6, a7_t a7, a8_t a8 )
			{ return ( (object_.*method_)( a0, a1, a2, a3, a4, a5, a6, a7, a8 ) ); }
		template<typename METHOD_t, typename CLASS_t,
			typename a0_t, typename a1_t, typename a2_t, typename a3_t,
			typename a4_t, typename a5_t, typename a6_t, typename a7_t>
		static return_t go( METHOD_t method_, CLASS_t object_,
			a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4,
			a5_t a5, a6_t a6, a7_t a7 )
			{ return ( (object_.*method_)( a0, a1, a2, a3, a4, a5, a6, a7 ) ); }
		template<typename METHOD_t, typename CLASS_t,
			typename a0_t, typename a1_t, typename a2_t, typename a3_t,
			typename a4_t, typename a5_t, typename a6_t>
		static return_t go( METHOD_t method_, CLASS_t object_,
			a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4,
			a5_t a5, a6_t a6 )
			{ return ( (object_.*method_)( a0, a1, a2, a3, a4, a5, a6 ) ); }
		template<typename METHOD_t, typename CLASS_t,
			typename a0_t, typename a1_t, typename a2_t, typename a3_t,
			typename a4_t, typename a5_t>
		static return_t go( METHOD_t method_, CLASS_t object_,
			a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4,
			a5_t a5 )
			{ return ( (object_.*method_)( a0, a1, a2, a3, a4, a5 ) ); }
		template<typename METHOD_t, typename CLASS_t,
			typename a0_t, typename a1_t, typename a2_t, typename a3_t,
			typename a4_t>
		static return_t go( METHOD_t method_, CLASS_t object_,
			a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4 )
			{ return ( (object_.*method_)( a0, a1, a2, a3, a4 ) ); }
		template<typename METHOD_t, typename CLASS_t,
			typename a0_t, typename a1_t, typename a2_t, typename a3_t>
		static return_t go( METHOD_t method_, CLASS_t object_,
			a0_t a0, a1_t a1, a2_t a2, a3_t a3 )
			{ return ( (object_.*method_)( a0, a1, a2, a3 ) ); }
		template<typename METHOD_t, typename CLASS_t,
			typename a0_t, typename a1_t, typename a2_t>
		static return_t go( METHOD_t method_, CLASS_t object_,
			a0_t a0, a1_t a1, a2_t a2 )
			{ return ( (object_.*method_)( a0, a1, a2 ) ); }
		template<typename METHOD_t, typename CLASS_t,
			typename a0_t, typename a1_t>
		static return_t go( METHOD_t method_, CLASS_t object_,
			a0_t a0, a1_t a1 )
			{ return ( (object_.*method_)( a0, a1 ) ); }
		template<typename METHOD_t, typename CLASS_t,
			typename a0_t>
		static return_t go( METHOD_t method_, CLASS_t object_,
			a0_t a0 )
			{ return ( (object_.*method_)( a0 ) ); }
		template<typename METHOD_t, typename CLASS_t>
		static return_t go( METHOD_t method_, CLASS_t object_ )
			{ return ( (object_.*method_)() ); }
		template<typename CLASS_t>
		static CLASS_t* id( CLASS_t& object_ )
			{ return ( &object_ ); }
		};
	template<typename return_t>
	struct invoke<return_t, PTR>
		{
		template<typename METHOD_t, typename CLASS_t,
			typename a0_t, typename a1_t, typename a2_t, typename a3_t,
			typename a4_t, typename a5_t, typename a6_t, typename a7_t,
			typename a8_t, typename a9_t>
		static return_t go( METHOD_t method_, CLASS_t object_,
			a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4,
			a5_t a5, a6_t a6, a7_t a7, a8_t a8, a9_t a9 )
			{ return ( (object_->*method_)( a0, a1, a2, a3, a4, a5, a6, a7, a8, a9 ) ); }
		template<typename METHOD_t, typename CLASS_t,
			typename a0_t, typename a1_t, typename a2_t, typename a3_t,
			typename a4_t, typename a5_t, typename a6_t, typename a7_t,
			typename a8_t>
		static return_t go( METHOD_t method_, CLASS_t object_,
			a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4,
			a5_t a5, a6_t a6, a7_t a7, a8_t a8 )
			{ return ( (object_->*method_)( a0, a1, a2, a3, a4, a5, a6, a7, a8 ) ); }
		template<typename METHOD_t, typename CLASS_t,
			typename a0_t, typename a1_t, typename a2_t, typename a3_t,
			typename a4_t, typename a5_t, typename a6_t, typename a7_t>
		static return_t go( METHOD_t method_, CLASS_t object_,
			a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4,
			a5_t a5, a6_t a6, a7_t a7 )
			{ return ( (object_->*method_)( a0, a1, a2, a3, a4, a5, a6, a7 ) ); }
		template<typename METHOD_t, typename CLASS_t,
			typename a0_t, typename a1_t, typename a2_t, typename a3_t,
			typename a4_t, typename a5_t, typename a6_t>
		static return_t go( METHOD_t method_, CLASS_t object_,
			a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4,
			a5_t a5, a6_t a6 )
			{ return ( (object_->*method_)( a0, a1, a2, a3, a4, a5, a6 ) ); }
		template<typename METHOD_t, typename CLASS_t,
			typename a0_t, typename a1_t, typename a2_t, typename a3_t,
			typename a4_t, typename a5_t>
		static return_t go( METHOD_t method_, CLASS_t object_,
			a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4,
			a5_t a5 )
			{ return ( (object_->*method_)( a0, a1, a2, a3, a4, a5 ) ); }
		template<typename METHOD_t, typename CLASS_t,
			typename a0_t, typename a1_t, typename a2_t, typename a3_t,
			typename a4_t>
		static return_t go( METHOD_t method_, CLASS_t object_,
			a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4 )
			{ return ( (object_->*method_)( a0, a1, a2, a3, a4 ) ); }
		template<typename METHOD_t, typename CLASS_t,
			typename a0_t, typename a1_t, typename a2_t, typename a3_t>
		static return_t go( METHOD_t method_, CLASS_t object_,
			a0_t a0, a1_t a1, a2_t a2, a3_t a3 )
			{ return ( (object_->*method_)( a0, a1, a2, a3 ) ); }
		template<typename METHOD_t, typename CLASS_t,
			typename a0_t, typename a1_t, typename a2_t>
		static return_t go( METHOD_t method_, CLASS_t object_,
			a0_t a0, a1_t a1, a2_t a2 )
			{ return ( (object_->*method_)( a0, a1, a2 ) ); }
		template<typename METHOD_t, typename CLASS_t,
			typename a0_t, typename a1_t>
		static return_t go( METHOD_t method_, CLASS_t object_,
			a0_t a0, a1_t a1 )
			{ return ( (object_->*method_)( a0, a1 ) ); }
		template<typename METHOD_t, typename CLASS_t,
			typename a0_t>
		static return_t go( METHOD_t method_, CLASS_t object_,
			a0_t a0 )
			{ return ( (object_->*method_)( a0 ) ); }
		template<typename METHOD_t, typename CLASS_t>
		static return_t go( METHOD_t method_, CLASS_t object_ )
			{ return ( (object_->*method_)() ); }
		template<typename CLASS_t>
		static CLASS_t id( CLASS_t object_ )
			{ return ( object_ ); }
		};
	template<typename return_t>
	struct invoke<return_t, FREE>
		{
		template<typename METHOD_t, typename dummy_t, typename CLASS_t,
			typename a0_t, typename a1_t, typename a2_t, typename a3_t,
			typename a4_t, typename a5_t, typename a6_t, typename a7_t,
			typename a8_t, typename a9_t>
		static return_t go( METHOD_t method_, dummy_t, CLASS_t object_,
			a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4,
			a5_t a5, a6_t a6, a7_t a7, a8_t a8, a9_t a9 )
			{ return ( (object_->*method_)( a0, a1, a2, a3, a4, a5, a6, a7, a8, a9 ) ); }
		template<typename METHOD_t, typename dummy_t, typename CLASS_t,
			typename a0_t, typename a1_t, typename a2_t, typename a3_t,
			typename a4_t, typename a5_t, typename a6_t, typename a7_t,
			typename a8_t>
		static return_t go( METHOD_t method_, dummy_t, CLASS_t object_,
			a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4,
			a5_t a5, a6_t a6, a7_t a7, a8_t a8 )
			{ return ( (object_->*method_)( a0, a1, a2, a3, a4, a5, a6, a7, a8 ) ); }
		template<typename METHOD_t, typename dummy_t, typename CLASS_t,
			typename a0_t, typename a1_t, typename a2_t, typename a3_t,
			typename a4_t, typename a5_t, typename a6_t, typename a7_t>
		static return_t go( METHOD_t method_, dummy_t, CLASS_t object_,
			a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4,
			a5_t a5, a6_t a6, a7_t a7 )
			{ return ( (object_->*method_)( a0, a1, a2, a3, a4, a5, a6, a7 ) ); }
		template<typename METHOD_t, typename dummy_t, typename CLASS_t,
			typename a0_t, typename a1_t, typename a2_t, typename a3_t,
			typename a4_t, typename a5_t, typename a6_t>
		static return_t go( METHOD_t method_, dummy_t, CLASS_t object_,
			a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4,
			a5_t a5, a6_t a6 )
			{ return ( (object_->*method_)( a0, a1, a2, a3, a4, a5, a6 ) ); }
		template<typename METHOD_t, typename dummy_t, typename CLASS_t,
			typename a0_t, typename a1_t, typename a2_t, typename a3_t,
			typename a4_t, typename a5_t>
		static return_t go( METHOD_t method_, dummy_t, CLASS_t object_,
			a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4,
			a5_t a5 )
			{ return ( (object_->*method_)( a0, a1, a2, a3, a4, a5 ) ); }
		template<typename METHOD_t, typename dummy_t, typename CLASS_t,
			typename a0_t, typename a1_t, typename a2_t, typename a3_t,
			typename a4_t>
		static return_t go( METHOD_t method_, dummy_t, CLASS_t object_,
			a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4 )
			{ return ( (object_->*method_)( a0, a1, a2, a3, a4 ) ); }
		template<typename METHOD_t, typename dummy_t, typename CLASS_t,
			typename a0_t, typename a1_t, typename a2_t, typename a3_t>
		static return_t go( METHOD_t method_, dummy_t, CLASS_t object_,
			a0_t a0, a1_t a1, a2_t a2, a3_t a3 )
			{ return ( (object_->*method_)( a0, a1, a2, a3 ) ); }
		template<typename METHOD_t, typename dummy_t, typename CLASS_t,
			typename a0_t, typename a1_t, typename a2_t>
		static return_t go( METHOD_t method_, dummy_t, CLASS_t object_,
			a0_t a0, a1_t a1, a2_t a2 )
			{ return ( (object_->*method_)( a0, a1, a2 ) ); }
		template<typename METHOD_t, typename dummy_t, typename CLASS_t,
			typename a0_t, typename a1_t>
		static return_t go( METHOD_t method_, dummy_t, CLASS_t object_,
			a0_t a0, a1_t a1 )
			{ return ( (object_->*method_)( a0, a1 ) ); }
		template<typename METHOD_t, typename dummy_t, typename CLASS_t,
			typename a0_t>
		static return_t go( METHOD_t method_, dummy_t, CLASS_t object_,
			a0_t a0 )
			{ return ( (object_->*method_)( a0 ) ); }
		template<typename METHOD_t, typename dummy_t, typename CLASS_t>
		static return_t go( METHOD_t method_, dummy_t, CLASS_t object_ )
			{ return ( (object_->*method_)() ); }
		template<typename CLASS_t>
		static CLASS_t id( CLASS_t object_ )
			{ return ( object_ ); }
		};
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
		{ return ( object_resolver::invoke<return_t, object_resolver::object_type<CLASS_t>::value>::go( _method, _object, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9 ) ); }
	template<typename a0_t, typename a1_t, typename a2_t, typename a3_t,
		typename a4_t, typename a5_t, typename a6_t, typename a7_t,
		typename a8_t, typename a9_t>
	return_t operator()(
			a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4,
			a5_t a5, a6_t a6, a7_t a7, a8_t a8, a9_t a9 ) const
		{ return ( object_resolver::invoke<return_t, object_resolver::object_type<CLASS_t>::value>::go( _method, _object, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9 ) ); }
	template<typename a0_t, typename a1_t, typename a2_t, typename a3_t,
		typename a4_t, typename a5_t, typename a6_t, typename a7_t,
		typename a8_t>
	return_t operator()(
			a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4,
			a5_t a5, a6_t a6, a7_t a7, a8_t a8 )
		{ return ( object_resolver::invoke<return_t, object_resolver::object_type<CLASS_t>::value>::go( _method, _object, a0, a1, a2, a3, a4, a5, a6, a7, a8 ) ); }
	template<typename a0_t, typename a1_t, typename a2_t, typename a3_t,
		typename a4_t, typename a5_t, typename a6_t, typename a7_t,
		typename a8_t>
	return_t operator()(
			a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4,
			a5_t a5, a6_t a6, a7_t a7, a8_t a8 ) const
		{ return ( object_resolver::invoke<return_t, object_resolver::object_type<CLASS_t>::value>::go( _method, _object, a0, a1, a2, a3, a4, a5, a6, a7, a8 ) ); }
	template<typename a0_t, typename a1_t, typename a2_t, typename a3_t,
		typename a4_t, typename a5_t, typename a6_t, typename a7_t>
	return_t operator()(
			a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4,
			a5_t a5, a6_t a6, a7_t a7 )
		{ return ( object_resolver::invoke<return_t, object_resolver::object_type<CLASS_t>::value>::go( _method, _object, a0, a1, a2, a3, a4, a5, a6, a7 ) ); }
	template<typename a0_t, typename a1_t, typename a2_t, typename a3_t,
		typename a4_t, typename a5_t, typename a6_t, typename a7_t>
	return_t operator()(
			a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4,
			a5_t a5, a6_t a6, a7_t a7 ) const
		{ return ( object_resolver::invoke<return_t, object_resolver::object_type<CLASS_t>::value>::go( _method, _object, a0, a1, a2, a3, a4, a5, a6, a7 ) ); }
	template<typename a0_t, typename a1_t, typename a2_t, typename a3_t,
		typename a4_t, typename a5_t, typename a6_t>
	return_t operator()(
			a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4,
			a5_t a5, a6_t a6 )
		{ return ( object_resolver::invoke<return_t, object_resolver::object_type<CLASS_t>::value>::go( _method, _object, a0, a1, a2, a3, a4, a5, a6 ) ); }
	template<typename a0_t, typename a1_t, typename a2_t, typename a3_t,
		typename a4_t, typename a5_t, typename a6_t>
	return_t operator()(
			a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4,
			a5_t a5, a6_t a6 ) const
		{ return ( object_resolver::invoke<return_t, object_resolver::object_type<CLASS_t>::value>::go( _method, _object, a0, a1, a2, a3, a4, a5, a6 ) ); }
	template<typename a0_t, typename a1_t, typename a2_t, typename a3_t,
		typename a4_t, typename a5_t>
	return_t operator()(
			a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4,
			a5_t a5 )
		{ return ( object_resolver::invoke<return_t, object_resolver::object_type<CLASS_t>::value>::go( _method, _object, a0, a1, a2, a3, a4, a5 ) ); }
	template<typename a0_t, typename a1_t, typename a2_t, typename a3_t,
		typename a4_t, typename a5_t>
	return_t operator()(
			a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4,
			a5_t a5 ) const
		{ return ( object_resolver::invoke<return_t, object_resolver::object_type<CLASS_t>::value>::go( _method, _object, a0, a1, a2, a3, a4, a5 ) ); }
	template<typename a0_t, typename a1_t, typename a2_t, typename a3_t,
		typename a4_t>
	return_t operator()(
			a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4 )
		{ return ( object_resolver::invoke<return_t, object_resolver::object_type<CLASS_t>::value>::go( _method, _object, a0, a1, a2, a3, a4 ) ); }
	template<typename a0_t, typename a1_t, typename a2_t, typename a3_t,
		typename a4_t>
	return_t operator()(
			a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4 ) const
		{ return ( object_resolver::invoke<return_t, object_resolver::object_type<CLASS_t>::value>::go( _method, _object, a0, a1, a2, a3, a4 ) ); }
	template<typename a0_t, typename a1_t, typename a2_t, typename a3_t>
	return_t operator()( a0_t a0, a1_t a1, a2_t a2, a3_t a3 )
		{ return ( object_resolver::invoke<return_t, object_resolver::object_type<CLASS_t>::value>::go( _method, _object, a0, a1, a2, a3 ) ); }
	template<typename a0_t, typename a1_t, typename a2_t, typename a3_t>
	return_t operator()( a0_t a0, a1_t a1, a2_t a2, a3_t a3 ) const
		{ return ( object_resolver::invoke<return_t, object_resolver::object_type<CLASS_t>::value>::go( _method, _object, a0, a1, a2, a3 ) ); }
	template<typename a0_t, typename a1_t, typename a2_t>
	return_t operator()( a0_t a0, a1_t a1, a2_t a2 )
		{ return ( object_resolver::invoke<return_t, object_resolver::object_type<CLASS_t>::value>::go( _method, _object, a0, a1, a2 ) ); }
	template<typename a0_t, typename a1_t, typename a2_t>
	return_t operator()( a0_t a0, a1_t a1, a2_t a2 ) const
		{ return ( object_resolver::invoke<return_t, object_resolver::object_type<CLASS_t>::value>::go( _method, _object, a0, a1, a2 ) ); }
	template<typename a0_t, typename a1_t>
	return_t operator()( a0_t a0, a1_t a1 )
		{ return ( object_resolver::invoke<return_t, object_resolver::object_type<CLASS_t>::value>::go( _method, _object, a0, a1 ) ); }
	template<typename a0_t, typename a1_t>
	return_t operator()( a0_t a0, a1_t a1 ) const
		{ return ( object_resolver::invoke<return_t, object_resolver::object_type<CLASS_t>::value>::go( _method, _object, a0, a1 ) ); }
	template<typename a0_t>
	return_t operator()( a0_t a0 )
		{ return ( object_resolver::invoke<return_t, object_resolver::object_type<CLASS_t>::value>::go( _method, _object, a0 ) ); }
	template<typename a0_t>
	return_t operator()( a0_t a0 ) const
		{ return ( object_resolver::invoke<return_t, object_resolver::object_type<CLASS_t>::value>::go( _method, _object, a0 ) ); }
	return_t operator()( void )
		{ return ( object_resolver::invoke<return_t, object_resolver::object_type<CLASS_t>::value>::go( _method, _object ) ); }
	return_t operator()( void ) const
		{ return ( object_resolver::invoke<return_t, object_resolver::object_type<CLASS_t>::value>::go( _method, _object ) ); }
	void const* id( void ) const
		{ return ( object_resolver::invoke<return_t, object_resolver::object_type<CLASS_t>::value>::id( _object ) ); }
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
	typename a0_t = trait::no_type,	typename a1_t = trait::no_type,
	typename a2_t = trait::no_type,	typename a3_t = trait::no_type,
	typename a4_t = trait::no_type,	typename a5_t = trait::no_type,
	typename a6_t = trait::no_type,	typename a7_t = trait::no_type,
	typename a8_t = trait::no_type,	typename a9_t = trait::no_type>
class HBoundCall;

/*! \cond */
template<typename CALL_t>
class HBoundCall<CALL_t,
	trait::no_type,	trait::no_type,
	trait::no_type,	trait::no_type,
	trait::no_type,	trait::no_type,
	trait::no_type,	trait::no_type,
	trait::no_type,	trait::no_type> : public HBoundCallBase<CALL_t>
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
	trait::no_type,	trait::no_type,
	trait::no_type,	trait::no_type,
	trait::no_type,	trait::no_type,
	trait::no_type,	trait::no_type,
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
	trait::no_type,	trait::no_type,
	trait::no_type,	trait::no_type,
	trait::no_type,	trait::no_type,
	trait::no_type,	trait::no_type> : public HBoundCallBase<CALL_t>
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
	trait::no_type,	trait::no_type,
	trait::no_type,	trait::no_type,
	trait::no_type,	trait::no_type,
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
	trait::no_type,	trait::no_type,
	trait::no_type,	trait::no_type,
	trait::no_type,	trait::no_type> : public HBoundCallBase<CALL_t>
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
	trait::no_type,	trait::no_type,
	trait::no_type,	trait::no_type,
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
	trait::no_type,	trait::no_type,
	trait::no_type,	trait::no_type> : public HBoundCallBase<CALL_t>
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
	trait::no_type,	trait::no_type,
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
	trait::no_type,	trait::no_type> : public HBoundCallBase<CALL_t>
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

template<typename METHOD_t,
	typename a0_t = trait::no_type, typename a1_t = trait::no_type,
	typename a2_t = trait::no_type, typename a3_t = trait::no_type,
	typename a4_t = trait::no_type, typename a5_t = trait::no_type,
	typename a6_t = trait::no_type, typename a7_t = trait::no_type,
	typename a8_t = trait::no_type, typename a9_t = trait::no_type, typename a10_t = trait::no_type>
struct bound_call_calculator
	{
	struct functor
		{
		typedef HFunctor<a0_t, METHOD_t> functor_t;
		typedef HBoundCall<functor_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t, a10_t> type;
		inline static HBoundCallInterface::ptr_t make( METHOD_t m, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7, a8_t a8, a9_t a9, a10_t a10 )
			{ return ( HBoundCallInterface::ptr_t( new type( functor_t( a0, m ), a1, a2, a3, a4, a5, a6, a7, a8, a9, a10 ) ) ); }
		inline static HBoundCallInterface::ptr_t make( METHOD_t m, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7, a8_t a8, a9_t a9 )
			{ return ( HBoundCallInterface::ptr_t( new type( functor_t( a0, m ), a1, a2, a3, a4, a5, a6, a7, a8, a9 ) ) ); }
		inline static HBoundCallInterface::ptr_t make( METHOD_t m, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7, a8_t a8 )
			{ return ( HBoundCallInterface::ptr_t( new type( functor_t( a0, m ), a1, a2, a3, a4, a5, a6, a7, a8 ) ) ); }
		inline static HBoundCallInterface::ptr_t make( METHOD_t m, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7 )
			{ return ( HBoundCallInterface::ptr_t( new type( functor_t( a0, m ), a1, a2, a3, a4, a5, a6, a7 ) ) ); }
		inline static HBoundCallInterface::ptr_t make( METHOD_t m, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6 )
			{ return ( HBoundCallInterface::ptr_t( new type( functor_t( a0, m ), a1, a2, a3, a4, a5, a6 ) ) ); }
		inline static HBoundCallInterface::ptr_t make( METHOD_t m, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5 )
			{ return ( HBoundCallInterface::ptr_t( new type( functor_t( a0, m ), a1, a2, a3, a4, a5 ) ) ); }
		inline static HBoundCallInterface::ptr_t make( METHOD_t m, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4 )
			{ return ( HBoundCallInterface::ptr_t( new type( functor_t( a0, m ), a1, a2, a3, a4 ) ) ); }
		inline static HBoundCallInterface::ptr_t make( METHOD_t m, a0_t a0, a1_t a1, a2_t a2, a3_t a3 )
			{ return ( HBoundCallInterface::ptr_t( new type( functor_t( a0, m ), a1, a2, a3 ) ) ); }
		inline static HBoundCallInterface::ptr_t make( METHOD_t m, a0_t a0, a1_t a1, a2_t a2 )
			{ return ( HBoundCallInterface::ptr_t( new type( functor_t( a0, m ), a1, a2 ) ) ); }
		inline static HBoundCallInterface::ptr_t make( METHOD_t m, a0_t a0, a1_t a1 )
			{ return ( HBoundCallInterface::ptr_t( new type( functor_t( a0, m ), a1 ) ) ); }
		inline static HBoundCallInterface::ptr_t make( METHOD_t m, a0_t a0 )
			{ return ( HBoundCallInterface::ptr_t( new type( functor_t( a0, m ) ) ) ); }
		};
	struct function
		{
		typedef HBoundCall<METHOD_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t> type;
		inline static HBoundCallInterface::ptr_t make( METHOD_t m, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7, a8_t a8, a9_t a9 )
			{ return ( HBoundCallInterface::ptr_t( new type( m, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9 ) ) ); }
		inline static HBoundCallInterface::ptr_t make( METHOD_t m, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7, a8_t a8 )
			{ return ( HBoundCallInterface::ptr_t( new type( m, a0, a1, a2, a3, a4, a5, a6, a7, a8 ) ) ); }
		inline static HBoundCallInterface::ptr_t make( METHOD_t m, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7 )
			{ return ( HBoundCallInterface::ptr_t( new type( m, a0, a1, a2, a3, a4, a5, a6, a7 ) ) ); }
		inline static HBoundCallInterface::ptr_t make( METHOD_t m, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6 )
			{ return ( HBoundCallInterface::ptr_t( new type( m, a0, a1, a2, a3, a4, a5, a6 ) ) ); }
		inline static HBoundCallInterface::ptr_t make( METHOD_t m, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5 )
			{ return ( HBoundCallInterface::ptr_t( new type( m, a0, a1, a2, a3, a4, a5 ) ) ); }
		inline static HBoundCallInterface::ptr_t make( METHOD_t m, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4 )
			{ return ( HBoundCallInterface::ptr_t( new type( m, a0, a1, a2, a3, a4 ) ) ); }
		inline static HBoundCallInterface::ptr_t make( METHOD_t m, a0_t a0, a1_t a1, a2_t a2, a3_t a3 )
			{ return ( HBoundCallInterface::ptr_t( new type( m, a0, a1, a2, a3 ) ) ); }
		inline static HBoundCallInterface::ptr_t make( METHOD_t m, a0_t a0, a1_t a1, a2_t a2 )
			{ return ( HBoundCallInterface::ptr_t( new type( m, a0, a1, a2 ) ) ); }
		inline static HBoundCallInterface::ptr_t make( METHOD_t m, a0_t a0, a1_t a1 )
			{ return ( HBoundCallInterface::ptr_t( new type( m, a0, a1 ) ) ); }
		inline static HBoundCallInterface::ptr_t make( METHOD_t m, a0_t a0 )
			{ return ( HBoundCallInterface::ptr_t( new type( m, a0 ) ) ); }
		inline static HBoundCallInterface::ptr_t make( METHOD_t m )
			{ return ( HBoundCallInterface::ptr_t( new type( m ) ) ); }
		};
	typedef typename trait::ternary<trait::is_member<METHOD_t>::value, functor, function>::type type;
	};

template<typename METHOD_t>
HBoundCallInterface::ptr_t bound_call( METHOD_t A_METHOD )
	{ return ( bound_call_calculator<METHOD_t>::type::make( A_METHOD ) ); }

template<typename METHOD_t, typename a0_t>
HBoundCallInterface::ptr_t bound_call( METHOD_t A_METHOD, a0_t a0 )
	{ return ( bound_call_calculator<METHOD_t, a0_t>::type::make( A_METHOD, a0 ) ); }

template<typename METHOD_t, typename a0_t, typename a1_t>
HBoundCallInterface::ptr_t bound_call( METHOD_t A_METHOD, a0_t a0, a1_t a1 )
	{ return ( bound_call_calculator<METHOD_t, a0_t, a1_t>::type::make( A_METHOD, a0, a1 ) ); }

template<typename METHOD_t, typename a0_t, typename a1_t, typename a2_t>
HBoundCallInterface::ptr_t bound_call( METHOD_t A_METHOD, a0_t a0, a1_t a1, a2_t a2 )
	{ return ( bound_call_calculator<METHOD_t, a0_t, a1_t, a2_t>::type::make( A_METHOD, a0, a1, a2 ) ); }

template<typename METHOD_t, typename a0_t, typename a1_t, typename a2_t,
	typename a3_t>
HBoundCallInterface::ptr_t bound_call( METHOD_t A_METHOD, a0_t a0, a1_t a1, a2_t a2, a3_t a3 )
	{ return ( bound_call_calculator<METHOD_t, a0_t, a1_t, a2_t, a3_t>::type::make( A_METHOD, a0, a1, a2, a3 ) ); }

template<typename METHOD_t, typename a0_t, typename a1_t, typename a2_t,
	typename a3_t, typename a4_t>
HBoundCallInterface::ptr_t bound_call( METHOD_t A_METHOD, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4 )
	{ return ( bound_call_calculator<METHOD_t, a0_t, a1_t, a2_t, a3_t, a4_t>::type::make( A_METHOD, a0, a1, a2, a3, a4 ) ); }

template<typename METHOD_t, typename a0_t, typename a1_t, typename a2_t,
	typename a3_t, typename a4_t, typename a5_t>
HBoundCallInterface::ptr_t bound_call(
						 METHOD_t A_METHOD, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5 )
	{ return ( bound_call_calculator<METHOD_t,
			a0_t, a1_t, a2_t, a3_t, a4_t, a5_t>::type::make( A_METHOD, a0, a1, a2, a3, a4, a5 ) ); }

template<typename METHOD_t, typename a0_t, typename a1_t, typename a2_t,
	typename a3_t, typename a4_t, typename a5_t, typename a6_t>
HBoundCallInterface::ptr_t bound_call(
						 METHOD_t A_METHOD, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6 )
	{ return ( bound_call_calculator<METHOD_t,
			a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t>::type::make( A_METHOD, a0, a1, a2, a3, a4, a5, a6 ) ); }

template<typename METHOD_t, typename a0_t, typename a1_t, typename a2_t,
	typename a3_t, typename a4_t, typename a5_t, typename a6_t, typename a7_t>
HBoundCallInterface::ptr_t bound_call(
						 METHOD_t A_METHOD, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7 )
	{ return ( bound_call_calculator<METHOD_t,
			a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t>::type::make( A_METHOD, a0, a1, a2, a3, a4, a5, a6, a7 ) ); }

template<typename METHOD_t, typename a0_t, typename a1_t, typename a2_t,
	typename a3_t, typename a4_t, typename a5_t, typename a6_t, typename a7_t,
	typename a8_t>
HBoundCallInterface::ptr_t bound_call(
						 METHOD_t A_METHOD, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7, a8_t a8 )
	{ return ( bound_call_calculator<METHOD_t,
			a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t>::type::make( A_METHOD, a0, a1, a2, a3, a4, a5, a6, a7, a8 ) ); }

template<typename METHOD_t, typename a0_t, typename a1_t, typename a2_t,
	typename a3_t, typename a4_t, typename a5_t, typename a6_t, typename a7_t,
	typename a8_t, typename a9_t>
HBoundCallInterface::ptr_t bound_call(
						 METHOD_t A_METHOD, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7, a8_t a8, a9_t a9 )
	{ return ( bound_call_calculator<METHOD_t,
			a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t>::type::make( A_METHOD, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9 ) ); }

template<typename METHOD_t, typename a0_t, typename a1_t, typename a2_t,
	typename a3_t, typename a4_t, typename a5_t, typename a6_t, typename a7_t,
	typename a8_t, typename a9_t, typename a10_t>
HBoundCallInterface::ptr_t bound_call(
						 METHOD_t A_METHOD, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7, a8_t a8, a9_t a9, a10_t a10 )
	{ return ( bound_call_calculator<METHOD_t,
			a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t, a10_t>::type::make( A_METHOD, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10 ) ); }

/*! \brief Implementation of abstration of any-method of any-class invocation.
 *
 * \tparam free_args - how many free standing args this call have.
 * \tparam return_t - call result type.
 * \tparam CALL_t - method or function to be invoked.
 * \tparam a0_t - first argument to given METHOD_t of CALL_t.
 * \tparam a1_t - second argument to given METHOD_t of CALL_t.
 * \tparam aN_t - N-th argument to given METHOD_t of CALL_t.
 */
template<int free_args, typename return_t, typename CALL_t,
	typename a0_t = trait::no_type,	typename a1_t = trait::no_type,
	typename a2_t = trait::no_type,	typename a3_t = trait::no_type,
	typename a4_t = trait::no_type,	typename a5_t = trait::no_type,
	typename a6_t = trait::no_type,	typename a7_t = trait::no_type,
	typename a8_t = trait::no_type,	typename a9_t = trait::no_type>
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
		typedef HCall<free_standing_args::value, typename trait::return_type<METHOD_t>::type,
						functor_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t, a10_t> type;
		inline static type make( METHOD_t m, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7, a8_t a8, a9_t a9, a10_t a10 )
			{ return type( functor_t( a0, m ), a1, a2, a3, a4, a5, a6, a7, a8, a9, a10 ); }
		inline static type make( METHOD_t m, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7, a8_t a8, a9_t a9 )
			{ return type( functor_t( a0, m ), a1, a2, a3, a4, a5, a6, a7, a8, a9 ); }
		inline static type make( METHOD_t m, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7, a8_t a8 )
			{ return type( functor_t( a0, m ), a1, a2, a3, a4, a5, a6, a7, a8 ); }
		inline static type make( METHOD_t m, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7 )
			{ return type( functor_t( a0, m ), a1, a2, a3, a4, a5, a6, a7 ); }
		inline static type make( METHOD_t m, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6 )
			{ return type( functor_t( a0, m ), a1, a2, a3, a4, a5, a6 ); }
		inline static type make( METHOD_t m, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5 )
			{ return type( functor_t( a0, m ), a1, a2, a3, a4, a5 ); }
		inline static type make( METHOD_t m, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4 )
			{ return type( functor_t( a0, m ), a1, a2, a3, a4 ); }
		inline static type make( METHOD_t m, a0_t a0, a1_t a1, a2_t a2, a3_t a3 )
			{ return type( functor_t( a0, m ), a1, a2, a3 ); }
		inline static type make( METHOD_t m, a0_t a0, a1_t a1, a2_t a2 )
			{ return type( functor_t( a0, m ), a1, a2 ); }
		inline static type make( METHOD_t m, a0_t a0, a1_t a1 )
			{ return type( functor_t( a0, m ), a1 ); }
		inline static type make( METHOD_t m, a0_t a0 )
			{ return type( functor_t( a0, m ) ); }
		};
	struct functor_this
		{
		typedef HFunctor<a0_t, METHOD_t> functor_t;
		typedef HCall<free_standing_args::value, typename trait::return_type<METHOD_t>::type,
						functor_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t, a10_t> type;
		inline static type make( METHOD_t m, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7, a8_t a8, a9_t a9, a10_t a10 )
			{ return type( functor_t( a0, m ), a1, a2, a3, a4, a5, a6, a7, a8, a9, a10 ); }
		inline static type make( METHOD_t m, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7, a8_t a8, a9_t a9 )
			{ return type( functor_t( a0, m ), a1, a2, a3, a4, a5, a6, a7, a8, a9 ); }
		inline static type make( METHOD_t m, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7, a8_t a8 )
			{ return type( functor_t( a0, m ), a1, a2, a3, a4, a5, a6, a7, a8 ); }
		inline static type make( METHOD_t m, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7 )
			{ return type( functor_t( a0, m ), a1, a2, a3, a4, a5, a6, a7 ); }
		inline static type make( METHOD_t m, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6 )
			{ return type( functor_t( a0, m ), a1, a2, a3, a4, a5, a6 ); }
		inline static type make( METHOD_t m, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5 )
			{ return type( functor_t( a0, m ), a1, a2, a3, a4, a5 ); }
		inline static type make( METHOD_t m, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4 )
			{ return type( functor_t( a0, m ), a1, a2, a3, a4 ); }
		inline static type make( METHOD_t m, a0_t a0, a1_t a1, a2_t a2, a3_t a3 )
			{ return type( functor_t( a0, m ), a1, a2, a3 ); }
		inline static type make( METHOD_t m, a0_t a0, a1_t a1, a2_t a2 )
			{ return type( functor_t( a0, m ), a1, a2 ); }
		inline static type make( METHOD_t m, a0_t a0, a1_t a1 )
			{ return type( functor_t( a0, m ), a1 ); }
		inline static type make( METHOD_t m, a0_t a0 )
			{ return type( functor_t( a0, m ) ); }
		};
	struct function
		{
		typedef HCall<free_standing_args::value, typename trait::return_type<METHOD_t>::type,
						METHOD_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t> type;
		inline static type make( METHOD_t m, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7, a8_t a8, a9_t a9 )
			{ return type( m, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9 ); }
		inline static type make( METHOD_t m, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7, a8_t a8 )
			{ return type( m, a0, a1, a2, a3, a4, a5, a6, a7, a8 ); }
		inline static type make( METHOD_t m, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7 )
			{ return type( m, a0, a1, a2, a3, a4, a5, a6, a7 ); }
		inline static type make( METHOD_t m, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6 )
			{ return type( m, a0, a1, a2, a3, a4, a5, a6 ); }
		inline static type make( METHOD_t m, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5 )
			{ return type( m, a0, a1, a2, a3, a4, a5 ); }
		inline static type make( METHOD_t m, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4 )
			{ return type( m, a0, a1, a2, a3, a4 ); }
		inline static type make( METHOD_t m, a0_t a0, a1_t a1, a2_t a2, a3_t a3 )
			{ return type( m, a0, a1, a2, a3 ); }
		inline static type make( METHOD_t m, a0_t a0, a1_t a1, a2_t a2 )
			{ return type( m, a0, a1, a2 ); }
		inline static type make( METHOD_t m, a0_t a0, a1_t a1 )
			{ return type( m, a0, a1 ); }
		inline static type make( METHOD_t m, a0_t a0 )
			{ return type( m, a0 ); }
		inline static type make( METHOD_t m )
			{ return type( m ); }
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

/*! \cond */
template<typename return_t, typename CALL_t>
class HCall<0, return_t, CALL_t,
	trait::no_type, trait::no_type, trait::no_type, trait::no_type,
	trait::no_type, trait::no_type, trait::no_type, trait::no_type,
	trait::no_type,	trait::no_type>
	{
	CALL_t _call;
public:
	HCall( CALL_t call_ )
		: _call( call_ ) {}
	return_t operator()( void ) { return ( _call() ); }
	return_t operator()( void ) const { return ( _call() ); }
	};

template<typename return_t, typename CALL_t, typename a0_t>
class HCall<0, return_t, CALL_t, a0_t,
	trait::no_type,	trait::no_type, trait::no_type, trait::no_type,
	trait::no_type,	trait::no_type, trait::no_type, trait::no_type,
	trait::no_type>
	{
	CALL_t _call;
	a0_t _a0;
public:
	HCall( CALL_t call_, a0_t a0 )
		: _call( call_ ), _a0( a0 ) {}
	return_t operator()( void ) { return ( _call( _a0 ) ); }
	return_t operator()( void ) const { return ( _call( _a0 ) ); }
	};

template<typename return_t, typename CALL_t, typename a0_t, typename a1_t>
class HCall<0, return_t, CALL_t, a0_t, a1_t,
	trait::no_type,	trait::no_type, trait::no_type, trait::no_type,
	trait::no_type,	trait::no_type, trait::no_type, trait::no_type>
	{
	CALL_t _call;
	a0_t _a0;
	a1_t _a1;
public:
	HCall( CALL_t call_, a0_t a0, a1_t a1 )
		: _call( call_ ), _a0( a0 ), _a1( a1 ) {}
	return_t operator()( void ) { return ( _call( _a0, _a1 ) ); }
	return_t operator()( void ) const { return ( _call( _a0, _a1 ) ); }
	};

template<typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t>
class HCall<0, return_t, CALL_t, a0_t, a1_t, a2_t,
	trait::no_type,	trait::no_type, trait::no_type, trait::no_type,
	trait::no_type,	trait::no_type, trait::no_type>
	{
	CALL_t _call;
	a0_t _a0;
	a1_t _a1;
	a2_t _a2;
public:
	HCall( CALL_t call_, a0_t a0, a1_t a1, a2_t a2 )
		: _call( call_ ), _a0( a0 ), _a1( a1 ), _a2( a2 ) {}
	return_t operator()( void ) { return ( _call( _a0, _a1, _a2 ) ); }
	return_t operator()( void ) const { return ( _call( _a0, _a1, _a2 ) ); }
	};

template<typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t>
class HCall<0, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t,
	trait::no_type,	trait::no_type, trait::no_type, trait::no_type,
	trait::no_type,	trait::no_type>
	{
	CALL_t _call;
	a0_t _a0;
	a1_t _a1;
	a2_t _a2;
	a3_t _a3;
public:
	HCall( CALL_t call_, a0_t a0, a1_t a1, a2_t a2, a3_t a3 )
		: _call( call_ ), _a0( a0 ), _a1( a1 ), _a2( a2 ), _a3( a3 ) {}
	return_t operator()( void ) { return ( _call( _a0, _a1, _a2, _a3 ) ); }
	return_t operator()( void ) const { return ( _call( _a0, _a1, _a2, _a3 ) ); }
	};

template<typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t>
class HCall<0, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t,
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
	HCall( CALL_t call_, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4 )
		: _call( call_ ), _a0( a0 ), _a1( a1 ), _a2( a2 ), _a3( a3 ), _a4( a4 ) {}
	return_t operator()( void ) { return ( _call( _a0, _a1, _a2, _a3, _a4 ) ); }
	return_t operator()( void ) const { return ( _call( _a0, _a1, _a2, _a3, _a4 ) ); }
	};

template<typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t>
class HCall<0, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t,
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
	HCall( CALL_t call_, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5 )
		: _call( call_ ), _a0( a0 ), _a1( a1 ), _a2( a2 ), _a3( a3 ), _a4( a4 ), _a5( a5 ) {}
	return_t operator()( void ) { return ( _call( _a0, _a1, _a2, _a3, _a4, _a5 ) ); }
	return_t operator()( void ) const { return ( _call( _a0, _a1, _a2, _a3, _a4, _a5 ) ); }
	};

template<typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t, typename a6_t>
class HCall<0, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t,
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
	HCall( CALL_t call_, a0_t a0, a1_t a1,
			a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6 )
		: _call( call_ ), _a0( a0 ), _a1( a1 ), _a2( a2 ), _a3( a3 ), _a4( a4 ), _a5( a5 ), _a6( a6 ) {}
	return_t operator()( void ) { return ( _call( _a0, _a1, _a2, _a3, _a4, _a5, _a6 ) ); }
	return_t operator()( void ) const { return ( _call( _a0, _a1, _a2, _a3, _a4, _a5, _a6 ) ); }
	};

template<typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t, typename a6_t, typename a7_t>
class HCall<0, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t,
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
	HCall( CALL_t call_, a0_t a0, a1_t a1,
			a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7 )
		: _call( call_ ), _a0( a0 ), _a1( a1 ), _a2( a2 ), _a3( a3 ), _a4( a4 ), _a5( a5 ), _a6( a6 ), _a7( a7 ) {}
	return_t operator()( void ) { return ( _call( _a0, _a1, _a2, _a3, _a4, _a5, _a6, _a7 ) ); }
	return_t operator()( void ) const { return ( _call( _a0, _a1, _a2, _a3, _a4, _a5, _a6, _a7 ) ); }
	};

template<typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t, typename a6_t, typename a7_t,
	typename a8_t>
class HCall<0, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t,
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
	HCall( CALL_t call_, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7, a8_t a8 )
		: _call( call_ ), _a0( a0 ), _a1( a1 ), _a2( a2 ), _a3( a3 ), _a4( a4 ), _a5( a5 ), _a6( a6 ), _a7( a7 ), _a8( a8 ) {}
	return_t operator()( void ) { return ( _call( _a0, _a1, _a2, _a3, _a4, _a5, _a6, _a7, _a8 ) ); }
	return_t operator()( void ) const { return ( _call( _a0, _a1, _a2, _a3, _a4, _a5, _a6, _a7, _a8 ) ); }
	};

template<typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t, typename a6_t, typename a7_t,
	typename a8_t, typename a9_t>
class HCall<0, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t>
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
	HCall( CALL_t call_, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7, a8_t a8, a9_t a9 )
		: _call( call_ ), _a0( a0 ), _a1( a1 ), _a2( a2 ), _a3( a3 ), _a4( a4 ), _a5( a5 ), _a6( a6 ), _a7( a7 ), _a8( a8 ), _a9( a9 ) {}
	return_t operator()( void ) { return ( _call( _a0, _a1, _a2, _a3, _a4, _a5, _a6, _a7, _a8, _a9 ) ); }
	return_t operator()( void ) const { return ( _call( _a0, _a1, _a2, _a3, _a4, _a5, _a6, _a7, _a8, _a9 ) ); }
	};

template<typename return_t, typename CALL_t, typename a0_t>
class HCall<1, return_t, CALL_t, a0_t,
	trait::no_type,	trait::no_type, trait::no_type, trait::no_type,
	trait::no_type,	trait::no_type, trait::no_type, trait::no_type,
	trait::no_type>
	{
	CALL_t _call;
	a0_t _a0;
public:
	HCall( CALL_t call_, a0_t a0 )
		: _call( call_ ), _a0( a0 ) {}
	template<typename fa0_t>
	return_t operator()( fa0_t a0 ) { return ( _call( a0 ) ); }
	template<typename fa0_t>
	return_t operator()( fa0_t a0 ) const { return ( _call( a0 ) ); }
	};

template<typename return_t, typename CALL_t, typename a0_t, typename a1_t>
class HCall<1, return_t, CALL_t, a0_t, a1_t,
	trait::no_type,	trait::no_type, trait::no_type, trait::no_type,
	trait::no_type,	trait::no_type, trait::no_type, trait::no_type>
	{
	CALL_t _call;
	a0_t _a0;
	a1_t _a1;
public:
	HCall( CALL_t call_, a0_t a0, a1_t a1 )
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
class HCall<1, return_t, CALL_t, a0_t, a1_t, a2_t,
	trait::no_type,	trait::no_type, trait::no_type, trait::no_type,
	trait::no_type,	trait::no_type, trait::no_type>
	{
	CALL_t _call;
	a0_t _a0;
	a1_t _a1;
	a2_t _a2;
public:
	HCall( CALL_t call_, a0_t a0, a1_t a1, a2_t a2 )
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
class HCall<1, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t,
	trait::no_type,	trait::no_type, trait::no_type, trait::no_type,
	trait::no_type,	trait::no_type>
	{
	CALL_t _call;
	a0_t _a0;
	a1_t _a1;
	a2_t _a2;
	a3_t _a3;
public:
	HCall( CALL_t call_, a0_t a0, a1_t a1, a2_t a2, a3_t a3 )
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
class HCall<1, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t,
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
	HCall( CALL_t call_, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4 )
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
class HCall<1, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t,
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
	HCall( CALL_t call_, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5 )
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
class HCall<1, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t,
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
	HCall( CALL_t call_, a0_t a0, a1_t a1,
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
class HCall<1, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t,
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
	HCall( CALL_t call_, a0_t a0, a1_t a1,
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
class HCall<1, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t,
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
	HCall( CALL_t call_, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7, a8_t a8 )
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
class HCall<1, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t>
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
	HCall( CALL_t call_, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7, a8_t a8, a9_t a9 )
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

template<typename return_t, typename CALL_t, typename a0_t, typename a1_t>
class HCall<2, return_t, CALL_t, a0_t, a1_t,
	trait::no_type,	trait::no_type, trait::no_type, trait::no_type,
	trait::no_type,	trait::no_type, trait::no_type, trait::no_type>
	{
	CALL_t _call;
	a0_t _a0;
	a1_t _a1;
public:
	HCall( CALL_t call_, a0_t a0, a1_t a1 )
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
class HCall<2, return_t, CALL_t, a0_t, a1_t, a2_t,
	trait::no_type,	trait::no_type, trait::no_type, trait::no_type,
	trait::no_type,	trait::no_type, trait::no_type>
	{
	CALL_t _call;
	a0_t _a0;
	a1_t _a1;
	a2_t _a2;
public:
	HCall( CALL_t call_, a0_t a0, a1_t a1, a2_t a2 )
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
class HCall<2, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t,
	trait::no_type,	trait::no_type, trait::no_type, trait::no_type,
	trait::no_type,	trait::no_type>
	{
	CALL_t _call;
	a0_t _a0;
	a1_t _a1;
	a2_t _a2;
	a3_t _a3;
public:
	HCall( CALL_t call_, a0_t a0, a1_t a1, a2_t a2, a3_t a3 )
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
class HCall<2, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t,
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
	HCall( CALL_t call_, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4 )
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
class HCall<2, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t,
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
	HCall( CALL_t call_, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5 )
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
class HCall<2, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t,
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
	HCall( CALL_t call_, a0_t a0, a1_t a1,
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
class HCall<2, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t,
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
	HCall( CALL_t call_, a0_t a0, a1_t a1,
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
class HCall<2, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t,
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
	HCall( CALL_t call_, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7, a8_t a8 )
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
class HCall<2, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t>
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
	HCall( CALL_t call_, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7, a8_t a8, a9_t a9 )
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
	typename a0_t, typename a1_t, typename a2_t>
class HCall<3, return_t, CALL_t, a0_t, a1_t, a2_t,
	trait::no_type,	trait::no_type, trait::no_type, trait::no_type,
	trait::no_type,	trait::no_type, trait::no_type>
	{
	CALL_t _call;
	a0_t _a0;
	a1_t _a1;
	a2_t _a2;
public:
	HCall( CALL_t call_, a0_t a0, a1_t a1, a2_t a2 )
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
class HCall<3, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t,
	trait::no_type,	trait::no_type, trait::no_type, trait::no_type,
	trait::no_type,	trait::no_type>
	{
	CALL_t _call;
	a0_t _a0;
	a1_t _a1;
	a2_t _a2;
	a3_t _a3;
public:
	HCall( CALL_t call_, a0_t a0, a1_t a1, a2_t a2, a3_t a3 )
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
class HCall<3, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t,
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
	HCall( CALL_t call_, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4 )
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
class HCall<3, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t,
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
	HCall( CALL_t call_, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5 )
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
class HCall<3, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t,
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
	HCall( CALL_t call_, a0_t a0, a1_t a1,
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
class HCall<3, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t,
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
	HCall( CALL_t call_, a0_t a0, a1_t a1,
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
class HCall<3, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t,
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
	HCall( CALL_t call_, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7, a8_t a8 )
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
class HCall<3, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t>
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
	HCall( CALL_t call_, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7, a8_t a8, a9_t a9 )
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
	typename a0_t, typename a1_t, typename a2_t, typename a3_t>
class HCall<4, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t,
	trait::no_type,	trait::no_type, trait::no_type, trait::no_type,
	trait::no_type,	trait::no_type>
	{
	CALL_t _call;
	a0_t _a0;
	a1_t _a1;
	a2_t _a2;
	a3_t _a3;
public:
	HCall( CALL_t call_, a0_t a0, a1_t a1, a2_t a2, a3_t a3 )
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
class HCall<4, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t,
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
	HCall( CALL_t call_, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4 )
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
class HCall<4, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t,
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
	HCall( CALL_t call_, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5 )
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
class HCall<4, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t,
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
	HCall( CALL_t call_, a0_t a0, a1_t a1,
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
class HCall<4, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t,
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
	HCall( CALL_t call_, a0_t a0, a1_t a1,
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
class HCall<4, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t,
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
	HCall( CALL_t call_, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7, a8_t a8 )
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
class HCall<4, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t>
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
	HCall( CALL_t call_, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7, a8_t a8, a9_t a9 )
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
	typename a4_t>
class HCall<5, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t,
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
	HCall( CALL_t call_, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4 )
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
class HCall<5, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t,
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
	HCall( CALL_t call_, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5 )
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
class HCall<5, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t,
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
	HCall( CALL_t call_, a0_t a0, a1_t a1,
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
class HCall<5, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t,
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
	HCall( CALL_t call_, a0_t a0, a1_t a1,
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
class HCall<5, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t,
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
	HCall( CALL_t call_, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7, a8_t a8 )
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
class HCall<5, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t>
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
	HCall( CALL_t call_, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7, a8_t a8, a9_t a9 )
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
	typename a4_t, typename a5_t>
class HCall<6, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t,
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
	HCall( CALL_t call_, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5 )
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
class HCall<6, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t,
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
	HCall( CALL_t call_, a0_t a0, a1_t a1,
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
class HCall<6, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t,
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
	HCall( CALL_t call_, a0_t a0, a1_t a1,
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
class HCall<6, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t,
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
	HCall( CALL_t call_, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7, a8_t a8 )
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
class HCall<6, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t>
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
	HCall( CALL_t call_, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7, a8_t a8, a9_t a9 )
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
	typename a4_t, typename a5_t, typename a6_t>
class HCall<7, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t,
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
	HCall( CALL_t call_, a0_t a0, a1_t a1,
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
class HCall<7, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t,
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
	HCall( CALL_t call_, a0_t a0, a1_t a1,
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
class HCall<7, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t,
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
	HCall( CALL_t call_, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7, a8_t a8 )
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
class HCall<7, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t>
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
	HCall( CALL_t call_, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7, a8_t a8, a9_t a9 )
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
	typename a4_t, typename a5_t, typename a6_t, typename a7_t>
class HCall<8, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t,
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
	HCall( CALL_t call_, a0_t a0, a1_t a1,
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
class HCall<8, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t,
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
	HCall( CALL_t call_, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7, a8_t a8 )
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
class HCall<8, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t>
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
	HCall( CALL_t call_, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7, a8_t a8, a9_t a9 )
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
	typename a8_t>
class HCall<9, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t,
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
	HCall( CALL_t call_, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7, a8_t a8 )
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
class HCall<9, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t>
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
	HCall( CALL_t call_, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7, a8_t a8, a9_t a9 )
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
	typename a8_t, typename a9_t>
class HCall<10, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t>
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
	HCall( CALL_t call_, a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7, a8_t a8, a9_t a9 )
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

