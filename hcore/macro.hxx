/* Read yaal/LICENSE.md file for copyright and licensing information. */

/*! \file hcore/macro.hxx
 * \brief Globally visible preprocessor macros are stored here.
 */

#ifndef YAAL_HCORE_MACRO_HXX_INCLUDED
#define YAAL_HCORE_MACRO_HXX_INCLUDED 1

#ifdef _
#	error Yaal redefines _ macro.
#endif /* #ifdef _ */
#ifdef M_EXPAND
#	error Yaal redefines M_EXPAND macro.
#endif /* #ifdef M_EXPAND */
#ifdef M_STRINGIFY_REAL
#	error Yaal redefines M_STRINGIFY_REAL macro.
#endif /* #ifdef M_STRINGIFY_REAL */
#ifdef M_CONCAT_REAL
#	error Yaal redefines M_CONCAT_REAL macro.
#endif /* #ifdef M_CONCAT_REAL */
#ifdef M_CONCAT
#	error Yaal redefines M_CONCAT macro.
#endif /* #ifdef M_CONCAT */
#ifdef M_STRINGIFY
#	error Yaal redefines M_STRINGIFY macro.
#endif /* #ifdef M_STRINGIFY */
#ifdef M_YAAL_AUTO_ID
#	error Yaal redefines M_YAAL_AUTO_ID macro.
#endif /* #ifdef M_YAAL_AUTO_ID */
#ifdef M_VCSID
#	error Yaal redefines M_VCSID macro.
#endif /* #ifdef M_VCSID */
#ifdef M_THROW
#	error Yaal redefines M_THROW macro.
#endif /* #ifdef M_THROW */
#ifdef M_PROLOG
#	error Yaal redefines M_PROLOG macro.
#endif /* #ifdef M_PROLOG */
#ifdef M_EPILOG
#	error Yaal redefines M_EPILOG macro.
#endif /* #ifdef M_EPILOG */
#ifdef M_DESTRUCTOR_EPILOG
#	error Yaal redefines M_DESTRUCTOR_EPILOG macro.
#endif /* #ifdef M_DESTRUCTOR_EPILOG */
#ifdef M_SAFE
#	error Yaal redefines M_SAFE macro.
#endif /* #ifdef M_SAFE */
#ifdef M_FINAL
#	error Yaal redefines M_FINAL macro.
#endif /* #ifdef M_FINAL */
#ifdef M_ENSURE
#	error Yaal redefines M_ENSURE macro.
#endif /* #ifdef M_ENSURE */
#ifdef M_MACRO_ARGC_HELPER
#	error Yaal redefines M_MACRO_ARGC_HELPER macro.
#endif /* #ifdef M_MACRO_ARGC_HELPER */
#ifdef M_MACRO_ARGC
#	error Yaal redefines M_MACRO_ARGC macro.
#endif /* #ifdef M_MACRO_ARGC */
#ifdef M_ENSURE_1
#	error Yaal redefines M_ENSURE_1 macro.
#endif /* #ifdef M_ENSURE_1 */
#ifdef M_ENSURE_2
#	error Yaal redefines M_ENSURE_2 macro.
#endif /* #ifdef M_ENSURE_2 */
#ifdef M_NUM_ARGC
#	error Yaal redefines M_NUM_ARGC macro.
#endif /* #ifdef M_NUM_ARGC */
#ifdef M_NUM_ARGC_COUNTER
#	error Yaal redefines M_NUM_ARGC_COUNTER macro.
#endif /* #ifdef M_NUM_ARGC_COUNTER */
#ifdef M_ENSURE_T
#	error Yaal redefines M_ENSURE_T macro.
#endif /* #ifdef M_ENSURE_T */
#ifdef M_ASSERT
#	error Yaal redefines M_ASSERT macro.
#endif /* #ifdef M_ASSERT */
#ifdef M_DEBUG_CODE
#	error Yaal redefines M_DEBUG_CODE macro.
#endif /* #ifdef M_DEBUG_CODE */
#ifdef M_NDEBUG_CODE
#	error Yaal redefines M_NDEBUG_CODE macro.
#endif /* #ifdef M_NDEBUG_CODE */
#ifdef M_TEMP_FAILURE_RETRY
#	error Yaal redefines M_TEMP_FAILURE_RETRY macro.
#endif /* #ifdef M_TEMP_FAILURE_RETRY */
#ifdef M_ATTRIBUTE_SPEC
#	error Yaal redefines M_ATTRIBUTE_SPEC macro.
#endif /* #ifdef M_ATTRIBUTE_SPEC */
#ifdef M_EXPORT_SYMBOL
#	error Yaal redefines M_EXPORT_SYMBOL macro.
#endif /* #ifdef M_EXPORT_SYMBOL */
#ifdef M_YAAL_HCORE_PUBLIC_API
#	error Yaal redefines M_YAAL_HCORE_PUBLIC_API macro.
#endif /* #ifdef M_YAAL_HCORE_PUBLIC_API */
#ifdef M_YAAL_HCORE_EXTERN
#	error Yaal redefines M_YAAL_HCORE_EXTERN macro.
#endif /* #ifdef M_YAAL_HCORE_EXTERN */
#ifdef M_YAAL_TOOLS_PUBLIC_API
#	error Yaal redefines M_YAAL_TOOLS_PUBLIC_API macro.
#endif /* #ifdef M_YAAL_TOOLS_PUBLIC_API */
#ifdef M_YAAL_TOOLS_EXTERN
#	error Yaal redefines M_YAAL_TOOLS_EXTERN macro.
#endif /* #ifdef M_YAAL_TOOLS_EXTERN */
#ifdef M_YAAL_DBWRAPPER_PUBLIC_API
#	error Yaal redefines M_YAAL_DBWRAPPER_PUBLIC_API macro.
#endif /* #ifdef M_YAAL_DBWRAPPER_PUBLIC_API */
#ifdef M_YAAL_DBWRAPPER_EXTERN
#	error Yaal redefines M_YAAL_DBWRAPPER_EXTERN macro.
#endif /* #ifdef M_YAAL_DBWRAPPER_EXTERN */
#ifdef M_YAAL_HCONSOLE_PUBLIC_API
#	error Yaal redefines M_YAAL_HCONSOLE_PUBLIC_API macro.
#endif /* #ifdef M_YAAL_HCONSOLE_PUBLIC_API */
#ifdef M_YAAL_HCONSOLE_EXTERN
#	error Yaal redefines M_YAAL_HCONSOLE_EXTERN macro.
#endif /* #ifdef M_YAAL_HCONSOLE_EXTERN */
#ifdef M_YAAL_HDATA_PUBLIC_API
#	error Yaal redefines M_YAAL_HDATA_PUBLIC_API macro.
#endif /* #ifdef M_YAAL_HDATA_PUBLIC_API */
#ifdef M_YAAL_HDATA_EXTERN
#	error Yaal redefines M_YAAL_HDATA_EXTERN macro.
#endif /* #ifdef M_YAAL_HDATA_EXTERN */

/*! \brief gettext library API convenience macro.
 */
#define _(string) dgettext( PACKAGE_NAME, string )

/*! \cond */
#define M_EXPAND( x ) x
#define M_STRINGIFY_REAL( x ) #x
#define M_CONCAT_REAL( a, b ) a ## b
#define M_MACRO_ARGC_HELPER( MACRO, NUM, ... ) M_EXPAND( M_CONCAT( MACRO, NUM )( __VA_ARGS__ ) )
#define M_NUM_ARGC( ... ) M_EXPAND( M_NUM_ARGC_COUNTER( __VA_ARGS__, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 ) )
#define M_NUM_ARGC_COUNTER( a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, N, ... ) N
/*! \endcond */
/*! \brief Create unique C++ identifier from two other identifier.
 *
 * \param a - first part of identifier.
 * \param b - second part of identifier (usually __LINE__ or __COUNTER__).
 * \return EVAL(a)EVAL(b) - where EVAL(x) is x preprocessed by CPP.
 */
#define M_CONCAT( a, b ) M_CONCAT_REAL( a, b )
#define M_STRINGIFY( x ) M_STRINGIFY_REAL( x )
/*! \brief Create call which name differs based on number of arguments passed to this macro.
 *
 * \param MACRO - name for a call to make.
 * \param ... - any number of arguments that shall be passed to MACRO call.
 * \return A call to MACRO_n( ... ), where n is number of argument originally passed to M_MACRO_ARGC.
 */
#define M_MACRO_ARGC( MACRO, ... ) M_MACRO_ARGC_HELPER( MACRO, M_NUM_ARGC( __VA_ARGS__ ), __VA_ARGS__ )
/*! \brief Create automatic C++ identifier name.
 *
 * \param name - base for C++ identifier name.
 * \return Automatic C++ identifier name.
 */
#define M_YAAL_AUTO_ID( name ) M_CONCAT( name, __LINE__ )
#ifndef __ID__
#define __ID__ __FILE__ " " __DATE__ " " __TIME__
#endif /* #ifndef __ID__ */
#ifndef __TID__
#define __TID__ ""
#endif /* #ifndef __TID__ */
/*! \brief Create object file identifier for ident(1) program.
 */
#define M_VCSID( id ) namespace { static char const M_CONCAT( wrapper_VCSID, __LINE__ )[] __attribute__((unused)) = id; }
/*! \brief Throw auto generated exception.
 *
 * \param msg - message for exception.
 * \param e_no - error code for exception.
 */
#define M_THROW( msg, e_no ) yaal::hcore::throw_exception<this_type>( __FILE__, __LINE__, __PRETTY_FUNCTION__, msg, e_no )
/*! \brief First statement of every exception guarded function/method.
 */
#define M_PROLOG try {
/*! \brief Last statement of every exception guarded function/method.
 */
#define M_EPILOG } catch ( yaal::hcore::HException& e ) { e.log( __FILE__, __LINE__, __PRETTY_FUNCTION__ ); throw; }
#define M_DESTRUCTOR_EPILOG } \
	catch ( yaal::hcore::HException& e ) { \
		e.log( __FILE__, __LINE__, __PRETTY_FUNCTION__ ); \
		yaal::hcore::kill_interior( yaal::hcore::HString( __FILE__ ":" M_STRINGIFY( __LINE__ ) ": destructor of `" ).append( yaal::hcore::type_name( this ) ).append( "' throws `" ).append( e.what() ).append( "' in `" ).append( __PRETTY_FUNCTION__ ).append( "'." ) ); \
	} catch ( ... ) { \
		yaal::hcore::kill_interior( yaal::hcore::HString( __FILE__ ":" M_STRINGIFY( __LINE__ ) ": destructor of `" ).append( yaal::hcore::type_name( this ) ).append( "' throws in `" ).append( __PRETTY_FUNCTION__ ).append( "'." ) ); \
	}
#define M_SAFE( code ) do { \
	try { \
		code; \
	} catch ( yaal::hcore::HException& e ) { \
		e.log( __FILE__, __LINE__, __PRETTY_FUNCTION__ ); \
		yaal::hcore::kill_interior( yaal::hcore::HString( __FILE__ ":" M_STRINGIFY( __LINE__ ) ": `" #code "' throws `" ).append( e.what() ).append( "' in `" ).append( __PRETTY_FUNCTION__ ).append( "'." ) ); \
	} catch ( ... ) { \
		yaal::hcore::kill_interior( yaal::hcore::HString( __FILE__ ":" M_STRINGIFY( __LINE__ ) ": `" #code "' throws in `" ).append( __PRETTY_FUNCTION__ ).append( "'." ) ); \
	} \
} while ( 0 )
/*! \brief Last statement in <tt>int main( int, char** )</tt>.
 */
#define M_FINAL } \
	catch ( yaal::hcore::HException& e ) { \
		e.log( __FILE__, __LINE__, __PRETTY_FUNCTION__ ); \
		e.print_error(); \
		return ( -1 ); \
	} catch ( yaal::hcore::HFailedAssertion const& ) { \
		return ( -2 ); \
	} catch ( int retVal ) { \
		return ( retVal ? retVal : -3 ); \
	}
/*! \brief Throw HExceptionT<> is condition is not met.
 *
 * Use this macro to test return status of low-level system function calls.
 *
 * \param condition - condition to be tested.
 */
#define M_ENSURE( ... ) M_MACRO_ARGC( M_ENSURE_, __VA_ARGS__ )
#define M_ENSURE_1( condition ) do { if ( ! ( condition ) ) { yaal::hcore::throw_exception<this_type>( __FILE__, __LINE__, __PRETTY_FUNCTION__, error_message( errno ), errno ); } } while ( 0 )
#define M_ENSURE_2( condition, comment ) do { if ( ! ( condition ) ) { yaal::hcore::throw_exception<this_type>( __FILE__, __LINE__, __PRETTY_FUNCTION__, error_message( errno ), errno, comment ); } } while ( 0 )
#define M_ENSURE_3( condition, comment, errNo ) do { if ( ! ( condition ) ) { yaal::hcore::throw_exception<this_type>( __FILE__, __LINE__, __PRETTY_FUNCTION__, error_message( errNo ), errNo, comment ); } } while ( 0 )
#define M_ENSURE_T( condition, type ) do { if ( ! ( condition ) ) { yaal::hcore::throw_exception<type>( __FILE__, __LINE__, __PRETTY_FUNCTION__, error_message( errno ), errno ); } } while ( 0 )
#ifndef NDEBUG
/*! \brief Run-time assertion tester.
 *
 * This macro has meaning only in debug build.
 *
 * \param condition - condition to be tested.
 * \post Normal program flow is continued only if condition is met, otherwise HFailedAssertion exception is thrown.
 */
#	define M_ASSERT( condition ) do { if ( ! ( condition ) ) yaal::hcore::failed_assert( __FILE__, __LINE__, __PRETTY_FUNCTION__, #condition ); } while ( 0 )
# define M_DEBUG_CODE( code ) code
# define M_NDEBUG_CODE( code ) /**/
#else /* #ifndef NDEBUG */
#	define M_ASSERT( c ) /**/
# define M_DEBUG_CODE( code ) /**/
# define M_NDEBUG_CODE( code ) code
#endif /* #else #ifndef NDEBUG */
/*! \brief Run given code exactly once at the end of current scope.
 *
 * \param code - code to be run.
 */
#if defined( HAVE_DECL_TEMP_FAILURE_RETRY ) && ( HAVE_DECL_TEMP_FAILURE_RETRY == 1 )
/*! \brief Reimplement TEMP_FAILURE_RETRY macro.
 * Some of the original implementations use C-style-casts which messes our build.
 */
#	define M_TEMP_FAILURE_RETRY( expr ) ( __extension__( { \
	int long __result( 0 ); \
	do __result = static_cast<int long>( expr ); \
	while ( ( __result == -1 ) && ( errno == EINTR ) ); \
	__result; \
} ) )
#else /* #if defined( HAVE_DECL_TEMP_FAILURE_RETRY ) && ( HAVE_DECL_TEMP_FAILURE_RETRY == 1 ) */
#	define M_TEMP_FAILURE_RETRY( expr ) ( expr )
#endif /* #else #if defined( HAVE_DECL_TEMP_FAILURE_RETRY ) && ( HAVE_DECL_TEMP_FAILURE_RETRY == 1 ) */

#ifdef __MSVCXX__
#define M_ATTRIBUTE_SPEC( ... ) __declspec( __VA_ARGS__ )
#else /* #ifdef __MSVCXX__ */
#define M_ATTRIBUTE_SPEC( ... ) __attribute__(( __VA_ARGS__ ))
#endif /* #else #ifdef __MSVCXX__ */

#if defined( __MSVCXX__ ) || defined( __HOST_OS_TYPE_CYGWIN__ )
#define M_EXPORT_SYMBOL M_ATTRIBUTE_SPEC( dllexport )
#ifdef __YAAL_HCORE_BUILD__
#define M_YAAL_HCORE_PUBLIC_API M_ATTRIBUTE_SPEC( dllexport )
#define M_YAAL_HCORE_EXTERN
#else /* #ifdef __YAAL_HCORE_BUILD__ */
#define M_YAAL_HCORE_PUBLIC_API M_ATTRIBUTE_SPEC( dllimport )
#define M_YAAL_HCORE_EXTERN extern
#endif /* #else #ifdef __YAAL_HCORE_BUILD__ */
#ifdef __YAAL_TOOLS_BUILD__
#define M_YAAL_TOOLS_PUBLIC_API M_ATTRIBUTE_SPEC( dllexport )
#define M_YAAL_TOOLS_EXTERN
#else /* #ifdef __YAAL_TOOLS_BUILD__ */
#define M_YAAL_TOOLS_PUBLIC_API M_ATTRIBUTE_SPEC( dllimport )
#define M_YAAL_TOOLS_EXTERN extern
#endif /* #else #ifdef __YAAL_TOOLS_BUILD__ */
#ifdef __YAAL_DBWRAPPER_BUILD__
#define M_YAAL_DBWRAPPER_PUBLIC_API M_ATTRIBUTE_SPEC( dllexport )
#define M_YAAL_DBWRAPPER_EXTERN
#else /* #ifdef __YAAL_DBWRAPPER_BUILD__ */
#define M_YAAL_DBWRAPPER_PUBLIC_API M_ATTRIBUTE_SPEC( dllimport )
#define M_YAAL_DBWRAPPER_EXTERN extern
#endif /* #else #ifdef __YAAL_DBWRAPPER_BUILD__ */
#ifdef __YAAL_HCONSOLE_BUILD__
#define M_YAAL_HCONSOLE_PUBLIC_API M_ATTRIBUTE_SPEC( dllexport )
#define M_YAAL_HCONSOLE_EXTERN
#else /* #ifdef __YAAL_HCONSOLE_BUILD__ */
#define M_YAAL_HCONSOLE_PUBLIC_API M_ATTRIBUTE_SPEC( dllimport )
#define M_YAAL_HCONSOLE_EXTERN extern
#endif /* #else #ifdef __YAAL_HCONSOLE_BUILD__ */
#ifdef __YAAL_HDATA_BUILD__
#define M_YAAL_HDATA_PUBLIC_API M_ATTRIBUTE_SPEC( dllexport )
#define M_YAAL_HDATA_EXTERN
#else /* #ifdef __YAAL_HDATA_BUILD__ */
#define M_YAAL_HDATA_PUBLIC_API M_ATTRIBUTE_SPEC( dllexport )
#define M_YAAL_HDATA_EXTERN extern
#endif /* #else #ifdef __YAAL_HDATA_BUILD__ */
#else /* #if defined( __MSVCXX__ ) || defined( __HOST_OS_TYPE_CYGWIN__ ) */
#define M_EXPORT_SYMBOL /**/
#define M_YAAL_HCORE_PUBLIC_API /**/
#define M_YAAL_HCORE_EXTERN /**/
#define M_YAAL_TOOLS_PUBLIC_API /**/
#define M_YAAL_TOOLS_EXTERN /**/
#define M_YAAL_DBWRAPPER_PUBLIC_API /**/
#define M_YAAL_DBWRAPPER_EXTERN /**/
#define M_YAAL_HCONSOLE_PUBLIC_API /**/
#define M_YAAL_HCONSOLE_EXTERN /**/
#define M_YAAL_HDATA_PUBLIC_API /**/
#define M_YAAL_HDATA_EXTERN /**/
#undef __declspec
#define __declspec( x ) /**/
#endif /* #else #ifdef __MSVCXX__ */

#endif /* #ifndef YAAL_HCORE_MACRO_HXX_INCLUDED */

