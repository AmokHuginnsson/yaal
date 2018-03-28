/* Read yaal/LICENSE.md file for copyright and licensing information. */

/*! \file hcore/functional.hxx
 * \brief Higher order programming basics.
 */

#ifndef YAAL_HCORE_FUNCTIONAL_HXX_INCLUDED
#define YAAL_HCORE_FUNCTIONAL_HXX_INCLUDED 1

#include "hcore/trait.hxx"

namespace yaal {

/*! \brief Meta-data definition for generator function functors.
 *
 * Type descriptions:
 *
 * \tparam result_t - result type of generator function.
 */
template<typename result_t>
struct nullary_function {
	typedef result_t result_type;
	typedef nullary_function<result_type> this_type;
	virtual ~nullary_function( void ) {}
};

/*! \brief Meta-data definition for unary function functors.
 *
 * Type descriptions:
 *
 * \tparam result_t - result type of unary function.
 * \tparam arg_t - argument type of unary function.
 */
template<typename result_t, typename arg_t>
struct unary_function {
	typedef result_t result_type;
	typedef arg_t argument_type;
	typedef unary_function<result_type, argument_type> this_type;
	virtual ~unary_function( void ) {}
};

/*! \brief Meta-data definition for binary function functors.
 *
 * Type descriptions:
 *
 * \tparam result_t - result type of binary function.
 * \tparam arg1st_t - first argument type of binary function.
 * \tparam arg2nd_t - second argument type of binary function.
 */
template<typename result_t, typename arg1st_t, typename arg2nd_t>
struct binary_function {
	typedef result_t result_type;
	typedef arg1st_t first_argument_type;
	typedef arg2nd_t second_argument_type;
	typedef binary_function<result_type, first_argument_type, second_argument_type> this_type;
	virtual ~binary_function( void ) {}
};

/*! \brief Meta-function functor for binding given invariable argument as second argument to any function.
 *
 * Invariable argument may be bound to meta-function functor as well as old style C free standing function.
 *
 * Type descriptions:
 *
 * \tparam function_t - type of function to be bound.
 * \tparam value_t - type of invariable argument to be bound.
 */
template<typename function_t, typename value_t, int bound_no>
class HBinder : public unary_function<
			typename trait::return_type<function_t>::type,
			typename trait::argument_type<function_t, bound_no>::type
		> {
public:
	typedef unary_function<
			typename trait::return_type<function_t>::type,
			typename trait::argument_type<function_t, bound_no>::type
		> base_type;
	typedef typename base_type::result_type result_type;
	typedef typename base_type::argument_type argument_type;
private:
	function_t _call;
	value_t _value;
public:
	HBinder( function_t, value_t );
	template<typename tType>
	result_type operator()( tType value ) const {
		return ( bound_no == 0 ? _call( _value, value ) : _call( value, _value ) );
	}
};

template<typename function_t, typename value_t, int bound_no>
HBinder<function_t, value_t, bound_no>::HBinder( function_t func, value_t value )
	: _call( func ), _value( value )
	{ }

/*! \brief Convenience function to construct HBinder<> object.
 *
 * \param func - function to bind first argument to.
 * \param value - a bound value.
 * \return HBinder<>, a function object that encapsulates func function and its bound value.
 */
template<typename function_t, typename value_t>
HBinder<function_t, value_t, 0> bind1st( function_t func, value_t value ) {
	return ( HBinder<function_t, value_t, 0>( func, value ) );
}

/*! \brief Convenience function to construct HBinder<> object.
 *
 * \param func - function to bind second argument to.
 * \param value - a bound value.
 * \return HBinder<>, a function object that encapsulates func function and its bound value.
 */
template<typename function_t, typename value_t>
HBinder<function_t, value_t, 1> bind2nd( function_t func, value_t value ) {
	return ( HBinder<function_t, value_t, 1>( func, value ) );
}

/*! \brief A reference type wrapper.
 *
 * Pass arguments by reference instead of by value with this trait.
 *
 * \tparam basic_t - type to be wrapper as reference.
 */
template<typename basic_t>
class reference {
	basic_t* _ref;
public:
	explicit reference( basic_t& obj ) : _ref( &obj ) {}
	reference( reference const& ) = default;
	reference( reference&& ) noexcept = default;
	reference& operator = ( reference const& ) = default;
	reference& operator = ( reference&& ) noexcept = default;
	operator basic_t& ( void ) const
		{ return ( *_ref ); }
	basic_t& operator->( void )
		{ return ( *_ref ); }
	template<typename basic_assignable_t>
	basic_t& operator = ( basic_assignable_t const& v ) {
		*_ref = v;
		return ( *_ref );
	}
	template<typename... args_t>
	decltype ( (*_ref)( trait::declval<args_t>()... ) ) operator()( args_t&&... args_ ) const {
		return ( (*_ref)( yaal::forward<args_t>( args_ )... ) );
	}
};

/*! \brief Convenience function, returns trait::reference<> object.
 *
 * \param obj - object that trait::reference<> shall be based on.
 * \return trait::reference<> wrapper of \e obj object.
 */
template<typename basic_t>
reference<basic_t> ref( basic_t& obj ) {
	return ( reference<basic_t>( obj ) );
}

/*! \brief Convenience function, returns trait::reference<const> object.
 *
 * \param obj - object that trait::reference<const> shall be based on.
 * \return trait::reference<const> wrapper of \e obj object.
 */
template<typename basic_t>
reference<basic_t const> cref( basic_t const& obj ) {
	return ( reference<basic_t const>( obj ) );
}

/*! \brief Meta function functor plus operator.
 *
 * \tparam tType - type of added objects.
 */
template<typename tType>
struct plus : public binary_function<tType, tType, tType> {
	tType operator()( tType const& a, tType const& b ) const
		{ return ( a + b ); }
};

/*! \brief Meta function function minus operator.
 *
 * \tparam tType - type of subtracted objects.
 */
template<typename tType>
struct minus : public binary_function<tType, tType, tType> {
	tType operator()( tType const& a, tType const& b ) const
		{ return ( a - b ); }
};

/*! \brief A functor performing multiplication operation.
 *
 * \tparam tType - type of multiplied objects.
 */
template<typename tType>
struct multiplies : public binary_function<tType, tType, tType> {
	/*! \brief Functor executor.
	 *
	 * \param a - first factor of multiplication.
	 * \param b - second factor of multiplication.
	 * \return result of multiplication a times b.
	 */
	tType operator()( tType const& a, tType const& b ) const
		{ return ( a * b ); }
};

/*! \brief A functor performing division operation.
 *
 * \tparam tType - type of divided/ing objects.
 */
template<typename tType>
struct divides : public binary_function<tType, tType, tType> {
	/*! \brief Functor executor.
	 *
	 * \param a - dividend part of division.
	 * \param b - divisor part of division.
	 * \return quotient of a divided by b.
	 */
	tType operator()( tType const& a, tType const& b ) const
		{ return ( a / b ); }
};

/*! \brief A functor performing modulo operation.
 *
 * \tparam tType - type of divided/ing objects.
 */
template<typename tType>
struct modulus : public binary_function<tType, tType, tType> {
	/*! \brief Functor executor.
	 *
	 * \param a - dividend part of division.
	 * \param b - divisor part of division.
	 * \return quotient of a modulo b.
	 */
	tType operator()( tType const& a, tType const& b ) const
		{ return ( a % b ); }
};

/*! \brief Calculate negative value of a number.
 *
 * \tparam tType - number type to be negated.
 */
template<typename tType>
struct negate : public unary_function<tType, tType> {
	/*! \brief Functor executor.
	 *
	 * \param val_ - value to be negated.
	 * \return A negated value, that is: -val_.
	 */
	tType operator()( tType const& val_ ) const
		{ return ( - val_ ); }
};

/*! \brief Generic equality comparison operator.
 *
 * \tparam tType - type of compared objects.
 */
template<typename tType>
struct equal_to : public binary_function<bool, tType, tType> {
	/*! \brief Comparison executor.
	 *
	 * \param left_ - first of the objects to compare.
	 * \param right_ - second of the objects to compare.
	 * \return True iff (if and only if) left_ == right_ - object are equal.
	 */
	bool operator()( tType const& left_, tType const& right_ ) const {
		return ( left_ == right_ );
	}
};

/*! \brief Generic non equality comparison operator.
 *
 * \tparam tType - type of compared objects.
 */
template<typename tType>
struct not_equal_to : public binary_function<bool, tType, tType> {
	/*! \brief Comparison executor.
	 *
	 * \param left_ - first of the objects to compare.
	 * \param right_ - second of the objects to compare.
	 * \return True iff (if and only if) left_ != right_ - object are not equal.
	 */
	bool operator()( tType const& left_, tType const& right_ ) const {
		return ( left_ != right_ );
	}
};

/*! \brief Generic less (object ordering) operator.
 *
 * \tparam tType - type of compared objects.
 */
template<typename tType>
struct less : public binary_function<bool, tType, tType> {
	/*! \brief Comparison executor.
	 *
	 * \param left_ - first of the objects to compare.
	 * \param right_ - second of the objects to compare.
	 * \return True iff (if and only if) left_ < right_ - first objects is lesser than second object.
	 */
	bool operator()( tType const& left_, tType const& right_ ) const {
		return ( left_ < right_ );
	}
};

/*! \brief Generic greater (object ordering) operator.
 *
 * \tparam tType - type of compared objects.
 */
template<typename tType>
struct greater : public binary_function<bool, tType, tType> {
	/*! \brief Comparison executor.
	 *
	 * \param left_ - first of the objects to compare.
	 * \param right_ - second of the objects to compare.
	 * \return True iff (if and only if) left_ > right_ - first objects is greater than second object.
	 */
	bool operator()( tType const& left_, tType const& right_ ) const {
		return ( left_ > right_ );
	}
};

/*! \brief Generic less or equal (object ordering) operator.
 *
 * \tparam tType - type of compared objects.
 */
template<typename tType>
struct less_equal : public binary_function<bool, tType, tType> {
	/*! \brief Comparison executor.
	 *
	 * \param left_ - first of the objects to compare.
	 * \param right_ - second of the objects to compare.
	 * \return True iff (if and only if) left_ <= right_ - first objects is lesser or equal than second object.
	 */
	bool operator()( tType const& left_, tType const& right_ ) const {
		return ( left_ <= right_ );
	}
};

/*! \brief Generic greater or equal (object ordering) operator.
 *
 * \tparam tType - type of compared objects.
 */
template<typename tType>
struct greater_equal : public binary_function<bool, tType, tType> {
	/*! \brief Comparison executor.
	 *
	 * \param left_ - first of the objects to compare.
	 * \param right_ - second of the objects to compare.
	 * \return True iff (if and only if) left_ >= right_ - first objects is greater or equal than second object.
	 */
	bool operator()( tType const& left_, tType const& right_ ) const {
		return ( left_ >= right_ );
	}
};

/*! \brief Logical negation operator.
 */
template<typename tType = bool>
struct logical_not : public unary_function<bool, bool> {
	bool operator()( bool value_ ) const {
		return ( ! value_ );
	}
};

/*! \brief Logical `and' operator.
 */
template<typename tType = bool>
struct logical_and : public binary_function<bool, bool, bool> {
	bool operator()( bool a_, bool b_ ) const {
		return ( a_ && b_ );
	}
};

/*! \brief Logical `or' operator.
 */
template<typename tType = bool>
struct logical_or : public binary_function<bool, bool, bool> {
	bool operator()( bool a_, bool b_ ) const {
		return ( a_ || b_ );
	}
};

/*! \brief Calculate `exclusive or' logical value.
 *
 * \param p - first predicate.
 * \param q - second predicate.
 * \return p exor q <=> ( p ^ ~q ) v ( ~p ^ q ) )
 */
inline bool exor( bool p, bool q ) {
	return ( ( p && !q ) || ( !p && q ) );
}

/*! \brief Logical `xor' operator.
 */
template<typename tType = bool>
struct logical_exor : public binary_function<bool, bool, bool> {
	bool operator()( bool a_, bool b_ ) const {
		return ( exor( a_, b_ ) );
	}
};

/*! \brief Calculate logical biconditional value.
 *
 * \param p - first predicate.
 * \param q - second predicate.
 * \return p xnor q <=> ( p ^ q ) v ( ~ ( p v q ) )
 */
inline bool xnor( bool p, bool q ) {
	return ( ( p && q ) || ( ! ( p || q ) ) );
}

/*! \brief Logical `xnor' operator.
 */
template<typename tType = bool>
struct logical_xnor : public binary_function<bool, bool, bool> {
	bool operator()( bool a_, bool b_ ) const {
		return ( xnor( a_, b_ ) );
	}
};

/*! \brief Composition of two unary functions.
 */
template<typename F, typename G>
class unary_compose : public unary_function<typename F::result_type, typename G::argument_type> {
public:
	typedef unary_function<typename F::result_type, typename G::argument_type> base_type;
	typedef typename base_type::result_type result_type;
	typedef typename base_type::argument_type argument_type;
private:
	F _f;
	G _g;
public:
	unary_compose( F f_, G g_ ) : _f( f_ ), _g( g_ ) {}
	result_type operator()( typename trait::make_reference<argument_type const>::type arg_ ) const {
		return ( _f( _g( arg_ ) ) );
	}
};

/*! \brief Make unary function by statically composing two unary functions.
 */
template<typename F, typename G>
unary_compose<F, G> compose1( F f_, G g_ )
	{ return ( unary_compose<F, G>( f_, g_ ) ); }

/*! \brief Composition of binary functions with two unary functions.
 */
template<typename F, typename G1, typename G2>
class binary_compose : public unary_function<typename F::result_type, typename G1::argument_type> {
public:
	typedef unary_function<typename F::result_type, typename G1::argument_type> base_type;
	typedef typename base_type::result_type result_type;
	typedef typename base_type::argument_type argument_type;
private:
	F _f;
	G1 _g1;
	G2 _g2;
public:
	binary_compose( F f_, G1 g1_, G2 g2_ ) : _f( f_ ), _g1( g1_ ), _g2( g2_ ) {}
	result_type operator()( argument_type const& arg_ ) const {
		return ( _f( _g1( arg_ ), _g2( arg_ ) ) );
	}
};

/*! \brief Make binary function by statically composing binary function with two unary functions.
 */
template<typename F, typename G1, typename G2>
binary_compose<F, G1, G2> compose2( F f_, G1 g1_, G2 g2_ )
	{ return ( binary_compose<F, G1, G2>( f_, g1_, g2_ ) ); }

/*! \brief Logical negation of an unary function.
 */
template<typename F>
class unary_negate : public unary_function<typename F::result_type, typename F::argument_type> {
public:
	typedef unary_function<typename F::result_type, typename F::argument_type> base_type;
	typedef typename base_type::result_type result_type;
	typedef typename base_type::argument_type argument_type;
private:
	F _f;
public:
	unary_negate( F f_ ) : _f( f_ ) {}
	result_type operator()( argument_type const& arg_ ) const {
		return ( ! _f( arg_ ) );
	}
};

/*! \brief Statically negate logical value of unary function result.
 */
template<typename F>
unary_negate<F> not1( F f_ )
	{ return ( unary_negate<F>( f_ ) ); }

/*! \brief Logical negation of binary function.
 */
template<typename F>
class binary_negate : public binary_function<typename F::result_type, typename F::first_argument_type, typename F::second_argument_type> {
public:
	typedef binary_function<typename F::result_type, typename F::first_argument_type, typename F::second_argument_type> base_type;
	typedef typename base_type::result_type result_type;
	typedef typename base_type::first_argument_type first_argument_type;
	typedef typename base_type::second_argument_type second_argument_type;
private:
	F _f;
public:
	binary_negate( F f_ ) : _f( f_ ) {}
	result_type operator()( first_argument_type const& a1_, second_argument_type const& a2_ ) const {
		return ( ! _f( a1_, a2_ ) );
	}
};

/*! \brief Statically negate logical value of binary function result.
 */
template<typename F>
binary_negate<F> not2( F f_ )
	{ return ( binary_negate<F>( f_ ) ); }

/*! \brief Identity function.
 */
template<typename tType>
struct identity : public unary_function<tType, tType> {
	typedef identity<tType> this_type;
	typedef unary_function<tType, tType> base_type;
	typedef typename base_type::result_type result_type;
	typedef typename base_type::argument_type argument_type;
	typename trait::make_reference<result_type const>::type operator()( typename trait::make_reference<argument_type const>::type v_ ) const
		{ return ( v_ ); }
	typename trait::make_reference<result_type>::type operator()( typename trait::make_reference<argument_type>::type v_ )
		{ return ( v_ ); }
};

template<typename first_t, typename second_t>
struct project1st : public binary_function<first_t, first_t, second_t> {
	typedef project1st<first_t, second_t> this_type;
	typedef binary_function<first_t, first_t, second_t> base_type;
	typedef typename base_type::result_type result_type;
	typedef typename base_type::first_argument_type first_argument_type;
	typedef typename base_type::second_argument_type second_argument_type;
	typename trait::make_reference<result_type const>::type operator()( typename trait::make_reference<first_argument_type const>::type first_, typename trait::make_reference<second_argument_type const>::type ) const
		{ return ( first_ ); }
	typename trait::make_reference<result_type>::type operator()( typename trait::make_reference<first_argument_type>::type first_, typename trait::make_reference<second_argument_type>::type )
		{ return ( first_ ); }
};

template<typename first_t, typename second_t>
struct project2nd : public binary_function<second_t, first_t, second_t> {
	typedef project2nd<first_t, second_t> this_type;
	typedef binary_function<second_t, first_t, second_t> base_type;
	typedef typename base_type::result_type result_type;
	typedef typename base_type::first_argument_type first_argument_type;
	typedef typename base_type::second_argument_type second_argument_type;
	typename trait::make_reference<result_type const>::type operator()( typename trait::make_reference<first_argument_type const>::type, typename trait::make_reference<second_argument_type const>::type second_ ) const
		{ return ( second_ ); }
	typename trait::make_reference<result_type>::type operator()( typename trait::make_reference<first_argument_type>::type, typename trait::make_reference<second_argument_type>::type second_ )
		{ return ( second_ ); }
};

/*! \brief Constant that can be used in nullary function contexts.
 */
template<typename tType>
class constant : public nullary_function<tType> {
	typedef nullary_function<tType> base_type;
	typedef typename base_type::result_type result_type;
	tType _constant;
public:
	constant( tType constant_ ) : _constant( constant_ ) {}
	result_type operator()( void )
		{ return ( _constant ); }
};

/*! \brief Functional (unary function) wrapper for non-functional unary functions.
 */
template<typename return_t, typename argument_t>
class pointer_to_unary_function : public unary_function<return_t, argument_t> {
public:
	typedef return_t ( *function_t )( argument_t );
private:
	function_t _function;
public:
	pointer_to_unary_function( function_t function_ ) : _function( function_ ) {}
	return_t operator()( argument_t arg_ ) const
		{ return ( _function( arg_ ) ); }
};

template<typename return_t, typename argument_t>
pointer_to_unary_function<return_t, argument_t> ptr_fun( return_t (*function_)( argument_t ) )
	{ return ( pointer_to_unary_function<return_t, argument_t>( function_ ) ); }

/*! \brief Functional (binary function) wrapper for non-functional binary functions.
 */
template<typename return_t, typename first_argument_t, typename second_argument_t>
class pointer_to_binary_function : public binary_function<return_t, first_argument_t, second_argument_t> {
public:
	typedef return_t ( *function_t )( first_argument_t, second_argument_t );
private:
	function_t _function;
public:
	pointer_to_binary_function( function_t function_ ) : _function( function_ ) {}
	return_t operator()( first_argument_t firstArgument_, second_argument_t secondArgument_ ) const
		{ return ( _function( firstArgument_, secondArgument_ ) ); }
};

template<typename return_t, typename first_argument_t, typename second_argument_t>
pointer_to_binary_function<return_t, first_argument_t, second_argument_t> ptr_fun( return_t (*function_)( first_argument_t, second_argument_t ) )
	{ return ( pointer_to_binary_function<return_t, first_argument_t, second_argument_t>( function_ ) ); }

template<typename R, typename C>
class pointer_to_nullary_member_function {
	typedef R (C::*member_t)( void );
	member_t _member;
public:
	pointer_to_nullary_member_function( member_t member_ ) : _member( member_ ) {}
/* cppcheck-suppress functionConst */
	R operator()( C* object_ ) {
		return ( (object_->*_member)() );
	}
};

template<typename R, typename C>
class pointer_to_nullary_const_member_function {
	typedef R (C::*member_t)( void ) const;
	member_t _member;
public:
	pointer_to_nullary_const_member_function( member_t member_ ) : _member( member_ ) {}
	R operator()( C const* object_ ) const {
		return ( (object_->*_member)() );
	}
};

template<typename R, typename C>
class pointer_to_nullary_member_function_ref {
	typedef R (C::*member_t)( void );
	member_t _member;
public:
	pointer_to_nullary_member_function_ref( member_t member_ ) : _member( member_ ) {}
/* cppcheck-suppress functionConst */
	R operator()( C& object_ ) {
		return ( (object_.*_member)() );
	}
};

template<typename R, typename C>
class pointer_to_nullary_const_member_function_ref {
	typedef R (C::*member_t)( void ) const;
	member_t _member;
public:
	pointer_to_nullary_const_member_function_ref( member_t member_ ) : _member( member_ ) {}
	R operator()( C const& object_ ) const {
		return ( (object_.*_member)() );
	}
};

template<typename R, typename C>
pointer_to_nullary_member_function<R, C> mem_fun( R (C::*member_)( void ) ) {
	return ( pointer_to_nullary_member_function<R, C>( member_ ) );
}

template<typename R, typename C>
pointer_to_nullary_const_member_function<R, C> mem_fun( R (C::*member_)( void ) const ) {
	return ( pointer_to_nullary_const_member_function<R, C>( member_ ) );
}

template<typename R, typename C>
pointer_to_nullary_member_function_ref<R, C> mem_fun_ref( R (C::*member_)( void ) ) {
	return ( pointer_to_nullary_member_function_ref<R, C>( member_ ) );
}

template<typename R, typename C>
pointer_to_nullary_const_member_function_ref<R, C> mem_fun_ref( R (C::*member_)( void ) const ) {
	return ( pointer_to_nullary_const_member_function_ref<R, C>( member_ ) );
}

template<typename R, typename C, typename T>
class pointer_to_unary_member_function {
	typedef R (C::*member_t)( T );
	member_t _member;
public:
	pointer_to_unary_member_function( member_t member_ ) : _member( member_ ) {}
/* cppcheck-suppress functionConst */
	R operator()( C* object_, T arg_ ) {
		return ( (object_->*_member)( arg_ ) );
	}
};

template<typename R, typename C, typename T>
class pointer_to_unary_const_member_function {
	typedef R (C::*member_t)( T ) const;
	member_t _member;
public:
	pointer_to_unary_const_member_function( member_t member_ ) : _member( member_ ) {}
	R operator()( C const* object_, T arg_ ) const {
		return ( (object_->*_member)( arg_ ) );
	}
};

template<typename R, typename C, typename T>
class pointer_to_unary_member_function_ref {
	typedef R (C::*member_t)( T );
	member_t _member;
public:
	pointer_to_unary_member_function_ref( member_t member_ ) : _member( member_ ) {}
/* cppcheck-suppress functionConst */
	R operator()( C& object_, T arg_ ) {
		return ( (object_.*_member)( arg_ ) );
	}
};

template<typename R, typename C, typename T>
class pointer_to_unary_const_member_function_ref {
	typedef R (C::*member_t)( T ) const;
	member_t _member;
public:
	pointer_to_unary_const_member_function_ref( member_t member_ ) : _member( member_ ) {}
	R operator()( C const& object_, T arg_ ) const {
		return ( (object_.*_member)( arg_ ) );
	}
};

template<typename R, typename C, typename T>
pointer_to_unary_member_function<R, C, T> mem_fun1( R (C::*member_)( T ) ) {
	return ( pointer_to_unary_member_function<R, C, T>( member_ ) );
}

template<typename R, typename C, typename T>
pointer_to_unary_const_member_function<R, C, T> mem_fun1( R (C::*member_)( T ) const ) {
	return ( pointer_to_unary_const_member_function<R, C, T>( member_ ) );
}

template<typename R, typename C, typename T>
pointer_to_unary_member_function_ref<R, C, T> mem_fun1_ref( R (C::*member_)( T ) ) {
	return ( pointer_to_unary_member_function_ref<R, C, T>( member_ ) );
}

template<typename R, typename C, typename T>
pointer_to_unary_const_member_function_ref<R, C, T> mem_fun1_ref( R (C::*member_)( T ) const ) {
	return ( pointer_to_unary_const_member_function_ref<R, C, T>( member_ ) );
}

template<typename F, typename G1, typename G2>
class binary_composition : public binary_function<typename F::result_type, typename G1::argument_type, typename G2::argument_type> {
public:
	typedef binary_function<typename F::result_type, typename G1::argument_type, typename G2::argument_type> base_type;
	typedef typename base_type::result_type result_type;
	typedef typename base_type::first_argument_type first_argument_type;
	typedef typename base_type::second_argument_type second_argument_type;
private:
	F _f;
	G1 _g1;
	G2 _g2;
public:
	binary_composition( F f_, G1 g1_, G2 g2_ ) : _f( f_ ), _g1( g1_ ), _g2( g2_ ) {}
	result_type operator()( typename trait::make_reference<first_argument_type const>::type arg1_, typename trait::make_reference<second_argument_type const>::type arg2_ ) const {
		return ( _f( _g1( arg1_ ), _g2( arg2_ ) ) );
	}
};

template<typename F, typename G1, typename G2>
binary_composition<F, G1, G2> compose_binary( F f_, G1 g1_, G2 g2_ )
	{ return ( binary_composition<F, G1, G2>( f_, g1_, g2_ ) ); }

template<typename T>
struct select1st : public unary_function<typename T::first_type, T> {
	typedef select1st<T> this_type;
	typedef unary_function<typename T::first_type, T> base_type;
	typedef typename base_type::result_type result_type;
	typedef typename base_type::argument_type argument_type;
	typename trait::make_reference<result_type const>::type operator()( typename trait::make_reference<argument_type const>::type arg_ ) const
		{ return ( arg_.first ); }
	typename trait::make_reference<result_type>::type operator()( typename trait::make_reference<argument_type>::type arg_ )
		{ return ( arg_.first ); }
};

template<typename T>
struct select2nd : public unary_function<typename T::second_type, T> {
	typedef select2nd<T> this_type;
	typedef unary_function<typename T::second_type, T> base_type;
	typedef typename base_type::result_type result_type;
	typedef typename base_type::argument_type argument_type;
	typename trait::make_reference<result_type const>::type operator()( typename trait::make_reference<argument_type const>::type arg_ ) const
		{ return ( arg_.second ); }
	typename trait::make_reference<result_type>::type operator()( typename trait::make_reference<argument_type>::type arg_ )
		{ return ( arg_.second ); }
};

template<typename to_t, typename from_t>
struct bound_cast : public unary_function<to_t, from_t> {
	to_t operator()( from_t from_ ) const {
		return ( static_cast<to_t>( from_ ) );
	}
};

}

#endif /* #ifndef YAAL_HCORE_FUNCTIONAL_HXX_INCLUDED */

