/* Read yaal/LICENSE.md file for copyright and licensing information. */
/*! \file hcore/hfunctor.hxx
 * \brief Declaration and implementation of HFunctor<> template.
 */

#ifndef YAAL_HCORE_HFUNCTOR_HXX_INCLUDED
#define YAAL_HCORE_HFUNCTOR_HXX_INCLUDED 1

#include "hcore/base.hxx"
#include "hcore/algorithm_low.hxx"
#include "hcore/placeholder.hxx"
#include "hcore/hpointer.hxx"
#include "hcore/hresource.hxx"

namespace yaal {

namespace hcore {

/*! \cond */

namespace free_standing_call_args {

template<int, int, typename, typename, typename = trait::no_type, typename = trait::no_type,
	typename = trait::no_type, typename = trait::no_type,
	typename = trait::no_type, typename = trait::no_type,
	typename = trait::no_type, typename = trait::no_type,
	typename = trait::no_type, typename = trait::no_type,
	typename = trait::no_type>
struct getter;

template<typename arg_t, typename fa0_t, typename fa1_t = trait::no_type,
	typename fa2_t = trait::no_type, typename fa3_t = trait::no_type,
	typename fa4_t = trait::no_type, typename fa5_t = trait::no_type,
	typename fa6_t = trait::no_type, typename fa7_t = trait::no_type,
	typename fa8_t = trait::no_type, typename fa9_t = trait::no_type,
	typename fa10_t = trait::no_type>
struct select {
	typedef typename trait::strip<arg_t>::type stripped_arg_t;
	typedef typename trait::ternary<trait::same_type<higher_order::placeholder<1>, stripped_arg_t>::value, fa0_t,
					typename trait::ternary<trait::same_type<higher_order::placeholder<2>, stripped_arg_t>::value, fa1_t,
					typename trait::ternary<trait::same_type<higher_order::placeholder<3>, stripped_arg_t>::value, fa2_t,
					typename trait::ternary<trait::same_type<higher_order::placeholder<4>, stripped_arg_t>::value, fa3_t,
					typename trait::ternary<trait::same_type<higher_order::placeholder<5>, stripped_arg_t>::value, fa4_t,
					typename trait::ternary<trait::same_type<higher_order::placeholder<6>, stripped_arg_t>::value, fa5_t,
					typename trait::ternary<trait::same_type<higher_order::placeholder<7>, stripped_arg_t>::value, fa6_t,
					typename trait::ternary<trait::same_type<higher_order::placeholder<8>, stripped_arg_t>::value, fa7_t,
					typename trait::ternary<trait::same_type<higher_order::placeholder<9>, stripped_arg_t>::value, fa8_t,
					typename trait::ternary<trait::same_type<higher_order::placeholder<10>, stripped_arg_t>::value, fa9_t,
					typename trait::ternary<trait::same_type<higher_order::placeholder<11>, stripped_arg_t>::value, fa10_t,
					arg_t>::type>::type>::type>::type>::type>::type>::type>::type>::type>::type>::type type;

};

template<typename arg_t, typename fa0_t>
typename select<arg_t, fa0_t>::type resolve_arg_get( arg_t&& a, fa0_t&& fa0 ) {
	typedef typename select<arg_t, fa0_t>::type return_t;
	return (
		getter<
			1,
			trait::find_type<
				typename trait::strip<arg_t>::type,
				higher_order::placeholder<1>
			>::value,
			arg_t,
			fa0_t
		>::template get<return_t>(
			yaal::forward<arg_t>( a ),
			yaal::forward<fa0_t>( fa0 )
		)
	);
}

template<typename arg_t, typename fa0_t, typename fa1_t>
typename select<arg_t, fa0_t, fa1_t>::type resolve_arg_get( arg_t&& a, fa0_t&& fa0, fa1_t&& fa1 ) {
	typedef typename select<arg_t, fa0_t, fa1_t>::type return_t;
	return (
		getter<
			2,
			trait::find_type<
				typename trait::strip<arg_t>::type,
				higher_order::placeholder<1>,
				higher_order::placeholder<2>
			>::value,
			arg_t, fa0_t, fa1_t
		>::template get<return_t>(
			yaal::forward<arg_t>( a ),
			yaal::forward<fa0_t>( fa0 ),
			yaal::forward<fa1_t>( fa1 )
		)
	);
}

template<typename arg_t, typename fa0_t, typename fa1_t, typename fa2_t>
typename select<arg_t, fa0_t, fa1_t, fa2_t>::type resolve_arg_get( arg_t&& a, fa0_t&& fa0, fa1_t&& fa1, fa2_t&& fa2 ) {
	typedef typename select<arg_t, fa0_t, fa1_t, fa2_t>::type return_t;
	return (
		getter<
			3,
			trait::find_type<
				typename trait::strip<arg_t>::type,
				higher_order::placeholder<1>,
				higher_order::placeholder<2>,
				higher_order::placeholder<3>
			>::value,
			arg_t, fa0_t, fa1_t, fa2_t
		>::template get<return_t>(
			yaal::forward<arg_t>( a ),
			yaal::forward<fa0_t>( fa0 ),
			yaal::forward<fa1_t>( fa1 ),
			yaal::forward<fa2_t>( fa2 )
		)
	);
}

template<typename arg_t, typename fa0_t, typename fa1_t, typename fa2_t, typename fa3_t>
typename select<arg_t, fa0_t, fa1_t, fa2_t, fa3_t>::type resolve_arg_get( arg_t&& a, fa0_t&& fa0, fa1_t&& fa1, fa2_t&& fa2, fa3_t&& fa3 ) {
	typedef typename select<arg_t, fa0_t, fa1_t, fa2_t, fa3_t>::type return_t;
	return (
		getter<
			4,
			trait::find_type<
				typename trait::strip<arg_t>::type,
				higher_order::placeholder<1>,
				higher_order::placeholder<2>,
				higher_order::placeholder<3>,
				higher_order::placeholder<4>
			>::value,
			arg_t, fa0_t, fa1_t, fa2_t, fa3_t
		>::template get<return_t>(
			yaal::forward<arg_t>( a ),
			yaal::forward<fa0_t>( fa0 ),
			yaal::forward<fa1_t>( fa1 ),
			yaal::forward<fa2_t>( fa2 ),
			yaal::forward<fa3_t>( fa3 )
		)
	);
}

template<
	typename arg_t, typename fa0_t, typename fa1_t, typename fa2_t, typename fa3_t,
	typename fa4_t
>
typename select<
	arg_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t
>::type resolve_arg_get( arg_t&& a, fa0_t&& fa0, fa1_t&& fa1, fa2_t&& fa2, fa3_t&& fa3, fa4_t&& fa4 ) {
	typedef typename select<arg_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t>::type return_t;
	return (
		getter<
			5,
			trait::find_type<
				typename trait::strip<arg_t>::type,
				higher_order::placeholder<1>,
				higher_order::placeholder<2>,
				higher_order::placeholder<3>,
				higher_order::placeholder<4>,
				higher_order::placeholder<5>
			>::value,
			arg_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t
		>::template get<return_t>(
			yaal::forward<arg_t>( a ),
			yaal::forward<fa0_t>( fa0 ),
			yaal::forward<fa1_t>( fa1 ),
			yaal::forward<fa2_t>( fa2 ),
			yaal::forward<fa3_t>( fa3 ),
			yaal::forward<fa4_t>( fa4 )
		)
	);
}

template<
	typename arg_t, typename fa0_t, typename fa1_t, typename fa2_t, typename fa3_t,
	typename fa4_t, typename fa5_t
>
typename select<
	arg_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t
>::type resolve_arg_get( arg_t&& a, fa0_t&& fa0, fa1_t&& fa1, fa2_t&& fa2, fa3_t&& fa3, fa4_t&& fa4, fa5_t&& fa5 ) {
	typedef typename select<arg_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t>::type return_t;
	return (
		getter<
			6,
			trait::find_type<
				typename trait::strip<arg_t>::type,
				higher_order::placeholder<1>,
				higher_order::placeholder<2>,
				higher_order::placeholder<3>,
				higher_order::placeholder<4>,
				higher_order::placeholder<5>,
				higher_order::placeholder<6>
			>::value,
			arg_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t
		>::template get<return_t>(
			yaal::forward<arg_t>( a ),
			yaal::forward<fa0_t>( fa0 ),
			yaal::forward<fa1_t>( fa1 ),
			yaal::forward<fa2_t>( fa2 ),
			yaal::forward<fa3_t>( fa3 ),
			yaal::forward<fa4_t>( fa4 ),
			yaal::forward<fa5_t>( fa5 )
		)
	);
}

template<
	typename arg_t, typename fa0_t, typename fa1_t, typename fa2_t, typename fa3_t,
	typename fa4_t, typename fa5_t, typename fa6_t
>
typename select<
	arg_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t
>::type resolve_arg_get( arg_t&& a, fa0_t&& fa0, fa1_t&& fa1, fa2_t&& fa2, fa3_t&& fa3, fa4_t&& fa4, fa5_t&& fa5, fa6_t&& fa6 ) {
	typedef typename select<arg_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t>::type return_t;
	return (
		getter<
			7,
			trait::find_type<
				typename trait::strip<arg_t>::type,
				higher_order::placeholder<1>,
				higher_order::placeholder<2>,
				higher_order::placeholder<3>,
				higher_order::placeholder<4>,
				higher_order::placeholder<5>,
				higher_order::placeholder<6>,
				higher_order::placeholder<7>
			>::value,
			arg_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t
		>::template get<return_t>(
			yaal::forward<arg_t>( a ),
			yaal::forward<fa0_t>( fa0 ),
			yaal::forward<fa1_t>( fa1 ),
			yaal::forward<fa2_t>( fa2 ),
			yaal::forward<fa3_t>( fa3 ),
			yaal::forward<fa4_t>( fa4 ),
			yaal::forward<fa5_t>( fa5 ),
			yaal::forward<fa6_t>( fa6 )
		)
	);
}

template<
	typename arg_t, typename fa0_t, typename fa1_t, typename fa2_t, typename fa3_t,
	typename fa4_t, typename fa5_t, typename fa6_t, typename fa7_t
>
typename select<
	arg_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t
>::type resolve_arg_get( arg_t&& a, fa0_t&& fa0, fa1_t&& fa1, fa2_t&& fa2, fa3_t&& fa3, fa4_t&& fa4, fa5_t&& fa5, fa6_t&& fa6, fa7_t&& fa7 ) {
	typedef typename select<arg_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t>::type return_t;
	return (
		getter<
			8,
			trait::find_type<
				typename trait::strip<arg_t>::type,
				higher_order::placeholder<1>,
				higher_order::placeholder<2>,
				higher_order::placeholder<3>,
				higher_order::placeholder<4>,
				higher_order::placeholder<5>,
				higher_order::placeholder<6>,
				higher_order::placeholder<7>,
				higher_order::placeholder<8>
			>::value,
			arg_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t
		>::template get<return_t>(
			yaal::forward<arg_t>( a ),
			yaal::forward<fa0_t>( fa0 ),
			yaal::forward<fa1_t>( fa1 ),
			yaal::forward<fa2_t>( fa2 ),
			yaal::forward<fa3_t>( fa3 ),
			yaal::forward<fa4_t>( fa4 ),
			yaal::forward<fa5_t>( fa5 ),
			yaal::forward<fa6_t>( fa6 ),
			yaal::forward<fa7_t>( fa7 )
		)
	);
}

template<
	typename arg_t, typename fa0_t, typename fa1_t, typename fa2_t, typename fa3_t,
	typename fa4_t, typename fa5_t, typename fa6_t, typename fa7_t, typename fa8_t
>
typename select<
	arg_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t
>::type resolve_arg_get( arg_t&& a, fa0_t&& fa0, fa1_t&& fa1, fa2_t&& fa2, fa3_t&& fa3, fa4_t&& fa4, fa5_t&& fa5, fa6_t&& fa6, fa7_t&& fa7, fa8_t&& fa8 ) {
	typedef typename select<arg_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t>::type return_t;
	return (
		getter<
			9,
			trait::find_type<
				typename trait::strip<arg_t>::type,
				higher_order::placeholder<1>,
				higher_order::placeholder<2>,
				higher_order::placeholder<3>,
				higher_order::placeholder<4>,
				higher_order::placeholder<5>,
				higher_order::placeholder<6>,
				higher_order::placeholder<7>,
				higher_order::placeholder<8>,
				higher_order::placeholder<9>
			>::value,
			arg_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t
		>::template get<return_t>(
			yaal::forward<arg_t>( a ),
			yaal::forward<fa0_t>( fa0 ),
			yaal::forward<fa1_t>( fa1 ),
			yaal::forward<fa2_t>( fa2 ),
			yaal::forward<fa3_t>( fa3 ),
			yaal::forward<fa4_t>( fa4 ),
			yaal::forward<fa5_t>( fa5 ),
			yaal::forward<fa6_t>( fa6 ),
			yaal::forward<fa7_t>( fa7 ),
			yaal::forward<fa8_t>( fa8 )
		)
	);
}

template<
	typename arg_t, typename fa0_t, typename fa1_t, typename fa2_t, typename fa3_t,
	typename fa4_t, typename fa5_t, typename fa6_t, typename fa7_t, typename fa8_t,
	typename fa9_t
>
typename select<
	arg_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t, fa9_t
>::type resolve_arg_get( arg_t&& a, fa0_t&& fa0, fa1_t&& fa1, fa2_t&& fa2, fa3_t&& fa3, fa4_t&& fa4, fa5_t&& fa5, fa6_t&& fa6, fa7_t&& fa7, fa8_t&& fa8, fa9_t&& fa9 ) {
	typedef typename select<arg_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t, fa9_t>::type return_t;
	return (
		getter<
			10,
			trait::find_type<
				typename trait::strip<arg_t>::type,
				higher_order::placeholder<1>,
				higher_order::placeholder<2>,
				higher_order::placeholder<3>,
				higher_order::placeholder<4>,
				higher_order::placeholder<5>,
				higher_order::placeholder<6>,
				higher_order::placeholder<7>,
				higher_order::placeholder<8>,
				higher_order::placeholder<9>,
				higher_order::placeholder<10>
			>::value,
			arg_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t, fa9_t
		>::template get<return_t>(
			yaal::forward<arg_t>( a ),
			yaal::forward<fa0_t>( fa0 ),
			yaal::forward<fa1_t>( fa1 ),
			yaal::forward<fa2_t>( fa2 ),
			yaal::forward<fa3_t>( fa3 ),
			yaal::forward<fa4_t>( fa4 ),
			yaal::forward<fa5_t>( fa5 ),
			yaal::forward<fa6_t>( fa6 ),
			yaal::forward<fa7_t>( fa7 ),
			yaal::forward<fa8_t>( fa8 ),
			yaal::forward<fa9_t>( fa9 )
		)
	);
}

template<
	typename arg_t, typename fa0_t, typename fa1_t, typename fa2_t, typename fa3_t,
	typename fa4_t, typename fa5_t, typename fa6_t, typename fa7_t, typename fa8_t,
	typename fa9_t, typename fa10_t
>
typename select<
	arg_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t, fa9_t, fa10_t
>::type resolve_arg_get( arg_t&& a, fa0_t&& fa0, fa1_t&& fa1, fa2_t&& fa2, fa3_t&& fa3, fa4_t&& fa4, fa5_t&& fa5, fa6_t&& fa6, fa7_t&& fa7, fa8_t&& fa8, fa9_t&& fa9, fa10_t&& fa10 ) {
	typedef typename select<arg_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t, fa9_t, fa10_t>::type return_t;
	return (
		getter<
			11,
			trait::find_type<
				typename trait::strip<arg_t>::type,
				higher_order::placeholder<1>,
				higher_order::placeholder<2>,
				higher_order::placeholder<3>,
				higher_order::placeholder<4>,
				higher_order::placeholder<5>,
				higher_order::placeholder<6>,
				higher_order::placeholder<7>,
				higher_order::placeholder<8>,
				higher_order::placeholder<9>,
				higher_order::placeholder<10>,
				higher_order::placeholder<11>
			>::value,
			arg_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t, fa9_t, fa10_t
		>::template get<return_t>(
			yaal::forward<arg_t>( a ),
			yaal::forward<fa0_t>( fa0 ),
			yaal::forward<fa1_t>( fa1 ),
			yaal::forward<fa2_t>( fa2 ),
			yaal::forward<fa3_t>( fa3 ),
			yaal::forward<fa4_t>( fa4 ),
			yaal::forward<fa5_t>( fa5 ),
			yaal::forward<fa6_t>( fa6 ),
			yaal::forward<fa7_t>( fa7 ),
			yaal::forward<fa8_t>( fa8 ),
			yaal::forward<fa9_t>( fa9 ),
			yaal::forward<fa10_t>( fa10 )
		)
	);
}

template<int N, typename arg_t, typename fa0_t, typename fa1_t, typename fa2_t, typename fa3_t,
	typename fa4_t, typename fa5_t, typename fa6_t, typename fa7_t, typename fa8_t, typename fa9_t, typename fa10_t>
struct getter<N, 0, arg_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t, fa9_t, fa10_t> {
	template<typename return_t>
	inline static return_t get( arg_t, fa0_t fa0, fa1_t = fa1_t(), fa2_t = fa2_t(), fa3_t = fa3_t(), fa4_t = fa4_t(), fa5_t = fa5_t(), fa6_t = fa6_t(), fa7_t = fa7_t(), fa8_t = fa8_t(), fa9_t = fa9_t(), fa10_t = fa10_t() )
		{ return fa0; }
};

template<int N, typename arg_t, typename fa0_t, typename fa1_t, typename fa2_t, typename fa3_t,
	typename fa4_t, typename fa5_t, typename fa6_t, typename fa7_t, typename fa8_t, typename fa9_t, typename fa10_t>
struct getter<N, 1, arg_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t, fa9_t, fa10_t> {
	template<typename return_t>
	inline static return_t get( arg_t, fa0_t, fa1_t fa1, fa2_t = fa2_t(), fa3_t = fa3_t(), fa4_t = fa4_t(), fa5_t = fa5_t(), fa6_t = fa6_t(), fa7_t = fa7_t(), fa8_t = fa8_t(), fa9_t = fa9_t(), fa10_t = fa10_t() )
		{ return fa1; }
};

template<int N, typename arg_t, typename fa0_t, typename fa1_t, typename fa2_t, typename fa3_t,
	typename fa4_t, typename fa5_t, typename fa6_t, typename fa7_t, typename fa8_t, typename fa9_t, typename fa10_t>
struct getter<N, 2, arg_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t, fa9_t, fa10_t> {
	template<typename return_t>
	inline static return_t get( arg_t, fa0_t, fa1_t, fa2_t fa2, fa3_t = fa3_t(), fa4_t = fa4_t(), fa5_t = fa5_t(), fa6_t = fa6_t(), fa7_t = fa7_t(), fa8_t = fa8_t(), fa9_t = fa9_t(), fa10_t = fa10_t() )
		{ return fa2; }
};

template<int N, typename arg_t, typename fa0_t, typename fa1_t, typename fa2_t, typename fa3_t,
	typename fa4_t, typename fa5_t, typename fa6_t, typename fa7_t, typename fa8_t, typename fa9_t, typename fa10_t>
struct getter<N, 3, arg_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t, fa9_t, fa10_t> {
	template<typename return_t>
	inline static return_t get( arg_t, fa0_t, fa1_t, fa2_t, fa3_t fa3, fa4_t = fa4_t(), fa5_t = fa5_t(), fa6_t = fa6_t(), fa7_t = fa7_t(), fa8_t = fa8_t(), fa9_t = fa9_t(), fa10_t = fa10_t() )
		{ return fa3; }
};

template<int N, typename arg_t, typename fa0_t, typename fa1_t, typename fa2_t, typename fa3_t,
	typename fa4_t, typename fa5_t, typename fa6_t, typename fa7_t, typename fa8_t, typename fa9_t, typename fa10_t>
struct getter<N, 4, arg_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t, fa9_t, fa10_t> {
	template<typename return_t>
	inline static return_t get( arg_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t fa4, fa5_t = fa5_t(), fa6_t = fa6_t(), fa7_t = fa7_t(), fa8_t = fa8_t(), fa9_t = fa9_t(), fa10_t = fa10_t() )
		{ return fa4; }
};

template<int N, typename arg_t, typename fa0_t, typename fa1_t, typename fa2_t, typename fa3_t,
	typename fa4_t, typename fa5_t, typename fa6_t, typename fa7_t, typename fa8_t, typename fa9_t, typename fa10_t>
struct getter<N, 5, arg_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t, fa9_t, fa10_t> {
	template<typename return_t>
	inline static return_t get( arg_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t fa5, fa6_t = fa6_t(), fa7_t = fa7_t(), fa8_t = fa8_t(), fa9_t = fa9_t(), fa10_t = fa10_t() )
		{ return fa5; }
};

template<int N, typename arg_t, typename fa0_t, typename fa1_t, typename fa2_t, typename fa3_t,
	typename fa4_t, typename fa5_t, typename fa6_t, typename fa7_t, typename fa8_t, typename fa9_t, typename fa10_t>
struct getter<N, 6, arg_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t, fa9_t, fa10_t> {
	template<typename return_t>
	inline static return_t get( arg_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t fa6, fa7_t = fa7_t(), fa8_t = fa8_t(), fa9_t = fa9_t(), fa10_t = fa10_t() )
		{ return fa6; }
};

template<int N, typename arg_t, typename fa0_t, typename fa1_t, typename fa2_t, typename fa3_t,
	typename fa4_t, typename fa5_t, typename fa6_t, typename fa7_t, typename fa8_t, typename fa9_t, typename fa10_t>
struct getter<N, 7, arg_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t, fa9_t, fa10_t> {
	template<typename return_t>
	inline static return_t get( arg_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t fa7, fa8_t = fa8_t(), fa9_t = fa9_t(), fa10_t = fa10_t() )
		{ return fa7; }
};

template<int N, typename arg_t, typename fa0_t, typename fa1_t, typename fa2_t, typename fa3_t,
	typename fa4_t, typename fa5_t, typename fa6_t, typename fa7_t, typename fa8_t, typename fa9_t, typename fa10_t>
struct getter<N, 8, arg_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t, fa9_t, fa10_t> {
	template<typename return_t>
	inline static return_t get( arg_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t fa8, fa9_t = fa9_t(), fa10_t = fa10_t() )
		{ return fa8; }
};

template<int N, typename arg_t, typename fa0_t, typename fa1_t, typename fa2_t, typename fa3_t,
	typename fa4_t, typename fa5_t, typename fa6_t, typename fa7_t, typename fa8_t, typename fa9_t, typename fa10_t>
struct getter<N, 9, arg_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t, fa9_t, fa10_t> {
	template<typename return_t>
	inline static return_t get( arg_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t, fa9_t fa9, fa10_t = fa10_t() )
		{ return fa9; }
};

template<int N, typename arg_t, typename fa0_t, typename fa1_t, typename fa2_t, typename fa3_t,
	typename fa4_t, typename fa5_t, typename fa6_t, typename fa7_t, typename fa8_t, typename fa9_t, typename fa10_t>
struct getter<N, 10, arg_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t, fa9_t, fa10_t> {
	template<typename return_t>
	inline static return_t get( arg_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t, fa9_t, fa10_t fa10 )
		{ return fa10; }
};

template<int N, typename arg_t, typename fa0_t, typename fa1_t, typename fa2_t, typename fa3_t,
	typename fa4_t, typename fa5_t, typename fa6_t, typename fa7_t, typename fa8_t, typename fa9_t, typename fa10_t>
struct getter<N, -1, arg_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t, fa9_t, fa10_t> {
	template<typename return_t>
	inline static return_t get( arg_t a, fa0_t, fa1_t = fa1_t(), fa2_t = fa2_t(), fa3_t = fa3_t(), fa4_t = fa4_t(), fa5_t = fa5_t(), fa6_t = fa6_t(), fa7_t = fa7_t(), fa8_t = fa8_t(), fa9_t = fa9_t() )
		{ return a; }
};

}

struct object_resolver {
	template<typename tType>
	struct is_smart_pointer {
		template<typename T>
		static constexpr bool sfinae( typename yaal::hcore::HPointer<T> const* ) {
			return ( true );
		}
		template<typename T, typename D>
		static constexpr bool sfinae( typename yaal::hcore::HResource<T, D> const* ) {
			return ( true );
		}
		static constexpr bool sfinae( ... ) {
			return ( false );
		}
		static bool const value = sfinae( static_cast<typename trait::strip_reference<tType>::type const*>( nullptr ) );
	};
	typedef enum { REF, PTR, SMART, FREE_REF, FREE_PTR } object_type_t;
	template<typename CLASS_t, typename arg_t = int>
	struct object_type {
		static object_type_t const value = static_cast<object_type_t>(
				meta::ternary<
						meta::greater<
								trait::find_type<
										typename trait::strip<CLASS_t>::type,
										higher_order::placeholder<1>,
										higher_order::placeholder<2>,
										higher_order::placeholder<3>,
										higher_order::placeholder<4>,
										higher_order::placeholder<5>,
										higher_order::placeholder<6>,
										higher_order::placeholder<7>,
										higher_order::placeholder<8>,
										higher_order::placeholder<9>,
										higher_order::placeholder<10>
								>::value,
								-1
						>::value,
						meta::ternary<trait::is_pointer<typename trait::strip<arg_t>::type>::value, FREE_PTR, FREE_REF>::value,
						meta::ternary<
							trait::is_pointer<typename trait::strip<CLASS_t>::type>::value,
							PTR,
							meta::ternary<is_smart_pointer<typename trait::strip<CLASS_t>::type>::value, SMART, REF>::value
						>::value
				>::value
		);
	};
	template<typename return_t, object_type_t>
	struct invoke;

	template<typename return_t>
	struct invoke<return_t, SMART> {
		template<typename METHOD_t, typename CLASS_t, typename... arg_t>
		static return_t go( METHOD_t method_, CLASS_t&& object_, arg_t&&... arg_ ) {
			return ( (*object_.*method_)( yaal::forward<arg_t>( arg_ )... ) );
		}
		template<typename CLASS_t, typename dummy_t>
		static CLASS_t const* id( CLASS_t const& object_, dummy_t ) {
			return ( reinterpret_cast<CLASS_t const*>( object_.raw() ) );
		}
	};
	template<typename return_t>
	struct invoke<return_t, REF> {
		template<typename METHOD_t, typename CLASS_t, typename... arg_t>
		static return_t go( METHOD_t method_, CLASS_t object_, arg_t&&... arg_ ) {
			return ( (object_.*method_)( yaal::forward<arg_t>( arg_ )... ) );
		}
		template<typename CLASS_t, typename dummy_t>
		static CLASS_t* id( CLASS_t object_, dummy_t ) {
			return ( &object_ );
		}
	};
	template<typename return_t>
	struct invoke<return_t, PTR> {
		template<typename METHOD_t, typename CLASS_t, typename... arg_t>
		static return_t go( METHOD_t method_, CLASS_t&& object_, arg_t&&... arg_ ) {
			return ( (object_->*method_)( yaal::forward<arg_t>( arg_ )... ) );
		}
		template<typename CLASS_t, typename dummy_t>
		static CLASS_t id( CLASS_t object_, dummy_t ) {
			return object_;
		}
	};
	template<typename return_t>
	struct invoke<return_t, FREE_REF> {
		template<typename METHOD_t, typename dummy_t, typename CLASS_t, typename... arg_t>
		static return_t go( METHOD_t method_, dummy_t, CLASS_t& object_, arg_t&&... arg_ ) {
			return ( (object_.*method_)( yaal::forward<arg_t>( arg_ )... ) );
		}
		template<typename dummy_t, typename CLASS_t>
		static CLASS_t* id( dummy_t, CLASS_t& object_ ) {
			return ( &object_ );
		}
	};
	template<typename return_t>
	struct invoke<return_t, FREE_PTR> {
		template<typename METHOD_t, typename dummy_t, typename CLASS_t, typename... arg_t>
		static return_t go( METHOD_t method_, dummy_t, CLASS_t object_, arg_t&&... arg_ ) {
			return ( (object_->*method_)( yaal::forward<arg_t>( arg_ )... ) );
		}
		template<typename dummy_t, typename CLASS_t>
		static CLASS_t id( dummy_t, CLASS_t object_ ) {
			return object_;
		}
	};
};

/*! \endcond */

/*! \brief Make a functor from any (class, method) pair.
 */
template<typename CLASS_t, typename METHOD_t>
class HFunctor {
	typename trait::decay<CLASS_t>::type _object;
	typename trait::decay<METHOD_t>::type _method;
	typedef typename trait::return_type<METHOD_t>::type return_t;
public:
	HFunctor( CLASS_t&& object_, METHOD_t&& method_ )
		: _object( yaal::forward<CLASS_t>( object_ ) ), _method( yaal::forward<METHOD_t>( method_ ) ) {
		return;
	}
	HFunctor( HFunctor const& ) = default;
	HFunctor( HFunctor&& ) = default;

	template<typename arg0_t, typename... arg_t>
	return_t operator()( arg0_t&& arg0_, arg_t&&... arg_ ) {
		return ( object_resolver::invoke<return_t, object_resolver::object_type<CLASS_t, arg0_t>::value>::go(
					_method, _object, yaal::forward<arg0_t>( arg0_ ), yaal::forward<arg_t>( arg_ )... ) );
	}

	template<typename arg0_t, typename... arg_t>
	return_t operator()( arg0_t&& arg0_, arg_t&&... arg_ ) const {
		return ( object_resolver::invoke<return_t, object_resolver::object_type<CLASS_t, arg0_t>::value>::go(
					_method, _object, yaal::forward<arg0_t>( arg0_ ), yaal::forward<arg_t>( arg_ )... ) );
	}

	return_t operator()( void ) {
		return ( object_resolver::invoke<return_t, object_resolver::object_type<CLASS_t>::value>::go( _method, _object ) );
	}
	return_t operator()( void ) const {
		return ( object_resolver::invoke<return_t, object_resolver::object_type<CLASS_t>::value>::go( _method, _object ) );
	}

	void const* id( void ) const {
		return ( bit_cast<void const*>( object_resolver::invoke<return_t, object_resolver::object_type<CLASS_t>::value>::id( _object, _method ) ) );
	}
};

}

}

#endif /* #ifndef YAAL_HCORE_HFUNCTOR_HXX_INCLUDED */

