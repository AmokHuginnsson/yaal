/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	hexception.hxx - this file is integral part of `yaal' project.

  i.  You may not make any changes in Copyright information.
  ii. You must attach Copyright information to any part of every copy
      of this software.

Copyright:

 You can use this software free of charge and you can redistribute its binary
 package freely but:
  1. You are not allowed to use any part of sources of this software.
  2. You are not allowed to redistribute any part of sources of this software.
  3. You are not allowed to reverse engineer this software.
  4. If you want to distribute a binary package of this software you cannot
     demand any fees for it. You cannot even demand
     a return of cost of the media or distribution (CD for example).
  5. You cannot involve this software in any commercial activity (for example
     as a free add-on to paid software or newspaper).
 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE. Use it at your own risk.
*/
/*! \file hcore/hexception.hxx
 * \brief HException and HExceptionT<> class declaration.
 *
 * Auto-exception-hierachy stuff is decalred here.
 */

#ifndef YAAL_HCORE_HEXCEPTION_HXX_INCLUDED
#define YAAL_HCORE_HEXCEPTION_HXX_INCLUDED 1

#include <cerrno>
#include <cstddef>
#include <typeinfo>

#ifdef __YAAL_BUILD__
#	include "config.hxx"
#endif /* __YAAL_BUILD__ */
#include "hcore/base.hxx"
#include "hcore/assert.hxx"
#include "hcore/trait.hxx"
#include "hcore/hstring.hxx"

/* cppcheck-suppress variableHidingTypedef */
typedef void this_type;
/* cppcheck-suppress variableHidingTypedef */
typedef void base_type;

namespace yaal {

/*! \brief Core functionality.
 *
 * This namespace contains core library functionality that is
 * independent from UI or external software.
 */
namespace hcore {

extern M_YAAL_HCORE_PUBLIC_API int _debugLevel_;
struct DEBUG_LEVEL {
	static int const PRINT_PROGRAM_OPTIONS = 3;
	static int const PRINT_EXCEPTION_STACK = 5;
	static int const GDB = 7;
	static int const ABORT_ON_ASSERT = 10;
};

/*! \brief Base exception class for YAAL software.
 *
 * This class is base class for all exceptions used (generated)
 * in \e yaal library.
 */
class HException {
protected:
	int  _code; /*!< Numeric code for given exception type/cause. */
	int  _frame; /*!< Frame counter for step-by-step unwinding with logging. */
	char const* _fileName; /*!< Log each frame only once. */
	char const* _functionName; /*!< Log each frame only once. */
	HString _message; /*!< Exception messege. */
	/*! \brief Construct exception object.
	 *
	 * \param fileName_ - source code file name where exception has been generated.
	 * \param line_ - line of code where throw occured.
	 * \param functionName_ - name of the function that throws this exception.
	 * \param message_ - an exception description.
	 * \param code_ - error code.
	 */
	HException( char const* fileName_, int line_, char const* functionName_,
			HString const& message_, int const code_ = 0 );
	HException( HException const& );
public:
	virtual ~HException( void );
	void print_error( void ) const;
	/*! \brief Log exception frame.
	 *
	 * \param fileName_ - source code file name where exception has been caught.
	 * \param line_ - line of code where where exception has been caught.
	 * \param functionName_ - name of the function that is on unwinding path.
	 */
	void log( char const* const fileName_, int line_, char const* const functionName_ );
	char const* what( void ) const;
	int code( void ) const;
	static void set_error_stream( void* );
private:
	HException& operator = ( HException const& );
};

extern M_YAAL_HCORE_PUBLIC_API char const* const _exceptionType_;

/*! \brief Demangle symbol name.
 *
 * \param symbol_ - a symbol name to demangle.
 * \return demangled version of symbol name.
 */
HString demangle( char const* );

template<typename tType>
HString type_name( tType const* object_ ) {
	return ( demangle( typeid( *object_ ).name() ) );
}

template<typename tType>
HString type_name( void ) {
	return ( demangle( typeid( tType ).name() ) );
}

/*! \brief Template used to create type specyfic exceptions.
 */
template<typename tType, typename base_type_t = HException>
class HExceptionT : public base_type_t {
public:
	typedef base_type_t base_type;
	typedef HExceptionT<tType, base_type> this_type;
	HExceptionT( HString const& reason_, int code_ = errno, HString const& symbol_ = type_name<tType>() )
		: base_type( _exceptionType_, 0, symbol_.raw(), reason_, code_ )
		{ }
	HExceptionT( char const* fileName_,
			int line_, char const* functionName_,
			HString const& reason_, int const code_ )
		: base_type( fileName_, line_, functionName_, reason_, code_ )
		{	}
};

typedef HExceptionT<HString> HStringException;
typedef HExceptionT<LexicalCast> HLexicalCastException;

/*! \brief Yaal's default exception handling policy.
 */
struct HGlobalScopeExceptionHandlingPolicy {
	static void handle_exception( void ) __attribute__(( __noreturn__ ));
};

/*! \brief Exception safe wrapper for global variables.
 */
template<typename global_t, typename exception_handling_policy_t = HGlobalScopeExceptionHandlingPolicy>
class HExceptionSafeGlobal {
	global_t _object;
public:
/* cppcheck-suppress functionConst */
	HExceptionSafeGlobal( void ) try : _object() {} catch ( ... ) { exception_handling_policy_t::handle_exception(); }
	template<typename a0_t>
	HExceptionSafeGlobal( a0_t const& a0_ ) try : _object( a0_ ) {} catch ( ... ) { exception_handling_policy_t::handle_exception(); }
	template<typename a0_t, typename a1_t>
	HExceptionSafeGlobal( a0_t const& a0_, a1_t const& a1_ ) try : _object( a0_, a1_ ) {} catch ( ... ) { exception_handling_policy_t::handle_exception(); }
	global_t& instance( void )
		{ return ( _object ); }
};

void kill_interior( char const* const = NULL );

/*
 * A,     AE,      AE = ET<A, E>
 * B : A, BE : AE, BE = ET<B, AE>
 * C : B, CE : BE, CE = ET<C, BE>
 */

/*! \brief Helper %tools for automatic creation for (YEH) yaal exception hierarchy.
 *
 * Non of members of this util namespace should be used directly.
 */
namespace exception_auto_hierarchy {

template<typename tType>
struct parent_exception;

/*! \brief Meta-function, parent_exception helper, gets existing parent exception or creates newone.
 */
template<typename tType>
struct void_to_exception {
	typedef typename parent_exception<tType>::parent_exception_t parent_exception_t;
	typedef HExceptionT<tType, parent_exception_t> exception_t;
};

/*! \cond */
template<>
struct void_to_exception<void> {
	typedef yaal::hcore::HException exception_t;
};
/*! \endcond */

template<int const, typename>
struct existing_hier;

/*! \brief Get superclass in hierarchy if one exists, predefined value otherwise.
 */
template<typename tType>
struct context_hier {
	template<typename real_class>
	static trait::true_type has_hier( typename real_class::base_type* );
	template<typename real_class>
	static trait::false_type has_hier( ... );
	typedef typename existing_hier<sizeof ( has_hier<tType>( 0 ) ), tType>::type type;
};

/*! \brief Get type of existing superclass (in hierarchy) of a type.
 *
 * \tparam subclass - subclass type in hierarchy.
 * \retval type - superclass in hierarchy.
 */
template<typename subclass_t>
struct existing_hier<sizeof ( trait::true_type ), subclass_t> {
	/*! \brief Meta-function, get typedef from protected section of a type.
	 *
	 * \tparam hermetic - type to infiltrate.
	 * \retval type - superclass in hierarchy.
	 */
	template<typename hermetic>
	struct get_protected_typedef : public hermetic {
		~get_protected_typedef( void );
		typedef typename hermetic::base_type type;
	};
	typedef typename get_protected_typedef<subclass_t>::type type;
};

/*! \cond */
template<typename subclass>
struct existing_hier<sizeof ( trait::false_type ), subclass> {
	typedef base_type type;
};
/*! \endcond */

/*! \brief Meta-function to retrieve parent exception of given exception.
 *
 * \tparam tType - child exception type.
 */
template<typename tType>
struct parent_exception {
	typedef typename context_hier<tType>::type proposed_type_hier_t;
	typedef typename void_to_exception<proposed_type_hier_t>::exception_t parent_exception_t;
};

/*! \cond */
template<>
struct parent_exception<void> {
	typedef yaal::hcore::HException parent_exception_t;
};
/*! \endcond */

}

template<typename tType, typename code_t>
void throw_exception( char const*, int, char const*, HString const&, code_t const&, HString const& = HString() ) __attribute__(( __noreturn__ ));
template<typename tType, typename code_t>
void throw_exception( char const* file, int line, char const* function, HString const& message, code_t const& code, HString const& reason ) {
	typedef typename exception_auto_hierarchy::parent_exception<tType>::parent_exception_t parent_exception_t;
	typedef typename yaal::hcore::HExceptionT<tType, parent_exception_t> exception_t;
	throw exception_t( file, line, function, ! reason.is_empty() ? message + ": " + reason : message, static_cast<int>( code ) );
}

template<typename tType, typename code_t>
void throw_exception( char const*, int, char const*, HString const&, code_t const&, HString const&, HString const& ) __attribute__(( __noreturn__ ));
template<typename tType, typename code_t>
void throw_exception( char const* file, int line, char const* function, HString const& message, code_t const& code, HString const& reason, HString const& comment ) {
	typedef typename exception_auto_hierarchy::parent_exception<tType>::parent_exception_t parent_exception_t;
	typedef typename yaal::hcore::HExceptionT<tType, parent_exception_t> exception_t;
	throw exception_t( file, line, function, message + ": " + reason + ": " + comment, static_cast<int>( code ) );
}

}

}

#endif /* #ifndef YAAL_HCORE_HEXCEPTION_HXX_INCLUDED */

