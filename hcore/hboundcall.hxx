/* Read yaal/LICENSE.md file for copyright and licensing information. */
/*! \file hcore/boundcall.hxx
 * \brief Declaration and implementation of HBoundCall<> template.
 */

#ifndef YAAL_HCORE_HBOUNDCALL_HXX_INCLUDED
#define YAAL_HCORE_HBOUNDCALL_HXX_INCLUDED 1

#include <functional>

#include "hcore/hcall.hxx"
#include "hcore/hpointer.hxx"
#include "hcore/memory.hxx"
#include "hcore/hvariant.hxx"

namespace yaal {

namespace hcore {

namespace defaults {

typedef void simple_call( void );

}

template<typename signature_t = defaults::simple_call>
class HBoundCall {
public:
	typedef HBoundCall<signature_t> this_type;
	typedef typename trait::return_type<signature_t>::type result_type;
	typedef typename trait::argument_type<signature_t, 0>::type argument_type;
	typedef typename trait::argument_type<signature_t, 0>::type first_argument_type;
	typedef typename trait::argument_type<signature_t, 1>::type second_argument_type;
	typedef typename trait::argument_type<signature_t, 2>::type third_argument_type;
	typedef typename trait::argument_type<signature_t, 3>::type fourth_argument_type;
	typedef typename trait::argument_type<signature_t, 4>::type fifth_argument_type;
	typedef typename trait::argument_type<signature_t, 5>::type sixth_argument_type;
	typedef typename trait::argument_type<signature_t, 6>::type seventh_argument_type;
	typedef typename trait::argument_type<signature_t, 7>::type eighth_argument_type;
	typedef typename trait::argument_type<signature_t, 8>::type ninth_argument_type;
	typedef typename trait::argument_type<signature_t, 9>::type tenth_argument_type;
	typedef typename trait::argument_type<signature_t, 10>::type eleventh_argument_type;
	typedef HCallInterface<trait::argument_count<signature_t>::value,
					result_type,
					first_argument_type,
					second_argument_type,
					third_argument_type,
					fourth_argument_type,
					fifth_argument_type,
					sixth_argument_type,
					seventh_argument_type,
					eighth_argument_type,
					ninth_argument_type,
					tenth_argument_type,
					eleventh_argument_type
					> call_t;
	typedef HPointer<call_t> call_ptr_t;
	typedef std::function<signature_t> function_t;
	typedef HPointer<function_t> function_ptr_t;
	typedef HVariant<call_ptr_t, function_ptr_t> call_fwd_t;
private:
	call_fwd_t _call;
public:
	HBoundCall( void )
		: _call() {
		return;
	}
	template<int free_args_count, typename return_t, typename CALL_type,
		typename b0_t, typename b1_t,
		typename b2_t, typename b3_t,
		typename b4_t, typename b5_t,
		typename b6_t, typename b7_t,
		typename b8_t, typename b9_t,
		typename b10_t>
	HBoundCall( HCall<free_args_count, return_t, CALL_type, b0_t, b1_t, b2_t, b3_t, b4_t, b5_t, b6_t, b7_t, b8_t, b9_t, b10_t> const& rawCall_ )
		: _call(
				pointer_static_cast<call_t>(
					make_pointer<
						HCall<
							free_args_count, return_t, CALL_type,
							b0_t, b1_t, b2_t, b3_t, b4_t, b5_t,
							b6_t, b7_t, b8_t, b9_t, b10_t
						>
					>( rawCall_ )
				)
			) {
		return;
	}
	template<int free_args_count, typename return_t, typename CALL_type,
		typename b0_t, typename b1_t,
		typename b2_t, typename b3_t,
		typename b4_t, typename b5_t,
		typename b6_t, typename b7_t,
		typename b8_t, typename b9_t,
		typename b10_t>
	HBoundCall( HCall<free_args_count, return_t, CALL_type, b0_t, b1_t, b2_t, b3_t, b4_t, b5_t, b6_t, b7_t, b8_t, b9_t, b10_t>&& rawCall_ )
		: _call(
				pointer_static_cast<call_t>(
					make_pointer<
						HCall<
							free_args_count, return_t, CALL_type,
							b0_t, b1_t, b2_t, b3_t, b4_t, b5_t,
							b6_t, b7_t, b8_t, b9_t, b10_t
						>
					>( yaal::move( rawCall_ ) )
				)
			) {
		return;
	}
	HBoundCall( function_t const& foreignCall_ )
		: _call( make_pointer<function_t>( foreignCall_ ) ) {
		return;
	}
	HBoundCall( function_t&& foreignCall_ ) noexcept
		: _call( make_pointer<function_t>( yaal::move( foreignCall_ ) ) ) {
		return;
	}
	HBoundCall( HBoundCall const& ) = default;
	HBoundCall& operator = ( HBoundCall const& ) = default;
	HBoundCall( HBoundCall&& ) noexcept = default;
	HBoundCall& operator = ( HBoundCall&& ) noexcept = default;
	template<typename... arg_t>
	result_type operator()( arg_t&&... arg_ ) const {
		int const t( _call.type() );
		M_ASSERT( ( t != call_fwd_t::INVALID ) && "invoked an unbound call" );
		if ( t == 0 ) {
			return ( (*_call.template get<call_ptr_t>())( yaal::forward<arg_t>( arg_ )... ) );
		}
		return ( (*_call.template get<function_ptr_t>())( yaal::forward<arg_t>( arg_ )... ) );
	}
	template<typename... arg_t>
	result_type operator()( arg_t&&... arg_ ) {
		int const t( _call.type() );
		M_ASSERT( ( t != call_fwd_t::INVALID ) && "invoked an unbound call" );
		if ( t == 0 ) {
			return ( (*_call.template get<call_ptr_t>())( yaal::forward<arg_t>( arg_ )... ) );
		}
		return ( (*_call.template get<function_ptr_t>())( yaal::forward<arg_t>( arg_ )... ) );
	}
	void const* id( void ) const {
		int const t( _call.type() );
		if ( t == 0 ) {
			return ( _call.template get<call_ptr_t>()->id() );
		} else if ( t == 1 ) {
			return ( _call.template get<function_ptr_t>().raw() );
		}
		return ( nullptr );
	}
	void reset( void ) {
		int const t( _call.type() );
		if ( t == 0 ) {
			_call.template get<call_ptr_t>().reset();
		} else if ( t == 1 ) {
			_call.template get<function_ptr_t>().reset();
		}
		return;
	}
	bool operator ! ( void ) const {
		int const t( _call.type() );
		if ( t == 0 ) {
			return ( ! _call.template get<call_ptr_t>() );
		} else if ( t == 1 ) {
			return ( ! _call.template get<function_ptr_t>() );
		}
		return ( true );
	}
	bool operator == ( HBoundCall const& other_ ) const {
		return ( _call == other_._call );
	}
	bool operator != ( HBoundCall const& other_ ) const {
		return ( _call != other_._call );
	}
	void swap( HBoundCall& boundCall_ ) {
		if ( &boundCall_ != this ) {
			yaal::swap( boundCall_._call, _call );
		}
		return;
	}
};

template<typename bound_call_t, typename... T>
bound_call_t bound_call( T&&... arg_ ) {
	return ( yaal::hcore::call( yaal::forward<T>( arg_ )... ) );
}


template<typename signature_t>
inline void swap( HBoundCall<signature_t>& a, HBoundCall<signature_t>& b ) {
	a.swap( b );
}

}

}

#endif /* #ifndef YAAL_HCORE_HBOUNDCALL_HXX_INCLUDED */

