/* Read yaal/LICENSE.md file for copyright and licensing information. */
/*! \file hcore/hcall.hxx
 * \brief Declaration and implementation of HCall<> template.
 */

#ifndef YAAL_HCORE_HCALL_HXX_INCLUDED
#define YAAL_HCORE_HCALL_HXX_INCLUDED 1

#include "hcore/hfunctor.hxx"
#include "hcore/functional.hxx"

namespace yaal {

namespace hcore {

/*! \brief Implementation of abstraction of any-method of any-class invocation.
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

/*! \brief Interface to abstraction of any-method of any-class invocation.
 */
template<typename return_t>
class HCallInterface<0, return_t> {
public:
	virtual ~HCallInterface( void ) {}
	return_t operator()( void ) {
		return ( do_invoke() );
	}
	return_t operator()( void ) const {
		return ( do_invoke() );
	}
	void const* id( void ) const {
		return ( do_id() );
	}
protected:
	virtual return_t do_invoke( void ) = 0;
	virtual return_t do_invoke( void ) const = 0;
	virtual void const* do_id( void ) const = 0;
};

template<typename return_t, typename a0_t>
class HCallInterface<1, return_t, a0_t> : public unary_function<return_t, a0_t> {
public:
	typedef unary_function<return_t, a0_t> base_type;
	typedef typename base_type::result_type result_type;
	typedef typename base_type::argument_type argument_type;
	virtual ~HCallInterface( void ) {}
	return_t operator()( a0_t a0 ) {
		return ( do_invoke( a0 ) );
	}
	return_t operator()( a0_t a0 ) const {
		return ( do_invoke( a0 ) );
	}
	return_t operator()( typename trait::strip_reference<a0_t>::type* a0 ) {
		return ( do_invoke( *a0 ) );
	}
	return_t operator()( typename trait::strip_reference<a0_t>::type const* a0 ) const {
		return ( do_invoke( *a0 ) );
	}
	return_t operator()( HPointer<typename trait::strip<a0_t>::type> a0 ) {
		return ( do_invoke( *a0 ) );
	}
	return_t operator()( HPointer<typename trait::strip<a0_t>::type> a0 ) const {
		return ( do_invoke( *a0 ) );
	}
	return_t operator()( HResource<typename trait::strip<a0_t>::type> a0 ) {
		return ( do_invoke( *a0 ) );
	}
	return_t operator()( HResource<typename trait::strip<a0_t>::type> a0 ) const {
		return ( do_invoke( *a0 ) );
	}
	void const* id( void ) const {
		return ( do_id() );
	}
protected:
	virtual return_t do_invoke( a0_t ) = 0;
	virtual return_t do_invoke( a0_t ) const = 0;
	virtual void const* do_id( void ) const = 0;
};

template<typename return_t, typename a0_t, typename a1_t>
class HCallInterface<2, return_t, a0_t, a1_t>
		: public binary_function<return_t, a0_t, a1_t> {
public:
	typedef binary_function<return_t, a0_t, a1_t> base_type;
	typedef typename base_type::result_type result_type;
	typedef typename base_type::first_argument_type first_argument_type;
	typedef typename base_type::second_argument_type second_argument_type;
	virtual ~HCallInterface( void ) {}
	return_t operator()( a0_t a0, a1_t a1 ) {
		return ( do_invoke( a0, a1 ) );
	}
	return_t operator()( a0_t a0, a1_t a1 ) const {
		return ( do_invoke( a0, a1 ) );
	}
	return_t operator()( typename trait::strip_reference<a0_t>::type* a0, a1_t a1 ) {
		return ( do_invoke( *a0, a1 ) );
	}
	return_t operator()( typename trait::strip_reference<a0_t>::type const* a0, a1_t a1 ) const {
		return ( do_invoke( *a0, a1 ) );
	}
	return_t operator()( HPointer<typename trait::strip<a0_t>::type> a0, a1_t a1 ) {
		return ( do_invoke( *a0, a1 ) );
	}
	return_t operator()( HPointer<typename trait::strip<a0_t>::type> a0, a1_t a1 ) const {
		return ( do_invoke( *a0, a1 ) );
	}
	return_t operator()( HResource<typename trait::strip<a0_t>::type> a0, a1_t a1 ) {
		return ( do_invoke( *a0, a1 ) );
	}
	return_t operator()( HResource<typename trait::strip<a0_t>::type> a0, a1_t a1 ) const {
		return ( do_invoke( *a0, a1 ) );
	}
	void const* id( void ) const {
		return ( do_id() );
	}
protected:
	virtual return_t do_invoke( a0_t, a1_t ) = 0;
	virtual return_t do_invoke( a0_t, a1_t ) const = 0;
	virtual void const* do_id( void ) const = 0;
};

template<typename return_t, typename a0_t, typename a1_t, typename a2_t>
class HCallInterface<3, return_t, a0_t, a1_t, a2_t> {
public:
	typedef return_t result_type;
	typedef a0_t first_argument_type;
	typedef a1_t second_argument_type;
	typedef a2_t third_argument_type;
	virtual ~HCallInterface( void ) {}
	return_t operator()( a0_t a0, a1_t a1, a2_t a2 ) {
		return ( do_invoke( a0, a1, a2 ) );
	}
	return_t operator()( a0_t a0, a1_t a1, a2_t a2 ) const {
		return ( do_invoke( a0, a1, a2 ) );
	}
	return_t operator()( typename trait::strip_reference<a0_t>::type* a0, a1_t a1, a2_t a2 ) {
		return ( do_invoke( *a0, a1, a2 ) );
	}
	return_t operator()( typename trait::strip_reference<a0_t>::type const* a0, a1_t a1, a2_t a2 ) const {
		return ( do_invoke( *a0, a1, a2 ) );
	}
	return_t operator()( HPointer<typename trait::strip<a0_t>::type> a0, a1_t a1, a2_t a2 ) {
		return ( do_invoke( *a0, a1, a2 ) );
	}
	return_t operator()( HPointer<typename trait::strip<a0_t>::type> a0, a1_t a1, a2_t a2 ) const {
		return ( do_invoke( *a0, a1, a2 ) );
	}
	return_t operator()( HResource<typename trait::strip<a0_t>::type> a0, a1_t a1, a2_t a2 ) {
		return ( do_invoke( *a0, a1, a2 ) );
	}
	return_t operator()( HResource<typename trait::strip<a0_t>::type> a0, a1_t a1, a2_t a2 ) const {
		return ( do_invoke( *a0, a1, a2 ) );
	}
	void const* id( void ) const {
		return ( do_id() );
	}
protected:
	virtual return_t do_invoke( a0_t, a1_t, a2_t ) = 0;
	virtual return_t do_invoke( a0_t, a1_t, a2_t ) const = 0;
	virtual void const* do_id( void ) const = 0;
};

template<typename return_t, typename a0_t, typename a1_t, typename a2_t, typename a3_t>
class HCallInterface<4, return_t, a0_t, a1_t, a2_t, a3_t> {
public:
	typedef return_t result_type;
	typedef a0_t first_argument_type;
	typedef a1_t second_argument_type;
	typedef a2_t third_argument_type;
	typedef a3_t fourth_argument_type;
	virtual ~HCallInterface( void ) {}
	return_t operator()( a0_t a0, a1_t a1, a2_t a2, a3_t a3 ) {
		return ( do_invoke( a0, a1, a2, a3 ) );
	}
	return_t operator()( a0_t a0, a1_t a1, a2_t a2, a3_t a3 ) const {
		return ( do_invoke( a0, a1, a2, a3 ) );
	}
	return_t operator()( typename trait::strip_reference<a0_t>::type* a0, a1_t a1, a2_t a2, a3_t a3 ) {
		return ( do_invoke( *a0, a1, a2, a3 ) );
	}
	return_t operator()( typename trait::strip_reference<a0_t>::type const* a0, a1_t a1, a2_t a2, a3_t a3 ) const {
		return ( do_invoke( *a0, a1, a2, a3 ) );
	}
	return_t operator()( HPointer<typename trait::strip<a0_t>::type> a0, a1_t a1, a2_t a2, a3_t a3 ) {
		return ( do_invoke( *a0, a1, a2, a3 ) );
	}
	return_t operator()( HPointer<typename trait::strip<a0_t>::type> a0, a1_t a1, a2_t a2, a3_t a3 ) const {
		return ( do_invoke( *a0, a1, a2, a3 ) );
	}
	return_t operator()( HResource<typename trait::strip<a0_t>::type> a0, a1_t a1, a2_t a2, a3_t a3 ) {
		return ( do_invoke( *a0, a1, a2, a3 ) );
	}
	return_t operator()( HResource<typename trait::strip<a0_t>::type> a0, a1_t a1, a2_t a2, a3_t a3 ) const {
		return ( do_invoke( *a0, a1, a2, a3 ) );
	}
	void const* id( void ) const {
		return ( do_id() );
	}
protected:
	virtual return_t do_invoke( a0_t, a1_t, a2_t, a3_t ) = 0;
	virtual return_t do_invoke( a0_t, a1_t, a2_t, a3_t ) const = 0;
	virtual void const* do_id( void ) const = 0;
};

template<typename return_t, typename a0_t, typename a1_t, typename a2_t, typename a3_t, typename a4_t>
class HCallInterface<5, return_t, a0_t, a1_t, a2_t, a3_t, a4_t> {
public:
	typedef return_t result_type;
	typedef a0_t first_argument_type;
	typedef a1_t second_argument_type;
	typedef a2_t third_argument_type;
	typedef a3_t fourth_argument_type;
	typedef a4_t fifth_argument_type;
	virtual ~HCallInterface( void ) {}
	return_t operator()( a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4 ) {
		return ( do_invoke( a0, a1, a2, a3, a4 ) );
	}
	return_t operator()( a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4 ) const {
		return ( do_invoke( a0, a1, a2, a3, a4 ) );
	}
	return_t operator()( typename trait::strip_reference<a0_t>::type* a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4 ) {
		return ( do_invoke( *a0, a1, a2, a3, a4 ) );
	}
	return_t operator()( typename trait::strip_reference<a0_t>::type const* a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4 ) const {
		return ( do_invoke( *a0, a1, a2, a3, a4 ) );
	}
	return_t operator()( HPointer<typename trait::strip<a0_t>::type> a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4 ) {
		return ( do_invoke( *a0, a1, a2, a3, a4 ) );
	}
	return_t operator()( HPointer<typename trait::strip<a0_t>::type> a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4 ) const {
		return ( do_invoke( *a0, a1, a2, a3, a4 ) );
	}
	return_t operator()( HResource<typename trait::strip<a0_t>::type> a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4 ) {
		return ( do_invoke( *a0, a1, a2, a3, a4 ) );
	}
	return_t operator()( HResource<typename trait::strip<a0_t>::type> a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4 ) const {
		return ( do_invoke( *a0, a1, a2, a3, a4 ) );
	}
	void const* id( void ) const {
		return ( do_id() );
	}
protected:
	virtual return_t do_invoke( a0_t, a1_t, a2_t, a3_t, a4_t ) = 0;
	virtual return_t do_invoke( a0_t, a1_t, a2_t, a3_t, a4_t ) const = 0;
	virtual void const* do_id( void ) const = 0;
};

template<typename return_t, typename a0_t, typename a1_t, typename a2_t, typename a3_t, typename a4_t, typename a5_t>
class HCallInterface<6, return_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t> {
public:
	typedef return_t result_type;
	typedef a0_t first_argument_type;
	typedef a1_t second_argument_type;
	typedef a2_t third_argument_type;
	typedef a3_t fourth_argument_type;
	typedef a4_t fifth_argument_type;
	typedef a5_t sixth_argument_type;
	virtual ~HCallInterface( void ) {}
	return_t operator()( a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5 ) {
		return ( do_invoke( a0, a1, a2, a3, a4, a5 ) );
	}
	return_t operator()( a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5 ) const {
		return ( do_invoke( a0, a1, a2, a3, a4, a5 ) );
	}
	return_t operator()( typename trait::strip_reference<a0_t>::type* a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5 ) {
		return ( do_invoke( *a0, a1, a2, a3, a4, a5 ) );
	}
	return_t operator()( typename trait::strip_reference<a0_t>::type const* a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5 ) const {
		return ( do_invoke( *a0, a1, a2, a3, a4, a5 ) );
	}
	return_t operator()( HPointer<typename trait::strip<a0_t>::type> a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5 ) {
		return ( do_invoke( *a0, a1, a2, a3, a4, a5 ) );
	}
	return_t operator()( HPointer<typename trait::strip<a0_t>::type> a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5 ) const {
		return ( do_invoke( *a0, a1, a2, a3, a4, a5 ) );
	}
	return_t operator()( HResource<typename trait::strip<a0_t>::type> a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5 ) {
		return ( do_invoke( *a0, a1, a2, a3, a4, a5 ) );
	}
	return_t operator()( HResource<typename trait::strip<a0_t>::type> a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5 ) const {
		return ( do_invoke( *a0, a1, a2, a3, a4, a5 ) );
	}
	void const* id( void ) const {
		return ( do_id() );
	}
protected:
	virtual return_t do_invoke( a0_t, a1_t, a2_t, a3_t, a4_t, a5_t ) = 0;
	virtual return_t do_invoke( a0_t, a1_t, a2_t, a3_t, a4_t, a5_t ) const = 0;
	virtual void const* do_id( void ) const = 0;
};

template<typename return_t, typename a0_t, typename a1_t, typename a2_t, typename a3_t, typename a4_t, typename a5_t, typename a6_t>
class HCallInterface<7, return_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t> {
public:
	typedef return_t result_type;
	typedef a0_t first_argument_type;
	typedef a1_t second_argument_type;
	typedef a2_t third_argument_type;
	typedef a3_t fourth_argument_type;
	typedef a4_t fifth_argument_type;
	typedef a5_t sixth_argument_type;
	typedef a6_t seventh_argument_type;
	virtual ~HCallInterface( void ) {}
	return_t operator()( a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6 ) {
		return ( do_invoke( a0, a1, a2, a3, a4, a5, a6 ) );
	}
	return_t operator()( a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6 ) const {
		return ( do_invoke( a0, a1, a2, a3, a4, a5, a6 ) );
	}
	return_t operator()( typename trait::strip_reference<a0_t>::type* a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6 ) {
		return ( do_invoke( *a0, a1, a2, a3, a4, a5, a6 ) );
	}
	return_t operator()( typename trait::strip_reference<a0_t>::type const* a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6 ) const {
		return ( do_invoke( *a0, a1, a2, a3, a4, a5, a6 ) );
	}
	return_t operator()( HPointer<typename trait::strip<a0_t>::type> a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6 ) {
		return ( do_invoke( *a0, a1, a2, a3, a4, a5, a6 ) );
	}
	return_t operator()( HPointer<typename trait::strip<a0_t>::type> a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6 ) const {
		return ( do_invoke( *a0, a1, a2, a3, a4, a5, a6 ) );
	}
	return_t operator()( HResource<typename trait::strip<a0_t>::type> a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6 ) {
		return ( do_invoke( *a0, a1, a2, a3, a4, a5, a6 ) );
	}
	return_t operator()( HResource<typename trait::strip<a0_t>::type> a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6 ) const {
		return ( do_invoke( *a0, a1, a2, a3, a4, a5, a6 ) );
	}
	void const* id( void ) const {
		return ( do_id() );
	}
protected:
	virtual return_t do_invoke( a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t ) = 0;
	virtual return_t do_invoke( a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t ) const = 0;
	virtual void const* do_id( void ) const = 0;
};

template<typename return_t, typename a0_t, typename a1_t, typename a2_t, typename a3_t, typename a4_t, typename a5_t, typename a6_t, typename a7_t>
class HCallInterface<8, return_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t> {
public:
	typedef return_t result_type;
	typedef a0_t first_argument_type;
	typedef a1_t second_argument_type;
	typedef a2_t third_argument_type;
	typedef a3_t fourth_argument_type;
	typedef a4_t fifth_argument_type;
	typedef a5_t sixth_argument_type;
	typedef a6_t seventh_argument_type;
	typedef a7_t eighth_argument_type;
	virtual ~HCallInterface( void ) {}
	return_t operator()( a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7 ) {
		return ( do_invoke( a0, a1, a2, a3, a4, a5, a6, a7 ) );
	}
	return_t operator()( a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7 ) const {
		return ( do_invoke( a0, a1, a2, a3, a4, a5, a6, a7 ) );
	}
	return_t operator()( typename trait::strip_reference<a0_t>::type* a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7 ) {
		return ( do_invoke( *a0, a1, a2, a3, a4, a5, a6, a7 ) );
	}
	return_t operator()( typename trait::strip_reference<a0_t>::type const* a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7 ) const {
		return ( do_invoke( *a0, a1, a2, a3, a4, a5, a6, a7 ) );
	}
	return_t operator()( HPointer<typename trait::strip<a0_t>::type> a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7 ) {
		return ( do_invoke( *a0, a1, a2, a3, a4, a5, a6, a7 ) );
	}
	return_t operator()( HPointer<typename trait::strip<a0_t>::type> a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7 ) const {
		return ( do_invoke( *a0, a1, a2, a3, a4, a5, a6, a7 ) );
	}
	return_t operator()( HResource<typename trait::strip<a0_t>::type> a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7 ) {
		return ( do_invoke( *a0, a1, a2, a3, a4, a5, a6, a7 ) );
	}
	return_t operator()( HResource<typename trait::strip<a0_t>::type> a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7 ) const {
		return ( do_invoke( *a0, a1, a2, a3, a4, a5, a6, a7 ) );
	}
	void const* id( void ) const {
		return ( do_id() );
	}
protected:
	virtual return_t do_invoke( a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t ) = 0;
	virtual return_t do_invoke( a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t ) const = 0;
	virtual void const* do_id( void ) const = 0;
};

template<typename return_t, typename a0_t, typename a1_t, typename a2_t, typename a3_t, typename a4_t, typename a5_t, typename a6_t, typename a7_t, typename a8_t>
class HCallInterface<9, return_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t> {
public:
	typedef return_t result_type;
	typedef a0_t first_argument_type;
	typedef a1_t second_argument_type;
	typedef a2_t third_argument_type;
	typedef a3_t fourth_argument_type;
	typedef a4_t fifth_argument_type;
	typedef a5_t sixth_argument_type;
	typedef a6_t seventh_argument_type;
	typedef a7_t eighth_argument_type;
	typedef a8_t ninth_argument_type;
	virtual ~HCallInterface( void ) {}
	return_t operator()( a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7, a8_t a8 ) {
		return ( do_invoke( a0, a1, a2, a3, a4, a5, a6, a7, a8 ) );
	}
	return_t operator()( a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7, a8_t a8 ) const {
		return ( do_invoke( a0, a1, a2, a3, a4, a5, a6, a7, a8 ) );
	}
	return_t operator()( typename trait::strip_reference<a0_t>::type* a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7, a8_t a8 ) {
		return ( do_invoke( *a0, a1, a2, a3, a4, a5, a6, a7, a8 ) );
	}
	return_t operator()( typename trait::strip_reference<a0_t>::type const* a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7, a8_t a8 ) const {
		return ( do_invoke( *a0, a1, a2, a3, a4, a5, a6, a7, a8 ) );
	}
	return_t operator()( HPointer<typename trait::strip<a0_t>::type> a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7, a8_t a8 ) {
		return ( do_invoke( *a0, a1, a2, a3, a4, a5, a6, a7, a8 ) );
	}
	return_t operator()( HPointer<typename trait::strip<a0_t>::type> a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7, a8_t a8 ) const {
		return ( do_invoke( *a0, a1, a2, a3, a4, a5, a6, a7, a8 ) );
	}
	return_t operator()( HResource<typename trait::strip<a0_t>::type> a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7, a8_t a8 ) {
		return ( do_invoke( *a0, a1, a2, a3, a4, a5, a6, a7, a8 ) );
	}
	return_t operator()( HResource<typename trait::strip<a0_t>::type> a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7, a8_t a8 ) const {
		return ( do_invoke( *a0, a1, a2, a3, a4, a5, a6, a7, a8 ) );
	}
	void const* id( void ) const {
		return ( do_id() );
	}
protected:
	virtual return_t do_invoke( a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t ) = 0;
	virtual return_t do_invoke( a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t ) const = 0;
	virtual void const* do_id( void ) const = 0;
};

template<typename return_t, typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t, typename a6_t, typename a7_t, typename a8_t, typename a9_t>
class HCallInterface<10, return_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t> {
public:
	typedef return_t result_type;
	typedef a0_t first_argument_type;
	typedef a1_t second_argument_type;
	typedef a2_t third_argument_type;
	typedef a3_t fourth_argument_type;
	typedef a4_t fifth_argument_type;
	typedef a5_t sixth_argument_type;
	typedef a6_t seventh_argument_type;
	typedef a7_t eighth_argument_type;
	typedef a8_t ninth_argument_type;
	typedef a9_t tenth_argument_type;
	virtual ~HCallInterface( void ) {}
	return_t operator()( a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7, a8_t a8, a9_t a9 ) {
		return ( do_invoke( a0, a1, a2, a3, a4, a5, a6, a7, a8, a9 ) );
	}
	return_t operator()( a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7, a8_t a8, a9_t a9 ) const {
		return ( do_invoke( a0, a1, a2, a3, a4, a5, a6, a7, a8, a9 ) );
	}
	return_t operator()( typename trait::strip_reference<a0_t>::type* a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7, a8_t a8, a9_t a9 ) {
		return ( do_invoke( *a0, a1, a2, a3, a4, a5, a6, a7, a8, a9 ) );
	}
	return_t operator()( typename trait::strip_reference<a0_t>::type const* a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7, a8_t a8, a9_t a9 ) const {
		return ( do_invoke( *a0, a1, a2, a3, a4, a5, a6, a7, a8, a9 ) );
	}
	return_t operator()( HPointer<typename trait::strip<a0_t>::type> a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7, a8_t a8, a9_t a9 ) {
		return ( do_invoke( *a0, a1, a2, a3, a4, a5, a6, a7, a8, a9 ) );
	}
	return_t operator()( HPointer<typename trait::strip<a0_t>::type> a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7, a8_t a8, a9_t a9 ) const {
		return ( do_invoke( *a0, a1, a2, a3, a4, a5, a6, a7, a8, a9 ) );
	}
	return_t operator()( HResource<typename trait::strip<a0_t>::type> a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7, a8_t a8, a9_t a9 ) {
		return ( do_invoke( *a0, a1, a2, a3, a4, a5, a6, a7, a8, a9 ) );
	}
	return_t operator()( HResource<typename trait::strip<a0_t>::type> a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7, a8_t a8, a9_t a9 ) const {
		return ( do_invoke( *a0, a1, a2, a3, a4, a5, a6, a7, a8, a9 ) );
	}
	void const* id( void ) const {
		return ( do_id() );
	}
protected:
	virtual return_t do_invoke( a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t ) = 0;
	virtual return_t do_invoke( a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t ) const = 0;
	virtual void const* do_id( void ) const = 0;
};

template<typename return_t, typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t, typename a6_t, typename a7_t, typename a8_t, typename a9_t, typename a10_t>
class HCallInterface<11, return_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t, a10_t> {
public:
	typedef return_t result_type;
	typedef a0_t first_argument_type;
	typedef a1_t second_argument_type;
	typedef a2_t third_argument_type;
	typedef a3_t fourth_argument_type;
	typedef a4_t fifth_argument_type;
	typedef a5_t sixth_argument_type;
	typedef a6_t seventh_argument_type;
	typedef a7_t eighth_argument_type;
	typedef a8_t ninth_argument_type;
	typedef a9_t tenth_argument_type;
	typedef a10_t eleventh_argument_type;
	virtual ~HCallInterface( void ) {}
	return_t operator()( a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7, a8_t a8, a9_t a9, a10_t a10 ) {
		return ( do_invoke( a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10 ) );
	}
	return_t operator()( a0_t a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7, a8_t a8, a9_t a9, a10_t a10 ) const {
		return ( do_invoke( a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10 ) );
	}
	return_t operator()( typename trait::strip_reference<a0_t>::type* a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7, a8_t a8, a9_t a9, a10_t a10 ) {
		return ( do_invoke( *a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10 ) );
	}
	return_t operator()( typename trait::strip_reference<a0_t>::type const* a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7, a8_t a8, a9_t a9, a10_t a10 ) const {
		return ( do_invoke( *a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10 ) );
	}
	return_t operator()( HPointer<typename trait::strip<a0_t>::type> a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7, a8_t a8, a9_t a9, a10_t a10 ) {
		return ( do_invoke( *a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10 ) );
	}
	return_t operator()( HPointer<typename trait::strip<a0_t>::type> a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7, a8_t a8, a9_t a9, a10_t a10 ) const {
		return ( do_invoke( *a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10 ) );
	}
	return_t operator()( HResource<typename trait::strip<a0_t>::type> a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7, a8_t a8, a9_t a9, a10_t a10 ) {
		return ( do_invoke( *a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10 ) );
	}
	return_t operator()( HResource<typename trait::strip<a0_t>::type> a0, a1_t a1, a2_t a2, a3_t a3, a4_t a4, a5_t a5, a6_t a6, a7_t a7, a8_t a8, a9_t a9, a10_t a10 ) const {
		return ( do_invoke( *a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10 ) );
	}
	void const* id( void ) const {
		return ( do_id() );
	}
protected:
	virtual return_t do_invoke( a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t, a10_t ) = 0;
	virtual return_t do_invoke( a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t, a10_t ) const = 0;
	virtual void const* do_id( void ) const = 0;
};

template<typename T>
struct has_id_member {
private:
	template<typename X> static auto test( int ) -> decltype( static_cast<X*>( nullptr )->id(), static_cast<trait::true_type*>( nullptr ) );
	template<typename> static trait::false_type* test( ... );
public:
	static bool const value = trait::same_type<decltype( test<T>( 0 ) ), trait::true_type*>::value;
};

template<typename CLASS_t, typename T = decltype( static_cast<CLASS_t*>( nullptr )->id() )>
void const* caller_id( CLASS_t const& object_, T const* = nullptr ) {
	return ( object_.id() );
}

template<typename CLASS_t, typename T = typename trait::enable_if<trait::is_function_pointer<typename trait::decay<CLASS_t>::type>::value>::type>
void const* caller_id( CLASS_t const& object_, T const** = nullptr ) {
	return ( bit_cast<void const*>( object_ ) );
}

template<
	typename CLASS_t,
	typename T = typename trait::enable_if<!trait::is_function_pointer<typename trait::decay<CLASS_t>::type>::value>::type,
	typename U = typename trait::enable_if<!has_id_member<CLASS_t>::value>::type
>
void const* caller_id( CLASS_t const& object_, T const*** = nullptr ) {
	return ( &object_ );
}

/*! \brief Implementation of abstraction of any-method of any-class invocation.
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
	typename fa8_t = trait::no_type, typename fa9_t = trait::no_type,
	typename fa10_t = trait::no_type>
struct call_calculator {
	static int const free_standing_args_count =
		+ meta::ternary<trait::is_kind_of<fa0_t, higher_order::placeholder_base>::value, 1, 0>::value
		+ meta::ternary<trait::is_kind_of<fa1_t, higher_order::placeholder_base>::value, 1, 0>::value
		+ meta::ternary<trait::is_kind_of<fa2_t, higher_order::placeholder_base>::value, 1, 0>::value
		+ meta::ternary<trait::is_kind_of<fa3_t, higher_order::placeholder_base>::value, 1, 0>::value
		+ meta::ternary<trait::is_kind_of<fa4_t, higher_order::placeholder_base>::value, 1, 0>::value
		+ meta::ternary<trait::is_kind_of<fa5_t, higher_order::placeholder_base>::value, 1, 0>::value
		+ meta::ternary<trait::is_kind_of<fa6_t, higher_order::placeholder_base>::value, 1, 0>::value
		+ meta::ternary<trait::is_kind_of<fa7_t, higher_order::placeholder_base>::value, 1, 0>::value
		+ meta::ternary<trait::is_kind_of<fa8_t, higher_order::placeholder_base>::value, 1, 0>::value
		+ meta::ternary<trait::is_kind_of<fa9_t, higher_order::placeholder_base>::value, 1, 0>::value
		+ meta::ternary<trait::is_kind_of<fa10_t, higher_order::placeholder_base>::value, 1, 0>::value;
	template<
		typename ra0_t, typename f0_t,
		typename ra1_t, typename f1_t,
		typename ra2_t, typename f2_t,
		typename ra3_t, typename f3_t,
		typename ra4_t, typename f4_t,
		typename ra5_t, typename f5_t,
		typename ra6_t, typename f6_t,
		typename ra7_t, typename f7_t,
		typename ra8_t, typename f8_t,
		typename ra9_t, typename f9_t,
		typename ra10_t, typename f10_t
	>
	struct free_standing_args {
		template<typename T>
		struct strip {
			typedef typename trait::ternary<trait::is_kind_of<T, higher_order::placeholder_base>::value, typename trait::strip<T>::type, T>::type type;
		};
		typedef typename strip<f0_t>::type sa0_t;
		typedef typename strip<f1_t>::type sa1_t;
		typedef typename strip<f2_t>::type sa2_t;
		typedef typename strip<f3_t>::type sa3_t;
		typedef typename strip<f4_t>::type sa4_t;
		typedef typename strip<f5_t>::type sa5_t;
		typedef typename strip<f6_t>::type sa6_t;
		typedef typename strip<f7_t>::type sa7_t;
		typedef typename strip<f8_t>::type sa8_t;
		typedef typename strip<f9_t>::type sa9_t;
		typedef typename strip<f10_t>::type sa10_t;
		/*
		 * saN_t are types deduced from call() arguments with placeholder being stripped.
		 * raN_t are true argument types of function/method.
		 */
		typedef typename trait::select_index<trait::find_type<higher_order::placeholder<1>,
			sa0_t, sa1_t, sa2_t, sa3_t, sa4_t, sa5_t, sa6_t, sa7_t, sa8_t, sa9_t, sa10_t>::value,
			ra0_t, ra1_t, ra2_t, ra3_t, ra4_t, ra5_t, ra6_t, ra7_t, ra8_t, ra9_t, ra10_t>::type a0_t;
		typedef typename trait::select_index<trait::find_type<higher_order::placeholder<2>,
			sa0_t, sa1_t, sa2_t, sa3_t, sa4_t, sa5_t, sa6_t, sa7_t, sa8_t, sa9_t, sa10_t>::value,
			ra0_t, ra1_t, ra2_t, ra3_t, ra4_t, ra5_t, ra6_t, ra7_t, ra8_t, ra9_t, ra10_t>::type a1_t;
		typedef typename trait::select_index<trait::find_type<higher_order::placeholder<3>,
			sa0_t, sa1_t, sa2_t, sa3_t, sa4_t, sa5_t, sa6_t, sa7_t, sa8_t, sa9_t, sa10_t>::value,
			ra0_t, ra1_t, ra2_t, ra3_t, ra4_t, ra5_t, ra6_t, ra7_t, ra8_t, ra9_t, ra10_t>::type a2_t;
		typedef typename trait::select_index<trait::find_type<higher_order::placeholder<4>,
			sa0_t, sa1_t, sa2_t, sa3_t, sa4_t, sa5_t, sa6_t, sa7_t, sa8_t, sa9_t, sa10_t>::value,
			ra0_t, ra1_t, ra2_t, ra3_t, ra4_t, ra5_t, ra6_t, ra7_t, ra8_t, ra9_t, ra10_t>::type a3_t;
		typedef typename trait::select_index<trait::find_type<higher_order::placeholder<5>,
			sa0_t, sa1_t, sa2_t, sa3_t, sa4_t, sa5_t, sa6_t, sa7_t, sa8_t, sa9_t, sa10_t>::value,
			ra0_t, ra1_t, ra2_t, ra3_t, ra4_t, ra5_t, ra6_t, ra7_t, ra8_t, ra9_t, ra10_t>::type a4_t;
		typedef typename trait::select_index<trait::find_type<higher_order::placeholder<6>,
			sa0_t, sa1_t, sa2_t, sa3_t, sa4_t, sa5_t, sa6_t, sa7_t, sa8_t, sa9_t, sa10_t>::value,
			ra0_t, ra1_t, ra2_t, ra3_t, ra4_t, ra5_t, ra6_t, ra7_t, ra8_t, ra9_t, ra10_t>::type a5_t;
		typedef typename trait::select_index<trait::find_type<higher_order::placeholder<7>,
			sa0_t, sa1_t, sa2_t, sa3_t, sa4_t, sa5_t, sa6_t, sa7_t, sa8_t, sa9_t, sa10_t>::value,
			ra0_t, ra1_t, ra2_t, ra3_t, ra4_t, ra5_t, ra6_t, ra7_t, ra8_t, ra9_t, ra10_t>::type a6_t;
		typedef typename trait::select_index<trait::find_type<higher_order::placeholder<8>,
			sa0_t, sa1_t, sa2_t, sa3_t, sa4_t, sa5_t, sa6_t, sa7_t, sa8_t, sa9_t, sa10_t>::value,
			ra0_t, ra1_t, ra2_t, ra3_t, ra4_t, ra5_t, ra6_t, ra7_t, ra8_t, ra9_t, ra10_t>::type a7_t;
		typedef typename trait::select_index<trait::find_type<higher_order::placeholder<9>,
			sa0_t, sa1_t, sa2_t, sa3_t, sa4_t, sa5_t, sa6_t, sa7_t, sa8_t, sa9_t, sa10_t>::value,
			ra0_t, ra1_t, ra2_t, ra3_t, ra4_t, ra5_t, ra6_t, ra7_t, ra8_t, ra9_t, ra10_t>::type a8_t;
		typedef typename trait::select_index<trait::find_type<higher_order::placeholder<10>,
			sa0_t, sa1_t, sa2_t, sa3_t, sa4_t, sa5_t, sa6_t, sa7_t, sa8_t, sa9_t, sa10_t>::value,
			ra0_t, ra1_t, ra2_t, ra3_t, ra4_t, ra5_t, ra6_t, ra7_t, ra8_t, ra9_t, ra10_t>::type a9_t;
		typedef typename trait::select_index<trait::find_type<higher_order::placeholder<11>,
			sa0_t, sa1_t, sa2_t, sa3_t, sa4_t, sa5_t, sa6_t, sa7_t, sa8_t, sa9_t, sa10_t>::value,
			ra0_t, ra1_t, ra2_t, ra3_t, ra4_t, ra5_t, ra6_t, ra7_t, ra8_t, ra9_t, ra10_t>::type a10_t;
	};
	/*
	 * For following uses:
	 * class Class {
	 *   void foo( char const*, char const* );
	 * };
	 * Class obj;
	 * auto c = call( &Class::foo, &obj, "var1", _1 );
	 * c( "var2" );
	 */
	struct functor {
		typedef HFunctor<fa0_t, METHOD_t> call_type;
		typedef HCall<free_standing_args_count, functor, typename trait::return_type<METHOD_t>::type,
						call_type, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t, fa9_t, fa10_t> type;
		typedef free_standing_args<
			typename trait::argument_type<METHOD_t, 0>::type, fa1_t,
			typename trait::argument_type<METHOD_t, 1>::type, fa2_t,
			typename trait::argument_type<METHOD_t, 2>::type, fa3_t,
			typename trait::argument_type<METHOD_t, 3>::type, fa4_t,
			typename trait::argument_type<METHOD_t, 4>::type, fa5_t,
			typename trait::argument_type<METHOD_t, 5>::type, fa6_t,
			typename trait::argument_type<METHOD_t, 6>::type, fa7_t,
			typename trait::argument_type<METHOD_t, 7>::type, fa8_t,
			typename trait::argument_type<METHOD_t, 8>::type, fa9_t,
			typename trait::argument_type<METHOD_t, 9>::type, fa10_t,
			trait::no_type, trait::no_type
		> sorted_real_args;
		typedef typename sorted_real_args::a0_t arg0_t;
		typedef HCallInterface<free_standing_args_count, typename trait::return_type<METHOD_t>::type,
						arg0_t, typename sorted_real_args::a1_t, typename sorted_real_args::a2_t, typename sorted_real_args::a3_t,
						typename sorted_real_args::a4_t, typename sorted_real_args::a5_t, typename sorted_real_args::a6_t, typename sorted_real_args::a7_t,
						typename sorted_real_args::a8_t, typename sorted_real_args::a9_t, typename sorted_real_args::a10_t> interface_type;
		typedef typename trait::decay<fa1_t>::type v0_t;
		typedef typename trait::decay<fa2_t>::type v1_t;
		typedef typename trait::decay<fa3_t>::type v2_t;
		typedef typename trait::decay<fa4_t>::type v3_t;
		typedef typename trait::decay<fa5_t>::type v4_t;
		typedef typename trait::decay<fa6_t>::type v5_t;
		typedef typename trait::decay<fa7_t>::type v6_t;
		typedef typename trait::decay<fa8_t>::type v7_t;
		typedef typename trait::decay<fa9_t>::type v8_t;
		typedef typename trait::decay<fa10_t>::type v9_t;
		typedef typename trait::no_type v10_t;
		inline static type make( METHOD_t&& m, fa0_t&& fa0, fa1_t&& fa1, fa2_t&& fa2, fa3_t&& fa3, fa4_t&& fa4, fa5_t&& fa5, fa6_t&& fa6, fa7_t&& fa7, fa8_t&& fa8, fa9_t&& fa9, fa10_t&& fa10 ) {
			return ( type( call_type( yaal::forward<fa0_t>( fa0 ), yaal::forward<METHOD_t>( m ) ),
						yaal::forward<fa1_t>( fa1 ), yaal::forward<fa2_t>( fa2 ), yaal::forward<fa3_t>( fa3 ), yaal::forward<fa4_t>( fa4 ),
						yaal::forward<fa5_t>( fa5 ), yaal::forward<fa6_t>( fa6 ), yaal::forward<fa7_t>( fa7 ), yaal::forward<fa8_t>( fa8 ),
						yaal::forward<fa9_t>( fa9 ), yaal::forward<fa10_t>( fa10 ) ) );
		}
		inline static type make( METHOD_t&& m, fa0_t&& fa0, fa1_t&& fa1, fa2_t&& fa2, fa3_t&& fa3, fa4_t&& fa4, fa5_t&& fa5, fa6_t&& fa6, fa7_t&& fa7, fa8_t&& fa8, fa9_t&& fa9 ) {
			return ( type( call_type( yaal::forward<fa0_t>( fa0 ), yaal::forward<METHOD_t>( m ) ),
						yaal::forward<fa1_t>( fa1 ), yaal::forward<fa2_t>( fa2 ), yaal::forward<fa3_t>( fa3 ), yaal::forward<fa4_t>( fa4 ),
						yaal::forward<fa5_t>( fa5 ), yaal::forward<fa6_t>( fa6 ), yaal::forward<fa7_t>( fa7 ), yaal::forward<fa8_t>( fa8 ),
						yaal::forward<fa9_t>( fa9 ) ) );
		}
		inline static type make( METHOD_t&& m, fa0_t&& fa0, fa1_t&& fa1, fa2_t&& fa2, fa3_t&& fa3, fa4_t&& fa4, fa5_t&& fa5, fa6_t&& fa6, fa7_t&& fa7, fa8_t&& fa8 ) {
			return ( type( call_type( yaal::forward<fa0_t>( fa0 ), yaal::forward<METHOD_t>( m ) ),
						yaal::forward<fa1_t>( fa1 ), yaal::forward<fa2_t>( fa2 ), yaal::forward<fa3_t>( fa3 ), yaal::forward<fa4_t>( fa4 ),
						yaal::forward<fa5_t>( fa5 ), yaal::forward<fa6_t>( fa6 ), yaal::forward<fa7_t>( fa7 ), yaal::forward<fa8_t>( fa8 ) ) );
		}
		inline static type make( METHOD_t&& m, fa0_t&& fa0, fa1_t&& fa1, fa2_t&& fa2, fa3_t&& fa3, fa4_t&& fa4, fa5_t&& fa5, fa6_t&& fa6, fa7_t&& fa7 ) {
			return ( type( call_type( yaal::forward<fa0_t>( fa0 ), yaal::forward<METHOD_t>( m ) ),
						yaal::forward<fa1_t>( fa1 ), yaal::forward<fa2_t>( fa2 ), yaal::forward<fa3_t>( fa3 ), yaal::forward<fa4_t>( fa4 ),
						yaal::forward<fa5_t>( fa5 ), yaal::forward<fa6_t>( fa6 ), yaal::forward<fa7_t>( fa7 ) ) );
		}
		inline static type make( METHOD_t&& m, fa0_t&& fa0, fa1_t&& fa1, fa2_t&& fa2, fa3_t&& fa3, fa4_t&& fa4, fa5_t&& fa5, fa6_t&& fa6 ) {
			return ( type( call_type( yaal::forward<fa0_t>( fa0 ), yaal::forward<METHOD_t>( m ) ),
						yaal::forward<fa1_t>( fa1 ), yaal::forward<fa2_t>( fa2 ), yaal::forward<fa3_t>( fa3 ), yaal::forward<fa4_t>( fa4 ),
						yaal::forward<fa5_t>( fa5 ), yaal::forward<fa6_t>( fa6 ) ) );
		}
		inline static type make( METHOD_t&& m, fa0_t&& fa0, fa1_t&& fa1, fa2_t&& fa2, fa3_t&& fa3, fa4_t&& fa4, fa5_t&& fa5 ) {
			return ( type( call_type( yaal::forward<fa0_t>( fa0 ), yaal::forward<METHOD_t>( m ) ),
						yaal::forward<fa1_t>( fa1 ), yaal::forward<fa2_t>( fa2 ), yaal::forward<fa3_t>( fa3 ), yaal::forward<fa4_t>( fa4 ),
						yaal::forward<fa5_t>( fa5 ) ) );
		}
		inline static type make( METHOD_t&& m, fa0_t&& fa0, fa1_t&& fa1, fa2_t&& fa2, fa3_t&& fa3, fa4_t&& fa4 ) {
			return ( type( call_type( yaal::forward<fa0_t>( fa0 ), yaal::forward<METHOD_t>( m ) ),
						yaal::forward<fa1_t>( fa1 ), yaal::forward<fa2_t>( fa2 ), yaal::forward<fa3_t>( fa3 ), yaal::forward<fa4_t>( fa4 ) ) );
		}
		inline static type make( METHOD_t&& m, fa0_t&& fa0, fa1_t&& fa1, fa2_t&& fa2, fa3_t&& fa3 ) {
			return ( type( call_type( yaal::forward<fa0_t>( fa0 ), yaal::forward<METHOD_t>( m ) ), yaal::forward<fa1_t>( fa1 ), yaal::forward<fa2_t>( fa2 ), yaal::forward<fa3_t>( fa3 ) ) );
		}
		inline static type make( METHOD_t&& m, fa0_t&& fa0, fa1_t&& fa1, fa2_t&& fa2 ) {
			return ( type( call_type( yaal::forward<fa0_t>( fa0 ), yaal::forward<METHOD_t>( m ) ), yaal::forward<fa1_t>( fa1 ), yaal::forward<fa2_t>( fa2 ) ) );
		}
		inline static type make( METHOD_t&& m, fa0_t&& fa0, fa1_t&& fa1 ) {
			return ( type( call_type( yaal::forward<fa0_t>( fa0 ), yaal::forward<METHOD_t>( m ) ), yaal::forward<fa1_t>( fa1 ) ) );
		}
		inline static type make( METHOD_t&& m, fa0_t&& fa0 ) {
			return ( type( call_type( yaal::forward<fa0_t>( fa0 ), yaal::forward<METHOD_t>( m ) ) ) );
		}
	};
	/*
	 * For following uses:
	 * class Class {
	 *   void foo( char const*, char const* );
	 * };
	 * Class obj;
	 * auto c = call( &Class::foo, _1, "var1", _2 );
	 * c( &obj, "var2" );
	 */
	struct functor_this {
		typedef HFunctor<fa0_t, METHOD_t> call_type;
		typedef HCall<free_standing_args_count, functor_this, typename trait::return_type<METHOD_t>::type,
						call_type, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t, fa9_t, fa10_t> type;
		typedef free_standing_args<
			typename trait::class_type<METHOD_t>::type&, fa0_t,
			typename trait::argument_type<METHOD_t, 0>::type, fa1_t,
			typename trait::argument_type<METHOD_t, 1>::type, fa2_t,
			typename trait::argument_type<METHOD_t, 2>::type, fa3_t,
			typename trait::argument_type<METHOD_t, 3>::type, fa4_t,
			typename trait::argument_type<METHOD_t, 4>::type, fa5_t,
			typename trait::argument_type<METHOD_t, 5>::type, fa6_t,
			typename trait::argument_type<METHOD_t, 6>::type, fa7_t,
			typename trait::argument_type<METHOD_t, 7>::type, fa8_t,
			typename trait::argument_type<METHOD_t, 8>::type, fa9_t,
			typename trait::argument_type<METHOD_t, 9>::type, fa10_t
		> sorted_real_args;
		typedef typename trait::ternary<trait::is_member_const<METHOD_t>::value, typename trait::make_const<typename sorted_real_args::a0_t>::type,
							typename sorted_real_args::a0_t>::type arg0_t;
		typedef HCallInterface<free_standing_args_count, typename trait::return_type<METHOD_t>::type,
						arg0_t,
						typename sorted_real_args::a1_t, typename sorted_real_args::a2_t, typename sorted_real_args::a3_t,
						typename sorted_real_args::a4_t, typename sorted_real_args::a5_t, typename sorted_real_args::a6_t, typename sorted_real_args::a7_t,
						typename sorted_real_args::a8_t, typename sorted_real_args::a9_t, typename sorted_real_args::a10_t> interface_type;
		typedef typename trait::decay<fa0_t>::type v0_t;
		typedef typename trait::decay<fa1_t>::type v1_t;
		typedef typename trait::decay<fa2_t>::type v2_t;
		typedef typename trait::decay<fa3_t>::type v3_t;
		typedef typename trait::decay<fa4_t>::type v4_t;
		typedef typename trait::decay<fa5_t>::type v5_t;
		typedef typename trait::decay<fa6_t>::type v6_t;
		typedef typename trait::decay<fa7_t>::type v7_t;
		typedef typename trait::decay<fa8_t>::type v8_t;
		typedef typename trait::decay<fa9_t>::type v9_t;
		typedef typename trait::decay<fa10_t>::type v10_t;
		inline static type make( METHOD_t&& m, fa0_t&& fa0, fa1_t&& fa1, fa2_t&& fa2, fa3_t&& fa3, fa4_t&& fa4, fa5_t&& fa5, fa6_t&& fa6, fa7_t&& fa7, fa8_t&& fa8, fa9_t&& fa9, fa10_t&& fa10 ) {
			return ( type( call_type( yaal::forward<fa0_t>( fa0 ), yaal::forward<METHOD_t>( m ) ),
						yaal::forward<fa0_t>( fa0 ), yaal::forward<fa1_t>( fa1 ), yaal::forward<fa2_t>( fa2 ), yaal::forward<fa3_t>( fa3 ), yaal::forward<fa4_t>( fa4 ),
						yaal::forward<fa5_t>( fa5 ), yaal::forward<fa6_t>( fa6 ), yaal::forward<fa7_t>( fa7 ), yaal::forward<fa8_t>( fa8 ), yaal::forward<fa9_t>( fa9 ),
						yaal::forward<fa10_t>( fa10 ) ) );
		}
		inline static type make( METHOD_t&& m, fa0_t&& fa0, fa1_t&& fa1, fa2_t&& fa2, fa3_t&& fa3, fa4_t&& fa4, fa5_t&& fa5, fa6_t&& fa6, fa7_t&& fa7, fa8_t&& fa8, fa9_t&& fa9 ) {
			return ( type( call_type( yaal::forward<fa0_t>( fa0 ), yaal::forward<METHOD_t>( m ) ),
						yaal::forward<fa0_t>( fa0 ), yaal::forward<fa1_t>( fa1 ), yaal::forward<fa2_t>( fa2 ), yaal::forward<fa3_t>( fa3 ), yaal::forward<fa4_t>( fa4 ),
						yaal::forward<fa5_t>( fa5 ), yaal::forward<fa6_t>( fa6 ), yaal::forward<fa7_t>( fa7 ), yaal::forward<fa8_t>( fa8 ), yaal::forward<fa9_t>( fa9 ) ) );
		}
		inline static type make( METHOD_t&& m, fa0_t&& fa0, fa1_t&& fa1, fa2_t&& fa2, fa3_t&& fa3, fa4_t&& fa4, fa5_t&& fa5, fa6_t&& fa6, fa7_t&& fa7, fa8_t&& fa8 ) {
			return ( type( call_type( yaal::forward<fa0_t>( fa0 ), yaal::forward<METHOD_t>( m ) ),
						yaal::forward<fa0_t>( fa0 ), yaal::forward<fa1_t>( fa1 ), yaal::forward<fa2_t>( fa2 ), yaal::forward<fa3_t>( fa3 ), yaal::forward<fa4_t>( fa4 ),
						yaal::forward<fa5_t>( fa5 ), yaal::forward<fa6_t>( fa6 ), yaal::forward<fa7_t>( fa7 ), yaal::forward<fa8_t>( fa8 ) ) );
		}
		inline static type make( METHOD_t&& m, fa0_t&& fa0, fa1_t&& fa1, fa2_t&& fa2, fa3_t&& fa3, fa4_t&& fa4, fa5_t&& fa5, fa6_t&& fa6, fa7_t&& fa7 ) {
			return ( type( call_type( yaal::forward<fa0_t>( fa0 ), yaal::forward<METHOD_t>( m ) ),
						yaal::forward<fa0_t>( fa0 ), yaal::forward<fa1_t>( fa1 ), yaal::forward<fa2_t>( fa2 ), yaal::forward<fa3_t>( fa3 ), yaal::forward<fa4_t>( fa4 ),
						yaal::forward<fa5_t>( fa5 ), yaal::forward<fa6_t>( fa6 ), yaal::forward<fa7_t>( fa7 ) ) );
		}
		inline static type make( METHOD_t&& m, fa0_t&& fa0, fa1_t&& fa1, fa2_t&& fa2, fa3_t&& fa3, fa4_t&& fa4, fa5_t&& fa5, fa6_t&& fa6 ) {
			return ( type( call_type( yaal::forward<fa0_t>( fa0 ), yaal::forward<METHOD_t>( m ) ),
						yaal::forward<fa0_t>( fa0 ), yaal::forward<fa1_t>( fa1 ), yaal::forward<fa2_t>( fa2 ), yaal::forward<fa3_t>( fa3 ), yaal::forward<fa4_t>( fa4 ),
						yaal::forward<fa5_t>( fa5 ), yaal::forward<fa6_t>( fa6 ) ) );
		}
		inline static type make( METHOD_t&& m, fa0_t&& fa0, fa1_t&& fa1, fa2_t&& fa2, fa3_t&& fa3, fa4_t&& fa4, fa5_t&& fa5 ) {
			return ( type( call_type( yaal::forward<fa0_t>( fa0 ), yaal::forward<METHOD_t>( m ) ),
						yaal::forward<fa0_t>( fa0 ), yaal::forward<fa1_t>( fa1 ), yaal::forward<fa2_t>( fa2 ), yaal::forward<fa3_t>( fa3 ), yaal::forward<fa4_t>( fa4 ),
						yaal::forward<fa5_t>( fa5 ) ) );
		}
		inline static type make( METHOD_t&& m, fa0_t&& fa0, fa1_t&& fa1, fa2_t&& fa2, fa3_t&& fa3, fa4_t&& fa4 ) {
			return ( type( call_type( yaal::forward<fa0_t>( fa0 ), yaal::forward<METHOD_t>( m ) ),
						yaal::forward<fa0_t>( fa0 ), yaal::forward<fa1_t>( fa1 ), yaal::forward<fa2_t>( fa2 ), yaal::forward<fa3_t>( fa3 ), yaal::forward<fa4_t>( fa4 ) ) );
		}
		inline static type make( METHOD_t&& m, fa0_t&& fa0, fa1_t&& fa1, fa2_t&& fa2, fa3_t&& fa3 ) {
			return ( type( call_type( yaal::forward<fa0_t>( fa0 ), yaal::forward<METHOD_t>( m ) ),
						yaal::forward<fa0_t>( fa0 ), yaal::forward<fa1_t>( fa1 ), yaal::forward<fa2_t>( fa2 ), yaal::forward<fa3_t>( fa3 ) ) );
		}
		inline static type make( METHOD_t&& m, fa0_t&& fa0, fa1_t&& fa1, fa2_t&& fa2 ) {
			return ( type( call_type( yaal::forward<fa0_t>( fa0 ), yaal::forward<METHOD_t>( m ) ),
						yaal::forward<fa0_t>( fa0 ), yaal::forward<fa1_t>( fa1 ), yaal::forward<fa2_t>( fa2 ) ) );
		}
		inline static type make( METHOD_t&& m, fa0_t&& fa0, fa1_t&& fa1 ) {
			return ( type( call_type( yaal::forward<fa0_t>( fa0 ), yaal::forward<METHOD_t>( m ) ), yaal::forward<fa0_t>( fa0 ), yaal::forward<fa1_t>( fa1 ) ) );
		}
		inline static type make( METHOD_t&& m, fa0_t&& fa0 ) {
			return ( type( call_type( yaal::forward<fa0_t>( fa0 ), yaal::forward<METHOD_t>( m ) ), yaal::forward<fa0_t>( fa0 ) ) );
		}
	};
	/*
	 * For following uses:
	 * void foo( char const*, char const* );
	 * auto c = call( &foo, "var1", _1 );
	 * c( "var2" );
	 */
	struct function {
		typedef METHOD_t call_type;
		typedef HCall<free_standing_args_count, function, typename trait::return_type<METHOD_t>::type,
						METHOD_t, fa0_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t, fa9_t> type;
		typedef free_standing_args<
			typename trait::argument_type<METHOD_t, 0>::type, fa0_t,
			typename trait::argument_type<METHOD_t, 1>::type, fa1_t,
			typename trait::argument_type<METHOD_t, 2>::type, fa2_t,
			typename trait::argument_type<METHOD_t, 3>::type, fa3_t,
			typename trait::argument_type<METHOD_t, 4>::type, fa4_t,
			typename trait::argument_type<METHOD_t, 5>::type, fa5_t,
			typename trait::argument_type<METHOD_t, 6>::type, fa6_t,
			typename trait::argument_type<METHOD_t, 7>::type, fa7_t,
			typename trait::argument_type<METHOD_t, 8>::type, fa8_t,
			typename trait::argument_type<METHOD_t, 9>::type, fa9_t,
			typename trait::argument_type<METHOD_t, 10>::type, fa10_t
		> sorted_real_args;
		typedef typename sorted_real_args::a0_t arg0_t;
		typedef HCallInterface<free_standing_args_count, typename trait::return_type<METHOD_t>::type,
						arg0_t, typename sorted_real_args::a1_t, typename sorted_real_args::a2_t, typename sorted_real_args::a3_t,
						typename sorted_real_args::a4_t, typename sorted_real_args::a5_t, typename sorted_real_args::a6_t, typename sorted_real_args::a7_t,
						typename sorted_real_args::a8_t, typename sorted_real_args::a9_t, typename sorted_real_args::a10_t> interface_type;
		typedef typename trait::decay<fa0_t>::type v0_t;
		typedef typename trait::decay<fa1_t>::type v1_t;
		typedef typename trait::decay<fa2_t>::type v2_t;
		typedef typename trait::decay<fa3_t>::type v3_t;
		typedef typename trait::decay<fa4_t>::type v4_t;
		typedef typename trait::decay<fa5_t>::type v5_t;
		typedef typename trait::decay<fa6_t>::type v6_t;
		typedef typename trait::decay<fa7_t>::type v7_t;
		typedef typename trait::decay<fa8_t>::type v8_t;
		typedef typename trait::decay<fa9_t>::type v9_t;
		typedef typename trait::decay<fa10_t>::type v10_t;
		inline static type make( METHOD_t&& m, fa0_t&& fa0, fa1_t&& fa1, fa2_t&& fa2, fa3_t&& fa3, fa4_t&& fa4, fa5_t&& fa5, fa6_t&& fa6, fa7_t&& fa7, fa8_t&& fa8, fa9_t&& fa9 ) {
			return ( type( yaal::forward<METHOD_t>( m ), yaal::forward<fa0_t>( fa0 ),
						yaal::forward<fa1_t>( fa1 ), yaal::forward<fa2_t>( fa2 ), yaal::forward<fa3_t>( fa3 ), yaal::forward<fa4_t>( fa4 ),
						yaal::forward<fa5_t>( fa5 ), yaal::forward<fa6_t>( fa6 ), yaal::forward<fa7_t>( fa7 ), yaal::forward<fa8_t>( fa8 ),
						yaal::forward<fa9_t>( fa9 ) ) );
		}
		inline static type make( METHOD_t&& m, fa0_t&& fa0, fa1_t&& fa1, fa2_t&& fa2, fa3_t&& fa3, fa4_t&& fa4, fa5_t&& fa5, fa6_t&& fa6, fa7_t&& fa7, fa8_t&& fa8 ) {
			return ( type( yaal::forward<METHOD_t>( m ), yaal::forward<fa0_t>( fa0 ),
						yaal::forward<fa1_t>( fa1 ), yaal::forward<fa2_t>( fa2 ), yaal::forward<fa3_t>( fa3 ), yaal::forward<fa4_t>( fa4 ),
						yaal::forward<fa5_t>( fa5 ), yaal::forward<fa6_t>( fa6 ), yaal::forward<fa7_t>( fa7 ), yaal::forward<fa8_t>( fa8 ) ) );
		}
		inline static type make( METHOD_t&& m, fa0_t&& fa0, fa1_t&& fa1, fa2_t&& fa2, fa3_t&& fa3, fa4_t&& fa4, fa5_t&& fa5, fa6_t&& fa6, fa7_t&& fa7 ) {
			return ( type( yaal::forward<METHOD_t>( m ), yaal::forward<fa0_t>( fa0 ),
						yaal::forward<fa1_t>( fa1 ), yaal::forward<fa2_t>( fa2 ), yaal::forward<fa3_t>( fa3 ), yaal::forward<fa4_t>( fa4 ),
						yaal::forward<fa5_t>( fa5 ), yaal::forward<fa6_t>( fa6 ), yaal::forward<fa7_t>( fa7 ) ) );
		}
		inline static type make( METHOD_t&& m, fa0_t&& fa0, fa1_t&& fa1, fa2_t&& fa2, fa3_t&& fa3, fa4_t&& fa4, fa5_t&& fa5, fa6_t&& fa6 ) {
			return ( type( yaal::forward<METHOD_t>( m ), yaal::forward<fa0_t>( fa0 ),
						yaal::forward<fa1_t>( fa1 ), yaal::forward<fa2_t>( fa2 ), yaal::forward<fa3_t>( fa3 ), yaal::forward<fa4_t>( fa4 ),
						yaal::forward<fa5_t>( fa5 ), yaal::forward<fa6_t>( fa6 ) ) );
		}
		inline static type make( METHOD_t&& m, fa0_t&& fa0, fa1_t&& fa1, fa2_t&& fa2, fa3_t&& fa3, fa4_t&& fa4, fa5_t&& fa5 ) {
			return ( type( yaal::forward<METHOD_t>( m ), yaal::forward<fa0_t>( fa0 ),
						yaal::forward<fa1_t>( fa1 ), yaal::forward<fa2_t>( fa2 ), yaal::forward<fa3_t>( fa3 ), yaal::forward<fa4_t>( fa4 ),
						yaal::forward<fa5_t>( fa5 ) ) );
		}
		inline static type make( METHOD_t&& m, fa0_t&& fa0, fa1_t&& fa1, fa2_t&& fa2, fa3_t&& fa3, fa4_t&& fa4 ) {
			return ( type( yaal::forward<METHOD_t>( m ), yaal::forward<fa0_t>( fa0 ),
						yaal::forward<fa1_t>( fa1 ), yaal::forward<fa2_t>( fa2 ), yaal::forward<fa3_t>( fa3 ), yaal::forward<fa4_t>( fa4 ) ) );
		}
		inline static type make( METHOD_t&& m, fa0_t&& fa0, fa1_t&& fa1, fa2_t&& fa2, fa3_t&& fa3 ) {
			return ( type( yaal::forward<METHOD_t>( m ), yaal::forward<fa0_t>( fa0 ),
						yaal::forward<fa1_t>( fa1 ), yaal::forward<fa2_t>( fa2 ), yaal::forward<fa3_t>( fa3 ) ) );
		}
		inline static type make( METHOD_t&& m, fa0_t&& fa0, fa1_t&& fa1, fa2_t&& fa2 ) {
			return ( type( yaal::forward<METHOD_t>( m ), yaal::forward<fa0_t>( fa0 ),
						yaal::forward<fa1_t>( fa1 ), yaal::forward<fa2_t>( fa2 ) ) );
		}
		inline static type make( METHOD_t&& m, fa0_t&& fa0, fa1_t&& fa1 ) {
			return ( type( yaal::forward<METHOD_t>( m ), yaal::forward<fa0_t>( fa0 ), yaal::forward<fa1_t>( fa1 ) ) );
		}
		inline static type make( METHOD_t&& m, fa0_t&& fa0 ) {
			return ( type( yaal::forward<METHOD_t>( m ), yaal::forward<fa0_t>( fa0 ) ) );
		}
		inline static type make( METHOD_t&& m ) {
			return ( type( yaal::forward<METHOD_t>( m ) ) );
		}
	};
	struct field {
		typedef HCall<-2, field, typename trait::field_type<METHOD_t>::type,
						fa0_t, METHOD_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t, fa9_t> type;
		inline static type make( METHOD_t&& m, fa0_t&& a0 ) {
			return ( type( yaal::forward<METHOD_t>( m ), yaal::forward<fa0_t>( a0 ) ) );
		}
	};
	struct field_this {
		typedef HCall<-1, field_this, typename trait::field_type<METHOD_t>::type,
						fa0_t, METHOD_t, fa1_t, fa2_t, fa3_t, fa4_t, fa5_t, fa6_t, fa7_t, fa8_t, fa9_t> type;
		inline static type make( METHOD_t&& m, fa0_t&& ) {
			return ( type( yaal::forward<METHOD_t>( m ) ) );
		}
	};
	typedef typename trait::ternary<
			trait::is_member<METHOD_t>::value, /* we have method */
			typename trait::ternary<
					meta::greater<
							trait::find_type<
									typename trait::strip<fa0_t>::type, /* is 'this' a free standing arg? */
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
					functor_this,
					functor
			>::type,
			typename trait::ternary<
					trait::is_field<METHOD_t>::value,
					typename trait::ternary<
							trait::same_type<typename trait::strip<fa0_t>::type, higher_order::placeholder<1> >::value,
							field_this,
							field
					>::type,
					function
			>::type
	>::type type;
};

/*! \brief Base class for HCall<1..10>.
 */
template<int free_args, typename descriptor>
class HCallBase : public descriptor::interface_type {
protected:
	typedef typename descriptor::call_type CALL_t;
	typename trait::decay<CALL_t>::type _call;
public:
	HCallBase( CALL_t&& call_ )
		: _call( yaal::forward<CALL_t>( call_ ) ) {}
protected:
	virtual void const* do_id( void ) const {
		return ( caller_id( _call ) );
	}
};

template<typename descriptor, typename return_t, typename class_t, typename field_t>
class HCall<-2, descriptor, return_t, class_t, field_t> {
public:
	typedef HCall<-2, descriptor, return_t, class_t, field_t> this_type;
private:
	typename trait::decay<field_t>::type _field;
	typename trait::decay<class_t>::type _object;
public:
	HCall( field_t&& field_, class_t&& object_ )
		: _field( yaal::forward<field_t>( field_ ) ), _object( yaal::forward<class_t>( object_ ) ) {
		return;
	}
	HCall( HCall const& ) = default;
	HCall& operator = ( HCall const& ) = default;
	return_t const& operator()( void ) const {
		return ( _object->*_field );
	}
};

template<typename descriptor, typename return_t, typename class_t, typename field_t>
class HCall<-1, descriptor, return_t, class_t, field_t> : public unary_function<return_t, typename trait::class_type<field_t>::type> {
public:
	typedef HCall<-1, descriptor, return_t, class_t, field_t> this_type;
	typedef unary_function<return_t, typename trait::class_type<field_t>::type> base_type;
	typedef typename base_type::result_type result_type;
	typedef typename base_type::argument_type argument_type;
private:
	typename trait::decay<field_t>::type _field;
public:
	HCall( field_t&& field_ )
		: _field( yaal::forward<field_t>( field_ ) ) {
		return;
	}
	HCall( HCall const& ) = default;
	HCall& operator = ( HCall const& ) = default;
	return_t const& operator()( argument_type const& obj_ ) const {
		return ( obj_.*_field );
	}
	return_t& operator()( argument_type& obj_ ) {
		return ( obj_.*_field );
	}
};

/*! \cond */

template<typename descriptor, typename return_t, typename CALL_t>
class HCall<0, descriptor, return_t, CALL_t>
		: public HCallBase<0, descriptor> {
public:
	typedef HCall<0, descriptor, return_t, CALL_t> this_type;
	typedef HCallBase<0, descriptor> base_type;
public:
	HCall( CALL_t&& call_ )
		: base_type( yaal::forward<CALL_t>( call_ ) ) {
		return;
	}
	HCall( HCall const& ) = default;
	HCall( HCall&& ) noexcept = default;
	HCall& operator = ( HCall const& ) = default;
	virtual return_t do_invoke( void ) {
		return ( (base_type::_call)() );
	}
	virtual return_t do_invoke( void ) const {
		return ( (base_type::_call)() );
	}
};

template<typename descriptor, typename return_t, typename CALL_t, typename a0_t>
class HCall<0, descriptor, return_t, CALL_t, a0_t>
		: public HCallBase<0, descriptor> {
public:
	typedef HCall<0, descriptor, return_t, CALL_t, a0_t> this_type;
	typedef HCallBase<0, descriptor> base_type;
private:
	typename descriptor::v0_t _a0;
public:
	HCall( CALL_t&& call_, a0_t&& a0 )
		: base_type( yaal::forward<CALL_t>( call_ ) ), _a0( yaal::forward<a0_t>( a0 ) ) {
		return;
	}
	HCall( HCall const& ) = default;
	HCall( HCall&& ) noexcept = default;
	HCall& operator = ( HCall const& ) = default;
	virtual return_t do_invoke( void ) {
		return ( (base_type::_call)( _a0 ) );
	}
	virtual return_t do_invoke( void ) const {
		return ( (base_type::_call)( _a0 ) );
	}
};

template<typename descriptor, typename return_t, typename CALL_t, typename a0_t, typename a1_t>
class HCall<0, descriptor, return_t, CALL_t, a0_t, a1_t>
		: public HCallBase<0, descriptor> {
public:
	typedef HCall<0, descriptor, return_t, CALL_t, a0_t, a1_t> this_type;
	typedef HCallBase<0, descriptor> base_type;
private:
	typename descriptor::v0_t _a0;
	typename descriptor::v1_t _a1;
public:
	HCall( CALL_t&& call_, a0_t&& a0, a1_t&& a1 )
		: base_type( yaal::forward<CALL_t>( call_ ) ), _a0( yaal::forward<a0_t>( a0 ) ), _a1( yaal::forward<a1_t>( a1 ) ) {
		return;
	}
	HCall( HCall const& ) = default;
	HCall( HCall&& ) noexcept = default;
	HCall& operator = ( HCall const& ) = default;
	virtual return_t do_invoke( void ) {
		return ( (base_type::_call)( _a0, _a1 ) );
	}
	virtual return_t do_invoke( void ) const {
		return ( (base_type::_call)( _a0, _a1 ) );
	}
};

template<typename descriptor, typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t>
class HCall<0, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t>
		: public HCallBase<0, descriptor> {
public:
	typedef HCall<0, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t> this_type;
	typedef HCallBase<0, descriptor> base_type;
private:
	typename descriptor::v0_t _a0;
	typename descriptor::v1_t _a1;
	typename descriptor::v2_t _a2;
public:
	HCall( CALL_t&& call_, a0_t&& a0, a1_t&& a1, a2_t&& a2 )
		: base_type( yaal::forward<CALL_t>( call_ ) ), _a0( yaal::forward<a0_t>( a0 ) ),
		_a1( yaal::forward<a1_t>( a1 ) ), _a2( yaal::forward<a2_t>( a2 ) ) {
		return;
	}
	HCall( HCall const& ) = default;
	HCall( HCall&& ) noexcept = default;
	HCall& operator = ( HCall const& ) = default;
	virtual return_t do_invoke( void ) {
		return ( (base_type::_call)( _a0, _a1, _a2 ) );
	}
	virtual return_t do_invoke( void ) const {
		return ( (base_type::_call)( _a0, _a1, _a2 ) );
	}
};

template<typename descriptor, typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t>
class HCall<0, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t>
		: public HCallBase<0, descriptor> {
public:
	typedef HCall<0, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t> this_type;
	typedef HCallBase<0, descriptor> base_type;
private:
	typename descriptor::v0_t _a0;
	typename descriptor::v1_t _a1;
	typename descriptor::v2_t _a2;
	typename descriptor::v3_t _a3;
public:
	HCall( CALL_t&& call_, a0_t&& a0, a1_t&& a1, a2_t&& a2, a3_t&& a3 )
		: base_type( yaal::forward<CALL_t>( call_ ) ), _a0( yaal::forward<a0_t>( a0 ) ),
		_a1( yaal::forward<a1_t>( a1 ) ), _a2( yaal::forward<a2_t>( a2 ) ), _a3( yaal::forward<a3_t>( a3 ) ) {
		return;
	}
	HCall( HCall const& ) = default;
	HCall( HCall&& ) noexcept = default;
	HCall& operator = ( HCall const& ) = default;
	virtual return_t do_invoke( void ) {
		return ( (base_type::_call)( _a0, _a1, _a2, _a3 ) );
	}
	virtual return_t do_invoke( void ) const {
		return ( (base_type::_call)( _a0, _a1, _a2, _a3 ) );
	}
};

template<typename descriptor, typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t>
class HCall<0, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t>
		: public HCallBase<0, descriptor> {
public:
	typedef HCall<0, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t> this_type;
	typedef HCallBase<0, descriptor> base_type;
private:
	typename descriptor::v0_t _a0;
	typename descriptor::v1_t _a1;
	typename descriptor::v2_t _a2;
	typename descriptor::v3_t _a3;
	typename descriptor::v4_t _a4;
public:
	HCall( CALL_t&& call_, a0_t&& a0, a1_t&& a1, a2_t&& a2, a3_t&& a3, a4_t&& a4 )
		: base_type( yaal::forward<CALL_t>( call_ ) ), _a0( yaal::forward<a0_t>( a0 ) ),
		_a1( yaal::forward<a1_t>( a1 ) ), _a2( yaal::forward<a2_t>( a2 ) ),
		_a3( yaal::forward<a3_t>( a3 ) ), _a4( yaal::forward<a4_t>( a4 ) ) {
		return;
	}
	HCall( HCall const& ) = default;
	HCall( HCall&& ) noexcept = default;
	HCall& operator = ( HCall const& ) = default;
	virtual return_t do_invoke( void ) {
		return ( (base_type::_call)( _a0, _a1, _a2, _a3, _a4 ) );
	}
	virtual return_t do_invoke( void ) const {
		return ( (base_type::_call)( _a0, _a1, _a2, _a3, _a4 ) );
	}
};

template<typename descriptor, typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t>
class HCall<0, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t>
		: public HCallBase<0, descriptor> {
public:
	typedef HCall<0, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t> this_type;
	typedef HCallBase<0, descriptor> base_type;
private:
	typename descriptor::v0_t _a0;
	typename descriptor::v1_t _a1;
	typename descriptor::v2_t _a2;
	typename descriptor::v3_t _a3;
	typename descriptor::v4_t _a4;
	typename descriptor::v5_t _a5;
public:
	HCall( CALL_t&& call_, a0_t&& a0, a1_t&& a1, a2_t&& a2, a3_t&& a3, a4_t&& a4, a5_t&& a5 )
		: base_type( yaal::forward<CALL_t>( call_ ) ), _a0( yaal::forward<a0_t>( a0 ) ),
		_a1( yaal::forward<a1_t>( a1 ) ), _a2( yaal::forward<a2_t>( a2 ) ),
		_a3( yaal::forward<a3_t>( a3 ) ), _a4( yaal::forward<a4_t>( a4 ) ), _a5( yaal::forward<a5_t>( a5 ) ) {
		return;
	}
	HCall( HCall const& ) = default;
	HCall( HCall&& ) noexcept = default;
	HCall& operator = ( HCall const& ) = default;
	virtual return_t do_invoke( void ) {
		return ( (base_type::_call)( _a0, _a1, _a2, _a3, _a4, _a5 ) );
	}
	virtual return_t do_invoke( void ) const {
		return ( (base_type::_call)( _a0, _a1, _a2, _a3, _a4, _a5 ) );
	}
};

template<typename descriptor, typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t, typename a6_t>
class HCall<0, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t>
		: public HCallBase<0, descriptor> {
public:
	typedef HCall<0, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t> this_type;
	typedef HCallBase<0, descriptor> base_type;
private:
	typename descriptor::v0_t _a0;
	typename descriptor::v1_t _a1;
	typename descriptor::v2_t _a2;
	typename descriptor::v3_t _a3;
	typename descriptor::v4_t _a4;
	typename descriptor::v5_t _a5;
	typename descriptor::v6_t _a6;
public:
	HCall( CALL_t&& call_, a0_t&& a0, a1_t&& a1,
			a2_t a2, a3_t&& a3, a4_t&& a4, a5_t&& a5, a6_t&& a6 )
		: base_type( yaal::forward<CALL_t>( call_ ) ), _a0( yaal::forward<a0_t>( a0 ) ),
		_a1( yaal::forward<a1_t>( a1 ) ), _a2( yaal::forward<a2_t>( a2 ) ),
		_a3( yaal::forward<a3_t>( a3 ) ), _a4( yaal::forward<a4_t>( a4 ) ),
		_a5( yaal::forward<a5_t>( a5 ) ), _a6( yaal::forward<a6_t>( a6 ) ) {
		return;
	}
	HCall( HCall const& ) = default;
	HCall( HCall&& ) noexcept = default;
	HCall& operator = ( HCall const& ) = default;
	virtual return_t do_invoke( void ) {
		return ( (base_type::_call)( _a0, _a1, _a2, _a3, _a4, _a5, _a6 ) );
	}
	virtual return_t do_invoke( void ) const {
		return ( (base_type::_call)( _a0, _a1, _a2, _a3, _a4, _a5, _a6 ) );
	}
};

template<typename descriptor, typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t, typename a6_t, typename a7_t>
class HCall<0, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t>
		: public HCallBase<0, descriptor> {
public:
	typedef HCall<0, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t> this_type;
	typedef HCallBase<0, descriptor> base_type;
private:
	typename descriptor::v0_t _a0;
	typename descriptor::v1_t _a1;
	typename descriptor::v2_t _a2;
	typename descriptor::v3_t _a3;
	typename descriptor::v4_t _a4;
	typename descriptor::v5_t _a5;
	typename descriptor::v6_t _a6;
	typename descriptor::v7_t _a7;
public:
	HCall( CALL_t&& call_, a0_t&& a0, a1_t&& a1,
			a2_t a2, a3_t&& a3, a4_t&& a4, a5_t&& a5, a6_t&& a6, a7_t&& a7 )
		: base_type( yaal::forward<CALL_t>( call_ ) ), _a0( yaal::forward<a0_t>( a0 ) ),
		_a1( yaal::forward<a1_t>( a1 ) ), _a2( yaal::forward<a2_t>( a2 ) ),
		_a3( yaal::forward<a3_t>( a3 ) ), _a4( yaal::forward<a4_t>( a4 ) ),
		_a5( yaal::forward<a5_t>( a5 ) ), _a6( yaal::forward<a6_t>( a6 ) ), _a7( yaal::forward<a7_t>( a7 ) ) {
		return;
	}
	HCall( HCall const& ) = default;
	HCall( HCall&& ) noexcept = default;
	HCall& operator = ( HCall const& ) = default;
	virtual return_t do_invoke( void ) {
		return ( (base_type::_call)( _a0, _a1, _a2, _a3, _a4, _a5, _a6, _a7 ) );
	}
	virtual return_t do_invoke( void ) const {
		return ( (base_type::_call)( _a0, _a1, _a2, _a3, _a4, _a5, _a6, _a7 ) );
	}
};

template<typename descriptor, typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t, typename a6_t, typename a7_t,
	typename a8_t>
class HCall<0, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t>
		: public HCallBase<0, descriptor> {
public:
	typedef HCall<0, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t> this_type;
	typedef HCallBase<0, descriptor> base_type;
private:
	typename descriptor::v0_t _a0;
	typename descriptor::v1_t _a1;
	typename descriptor::v2_t _a2;
	typename descriptor::v3_t _a3;
	typename descriptor::v4_t _a4;
	typename descriptor::v5_t _a5;
	typename descriptor::v6_t _a6;
	typename descriptor::v7_t _a7;
	typename descriptor::v8_t _a8;
public:
	HCall( CALL_t&& call_, a0_t&& a0, a1_t&& a1, a2_t&& a2, a3_t&& a3, a4_t&& a4, a5_t&& a5, a6_t&& a6, a7_t&& a7, a8_t&& a8 )
		: base_type( yaal::forward<CALL_t>( call_ ) ), _a0( yaal::forward<a0_t>( a0 ) ),
		_a1( yaal::forward<a1_t>( a1 ) ), _a2( yaal::forward<a2_t>( a2 ) ),
		_a3( yaal::forward<a3_t>( a3 ) ), _a4( yaal::forward<a4_t>( a4 ) ),
		_a5( yaal::forward<a5_t>( a5 ) ), _a6( yaal::forward<a6_t>( a6 ) ),
		_a7( yaal::forward<a7_t>( a7 ) ), _a8( yaal::forward<a8_t>( a8 ) ) {
		return;
	}
	HCall( HCall const& ) = default;
	HCall( HCall&& ) noexcept = default;
	HCall& operator = ( HCall const& ) = default;
	virtual return_t do_invoke( void ) {
		return ( (base_type::_call)( _a0, _a1, _a2, _a3, _a4, _a5, _a6, _a7, _a8 ) );
	}
	virtual return_t do_invoke( void ) const {
		return ( (base_type::_call)( _a0, _a1, _a2, _a3, _a4, _a5, _a6, _a7, _a8 ) );
	}
};

template<typename descriptor, typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t, typename a6_t, typename a7_t,
	typename a8_t, typename a9_t>
class HCall<0, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t>
		: public HCallBase<0, descriptor> {
public:
	typedef HCall<0, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t> this_type;
	typedef HCallBase<0, descriptor> base_type;
private:
	typename descriptor::v0_t _a0;
	typename descriptor::v1_t _a1;
	typename descriptor::v2_t _a2;
	typename descriptor::v3_t _a3;
	typename descriptor::v4_t _a4;
	typename descriptor::v5_t _a5;
	typename descriptor::v6_t _a6;
	typename descriptor::v7_t _a7;
	typename descriptor::v8_t _a8;
	typename descriptor::v9_t _a9;
public:
	HCall( CALL_t&& call_, a0_t&& a0, a1_t&& a1, a2_t&& a2, a3_t&& a3, a4_t&& a4, a5_t&& a5, a6_t&& a6, a7_t&& a7, a8_t&& a8, a9_t&& a9 )
		: base_type( yaal::forward<CALL_t>( call_ ) ), _a0( yaal::forward<a0_t>( a0 ) ),
		_a1( yaal::forward<a1_t>( a1 ) ), _a2( yaal::forward<a2_t>( a2 ) ),
		_a3( yaal::forward<a3_t>( a3 ) ), _a4( yaal::forward<a4_t>( a4 ) ),
		_a5( yaal::forward<a5_t>( a5 ) ), _a6( yaal::forward<a6_t>( a6 ) ),
		_a7( yaal::forward<a7_t>( a7 ) ), _a8( yaal::forward<a8_t>( a8 ) ), _a9( yaal::forward<a9_t>( a9 ) ) {
		return;
	}
	HCall( HCall const& ) = default;
	HCall( HCall&& ) noexcept = default;
	HCall& operator = ( HCall const& ) = default;
	virtual return_t do_invoke( void ) {
		return ( (base_type::_call)( _a0, _a1, _a2, _a3, _a4, _a5, _a6, _a7, _a8, _a9 ) );
	}
	virtual return_t do_invoke( void ) const {
		return ( (base_type::_call)( _a0, _a1, _a2, _a3, _a4, _a5, _a6, _a7, _a8, _a9 ) );
	}
};

template<typename descriptor, typename return_t, typename CALL_t, typename a0_t>
class HCall<1, descriptor, return_t, CALL_t, a0_t>
		: public HCallBase<1, descriptor> {
public:
	typedef HCall<1, descriptor, return_t, CALL_t, a0_t> this_type;
	typedef HCallBase<1, descriptor> base_type;
private:
	typedef typename descriptor::arg0_t fa0_t;
	typename descriptor::v0_t _a0;
public:
	HCall( CALL_t&& call_, a0_t&& a0 )
		: base_type( yaal::forward<CALL_t>( call_ ) ), _a0( yaal::forward<a0_t>( a0 ) ) {
		return;
	}
	HCall( HCall const& ) = default;
	HCall( HCall&& ) noexcept = default;
	HCall& operator = ( HCall const& ) = default;
	virtual return_t do_invoke( fa0_t a0 ) {
		return ( (base_type::_call)( free_standing_call_args::resolve_arg_get( _a0, a0 ) ) );
	}
	virtual return_t do_invoke( fa0_t a0 ) const {
		return ( (base_type::_call)( free_standing_call_args::resolve_arg_get( _a0, a0 ) ) );
	}
};

template<typename descriptor, typename return_t, typename CALL_t, typename a0_t, typename a1_t>
class HCall<1, descriptor, return_t, CALL_t, a0_t, a1_t>
		: public HCallBase<1, descriptor> {
public:
	typedef HCall<1, descriptor, return_t, CALL_t, a0_t, a1_t> this_type;
	typedef HCallBase<1, descriptor> base_type;
private:
	typedef typename descriptor::arg0_t fa0_t;
	typename descriptor::v0_t _a0;
	typename descriptor::v1_t _a1;
public:
	HCall( CALL_t&& call_, a0_t&& a0, a1_t&& a1 )
		: base_type( yaal::forward<CALL_t>( call_ ) ), _a0( yaal::forward<a0_t>( a0 ) ), _a1( yaal::forward<a1_t>( a1 ) ) {
		return;
	}
	HCall( HCall const& ) = default;
	HCall( HCall&& ) noexcept = default;
	HCall& operator = ( HCall const& ) = default;
	virtual return_t do_invoke( fa0_t a0 ) {
		return ( (base_type::_call)(
				free_standing_call_args::resolve_arg_get( _a0, a0 ),
				free_standing_call_args::resolve_arg_get( _a1, a0 ) ) ); }
	virtual return_t do_invoke( fa0_t a0 ) const { return ( (base_type::_call)(
				free_standing_call_args::resolve_arg_get( _a0, a0 ),
				free_standing_call_args::resolve_arg_get( _a1, a0 ) ) ); }
};

template<typename descriptor, typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t>
class HCall<1, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t>
		: public HCallBase<1, descriptor> {
public:
	typedef HCall<1, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t> this_type;
	typedef HCallBase<1, descriptor> base_type;
private:
	typedef typename descriptor::arg0_t fa0_t;
	typename descriptor::v0_t _a0;
	typename descriptor::v1_t _a1;
	typename descriptor::v2_t _a2;
public:
	HCall( CALL_t&& call_, a0_t&& a0, a1_t&& a1, a2_t&& a2 )
		: base_type( yaal::forward<CALL_t>( call_ ) ), _a0( yaal::forward<a0_t>( a0 ) ),
		_a1( yaal::forward<a1_t>( a1 ) ), _a2( yaal::forward<a2_t>( a2 ) ) {
		return;
	}
	HCall( HCall const& ) = default;
	HCall( HCall&& ) noexcept = default;
	HCall& operator = ( HCall const& ) = default;
	virtual return_t do_invoke( fa0_t a0 ) {
		return ( (base_type::_call)(
				free_standing_call_args::resolve_arg_get( _a0, a0 ),
				free_standing_call_args::resolve_arg_get( _a1, a0 ),
				free_standing_call_args::resolve_arg_get( _a2, a0 ) ) ); }
	virtual return_t do_invoke( fa0_t a0 ) const { return ( (base_type::_call)(
				free_standing_call_args::resolve_arg_get( _a0, a0 ),
				free_standing_call_args::resolve_arg_get( _a1, a0 ),
				free_standing_call_args::resolve_arg_get( _a2, a0 ) ) ); }
};

template<typename descriptor, typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t>
class HCall<1, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t>
		: public HCallBase<1, descriptor> {
public:
	typedef HCall<1, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t> this_type;
	typedef HCallBase<1, descriptor> base_type;
private:
	typedef typename descriptor::arg0_t fa0_t;
	typename descriptor::v0_t _a0;
	typename descriptor::v1_t _a1;
	typename descriptor::v2_t _a2;
	typename descriptor::v3_t _a3;
public:
	HCall( CALL_t&& call_, a0_t&& a0, a1_t&& a1, a2_t&& a2, a3_t&& a3 )
		: base_type( yaal::forward<CALL_t>( call_ ) ), _a0( yaal::forward<a0_t>( a0 ) ),
		_a1( yaal::forward<a1_t>( a1 ) ), _a2( yaal::forward<a2_t>( a2 ) ), _a3( yaal::forward<a3_t>( a3 ) ) {
		return;
	}
	HCall( HCall const& ) = default;
	HCall( HCall&& ) noexcept = default;
	HCall& operator = ( HCall const& ) = default;
	virtual return_t do_invoke( fa0_t a0 ) {
		return ( (base_type::_call)(
				free_standing_call_args::resolve_arg_get( _a0, a0 ),
				free_standing_call_args::resolve_arg_get( _a1, a0 ),
				free_standing_call_args::resolve_arg_get( _a2, a0 ),
				free_standing_call_args::resolve_arg_get( _a3, a0 ) ) ); }
	virtual return_t do_invoke( fa0_t a0 ) const { return ( (base_type::_call)(
				free_standing_call_args::resolve_arg_get( _a0, a0 ),
				free_standing_call_args::resolve_arg_get( _a1, a0 ),
				free_standing_call_args::resolve_arg_get( _a2, a0 ),
				free_standing_call_args::resolve_arg_get( _a3, a0 ) ) ); }
};

template<typename descriptor, typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t>
class HCall<1, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t>
		: public HCallBase<1, descriptor> {
public:
	typedef HCall<1, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t> this_type;
	typedef HCallBase<1, descriptor> base_type;
private:
	typedef typename descriptor::arg0_t fa0_t;
	typename descriptor::v0_t _a0;
	typename descriptor::v1_t _a1;
	typename descriptor::v2_t _a2;
	typename descriptor::v3_t _a3;
	typename descriptor::v4_t _a4;
public:
	HCall( CALL_t&& call_, a0_t&& a0, a1_t&& a1, a2_t&& a2, a3_t&& a3, a4_t&& a4 )
		: base_type( yaal::forward<CALL_t>( call_ ) ), _a0( yaal::forward<a0_t>( a0 ) ),
		_a1( yaal::forward<a1_t>( a1 ) ), _a2( yaal::forward<a2_t>( a2 ) ),
		_a3( yaal::forward<a3_t>( a3 ) ), _a4( yaal::forward<a4_t>( a4 ) ) {
		return;
	}
	HCall( HCall const& ) = default;
	HCall( HCall&& ) noexcept = default;
	HCall& operator = ( HCall const& ) = default;
	virtual return_t do_invoke( fa0_t a0 ) {
		return ( (base_type::_call)(
				free_standing_call_args::resolve_arg_get( _a0, a0 ),
				free_standing_call_args::resolve_arg_get( _a1, a0 ),
				free_standing_call_args::resolve_arg_get( _a2, a0 ),
				free_standing_call_args::resolve_arg_get( _a3, a0 ),
				free_standing_call_args::resolve_arg_get( _a4, a0 ) ) ); }
	virtual return_t do_invoke( fa0_t a0 ) const { return ( (base_type::_call)(
				free_standing_call_args::resolve_arg_get( _a0, a0 ),
				free_standing_call_args::resolve_arg_get( _a1, a0 ),
				free_standing_call_args::resolve_arg_get( _a2, a0 ),
				free_standing_call_args::resolve_arg_get( _a3, a0 ),
				free_standing_call_args::resolve_arg_get( _a4, a0 ) ) ); }
};

template<typename descriptor, typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t>
class HCall<1, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t>
		: public HCallBase<1, descriptor> {
public:
	typedef HCall<1, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t> this_type;
	typedef HCallBase<1, descriptor> base_type;
private:
	typedef typename descriptor::arg0_t fa0_t;
	typename descriptor::v0_t _a0;
	typename descriptor::v1_t _a1;
	typename descriptor::v2_t _a2;
	typename descriptor::v3_t _a3;
	typename descriptor::v4_t _a4;
	typename descriptor::v5_t _a5;
public:
	HCall( CALL_t&& call_, a0_t&& a0, a1_t&& a1, a2_t&& a2, a3_t&& a3, a4_t&& a4, a5_t&& a5 )
		: base_type( yaal::forward<CALL_t>( call_ ) ), _a0( yaal::forward<a0_t>( a0 ) ),
		_a1( yaal::forward<a1_t>( a1 ) ), _a2( yaal::forward<a2_t>( a2 ) ),
		_a3( yaal::forward<a3_t>( a3 ) ), _a4( yaal::forward<a4_t>( a4 ) ), _a5( yaal::forward<a5_t>( a5 ) ) {
		return;
	}
	HCall( HCall const& ) = default;
	HCall( HCall&& ) noexcept = default;
	HCall& operator = ( HCall const& ) = default;
	virtual return_t do_invoke( fa0_t a0 ) {
		return ( (base_type::_call)(
				free_standing_call_args::resolve_arg_get( _a0, a0 ),
				free_standing_call_args::resolve_arg_get( _a1, a0 ),
				free_standing_call_args::resolve_arg_get( _a2, a0 ),
				free_standing_call_args::resolve_arg_get( _a3, a0 ),
				free_standing_call_args::resolve_arg_get( _a4, a0 ),
				free_standing_call_args::resolve_arg_get( _a5, a0 ) ) ); }
	virtual return_t do_invoke( fa0_t a0 ) const { return ( (base_type::_call)(
				free_standing_call_args::resolve_arg_get( _a0, a0 ),
				free_standing_call_args::resolve_arg_get( _a1, a0 ),
				free_standing_call_args::resolve_arg_get( _a2, a0 ),
				free_standing_call_args::resolve_arg_get( _a3, a0 ),
				free_standing_call_args::resolve_arg_get( _a4, a0 ),
				free_standing_call_args::resolve_arg_get( _a5, a0 ) ) ); }
};

template<typename descriptor, typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t, typename a6_t>
class HCall<1, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t>
		: public HCallBase<1, descriptor> {
public:
	typedef HCall<1, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t> this_type;
	typedef HCallBase<1, descriptor> base_type;
private:
	typedef typename descriptor::arg0_t fa0_t;
	typename descriptor::v0_t _a0;
	typename descriptor::v1_t _a1;
	typename descriptor::v2_t _a2;
	typename descriptor::v3_t _a3;
	typename descriptor::v4_t _a4;
	typename descriptor::v5_t _a5;
	typename descriptor::v6_t _a6;
public:
	HCall( CALL_t&& call_, a0_t&& a0, a1_t&& a1,
			a2_t a2, a3_t&& a3, a4_t&& a4, a5_t&& a5, a6_t&& a6 )
		: base_type( yaal::forward<CALL_t>( call_ ) ), _a0( yaal::forward<a0_t>( a0 ) ),
		_a1( yaal::forward<a1_t>( a1 ) ), _a2( yaal::forward<a2_t>( a2 ) ),
		_a3( yaal::forward<a3_t>( a3 ) ), _a4( yaal::forward<a4_t>( a4 ) ),
		_a5( yaal::forward<a5_t>( a5 ) ), _a6( yaal::forward<a6_t>( a6 ) ) {
		return;
	}
	HCall( HCall const& ) = default;
	HCall( HCall&& ) noexcept = default;
	HCall& operator = ( HCall const& ) = default;
	virtual return_t do_invoke( fa0_t a0 ) {
		return ( (base_type::_call)(
				free_standing_call_args::resolve_arg_get( _a0, a0 ),
				free_standing_call_args::resolve_arg_get( _a1, a0 ),
				free_standing_call_args::resolve_arg_get( _a2, a0 ),
				free_standing_call_args::resolve_arg_get( _a3, a0 ),
				free_standing_call_args::resolve_arg_get( _a4, a0 ),
				free_standing_call_args::resolve_arg_get( _a5, a0 ),
				free_standing_call_args::resolve_arg_get( _a6, a0 ) ) ); }
	virtual return_t do_invoke( fa0_t a0 ) const { return ( (base_type::_call)(
				free_standing_call_args::resolve_arg_get( _a0, a0 ),
				free_standing_call_args::resolve_arg_get( _a1, a0 ),
				free_standing_call_args::resolve_arg_get( _a2, a0 ),
				free_standing_call_args::resolve_arg_get( _a3, a0 ),
				free_standing_call_args::resolve_arg_get( _a4, a0 ),
				free_standing_call_args::resolve_arg_get( _a5, a0 ),
				free_standing_call_args::resolve_arg_get( _a6, a0 ) ) ); }
};

template<typename descriptor, typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t, typename a6_t, typename a7_t>
class HCall<1, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t>
		: public HCallBase<1, descriptor> {
public:
	typedef HCall<1, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t> this_type;
	typedef HCallBase<1, descriptor> base_type;
private:
	typedef typename descriptor::arg0_t fa0_t;
	typename descriptor::v0_t _a0;
	typename descriptor::v1_t _a1;
	typename descriptor::v2_t _a2;
	typename descriptor::v3_t _a3;
	typename descriptor::v4_t _a4;
	typename descriptor::v5_t _a5;
	typename descriptor::v6_t _a6;
	typename descriptor::v7_t _a7;
public:
	HCall( CALL_t&& call_, a0_t&& a0, a1_t&& a1,
			a2_t a2, a3_t&& a3, a4_t&& a4, a5_t&& a5, a6_t&& a6, a7_t&& a7 )
		: base_type( yaal::forward<CALL_t>( call_ ) ), _a0( yaal::forward<a0_t>( a0 ) ),
		_a1( yaal::forward<a1_t>( a1 ) ), _a2( yaal::forward<a2_t>( a2 ) ),
		_a3( yaal::forward<a3_t>( a3 ) ), _a4( yaal::forward<a4_t>( a4 ) ),
		_a5( yaal::forward<a5_t>( a5 ) ), _a6( yaal::forward<a6_t>( a6 ) ), _a7( yaal::forward<a7_t>( a7 ) ) {
		return;
	}
	HCall( HCall const& ) = default;
	HCall( HCall&& ) noexcept = default;
	HCall& operator = ( HCall const& ) = default;
	virtual return_t do_invoke( fa0_t a0 ) {
		return ( (base_type::_call)(
				free_standing_call_args::resolve_arg_get( _a0, a0 ),
				free_standing_call_args::resolve_arg_get( _a1, a0 ),
				free_standing_call_args::resolve_arg_get( _a2, a0 ),
				free_standing_call_args::resolve_arg_get( _a3, a0 ),
				free_standing_call_args::resolve_arg_get( _a4, a0 ),
				free_standing_call_args::resolve_arg_get( _a5, a0 ),
				free_standing_call_args::resolve_arg_get( _a6, a0 ),
				free_standing_call_args::resolve_arg_get( _a7, a0 ) ) ); }
	virtual return_t do_invoke( fa0_t a0 ) const { return ( (base_type::_call)(
				free_standing_call_args::resolve_arg_get( _a0, a0 ),
				free_standing_call_args::resolve_arg_get( _a1, a0 ),
				free_standing_call_args::resolve_arg_get( _a2, a0 ),
				free_standing_call_args::resolve_arg_get( _a3, a0 ),
				free_standing_call_args::resolve_arg_get( _a4, a0 ),
				free_standing_call_args::resolve_arg_get( _a5, a0 ),
				free_standing_call_args::resolve_arg_get( _a6, a0 ),
				free_standing_call_args::resolve_arg_get( _a7, a0 ) ) ); }
};

template<typename descriptor, typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t, typename a6_t, typename a7_t,
	typename a8_t>
class HCall<1, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t>
		: public HCallBase<1, descriptor> {
public:
	typedef HCall<1, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t> this_type;
	typedef HCallBase<1, descriptor> base_type;
private:
	typedef typename descriptor::arg0_t fa0_t;
	typename descriptor::v0_t _a0;
	typename descriptor::v1_t _a1;
	typename descriptor::v2_t _a2;
	typename descriptor::v3_t _a3;
	typename descriptor::v4_t _a4;
	typename descriptor::v5_t _a5;
	typename descriptor::v6_t _a6;
	typename descriptor::v7_t _a7;
	typename descriptor::v8_t _a8;
public:
	HCall( CALL_t&& call_, a0_t&& a0, a1_t&& a1, a2_t&& a2, a3_t&& a3, a4_t&& a4, a5_t&& a5, a6_t&& a6, a7_t&& a7, a8_t&& a8 )
		: base_type( yaal::forward<CALL_t>( call_ ) ), _a0( yaal::forward<a0_t>( a0 ) ),
		_a1( yaal::forward<a1_t>( a1 ) ), _a2( yaal::forward<a2_t>( a2 ) ),
		_a3( yaal::forward<a3_t>( a3 ) ), _a4( yaal::forward<a4_t>( a4 ) ),
		_a5( yaal::forward<a5_t>( a5 ) ), _a6( yaal::forward<a6_t>( a6 ) ),
		_a7( yaal::forward<a7_t>( a7 ) ), _a8( yaal::forward<a8_t>( a8 ) ) {
		return;
	}
	HCall( HCall const& ) = default;
	HCall( HCall&& ) noexcept = default;
	HCall& operator = ( HCall const& ) = default;
	virtual return_t do_invoke( fa0_t a0 ) {
		return ( (base_type::_call)(
				free_standing_call_args::resolve_arg_get( _a0, a0 ),
				free_standing_call_args::resolve_arg_get( _a1, a0 ),
				free_standing_call_args::resolve_arg_get( _a2, a0 ),
				free_standing_call_args::resolve_arg_get( _a3, a0 ),
				free_standing_call_args::resolve_arg_get( _a4, a0 ),
				free_standing_call_args::resolve_arg_get( _a5, a0 ),
				free_standing_call_args::resolve_arg_get( _a6, a0 ),
				free_standing_call_args::resolve_arg_get( _a7, a0 ),
				free_standing_call_args::resolve_arg_get( _a8, a0 ) ) ); }
	virtual return_t do_invoke( fa0_t a0 ) const { return ( (base_type::_call)(
				free_standing_call_args::resolve_arg_get( _a0, a0 ),
				free_standing_call_args::resolve_arg_get( _a1, a0 ),
				free_standing_call_args::resolve_arg_get( _a2, a0 ),
				free_standing_call_args::resolve_arg_get( _a3, a0 ),
				free_standing_call_args::resolve_arg_get( _a4, a0 ),
				free_standing_call_args::resolve_arg_get( _a5, a0 ),
				free_standing_call_args::resolve_arg_get( _a6, a0 ),
				free_standing_call_args::resolve_arg_get( _a7, a0 ),
				free_standing_call_args::resolve_arg_get( _a8, a0 ) ) ); }
};

template<typename descriptor, typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t, typename a6_t, typename a7_t,
	typename a8_t, typename a9_t>
class HCall<1, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t>
		: public HCallBase<1, descriptor> {
public:
	typedef HCall<1, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t> this_type;
	typedef HCallBase<1, descriptor> base_type;
private:
	typedef typename descriptor::arg0_t fa0_t;
	typename descriptor::v0_t _a0;
	typename descriptor::v1_t _a1;
	typename descriptor::v2_t _a2;
	typename descriptor::v3_t _a3;
	typename descriptor::v4_t _a4;
	typename descriptor::v5_t _a5;
	typename descriptor::v6_t _a6;
	typename descriptor::v7_t _a7;
	typename descriptor::v8_t _a8;
	typename descriptor::v9_t _a9;
public:
	HCall( CALL_t&& call_, a0_t&& a0, a1_t&& a1, a2_t&& a2, a3_t&& a3, a4_t&& a4, a5_t&& a5, a6_t&& a6, a7_t&& a7, a8_t&& a8, a9_t&& a9 )
		: base_type( yaal::forward<CALL_t>( call_ ) ), _a0( yaal::forward<a0_t>( a0 ) ),
		_a1( yaal::forward<a1_t>( a1 ) ), _a2( yaal::forward<a2_t>( a2 ) ),
		_a3( yaal::forward<a3_t>( a3 ) ), _a4( yaal::forward<a4_t>( a4 ) ),
		_a5( yaal::forward<a5_t>( a5 ) ), _a6( yaal::forward<a6_t>( a6 ) ),
		_a7( yaal::forward<a7_t>( a7 ) ), _a8( yaal::forward<a8_t>( a8 ) ), _a9( yaal::forward<a9_t>( a9 ) ) {
		return;
	}
	HCall( HCall const& ) = default;
	HCall( HCall&& ) noexcept = default;
	HCall& operator = ( HCall const& ) = default;
	virtual return_t do_invoke( fa0_t a0 ) {
		return ( (base_type::_call)(
				free_standing_call_args::resolve_arg_get( _a0, a0 ),
				free_standing_call_args::resolve_arg_get( _a1, a0 ),
				free_standing_call_args::resolve_arg_get( _a2, a0 ),
				free_standing_call_args::resolve_arg_get( _a3, a0 ),
				free_standing_call_args::resolve_arg_get( _a4, a0 ),
				free_standing_call_args::resolve_arg_get( _a5, a0 ),
				free_standing_call_args::resolve_arg_get( _a6, a0 ),
				free_standing_call_args::resolve_arg_get( _a7, a0 ),
				free_standing_call_args::resolve_arg_get( _a8, a0 ),
				free_standing_call_args::resolve_arg_get( _a9, a0 ) ) ); }
	virtual return_t do_invoke( fa0_t a0 ) const { return ( (base_type::_call)(
				free_standing_call_args::resolve_arg_get( _a0, a0 ),
				free_standing_call_args::resolve_arg_get( _a1, a0 ),
				free_standing_call_args::resolve_arg_get( _a2, a0 ),
				free_standing_call_args::resolve_arg_get( _a3, a0 ),
				free_standing_call_args::resolve_arg_get( _a4, a0 ),
				free_standing_call_args::resolve_arg_get( _a5, a0 ),
				free_standing_call_args::resolve_arg_get( _a6, a0 ),
				free_standing_call_args::resolve_arg_get( _a7, a0 ),
				free_standing_call_args::resolve_arg_get( _a8, a0 ),
				free_standing_call_args::resolve_arg_get( _a9, a0 ) ) ); }
};

template<typename descriptor, typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t, typename a6_t, typename a7_t,
	typename a8_t, typename a9_t, typename a10_t>
class HCall<1, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t, a10_t>
		: public HCallBase<1, descriptor> {
public:
	typedef HCall<1, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t, a10_t> this_type;
	typedef HCallBase<1, descriptor> base_type;
private:
	typedef typename descriptor::arg0_t fa0_t;
	typename descriptor::v0_t _a0;
	typename descriptor::v1_t _a1;
	typename descriptor::v2_t _a2;
	typename descriptor::v3_t _a3;
	typename descriptor::v4_t _a4;
	typename descriptor::v5_t _a5;
	typename descriptor::v6_t _a6;
	typename descriptor::v7_t _a7;
	typename descriptor::v8_t _a8;
	typename descriptor::v9_t _a9;
	typename descriptor::v10_t _a10;
public:
	HCall( CALL_t&& call_, a0_t&& a0, a1_t&& a1, a2_t&& a2, a3_t&& a3, a4_t&& a4, a5_t&& a5, a6_t&& a6, a7_t&& a7, a8_t&& a8, a9_t&& a9, a10_t&& a10)
		: base_type( yaal::forward<CALL_t>( call_ ) ), _a0( yaal::forward<a0_t>( a0 ) ),
		_a1( yaal::forward<a1_t>( a1 ) ), _a2( yaal::forward<a2_t>( a2 ) ),
		_a3( yaal::forward<a3_t>( a3 ) ), _a4( yaal::forward<a4_t>( a4 ) ),
		_a5( yaal::forward<a5_t>( a5 ) ), _a6( yaal::forward<a6_t>( a6 ) ),
		_a7( yaal::forward<a7_t>( a7 ) ), _a8( yaal::forward<a8_t>( a8 ) ),
		_a9( yaal::forward<a9_t>( a9 ) ), _a10( yaal::forward<a10_t>( a10 ) ) {
		return;
	}
	HCall( HCall const& ) = default;
	HCall( HCall&& ) noexcept = default;
	HCall& operator = ( HCall const& ) = default;
	virtual return_t do_invoke( fa0_t a0 ) {
		return ( (base_type::_call)(
				free_standing_call_args::resolve_arg_get( _a0, a0 ),
				free_standing_call_args::resolve_arg_get( _a1, a0 ),
				free_standing_call_args::resolve_arg_get( _a2, a0 ),
				free_standing_call_args::resolve_arg_get( _a3, a0 ),
				free_standing_call_args::resolve_arg_get( _a4, a0 ),
				free_standing_call_args::resolve_arg_get( _a5, a0 ),
				free_standing_call_args::resolve_arg_get( _a6, a0 ),
				free_standing_call_args::resolve_arg_get( _a7, a0 ),
				free_standing_call_args::resolve_arg_get( _a8, a0 ),
				free_standing_call_args::resolve_arg_get( _a9, a0 ),
				free_standing_call_args::resolve_arg_get( _a10, a0 ) ) ); }
	virtual return_t do_invoke( fa0_t a0 ) const { return ( (base_type::_call)(
				free_standing_call_args::resolve_arg_get( _a0, a0 ),
				free_standing_call_args::resolve_arg_get( _a1, a0 ),
				free_standing_call_args::resolve_arg_get( _a2, a0 ),
				free_standing_call_args::resolve_arg_get( _a3, a0 ),
				free_standing_call_args::resolve_arg_get( _a4, a0 ),
				free_standing_call_args::resolve_arg_get( _a5, a0 ),
				free_standing_call_args::resolve_arg_get( _a6, a0 ),
				free_standing_call_args::resolve_arg_get( _a7, a0 ),
				free_standing_call_args::resolve_arg_get( _a8, a0 ),
				free_standing_call_args::resolve_arg_get( _a9, a0 ),
				free_standing_call_args::resolve_arg_get( _a10, a0 ) ) ); }
};

template<typename descriptor, typename return_t, typename CALL_t, typename a0_t, typename a1_t>
class HCall<2, descriptor, return_t, CALL_t, a0_t, a1_t>
		: public HCallBase<2, descriptor> {
public:
	typedef HCall<2, descriptor, return_t, CALL_t, a0_t, a1_t> this_type;
	typedef HCallBase<2, descriptor> base_type;
private:
	typedef typename descriptor::arg0_t fa0_t;
	typedef typename descriptor::sorted_real_args::a1_t fa1_t;
	typename descriptor::v0_t _a0;
	typename descriptor::v1_t _a1;
public:
	HCall( CALL_t&& call_, a0_t&& a0, a1_t&& a1 )
		: base_type( yaal::forward<CALL_t>( call_ ) ), _a0( yaal::forward<a0_t>( a0 ) ), _a1( yaal::forward<a1_t>( a1 ) ) {
		return;
	}
	HCall( HCall const& ) = default;
	HCall( HCall&& ) noexcept = default;
	HCall& operator = ( HCall const& ) = default;
	virtual return_t do_invoke( fa0_t a0, fa1_t a1 ) {
		return ( (base_type::_call)(
				free_standing_call_args::resolve_arg_get( _a0, a0, a1 ),
				free_standing_call_args::resolve_arg_get( _a1, a0, a1 ) ) ); }
	virtual return_t do_invoke( fa0_t a0, fa1_t a1 ) const { return ( (base_type::_call)(
				free_standing_call_args::resolve_arg_get( _a0, a0, a1 ),
				free_standing_call_args::resolve_arg_get( _a1, a0, a1 ) ) ); }
};

template<typename descriptor, typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t>
class HCall<2, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t>
		: public HCallBase<2, descriptor> {
public:
	typedef HCall<2, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t> this_type;
	typedef HCallBase<2, descriptor> base_type;
private:
	typedef typename descriptor::arg0_t fa0_t;
	typedef typename descriptor::sorted_real_args::a1_t fa1_t;
	typename descriptor::v0_t _a0;
	typename descriptor::v1_t _a1;
	typename descriptor::v2_t _a2;
public:
	HCall( CALL_t&& call_, a0_t&& a0, a1_t&& a1, a2_t&& a2 )
		: base_type( yaal::forward<CALL_t>( call_ ) ), _a0( yaal::forward<a0_t>( a0 ) ),
		_a1( yaal::forward<a1_t>( a1 ) ), _a2( yaal::forward<a2_t>( a2 ) ) {
		return;
	}
	HCall( HCall const& ) = default;
	HCall( HCall&& ) noexcept = default;
	HCall& operator = ( HCall const& ) = default;
	virtual return_t do_invoke( fa0_t a0, fa1_t a1 ) {
		return ( (base_type::_call)(
				free_standing_call_args::resolve_arg_get( _a0, a0, a1 ),
				free_standing_call_args::resolve_arg_get( _a1, a0, a1 ),
				free_standing_call_args::resolve_arg_get( _a2, a0, a1 ) ) ); }
	virtual return_t do_invoke( fa0_t a0, fa1_t a1 ) const { return ( (base_type::_call)(
				free_standing_call_args::resolve_arg_get( _a0, a0, a1 ),
				free_standing_call_args::resolve_arg_get( _a1, a0, a1 ),
				free_standing_call_args::resolve_arg_get( _a2, a0, a1 ) ) ); }
};

template<typename descriptor, typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t>
class HCall<2, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t>
		: public HCallBase<2, descriptor> {
public:
	typedef HCall<2, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t> this_type;
	typedef HCallBase<2, descriptor> base_type;
private:
	typedef typename descriptor::arg0_t fa0_t;
	typedef typename descriptor::sorted_real_args::a1_t fa1_t;
	typename descriptor::v0_t _a0;
	typename descriptor::v1_t _a1;
	typename descriptor::v2_t _a2;
	typename descriptor::v3_t _a3;
public:
	HCall( CALL_t&& call_, a0_t&& a0, a1_t&& a1, a2_t&& a2, a3_t&& a3 )
		: base_type( yaal::forward<CALL_t>( call_ ) ), _a0( yaal::forward<a0_t>( a0 ) ),
		_a1( yaal::forward<a1_t>( a1 ) ), _a2( yaal::forward<a2_t>( a2 ) ), _a3( yaal::forward<a3_t>( a3 ) ) {
		return;
	}
	HCall( HCall const& ) = default;
	HCall( HCall&& ) noexcept = default;
	HCall& operator = ( HCall const& ) = default;
	virtual return_t do_invoke( fa0_t a0, fa1_t a1 ) {
		return ( (base_type::_call)(
				free_standing_call_args::resolve_arg_get( _a0, a0, a1 ),
				free_standing_call_args::resolve_arg_get( _a1, a0, a1 ),
				free_standing_call_args::resolve_arg_get( _a2, a0, a1 ),
				free_standing_call_args::resolve_arg_get( _a3, a0, a1 ) ) ); }
	virtual return_t do_invoke( fa0_t a0, fa1_t a1 ) const { return ( (base_type::_call)(
				free_standing_call_args::resolve_arg_get( _a0, a0, a1 ),
				free_standing_call_args::resolve_arg_get( _a1, a0, a1 ),
				free_standing_call_args::resolve_arg_get( _a2, a0, a1 ),
				free_standing_call_args::resolve_arg_get( _a3, a0, a1 ) ) ); }
};

template<typename descriptor, typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t>
class HCall<2, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t>
		: public HCallBase<2, descriptor> {
public:
	typedef HCall<2, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t> this_type;
	typedef HCallBase<2, descriptor> base_type;
private:
	typedef typename descriptor::arg0_t fa0_t;
	typedef typename descriptor::sorted_real_args::a1_t fa1_t;
	typename descriptor::v0_t _a0;
	typename descriptor::v1_t _a1;
	typename descriptor::v2_t _a2;
	typename descriptor::v3_t _a3;
	typename descriptor::v4_t _a4;
public:
	HCall( CALL_t&& call_, a0_t&& a0, a1_t&& a1, a2_t&& a2, a3_t&& a3, a4_t&& a4 )
		: base_type( yaal::forward<CALL_t>( call_ ) ), _a0( yaal::forward<a0_t>( a0 ) ),
		_a1( yaal::forward<a1_t>( a1 ) ), _a2( yaal::forward<a2_t>( a2 ) ),
		_a3( yaal::forward<a3_t>( a3 ) ), _a4( yaal::forward<a4_t>( a4 ) ) {
		return;
	}
	HCall( HCall const& ) = default;
	HCall( HCall&& ) noexcept = default;
	HCall& operator = ( HCall const& ) = default;
	virtual return_t do_invoke( fa0_t a0, fa1_t a1 ) {
		return ( (base_type::_call)(
				free_standing_call_args::resolve_arg_get( _a0, a0, a1 ),
				free_standing_call_args::resolve_arg_get( _a1, a0, a1 ),
				free_standing_call_args::resolve_arg_get( _a2, a0, a1 ),
				free_standing_call_args::resolve_arg_get( _a3, a0, a1 ),
				free_standing_call_args::resolve_arg_get( _a4, a0, a1 ) ) ); }
	virtual return_t do_invoke( fa0_t a0, fa1_t a1 ) const { return ( (base_type::_call)(
				free_standing_call_args::resolve_arg_get( _a0, a0, a1 ),
				free_standing_call_args::resolve_arg_get( _a1, a0, a1 ),
				free_standing_call_args::resolve_arg_get( _a2, a0, a1 ),
				free_standing_call_args::resolve_arg_get( _a3, a0, a1 ),
				free_standing_call_args::resolve_arg_get( _a4, a0, a1 ) ) ); }
};

template<typename descriptor, typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t>
class HCall<2, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t>
		: public HCallBase<2, descriptor> {
public:
	typedef HCall<2, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t> this_type;
	typedef HCallBase<2, descriptor> base_type;
private:
	typedef typename descriptor::arg0_t fa0_t;
	typedef typename descriptor::sorted_real_args::a1_t fa1_t;
	typename descriptor::v0_t _a0;
	typename descriptor::v1_t _a1;
	typename descriptor::v2_t _a2;
	typename descriptor::v3_t _a3;
	typename descriptor::v4_t _a4;
	typename descriptor::v5_t _a5;
public:
	HCall( CALL_t&& call_, a0_t&& a0, a1_t&& a1, a2_t&& a2, a3_t&& a3, a4_t&& a4, a5_t&& a5 )
		: base_type( yaal::forward<CALL_t>( call_ ) ), _a0( yaal::forward<a0_t>( a0 ) ),
		_a1( yaal::forward<a1_t>( a1 ) ), _a2( yaal::forward<a2_t>( a2 ) ),
		_a3( yaal::forward<a3_t>( a3 ) ), _a4( yaal::forward<a4_t>( a4 ) ), _a5( yaal::forward<a5_t>( a5 ) ) {
		return;
	}
	HCall( HCall const& ) = default;
	HCall( HCall&& ) noexcept = default;
	HCall& operator = ( HCall const& ) = default;
	virtual return_t do_invoke( fa0_t a0, fa1_t a1 ) {
		return ( (base_type::_call)(
				free_standing_call_args::resolve_arg_get( _a0, a0, a1 ),
				free_standing_call_args::resolve_arg_get( _a1, a0, a1 ),
				free_standing_call_args::resolve_arg_get( _a2, a0, a1 ),
				free_standing_call_args::resolve_arg_get( _a3, a0, a1 ),
				free_standing_call_args::resolve_arg_get( _a4, a0, a1 ),
				free_standing_call_args::resolve_arg_get( _a5, a0, a1 ) ) ); }
	virtual return_t do_invoke( fa0_t a0, fa1_t a1 ) const { return ( (base_type::_call)(
				free_standing_call_args::resolve_arg_get( _a0, a0, a1 ),
				free_standing_call_args::resolve_arg_get( _a1, a0, a1 ),
				free_standing_call_args::resolve_arg_get( _a2, a0, a1 ),
				free_standing_call_args::resolve_arg_get( _a3, a0, a1 ),
				free_standing_call_args::resolve_arg_get( _a4, a0, a1 ),
				free_standing_call_args::resolve_arg_get( _a5, a0, a1 ) ) ); }
};

template<typename descriptor, typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t, typename a6_t>
class HCall<2, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t>
		: public HCallBase<2, descriptor> {
public:
	typedef HCall<2, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t> this_type;
	typedef HCallBase<2, descriptor> base_type;
private:
	typedef typename descriptor::arg0_t fa0_t;
	typedef typename descriptor::sorted_real_args::a1_t fa1_t;
	typename descriptor::v0_t _a0;
	typename descriptor::v1_t _a1;
	typename descriptor::v2_t _a2;
	typename descriptor::v3_t _a3;
	typename descriptor::v4_t _a4;
	typename descriptor::v5_t _a5;
	typename descriptor::v6_t _a6;
public:
	HCall( CALL_t&& call_, a0_t&& a0, a1_t&& a1,
			a2_t a2, a3_t&& a3, a4_t&& a4, a5_t&& a5, a6_t&& a6 )
		: base_type( yaal::forward<CALL_t>( call_ ) ), _a0( yaal::forward<a0_t>( a0 ) ),
		_a1( yaal::forward<a1_t>( a1 ) ), _a2( yaal::forward<a2_t>( a2 ) ),
		_a3( yaal::forward<a3_t>( a3 ) ), _a4( yaal::forward<a4_t>( a4 ) ),
		_a5( yaal::forward<a5_t>( a5 ) ), _a6( yaal::forward<a6_t>( a6 ) ) {
		return;
	}
	HCall( HCall const& ) = default;
	HCall( HCall&& ) noexcept = default;
	HCall& operator = ( HCall const& ) = default;
	virtual return_t do_invoke( fa0_t a0, fa1_t a1 ) {
		return ( (base_type::_call)(
				free_standing_call_args::resolve_arg_get( _a0, a0, a1 ),
				free_standing_call_args::resolve_arg_get( _a1, a0, a1 ),
				free_standing_call_args::resolve_arg_get( _a2, a0, a1 ),
				free_standing_call_args::resolve_arg_get( _a3, a0, a1 ),
				free_standing_call_args::resolve_arg_get( _a4, a0, a1 ),
				free_standing_call_args::resolve_arg_get( _a5, a0, a1 ),
				free_standing_call_args::resolve_arg_get( _a6, a0, a1 ) ) ); }
	virtual return_t do_invoke( fa0_t a0, fa1_t a1 ) const { return ( (base_type::_call)(
				free_standing_call_args::resolve_arg_get( _a0, a0, a1 ),
				free_standing_call_args::resolve_arg_get( _a1, a0, a1 ),
				free_standing_call_args::resolve_arg_get( _a2, a0, a1 ),
				free_standing_call_args::resolve_arg_get( _a3, a0, a1 ),
				free_standing_call_args::resolve_arg_get( _a4, a0, a1 ),
				free_standing_call_args::resolve_arg_get( _a5, a0, a1 ),
				free_standing_call_args::resolve_arg_get( _a6, a0, a1 ) ) ); }
};

template<typename descriptor, typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t, typename a6_t, typename a7_t>
class HCall<2, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t>
		: public HCallBase<2, descriptor> {
public:
	typedef HCall<2, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t> this_type;
	typedef HCallBase<2, descriptor> base_type;
private:
	typedef typename descriptor::arg0_t fa0_t;
	typedef typename descriptor::sorted_real_args::a1_t fa1_t;
	typename descriptor::v0_t _a0;
	typename descriptor::v1_t _a1;
	typename descriptor::v2_t _a2;
	typename descriptor::v3_t _a3;
	typename descriptor::v4_t _a4;
	typename descriptor::v5_t _a5;
	typename descriptor::v6_t _a6;
	typename descriptor::v7_t _a7;
public:
	HCall( CALL_t&& call_, a0_t&& a0, a1_t&& a1,
			a2_t a2, a3_t&& a3, a4_t&& a4, a5_t&& a5, a6_t&& a6, a7_t&& a7 )
		: base_type( yaal::forward<CALL_t>( call_ ) ), _a0( yaal::forward<a0_t>( a0 ) ),
		_a1( yaal::forward<a1_t>( a1 ) ), _a2( yaal::forward<a2_t>( a2 ) ),
		_a3( yaal::forward<a3_t>( a3 ) ), _a4( yaal::forward<a4_t>( a4 ) ),
		_a5( yaal::forward<a5_t>( a5 ) ), _a6( yaal::forward<a6_t>( a6 ) ), _a7( yaal::forward<a7_t>( a7 ) ) {
		return;
	}
	HCall( HCall const& ) = default;
	HCall( HCall&& ) noexcept = default;
	HCall& operator = ( HCall const& ) = default;
	virtual return_t do_invoke( fa0_t a0, fa1_t a1 ) {
		return ( (base_type::_call)(
				free_standing_call_args::resolve_arg_get( _a0, a0, a1 ),
				free_standing_call_args::resolve_arg_get( _a1, a0, a1 ),
				free_standing_call_args::resolve_arg_get( _a2, a0, a1 ),
				free_standing_call_args::resolve_arg_get( _a3, a0, a1 ),
				free_standing_call_args::resolve_arg_get( _a4, a0, a1 ),
				free_standing_call_args::resolve_arg_get( _a5, a0, a1 ),
				free_standing_call_args::resolve_arg_get( _a6, a0, a1 ),
				free_standing_call_args::resolve_arg_get( _a7, a0, a1 ) ) ); }
	virtual return_t do_invoke( fa0_t a0, fa1_t a1 ) const { return ( (base_type::_call)(
				free_standing_call_args::resolve_arg_get( _a0, a0, a1 ),
				free_standing_call_args::resolve_arg_get( _a1, a0, a1 ),
				free_standing_call_args::resolve_arg_get( _a2, a0, a1 ),
				free_standing_call_args::resolve_arg_get( _a3, a0, a1 ),
				free_standing_call_args::resolve_arg_get( _a4, a0, a1 ),
				free_standing_call_args::resolve_arg_get( _a5, a0, a1 ),
				free_standing_call_args::resolve_arg_get( _a6, a0, a1 ),
				free_standing_call_args::resolve_arg_get( _a7, a0, a1 ) ) ); }
};

template<typename descriptor, typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t, typename a6_t, typename a7_t,
	typename a8_t>
class HCall<2, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t>
		: public HCallBase<2, descriptor> {
public:
	typedef HCall<2, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t> this_type;
	typedef HCallBase<2, descriptor> base_type;
private:
	typedef typename descriptor::arg0_t fa0_t;
	typedef typename descriptor::sorted_real_args::a1_t fa1_t;
	typename descriptor::v0_t _a0;
	typename descriptor::v1_t _a1;
	typename descriptor::v2_t _a2;
	typename descriptor::v3_t _a3;
	typename descriptor::v4_t _a4;
	typename descriptor::v5_t _a5;
	typename descriptor::v6_t _a6;
	typename descriptor::v7_t _a7;
	typename descriptor::v8_t _a8;
public:
	HCall( CALL_t&& call_, a0_t&& a0, a1_t&& a1, a2_t&& a2, a3_t&& a3, a4_t&& a4, a5_t&& a5, a6_t&& a6, a7_t&& a7, a8_t&& a8 )
		: base_type( yaal::forward<CALL_t>( call_ ) ), _a0( yaal::forward<a0_t>( a0 ) ),
		_a1( yaal::forward<a1_t>( a1 ) ), _a2( yaal::forward<a2_t>( a2 ) ),
		_a3( yaal::forward<a3_t>( a3 ) ), _a4( yaal::forward<a4_t>( a4 ) ),
		_a5( yaal::forward<a5_t>( a5 ) ), _a6( yaal::forward<a6_t>( a6 ) ),
		_a7( yaal::forward<a7_t>( a7 ) ), _a8( yaal::forward<a8_t>( a8 ) ) {
		return;
	}
	HCall( HCall const& ) = default;
	HCall( HCall&& ) noexcept = default;
	HCall& operator = ( HCall const& ) = default;
	virtual return_t do_invoke( fa0_t a0, fa1_t a1 ) {
		return ( (base_type::_call)(
				free_standing_call_args::resolve_arg_get( _a0, a0, a1 ),
				free_standing_call_args::resolve_arg_get( _a1, a0, a1 ),
				free_standing_call_args::resolve_arg_get( _a2, a0, a1 ),
				free_standing_call_args::resolve_arg_get( _a3, a0, a1 ),
				free_standing_call_args::resolve_arg_get( _a4, a0, a1 ),
				free_standing_call_args::resolve_arg_get( _a5, a0, a1 ),
				free_standing_call_args::resolve_arg_get( _a6, a0, a1 ),
				free_standing_call_args::resolve_arg_get( _a7, a0, a1 ),
				free_standing_call_args::resolve_arg_get( _a8, a0, a1 ) ) ); }
	virtual return_t do_invoke( fa0_t a0, fa1_t a1 ) const { return ( (base_type::_call)(
				free_standing_call_args::resolve_arg_get( _a0, a0, a1 ),
				free_standing_call_args::resolve_arg_get( _a1, a0, a1 ),
				free_standing_call_args::resolve_arg_get( _a2, a0, a1 ),
				free_standing_call_args::resolve_arg_get( _a3, a0, a1 ),
				free_standing_call_args::resolve_arg_get( _a4, a0, a1 ),
				free_standing_call_args::resolve_arg_get( _a5, a0, a1 ),
				free_standing_call_args::resolve_arg_get( _a6, a0, a1 ),
				free_standing_call_args::resolve_arg_get( _a7, a0, a1 ),
				free_standing_call_args::resolve_arg_get( _a8, a0, a1 ) ) ); }
};

template<typename descriptor, typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t, typename a6_t, typename a7_t,
	typename a8_t, typename a9_t>
class HCall<2, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t>
		: public HCallBase<2, descriptor> {
public:
	typedef HCall<2, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t> this_type;
	typedef HCallBase<2, descriptor> base_type;
private:
	typedef typename descriptor::arg0_t fa0_t;
	typedef typename descriptor::sorted_real_args::a1_t fa1_t;
	typename descriptor::v0_t _a0;
	typename descriptor::v1_t _a1;
	typename descriptor::v2_t _a2;
	typename descriptor::v3_t _a3;
	typename descriptor::v4_t _a4;
	typename descriptor::v5_t _a5;
	typename descriptor::v6_t _a6;
	typename descriptor::v7_t _a7;
	typename descriptor::v8_t _a8;
	typename descriptor::v9_t _a9;
public:
	HCall( CALL_t&& call_, a0_t&& a0, a1_t&& a1, a2_t&& a2, a3_t&& a3, a4_t&& a4, a5_t&& a5, a6_t&& a6, a7_t&& a7, a8_t&& a8, a9_t&& a9 )
		: base_type( yaal::forward<CALL_t>( call_ ) ), _a0( yaal::forward<a0_t>( a0 ) ),
		_a1( yaal::forward<a1_t>( a1 ) ), _a2( yaal::forward<a2_t>( a2 ) ),
		_a3( yaal::forward<a3_t>( a3 ) ), _a4( yaal::forward<a4_t>( a4 ) ),
		_a5( yaal::forward<a5_t>( a5 ) ), _a6( yaal::forward<a6_t>( a6 ) ),
		_a7( yaal::forward<a7_t>( a7 ) ), _a8( yaal::forward<a8_t>( a8 ) ), _a9( yaal::forward<a9_t>( a9 ) ) {
		return;
	}
	HCall( HCall const& ) = default;
	HCall( HCall&& ) noexcept = default;
	HCall& operator = ( HCall const& ) = default;
	virtual return_t do_invoke( fa0_t a0, fa1_t a1 ) {
		return ( (base_type::_call)(
				free_standing_call_args::resolve_arg_get( _a0, a0, a1 ),
				free_standing_call_args::resolve_arg_get( _a1, a0, a1 ),
				free_standing_call_args::resolve_arg_get( _a2, a0, a1 ),
				free_standing_call_args::resolve_arg_get( _a3, a0, a1 ),
				free_standing_call_args::resolve_arg_get( _a4, a0, a1 ),
				free_standing_call_args::resolve_arg_get( _a5, a0, a1 ),
				free_standing_call_args::resolve_arg_get( _a6, a0, a1 ),
				free_standing_call_args::resolve_arg_get( _a7, a0, a1 ),
				free_standing_call_args::resolve_arg_get( _a8, a0, a1 ),
				free_standing_call_args::resolve_arg_get( _a9, a0, a1 ) ) ); }
	virtual return_t do_invoke( fa0_t a0, fa1_t a1 ) const { return ( (base_type::_call)(
				free_standing_call_args::resolve_arg_get( _a0, a0, a1 ),
				free_standing_call_args::resolve_arg_get( _a1, a0, a1 ),
				free_standing_call_args::resolve_arg_get( _a2, a0, a1 ),
				free_standing_call_args::resolve_arg_get( _a3, a0, a1 ),
				free_standing_call_args::resolve_arg_get( _a4, a0, a1 ),
				free_standing_call_args::resolve_arg_get( _a5, a0, a1 ),
				free_standing_call_args::resolve_arg_get( _a6, a0, a1 ),
				free_standing_call_args::resolve_arg_get( _a7, a0, a1 ),
				free_standing_call_args::resolve_arg_get( _a8, a0, a1 ),
				free_standing_call_args::resolve_arg_get( _a9, a0, a1 ) ) ); }
};

template<typename descriptor, typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t, typename a6_t, typename a7_t,
	typename a8_t, typename a9_t, typename a10_t>
class HCall<2, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t, a10_t>
		: public HCallBase<2, descriptor> {
public:
	typedef HCall<2, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t, a10_t> this_type;
	typedef HCallBase<2, descriptor> base_type;
private:
	typedef typename descriptor::arg0_t fa0_t;
	typedef typename descriptor::sorted_real_args::a1_t fa1_t;
	typename descriptor::v0_t _a0;
	typename descriptor::v1_t _a1;
	typename descriptor::v2_t _a2;
	typename descriptor::v3_t _a3;
	typename descriptor::v4_t _a4;
	typename descriptor::v5_t _a5;
	typename descriptor::v6_t _a6;
	typename descriptor::v7_t _a7;
	typename descriptor::v8_t _a8;
	typename descriptor::v9_t _a9;
	typename descriptor::v10_t _a10;
public:
	HCall( CALL_t&& call_, a0_t&& a0, a1_t&& a1, a2_t&& a2, a3_t&& a3, a4_t&& a4, a5_t&& a5, a6_t&& a6, a7_t&& a7, a8_t&& a8, a9_t&& a9, a10_t&& a10 )
		: base_type( yaal::forward<CALL_t>( call_ ) ), _a0( yaal::forward<a0_t>( a0 ) ),
		_a1( yaal::forward<a1_t>( a1 ) ), _a2( yaal::forward<a2_t>( a2 ) ),
		_a3( yaal::forward<a3_t>( a3 ) ), _a4( yaal::forward<a4_t>( a4 ) ),
		_a5( yaal::forward<a5_t>( a5 ) ), _a6( yaal::forward<a6_t>( a6 ) ),
		_a7( yaal::forward<a7_t>( a7 ) ), _a8( yaal::forward<a8_t>( a8 ) ),
		_a9( yaal::forward<a9_t>( a9 ) ), _a10( yaal::forward<a10_t>( a10 ) ) {
		return;
	}
	HCall( HCall const& ) = default;
	HCall( HCall&& ) noexcept = default;
	HCall& operator = ( HCall const& ) = default;
	virtual return_t do_invoke( fa0_t a0, fa1_t a1 ) {
		return ( (base_type::_call)(
				free_standing_call_args::resolve_arg_get( _a0, a0, a1 ),
				free_standing_call_args::resolve_arg_get( _a1, a0, a1 ),
				free_standing_call_args::resolve_arg_get( _a2, a0, a1 ),
				free_standing_call_args::resolve_arg_get( _a3, a0, a1 ),
				free_standing_call_args::resolve_arg_get( _a4, a0, a1 ),
				free_standing_call_args::resolve_arg_get( _a5, a0, a1 ),
				free_standing_call_args::resolve_arg_get( _a6, a0, a1 ),
				free_standing_call_args::resolve_arg_get( _a7, a0, a1 ),
				free_standing_call_args::resolve_arg_get( _a8, a0, a1 ),
				free_standing_call_args::resolve_arg_get( _a9, a0, a1 ),
				free_standing_call_args::resolve_arg_get( _a10, a0, a1 ) ) ); }
	virtual return_t do_invoke( fa0_t a0, fa1_t a1 ) const { return ( (base_type::_call)(
				free_standing_call_args::resolve_arg_get( _a0, a0, a1 ),
				free_standing_call_args::resolve_arg_get( _a1, a0, a1 ),
				free_standing_call_args::resolve_arg_get( _a2, a0, a1 ),
				free_standing_call_args::resolve_arg_get( _a3, a0, a1 ),
				free_standing_call_args::resolve_arg_get( _a4, a0, a1 ),
				free_standing_call_args::resolve_arg_get( _a5, a0, a1 ),
				free_standing_call_args::resolve_arg_get( _a6, a0, a1 ),
				free_standing_call_args::resolve_arg_get( _a7, a0, a1 ),
				free_standing_call_args::resolve_arg_get( _a8, a0, a1 ),
				free_standing_call_args::resolve_arg_get( _a9, a0, a1 ),
				free_standing_call_args::resolve_arg_get( _a10, a0, a1 ) ) ); }
};

template<typename descriptor, typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t>
class HCall<3, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t>
		: public HCallBase<3, descriptor> {
public:
	typedef HCall<3, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t> this_type;
	typedef HCallBase<3, descriptor> base_type;
private:
	typedef typename descriptor::arg0_t fa0_t;
	typedef typename descriptor::sorted_real_args::a1_t fa1_t;
	typedef typename descriptor::sorted_real_args::a2_t fa2_t;
	typename descriptor::v0_t _a0;
	typename descriptor::v1_t _a1;
	typename descriptor::v2_t _a2;
public:
	HCall( CALL_t&& call_, a0_t&& a0, a1_t&& a1, a2_t&& a2 )
		: base_type( yaal::forward<CALL_t>( call_ ) ), _a0( yaal::forward<a0_t>( a0 ) ),
		_a1( yaal::forward<a1_t>( a1 ) ), _a2( yaal::forward<a2_t>( a2 ) ) {
		return;
	}
	HCall( HCall const& ) = default;
	HCall( HCall&& ) noexcept = default;
	HCall& operator = ( HCall const& ) = default;
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2 ) {
		return ( (base_type::_call)(
				free_standing_call_args::resolve_arg_get( _a0, a0, a1, a2 ),
				free_standing_call_args::resolve_arg_get( _a1, a0, a1, a2 ),
				free_standing_call_args::resolve_arg_get( _a2, a0, a1, a2 ) ) ); }
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2 ) const { return ( (base_type::_call)(
				free_standing_call_args::resolve_arg_get( _a0, a0, a1, a2 ),
				free_standing_call_args::resolve_arg_get( _a1, a0, a1, a2 ),
				free_standing_call_args::resolve_arg_get( _a2, a0, a1, a2 ) ) ); }
};

template<typename descriptor, typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t>
class HCall<3, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t>
		: public HCallBase<3, descriptor> {
public:
	typedef HCall<3, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t> this_type;
	typedef HCallBase<3, descriptor> base_type;
private:
	typedef typename descriptor::arg0_t fa0_t;
	typedef typename descriptor::sorted_real_args::a1_t fa1_t;
	typedef typename descriptor::sorted_real_args::a2_t fa2_t;
	typename descriptor::v0_t _a0;
	typename descriptor::v1_t _a1;
	typename descriptor::v2_t _a2;
	typename descriptor::v3_t _a3;
public:
	HCall( CALL_t&& call_, a0_t&& a0, a1_t&& a1, a2_t&& a2, a3_t&& a3 )
		: base_type( yaal::forward<CALL_t>( call_ ) ), _a0( yaal::forward<a0_t>( a0 ) ),
		_a1( yaal::forward<a1_t>( a1 ) ), _a2( yaal::forward<a2_t>( a2 ) ), _a3( yaal::forward<a3_t>( a3 ) ) {
		return;
	}
	HCall( HCall const& ) = default;
	HCall( HCall&& ) noexcept = default;
	HCall& operator = ( HCall const& ) = default;
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2 ) {
		return ( (base_type::_call)(
				free_standing_call_args::resolve_arg_get( _a0, a0, a1, a2 ),
				free_standing_call_args::resolve_arg_get( _a1, a0, a1, a2 ),
				free_standing_call_args::resolve_arg_get( _a2, a0, a1, a2 ),
				free_standing_call_args::resolve_arg_get( _a3, a0, a1, a2 ) ) ); }
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2 ) const { return ( (base_type::_call)(
				free_standing_call_args::resolve_arg_get( _a0, a0, a1, a2 ),
				free_standing_call_args::resolve_arg_get( _a1, a0, a1, a2 ),
				free_standing_call_args::resolve_arg_get( _a2, a0, a1, a2 ),
				free_standing_call_args::resolve_arg_get( _a3, a0, a1, a2 ) ) ); }
};

template<typename descriptor, typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t>
class HCall<3, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t>
		: public HCallBase<3, descriptor> {
public:
	typedef HCall<3, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t> this_type;
	typedef HCallBase<3, descriptor> base_type;
private:
	typedef typename descriptor::arg0_t fa0_t;
	typedef typename descriptor::sorted_real_args::a1_t fa1_t;
	typedef typename descriptor::sorted_real_args::a2_t fa2_t;
	typename descriptor::v0_t _a0;
	typename descriptor::v1_t _a1;
	typename descriptor::v2_t _a2;
	typename descriptor::v3_t _a3;
	typename descriptor::v4_t _a4;
public:
	HCall( CALL_t&& call_, a0_t&& a0, a1_t&& a1, a2_t&& a2, a3_t&& a3, a4_t&& a4 )
		: base_type( yaal::forward<CALL_t>( call_ ) ), _a0( yaal::forward<a0_t>( a0 ) ),
		_a1( yaal::forward<a1_t>( a1 ) ), _a2( yaal::forward<a2_t>( a2 ) ),
		_a3( yaal::forward<a3_t>( a3 ) ), _a4( yaal::forward<a4_t>( a4 ) ) {
		return;
	}
	HCall( HCall const& ) = default;
	HCall( HCall&& ) noexcept = default;
	HCall& operator = ( HCall const& ) = default;
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2 ) {
		return ( (base_type::_call)(
				free_standing_call_args::resolve_arg_get( _a0, a0, a1, a2 ),
				free_standing_call_args::resolve_arg_get( _a1, a0, a1, a2 ),
				free_standing_call_args::resolve_arg_get( _a2, a0, a1, a2 ),
				free_standing_call_args::resolve_arg_get( _a3, a0, a1, a2 ),
				free_standing_call_args::resolve_arg_get( _a4, a0, a1, a2 ) ) ); }
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2 ) const { return ( (base_type::_call)(
				free_standing_call_args::resolve_arg_get( _a0, a0, a1, a2 ),
				free_standing_call_args::resolve_arg_get( _a1, a0, a1, a2 ),
				free_standing_call_args::resolve_arg_get( _a2, a0, a1, a2 ),
				free_standing_call_args::resolve_arg_get( _a3, a0, a1, a2 ),
				free_standing_call_args::resolve_arg_get( _a4, a0, a1, a2 ) ) ); }
};

template<typename descriptor, typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t>
class HCall<3, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t>
		: public HCallBase<3, descriptor> {
public:
	typedef HCall<3, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t> this_type;
	typedef HCallBase<3, descriptor> base_type;
private:
	typedef typename descriptor::arg0_t fa0_t;
	typedef typename descriptor::sorted_real_args::a1_t fa1_t;
	typedef typename descriptor::sorted_real_args::a2_t fa2_t;
	typename descriptor::v0_t _a0;
	typename descriptor::v1_t _a1;
	typename descriptor::v2_t _a2;
	typename descriptor::v3_t _a3;
	typename descriptor::v4_t _a4;
	typename descriptor::v5_t _a5;
public:
	HCall( CALL_t&& call_, a0_t&& a0, a1_t&& a1, a2_t&& a2, a3_t&& a3, a4_t&& a4, a5_t&& a5 )
		: base_type( yaal::forward<CALL_t>( call_ ) ), _a0( yaal::forward<a0_t>( a0 ) ),
		_a1( yaal::forward<a1_t>( a1 ) ), _a2( yaal::forward<a2_t>( a2 ) ),
		_a3( yaal::forward<a3_t>( a3 ) ), _a4( yaal::forward<a4_t>( a4 ) ), _a5( yaal::forward<a5_t>( a5 ) ) {
		return;
	}
	HCall( HCall const& ) = default;
	HCall( HCall&& ) noexcept = default;
	HCall& operator = ( HCall const& ) = default;
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2 ) {
		return ( (base_type::_call)(
				free_standing_call_args::resolve_arg_get( _a0, a0, a1, a2 ),
				free_standing_call_args::resolve_arg_get( _a1, a0, a1, a2 ),
				free_standing_call_args::resolve_arg_get( _a2, a0, a1, a2 ),
				free_standing_call_args::resolve_arg_get( _a3, a0, a1, a2 ),
				free_standing_call_args::resolve_arg_get( _a4, a0, a1, a2 ),
				free_standing_call_args::resolve_arg_get( _a5, a0, a1, a2 ) ) ); }
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2 ) const { return ( (base_type::_call)(
				free_standing_call_args::resolve_arg_get( _a0, a0, a1, a2 ),
				free_standing_call_args::resolve_arg_get( _a1, a0, a1, a2 ),
				free_standing_call_args::resolve_arg_get( _a2, a0, a1, a2 ),
				free_standing_call_args::resolve_arg_get( _a3, a0, a1, a2 ),
				free_standing_call_args::resolve_arg_get( _a4, a0, a1, a2 ),
				free_standing_call_args::resolve_arg_get( _a5, a0, a1, a2 ) ) ); }
};

template<typename descriptor, typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t, typename a6_t>
class HCall<3, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t>
		: public HCallBase<3, descriptor> {
public:
	typedef HCall<3, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t> this_type;
	typedef HCallBase<3, descriptor> base_type;
private:
	typedef typename descriptor::arg0_t fa0_t;
	typedef typename descriptor::sorted_real_args::a1_t fa1_t;
	typedef typename descriptor::sorted_real_args::a2_t fa2_t;
	typename descriptor::v0_t _a0;
	typename descriptor::v1_t _a1;
	typename descriptor::v2_t _a2;
	typename descriptor::v3_t _a3;
	typename descriptor::v4_t _a4;
	typename descriptor::v5_t _a5;
	typename descriptor::v6_t _a6;
public:
	HCall( CALL_t&& call_, a0_t&& a0, a1_t&& a1,
			a2_t a2, a3_t&& a3, a4_t&& a4, a5_t&& a5, a6_t&& a6 )
		: base_type( yaal::forward<CALL_t>( call_ ) ), _a0( yaal::forward<a0_t>( a0 ) ),
		_a1( yaal::forward<a1_t>( a1 ) ), _a2( yaal::forward<a2_t>( a2 ) ),
		_a3( yaal::forward<a3_t>( a3 ) ), _a4( yaal::forward<a4_t>( a4 ) ),
		_a5( yaal::forward<a5_t>( a5 ) ), _a6( yaal::forward<a6_t>( a6 ) ) {
		return;
	}
	HCall( HCall const& ) = default;
	HCall( HCall&& ) noexcept = default;
	HCall& operator = ( HCall const& ) = default;
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2 ) {
		return ( (base_type::_call)(
				free_standing_call_args::resolve_arg_get( _a0, a0, a1, a2 ),
				free_standing_call_args::resolve_arg_get( _a1, a0, a1, a2 ),
				free_standing_call_args::resolve_arg_get( _a2, a0, a1, a2 ),
				free_standing_call_args::resolve_arg_get( _a3, a0, a1, a2 ),
				free_standing_call_args::resolve_arg_get( _a4, a0, a1, a2 ),
				free_standing_call_args::resolve_arg_get( _a5, a0, a1, a2 ),
				free_standing_call_args::resolve_arg_get( _a6, a0, a1, a2 ) ) ); }
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2 ) const { return ( (base_type::_call)(
				free_standing_call_args::resolve_arg_get( _a0, a0, a1, a2 ),
				free_standing_call_args::resolve_arg_get( _a1, a0, a1, a2 ),
				free_standing_call_args::resolve_arg_get( _a2, a0, a1, a2 ),
				free_standing_call_args::resolve_arg_get( _a3, a0, a1, a2 ),
				free_standing_call_args::resolve_arg_get( _a4, a0, a1, a2 ),
				free_standing_call_args::resolve_arg_get( _a5, a0, a1, a2 ),
				free_standing_call_args::resolve_arg_get( _a6, a0, a1, a2 ) ) ); }
};

template<typename descriptor, typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t, typename a6_t, typename a7_t>
class HCall<3, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t>
		: public HCallBase<3, descriptor> {
public:
	typedef HCall<3, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t> this_type;
	typedef HCallBase<3, descriptor> base_type;
private:
	typedef typename descriptor::arg0_t fa0_t;
	typedef typename descriptor::sorted_real_args::a1_t fa1_t;
	typedef typename descriptor::sorted_real_args::a2_t fa2_t;
	typename descriptor::v0_t _a0;
	typename descriptor::v1_t _a1;
	typename descriptor::v2_t _a2;
	typename descriptor::v3_t _a3;
	typename descriptor::v4_t _a4;
	typename descriptor::v5_t _a5;
	typename descriptor::v6_t _a6;
	typename descriptor::v7_t _a7;
public:
	HCall( CALL_t&& call_, a0_t&& a0, a1_t&& a1,
			a2_t a2, a3_t&& a3, a4_t&& a4, a5_t&& a5, a6_t&& a6, a7_t&& a7 )
		: base_type( yaal::forward<CALL_t>( call_ ) ), _a0( yaal::forward<a0_t>( a0 ) ),
		_a1( yaal::forward<a1_t>( a1 ) ), _a2( yaal::forward<a2_t>( a2 ) ),
		_a3( yaal::forward<a3_t>( a3 ) ), _a4( yaal::forward<a4_t>( a4 ) ),
		_a5( yaal::forward<a5_t>( a5 ) ), _a6( yaal::forward<a6_t>( a6 ) ), _a7( yaal::forward<a7_t>( a7 ) ) {
		return;
	}
	HCall( HCall const& ) = default;
	HCall( HCall&& ) noexcept = default;
	HCall& operator = ( HCall const& ) = default;
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2 ) {
		return ( (base_type::_call)(
				free_standing_call_args::resolve_arg_get( _a0, a0, a1, a2 ),
				free_standing_call_args::resolve_arg_get( _a1, a0, a1, a2 ),
				free_standing_call_args::resolve_arg_get( _a2, a0, a1, a2 ),
				free_standing_call_args::resolve_arg_get( _a3, a0, a1, a2 ),
				free_standing_call_args::resolve_arg_get( _a4, a0, a1, a2 ),
				free_standing_call_args::resolve_arg_get( _a5, a0, a1, a2 ),
				free_standing_call_args::resolve_arg_get( _a6, a0, a1, a2 ),
				free_standing_call_args::resolve_arg_get( _a7, a0, a1, a2 ) ) ); }
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2 ) const { return ( (base_type::_call)(
				free_standing_call_args::resolve_arg_get( _a0, a0, a1, a2 ),
				free_standing_call_args::resolve_arg_get( _a1, a0, a1, a2 ),
				free_standing_call_args::resolve_arg_get( _a2, a0, a1, a2 ),
				free_standing_call_args::resolve_arg_get( _a3, a0, a1, a2 ),
				free_standing_call_args::resolve_arg_get( _a4, a0, a1, a2 ),
				free_standing_call_args::resolve_arg_get( _a5, a0, a1, a2 ),
				free_standing_call_args::resolve_arg_get( _a6, a0, a1, a2 ),
				free_standing_call_args::resolve_arg_get( _a7, a0, a1, a2 ) ) ); }
};

template<typename descriptor, typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t, typename a6_t, typename a7_t,
	typename a8_t>
class HCall<3, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t>
		: public HCallBase<3, descriptor> {
public:
	typedef HCall<3, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t> this_type;
	typedef HCallBase<3, descriptor> base_type;
private:
	typedef typename descriptor::arg0_t fa0_t;
	typedef typename descriptor::sorted_real_args::a1_t fa1_t;
	typedef typename descriptor::sorted_real_args::a2_t fa2_t;
	typename descriptor::v0_t _a0;
	typename descriptor::v1_t _a1;
	typename descriptor::v2_t _a2;
	typename descriptor::v3_t _a3;
	typename descriptor::v4_t _a4;
	typename descriptor::v5_t _a5;
	typename descriptor::v6_t _a6;
	typename descriptor::v7_t _a7;
	typename descriptor::v8_t _a8;
public:
	HCall( CALL_t&& call_, a0_t&& a0, a1_t&& a1, a2_t&& a2, a3_t&& a3, a4_t&& a4, a5_t&& a5, a6_t&& a6, a7_t&& a7, a8_t&& a8 )
		: base_type( yaal::forward<CALL_t>( call_ ) ), _a0( yaal::forward<a0_t>( a0 ) ),
		_a1( yaal::forward<a1_t>( a1 ) ), _a2( yaal::forward<a2_t>( a2 ) ),
		_a3( yaal::forward<a3_t>( a3 ) ), _a4( yaal::forward<a4_t>( a4 ) ),
		_a5( yaal::forward<a5_t>( a5 ) ), _a6( yaal::forward<a6_t>( a6 ) ),
		_a7( yaal::forward<a7_t>( a7 ) ), _a8( yaal::forward<a8_t>( a8 ) ) {
		return;
	}
	HCall( HCall const& ) = default;
	HCall( HCall&& ) noexcept = default;
	HCall& operator = ( HCall const& ) = default;
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2 ) {
		return ( (base_type::_call)(
				free_standing_call_args::resolve_arg_get( _a0, a0, a1, a2 ),
				free_standing_call_args::resolve_arg_get( _a1, a0, a1, a2 ),
				free_standing_call_args::resolve_arg_get( _a2, a0, a1, a2 ),
				free_standing_call_args::resolve_arg_get( _a3, a0, a1, a2 ),
				free_standing_call_args::resolve_arg_get( _a4, a0, a1, a2 ),
				free_standing_call_args::resolve_arg_get( _a5, a0, a1, a2 ),
				free_standing_call_args::resolve_arg_get( _a6, a0, a1, a2 ),
				free_standing_call_args::resolve_arg_get( _a7, a0, a1, a2 ),
				free_standing_call_args::resolve_arg_get( _a8, a0, a1, a2 ) ) ); }
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2 ) const { return ( (base_type::_call)(
				free_standing_call_args::resolve_arg_get( _a0, a0, a1, a2 ),
				free_standing_call_args::resolve_arg_get( _a1, a0, a1, a2 ),
				free_standing_call_args::resolve_arg_get( _a2, a0, a1, a2 ),
				free_standing_call_args::resolve_arg_get( _a3, a0, a1, a2 ),
				free_standing_call_args::resolve_arg_get( _a4, a0, a1, a2 ),
				free_standing_call_args::resolve_arg_get( _a5, a0, a1, a2 ),
				free_standing_call_args::resolve_arg_get( _a6, a0, a1, a2 ),
				free_standing_call_args::resolve_arg_get( _a7, a0, a1, a2 ),
				free_standing_call_args::resolve_arg_get( _a8, a0, a1, a2 ) ) ); }
};

template<typename descriptor, typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t, typename a6_t, typename a7_t,
	typename a8_t, typename a9_t>
class HCall<3, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t>
		: public HCallBase<3, descriptor> {
public:
	typedef HCall<3, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t> this_type;
	typedef HCallBase<3, descriptor> base_type;
private:
	typedef typename descriptor::arg0_t fa0_t;
	typedef typename descriptor::sorted_real_args::a1_t fa1_t;
	typedef typename descriptor::sorted_real_args::a2_t fa2_t;
	typename descriptor::v0_t _a0;
	typename descriptor::v1_t _a1;
	typename descriptor::v2_t _a2;
	typename descriptor::v3_t _a3;
	typename descriptor::v4_t _a4;
	typename descriptor::v5_t _a5;
	typename descriptor::v6_t _a6;
	typename descriptor::v7_t _a7;
	typename descriptor::v8_t _a8;
	typename descriptor::v9_t _a9;
public:
	HCall( CALL_t&& call_, a0_t&& a0, a1_t&& a1, a2_t&& a2, a3_t&& a3, a4_t&& a4, a5_t&& a5, a6_t&& a6, a7_t&& a7, a8_t&& a8, a9_t&& a9 )
		: base_type( yaal::forward<CALL_t>( call_ ) ), _a0( yaal::forward<a0_t>( a0 ) ),
		_a1( yaal::forward<a1_t>( a1 ) ), _a2( yaal::forward<a2_t>( a2 ) ),
		_a3( yaal::forward<a3_t>( a3 ) ), _a4( yaal::forward<a4_t>( a4 ) ),
		_a5( yaal::forward<a5_t>( a5 ) ), _a6( yaal::forward<a6_t>( a6 ) ),
		_a7( yaal::forward<a7_t>( a7 ) ), _a8( yaal::forward<a8_t>( a8 ) ), _a9( yaal::forward<a9_t>( a9 ) ) {
		return;
	}
	HCall( HCall const& ) = default;
	HCall( HCall&& ) noexcept = default;
	HCall& operator = ( HCall const& ) = default;
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2 ) {
		return ( (base_type::_call)(
				free_standing_call_args::resolve_arg_get( _a0, a0, a1, a2 ),
				free_standing_call_args::resolve_arg_get( _a1, a0, a1, a2 ),
				free_standing_call_args::resolve_arg_get( _a2, a0, a1, a2 ),
				free_standing_call_args::resolve_arg_get( _a3, a0, a1, a2 ),
				free_standing_call_args::resolve_arg_get( _a4, a0, a1, a2 ),
				free_standing_call_args::resolve_arg_get( _a5, a0, a1, a2 ),
				free_standing_call_args::resolve_arg_get( _a6, a0, a1, a2 ),
				free_standing_call_args::resolve_arg_get( _a7, a0, a1, a2 ),
				free_standing_call_args::resolve_arg_get( _a8, a0, a1, a2 ),
				free_standing_call_args::resolve_arg_get( _a9, a0, a1, a2 ) ) ); }
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2 ) const { return ( (base_type::_call)(
				free_standing_call_args::resolve_arg_get( _a0, a0, a1, a2 ),
				free_standing_call_args::resolve_arg_get( _a1, a0, a1, a2 ),
				free_standing_call_args::resolve_arg_get( _a2, a0, a1, a2 ),
				free_standing_call_args::resolve_arg_get( _a3, a0, a1, a2 ),
				free_standing_call_args::resolve_arg_get( _a4, a0, a1, a2 ),
				free_standing_call_args::resolve_arg_get( _a5, a0, a1, a2 ),
				free_standing_call_args::resolve_arg_get( _a6, a0, a1, a2 ),
				free_standing_call_args::resolve_arg_get( _a7, a0, a1, a2 ),
				free_standing_call_args::resolve_arg_get( _a8, a0, a1, a2 ),
				free_standing_call_args::resolve_arg_get( _a9, a0, a1, a2 ) ) ); }
};

template<typename descriptor, typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t, typename a6_t, typename a7_t,
	typename a8_t, typename a9_t, typename a10_t>
class HCall<3, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t, a10_t>
		: public HCallBase<3, descriptor> {
public:
	typedef HCall<3, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t, a10_t> this_type;
	typedef HCallBase<3, descriptor> base_type;
private:
	typedef typename descriptor::arg0_t fa0_t;
	typedef typename descriptor::sorted_real_args::a1_t fa1_t;
	typedef typename descriptor::sorted_real_args::a2_t fa2_t;
	typename descriptor::v0_t _a0;
	typename descriptor::v1_t _a1;
	typename descriptor::v2_t _a2;
	typename descriptor::v3_t _a3;
	typename descriptor::v4_t _a4;
	typename descriptor::v5_t _a5;
	typename descriptor::v6_t _a6;
	typename descriptor::v7_t _a7;
	typename descriptor::v8_t _a8;
	typename descriptor::v9_t _a9;
	typename descriptor::v10_t _a10;
public:
	HCall( CALL_t&& call_, a0_t&& a0, a1_t&& a1, a2_t&& a2, a3_t&& a3, a4_t&& a4, a5_t&& a5, a6_t&& a6, a7_t&& a7, a8_t&& a8, a9_t&& a9, a10_t&& a10 )
		: base_type( yaal::forward<CALL_t>( call_ ) ), _a0( yaal::forward<a0_t>( a0 ) ),
		_a1( yaal::forward<a1_t>( a1 ) ), _a2( yaal::forward<a2_t>( a2 ) ),
		_a3( yaal::forward<a3_t>( a3 ) ), _a4( yaal::forward<a4_t>( a4 ) ),
		_a5( yaal::forward<a5_t>( a5 ) ), _a6( yaal::forward<a6_t>( a6 ) ),
		_a7( yaal::forward<a7_t>( a7 ) ), _a8( yaal::forward<a8_t>( a8 ) ),
		_a9( yaal::forward<a9_t>( a9 ) ), _a10( yaal::forward<a10_t>( a10 ) ) {
		return;
	}
	HCall( HCall const& ) = default;
	HCall( HCall&& ) noexcept = default;
	HCall& operator = ( HCall const& ) = default;
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2 ) {
		return ( (base_type::_call)(
				free_standing_call_args::resolve_arg_get( _a0, a0, a1, a2 ),
				free_standing_call_args::resolve_arg_get( _a1, a0, a1, a2 ),
				free_standing_call_args::resolve_arg_get( _a2, a0, a1, a2 ),
				free_standing_call_args::resolve_arg_get( _a3, a0, a1, a2 ),
				free_standing_call_args::resolve_arg_get( _a4, a0, a1, a2 ),
				free_standing_call_args::resolve_arg_get( _a5, a0, a1, a2 ),
				free_standing_call_args::resolve_arg_get( _a6, a0, a1, a2 ),
				free_standing_call_args::resolve_arg_get( _a7, a0, a1, a2 ),
				free_standing_call_args::resolve_arg_get( _a8, a0, a1, a2 ),
				free_standing_call_args::resolve_arg_get( _a9, a0, a1, a2 ),
				free_standing_call_args::resolve_arg_get( _a10, a0, a1, a2 ) ) ); }
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2 ) const { return ( (base_type::_call)(
				free_standing_call_args::resolve_arg_get( _a0, a0, a1, a2 ),
				free_standing_call_args::resolve_arg_get( _a1, a0, a1, a2 ),
				free_standing_call_args::resolve_arg_get( _a2, a0, a1, a2 ),
				free_standing_call_args::resolve_arg_get( _a3, a0, a1, a2 ),
				free_standing_call_args::resolve_arg_get( _a4, a0, a1, a2 ),
				free_standing_call_args::resolve_arg_get( _a5, a0, a1, a2 ),
				free_standing_call_args::resolve_arg_get( _a6, a0, a1, a2 ),
				free_standing_call_args::resolve_arg_get( _a7, a0, a1, a2 ),
				free_standing_call_args::resolve_arg_get( _a8, a0, a1, a2 ),
				free_standing_call_args::resolve_arg_get( _a9, a0, a1, a2 ),
				free_standing_call_args::resolve_arg_get( _a10, a0, a1, a2 ) ) ); }
};

template<typename descriptor, typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t>
class HCall<4, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t>
		: public HCallBase<4, descriptor> {
public:
	typedef HCall<4, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t> this_type;
	typedef HCallBase<4, descriptor> base_type;
private:
	typedef typename descriptor::arg0_t fa0_t;
	typedef typename descriptor::sorted_real_args::a1_t fa1_t;
	typedef typename descriptor::sorted_real_args::a2_t fa2_t;
	typedef typename descriptor::sorted_real_args::a3_t fa3_t;
	typename descriptor::v0_t _a0;
	typename descriptor::v1_t _a1;
	typename descriptor::v2_t _a2;
	typename descriptor::v3_t _a3;
public:
	HCall( CALL_t&& call_, a0_t&& a0, a1_t&& a1, a2_t&& a2, a3_t&& a3 )
		: base_type( yaal::forward<CALL_t>( call_ ) ), _a0( yaal::forward<a0_t>( a0 ) ),
		_a1( yaal::forward<a1_t>( a1 ) ), _a2( yaal::forward<a2_t>( a2 ) ), _a3( yaal::forward<a3_t>( a3 ) ) {
		return;
	}
	HCall( HCall const& ) = default;
	HCall( HCall&& ) noexcept = default;
	HCall& operator = ( HCall const& ) = default;
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3 ) {
		return ( (base_type::_call)(
				free_standing_call_args::resolve_arg_get( _a0, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg_get( _a1, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg_get( _a2, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg_get( _a3, a0, a1, a2, a3 ) ) ); }
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3 ) const { return ( (base_type::_call)(
				free_standing_call_args::resolve_arg_get( _a0, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg_get( _a1, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg_get( _a2, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg_get( _a3, a0, a1, a2, a3 ) ) ); }
};

template<typename descriptor, typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t>
class HCall<4, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t>
		: public HCallBase<4, descriptor> {
public:
	typedef HCall<4, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t> this_type;
	typedef HCallBase<4, descriptor> base_type;
private:
	typedef typename descriptor::arg0_t fa0_t;
	typedef typename descriptor::sorted_real_args::a1_t fa1_t;
	typedef typename descriptor::sorted_real_args::a2_t fa2_t;
	typedef typename descriptor::sorted_real_args::a3_t fa3_t;
	typename descriptor::v0_t _a0;
	typename descriptor::v1_t _a1;
	typename descriptor::v2_t _a2;
	typename descriptor::v3_t _a3;
	typename descriptor::v4_t _a4;
public:
	HCall( CALL_t&& call_, a0_t&& a0, a1_t&& a1, a2_t&& a2, a3_t&& a3, a4_t&& a4 )
		: base_type( yaal::forward<CALL_t>( call_ ) ), _a0( yaal::forward<a0_t>( a0 ) ),
		_a1( yaal::forward<a1_t>( a1 ) ), _a2( yaal::forward<a2_t>( a2 ) ),
		_a3( yaal::forward<a3_t>( a3 ) ), _a4( yaal::forward<a4_t>( a4 ) ) {
		return;
	}
	HCall( HCall const& ) = default;
	HCall( HCall&& ) noexcept = default;
	HCall& operator = ( HCall const& ) = default;
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3 ) {
		return ( (base_type::_call)(
				free_standing_call_args::resolve_arg_get( _a0, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg_get( _a1, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg_get( _a2, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg_get( _a3, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg_get( _a4, a0, a1, a2, a3 ) ) ); }
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3 ) const { return ( (base_type::_call)(
				free_standing_call_args::resolve_arg_get( _a0, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg_get( _a1, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg_get( _a2, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg_get( _a3, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg_get( _a4, a0, a1, a2, a3 ) ) ); }
};

template<typename descriptor, typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t>
class HCall<4, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t>
		: public HCallBase<4, descriptor> {
public:
	typedef HCall<4, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t> this_type;
	typedef HCallBase<4, descriptor> base_type;
private:
	typedef typename descriptor::arg0_t fa0_t;
	typedef typename descriptor::sorted_real_args::a1_t fa1_t;
	typedef typename descriptor::sorted_real_args::a2_t fa2_t;
	typedef typename descriptor::sorted_real_args::a3_t fa3_t;
	typename descriptor::v0_t _a0;
	typename descriptor::v1_t _a1;
	typename descriptor::v2_t _a2;
	typename descriptor::v3_t _a3;
	typename descriptor::v4_t _a4;
	typename descriptor::v5_t _a5;
public:
	HCall( CALL_t&& call_, a0_t&& a0, a1_t&& a1, a2_t&& a2, a3_t&& a3, a4_t&& a4, a5_t&& a5 )
		: base_type( yaal::forward<CALL_t>( call_ ) ), _a0( yaal::forward<a0_t>( a0 ) ),
		_a1( yaal::forward<a1_t>( a1 ) ), _a2( yaal::forward<a2_t>( a2 ) ),
		_a3( yaal::forward<a3_t>( a3 ) ), _a4( yaal::forward<a4_t>( a4 ) ), _a5( yaal::forward<a5_t>( a5 ) ) {
		return;
	}
	HCall( HCall const& ) = default;
	HCall( HCall&& ) noexcept = default;
	HCall& operator = ( HCall const& ) = default;
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3 ) {
		return ( (base_type::_call)(
				free_standing_call_args::resolve_arg_get( _a0, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg_get( _a1, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg_get( _a2, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg_get( _a3, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg_get( _a4, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg_get( _a5, a0, a1, a2, a3 ) ) ); }
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3 ) const { return ( (base_type::_call)(
				free_standing_call_args::resolve_arg_get( _a0, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg_get( _a1, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg_get( _a2, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg_get( _a3, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg_get( _a4, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg_get( _a5, a0, a1, a2, a3 ) ) ); }
};

template<typename descriptor, typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t, typename a6_t>
class HCall<4, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t>
		: public HCallBase<4, descriptor> {
public:
	typedef HCall<4, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t> this_type;
	typedef HCallBase<4, descriptor> base_type;
private:
	typedef typename descriptor::arg0_t fa0_t;
	typedef typename descriptor::sorted_real_args::a1_t fa1_t;
	typedef typename descriptor::sorted_real_args::a2_t fa2_t;
	typedef typename descriptor::sorted_real_args::a3_t fa3_t;
	typename descriptor::v0_t _a0;
	typename descriptor::v1_t _a1;
	typename descriptor::v2_t _a2;
	typename descriptor::v3_t _a3;
	typename descriptor::v4_t _a4;
	typename descriptor::v5_t _a5;
	typename descriptor::v6_t _a6;
public:
	HCall( CALL_t&& call_, a0_t&& a0, a1_t&& a1,
			a2_t a2, a3_t&& a3, a4_t&& a4, a5_t&& a5, a6_t&& a6 )
		: base_type( yaal::forward<CALL_t>( call_ ) ), _a0( yaal::forward<a0_t>( a0 ) ),
		_a1( yaal::forward<a1_t>( a1 ) ), _a2( yaal::forward<a2_t>( a2 ) ),
		_a3( yaal::forward<a3_t>( a3 ) ), _a4( yaal::forward<a4_t>( a4 ) ),
		_a5( yaal::forward<a5_t>( a5 ) ), _a6( yaal::forward<a6_t>( a6 ) ) {
		return;
	}
	HCall( HCall const& ) = default;
	HCall( HCall&& ) noexcept = default;
	HCall& operator = ( HCall const& ) = default;
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3 ) {
		return ( (base_type::_call)(
				free_standing_call_args::resolve_arg_get( _a0, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg_get( _a1, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg_get( _a2, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg_get( _a3, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg_get( _a4, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg_get( _a5, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg_get( _a6, a0, a1, a2, a3 ) ) ); }
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3 ) const { return ( (base_type::_call)(
				free_standing_call_args::resolve_arg_get( _a0, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg_get( _a1, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg_get( _a2, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg_get( _a3, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg_get( _a4, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg_get( _a5, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg_get( _a6, a0, a1, a2, a3 ) ) ); }
};

template<typename descriptor, typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t, typename a6_t, typename a7_t>
class HCall<4, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t>
		: public HCallBase<4, descriptor> {
public:
	typedef HCall<4, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t> this_type;
	typedef HCallBase<4, descriptor> base_type;
private:
	typedef typename descriptor::arg0_t fa0_t;
	typedef typename descriptor::sorted_real_args::a1_t fa1_t;
	typedef typename descriptor::sorted_real_args::a2_t fa2_t;
	typedef typename descriptor::sorted_real_args::a3_t fa3_t;
	typename descriptor::v0_t _a0;
	typename descriptor::v1_t _a1;
	typename descriptor::v2_t _a2;
	typename descriptor::v3_t _a3;
	typename descriptor::v4_t _a4;
	typename descriptor::v5_t _a5;
	typename descriptor::v6_t _a6;
	typename descriptor::v7_t _a7;
public:
	HCall( CALL_t&& call_, a0_t&& a0, a1_t&& a1,
			a2_t a2, a3_t&& a3, a4_t&& a4, a5_t&& a5, a6_t&& a6, a7_t&& a7 )
		: base_type( yaal::forward<CALL_t>( call_ ) ), _a0( yaal::forward<a0_t>( a0 ) ),
		_a1( yaal::forward<a1_t>( a1 ) ), _a2( yaal::forward<a2_t>( a2 ) ),
		_a3( yaal::forward<a3_t>( a3 ) ), _a4( yaal::forward<a4_t>( a4 ) ),
		_a5( yaal::forward<a5_t>( a5 ) ), _a6( yaal::forward<a6_t>( a6 ) ), _a7( yaal::forward<a7_t>( a7 ) ) {
		return;
	}
	HCall( HCall const& ) = default;
	HCall( HCall&& ) noexcept = default;
	HCall& operator = ( HCall const& ) = default;
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3 ) {
		return ( (base_type::_call)(
				free_standing_call_args::resolve_arg_get( _a0, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg_get( _a1, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg_get( _a2, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg_get( _a3, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg_get( _a4, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg_get( _a5, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg_get( _a6, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg_get( _a7, a0, a1, a2, a3 ) ) ); }
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3 ) const { return ( (base_type::_call)(
				free_standing_call_args::resolve_arg_get( _a0, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg_get( _a1, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg_get( _a2, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg_get( _a3, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg_get( _a4, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg_get( _a5, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg_get( _a6, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg_get( _a7, a0, a1, a2, a3 ) ) ); }
};

template<typename descriptor, typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t, typename a6_t, typename a7_t,
	typename a8_t>
class HCall<4, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t>
		: public HCallBase<4, descriptor> {
public:
	typedef HCall<4, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t> this_type;
	typedef HCallBase<4, descriptor> base_type;
private:
	typedef typename descriptor::arg0_t fa0_t;
	typedef typename descriptor::sorted_real_args::a1_t fa1_t;
	typedef typename descriptor::sorted_real_args::a2_t fa2_t;
	typedef typename descriptor::sorted_real_args::a3_t fa3_t;
	typename descriptor::v0_t _a0;
	typename descriptor::v1_t _a1;
	typename descriptor::v2_t _a2;
	typename descriptor::v3_t _a3;
	typename descriptor::v4_t _a4;
	typename descriptor::v5_t _a5;
	typename descriptor::v6_t _a6;
	typename descriptor::v7_t _a7;
	typename descriptor::v8_t _a8;
public:
	HCall( CALL_t&& call_, a0_t&& a0, a1_t&& a1, a2_t&& a2, a3_t&& a3, a4_t&& a4, a5_t&& a5, a6_t&& a6, a7_t&& a7, a8_t&& a8 )
		: base_type( yaal::forward<CALL_t>( call_ ) ), _a0( yaal::forward<a0_t>( a0 ) ),
		_a1( yaal::forward<a1_t>( a1 ) ), _a2( yaal::forward<a2_t>( a2 ) ),
		_a3( yaal::forward<a3_t>( a3 ) ), _a4( yaal::forward<a4_t>( a4 ) ),
		_a5( yaal::forward<a5_t>( a5 ) ), _a6( yaal::forward<a6_t>( a6 ) ),
		_a7( yaal::forward<a7_t>( a7 ) ), _a8( yaal::forward<a8_t>( a8 ) ) {
		return;
	}
	HCall( HCall const& ) = default;
	HCall( HCall&& ) noexcept = default;
	HCall& operator = ( HCall const& ) = default;
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3 ) {
		return ( (base_type::_call)(
				free_standing_call_args::resolve_arg_get( _a0, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg_get( _a1, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg_get( _a2, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg_get( _a3, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg_get( _a4, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg_get( _a5, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg_get( _a6, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg_get( _a7, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg_get( _a8, a0, a1, a2, a3 ) ) ); }
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3 ) const { return ( (base_type::_call)(
				free_standing_call_args::resolve_arg_get( _a0, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg_get( _a1, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg_get( _a2, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg_get( _a3, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg_get( _a4, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg_get( _a5, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg_get( _a6, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg_get( _a7, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg_get( _a8, a0, a1, a2, a3 ) ) ); }
};

template<typename descriptor, typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t, typename a6_t, typename a7_t,
	typename a8_t, typename a9_t>
class HCall<4, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t>
		: public HCallBase<4, descriptor> {
public:
	typedef HCall<4, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t> this_type;
	typedef HCallBase<4, descriptor> base_type;
private:
	typedef typename descriptor::arg0_t fa0_t;
	typedef typename descriptor::sorted_real_args::a1_t fa1_t;
	typedef typename descriptor::sorted_real_args::a2_t fa2_t;
	typedef typename descriptor::sorted_real_args::a3_t fa3_t;
	typename descriptor::v0_t _a0;
	typename descriptor::v1_t _a1;
	typename descriptor::v2_t _a2;
	typename descriptor::v3_t _a3;
	typename descriptor::v4_t _a4;
	typename descriptor::v5_t _a5;
	typename descriptor::v6_t _a6;
	typename descriptor::v7_t _a7;
	typename descriptor::v8_t _a8;
	typename descriptor::v9_t _a9;
public:
	HCall( CALL_t&& call_, a0_t&& a0, a1_t&& a1, a2_t&& a2, a3_t&& a3, a4_t&& a4, a5_t&& a5, a6_t&& a6, a7_t&& a7, a8_t&& a8, a9_t&& a9 )
		: base_type( yaal::forward<CALL_t>( call_ ) ), _a0( yaal::forward<a0_t>( a0 ) ),
		_a1( yaal::forward<a1_t>( a1 ) ), _a2( yaal::forward<a2_t>( a2 ) ),
		_a3( yaal::forward<a3_t>( a3 ) ), _a4( yaal::forward<a4_t>( a4 ) ),
		_a5( yaal::forward<a5_t>( a5 ) ), _a6( yaal::forward<a6_t>( a6 ) ),
		_a7( yaal::forward<a7_t>( a7 ) ), _a8( yaal::forward<a8_t>( a8 ) ), _a9( yaal::forward<a9_t>( a9 ) ) {
		return;
	}
	HCall( HCall const& ) = default;
	HCall( HCall&& ) noexcept = default;
	HCall& operator = ( HCall const& ) = default;
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3 ) {
		return ( (base_type::_call)(
				free_standing_call_args::resolve_arg_get( _a0, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg_get( _a1, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg_get( _a2, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg_get( _a3, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg_get( _a4, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg_get( _a5, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg_get( _a6, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg_get( _a7, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg_get( _a8, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg_get( _a9, a0, a1, a2, a3 ) ) ); }
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3 ) const { return ( (base_type::_call)(
				free_standing_call_args::resolve_arg_get( _a0, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg_get( _a1, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg_get( _a2, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg_get( _a3, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg_get( _a4, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg_get( _a5, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg_get( _a6, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg_get( _a7, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg_get( _a8, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg_get( _a9, a0, a1, a2, a3 ) ) ); }
};

template<typename descriptor, typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t, typename a6_t, typename a7_t,
	typename a8_t, typename a9_t, typename a10_t>
class HCall<4, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t, a10_t>
		: public HCallBase<4, descriptor> {
public:
	typedef HCall<4, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t, a10_t> this_type;
	typedef HCallBase<4, descriptor> base_type;
private:
	typedef typename descriptor::arg0_t fa0_t;
	typedef typename descriptor::sorted_real_args::a1_t fa1_t;
	typedef typename descriptor::sorted_real_args::a2_t fa2_t;
	typedef typename descriptor::sorted_real_args::a3_t fa3_t;
	typename descriptor::v0_t _a0;
	typename descriptor::v1_t _a1;
	typename descriptor::v2_t _a2;
	typename descriptor::v3_t _a3;
	typename descriptor::v4_t _a4;
	typename descriptor::v5_t _a5;
	typename descriptor::v6_t _a6;
	typename descriptor::v7_t _a7;
	typename descriptor::v8_t _a8;
	typename descriptor::v9_t _a9;
	typename descriptor::v10_t _a10;
public:
	HCall( CALL_t&& call_, a0_t&& a0, a1_t&& a1, a2_t&& a2, a3_t&& a3, a4_t&& a4, a5_t&& a5, a6_t&& a6, a7_t&& a7, a8_t&& a8, a9_t&& a9, a10_t&& a10 )
		: base_type( yaal::forward<CALL_t>( call_ ) ), _a0( yaal::forward<a0_t>( a0 ) ),
		_a1( yaal::forward<a1_t>( a1 ) ), _a2( yaal::forward<a2_t>( a2 ) ),
		_a3( yaal::forward<a3_t>( a3 ) ), _a4( yaal::forward<a4_t>( a4 ) ),
		_a5( yaal::forward<a5_t>( a5 ) ), _a6( yaal::forward<a6_t>( a6 ) ),
		_a7( yaal::forward<a7_t>( a7 ) ), _a8( yaal::forward<a8_t>( a8 ) ),
		_a9( yaal::forward<a9_t>( a9 ) ), _a10( yaal::forward<a10_t>( a10 ) ) {
		return;
	}
	HCall( HCall const& ) = default;
	HCall( HCall&& ) noexcept = default;
	HCall& operator = ( HCall const& ) = default;
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3 ) {
		return ( (base_type::_call)(
				free_standing_call_args::resolve_arg_get( _a0, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg_get( _a1, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg_get( _a2, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg_get( _a3, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg_get( _a4, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg_get( _a5, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg_get( _a6, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg_get( _a7, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg_get( _a8, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg_get( _a9, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg_get( _a10, a0, a1, a2, a3 ) ) ); }
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3 ) const { return ( (base_type::_call)(
				free_standing_call_args::resolve_arg_get( _a0, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg_get( _a1, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg_get( _a2, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg_get( _a3, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg_get( _a4, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg_get( _a5, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg_get( _a6, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg_get( _a7, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg_get( _a8, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg_get( _a9, a0, a1, a2, a3 ),
				free_standing_call_args::resolve_arg_get( _a10, a0, a1, a2, a3 ) ) ); }
};

template<typename descriptor, typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t>
class HCall<5, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t>
		: public HCallBase<5, descriptor> {
public:
	typedef HCall<5, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t> this_type;
	typedef HCallBase<5, descriptor> base_type;
private:
	typedef typename descriptor::arg0_t fa0_t;
	typedef typename descriptor::sorted_real_args::a1_t fa1_t;
	typedef typename descriptor::sorted_real_args::a2_t fa2_t;
	typedef typename descriptor::sorted_real_args::a3_t fa3_t;
	typedef typename descriptor::sorted_real_args::a4_t fa4_t;
	typename descriptor::v0_t _a0;
	typename descriptor::v1_t _a1;
	typename descriptor::v2_t _a2;
	typename descriptor::v3_t _a3;
	typename descriptor::v4_t _a4;
public:
	HCall( CALL_t&& call_, a0_t&& a0, a1_t&& a1, a2_t&& a2, a3_t&& a3, a4_t&& a4 )
		: base_type( yaal::forward<CALL_t>( call_ ) ), _a0( yaal::forward<a0_t>( a0 ) ),
		_a1( yaal::forward<a1_t>( a1 ) ), _a2( yaal::forward<a2_t>( a2 ) ),
		_a3( yaal::forward<a3_t>( a3 ) ), _a4( yaal::forward<a4_t>( a4 ) ) {
		return;
	}
	HCall( HCall const& ) = default;
	HCall( HCall&& ) noexcept = default;
	HCall& operator = ( HCall const& ) = default;
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3, fa4_t a4 ) {
		return ( (base_type::_call)(
				free_standing_call_args::resolve_arg_get( _a0, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg_get( _a1, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg_get( _a2, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg_get( _a3, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg_get( _a4, a0, a1, a2, a3, a4 ) ) ); }
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3, fa4_t a4 ) const { return ( (base_type::_call)(
				free_standing_call_args::resolve_arg_get( _a0, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg_get( _a1, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg_get( _a2, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg_get( _a3, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg_get( _a4, a0, a1, a2, a3, a4 ) ) ); }
};

template<typename descriptor, typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t>
class HCall<5, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t>
		: public HCallBase<5, descriptor> {
public:
	typedef HCall<5, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t> this_type;
	typedef HCallBase<5, descriptor> base_type;
private:
	typedef typename descriptor::arg0_t fa0_t;
	typedef typename descriptor::sorted_real_args::a1_t fa1_t;
	typedef typename descriptor::sorted_real_args::a2_t fa2_t;
	typedef typename descriptor::sorted_real_args::a3_t fa3_t;
	typedef typename descriptor::sorted_real_args::a4_t fa4_t;
	typename descriptor::v0_t _a0;
	typename descriptor::v1_t _a1;
	typename descriptor::v2_t _a2;
	typename descriptor::v3_t _a3;
	typename descriptor::v4_t _a4;
	typename descriptor::v5_t _a5;
public:
	HCall( CALL_t&& call_, a0_t&& a0, a1_t&& a1, a2_t&& a2, a3_t&& a3, a4_t&& a4, a5_t&& a5 )
		: base_type( yaal::forward<CALL_t>( call_ ) ), _a0( yaal::forward<a0_t>( a0 ) ),
		_a1( yaal::forward<a1_t>( a1 ) ), _a2( yaal::forward<a2_t>( a2 ) ),
		_a3( yaal::forward<a3_t>( a3 ) ), _a4( yaal::forward<a4_t>( a4 ) ), _a5( yaal::forward<a5_t>( a5 ) ) {
		return;
	}
	HCall( HCall const& ) = default;
	HCall( HCall&& ) noexcept = default;
	HCall& operator = ( HCall const& ) = default;
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3, fa4_t a4 ) {
		return ( (base_type::_call)(
				free_standing_call_args::resolve_arg_get( _a0, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg_get( _a1, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg_get( _a2, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg_get( _a3, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg_get( _a4, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg_get( _a5, a0, a1, a2, a3, a4 ) ) ); }
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3, fa4_t a4 ) const { return ( (base_type::_call)(
				free_standing_call_args::resolve_arg_get( _a0, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg_get( _a1, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg_get( _a2, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg_get( _a3, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg_get( _a4, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg_get( _a5, a0, a1, a2, a3, a4 ) ) ); }
};

template<typename descriptor, typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t, typename a6_t>
class HCall<5, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t>
		: public HCallBase<5, descriptor> {
public:
	typedef HCall<5, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t> this_type;
	typedef HCallBase<5, descriptor> base_type;
private:
	typedef typename descriptor::arg0_t fa0_t;
	typedef typename descriptor::sorted_real_args::a1_t fa1_t;
	typedef typename descriptor::sorted_real_args::a2_t fa2_t;
	typedef typename descriptor::sorted_real_args::a3_t fa3_t;
	typedef typename descriptor::sorted_real_args::a4_t fa4_t;
	typename descriptor::v0_t _a0;
	typename descriptor::v1_t _a1;
	typename descriptor::v2_t _a2;
	typename descriptor::v3_t _a3;
	typename descriptor::v4_t _a4;
	typename descriptor::v5_t _a5;
	typename descriptor::v6_t _a6;
public:
	HCall( CALL_t&& call_, a0_t&& a0, a1_t&& a1,
			a2_t a2, a3_t&& a3, a4_t&& a4, a5_t&& a5, a6_t&& a6 )
		: base_type( yaal::forward<CALL_t>( call_ ) ), _a0( yaal::forward<a0_t>( a0 ) ),
		_a1( yaal::forward<a1_t>( a1 ) ), _a2( yaal::forward<a2_t>( a2 ) ),
		_a3( yaal::forward<a3_t>( a3 ) ), _a4( yaal::forward<a4_t>( a4 ) ),
		_a5( yaal::forward<a5_t>( a5 ) ), _a6( yaal::forward<a6_t>( a6 ) ) {
		return;
	}
	HCall( HCall const& ) = default;
	HCall( HCall&& ) noexcept = default;
	HCall& operator = ( HCall const& ) = default;
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3, fa4_t a4 ) {
		return ( (base_type::_call)(
				free_standing_call_args::resolve_arg_get( _a0, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg_get( _a1, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg_get( _a2, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg_get( _a3, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg_get( _a4, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg_get( _a5, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg_get( _a6, a0, a1, a2, a3, a4 ) ) ); }
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3, fa4_t a4 ) const { return ( (base_type::_call)(
				free_standing_call_args::resolve_arg_get( _a0, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg_get( _a1, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg_get( _a2, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg_get( _a3, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg_get( _a4, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg_get( _a5, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg_get( _a6, a0, a1, a2, a3, a4 ) ) ); }
};

template<typename descriptor, typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t, typename a6_t, typename a7_t>
class HCall<5, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t>
		: public HCallBase<5, descriptor> {
public:
	typedef HCall<5, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t> this_type;
	typedef HCallBase<5, descriptor> base_type;
private:
	typedef typename descriptor::arg0_t fa0_t;
	typedef typename descriptor::sorted_real_args::a1_t fa1_t;
	typedef typename descriptor::sorted_real_args::a2_t fa2_t;
	typedef typename descriptor::sorted_real_args::a3_t fa3_t;
	typedef typename descriptor::sorted_real_args::a4_t fa4_t;
	typename descriptor::v0_t _a0;
	typename descriptor::v1_t _a1;
	typename descriptor::v2_t _a2;
	typename descriptor::v3_t _a3;
	typename descriptor::v4_t _a4;
	typename descriptor::v5_t _a5;
	typename descriptor::v6_t _a6;
	typename descriptor::v7_t _a7;
public:
	HCall( CALL_t&& call_, a0_t&& a0, a1_t&& a1,
			a2_t a2, a3_t&& a3, a4_t&& a4, a5_t&& a5, a6_t&& a6, a7_t&& a7 )
		: base_type( yaal::forward<CALL_t>( call_ ) ), _a0( yaal::forward<a0_t>( a0 ) ),
		_a1( yaal::forward<a1_t>( a1 ) ), _a2( yaal::forward<a2_t>( a2 ) ),
		_a3( yaal::forward<a3_t>( a3 ) ), _a4( yaal::forward<a4_t>( a4 ) ),
		_a5( yaal::forward<a5_t>( a5 ) ), _a6( yaal::forward<a6_t>( a6 ) ), _a7( yaal::forward<a7_t>( a7 ) ) {
		return;
	}
	HCall( HCall const& ) = default;
	HCall( HCall&& ) noexcept = default;
	HCall& operator = ( HCall const& ) = default;
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3, fa4_t a4 ) {
		return ( (base_type::_call)(
				free_standing_call_args::resolve_arg_get( _a0, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg_get( _a1, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg_get( _a2, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg_get( _a3, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg_get( _a4, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg_get( _a5, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg_get( _a6, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg_get( _a7, a0, a1, a2, a3, a4 ) ) ); }
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3, fa4_t a4 ) const { return ( (base_type::_call)(
				free_standing_call_args::resolve_arg_get( _a0, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg_get( _a1, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg_get( _a2, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg_get( _a3, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg_get( _a4, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg_get( _a5, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg_get( _a6, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg_get( _a7, a0, a1, a2, a3, a4 ) ) ); }
};

template<typename descriptor, typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t, typename a6_t, typename a7_t,
	typename a8_t>
class HCall<5, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t>
		: public HCallBase<5, descriptor> {
public:
	typedef HCall<5, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t> this_type;
	typedef HCallBase<5, descriptor> base_type;
private:
	typedef typename descriptor::arg0_t fa0_t;
	typedef typename descriptor::sorted_real_args::a1_t fa1_t;
	typedef typename descriptor::sorted_real_args::a2_t fa2_t;
	typedef typename descriptor::sorted_real_args::a3_t fa3_t;
	typedef typename descriptor::sorted_real_args::a4_t fa4_t;
	typename descriptor::v0_t _a0;
	typename descriptor::v1_t _a1;
	typename descriptor::v2_t _a2;
	typename descriptor::v3_t _a3;
	typename descriptor::v4_t _a4;
	typename descriptor::v5_t _a5;
	typename descriptor::v6_t _a6;
	typename descriptor::v7_t _a7;
	typename descriptor::v8_t _a8;
public:
	HCall( CALL_t&& call_, a0_t&& a0, a1_t&& a1, a2_t&& a2, a3_t&& a3, a4_t&& a4, a5_t&& a5, a6_t&& a6, a7_t&& a7, a8_t&& a8 )
		: base_type( yaal::forward<CALL_t>( call_ ) ), _a0( yaal::forward<a0_t>( a0 ) ),
		_a1( yaal::forward<a1_t>( a1 ) ), _a2( yaal::forward<a2_t>( a2 ) ),
		_a3( yaal::forward<a3_t>( a3 ) ), _a4( yaal::forward<a4_t>( a4 ) ),
		_a5( yaal::forward<a5_t>( a5 ) ), _a6( yaal::forward<a6_t>( a6 ) ),
		_a7( yaal::forward<a7_t>( a7 ) ), _a8( yaal::forward<a8_t>( a8 ) ) {
		return;
	}
	HCall( HCall const& ) = default;
	HCall( HCall&& ) noexcept = default;
	HCall& operator = ( HCall const& ) = default;
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3, fa4_t a4 ) {
		return ( (base_type::_call)(
				free_standing_call_args::resolve_arg_get( _a0, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg_get( _a1, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg_get( _a2, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg_get( _a3, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg_get( _a4, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg_get( _a5, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg_get( _a6, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg_get( _a7, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg_get( _a8, a0, a1, a2, a3, a4 ) ) ); }
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3, fa4_t a4 ) const { return ( (base_type::_call)(
				free_standing_call_args::resolve_arg_get( _a0, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg_get( _a1, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg_get( _a2, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg_get( _a3, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg_get( _a4, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg_get( _a5, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg_get( _a6, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg_get( _a7, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg_get( _a8, a0, a1, a2, a3, a4 ) ) ); }
};

template<typename descriptor, typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t, typename a6_t, typename a7_t,
	typename a8_t, typename a9_t>
class HCall<5, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t>
		: public HCallBase<5, descriptor> {
public:
	typedef HCall<5, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t> this_type;
	typedef HCallBase<5, descriptor> base_type;
private:
	typedef typename descriptor::arg0_t fa0_t;
	typedef typename descriptor::sorted_real_args::a1_t fa1_t;
	typedef typename descriptor::sorted_real_args::a2_t fa2_t;
	typedef typename descriptor::sorted_real_args::a3_t fa3_t;
	typedef typename descriptor::sorted_real_args::a4_t fa4_t;
	typename descriptor::v0_t _a0;
	typename descriptor::v1_t _a1;
	typename descriptor::v2_t _a2;
	typename descriptor::v3_t _a3;
	typename descriptor::v4_t _a4;
	typename descriptor::v5_t _a5;
	typename descriptor::v6_t _a6;
	typename descriptor::v7_t _a7;
	typename descriptor::v8_t _a8;
	typename descriptor::v9_t _a9;
public:
	HCall( CALL_t&& call_, a0_t&& a0, a1_t&& a1, a2_t&& a2, a3_t&& a3, a4_t&& a4, a5_t&& a5, a6_t&& a6, a7_t&& a7, a8_t&& a8, a9_t&& a9 )
		: base_type( yaal::forward<CALL_t>( call_ ) ), _a0( yaal::forward<a0_t>( a0 ) ),
		_a1( yaal::forward<a1_t>( a1 ) ), _a2( yaal::forward<a2_t>( a2 ) ),
		_a3( yaal::forward<a3_t>( a3 ) ), _a4( yaal::forward<a4_t>( a4 ) ),
		_a5( yaal::forward<a5_t>( a5 ) ), _a6( yaal::forward<a6_t>( a6 ) ),
		_a7( yaal::forward<a7_t>( a7 ) ), _a8( yaal::forward<a8_t>( a8 ) ), _a9( yaal::forward<a9_t>( a9 ) ) {
		return;
	}
	HCall( HCall const& ) = default;
	HCall( HCall&& ) noexcept = default;
	HCall& operator = ( HCall const& ) = default;
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3, fa4_t a4 ) {
		return ( (base_type::_call)(
				free_standing_call_args::resolve_arg_get( _a0, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg_get( _a1, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg_get( _a2, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg_get( _a3, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg_get( _a4, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg_get( _a5, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg_get( _a6, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg_get( _a7, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg_get( _a8, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg_get( _a9, a0, a1, a2, a3, a4 ) ) ); }
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3, fa4_t a4 ) const { return ( (base_type::_call)(
				free_standing_call_args::resolve_arg_get( _a0, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg_get( _a1, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg_get( _a2, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg_get( _a3, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg_get( _a4, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg_get( _a5, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg_get( _a6, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg_get( _a7, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg_get( _a8, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg_get( _a9, a0, a1, a2, a3, a4 ) ) ); }
};

template<typename descriptor, typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t, typename a6_t, typename a7_t,
	typename a8_t, typename a9_t, typename a10_t>
class HCall<5, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t, a10_t>
		: public HCallBase<5, descriptor> {
public:
	typedef HCall<5, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t, a10_t> this_type;
	typedef HCallBase<5, descriptor> base_type;
private:
	typedef typename descriptor::arg0_t fa0_t;
	typedef typename descriptor::sorted_real_args::a1_t fa1_t;
	typedef typename descriptor::sorted_real_args::a2_t fa2_t;
	typedef typename descriptor::sorted_real_args::a3_t fa3_t;
	typedef typename descriptor::sorted_real_args::a4_t fa4_t;
	typename descriptor::v0_t _a0;
	typename descriptor::v1_t _a1;
	typename descriptor::v2_t _a2;
	typename descriptor::v3_t _a3;
	typename descriptor::v4_t _a4;
	typename descriptor::v5_t _a5;
	typename descriptor::v6_t _a6;
	typename descriptor::v7_t _a7;
	typename descriptor::v8_t _a8;
	typename descriptor::v9_t _a9;
	typename descriptor::v10_t _a10;
public:
	HCall( CALL_t&& call_, a0_t&& a0, a1_t&& a1, a2_t&& a2, a3_t&& a3, a4_t&& a4, a5_t&& a5, a6_t&& a6, a7_t&& a7, a8_t&& a8, a9_t&& a9, a10_t&& a10 )
		: base_type( yaal::forward<CALL_t>( call_ ) ), _a0( yaal::forward<a0_t>( a0 ) ),
		_a1( yaal::forward<a1_t>( a1 ) ), _a2( yaal::forward<a2_t>( a2 ) ),
		_a3( yaal::forward<a3_t>( a3 ) ), _a4( yaal::forward<a4_t>( a4 ) ),
		_a5( yaal::forward<a5_t>( a5 ) ), _a6( yaal::forward<a6_t>( a6 ) ),
		_a7( yaal::forward<a7_t>( a7 ) ), _a8( yaal::forward<a8_t>( a8 ) ),
		_a9( yaal::forward<a9_t>( a9 ) ), _a10( yaal::forward<a10_t>( a10 ) ) {
		return;
	}
	HCall( HCall const& ) = default;
	HCall( HCall&& ) noexcept = default;
	HCall& operator = ( HCall const& ) = default;
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3, fa4_t a4 ) {
		return ( (base_type::_call)(
				free_standing_call_args::resolve_arg_get( _a0, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg_get( _a1, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg_get( _a2, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg_get( _a3, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg_get( _a4, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg_get( _a5, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg_get( _a6, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg_get( _a7, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg_get( _a8, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg_get( _a9, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg_get( _a10, a0, a1, a2, a3, a4 ) ) ); }
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3, fa4_t a4 ) const { return ( (base_type::_call)(
				free_standing_call_args::resolve_arg_get( _a0, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg_get( _a1, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg_get( _a2, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg_get( _a3, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg_get( _a4, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg_get( _a5, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg_get( _a6, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg_get( _a7, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg_get( _a8, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg_get( _a9, a0, a1, a2, a3, a4 ),
				free_standing_call_args::resolve_arg_get( _a10, a0, a1, a2, a3, a4 ) ) ); }
};

template<typename descriptor, typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t>
class HCall<6, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t>
		: public HCallBase<6, descriptor> {
public:
	typedef HCall<6, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t> this_type;
	typedef HCallBase<6, descriptor> base_type;
private:
	typedef typename descriptor::arg0_t fa0_t;
	typedef typename descriptor::sorted_real_args::a1_t fa1_t;
	typedef typename descriptor::sorted_real_args::a2_t fa2_t;
	typedef typename descriptor::sorted_real_args::a3_t fa3_t;
	typedef typename descriptor::sorted_real_args::a4_t fa4_t;
	typedef typename descriptor::sorted_real_args::a5_t fa5_t;
	typename descriptor::v0_t _a0;
	typename descriptor::v1_t _a1;
	typename descriptor::v2_t _a2;
	typename descriptor::v3_t _a3;
	typename descriptor::v4_t _a4;
	typename descriptor::v5_t _a5;
public:
	HCall( CALL_t&& call_, a0_t&& a0, a1_t&& a1, a2_t&& a2, a3_t&& a3, a4_t&& a4, a5_t&& a5 )
		: base_type( yaal::forward<CALL_t>( call_ ) ), _a0( yaal::forward<a0_t>( a0 ) ),
		_a1( yaal::forward<a1_t>( a1 ) ), _a2( yaal::forward<a2_t>( a2 ) ),
		_a3( yaal::forward<a3_t>( a3 ) ), _a4( yaal::forward<a4_t>( a4 ) ), _a5( yaal::forward<a5_t>( a5 ) ) {
		return;
	}
	HCall( HCall const& ) = default;
	HCall( HCall&& ) noexcept = default;
	HCall& operator = ( HCall const& ) = default;
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3, fa4_t a4, fa5_t a5 ) {
		return ( (base_type::_call)(
				free_standing_call_args::resolve_arg_get( _a0, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg_get( _a1, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg_get( _a2, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg_get( _a3, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg_get( _a4, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg_get( _a5, a0, a1, a2, a3, a4, a5 ) ) ); }
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3, fa4_t a4, fa5_t a5 ) const { return ( (base_type::_call)(
				free_standing_call_args::resolve_arg_get( _a0, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg_get( _a1, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg_get( _a2, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg_get( _a3, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg_get( _a4, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg_get( _a5, a0, a1, a2, a3, a4, a5 ) ) ); }
};

template<typename descriptor, typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t, typename a6_t>
class HCall<6, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t>
		: public HCallBase<6, descriptor> {
public:
	typedef HCall<6, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t> this_type;
	typedef HCallBase<6, descriptor> base_type;
private:
	typedef typename descriptor::arg0_t fa0_t;
	typedef typename descriptor::sorted_real_args::a1_t fa1_t;
	typedef typename descriptor::sorted_real_args::a2_t fa2_t;
	typedef typename descriptor::sorted_real_args::a3_t fa3_t;
	typedef typename descriptor::sorted_real_args::a4_t fa4_t;
	typedef typename descriptor::sorted_real_args::a5_t fa5_t;
	typename descriptor::v0_t _a0;
	typename descriptor::v1_t _a1;
	typename descriptor::v2_t _a2;
	typename descriptor::v3_t _a3;
	typename descriptor::v4_t _a4;
	typename descriptor::v5_t _a5;
	typename descriptor::v6_t _a6;
public:
	HCall( CALL_t&& call_, a0_t&& a0, a1_t&& a1,
			a2_t a2, a3_t&& a3, a4_t&& a4, a5_t&& a5, a6_t&& a6 )
		: base_type( yaal::forward<CALL_t>( call_ ) ), _a0( yaal::forward<a0_t>( a0 ) ),
		_a1( yaal::forward<a1_t>( a1 ) ), _a2( yaal::forward<a2_t>( a2 ) ),
		_a3( yaal::forward<a3_t>( a3 ) ), _a4( yaal::forward<a4_t>( a4 ) ),
		_a5( yaal::forward<a5_t>( a5 ) ), _a6( yaal::forward<a6_t>( a6 ) ) {
		return;
	}
	HCall( HCall const& ) = default;
	HCall( HCall&& ) noexcept = default;
	HCall& operator = ( HCall const& ) = default;
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3, fa4_t a4, fa5_t a5 ) {
		return ( (base_type::_call)(
				free_standing_call_args::resolve_arg_get( _a0, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg_get( _a1, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg_get( _a2, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg_get( _a3, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg_get( _a4, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg_get( _a5, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg_get( _a6, a0, a1, a2, a3, a4, a5 ) ) ); }
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3, fa4_t a4, fa5_t a5 ) const { return ( (base_type::_call)(
				free_standing_call_args::resolve_arg_get( _a0, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg_get( _a1, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg_get( _a2, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg_get( _a3, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg_get( _a4, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg_get( _a5, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg_get( _a6, a0, a1, a2, a3, a4, a5 ) ) ); }
};

template<typename descriptor, typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t, typename a6_t, typename a7_t>
class HCall<6, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t>
		: public HCallBase<6, descriptor> {
public:
	typedef HCall<6, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t> this_type;
	typedef HCallBase<6, descriptor> base_type;
private:
	typedef typename descriptor::arg0_t fa0_t;
	typedef typename descriptor::sorted_real_args::a1_t fa1_t;
	typedef typename descriptor::sorted_real_args::a2_t fa2_t;
	typedef typename descriptor::sorted_real_args::a3_t fa3_t;
	typedef typename descriptor::sorted_real_args::a4_t fa4_t;
	typedef typename descriptor::sorted_real_args::a5_t fa5_t;
	typename descriptor::v0_t _a0;
	typename descriptor::v1_t _a1;
	typename descriptor::v2_t _a2;
	typename descriptor::v3_t _a3;
	typename descriptor::v4_t _a4;
	typename descriptor::v5_t _a5;
	typename descriptor::v6_t _a6;
	typename descriptor::v7_t _a7;
public:
	HCall( CALL_t&& call_, a0_t&& a0, a1_t&& a1,
			a2_t a2, a3_t&& a3, a4_t&& a4, a5_t&& a5, a6_t&& a6, a7_t&& a7 )
		: base_type( yaal::forward<CALL_t>( call_ ) ), _a0( yaal::forward<a0_t>( a0 ) ),
		_a1( yaal::forward<a1_t>( a1 ) ), _a2( yaal::forward<a2_t>( a2 ) ),
		_a3( yaal::forward<a3_t>( a3 ) ), _a4( yaal::forward<a4_t>( a4 ) ),
		_a5( yaal::forward<a5_t>( a5 ) ), _a6( yaal::forward<a6_t>( a6 ) ), _a7( yaal::forward<a7_t>( a7 ) ) {
		return;
	}
	HCall( HCall const& ) = default;
	HCall( HCall&& ) noexcept = default;
	HCall& operator = ( HCall const& ) = default;
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3, fa4_t a4, fa5_t a5 ) {
		return ( (base_type::_call)(
				free_standing_call_args::resolve_arg_get( _a0, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg_get( _a1, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg_get( _a2, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg_get( _a3, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg_get( _a4, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg_get( _a5, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg_get( _a6, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg_get( _a7, a0, a1, a2, a3, a4, a5 ) ) ); }
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3, fa4_t a4, fa5_t a5 ) const { return ( (base_type::_call)(
				free_standing_call_args::resolve_arg_get( _a0, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg_get( _a1, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg_get( _a2, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg_get( _a3, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg_get( _a4, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg_get( _a5, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg_get( _a6, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg_get( _a7, a0, a1, a2, a3, a4, a5 ) ) ); }
};

template<typename descriptor, typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t, typename a6_t, typename a7_t,
	typename a8_t>
class HCall<6, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t>
		: public HCallBase<6, descriptor> {
public:
	typedef HCall<6, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t> this_type;
	typedef HCallBase<6, descriptor> base_type;
private:
	typedef typename descriptor::arg0_t fa0_t;
	typedef typename descriptor::sorted_real_args::a1_t fa1_t;
	typedef typename descriptor::sorted_real_args::a2_t fa2_t;
	typedef typename descriptor::sorted_real_args::a3_t fa3_t;
	typedef typename descriptor::sorted_real_args::a4_t fa4_t;
	typedef typename descriptor::sorted_real_args::a5_t fa5_t;
	typename descriptor::v0_t _a0;
	typename descriptor::v1_t _a1;
	typename descriptor::v2_t _a2;
	typename descriptor::v3_t _a3;
	typename descriptor::v4_t _a4;
	typename descriptor::v5_t _a5;
	typename descriptor::v6_t _a6;
	typename descriptor::v7_t _a7;
	typename descriptor::v8_t _a8;
public:
	HCall( CALL_t&& call_, a0_t&& a0, a1_t&& a1, a2_t&& a2, a3_t&& a3, a4_t&& a4, a5_t&& a5, a6_t&& a6, a7_t&& a7, a8_t&& a8 )
		: base_type( yaal::forward<CALL_t>( call_ ) ), _a0( yaal::forward<a0_t>( a0 ) ),
		_a1( yaal::forward<a1_t>( a1 ) ), _a2( yaal::forward<a2_t>( a2 ) ),
		_a3( yaal::forward<a3_t>( a3 ) ), _a4( yaal::forward<a4_t>( a4 ) ),
		_a5( yaal::forward<a5_t>( a5 ) ), _a6( yaal::forward<a6_t>( a6 ) ),
		_a7( yaal::forward<a7_t>( a7 ) ), _a8( yaal::forward<a8_t>( a8 ) ) {
		return;
	}
	HCall( HCall const& ) = default;
	HCall( HCall&& ) noexcept = default;
	HCall& operator = ( HCall const& ) = default;
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3, fa4_t a4, fa5_t a5 ) {
		return ( (base_type::_call)(
				free_standing_call_args::resolve_arg_get( _a0, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg_get( _a1, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg_get( _a2, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg_get( _a3, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg_get( _a4, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg_get( _a5, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg_get( _a6, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg_get( _a7, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg_get( _a8, a0, a1, a2, a3, a4, a5 ) ) ); }
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3, fa4_t a4, fa5_t a5 ) const { return ( (base_type::_call)(
				free_standing_call_args::resolve_arg_get( _a0, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg_get( _a1, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg_get( _a2, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg_get( _a3, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg_get( _a4, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg_get( _a5, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg_get( _a6, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg_get( _a7, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg_get( _a8, a0, a1, a2, a3, a4, a5 ) ) ); }
};

template<typename descriptor, typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t, typename a6_t, typename a7_t,
	typename a8_t, typename a9_t>
class HCall<6, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t>
		: public HCallBase<6, descriptor> {
public:
	typedef HCall<6, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t> this_type;
	typedef HCallBase<6, descriptor> base_type;
private:
	typedef typename descriptor::arg0_t fa0_t;
	typedef typename descriptor::sorted_real_args::a1_t fa1_t;
	typedef typename descriptor::sorted_real_args::a2_t fa2_t;
	typedef typename descriptor::sorted_real_args::a3_t fa3_t;
	typedef typename descriptor::sorted_real_args::a4_t fa4_t;
	typedef typename descriptor::sorted_real_args::a5_t fa5_t;
	typename descriptor::v0_t _a0;
	typename descriptor::v1_t _a1;
	typename descriptor::v2_t _a2;
	typename descriptor::v3_t _a3;
	typename descriptor::v4_t _a4;
	typename descriptor::v5_t _a5;
	typename descriptor::v6_t _a6;
	typename descriptor::v7_t _a7;
	typename descriptor::v8_t _a8;
	typename descriptor::v9_t _a9;
public:
	HCall( CALL_t&& call_, a0_t&& a0, a1_t&& a1, a2_t&& a2, a3_t&& a3, a4_t&& a4, a5_t&& a5, a6_t&& a6, a7_t&& a7, a8_t&& a8, a9_t&& a9 )
		: base_type( yaal::forward<CALL_t>( call_ ) ), _a0( yaal::forward<a0_t>( a0 ) ),
		_a1( yaal::forward<a1_t>( a1 ) ), _a2( yaal::forward<a2_t>( a2 ) ),
		_a3( yaal::forward<a3_t>( a3 ) ), _a4( yaal::forward<a4_t>( a4 ) ),
		_a5( yaal::forward<a5_t>( a5 ) ), _a6( yaal::forward<a6_t>( a6 ) ),
		_a7( yaal::forward<a7_t>( a7 ) ), _a8( yaal::forward<a8_t>( a8 ) ), _a9( yaal::forward<a9_t>( a9 ) ) {
		return;
	}
	HCall( HCall const& ) = default;
	HCall( HCall&& ) noexcept = default;
	HCall& operator = ( HCall const& ) = default;
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3, fa4_t a4, fa5_t a5 ) {
		return ( (base_type::_call)(
				free_standing_call_args::resolve_arg_get( _a0, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg_get( _a1, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg_get( _a2, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg_get( _a3, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg_get( _a4, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg_get( _a5, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg_get( _a6, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg_get( _a7, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg_get( _a8, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg_get( _a9, a0, a1, a2, a3, a4, a5 ) ) ); }
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3, fa4_t a4, fa5_t a5 ) const { return ( (base_type::_call)(
				free_standing_call_args::resolve_arg_get( _a0, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg_get( _a1, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg_get( _a2, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg_get( _a3, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg_get( _a4, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg_get( _a5, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg_get( _a6, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg_get( _a7, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg_get( _a8, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg_get( _a9, a0, a1, a2, a3, a4, a5 ) ) ); }
};

template<typename descriptor, typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t, typename a6_t, typename a7_t,
	typename a8_t, typename a9_t, typename a10_t>
class HCall<6, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t, a10_t>
		: public HCallBase<6, descriptor> {
public:
	typedef HCall<6, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t, a10_t> this_type;
	typedef HCallBase<6, descriptor> base_type;
private:
	typedef typename descriptor::arg0_t fa0_t;
	typedef typename descriptor::sorted_real_args::a1_t fa1_t;
	typedef typename descriptor::sorted_real_args::a2_t fa2_t;
	typedef typename descriptor::sorted_real_args::a3_t fa3_t;
	typedef typename descriptor::sorted_real_args::a4_t fa4_t;
	typedef typename descriptor::sorted_real_args::a5_t fa5_t;
	typename descriptor::v0_t _a0;
	typename descriptor::v1_t _a1;
	typename descriptor::v2_t _a2;
	typename descriptor::v3_t _a3;
	typename descriptor::v4_t _a4;
	typename descriptor::v5_t _a5;
	typename descriptor::v6_t _a6;
	typename descriptor::v7_t _a7;
	typename descriptor::v8_t _a8;
	typename descriptor::v9_t _a9;
	typename descriptor::v10_t _a10;
public:
	HCall( CALL_t&& call_, a0_t&& a0, a1_t&& a1, a2_t&& a2, a3_t&& a3, a4_t&& a4, a5_t&& a5, a6_t&& a6, a7_t&& a7, a8_t&& a8, a9_t&& a9, a10_t&& a10 )
		: base_type( yaal::forward<CALL_t>( call_ ) ), _a0( yaal::forward<a0_t>( a0 ) ),
		_a1( yaal::forward<a1_t>( a1 ) ), _a2( yaal::forward<a2_t>( a2 ) ),
		_a3( yaal::forward<a3_t>( a3 ) ), _a4( yaal::forward<a4_t>( a4 ) ),
		_a5( yaal::forward<a5_t>( a5 ) ), _a6( yaal::forward<a6_t>( a6 ) ),
		_a7( yaal::forward<a7_t>( a7 ) ), _a8( yaal::forward<a8_t>( a8 ) ),
		_a9( yaal::forward<a9_t>( a9 ) ), _a10( yaal::forward<a10_t>( a10 ) ) {
		return;
	}
	HCall( HCall const& ) = default;
	HCall( HCall&& ) noexcept = default;
	HCall& operator = ( HCall const& ) = default;
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3, fa4_t a4, fa5_t a5 ) {
		return ( (base_type::_call)(
				free_standing_call_args::resolve_arg_get( _a0, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg_get( _a1, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg_get( _a2, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg_get( _a3, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg_get( _a4, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg_get( _a5, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg_get( _a6, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg_get( _a7, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg_get( _a8, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg_get( _a9, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg_get( _a10, a0, a1, a2, a3, a4, a5 ) ) ); }
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3, fa4_t a4, fa5_t a5 ) const { return ( (base_type::_call)(
				free_standing_call_args::resolve_arg_get( _a0, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg_get( _a1, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg_get( _a2, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg_get( _a3, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg_get( _a4, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg_get( _a5, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg_get( _a6, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg_get( _a7, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg_get( _a8, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg_get( _a9, a0, a1, a2, a3, a4, a5 ),
				free_standing_call_args::resolve_arg_get( _a10, a0, a1, a2, a3, a4, a5 ) ) ); }
};

template<typename descriptor, typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t, typename a6_t>
class HCall<7, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t>
		: public HCallBase<7, descriptor> {
public:
	typedef HCall<7, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t> this_type;
	typedef HCallBase<7, descriptor> base_type;
private:
	typedef typename descriptor::arg0_t fa0_t;
	typedef typename descriptor::sorted_real_args::a1_t fa1_t;
	typedef typename descriptor::sorted_real_args::a2_t fa2_t;
	typedef typename descriptor::sorted_real_args::a3_t fa3_t;
	typedef typename descriptor::sorted_real_args::a4_t fa4_t;
	typedef typename descriptor::sorted_real_args::a5_t fa5_t;
	typedef typename descriptor::sorted_real_args::a6_t fa6_t;
	typename descriptor::v0_t _a0;
	typename descriptor::v1_t _a1;
	typename descriptor::v2_t _a2;
	typename descriptor::v3_t _a3;
	typename descriptor::v4_t _a4;
	typename descriptor::v5_t _a5;
	typename descriptor::v6_t _a6;
public:
	HCall( CALL_t&& call_, a0_t&& a0, a1_t&& a1,
			a2_t a2, a3_t&& a3, a4_t&& a4, a5_t&& a5, a6_t&& a6 )
		: base_type( yaal::forward<CALL_t>( call_ ) ), _a0( yaal::forward<a0_t>( a0 ) ),
		_a1( yaal::forward<a1_t>( a1 ) ), _a2( yaal::forward<a2_t>( a2 ) ),
		_a3( yaal::forward<a3_t>( a3 ) ), _a4( yaal::forward<a4_t>( a4 ) ),
		_a5( yaal::forward<a5_t>( a5 ) ), _a6( yaal::forward<a6_t>( a6 ) ) {
		return;
	}
	HCall( HCall const& ) = default;
	HCall( HCall&& ) noexcept = default;
	HCall& operator = ( HCall const& ) = default;
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3, fa4_t a4, fa5_t a5, fa6_t a6 ) {
		return ( (base_type::_call)(
				free_standing_call_args::resolve_arg_get( _a0, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg_get( _a1, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg_get( _a2, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg_get( _a3, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg_get( _a4, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg_get( _a5, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg_get( _a6, a0, a1, a2, a3, a4, a5, a6 ) ) ); }
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3, fa4_t a4, fa5_t a5, fa6_t a6 ) const { return ( (base_type::_call)(
				free_standing_call_args::resolve_arg_get( _a0, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg_get( _a1, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg_get( _a2, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg_get( _a3, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg_get( _a4, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg_get( _a5, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg_get( _a6, a0, a1, a2, a3, a4, a5, a6 ) ) ); }
};

template<typename descriptor, typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t, typename a6_t, typename a7_t>
class HCall<7, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t>
		: public HCallBase<7, descriptor> {
public:
	typedef HCall<7, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t> this_type;
	typedef HCallBase<7, descriptor> base_type;
private:
	typedef typename descriptor::arg0_t fa0_t;
	typedef typename descriptor::sorted_real_args::a1_t fa1_t;
	typedef typename descriptor::sorted_real_args::a2_t fa2_t;
	typedef typename descriptor::sorted_real_args::a3_t fa3_t;
	typedef typename descriptor::sorted_real_args::a4_t fa4_t;
	typedef typename descriptor::sorted_real_args::a5_t fa5_t;
	typedef typename descriptor::sorted_real_args::a6_t fa6_t;
	typename descriptor::v0_t _a0;
	typename descriptor::v1_t _a1;
	typename descriptor::v2_t _a2;
	typename descriptor::v3_t _a3;
	typename descriptor::v4_t _a4;
	typename descriptor::v5_t _a5;
	typename descriptor::v6_t _a6;
	typename descriptor::v7_t _a7;
public:
	HCall( CALL_t&& call_, a0_t&& a0, a1_t&& a1,
			a2_t a2, a3_t&& a3, a4_t&& a4, a5_t&& a5, a6_t&& a6, a7_t&& a7 )
		: base_type( yaal::forward<CALL_t>( call_ ) ), _a0( yaal::forward<a0_t>( a0 ) ),
		_a1( yaal::forward<a1_t>( a1 ) ), _a2( yaal::forward<a2_t>( a2 ) ),
		_a3( yaal::forward<a3_t>( a3 ) ), _a4( yaal::forward<a4_t>( a4 ) ),
		_a5( yaal::forward<a5_t>( a5 ) ), _a6( yaal::forward<a6_t>( a6 ) ), _a7( yaal::forward<a7_t>( a7 ) ) {
		return;
	}
	HCall( HCall const& ) = default;
	HCall( HCall&& ) noexcept = default;
	HCall& operator = ( HCall const& ) = default;
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3, fa4_t a4, fa5_t a5, fa6_t a6 ) {
		return ( (base_type::_call)(
				free_standing_call_args::resolve_arg_get( _a0, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg_get( _a1, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg_get( _a2, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg_get( _a3, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg_get( _a4, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg_get( _a5, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg_get( _a6, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg_get( _a7, a0, a1, a2, a3, a4, a5, a6 ) ) ); }
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3, fa4_t a4, fa5_t a5, fa6_t a6 ) const { return ( (base_type::_call)(
				free_standing_call_args::resolve_arg_get( _a0, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg_get( _a1, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg_get( _a2, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg_get( _a3, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg_get( _a4, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg_get( _a5, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg_get( _a6, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg_get( _a7, a0, a1, a2, a3, a4, a5, a6 ) ) ); }
};

template<typename descriptor, typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t, typename a6_t, typename a7_t,
	typename a8_t>
class HCall<7, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t>
		: public HCallBase<7, descriptor> {
public:
	typedef HCall<7, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t> this_type;
	typedef HCallBase<7, descriptor> base_type;
private:
	typedef typename descriptor::arg0_t fa0_t;
	typedef typename descriptor::sorted_real_args::a1_t fa1_t;
	typedef typename descriptor::sorted_real_args::a2_t fa2_t;
	typedef typename descriptor::sorted_real_args::a3_t fa3_t;
	typedef typename descriptor::sorted_real_args::a4_t fa4_t;
	typedef typename descriptor::sorted_real_args::a5_t fa5_t;
	typedef typename descriptor::sorted_real_args::a6_t fa6_t;
	typename descriptor::v0_t _a0;
	typename descriptor::v1_t _a1;
	typename descriptor::v2_t _a2;
	typename descriptor::v3_t _a3;
	typename descriptor::v4_t _a4;
	typename descriptor::v5_t _a5;
	typename descriptor::v6_t _a6;
	typename descriptor::v7_t _a7;
	typename descriptor::v8_t _a8;
public:
	HCall( CALL_t&& call_, a0_t&& a0, a1_t&& a1, a2_t&& a2, a3_t&& a3, a4_t&& a4, a5_t&& a5, a6_t&& a6, a7_t&& a7, a8_t&& a8 )
		: base_type( yaal::forward<CALL_t>( call_ ) ), _a0( yaal::forward<a0_t>( a0 ) ),
		_a1( yaal::forward<a1_t>( a1 ) ), _a2( yaal::forward<a2_t>( a2 ) ),
		_a3( yaal::forward<a3_t>( a3 ) ), _a4( yaal::forward<a4_t>( a4 ) ),
		_a5( yaal::forward<a5_t>( a5 ) ), _a6( yaal::forward<a6_t>( a6 ) ),
		_a7( yaal::forward<a7_t>( a7 ) ), _a8( yaal::forward<a8_t>( a8 ) ) {
		return;
	}
	HCall( HCall const& ) = default;
	HCall( HCall&& ) noexcept = default;
	HCall& operator = ( HCall const& ) = default;
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3, fa4_t a4, fa5_t a5, fa6_t a6 ) {
		return ( (base_type::_call)(
				free_standing_call_args::resolve_arg_get( _a0, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg_get( _a1, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg_get( _a2, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg_get( _a3, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg_get( _a4, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg_get( _a5, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg_get( _a6, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg_get( _a7, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg_get( _a8, a0, a1, a2, a3, a4, a5, a6 ) ) ); }
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3, fa4_t a4, fa5_t a5, fa6_t a6 ) const { return ( (base_type::_call)(
				free_standing_call_args::resolve_arg_get( _a0, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg_get( _a1, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg_get( _a2, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg_get( _a3, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg_get( _a4, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg_get( _a5, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg_get( _a6, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg_get( _a7, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg_get( _a8, a0, a1, a2, a3, a4, a5, a6 ) ) ); }
};

template<typename descriptor, typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t, typename a6_t, typename a7_t,
	typename a8_t, typename a9_t>
class HCall<7, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t>
		: public HCallBase<7, descriptor> {
public:
	typedef HCall<7, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t> this_type;
	typedef HCallBase<7, descriptor> base_type;
private:
	typedef typename descriptor::arg0_t fa0_t;
	typedef typename descriptor::sorted_real_args::a1_t fa1_t;
	typedef typename descriptor::sorted_real_args::a2_t fa2_t;
	typedef typename descriptor::sorted_real_args::a3_t fa3_t;
	typedef typename descriptor::sorted_real_args::a4_t fa4_t;
	typedef typename descriptor::sorted_real_args::a5_t fa5_t;
	typedef typename descriptor::sorted_real_args::a6_t fa6_t;
	typename descriptor::v0_t _a0;
	typename descriptor::v1_t _a1;
	typename descriptor::v2_t _a2;
	typename descriptor::v3_t _a3;
	typename descriptor::v4_t _a4;
	typename descriptor::v5_t _a5;
	typename descriptor::v6_t _a6;
	typename descriptor::v7_t _a7;
	typename descriptor::v8_t _a8;
	typename descriptor::v9_t _a9;
public:
	HCall( CALL_t&& call_, a0_t&& a0, a1_t&& a1, a2_t&& a2, a3_t&& a3, a4_t&& a4, a5_t&& a5, a6_t&& a6, a7_t&& a7, a8_t&& a8, a9_t&& a9 )
		: base_type( yaal::forward<CALL_t>( call_ ) ), _a0( yaal::forward<a0_t>( a0 ) ),
		_a1( yaal::forward<a1_t>( a1 ) ), _a2( yaal::forward<a2_t>( a2 ) ),
		_a3( yaal::forward<a3_t>( a3 ) ), _a4( yaal::forward<a4_t>( a4 ) ),
		_a5( yaal::forward<a5_t>( a5 ) ), _a6( yaal::forward<a6_t>( a6 ) ),
		_a7( yaal::forward<a7_t>( a7 ) ), _a8( yaal::forward<a8_t>( a8 ) ), _a9( yaal::forward<a9_t>( a9 ) ) {
		return;
	}
	HCall( HCall const& ) = default;
	HCall( HCall&& ) noexcept = default;
	HCall& operator = ( HCall const& ) = default;
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3, fa4_t a4, fa5_t a5, fa6_t a6 ) {
		return ( (base_type::_call)(
				free_standing_call_args::resolve_arg_get( _a0, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg_get( _a1, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg_get( _a2, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg_get( _a3, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg_get( _a4, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg_get( _a5, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg_get( _a6, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg_get( _a7, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg_get( _a8, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg_get( _a9, a0, a1, a2, a3, a4, a5, a6 ) ) ); }
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3, fa4_t a4, fa5_t a5, fa6_t a6 ) const { return ( (base_type::_call)(
				free_standing_call_args::resolve_arg_get( _a0, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg_get( _a1, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg_get( _a2, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg_get( _a3, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg_get( _a4, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg_get( _a5, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg_get( _a6, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg_get( _a7, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg_get( _a8, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg_get( _a9, a0, a1, a2, a3, a4, a5, a6 ) ) ); }
};

template<typename descriptor, typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t, typename a6_t, typename a7_t,
	typename a8_t, typename a9_t, typename a10_t>
class HCall<7, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t, a10_t>
		: public HCallBase<7, descriptor> {
public:
	typedef HCall<7, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t, a10_t> this_type;
	typedef HCallBase<7, descriptor> base_type;
private:
	typedef typename descriptor::arg0_t fa0_t;
	typedef typename descriptor::sorted_real_args::a1_t fa1_t;
	typedef typename descriptor::sorted_real_args::a2_t fa2_t;
	typedef typename descriptor::sorted_real_args::a3_t fa3_t;
	typedef typename descriptor::sorted_real_args::a4_t fa4_t;
	typedef typename descriptor::sorted_real_args::a5_t fa5_t;
	typedef typename descriptor::sorted_real_args::a6_t fa6_t;
	typename descriptor::v0_t _a0;
	typename descriptor::v1_t _a1;
	typename descriptor::v2_t _a2;
	typename descriptor::v3_t _a3;
	typename descriptor::v4_t _a4;
	typename descriptor::v5_t _a5;
	typename descriptor::v6_t _a6;
	typename descriptor::v7_t _a7;
	typename descriptor::v8_t _a8;
	typename descriptor::v9_t _a9;
	typename descriptor::v10_t _a10;
public:
	HCall( CALL_t&& call_, a0_t&& a0, a1_t&& a1, a2_t&& a2, a3_t&& a3, a4_t&& a4, a5_t&& a5, a6_t&& a6, a7_t&& a7, a8_t&& a8, a9_t&& a9, a10_t&& a10 )
		: base_type( yaal::forward<CALL_t>( call_ ) ), _a0( yaal::forward<a0_t>( a0 ) ),
		_a1( yaal::forward<a1_t>( a1 ) ), _a2( yaal::forward<a2_t>( a2 ) ),
		_a3( yaal::forward<a3_t>( a3 ) ), _a4( yaal::forward<a4_t>( a4 ) ),
		_a5( yaal::forward<a5_t>( a5 ) ), _a6( yaal::forward<a6_t>( a6 ) ),
		_a7( yaal::forward<a7_t>( a7 ) ), _a8( yaal::forward<a8_t>( a8 ) ),
		_a9( yaal::forward<a9_t>( a9 ) ), _a10( yaal::forward<a10_t>( a10 ) ) {
		return;
	}
	HCall( HCall const& ) = default;
	HCall( HCall&& ) noexcept = default;
	HCall& operator = ( HCall const& ) = default;
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3, fa4_t a4, fa5_t a5, fa6_t a6 ) {
		return ( (base_type::_call)(
				free_standing_call_args::resolve_arg_get( _a0, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg_get( _a1, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg_get( _a2, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg_get( _a3, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg_get( _a4, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg_get( _a5, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg_get( _a6, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg_get( _a7, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg_get( _a8, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg_get( _a9, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg_get( _a10, a0, a1, a2, a3, a4, a5, a6 ) ) ); }
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3, fa4_t a4, fa5_t a5, fa6_t a6 ) const { return ( (base_type::_call)(
				free_standing_call_args::resolve_arg_get( _a0, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg_get( _a1, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg_get( _a2, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg_get( _a3, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg_get( _a4, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg_get( _a5, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg_get( _a6, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg_get( _a7, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg_get( _a8, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg_get( _a9, a0, a1, a2, a3, a4, a5, a6 ),
				free_standing_call_args::resolve_arg_get( _a10, a0, a1, a2, a3, a4, a5, a6 ) ) ); }
};

template<typename descriptor, typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t, typename a6_t, typename a7_t>
class HCall<8, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t>
		: public HCallBase<8, descriptor> {
public:
	typedef HCall<8, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t> this_type;
	typedef HCallBase<8, descriptor> base_type;
private:
	typedef typename descriptor::arg0_t fa0_t;
	typedef typename descriptor::sorted_real_args::a1_t fa1_t;
	typedef typename descriptor::sorted_real_args::a2_t fa2_t;
	typedef typename descriptor::sorted_real_args::a3_t fa3_t;
	typedef typename descriptor::sorted_real_args::a4_t fa4_t;
	typedef typename descriptor::sorted_real_args::a5_t fa5_t;
	typedef typename descriptor::sorted_real_args::a6_t fa6_t;
	typedef typename descriptor::sorted_real_args::a7_t fa7_t;
	typename descriptor::v0_t _a0;
	typename descriptor::v1_t _a1;
	typename descriptor::v2_t _a2;
	typename descriptor::v3_t _a3;
	typename descriptor::v4_t _a4;
	typename descriptor::v5_t _a5;
	typename descriptor::v6_t _a6;
	typename descriptor::v7_t _a7;
public:
	HCall( CALL_t&& call_, a0_t&& a0, a1_t&& a1,
			a2_t a2, a3_t&& a3, a4_t&& a4, a5_t&& a5, a6_t&& a6, a7_t&& a7 )
		: base_type( yaal::forward<CALL_t>( call_ ) ), _a0( yaal::forward<a0_t>( a0 ) ),
		_a1( yaal::forward<a1_t>( a1 ) ), _a2( yaal::forward<a2_t>( a2 ) ),
		_a3( yaal::forward<a3_t>( a3 ) ), _a4( yaal::forward<a4_t>( a4 ) ),
		_a5( yaal::forward<a5_t>( a5 ) ), _a6( yaal::forward<a6_t>( a6 ) ), _a7( yaal::forward<a7_t>( a7 ) ) {
		return;
	}
	HCall( HCall const& ) = default;
	HCall( HCall&& ) noexcept = default;
	HCall& operator = ( HCall const& ) = default;
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3, fa4_t a4, fa5_t a5, fa6_t a6, fa7_t a7 ) {
		return ( (base_type::_call)(
				free_standing_call_args::resolve_arg_get( _a0, a0, a1, a2, a3, a4, a5, a6, a7 ),
				free_standing_call_args::resolve_arg_get( _a1, a0, a1, a2, a3, a4, a5, a6, a7 ),
				free_standing_call_args::resolve_arg_get( _a2, a0, a1, a2, a3, a4, a5, a6, a7 ),
				free_standing_call_args::resolve_arg_get( _a3, a0, a1, a2, a3, a4, a5, a6, a7 ),
				free_standing_call_args::resolve_arg_get( _a4, a0, a1, a2, a3, a4, a5, a6, a7 ),
				free_standing_call_args::resolve_arg_get( _a5, a0, a1, a2, a3, a4, a5, a6, a7 ),
				free_standing_call_args::resolve_arg_get( _a6, a0, a1, a2, a3, a4, a5, a6, a7 ),
				free_standing_call_args::resolve_arg_get( _a7, a0, a1, a2, a3, a4, a5, a6, a7 ) ) ); }
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3, fa4_t a4, fa5_t a5, fa6_t a6, fa7_t a7 ) const { return ( (base_type::_call)(
				free_standing_call_args::resolve_arg_get( _a0, a0, a1, a2, a3, a4, a5, a6, a7 ),
				free_standing_call_args::resolve_arg_get( _a1, a0, a1, a2, a3, a4, a5, a6, a7 ),
				free_standing_call_args::resolve_arg_get( _a2, a0, a1, a2, a3, a4, a5, a6, a7 ),
				free_standing_call_args::resolve_arg_get( _a3, a0, a1, a2, a3, a4, a5, a6, a7 ),
				free_standing_call_args::resolve_arg_get( _a4, a0, a1, a2, a3, a4, a5, a6, a7 ),
				free_standing_call_args::resolve_arg_get( _a5, a0, a1, a2, a3, a4, a5, a6, a7 ),
				free_standing_call_args::resolve_arg_get( _a6, a0, a1, a2, a3, a4, a5, a6, a7 ),
				free_standing_call_args::resolve_arg_get( _a7, a0, a1, a2, a3, a4, a5, a6, a7 ) ) ); }
};

template<typename descriptor, typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t, typename a6_t, typename a7_t,
	typename a8_t>
class HCall<8, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t>
		: public HCallBase<8, descriptor> {
public:
	typedef HCall<8, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t> this_type;
	typedef HCallBase<8, descriptor> base_type;
private:
	typedef typename descriptor::arg0_t fa0_t;
	typedef typename descriptor::sorted_real_args::a1_t fa1_t;
	typedef typename descriptor::sorted_real_args::a2_t fa2_t;
	typedef typename descriptor::sorted_real_args::a3_t fa3_t;
	typedef typename descriptor::sorted_real_args::a4_t fa4_t;
	typedef typename descriptor::sorted_real_args::a5_t fa5_t;
	typedef typename descriptor::sorted_real_args::a6_t fa6_t;
	typedef typename descriptor::sorted_real_args::a7_t fa7_t;
	typename descriptor::v0_t _a0;
	typename descriptor::v1_t _a1;
	typename descriptor::v2_t _a2;
	typename descriptor::v3_t _a3;
	typename descriptor::v4_t _a4;
	typename descriptor::v5_t _a5;
	typename descriptor::v6_t _a6;
	typename descriptor::v7_t _a7;
	typename descriptor::v8_t _a8;
public:
	HCall( CALL_t&& call_, a0_t&& a0, a1_t&& a1, a2_t&& a2, a3_t&& a3, a4_t&& a4, a5_t&& a5, a6_t&& a6, a7_t&& a7, a8_t&& a8 )
		: base_type( yaal::forward<CALL_t>( call_ ) ), _a0( yaal::forward<a0_t>( a0 ) ),
		_a1( yaal::forward<a1_t>( a1 ) ), _a2( yaal::forward<a2_t>( a2 ) ),
		_a3( yaal::forward<a3_t>( a3 ) ), _a4( yaal::forward<a4_t>( a4 ) ),
		_a5( yaal::forward<a5_t>( a5 ) ), _a6( yaal::forward<a6_t>( a6 ) ),
		_a7( yaal::forward<a7_t>( a7 ) ), _a8( yaal::forward<a8_t>( a8 ) ) {
		return;
	}
	HCall( HCall const& ) = default;
	HCall( HCall&& ) noexcept = default;
	HCall& operator = ( HCall const& ) = default;
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3, fa4_t a4, fa5_t a5, fa6_t a6, fa7_t a7 ) {
		return ( (base_type::_call)(
				free_standing_call_args::resolve_arg_get( _a0, a0, a1, a2, a3, a4, a5, a6, a7 ),
				free_standing_call_args::resolve_arg_get( _a1, a0, a1, a2, a3, a4, a5, a6, a7 ),
				free_standing_call_args::resolve_arg_get( _a2, a0, a1, a2, a3, a4, a5, a6, a7 ),
				free_standing_call_args::resolve_arg_get( _a3, a0, a1, a2, a3, a4, a5, a6, a7 ),
				free_standing_call_args::resolve_arg_get( _a4, a0, a1, a2, a3, a4, a5, a6, a7 ),
				free_standing_call_args::resolve_arg_get( _a5, a0, a1, a2, a3, a4, a5, a6, a7 ),
				free_standing_call_args::resolve_arg_get( _a6, a0, a1, a2, a3, a4, a5, a6, a7 ),
				free_standing_call_args::resolve_arg_get( _a7, a0, a1, a2, a3, a4, a5, a6, a7 ),
				free_standing_call_args::resolve_arg_get( _a8, a0, a1, a2, a3, a4, a5, a6, a7 ) ) ); }
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3, fa4_t a4, fa5_t a5, fa6_t a6, fa7_t a7 ) const { return ( (base_type::_call)(
				free_standing_call_args::resolve_arg_get( _a0, a0, a1, a2, a3, a4, a5, a6, a7 ),
				free_standing_call_args::resolve_arg_get( _a1, a0, a1, a2, a3, a4, a5, a6, a7 ),
				free_standing_call_args::resolve_arg_get( _a2, a0, a1, a2, a3, a4, a5, a6, a7 ),
				free_standing_call_args::resolve_arg_get( _a3, a0, a1, a2, a3, a4, a5, a6, a7 ),
				free_standing_call_args::resolve_arg_get( _a4, a0, a1, a2, a3, a4, a5, a6, a7 ),
				free_standing_call_args::resolve_arg_get( _a5, a0, a1, a2, a3, a4, a5, a6, a7 ),
				free_standing_call_args::resolve_arg_get( _a6, a0, a1, a2, a3, a4, a5, a6, a7 ),
				free_standing_call_args::resolve_arg_get( _a7, a0, a1, a2, a3, a4, a5, a6, a7 ),
				free_standing_call_args::resolve_arg_get( _a8, a0, a1, a2, a3, a4, a5, a6, a7 ) ) ); }
};

template<typename descriptor, typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t, typename a6_t, typename a7_t,
	typename a8_t, typename a9_t>
class HCall<8, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t>
		: public HCallBase<8, descriptor> {
public:
	typedef HCall<8, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t> this_type;
	typedef HCallBase<8, descriptor> base_type;
private:
	typedef typename descriptor::arg0_t fa0_t;
	typedef typename descriptor::sorted_real_args::a1_t fa1_t;
	typedef typename descriptor::sorted_real_args::a2_t fa2_t;
	typedef typename descriptor::sorted_real_args::a3_t fa3_t;
	typedef typename descriptor::sorted_real_args::a4_t fa4_t;
	typedef typename descriptor::sorted_real_args::a5_t fa5_t;
	typedef typename descriptor::sorted_real_args::a6_t fa6_t;
	typedef typename descriptor::sorted_real_args::a7_t fa7_t;
	typename descriptor::v0_t _a0;
	typename descriptor::v1_t _a1;
	typename descriptor::v2_t _a2;
	typename descriptor::v3_t _a3;
	typename descriptor::v4_t _a4;
	typename descriptor::v5_t _a5;
	typename descriptor::v6_t _a6;
	typename descriptor::v7_t _a7;
	typename descriptor::v8_t _a8;
	typename descriptor::v9_t _a9;
public:
	HCall( CALL_t&& call_, a0_t&& a0, a1_t&& a1, a2_t&& a2, a3_t&& a3, a4_t&& a4, a5_t&& a5, a6_t&& a6, a7_t&& a7, a8_t&& a8, a9_t&& a9 )
		: base_type( yaal::forward<CALL_t>( call_ ) ), _a0( yaal::forward<a0_t>( a0 ) ),
		_a1( yaal::forward<a1_t>( a1 ) ), _a2( yaal::forward<a2_t>( a2 ) ),
		_a3( yaal::forward<a3_t>( a3 ) ), _a4( yaal::forward<a4_t>( a4 ) ),
		_a5( yaal::forward<a5_t>( a5 ) ), _a6( yaal::forward<a6_t>( a6 ) ),
		_a7( yaal::forward<a7_t>( a7 ) ), _a8( yaal::forward<a8_t>( a8 ) ), _a9( yaal::forward<a9_t>( a9 ) ) {
		return;
	}
	HCall( HCall const& ) = default;
	HCall( HCall&& ) noexcept = default;
	HCall& operator = ( HCall const& ) = default;
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3, fa4_t a4, fa5_t a5, fa6_t a6, fa7_t a7 ) {
		return ( (base_type::_call)(
				free_standing_call_args::resolve_arg_get( _a0, a0, a1, a2, a3, a4, a5, a6, a7 ),
				free_standing_call_args::resolve_arg_get( _a1, a0, a1, a2, a3, a4, a5, a6, a7 ),
				free_standing_call_args::resolve_arg_get( _a2, a0, a1, a2, a3, a4, a5, a6, a7 ),
				free_standing_call_args::resolve_arg_get( _a3, a0, a1, a2, a3, a4, a5, a6, a7 ),
				free_standing_call_args::resolve_arg_get( _a4, a0, a1, a2, a3, a4, a5, a6, a7 ),
				free_standing_call_args::resolve_arg_get( _a5, a0, a1, a2, a3, a4, a5, a6, a7 ),
				free_standing_call_args::resolve_arg_get( _a6, a0, a1, a2, a3, a4, a5, a6, a7 ),
				free_standing_call_args::resolve_arg_get( _a7, a0, a1, a2, a3, a4, a5, a6, a7 ),
				free_standing_call_args::resolve_arg_get( _a8, a0, a1, a2, a3, a4, a5, a6, a7 ),
				free_standing_call_args::resolve_arg_get( _a9, a0, a1, a2, a3, a4, a5, a6, a7 ) ) ); }
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3, fa4_t a4, fa5_t a5, fa6_t a6, fa7_t a7 ) const { return ( (base_type::_call)(
				free_standing_call_args::resolve_arg_get( _a0, a0, a1, a2, a3, a4, a5, a6, a7 ),
				free_standing_call_args::resolve_arg_get( _a1, a0, a1, a2, a3, a4, a5, a6, a7 ),
				free_standing_call_args::resolve_arg_get( _a2, a0, a1, a2, a3, a4, a5, a6, a7 ),
				free_standing_call_args::resolve_arg_get( _a3, a0, a1, a2, a3, a4, a5, a6, a7 ),
				free_standing_call_args::resolve_arg_get( _a4, a0, a1, a2, a3, a4, a5, a6, a7 ),
				free_standing_call_args::resolve_arg_get( _a5, a0, a1, a2, a3, a4, a5, a6, a7 ),
				free_standing_call_args::resolve_arg_get( _a6, a0, a1, a2, a3, a4, a5, a6, a7 ),
				free_standing_call_args::resolve_arg_get( _a7, a0, a1, a2, a3, a4, a5, a6, a7 ),
				free_standing_call_args::resolve_arg_get( _a8, a0, a1, a2, a3, a4, a5, a6, a7 ),
				free_standing_call_args::resolve_arg_get( _a9, a0, a1, a2, a3, a4, a5, a6, a7 ) ) ); }
};

template<typename descriptor, typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t, typename a6_t, typename a7_t,
	typename a8_t, typename a9_t, typename a10_t>
class HCall<8, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t, a10_t>
		: public HCallBase<8, descriptor> {
public:
	typedef HCall<8, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t, a10_t> this_type;
	typedef HCallBase<8, descriptor> base_type;
private:
	typedef typename descriptor::arg0_t fa0_t;
	typedef typename descriptor::sorted_real_args::a1_t fa1_t;
	typedef typename descriptor::sorted_real_args::a2_t fa2_t;
	typedef typename descriptor::sorted_real_args::a3_t fa3_t;
	typedef typename descriptor::sorted_real_args::a4_t fa4_t;
	typedef typename descriptor::sorted_real_args::a5_t fa5_t;
	typedef typename descriptor::sorted_real_args::a6_t fa6_t;
	typedef typename descriptor::sorted_real_args::a7_t fa7_t;
	typename descriptor::v0_t _a0;
	typename descriptor::v1_t _a1;
	typename descriptor::v2_t _a2;
	typename descriptor::v3_t _a3;
	typename descriptor::v4_t _a4;
	typename descriptor::v5_t _a5;
	typename descriptor::v6_t _a6;
	typename descriptor::v7_t _a7;
	typename descriptor::v8_t _a8;
	typename descriptor::v9_t _a9;
	typename descriptor::v10_t _a10;
public:
	HCall( CALL_t&& call_, a0_t&& a0, a1_t&& a1, a2_t&& a2, a3_t&& a3, a4_t&& a4, a5_t&& a5, a6_t&& a6, a7_t&& a7, a8_t&& a8, a9_t&& a9, a10_t&& a10 )
		: base_type( yaal::forward<CALL_t>( call_ ) ), _a0( yaal::forward<a0_t>( a0 ) ),
		_a1( yaal::forward<a1_t>( a1 ) ), _a2( yaal::forward<a2_t>( a2 ) ),
		_a3( yaal::forward<a3_t>( a3 ) ), _a4( yaal::forward<a4_t>( a4 ) ),
		_a5( yaal::forward<a5_t>( a5 ) ), _a6( yaal::forward<a6_t>( a6 ) ),
		_a7( yaal::forward<a7_t>( a7 ) ), _a8( yaal::forward<a8_t>( a8 ) ),
		_a9( yaal::forward<a9_t>( a9 ) ), _a10( yaal::forward<a10_t>( a10 ) ) {
		return;
	}
	HCall( HCall const& ) = default;
	HCall( HCall&& ) noexcept = default;
	HCall& operator = ( HCall const& ) = default;
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3, fa4_t a4, fa5_t a5, fa6_t a6, fa7_t a7 ) {
		return ( (base_type::_call)(
				free_standing_call_args::resolve_arg_get( _a0, a0, a1, a2, a3, a4, a5, a6, a7 ),
				free_standing_call_args::resolve_arg_get( _a1, a0, a1, a2, a3, a4, a5, a6, a7 ),
				free_standing_call_args::resolve_arg_get( _a2, a0, a1, a2, a3, a4, a5, a6, a7 ),
				free_standing_call_args::resolve_arg_get( _a3, a0, a1, a2, a3, a4, a5, a6, a7 ),
				free_standing_call_args::resolve_arg_get( _a4, a0, a1, a2, a3, a4, a5, a6, a7 ),
				free_standing_call_args::resolve_arg_get( _a5, a0, a1, a2, a3, a4, a5, a6, a7 ),
				free_standing_call_args::resolve_arg_get( _a6, a0, a1, a2, a3, a4, a5, a6, a7 ),
				free_standing_call_args::resolve_arg_get( _a7, a0, a1, a2, a3, a4, a5, a6, a7 ),
				free_standing_call_args::resolve_arg_get( _a8, a0, a1, a2, a3, a4, a5, a6, a7 ),
				free_standing_call_args::resolve_arg_get( _a9, a0, a1, a2, a3, a4, a5, a6, a7 ),
				free_standing_call_args::resolve_arg_get( _a10, a0, a1, a2, a3, a4, a5, a6, a7 ) ) ); }
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3, fa4_t a4, fa5_t a5, fa6_t a6, fa7_t a7 ) const { return ( (base_type::_call)(
				free_standing_call_args::resolve_arg_get( _a0, a0, a1, a2, a3, a4, a5, a6, a7 ),
				free_standing_call_args::resolve_arg_get( _a1, a0, a1, a2, a3, a4, a5, a6, a7 ),
				free_standing_call_args::resolve_arg_get( _a2, a0, a1, a2, a3, a4, a5, a6, a7 ),
				free_standing_call_args::resolve_arg_get( _a3, a0, a1, a2, a3, a4, a5, a6, a7 ),
				free_standing_call_args::resolve_arg_get( _a4, a0, a1, a2, a3, a4, a5, a6, a7 ),
				free_standing_call_args::resolve_arg_get( _a5, a0, a1, a2, a3, a4, a5, a6, a7 ),
				free_standing_call_args::resolve_arg_get( _a6, a0, a1, a2, a3, a4, a5, a6, a7 ),
				free_standing_call_args::resolve_arg_get( _a7, a0, a1, a2, a3, a4, a5, a6, a7 ),
				free_standing_call_args::resolve_arg_get( _a8, a0, a1, a2, a3, a4, a5, a6, a7 ),
				free_standing_call_args::resolve_arg_get( _a9, a0, a1, a2, a3, a4, a5, a6, a7 ),
				free_standing_call_args::resolve_arg_get( _a10, a0, a1, a2, a3, a4, a5, a6, a7 ) ) ); }
};

template<typename descriptor, typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t, typename a6_t, typename a7_t,
	typename a8_t>
class HCall<9, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t>
		: public HCallBase<9, descriptor> {
public:
	typedef HCall<9, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t> this_type;
	typedef HCallBase<9, descriptor> base_type;
private:
	typedef typename descriptor::arg0_t fa0_t;
	typedef typename descriptor::sorted_real_args::a1_t fa1_t;
	typedef typename descriptor::sorted_real_args::a2_t fa2_t;
	typedef typename descriptor::sorted_real_args::a3_t fa3_t;
	typedef typename descriptor::sorted_real_args::a4_t fa4_t;
	typedef typename descriptor::sorted_real_args::a5_t fa5_t;
	typedef typename descriptor::sorted_real_args::a6_t fa6_t;
	typedef typename descriptor::sorted_real_args::a7_t fa7_t;
	typedef typename descriptor::sorted_real_args::a8_t fa8_t;
	typename descriptor::v0_t _a0;
	typename descriptor::v1_t _a1;
	typename descriptor::v2_t _a2;
	typename descriptor::v3_t _a3;
	typename descriptor::v4_t _a4;
	typename descriptor::v5_t _a5;
	typename descriptor::v6_t _a6;
	typename descriptor::v7_t _a7;
	typename descriptor::v8_t _a8;
public:
	HCall( CALL_t&& call_, a0_t&& a0, a1_t&& a1, a2_t&& a2, a3_t&& a3, a4_t&& a4, a5_t&& a5, a6_t&& a6, a7_t&& a7, a8_t&& a8 )
		: base_type( yaal::forward<CALL_t>( call_ ) ), _a0( yaal::forward<a0_t>( a0 ) ),
		_a1( yaal::forward<a1_t>( a1 ) ), _a2( yaal::forward<a2_t>( a2 ) ),
		_a3( yaal::forward<a3_t>( a3 ) ), _a4( yaal::forward<a4_t>( a4 ) ),
		_a5( yaal::forward<a5_t>( a5 ) ), _a6( yaal::forward<a6_t>( a6 ) ),
		_a7( yaal::forward<a7_t>( a7 ) ), _a8( yaal::forward<a8_t>( a8 ) ) {
		return;
	}
	HCall( HCall const& ) = default;
	HCall( HCall&& ) noexcept = default;
	HCall& operator = ( HCall const& ) = default;
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3, fa4_t a4, fa5_t a5, fa6_t a6, fa7_t a7, fa8_t a8 ) {
		return ( (base_type::_call)(
				free_standing_call_args::resolve_arg_get( _a0, a0, a1, a2, a3, a4, a5, a6, a7, a8 ),
				free_standing_call_args::resolve_arg_get( _a1, a0, a1, a2, a3, a4, a5, a6, a7, a8 ),
				free_standing_call_args::resolve_arg_get( _a2, a0, a1, a2, a3, a4, a5, a6, a7, a8 ),
				free_standing_call_args::resolve_arg_get( _a3, a0, a1, a2, a3, a4, a5, a6, a7, a8 ),
				free_standing_call_args::resolve_arg_get( _a4, a0, a1, a2, a3, a4, a5, a6, a7, a8 ),
				free_standing_call_args::resolve_arg_get( _a5, a0, a1, a2, a3, a4, a5, a6, a7, a8 ),
				free_standing_call_args::resolve_arg_get( _a6, a0, a1, a2, a3, a4, a5, a6, a7, a8 ),
				free_standing_call_args::resolve_arg_get( _a7, a0, a1, a2, a3, a4, a5, a6, a7, a8 ),
				free_standing_call_args::resolve_arg_get( _a8, a0, a1, a2, a3, a4, a5, a6, a7, a8 ) ) ); }
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3, fa4_t a4, fa5_t a5, fa6_t a6, fa7_t a7, fa8_t a8 ) const { return ( (base_type::_call)(
				free_standing_call_args::resolve_arg_get( _a0, a0, a1, a2, a3, a4, a5, a6, a7, a8 ),
				free_standing_call_args::resolve_arg_get( _a1, a0, a1, a2, a3, a4, a5, a6, a7, a8 ),
				free_standing_call_args::resolve_arg_get( _a2, a0, a1, a2, a3, a4, a5, a6, a7, a8 ),
				free_standing_call_args::resolve_arg_get( _a3, a0, a1, a2, a3, a4, a5, a6, a7, a8 ),
				free_standing_call_args::resolve_arg_get( _a4, a0, a1, a2, a3, a4, a5, a6, a7, a8 ),
				free_standing_call_args::resolve_arg_get( _a5, a0, a1, a2, a3, a4, a5, a6, a7, a8 ),
				free_standing_call_args::resolve_arg_get( _a6, a0, a1, a2, a3, a4, a5, a6, a7, a8 ),
				free_standing_call_args::resolve_arg_get( _a7, a0, a1, a2, a3, a4, a5, a6, a7, a8 ),
				free_standing_call_args::resolve_arg_get( _a8, a0, a1, a2, a3, a4, a5, a6, a7, a8 ) ) ); }
};

template<typename descriptor, typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t, typename a6_t, typename a7_t,
	typename a8_t, typename a9_t>
class HCall<9, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t>
		: public HCallBase<9, descriptor> {
public:
	typedef HCall<9, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t> this_type;
	typedef HCallBase<9, descriptor> base_type;
private:
	typedef typename descriptor::arg0_t fa0_t;
	typedef typename descriptor::sorted_real_args::a1_t fa1_t;
	typedef typename descriptor::sorted_real_args::a2_t fa2_t;
	typedef typename descriptor::sorted_real_args::a3_t fa3_t;
	typedef typename descriptor::sorted_real_args::a4_t fa4_t;
	typedef typename descriptor::sorted_real_args::a5_t fa5_t;
	typedef typename descriptor::sorted_real_args::a6_t fa6_t;
	typedef typename descriptor::sorted_real_args::a7_t fa7_t;
	typedef typename descriptor::sorted_real_args::a8_t fa8_t;
	typename descriptor::v0_t _a0;
	typename descriptor::v1_t _a1;
	typename descriptor::v2_t _a2;
	typename descriptor::v3_t _a3;
	typename descriptor::v4_t _a4;
	typename descriptor::v5_t _a5;
	typename descriptor::v6_t _a6;
	typename descriptor::v7_t _a7;
	typename descriptor::v8_t _a8;
	typename descriptor::v9_t _a9;
public:
	HCall( CALL_t&& call_, a0_t&& a0, a1_t&& a1, a2_t&& a2, a3_t&& a3, a4_t&& a4, a5_t&& a5, a6_t&& a6, a7_t&& a7, a8_t&& a8, a9_t&& a9 )
		: base_type( yaal::forward<CALL_t>( call_ ) ), _a0( yaal::forward<a0_t>( a0 ) ),
		_a1( yaal::forward<a1_t>( a1 ) ), _a2( yaal::forward<a2_t>( a2 ) ),
		_a3( yaal::forward<a3_t>( a3 ) ), _a4( yaal::forward<a4_t>( a4 ) ),
		_a5( yaal::forward<a5_t>( a5 ) ), _a6( yaal::forward<a6_t>( a6 ) ),
		_a7( yaal::forward<a7_t>( a7 ) ), _a8( yaal::forward<a8_t>( a8 ) ), _a9( yaal::forward<a9_t>( a9 ) ) {
		return;
	}
	HCall( HCall const& ) = default;
	HCall( HCall&& ) noexcept = default;
	HCall& operator = ( HCall const& ) = default;
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3, fa4_t a4, fa5_t a5, fa6_t a6, fa7_t a7, fa8_t a8 ) {
		return ( (base_type::_call)(
				free_standing_call_args::resolve_arg_get( _a0, a0, a1, a2, a3, a4, a5, a6, a7, a8 ),
				free_standing_call_args::resolve_arg_get( _a1, a0, a1, a2, a3, a4, a5, a6, a7, a8 ),
				free_standing_call_args::resolve_arg_get( _a2, a0, a1, a2, a3, a4, a5, a6, a7, a8 ),
				free_standing_call_args::resolve_arg_get( _a3, a0, a1, a2, a3, a4, a5, a6, a7, a8 ),
				free_standing_call_args::resolve_arg_get( _a4, a0, a1, a2, a3, a4, a5, a6, a7, a8 ),
				free_standing_call_args::resolve_arg_get( _a5, a0, a1, a2, a3, a4, a5, a6, a7, a8 ),
				free_standing_call_args::resolve_arg_get( _a6, a0, a1, a2, a3, a4, a5, a6, a7, a8 ),
				free_standing_call_args::resolve_arg_get( _a7, a0, a1, a2, a3, a4, a5, a6, a7, a8 ),
				free_standing_call_args::resolve_arg_get( _a8, a0, a1, a2, a3, a4, a5, a6, a7, a8 ),
				free_standing_call_args::resolve_arg_get( _a9, a0, a1, a2, a3, a4, a5, a6, a7, a8 ) ) ); }
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3, fa4_t a4, fa5_t a5, fa6_t a6, fa7_t a7, fa8_t a8 ) const { return ( (base_type::_call)(
				free_standing_call_args::resolve_arg_get( _a0, a0, a1, a2, a3, a4, a5, a6, a7, a8 ),
				free_standing_call_args::resolve_arg_get( _a1, a0, a1, a2, a3, a4, a5, a6, a7, a8 ),
				free_standing_call_args::resolve_arg_get( _a2, a0, a1, a2, a3, a4, a5, a6, a7, a8 ),
				free_standing_call_args::resolve_arg_get( _a3, a0, a1, a2, a3, a4, a5, a6, a7, a8 ),
				free_standing_call_args::resolve_arg_get( _a4, a0, a1, a2, a3, a4, a5, a6, a7, a8 ),
				free_standing_call_args::resolve_arg_get( _a5, a0, a1, a2, a3, a4, a5, a6, a7, a8 ),
				free_standing_call_args::resolve_arg_get( _a6, a0, a1, a2, a3, a4, a5, a6, a7, a8 ),
				free_standing_call_args::resolve_arg_get( _a7, a0, a1, a2, a3, a4, a5, a6, a7, a8 ),
				free_standing_call_args::resolve_arg_get( _a8, a0, a1, a2, a3, a4, a5, a6, a7, a8 ),
				free_standing_call_args::resolve_arg_get( _a9, a0, a1, a2, a3, a4, a5, a6, a7, a8 ) ) ); }
};

template<typename descriptor, typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t, typename a6_t, typename a7_t,
	typename a8_t, typename a9_t, typename a10_t>
class HCall<9, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t, a10_t>
		: public HCallBase<9, descriptor> {
public:
	typedef HCall<9, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t, a10_t> this_type;
	typedef HCallBase<9, descriptor> base_type;
private:
	typedef typename descriptor::arg0_t fa0_t;
	typedef typename descriptor::sorted_real_args::a1_t fa1_t;
	typedef typename descriptor::sorted_real_args::a2_t fa2_t;
	typedef typename descriptor::sorted_real_args::a3_t fa3_t;
	typedef typename descriptor::sorted_real_args::a4_t fa4_t;
	typedef typename descriptor::sorted_real_args::a5_t fa5_t;
	typedef typename descriptor::sorted_real_args::a6_t fa6_t;
	typedef typename descriptor::sorted_real_args::a7_t fa7_t;
	typedef typename descriptor::sorted_real_args::a8_t fa8_t;
	typename descriptor::v0_t _a0;
	typename descriptor::v1_t _a1;
	typename descriptor::v2_t _a2;
	typename descriptor::v3_t _a3;
	typename descriptor::v4_t _a4;
	typename descriptor::v5_t _a5;
	typename descriptor::v6_t _a6;
	typename descriptor::v7_t _a7;
	typename descriptor::v8_t _a8;
	typename descriptor::v9_t _a9;
	typename descriptor::v10_t _a10;
public:
	HCall( CALL_t&& call_, a0_t&& a0, a1_t&& a1, a2_t&& a2, a3_t&& a3, a4_t&& a4, a5_t&& a5, a6_t&& a6, a7_t&& a7, a8_t&& a8, a9_t&& a9, a10_t&& a10 )
		: base_type( yaal::forward<CALL_t>( call_ ) ), _a0( yaal::forward<a0_t>( a0 ) ),
		_a1( yaal::forward<a1_t>( a1 ) ), _a2( yaal::forward<a2_t>( a2 ) ),
		_a3( yaal::forward<a3_t>( a3 ) ), _a4( yaal::forward<a4_t>( a4 ) ),
		_a5( yaal::forward<a5_t>( a5 ) ), _a6( yaal::forward<a6_t>( a6 ) ),
		_a7( yaal::forward<a7_t>( a7 ) ), _a8( yaal::forward<a8_t>( a8 ) ),
		_a9( yaal::forward<a9_t>( a9 ) ), _a10( yaal::forward<a10_t>( a10 ) ) {
		return;
	}
	HCall( HCall const& ) = default;
	HCall( HCall&& ) noexcept = default;
	HCall& operator = ( HCall const& ) = default;
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3, fa4_t a4, fa5_t a5, fa6_t a6, fa7_t a7, fa8_t a8 ) {
		return ( (base_type::_call)(
				free_standing_call_args::resolve_arg_get( _a0, a0, a1, a2, a3, a4, a5, a6, a7, a8 ),
				free_standing_call_args::resolve_arg_get( _a1, a0, a1, a2, a3, a4, a5, a6, a7, a8 ),
				free_standing_call_args::resolve_arg_get( _a2, a0, a1, a2, a3, a4, a5, a6, a7, a8 ),
				free_standing_call_args::resolve_arg_get( _a3, a0, a1, a2, a3, a4, a5, a6, a7, a8 ),
				free_standing_call_args::resolve_arg_get( _a4, a0, a1, a2, a3, a4, a5, a6, a7, a8 ),
				free_standing_call_args::resolve_arg_get( _a5, a0, a1, a2, a3, a4, a5, a6, a7, a8 ),
				free_standing_call_args::resolve_arg_get( _a6, a0, a1, a2, a3, a4, a5, a6, a7, a8 ),
				free_standing_call_args::resolve_arg_get( _a7, a0, a1, a2, a3, a4, a5, a6, a7, a8 ),
				free_standing_call_args::resolve_arg_get( _a8, a0, a1, a2, a3, a4, a5, a6, a7, a8 ),
				free_standing_call_args::resolve_arg_get( _a9, a0, a1, a2, a3, a4, a5, a6, a7, a8 ),
				free_standing_call_args::resolve_arg_get( _a10, a0, a1, a2, a3, a4, a5, a6, a7, a8 ) ) ); }
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3, fa4_t a4, fa5_t a5, fa6_t a6, fa7_t a7, fa8_t a8 ) const { return ( (base_type::_call)(
				free_standing_call_args::resolve_arg_get( _a0, a0, a1, a2, a3, a4, a5, a6, a7, a8 ),
				free_standing_call_args::resolve_arg_get( _a1, a0, a1, a2, a3, a4, a5, a6, a7, a8 ),
				free_standing_call_args::resolve_arg_get( _a2, a0, a1, a2, a3, a4, a5, a6, a7, a8 ),
				free_standing_call_args::resolve_arg_get( _a3, a0, a1, a2, a3, a4, a5, a6, a7, a8 ),
				free_standing_call_args::resolve_arg_get( _a4, a0, a1, a2, a3, a4, a5, a6, a7, a8 ),
				free_standing_call_args::resolve_arg_get( _a5, a0, a1, a2, a3, a4, a5, a6, a7, a8 ),
				free_standing_call_args::resolve_arg_get( _a6, a0, a1, a2, a3, a4, a5, a6, a7, a8 ),
				free_standing_call_args::resolve_arg_get( _a7, a0, a1, a2, a3, a4, a5, a6, a7, a8 ),
				free_standing_call_args::resolve_arg_get( _a8, a0, a1, a2, a3, a4, a5, a6, a7, a8 ),
				free_standing_call_args::resolve_arg_get( _a9, a0, a1, a2, a3, a4, a5, a6, a7, a8 ),
				free_standing_call_args::resolve_arg_get( _a10, a0, a1, a2, a3, a4, a5, a6, a7, a8 ) ) ); }
};

template<typename descriptor, typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t, typename a6_t, typename a7_t,
	typename a8_t, typename a9_t>
class HCall<10, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t>
		: public HCallBase<10, descriptor> {
public:
	typedef HCall<10, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t> this_type;
	typedef HCallBase<10, descriptor> base_type;
private:
	typedef typename descriptor::arg0_t fa0_t;
	typedef typename descriptor::sorted_real_args::a1_t fa1_t;
	typedef typename descriptor::sorted_real_args::a2_t fa2_t;
	typedef typename descriptor::sorted_real_args::a3_t fa3_t;
	typedef typename descriptor::sorted_real_args::a4_t fa4_t;
	typedef typename descriptor::sorted_real_args::a5_t fa5_t;
	typedef typename descriptor::sorted_real_args::a6_t fa6_t;
	typedef typename descriptor::sorted_real_args::a7_t fa7_t;
	typedef typename descriptor::sorted_real_args::a8_t fa8_t;
	typedef typename descriptor::sorted_real_args::a9_t fa9_t;
	typename descriptor::v0_t _a0;
	typename descriptor::v1_t _a1;
	typename descriptor::v2_t _a2;
	typename descriptor::v3_t _a3;
	typename descriptor::v4_t _a4;
	typename descriptor::v5_t _a5;
	typename descriptor::v6_t _a6;
	typename descriptor::v7_t _a7;
	typename descriptor::v8_t _a8;
	typename descriptor::v9_t _a9;
public:
	HCall( CALL_t&& call_, a0_t&& a0, a1_t&& a1, a2_t&& a2, a3_t&& a3, a4_t&& a4, a5_t&& a5, a6_t&& a6, a7_t&& a7, a8_t&& a8, a9_t&& a9 )
		: base_type( yaal::forward<CALL_t>( call_ ) ), _a0( yaal::forward<a0_t>( a0 ) ),
		_a1( yaal::forward<a1_t>( a1 ) ), _a2( yaal::forward<a2_t>( a2 ) ),
		_a3( yaal::forward<a3_t>( a3 ) ), _a4( yaal::forward<a4_t>( a4 ) ),
		_a5( yaal::forward<a5_t>( a5 ) ), _a6( yaal::forward<a6_t>( a6 ) ),
		_a7( yaal::forward<a7_t>( a7 ) ), _a8( yaal::forward<a8_t>( a8 ) ), _a9( yaal::forward<a9_t>( a9 ) ) {
		return;
	}
	HCall( HCall const& ) = default;
	HCall( HCall&& ) noexcept = default;
	HCall& operator = ( HCall const& ) = default;
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3, fa4_t a4, fa5_t a5, fa6_t a6, fa7_t a7, fa8_t a8, fa9_t a9 ) {
		return ( (base_type::_call)(
				free_standing_call_args::resolve_arg_get( _a0, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9 ),
				free_standing_call_args::resolve_arg_get( _a1, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9 ),
				free_standing_call_args::resolve_arg_get( _a2, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9 ),
				free_standing_call_args::resolve_arg_get( _a3, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9 ),
				free_standing_call_args::resolve_arg_get( _a4, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9 ),
				free_standing_call_args::resolve_arg_get( _a5, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9 ),
				free_standing_call_args::resolve_arg_get( _a6, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9 ),
				free_standing_call_args::resolve_arg_get( _a7, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9 ),
				free_standing_call_args::resolve_arg_get( _a8, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9 ),
				free_standing_call_args::resolve_arg_get( _a9, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9 ) ) ); }
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3, fa4_t a4, fa5_t a5, fa6_t a6, fa7_t a7, fa8_t a8, fa9_t a9 ) const { return ( (base_type::_call)(
				free_standing_call_args::resolve_arg_get( _a0, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9 ),
				free_standing_call_args::resolve_arg_get( _a1, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9 ),
				free_standing_call_args::resolve_arg_get( _a2, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9 ),
				free_standing_call_args::resolve_arg_get( _a3, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9 ),
				free_standing_call_args::resolve_arg_get( _a4, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9 ),
				free_standing_call_args::resolve_arg_get( _a5, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9 ),
				free_standing_call_args::resolve_arg_get( _a6, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9 ),
				free_standing_call_args::resolve_arg_get( _a7, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9 ),
				free_standing_call_args::resolve_arg_get( _a8, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9 ),
				free_standing_call_args::resolve_arg_get( _a9, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9 ) ) ); }
};

template<typename descriptor, typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t, typename a6_t, typename a7_t,
	typename a8_t, typename a9_t, typename a10_t>
class HCall<10, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t, a10_t>
		: public HCallBase<10, descriptor> {
public:
	typedef HCall<10, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t, a10_t> this_type;
	typedef HCallBase<10, descriptor> base_type;
private:
	typedef typename descriptor::arg0_t fa0_t;
	typedef typename descriptor::sorted_real_args::a1_t fa1_t;
	typedef typename descriptor::sorted_real_args::a2_t fa2_t;
	typedef typename descriptor::sorted_real_args::a3_t fa3_t;
	typedef typename descriptor::sorted_real_args::a4_t fa4_t;
	typedef typename descriptor::sorted_real_args::a5_t fa5_t;
	typedef typename descriptor::sorted_real_args::a6_t fa6_t;
	typedef typename descriptor::sorted_real_args::a7_t fa7_t;
	typedef typename descriptor::sorted_real_args::a8_t fa8_t;
	typedef typename descriptor::sorted_real_args::a9_t fa9_t;
	typename descriptor::v0_t _a0;
	typename descriptor::v1_t _a1;
	typename descriptor::v2_t _a2;
	typename descriptor::v3_t _a3;
	typename descriptor::v4_t _a4;
	typename descriptor::v5_t _a5;
	typename descriptor::v6_t _a6;
	typename descriptor::v7_t _a7;
	typename descriptor::v8_t _a8;
	typename descriptor::v9_t _a9;
	typename descriptor::v10_t _a10;
public:
	HCall( CALL_t&& call_, a0_t&& a0, a1_t&& a1, a2_t&& a2, a3_t&& a3, a4_t&& a4, a5_t&& a5, a6_t&& a6, a7_t&& a7, a8_t&& a8, a9_t&& a9, a10_t&& a10 )
		: base_type( yaal::forward<CALL_t>( call_ ) ), _a0( yaal::forward<a0_t>( a0 ) ),
		_a1( yaal::forward<a1_t>( a1 ) ), _a2( yaal::forward<a2_t>( a2 ) ),
		_a3( yaal::forward<a3_t>( a3 ) ), _a4( yaal::forward<a4_t>( a4 ) ),
		_a5( yaal::forward<a5_t>( a5 ) ), _a6( yaal::forward<a6_t>( a6 ) ),
		_a7( yaal::forward<a7_t>( a7 ) ), _a8( yaal::forward<a8_t>( a8 ) ),
		_a9( yaal::forward<a9_t>( a9 ) ), _a10( yaal::forward<a10_t>( a10 ) ) {
		return;
	}
	HCall( HCall const& ) = default;
	HCall( HCall&& ) noexcept = default;
	HCall& operator = ( HCall const& ) = default;
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3, fa4_t a4, fa5_t a5, fa6_t a6, fa7_t a7, fa8_t a8, fa9_t a9 ) {
		return ( (base_type::_call)(
				free_standing_call_args::resolve_arg_get( _a0, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9 ),
				free_standing_call_args::resolve_arg_get( _a1, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9 ),
				free_standing_call_args::resolve_arg_get( _a2, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9 ),
				free_standing_call_args::resolve_arg_get( _a3, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9 ),
				free_standing_call_args::resolve_arg_get( _a4, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9 ),
				free_standing_call_args::resolve_arg_get( _a5, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9 ),
				free_standing_call_args::resolve_arg_get( _a6, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9 ),
				free_standing_call_args::resolve_arg_get( _a7, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9 ),
				free_standing_call_args::resolve_arg_get( _a8, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9 ),
				free_standing_call_args::resolve_arg_get( _a9, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9 ),
				free_standing_call_args::resolve_arg_get( _a10, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9 ) ) ); }
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3, fa4_t a4, fa5_t a5, fa6_t a6, fa7_t a7, fa8_t a8, fa9_t a9 ) const { return ( (base_type::_call)(
				free_standing_call_args::resolve_arg_get( _a0, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9 ),
				free_standing_call_args::resolve_arg_get( _a1, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9 ),
				free_standing_call_args::resolve_arg_get( _a2, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9 ),
				free_standing_call_args::resolve_arg_get( _a3, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9 ),
				free_standing_call_args::resolve_arg_get( _a4, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9 ),
				free_standing_call_args::resolve_arg_get( _a5, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9 ),
				free_standing_call_args::resolve_arg_get( _a6, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9 ),
				free_standing_call_args::resolve_arg_get( _a7, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9 ),
				free_standing_call_args::resolve_arg_get( _a8, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9 ),
				free_standing_call_args::resolve_arg_get( _a9, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9 ),
				free_standing_call_args::resolve_arg_get( _a10, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9 ) ) ); }
};

template<typename descriptor, typename return_t, typename CALL_t,
	typename a0_t, typename a1_t, typename a2_t, typename a3_t,
	typename a4_t, typename a5_t, typename a6_t, typename a7_t,
	typename a8_t, typename a9_t, typename a10_t>
class HCall<11, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t, a10_t>
		: public HCallBase<11, descriptor> {
public:
	typedef HCall<11, descriptor, return_t, CALL_t, a0_t, a1_t, a2_t, a3_t, a4_t, a5_t, a6_t, a7_t, a8_t, a9_t, a10_t> this_type;
	typedef HCallBase<11, descriptor> base_type;
private:
	typedef typename descriptor::arg0_t fa0_t;
	typedef typename descriptor::sorted_real_args::a1_t fa1_t;
	typedef typename descriptor::sorted_real_args::a2_t fa2_t;
	typedef typename descriptor::sorted_real_args::a3_t fa3_t;
	typedef typename descriptor::sorted_real_args::a4_t fa4_t;
	typedef typename descriptor::sorted_real_args::a5_t fa5_t;
	typedef typename descriptor::sorted_real_args::a6_t fa6_t;
	typedef typename descriptor::sorted_real_args::a7_t fa7_t;
	typedef typename descriptor::sorted_real_args::a8_t fa8_t;
	typedef typename descriptor::sorted_real_args::a9_t fa9_t;
	typedef typename descriptor::sorted_real_args::a10_t fa10_t;
	typename descriptor::v0_t _a0;
	typename descriptor::v1_t _a1;
	typename descriptor::v2_t _a2;
	typename descriptor::v3_t _a3;
	typename descriptor::v4_t _a4;
	typename descriptor::v5_t _a5;
	typename descriptor::v6_t _a6;
	typename descriptor::v7_t _a7;
	typename descriptor::v8_t _a8;
	typename descriptor::v9_t _a9;
	typename descriptor::v10_t _a10;
public:
	HCall( CALL_t&& call_, a0_t&& a0, a1_t&& a1, a2_t&& a2, a3_t&& a3, a4_t&& a4, a5_t&& a5, a6_t&& a6, a7_t&& a7, a8_t&& a8, a9_t&& a9, a10_t&& a10 )
		: base_type( yaal::forward<CALL_t>( call_ ) ), _a0( yaal::forward<a0_t>( a0 ) ),
		_a1( yaal::forward<a1_t>( a1 ) ), _a2( yaal::forward<a2_t>( a2 ) ),
		_a3( yaal::forward<a3_t>( a3 ) ), _a4( yaal::forward<a4_t>( a4 ) ),
		_a5( yaal::forward<a5_t>( a5 ) ), _a6( yaal::forward<a6_t>( a6 ) ),
		_a7( yaal::forward<a7_t>( a7 ) ), _a8( yaal::forward<a8_t>( a8 ) ),
		_a9( yaal::forward<a9_t>( a9 ) ), _a10( yaal::forward<a10_t>( a10 ) ) {
		return;
	}
	HCall( HCall const& ) = default;
	HCall( HCall&& ) noexcept = default;
	HCall& operator = ( HCall const& ) = default;
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3, fa4_t a4, fa5_t a5, fa6_t a6, fa7_t a7, fa8_t a8, fa9_t a9, fa10_t a10 ) {
		return ( (base_type::_call)(
				free_standing_call_args::resolve_arg_get( _a0, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10 ),
				free_standing_call_args::resolve_arg_get( _a1, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10 ),
				free_standing_call_args::resolve_arg_get( _a2, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10 ),
				free_standing_call_args::resolve_arg_get( _a3, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10 ),
				free_standing_call_args::resolve_arg_get( _a4, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10 ),
				free_standing_call_args::resolve_arg_get( _a5, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10 ),
				free_standing_call_args::resolve_arg_get( _a6, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10 ),
				free_standing_call_args::resolve_arg_get( _a7, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10 ),
				free_standing_call_args::resolve_arg_get( _a8, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10 ),
				free_standing_call_args::resolve_arg_get( _a9, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10 ),
				free_standing_call_args::resolve_arg_get( _a10, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10 ) ) ); }
	virtual return_t do_invoke( fa0_t a0, fa1_t a1, fa2_t a2, fa3_t a3, fa4_t a4, fa5_t a5, fa6_t a6, fa7_t a7, fa8_t a8, fa9_t a9, fa10_t a10 ) const { return ( (base_type::_call)(
				free_standing_call_args::resolve_arg_get( _a0, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10 ),
				free_standing_call_args::resolve_arg_get( _a1, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10 ),
				free_standing_call_args::resolve_arg_get( _a2, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10 ),
				free_standing_call_args::resolve_arg_get( _a3, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10 ),
				free_standing_call_args::resolve_arg_get( _a4, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10 ),
				free_standing_call_args::resolve_arg_get( _a5, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10 ),
				free_standing_call_args::resolve_arg_get( _a6, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10 ),
				free_standing_call_args::resolve_arg_get( _a7, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10 ),
				free_standing_call_args::resolve_arg_get( _a8, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10 ),
				free_standing_call_args::resolve_arg_get( _a9, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10 ),
				free_standing_call_args::resolve_arg_get( _a10, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10 ) ) ); }
};

/*! \endcond */

template<typename METHOD_t, typename... arg_t>
typename call_calculator<METHOD_t, arg_t...>::type::type call( METHOD_t&& A_METHOD, arg_t&&... arg_ ) {
	return ( call_calculator<METHOD_t, arg_t...>::type::make( yaal::forward<METHOD_t>( A_METHOD ), yaal::forward<arg_t>( arg_ )... ) );
}

}

}

#endif /* #ifndef YAAL_HCORE_HCALL_HXX_INCLUDED */

