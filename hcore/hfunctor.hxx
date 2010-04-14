/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hfunctor.hxx - this file is integral part of `yaal' project.

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
/*! \file hcore/hfunctor.hxx
 * \brief Declaration and implementation of HFunctor<> template.
 */

#ifndef YAAL_HCORE_HFUNCTOR_HXX_INCLUDED
#define YAAL_HCORE_HFUNCTOR_HXX_INCLUDED

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
	typename = trait::no_type, typename = trait::no_type,
	typename = trait::no_type>
struct getter;

template<int N, typename arg_t, typename fa0_t, typename fa1_t = trait::no_type,
	typename fa2_t = trait::no_type, typename fa3_t = trait::no_type,
	typename fa4_t = trait::no_type, typename fa5_t = trait::no_type,
	typename fa6_t = trait::no_type, typename fa7_t = trait::no_type,
	typename fa8_t = trait::no_type, typename fa9_t = trait::no_type,
	typename fa10_t = trait::no_type>
struct resolve_arg;

template<typename arg_t, typename fa0_t, typename fa1_t = trait::no_type,
	typename fa2_t = trait::no_type, typename fa3_t = trait::no_type,
	typename fa4_t = trait::no_type, typename fa5_t = trait::no_type,
	typename fa6_t = trait::no_type, typename fa7_t = trait::no_type,
	typename fa8_t = trait::no_type, typename fa9_t = trait::no_type,
	typename fa10_t = trait::no_type>
struct select
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
					typename trait::ternary<trait::same_type<arg<10>, arg_t>::value, fa9_t,
					typename trait::ternary<trait::same_type<arg<11>, arg_t>::value, fa10_t,
					arg_t>::type>::type>::type>::type>::type>::type>::type>::type>::type>::type>::type type;

	};

template<typename arg_t, typename fa0_t>
struct resolve_arg<1, arg_t, fa0_t, trait::no_type, trait::no_type, trait::no_type, trait::no_type, trait::no_type, trait::no_type, trait::no_type, trait::no_type, trait::no_type>
	{
	typedef typename select<arg_t, fa0_t>::type return_t;
	inline static return_t get( arg_t a, fa0_t fa0 )
		{
		return ( getter<1, trait::find_type<arg_t, arg<1> >::value, arg_t, fa0_t>::get( a, fa0 ) );
		}
	};

template<typename arg_t, typename fa0_t, typename fa1_t>
struct resolve_arg<2, arg_t, fa0_t, fa1_t, trait::no_type, trait::no_type, trait::no_type, trait::no_type, trait::no_type, trait::no_type, trait::no_type, trait::no_type>
	{
	typedef typename select<arg_t, fa0_t, fa1_t>::type return_t;
	inline static return_t get( arg_t a, fa0_t fa0, fa1_t fa1 )
		{
		return ( getter<2, trait::find_type<arg_t, arg<1>, arg<2> >::value, arg_t, fa0_t, fa1_t>::get( a, fa0, fa1 ) );
		}
	};

template<typename arg_t, typename fa0_t, typename fa1_t, typename fa2_t>
struct resolve_arg<3, arg_t, fa0_t, fa1_t, fa2_t, trait::no_type, trait::no_type, trait::no_type, trait::no_type, trait::no_type, trait::no_type, trait::no_type>
	{
	typedef typename select<arg_t, fa0_t, fa1_t, fa2_t>::type return_t;
	inline static return_t get( arg_t a, fa0_t fa0, fa1_t fa1, fa2_t fa2 )
		{
		return ( getter<3, trait::find_type<arg_t, arg<1>, arg<2>, arg<3> >::value, arg_t, fa0_t, fa1_t, fa2_t>::get( a, fa0, fa1, fa2 ) );
		}
	};

template<typename arg_t, typename fa0_t, typename fa1_t, typename fa2_t, typename fa3_t>
struct resolve_arg<4, arg_t, fa0_t, fa1_t, fa2_t, fa3_t, trait::no_type, trait::no_type, trait::no_type, trait::no_type, trait::no_type, trait::no_type>
	{
	typedef typename select<arg_t, fa0_t, fa1_t, fa2_t, fa3_t>::type return_t;
	inline static return_t get( arg_t a, fa0_t fa0, fa1_t fa1, fa2_t fa2, fa3_t fa3 )
		{
		return ( getter<4, trait::find_type<arg_t, arg<1>, arg<2>, arg<3>, arg<4> >::value,
				arg_t, fa0_t, fa1_t, fa2_t, fa3_t>::get( a, fa0, fa1, fa2, fa3 ) );
		}
	};

template<typename arg_t, typename fa0_t, typename fa1_t, typename fa2_t, typename fa3_t, typename fa4_t>
struct resolve_arg<5, arg_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, trait::no_type, trait::no_type, trait::no_type, trait::no_type, trait::no_type>
	{
	typedef typename select<arg_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t>::type return_t;
	inline static return_t get( arg_t a, fa0_t fa0, fa1_t fa1, fa2_t fa2, fa3_t fa3, fa4_t fa4 )
		{
		return ( getter<5, trait::find_type<arg_t, arg<1>, arg<2>, arg<3>, arg<4>, arg<5> >::value,
				arg_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t>::get( a, fa0, fa1, fa2, fa3, fa4 ) );
		}
	};

template<typename arg_t, typename fa0_t, typename fa1_t, typename fa2_t, typename fa3_t, typename fa4_t, typename fa5_t>
struct resolve_arg<6, arg_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, trait::no_type, trait::no_type, trait::no_type, trait::no_type>
	{
	typedef typename select<arg_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t>::type return_t;
	inline static return_t get( arg_t a, fa0_t fa0, fa1_t fa1, fa2_t fa2, fa3_t fa3, fa4_t fa4, fa5_t fa5 )
		{
		return ( getter<6, trait::find_type<arg_t, arg<1>, arg<2>, arg<3>, arg<4>, arg<5>, arg<6> >::value,
				arg_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t>::get( a, fa0, fa1, fa2, fa3, fa4, fa5 ) );
		}
	};

template<typename arg_t, typename fa0_t, typename fa1_t, typename fa2_t, typename fa3_t, typename fa4_t, typename fa5_t, typename fa6_t>
struct resolve_arg<7, arg_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, trait::no_type, trait::no_type, trait::no_type>
	{
	typedef typename select<arg_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t>::type return_t;
	inline static return_t get( arg_t a, fa0_t fa0, fa1_t fa1, fa2_t fa2, fa3_t fa3, fa4_t fa4, fa5_t fa5, fa6_t fa6 )
		{
		return ( getter<7, trait::find_type<arg_t, arg<1>, arg<2>, arg<3>, arg<4>, arg<5>, arg<6>, arg<7> >::value,
				arg_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t>::get( a, fa0, fa1, fa2, fa3, fa4, fa5, fa6 ) );
		}
	};

template<typename arg_t, typename fa0_t, typename fa1_t, typename fa2_t, typename fa3_t, typename fa4_t, typename fa5_t, typename fa6_t, typename fa7_t>
struct resolve_arg<8, arg_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, trait::no_type, trait::no_type>
	{
	typedef typename select<arg_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t>::type return_t;
	inline static return_t get( arg_t a, fa0_t fa0, fa1_t fa1, fa2_t fa2, fa3_t fa3, fa4_t fa4, fa5_t fa5, fa6_t fa6, fa7_t fa7 )
		{
		return ( getter<8, trait::find_type<arg_t, arg<1>, arg<2>, arg<3>, arg<4>, arg<5>, arg<6>, arg<7>, arg<8> >::value,
				arg_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t>::get( a, fa0, fa1, fa2, fa3, fa4, fa5, fa6, fa7 ) );
		}
	};

template<typename arg_t, typename fa0_t, typename fa1_t, typename fa2_t, typename fa3_t, typename fa4_t, typename fa5_t, typename fa6_t, typename fa7_t, typename fa8_t>
struct resolve_arg<9, arg_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t, trait::no_type>
	{
	typedef typename select<arg_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t>::type return_t;
	inline static return_t get( arg_t a, fa0_t fa0, fa1_t fa1, fa2_t fa2, fa3_t fa3, fa4_t fa4, fa5_t fa5, fa6_t fa6, fa7_t fa7, fa8_t fa8 )
		{
		return ( getter<9, trait::find_type<arg_t, arg<1>, arg<2>, arg<3>, arg<4>, arg<5>, arg<6>, arg<7>, arg<8>, arg<9> >::value,
				arg_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t>::get( a, fa0, fa1, fa2, fa3, fa4, fa5, fa6, fa7, fa8 ) );
		}
	};

template<typename arg_t, typename fa0_t, typename fa1_t, typename fa2_t, typename fa3_t, typename fa4_t, typename fa5_t, typename fa6_t, typename fa7_t, typename fa8_t, typename fa9_t>
struct resolve_arg<10, arg_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t, fa9_t>
	{
	typedef typename select<arg_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t, fa9_t>::type return_t;
	inline static return_t get( arg_t a, fa0_t fa0, fa1_t fa1, fa3_t fa3, fa4_t fa4, fa5_t fa5, fa6_t fa6, fa7_t fa7, fa8_t fa8, fa9_t fa9 )
		{
		return ( getter<10, trait::find_type<arg_t, arg<1>, arg<2>, arg<3>, arg<4>, arg<5>, arg<6>, arg<7>, arg<8>, arg<9>, arg<10> >::value,
				arg_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t, fa9_t>::get( a, fa0, fa1, fa3, fa4, fa5, fa6, fa7, fa8, fa9 ) );
		}
	};

template<typename arg_t, typename fa0_t, typename fa1_t, typename fa2_t, typename fa3_t, typename fa4_t, typename fa5_t, typename fa6_t, typename fa7_t, typename fa8_t, typename fa9_t, typename fa10_t>
struct resolve_arg<11, arg_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t, fa9_t, fa10_t>
	{
	typedef typename select<arg_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t, fa9_t, fa10_t>::type return_t;
	inline static return_t get( arg_t a, fa0_t fa0, fa1_t fa1, fa3_t fa3, fa4_t fa4, fa5_t fa5, fa6_t fa6, fa7_t fa7, fa8_t fa8, fa9_t fa9, fa10_t fa10 )
		{
		return ( getter<11, trait::find_type<arg_t, arg<1>, arg<2>, arg<3>, arg<4>, arg<5>, arg<6>, arg<7>, arg<8>, arg<9>, arg<10>, arg<11> >::value,
				arg_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t, fa9_t, fa10_t>::get( a, fa0, fa1, fa3, fa4, fa5, fa6, fa7, fa8, fa9, fa10 ) );
		}
	};

template<int N, typename arg_t, typename fa0_t, typename fa1_t, typename fa2_t, typename fa3_t, typename fa4_t, typename fa5_t, typename fa6_t, typename fa7_t, typename fa8_t, typename fa9_t, typename fa10_t>
struct getter<N, 0, arg_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t, fa9_t, fa10_t>
	{
	inline static typename resolve_arg<N, arg_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t, fa9_t, fa10_t>::return_t get( arg_t, fa0_t fa0, fa1_t = fa1_t(), fa2_t = fa2_t(), fa3_t = fa3_t(), fa4_t = fa4_t(), fa5_t = fa5_t(), fa6_t = fa6_t(), fa7_t = fa7_t(), fa8_t = fa8_t(), fa9_t = fa9_t(), fa10_t = fa10_t() )
		{ return ( fa0 ); }
	};

template<int N, typename arg_t, typename fa0_t, typename fa1_t, typename fa2_t, typename fa3_t, typename fa4_t, typename fa5_t, typename fa6_t, typename fa7_t, typename fa8_t, typename fa9_t, typename fa10_t>
struct getter<N, 1, arg_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t, fa9_t, fa10_t>
	{
	inline static typename resolve_arg<N, arg_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t, fa9_t, fa10_t>::return_t get( arg_t, fa0_t, fa1_t fa1, fa2_t = fa2_t(), fa3_t = fa3_t(), fa4_t = fa4_t(), fa5_t = fa5_t(), fa6_t = fa6_t(), fa7_t = fa7_t(), fa8_t = fa8_t(), fa9_t = fa9_t(), fa10_t = fa10_t() )
		{ return ( fa1 ); }
	};

template<int N, typename arg_t, typename fa0_t, typename fa1_t, typename fa2_t, typename fa3_t, typename fa4_t, typename fa5_t, typename fa6_t, typename fa7_t, typename fa8_t, typename fa9_t, typename fa10_t>
struct getter<N, 2, arg_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t, fa9_t, fa10_t>
	{
	inline static typename resolve_arg<N, arg_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t, fa9_t, fa10_t>::return_t get( arg_t, fa0_t, fa1_t, fa2_t fa2, fa3_t = fa3_t(), fa4_t = fa4_t(), fa5_t = fa5_t(), fa6_t = fa6_t(), fa7_t = fa7_t(), fa8_t = fa8_t(), fa9_t = fa9_t(), fa10_t = fa10_t() )
		{ return ( fa2 ); }
	};

template<int N, typename arg_t, typename fa0_t, typename fa1_t, typename fa2_t, typename fa3_t, typename fa4_t, typename fa5_t, typename fa6_t, typename fa7_t, typename fa8_t, typename fa9_t, typename fa10_t>
struct getter<N, 3, arg_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t, fa9_t, fa10_t>
	{
	inline static typename resolve_arg<N, arg_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t, fa9_t, fa10_t>::return_t get( arg_t, fa0_t, fa1_t, fa2_t, fa3_t fa3, fa4_t = fa4_t(), fa5_t = fa5_t(), fa6_t = fa6_t(), fa7_t = fa7_t(), fa8_t = fa8_t(), fa9_t = fa9_t(), fa10_t = fa10_t() )
		{ return ( fa3 ); }
	};

template<int N, typename arg_t, typename fa0_t, typename fa1_t, typename fa2_t, typename fa3_t, typename fa4_t, typename fa5_t, typename fa6_t, typename fa7_t, typename fa8_t, typename fa9_t, typename fa10_t>
struct getter<N, 4, arg_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t, fa9_t, fa10_t>
	{
	inline static typename resolve_arg<N, arg_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t, fa9_t, fa10_t>::return_t get( arg_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t fa4, fa5_t = fa5_t(), fa6_t = fa6_t(), fa7_t = fa7_t(), fa8_t = fa8_t(), fa9_t = fa9_t(), fa10_t = fa10_t() )
		{ return ( fa4 ); }
	};

template<int N, typename arg_t, typename fa0_t, typename fa1_t, typename fa2_t, typename fa3_t, typename fa4_t, typename fa5_t, typename fa6_t, typename fa7_t, typename fa8_t, typename fa9_t, typename fa10_t>
struct getter<N, 5, arg_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t, fa9_t, fa10_t>
	{
	inline static typename resolve_arg<N, arg_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t, fa9_t, fa10_t>::return_t get( arg_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t fa5, fa6_t = fa6_t(), fa7_t = fa7_t(), fa8_t = fa8_t(), fa9_t = fa9_t(), fa10_t = fa10_t() )
		{ return ( fa5 ); }
	};

template<int N, typename arg_t, typename fa0_t, typename fa1_t, typename fa2_t, typename fa3_t, typename fa4_t, typename fa5_t, typename fa6_t, typename fa7_t, typename fa8_t, typename fa9_t, typename fa10_t>
struct getter<N, 6, arg_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t, fa9_t, fa10_t>
	{
	inline static typename resolve_arg<N, arg_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t, fa9_t, fa10_t>::return_t get( arg_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t fa6, fa7_t = fa7_t(), fa8_t = fa8_t(), fa9_t = fa9_t(), fa10_t = fa10_t() )
		{ return ( fa6 ); }
	};

template<int N, typename arg_t, typename fa0_t, typename fa1_t, typename fa2_t, typename fa3_t, typename fa4_t, typename fa5_t, typename fa6_t, typename fa7_t, typename fa8_t, typename fa9_t, typename fa10_t>
struct getter<N, 7, arg_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t, fa9_t, fa10_t>
	{
	inline static typename resolve_arg<N, arg_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t, fa9_t, fa10_t>::return_t get( arg_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t fa7, fa8_t = fa8_t(), fa9_t = fa9_t(), fa10_t = fa10_t() )
		{ return ( fa7 ); }
	};

template<int N, typename arg_t, typename fa0_t, typename fa1_t, typename fa2_t, typename fa3_t, typename fa4_t, typename fa5_t, typename fa6_t, typename fa7_t, typename fa8_t, typename fa9_t, typename fa10_t>
struct getter<N, 8, arg_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t, fa9_t, fa10_t>
	{
	inline static typename resolve_arg<N, arg_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t, fa9_t, fa10_t>::return_t get( arg_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t fa8, fa9_t = fa9_t(), fa10_t = fa10_t() )
		{ return ( fa8 ); }
	};

template<int N, typename arg_t, typename fa0_t, typename fa1_t, typename fa2_t, typename fa3_t, typename fa4_t, typename fa5_t, typename fa6_t, typename fa7_t, typename fa8_t, typename fa9_t, typename fa10_t>
struct getter<N, 9, arg_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t, fa9_t, fa10_t>
	{
	inline static typename resolve_arg<N, arg_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t, fa9_t, fa10_t>::return_t get( arg_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t, fa9_t fa9, fa10_t = fa10_t() )
		{ return ( fa9 ); }
	};

template<int N, typename arg_t, typename fa0_t, typename fa1_t, typename fa2_t, typename fa3_t, typename fa4_t, typename fa5_t, typename fa6_t, typename fa7_t, typename fa8_t, typename fa9_t, typename fa10_t>
struct getter<N, 10, arg_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t, fa9_t, fa10_t>
	{
	inline static typename resolve_arg<N, arg_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t, fa9_t, fa10_t>::return_t get( arg_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t, fa9_t, fa10_t fa10 )
		{ return ( fa10 ); }
	};

template<int N, typename arg_t, typename fa0_t, typename fa1_t, typename fa2_t, typename fa3_t, typename fa4_t, typename fa5_t, typename fa6_t, typename fa7_t, typename fa8_t, typename fa9_t, typename fa10_t>
struct getter<N, -1, arg_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t, fa9_t, fa10_t>
	{
	inline static typename resolve_arg<N, arg_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t, fa9_t, fa10_t>::return_t get( arg_t a, fa0_t, fa1_t = fa1_t(), fa2_t = fa2_t(), fa3_t = fa3_t(), fa4_t = fa4_t(), fa5_t = fa5_t(), fa6_t = fa6_t(), fa7_t = fa7_t(), fa8_t = fa8_t(), fa9_t = fa9_t() )
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
static const free_standing_call_args::arg<11> _11;

}

struct object_resolver
	{
	typedef enum { REF, PTR, FREE } object_type_t;
	template<typename CLASS_t>
	struct object_type
		{
		static object_type_t const value = static_cast<object_type_t>( meta::ternary<meta::greater<trait::find_type<CLASS_t,
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
		template<typename CLASS_t, typename dummy_t>
		static CLASS_t* id( CLASS_t& object_, dummy_t )
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
		template<typename CLASS_t, typename dummy_t>
		static CLASS_t id( CLASS_t object_, dummy_t )
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
		template<typename dummy_t, typename CLASS_t>
		static CLASS_t id( dummy_t, CLASS_t object_ )
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
		{ return ( reinterpret_cast<void const*>( object_resolver::invoke<return_t, object_resolver::object_type<CLASS_t>::value>::id( _object, _method ) ) ); }
	};

}

}

#endif /* not YAAL_HCORE_HFUNCTOR_HXX_INCLUDED */

