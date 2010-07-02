/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	bound.hxx - this file is integral part of `yaal' project.

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

/*! \file hcore/bound.hxx
 * \brief HCall helpers for algorithms.
 */

#ifndef YAAL_HCORE_BOUND_HXX_INCLUDED
#define YAAL_HCORE_BOUND_HXX_INCLUDED

#include "hcore/hcall.hxx"

namespace yaal
{

namespace hcore
{

template<int free_args, typename descriptor_t, typename return_t, typename CALL_t,	typename a0_t, typename a1_t,
	typename a2_t, typename a3_t,	typename a4_t, typename a5_t,	typename a6_t, typename a7_t,
	typename a8_t, typename a9_t,	typename a10_t,
	int free_args2, typename descriptor2_t, typename return2_t, typename CALL2_t, typename b0_t, typename b1_t,
	typename b2_t, typename b3_t,	typename b4_t, typename b5_t,	typename b6_t, typename b7_t,
	typename b8_t, typename b9_t,	typename b10_t>
typename call_calculator<
	binary_composition<
		equal_to<return_t>,
		HCall<free_args, descriptor_t, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t, a10_t>,
		HCall<free_args2, descriptor2_t, return2_t, CALL2_t, b0_t, b1_t, b2_t, b3_t, b4_t, b5_t, b6_t, b7_t, b8_t, b9_t, b10_t>
	>,
	free_standing_call_args::arg<1>,
	free_standing_call_args::arg<2>
>::type::type
	operator == ( HCall<free_args, descriptor_t, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t, a10_t> const& left_,
			HCall<free_args2, descriptor2_t, return2_t, CALL2_t, b0_t, b1_t, b2_t, b3_t, b4_t, b5_t, b6_t, b7_t, b8_t, b9_t, b10_t> const& right_ )
	{
	return ( call( compose_binary( equal_to<return_t>(), left_, right_ ), _1, _2 ) );
	}

template<int free_args, typename descriptor_t, typename return_t, typename CALL_t,	typename a0_t, typename a1_t,
	typename a2_t, typename a3_t,	typename a4_t, typename a5_t,	typename a6_t, typename a7_t,
	typename a8_t, typename a9_t,	typename a10_t,
	int free_args2, typename descriptor2_t, typename return2_t, typename CALL2_t, typename b0_t, typename b1_t,
	typename b2_t, typename b3_t,	typename b4_t, typename b5_t,	typename b6_t, typename b7_t,
	typename b8_t, typename b9_t,	typename b10_t>
typename call_calculator<
	binary_composition<
		not_equal_to<return_t>,
		HCall<free_args, descriptor_t, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t, a10_t>,
		HCall<free_args2, descriptor2_t, return2_t, CALL2_t, b0_t, b1_t, b2_t, b3_t, b4_t, b5_t, b6_t, b7_t, b8_t, b9_t, b10_t>
	>,
	free_standing_call_args::arg<1>,
	free_standing_call_args::arg<2>
>::type::type
	operator != ( HCall<free_args, descriptor_t, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t, a10_t> const& left_,
			HCall<free_args2, descriptor2_t, return2_t, CALL2_t, b0_t, b1_t, b2_t, b3_t, b4_t, b5_t, b6_t, b7_t, b8_t, b9_t, b10_t> const& right_ )
	{
	return ( call( compose_binary( not_equal_to<return_t>(), left_, right_ ), _1, _2 ) );
	}

template<int free_args, typename descriptor_t, typename return_t, typename CALL_t,	typename a0_t, typename a1_t,
	typename a2_t, typename a3_t,	typename a4_t, typename a5_t,	typename a6_t, typename a7_t,
	typename a8_t, typename a9_t,	typename a10_t,
	int free_args2, typename descriptor2_t, typename return2_t, typename CALL2_t, typename b0_t, typename b1_t,
	typename b2_t, typename b3_t,	typename b4_t, typename b5_t,	typename b6_t, typename b7_t,
	typename b8_t, typename b9_t,	typename b10_t>
typename call_calculator<
	binary_composition<
		less<return_t>,
		HCall<free_args, descriptor_t, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t, a10_t>,
		HCall<free_args2, descriptor2_t, return2_t, CALL2_t, b0_t, b1_t, b2_t, b3_t, b4_t, b5_t, b6_t, b7_t, b8_t, b9_t, b10_t>
	>,
	free_standing_call_args::arg<1>,
	free_standing_call_args::arg<2>
>::type::type
	operator < ( HCall<free_args, descriptor_t, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t, a10_t> const& left_,
			HCall<free_args2, descriptor2_t, return2_t, CALL2_t, b0_t, b1_t, b2_t, b3_t, b4_t, b5_t, b6_t, b7_t, b8_t, b9_t, b10_t> const& right_ )
	{
	return ( call( compose_binary( less<return_t>(), left_, right_ ), _1, _2 ) );
	}

template<int free_args, typename descriptor_t, typename return_t, typename CALL_t,	typename a0_t, typename a1_t,
	typename a2_t, typename a3_t,	typename a4_t, typename a5_t,	typename a6_t, typename a7_t,
	typename a8_t, typename a9_t,	typename a10_t,
	int free_args2, typename descriptor2_t, typename return2_t, typename CALL2_t, typename b0_t, typename b1_t,
	typename b2_t, typename b3_t,	typename b4_t, typename b5_t,	typename b6_t, typename b7_t,
	typename b8_t, typename b9_t,	typename b10_t>
typename call_calculator<
	binary_composition<
		greater<return_t>,
		HCall<free_args, descriptor_t, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t, a10_t>,
		HCall<free_args2, descriptor2_t, return2_t, CALL2_t, b0_t, b1_t, b2_t, b3_t, b4_t, b5_t, b6_t, b7_t, b8_t, b9_t, b10_t>
	>,
	free_standing_call_args::arg<1>,
	free_standing_call_args::arg<2>
>::type::type
	operator > ( HCall<free_args, descriptor_t, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t, a10_t> const& left_,
			HCall<free_args2, descriptor2_t, return2_t, CALL2_t, b0_t, b1_t, b2_t, b3_t, b4_t, b5_t, b6_t, b7_t, b8_t, b9_t, b10_t> const& right_ )
	{
	return ( call( compose_binary( greater<return_t>(), left_, right_ ), _1, _2 ) );
	}

template<int free_args, typename descriptor_t, typename return_t, typename CALL_t,	typename a0_t, typename a1_t,
	typename a2_t, typename a3_t,	typename a4_t, typename a5_t,	typename a6_t, typename a7_t,
	typename a8_t, typename a9_t,	typename a10_t,
	int free_args2, typename descriptor2_t, typename return2_t, typename CALL2_t, typename b0_t, typename b1_t,
	typename b2_t, typename b3_t,	typename b4_t, typename b5_t,	typename b6_t, typename b7_t,
	typename b8_t, typename b9_t,	typename b10_t>
typename call_calculator<
	binary_composition<
		less_equal<return_t>,
		HCall<free_args, descriptor_t, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t, a10_t>,
		HCall<free_args2, descriptor2_t, return2_t, CALL2_t, b0_t, b1_t, b2_t, b3_t, b4_t, b5_t, b6_t, b7_t, b8_t, b9_t, b10_t>
	>,
	free_standing_call_args::arg<1>,
	free_standing_call_args::arg<2>
>::type::type
	operator <= ( HCall<free_args, descriptor_t, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t, a10_t> const& left_,
			HCall<free_args2, descriptor2_t, return2_t, CALL2_t, b0_t, b1_t, b2_t, b3_t, b4_t, b5_t, b6_t, b7_t, b8_t, b9_t, b10_t> const& right_ )
	{
	return ( call( compose_binary( less_equal<return_t>(), left_, right_ ), _1, _2 ) );
	}

template<int free_args, typename descriptor_t, typename return_t, typename CALL_t,	typename a0_t, typename a1_t,
	typename a2_t, typename a3_t,	typename a4_t, typename a5_t,	typename a6_t, typename a7_t,
	typename a8_t, typename a9_t,	typename a10_t,
	int free_args2, typename descriptor2_t, typename return2_t, typename CALL2_t, typename b0_t, typename b1_t,
	typename b2_t, typename b3_t,	typename b4_t, typename b5_t,	typename b6_t, typename b7_t,
	typename b8_t, typename b9_t,	typename b10_t>
typename call_calculator<
	binary_composition<
		greater_equal<return_t>,
		HCall<free_args, descriptor_t, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t, a10_t>,
		HCall<free_args2, descriptor2_t, return2_t, CALL2_t, b0_t, b1_t, b2_t, b3_t, b4_t, b5_t, b6_t, b7_t, b8_t, b9_t, b10_t>
	>,
	free_standing_call_args::arg<1>,
	free_standing_call_args::arg<2>
>::type::type
	operator >= ( HCall<free_args, descriptor_t, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t, a10_t> const& left_,
			HCall<free_args2, descriptor2_t, return2_t, CALL2_t, b0_t, b1_t, b2_t, b3_t, b4_t, b5_t, b6_t, b7_t, b8_t, b9_t, b10_t> const& right_ )
	{
	return ( call( compose_binary( greater_equal<return_t>(), left_, right_ ), _1, _2 ) );
	}

template<int free_args, typename descriptor_t, typename return_t, typename CALL_t,	typename a0_t, typename a1_t,
	typename a2_t, typename a3_t,	typename a4_t, typename a5_t,	typename a6_t, typename a7_t,
	typename a8_t, typename a9_t,	typename a10_t,
	typename constant_t>
typename call_calculator<
	unary_compose<
		HBinder<equal_to<return_t>, constant_t, 1>,
		HCall<free_args, descriptor_t, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t, a10_t>
	>,
	free_standing_call_args::arg<1>
>::type::type
	operator == ( HCall<free_args, descriptor_t, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t, a10_t> const& var_, constant_t const& constant_ )
	{
	return ( call( compose1( bind2nd( equal_to<return_t>(), constant_ ), var_ ), _1 ) );
	}

template<int free_args, typename descriptor_t, typename return_t, typename CALL_t,	typename a0_t, typename a1_t,
	typename a2_t, typename a3_t,	typename a4_t, typename a5_t,	typename a6_t, typename a7_t,
	typename a8_t, typename a9_t,	typename a10_t,
	typename constant_t>
typename call_calculator<
	unary_compose<
		HBinder<not_equal_to<return_t>, constant_t, 1>,
		HCall<free_args, descriptor_t, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t, a10_t>
	>,
	free_standing_call_args::arg<1>
>::type::type
	operator != ( HCall<free_args, descriptor_t, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t, a10_t> const& var_, constant_t const& constant_ )
	{
	return ( call( compose1( bind2nd( not_equal_to<return_t>(), constant_ ), var_ ), _1 ) );
	}

template<int free_args, typename descriptor_t, typename return_t, typename CALL_t,	typename a0_t, typename a1_t,
	typename a2_t, typename a3_t,	typename a4_t, typename a5_t,	typename a6_t, typename a7_t,
	typename a8_t, typename a9_t,	typename a10_t,
	typename constant_t>
typename call_calculator<
	unary_compose<
		HBinder<less<return_t>, constant_t, 1>,
		HCall<free_args, descriptor_t, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t, a10_t>
	>,
	free_standing_call_args::arg<1>
>::type::type
	operator < ( HCall<free_args, descriptor_t, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t, a10_t> const& var_, constant_t const& constant_ )
	{
	return ( call( compose1( bind2nd( less<return_t>(), constant_ ), var_ ), _1 ) );
	}

template<int free_args, typename descriptor_t, typename return_t, typename CALL_t,	typename a0_t, typename a1_t,
	typename a2_t, typename a3_t,	typename a4_t, typename a5_t,	typename a6_t, typename a7_t,
	typename a8_t, typename a9_t,	typename a10_t,
	typename constant_t>
typename call_calculator<
	unary_compose<
		HBinder<greater<return_t>, constant_t, 1>,
		HCall<free_args, descriptor_t, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t, a10_t>
	>,
	free_standing_call_args::arg<1>
>::type::type
	operator > ( HCall<free_args, descriptor_t, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t, a10_t> const& var_, constant_t const& constant_ )
	{
	return ( call( compose1( bind2nd( greater<return_t>(), constant_ ), var_ ), _1 ) );
	}

template<int free_args, typename descriptor_t, typename return_t, typename CALL_t,	typename a0_t, typename a1_t,
	typename a2_t, typename a3_t,	typename a4_t, typename a5_t,	typename a6_t, typename a7_t,
	typename a8_t, typename a9_t,	typename a10_t,
	typename constant_t>
typename call_calculator<
	unary_compose<
		HBinder<less_equal<return_t>, constant_t, 1>,
		HCall<free_args, descriptor_t, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t, a10_t>
	>,
	free_standing_call_args::arg<1>
>::type::type
	operator <= ( HCall<free_args, descriptor_t, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t, a10_t> const& var_, constant_t const& constant_ )
	{
	return ( call( compose1( bind2nd( less_equal<return_t>(), constant_ ), var_ ), _1 ) );
	}

template<int free_args, typename descriptor_t, typename return_t, typename CALL_t,	typename a0_t, typename a1_t,
	typename a2_t, typename a3_t,	typename a4_t, typename a5_t,	typename a6_t, typename a7_t,
	typename a8_t, typename a9_t,	typename a10_t,
	typename constant_t>
typename call_calculator<
	unary_compose<
		HBinder<greater_equal<return_t>, constant_t, 1>,
		HCall<free_args, descriptor_t, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t, a10_t>
	>,
	free_standing_call_args::arg<1>
>::type::type
	operator >= ( HCall<free_args, descriptor_t, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t, a10_t> const& var_, constant_t const& constant_ )
	{
	return ( call( compose1( bind2nd( greater_equal<return_t>(), constant_ ), var_ ), _1 ) );
	}

}

}

#endif /* not YAAL_HCORE_BOUND_HXX_INCLUDED */

