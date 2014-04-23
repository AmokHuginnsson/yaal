/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	macro.hxx - this file is integral part of `yaal' project.

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

/*! \file hcore/macro.hxx
 * \brief Globally visible preprocessor macros are stored here.
 */

#ifndef YAAL_HCORE_MACRO_HXX_INCLUDED
#define YAAL_HCORE_MACRO_HXX_INCLUDED 1

#ifdef _
#	error Yaal redefines _ macro.
#endif /* #ifdef _ */
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
#ifdef M_ENSURE_HELPER
#	error Yaal redefines M_ENSURE_HELPER macro.
#endif /* #ifdef M_ENSURE_HELPER */
#ifdef M_ENSURE_HELPER_FORWARD
#	error Yaal redefines M_ENSURE_HELPER_FORWARD macro.
#endif /* #ifdef M_ENSURE_HELPER_FORWARD */
#ifdef M_ENSURE_1
#	error Yaal redefines M_ENSURE_1 macro.
#endif /* #ifdef M_ENSURE_1 */
#ifdef M_ENSURE_2
#	error Yaal redefines M_ENSURE_2 macro.
#endif /* #ifdef M_ENSURE_2 */
#ifdef M_NUM_ARGS
#	error Yaal redefines M_NUM_ARGS macro.
#endif /* #ifdef M_NUM_ARGS */
#ifdef M_NUM_ARGS_COUNTER
#	error Yaal redefines M_NUM_ARGS_COUNTER macro.
#endif /* #ifdef M_NUM_ARGS_COUNTER */
#ifdef M_ENSURE_EX
#	error Yaal redefines M_ENSURE_EX macro.
#endif /* #ifdef M_ENSURE_EX */
#ifdef M_ASSERT
#	error Yaal redefines M_ASSERT macro.
#endif /* #ifdef M_ASSERT */
#ifdef M_AT_END_OF_SCOPE
#	error Yaal redefines M_AT_END_OF_SCOPE macro.
#endif /* #ifdef M_AT_END_OF_SCOPE */
#ifdef M_TEMP_FAILURE_RETRY
#	error Yaal redefines M_TEMP_FAILURE_RETRY macro.
#endif /* #ifdef M_TEMP_FAILURE_RETRY */
#ifdef countof
#	error Yaal redefines countof macro.
#endif /* #ifdef countof */
#ifdef M_EXPORT_SYMBOL
#	error Yaal redefines M_EXPORT_SYMBOL macro.
#endif /* #ifdef M_EXPORT_SYMBOL */
#ifdef M_YAAL_HCORE_PUBLIC_API
#	error Yaal redefines M_YAAL_HCORE_PUBLIC_API macro.
#endif /* #ifdef M_YAAL_HCORE_PUBLIC_API */
#ifdef M_YAAL_TOOLS_PUBLIC_API
#	error Yaal redefines M_YAAL_TOOLS_PUBLIC_API macro.
#endif /* #ifdef M_YAAL_TOOLS_PUBLIC_API */
#ifdef M_YAAL_DBWRAPPER_PUBLIC_API
#	error Yaal redefines M_YAAL_DBWRAPPER_PUBLIC_API macro.
#endif /* #ifdef M_YAAL_DBWRAPPER_PUBLIC_API */
#ifdef M_YAAL_HCONSOLE_PUBLIC_API
#	error Yaal redefines M_YAAL_HCONSOLE_PUBLIC_API macro.
#endif /* #ifdef M_YAAL_HCONSOLE_PUBLIC_API */
#ifdef M_YAAL_HDATA_PUBLIC_API
#	error Yaal redefines M_YAAL_HDATA_PUBLIC_API macro.
#endif /* #ifdef M_YAAL_HDATA_PUBLIC_API */

/*! \brief gettext library API convenience macro.
 */
#define _(string) dgettext( PACKAGE_NAME, string )

/*! \cond */
#define M_STRINGIFY_REAL( x ) #x
#define M_CONCAT_REAL( a, b ) a ## b
/*! \endcond */
/*! \brief Create uniqe C++ identifier from two other identifier.
 *
 * \param a - first part of identifier.
 * \param b - second part of identifier (usually __LINE__ or __COUNTER__).
 * \return EVAL(a)EVAL(b) - where EVAL(x) is x preprocessed by CPP.
 */
#define M_CONCAT( a, b ) M_CONCAT_REAL( a, b )
#define M_STRINGIFY( x ) M_STRINGIFY_REAL( x )
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
		yaal::hcore::kill_interior( ( __FILE__ ":" M_STRINGIFY( __LINE__ ) ": destructor of `" + yaal::hcore::type_name( this ) + "' throws `" + e.what() + "' in `" + __PRETTY_FUNCTION__ + "'." ).raw() ); \
	} catch ( ... ) { \
		yaal::hcore::kill_interior( ( __FILE__ ":" M_STRINGIFY( __LINE__ ) ": destructor of `" + yaal::hcore::type_name( this ) + "' throws in `" + __PRETTY_FUNCTION__ + "'." ).raw() ); \
	}
#define M_SAFE( code ) do { \
	try { \
		code; \
	} catch ( yaal::hcore::HException& e ) { \
		e.log( __FILE__, __LINE__, __PRETTY_FUNCTION__ ); \
		yaal::hcore::kill_interior( ( yaal::hcore::HString( __FILE__ ":" M_STRINGIFY( __LINE__ ) ": `" #code "' throws `" ) + e.what() + "' in `" + __PRETTY_FUNCTION__ + "'." ).raw() ); \
	} catch ( ... ) { \
		yaal::hcore::kill_interior( ( yaal::hcore::HString( __FILE__ ":" M_STRINGIFY( __LINE__ ) ": `" #code "' throws in `" ) + __PRETTY_FUNCTION__ + "'." ).raw() ); \
	} \
} while ( 0 )
/*! \brief Last statement in <tt>int main( int, char** )</tt>.
 */
#define M_FINAL } catch ( yaal::hcore::HException& e ) { e.log( __FILE__, __LINE__, __PRETTY_FUNCTION__ ); e.print_error(); return ( -1 ); } catch ( yaal::hcore::HFailedAssertion const& ) { return ( -2 ); } catch ( int retVal ) { return ( retVal ? retVal : -3 ); }
/*! \brief Throw HExceptionT<> is condition is not met.
 *
 * Use this macro to test return status of low-level system function calls.
 *
 * \param condition - condition to be tested.
 */
#define M_ENSURE( ... ) M_ENSURE_HELPER_FORWARD( M_NUM_ARGS( __VA_ARGS__ ), __VA_ARGS__ )
#define M_ENSURE_HELPER_FORWARD( NUM, ... ) M_ENSURE_HELPER( NUM, __VA_ARGS__ )
#define M_ENSURE_HELPER( NUM, ... ) M_ENSURE_##NUM( __VA_ARGS__ )
#define M_NUM_ARGS( ... ) M_NUM_ARGS_COUNTER( __VA_ARGS__, 2, 1 )
#define M_NUM_ARGS_COUNTER( a1, a2, N ) N
#define M_ENSURE_1( condition ) do { if ( ! ( condition ) ) { yaal::hcore::throw_exception<this_type>( __FILE__, __LINE__, __PRETTY_FUNCTION__, #condition, errno, error_message( errno ) ); } } while ( 0 )
#define M_ENSURE_2( condition, type ) do { if ( ! ( condition ) ) { yaal::hcore::throw_exception<type>( __FILE__, __LINE__, __PRETTY_FUNCTION__, #condition, errno, error_message( errno ) ); } } while ( 0 )
#define M_ENSURE_EX( condition, comment ) do { if ( ! ( condition ) ) { yaal::hcore::throw_exception<this_type>( __FILE__, __LINE__, __PRETTY_FUNCTION__, #condition, errno, error_message( errno ), comment ); } } while ( 0 )
#ifndef NDEBUG
/*! \brief Run-time assertion tester.
 *
 * This macro has meaning only in debug build.
 *
 * \param condition - condition to be tested.
 * \post Normal progam flow is continued only if condition is met, otherwise HFailedAssertion exception is thrown.
 */
#	define M_ASSERT( condition ) do { if ( ! ( condition ) ) yaal::hcore::failed_assert( __FILE__, __LINE__, __PRETTY_FUNCTION__, #condition ); } while ( 0 )
#else /* #ifndef NDEBUG */
#	define M_ASSERT( c ) /**/
#endif /* #else #ifndef NDEBUG */
/*! \brief Run given code exactly once at the end of currnt scope.
 *
 * \param code - code to be run.
 */
#define M_AT_END_OF_SCOPE( code ) \
class M_CONCAT( AtEndOfScope, __LINE__ ) { public: M_CONCAT( AtEndOfScope, __LINE__ )( void ) {} ~M_CONCAT( AtEndOfScope, __LINE__ )( void ) { do { code } while ( 0 ); } } M_CONCAT( atEndOfScope, __LINE__ )
#if defined( HAVE_DECL_TEMP_FAILURE_RETRY ) && ( HAVE_DECL_TEMP_FAILURE_RETRY == 1 )
/*! \brief Reinmplement TEMP_FAILURE_RETRY macro.
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
/*! \brief Convinience macro to obtain number of elements of declared array.
 */
#define countof( array ) ( yaal::meta::integer_cast<int, sizeof ( yaal::YaalArrayElementCountHelper( ( array ) ) )>::value )

#ifdef __MSVCXX__
#define M_EXPORT_SYMBOL __declspec( dllexport )
#ifdef __YAAL_HCORE_BUILD__
#define M_YAAL_HCORE_PUBLIC_API __declspec( dllexport )
#else /* #ifdef __YAAL_HCORE_BUILD__ */
#define M_YAAL_HCORE_PUBLIC_API __declspec( dllimport )
#endif /* #else #ifdef __YAAL_HCORE_BUILD__ */
#ifdef __YAAL_TOOLS_BUILD__
#define M_YAAL_TOOLS_PUBLIC_API __declspec( dllexport )
#else /* #ifdef __YAAL_TOOLS_BUILD__ */
#define M_YAAL_TOOLS_PUBLIC_API __declspec( dllimport )
#endif /* #else #ifdef __YAAL_TOOLS_BUILD__ */
#ifdef __YAAL_DBWRAPPER_BUILD__
#define M_YAAL_DBWRAPPER_PUBLIC_API __declspec( dllexport )
#else /* #ifdef __YAAL_DBWRAPPER_BUILD__ */
#define M_YAAL_DBWRAPPER_PUBLIC_API __declspec( dllimport )
#endif /* #else #ifdef __YAAL_DBWRAPPER_BUILD__ */
#ifdef __YAAL_HCONSOLE_BUILD__
#define M_YAAL_HCONSOLE_PUBLIC_API __declspec( dllexport )
#else /* #ifdef __YAAL_HCONSOLE_BUILD__ */
#define M_YAAL_HCONSOLE_PUBLIC_API __declspec( dllimport )
#endif /* #else #ifdef __YAAL_HCONSOLE_BUILD__ */
#ifdef __YAAL_HDATA_BUILD__
#define M_YAAL_HDATA_PUBLIC_API __declspec( dllexport )
#else /* #ifdef __YAAL_HDATA_BUILD__ */
#define M_YAAL_HDATA_PUBLIC_API __declspec( dllexport )
#endif /* #else #ifdef __YAAL_HDATA_BUILD__ */
#else /* #ifdef __MSVCXX__ */
#define M_EXPORT_SYMBOL /**/
#define M_YAAL_HCORE_PUBLIC_API /**/
#define M_YAAL_TOOLS_PUBLIC_API /**/
#define M_YAAL_DBWRAPPER_PUBLIC_API /**/
#define M_YAAL_HCONSOLE_PUBLIC_API /**/
#define M_YAAL_HDATA_PUBLIC_API /**/
#undef __declspec
#define __declspec( x ) /**/
#endif /* #else #ifdef __MSVCXX__ */

#endif /* #ifndef YAAL_HCORE_MACRO_HXX_INCLUDED */

