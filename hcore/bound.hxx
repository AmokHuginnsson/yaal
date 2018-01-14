/* Read yaal/LICENSE.md file for copyright and licensing information. */

/*! \file hcore/bound.hxx
 * \brief HCall helpers for algorithms.
 */

#ifndef YAAL_HCORE_BOUND_HXX_INCLUDED
#define YAAL_HCORE_BOUND_HXX_INCLUDED 1

#include "hcore/hcall.hxx"

namespace yaal {

namespace hcore {

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
	higher_order::placeholder<1> const&,
	higher_order::placeholder<2> const&
>::type::type
	operator == ( HCall<free_args, descriptor_t, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t, a10_t> const& left_,
			HCall<free_args2, descriptor2_t, return2_t, CALL2_t, b0_t, b1_t, b2_t, b3_t, b4_t, b5_t, b6_t, b7_t, b8_t, b9_t, b10_t> const& right_ ) {
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
	higher_order::placeholder<1> const&,
	higher_order::placeholder<2> const&
>::type::type
	operator != ( HCall<free_args, descriptor_t, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t, a10_t> const& left_,
			HCall<free_args2, descriptor2_t, return2_t, CALL2_t, b0_t, b1_t, b2_t, b3_t, b4_t, b5_t, b6_t, b7_t, b8_t, b9_t, b10_t> const& right_ ) {
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
	higher_order::placeholder<1> const&,
	higher_order::placeholder<2> const&
>::type::type
	operator < ( HCall<free_args, descriptor_t, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t, a10_t> const& left_,
			HCall<free_args2, descriptor2_t, return2_t, CALL2_t, b0_t, b1_t, b2_t, b3_t, b4_t, b5_t, b6_t, b7_t, b8_t, b9_t, b10_t> const& right_ ) {
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
	higher_order::placeholder<1> const&,
	higher_order::placeholder<2> const&
>::type::type
	operator > ( HCall<free_args, descriptor_t, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t, a10_t> const& left_,
			HCall<free_args2, descriptor2_t, return2_t, CALL2_t, b0_t, b1_t, b2_t, b3_t, b4_t, b5_t, b6_t, b7_t, b8_t, b9_t, b10_t> const& right_ ) {
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
	higher_order::placeholder<1> const&,
	higher_order::placeholder<2> const&
>::type::type
	operator <= ( HCall<free_args, descriptor_t, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t, a10_t> const& left_,
			HCall<free_args2, descriptor2_t, return2_t, CALL2_t, b0_t, b1_t, b2_t, b3_t, b4_t, b5_t, b6_t, b7_t, b8_t, b9_t, b10_t> const& right_ ) {
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
	higher_order::placeholder<1> const&,
	higher_order::placeholder<2> const&
>::type::type
	operator >= ( HCall<free_args, descriptor_t, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t, a10_t> const& left_,
			HCall<free_args2, descriptor2_t, return2_t, CALL2_t, b0_t, b1_t, b2_t, b3_t, b4_t, b5_t, b6_t, b7_t, b8_t, b9_t, b10_t> const& right_ ) {
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
	higher_order::placeholder<1> const&
>::type::type
	operator == ( HCall<free_args, descriptor_t, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t, a10_t> const& var_, constant_t const& constant_ ) {
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
	higher_order::placeholder<1> const&
>::type::type
	operator != ( HCall<free_args, descriptor_t, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t, a10_t> const& var_, constant_t const& constant_ ) {
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
	higher_order::placeholder<1> const&
>::type::type
	operator < ( HCall<free_args, descriptor_t, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t, a10_t> const& var_, constant_t const& constant_ ) {
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
	higher_order::placeholder<1> const&
>::type::type
	operator > ( HCall<free_args, descriptor_t, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t, a10_t> const& var_, constant_t const& constant_ ) {
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
	higher_order::placeholder<1> const&
>::type::type
	operator <= ( HCall<free_args, descriptor_t, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t, a10_t> const& var_, constant_t const& constant_ ) {
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
	higher_order::placeholder<1> const&
>::type::type
	operator >= ( HCall<free_args, descriptor_t, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t, a10_t> const& var_, constant_t const& constant_ ) {
	return ( call( compose1( bind2nd( greater_equal<return_t>(), constant_ ), var_ ), _1 ) );
}

}

}

#endif /* #ifndef YAAL_HCORE_BOUND_HXX_INCLUDED */

