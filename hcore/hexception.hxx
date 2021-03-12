/* Read yaal/LICENSE.md file for copyright and licensing information. */
/*! \file hcore/hexception.hxx
 * \brief HException and HExceptionT<> class declaration.
 *
 * Auto-exception-hierarchy stuff is declared here.
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
	static int const VERBOSE_MESSAGES = 1;
	static int const DEBUG_MESSAGES = 3;
	static int const DUMP_EXCEPTION_STACK = 5;
	static int const GDB = 7;
	static int const ABORT_ON_ASSERT = 10;
};

/*! \brief Base exception class for YAAL software.
 *
 * This class is base class for all exceptions used (generated)
 * in \e yaal library.
 */
class HException {
private:
	int  _code;  /*!< Numeric code for given exception type/cause. */
	int  _frame; /*!< Frame counter for step-by-step unwinding with logging. */
	char const* _fileName;     /*!< Log each frame only once. */
	char const* _functionName; /*!< Log each frame only once. */
	HUTF8String _message;      /*!< Exception message. */
	static bool _logEnabled;
protected:
	/*! \brief Construct exception object.
	 *
	 * \param fileName_ - source code file name where exception has been generated.
	 * \param line_ - line of code where throw occurred.
	 * \param functionName_ - name of the function that throws this exception.
	 * \param message_ - an exception description.
	 * \param code_ - error code.
	 * \param name_ - type name this exception.
	 */
	HException( char const* fileName_, int line_, char const* functionName_,
			HString const& message_, int const code_ = 0, HString const& name_ = HString() );
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
	void log( char const* fileName_, int line_, char const* functionName_ );
	char const* what( void ) const;
	int code( void ) const;
	static void set_error_stream( void* );
	static void enable_logging( void );
	static void disable_logging( void );
private:
	HException& operator = ( HException const& );
};

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
HString type_name( bool stripTemplateParams_ = false ) {
	HString typeName( demangle( typeid( tType ).name() ) );
	if ( stripTemplateParams_ ) {
		int long idx( typeName.find( '<'_ycp ) );
		if ( idx != HString::npos )
			typeName.erase( idx );
	}
	return typeName;
}

/*! \brief Template used to create type specific exceptions.
 */
template<typename tType, typename base_type_t = HException>
class HExceptionT : public base_type_t {
	static bool _decodeErrno;
	static yaal::hcore::HString const _name;
public:
	typedef base_type_t base_type;
	typedef HExceptionT<tType, base_type> this_type;
	HExceptionT( HString const& reason_, int code_ = errno )
		: base_type( nullptr, 0, nullptr, reason_ + ( _decodeErrno ? to_string( ": " ).append( error_message( code_ ) ) : "" ), code_, _name ) {
		return;
	}
	HExceptionT( char const* fileName_,
			int line_, char const* functionName_,
			HString const& reason_, int const code_, HString const& name_ = _name )
		: base_type( fileName_, line_, functionName_, reason_, code_, name_ ) {
		return;
	}
	static bool decode_errno( void ) {
		return ( _decodeErrno = true );
	}
};
template<typename tType, typename base_type_t>
bool HExceptionT<tType, base_type_t>::_decodeErrno = false;
template<typename tType, typename base_type_t>
yaal::hcore::HString const HExceptionT<tType, base_type_t>::_name = type_name<tType>( true ) + "Exception";

typedef HExceptionT<void> HRuntimeException;
typedef HExceptionT<HString> HStringException;
typedef HExceptionT<HUTF8String> HUTF8StringException;
typedef HExceptionT<LexicalCast> HLexicalCastException;
struct InvalidKey {};
typedef HExceptionT<InvalidKey> HInvalidKeyException;
struct InvalidArgument {};
typedef HExceptionT<InvalidArgument> HInvalidArgumentException;
struct OutOfRange {};
typedef HExceptionT<OutOfRange> HOutOfRangeException;

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
	global_t& instance( void ) {
		return ( _object );
	}
};

void kill_interior( yaal::hcore::HString const& );

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

/*! \brief Meta-function, parent_exception helper, gets existing parent exception or creates a new one.
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
 *
 * Access checking is part of substitution process,
 * hence one cannot detect existence of private or protected members types
 * using SFINAE based method.
 */
template<typename tType>
struct context_hier {
	template<typename real_class>
	static trait::true_type has_hier( typename real_class::base_type* );
	template<typename real_class>
	static trait::false_type has_hier( ... );
	typedef typename existing_hier<static_cast<int>( sizeof ( has_hier<tType>( nullptr ) ) ), tType>::type type;
};

/*! \brief Get type of existing superclass (in hierarchy) of a type.
 *
 * \tparam subclass - subclass type in hierarchy.
 * \retval type - superclass in hierarchy.
 */
template<typename subclass>
struct existing_hier<static_cast<int>( sizeof ( trait::true_type ) ), subclass> {
	typedef typename subclass::base_type type;
};

/*! \cond */
template<typename subclass>
struct existing_hier<static_cast<int>( sizeof ( trait::false_type ) ), subclass> {
	typedef void type;
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
void throw_exception( char const*, int, char const*, HString const&, code_t const& ) __attribute__(( __noreturn__ ));
template<typename tType, typename code_t>
void throw_exception( char const* file, int line, char const* function, HString const& message, code_t const& code ) {
	typedef typename exception_auto_hierarchy::parent_exception<tType>::parent_exception_t parent_exception_t;
	typedef typename yaal::hcore::HExceptionT<tType, parent_exception_t> exception_t;
	throw exception_t( file, line, function, message, static_cast<int>( code ) );
}

template<typename tType, typename code_t>
void throw_exception( char const*, int, char const*, HString const&, code_t const&, HString const& ) __attribute__(( __noreturn__ ));
template<typename tType, typename code_t>
void throw_exception( char const* file, int line, char const* function, HString const& message, code_t const& code, HString const& comment ) {
	typedef typename exception_auto_hierarchy::parent_exception<tType>::parent_exception_t parent_exception_t;
	typedef typename yaal::hcore::HExceptionT<tType, parent_exception_t> exception_t;
	throw exception_t( file, line, function, message + ": " + comment, static_cast<int>( code ) );
}

}

}

#endif /* #ifndef YAAL_HCORE_HEXCEPTION_HXX_INCLUDED */

