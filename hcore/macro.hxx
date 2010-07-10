/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	macro.hxx - this file is integral part of `yaal' project.

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

/*! \file hcore/macro.hxx
 * \brief Globaly visible preprocessor macros are stored here.
 */

#ifndef YAAL_HCORE_MACRO_HXX_INCLUDED
#define YAAL_HCORE_MACRO_HXX_INCLUDED 1

/*! \brief gettext library API convenience macro.
 */
#define _(string) dgettext( PACKAGE_NAME, string )

/*! \cond */
#define M_CONCAT_REAL( a, b ) a ## b
/*! \endcond */
/*! \brief Create uniqe C++ identifier from two other identifier.
 *
 * \param a - first part of identifier.
 * \param b - second part of identifier (usually __LINE__ or __COUNTER__).
 * \return EVAL(a)EVAL(b) - where EVAL(x) is x preprocessed by CPP.
 */
#define M_CONCAT( a, b ) M_CONCAT_REAL( a, b )
/*! \brief Create object file identifier for ident(1) program.
 */
#define M_VCSID(id) namespace { static char const* M_CONCAT(wrapper_VCSID, __LINE__)( void ) { M_CONCAT( wrapper_VCSID, __LINE__)(); return ( id ); } }
/*! \brief Throw auto generated exception.
 *
 * \param msg - message for exception.
 * \param e_no - error code for exception.
 */
#define M_THROW( msg, e_no ) yaal::hcore::throw_exception<this_type>( __FILE__, __PRETTY_FUNCTION__, __LINE__, msg, e_no )
/*! \brief First statement of every exception guarder function/method.
 */
#define M_PROLOG try{
/*! \brief Last statement of every exception guarder function/method.
 */
#define M_EPILOG } catch ( yaal::hcore::HException& e ){ e.log ( __FILE__, __PRETTY_FUNCTION__, __LINE__ ); throw; }
/*! \brief Last statement in <tt>int main( int, char** )</tt>.
 */
#define M_FINAL } catch ( yaal::hcore::HException& e ){ e.log ( __FILE__, __PRETTY_FUNCTION__, __LINE__ ); e.print_error(); } catch ( yaal::hcore::HFailedAssertion& ) { exit( -1 ); } catch ( int const& retVal ) { return ( retVal ); }
/*! \brief Throw HExceptionT<> is condition is not met.
 *
 * Use this macro to test return status of low-level system function calls.
 *
 * \param condition - condition to be tested.
 */
#define M_ENSURE( condition ) do { if ( ! ( condition ) ){ yaal::hcore::throw_exception<this_type>( __FILE__, __PRETTY_FUNCTION__, __LINE__, #condition, errno, error_message( errno ) ); } } while ( 0 )
#define M_ENSURE_EX( condition, comment ) do { if ( ! ( condition ) ){ yaal::hcore::throw_exception<this_type>( __FILE__, __PRETTY_FUNCTION__, __LINE__, #condition, errno, error_message( errno ), comment ); } } while ( 0 )
#ifndef NDEBUG
/*! \brief Run-time assertion tester.
 *
 * This macro has meaning only in debug build.
 *
 * \param condition - condition to be tested.
 * \post Normal progam flow is continued only if condition is met, otherwise HFailedAssertion exception is thrown.
 */
#	define M_ASSERT( condition ) do { if ( ! ( condition ) )yaal::hcore::HException::failed_assert( __FILE__, __PRETTY_FUNCTION__, __LINE__, #condition ); } while ( 0 )
#else /* NDEBUG */
#	define M_ASSERT( c ) /**/
#endif /* not NDEBUG */
/*! \brief Run given code exactly once at the end of currnt scope.
 *
 * \param code - code to be run.
 */
#define M_AT_END_OF_SCOPE( code ) \
class M_CONCAT( AtEndOfScope, __LINE__ ) { public: M_CONCAT( AtEndOfScope, __LINE__ )( void ) {} ~M_CONCAT( AtEndOfScope, __LINE__ )( void ) { do { code } while ( 0 ); } } M_CONCAT( atEndOfScope, __LINE__ )
/*! \brief Convinience macro to obtain number of elements of declared array.
 */
#define countof( array ) ( static_cast<int long>( sizeof ( yaal::YaalArrayElementCountHelper( ( array ) ) ) ) )

#ifdef __MSVCXX__
#ifdef __YAAL_HCORE_BUILD__
#define M_YAAL_HCORE_PUBLIC_API __declspec( dllexport )
#else /* __YAAL_HCORE_BUILD__ */
#define M_YAAL_HCORE_PUBLIC_API __declspec( dllimport )
#endif /* not __YAAL_HCORE_BUILD__ */
#ifdef __YAAL_TOOLS_BUILD__
#define M_YAAL_TOOLS_PUBLIC_API __declspec( dllexport )
#else /* __YAAL_TOOLS_BUILD__ */
#define M_YAAL_TOOLS_PUBLIC_API __declspec( dllimport )
#endif /* not __YAAL_TOOLS_BUILD__ */
#ifdef __YAAL_DBWRAPPER_BUILD__
#define M_YAAL_DBWRAPPER_PUBLIC_API __declspec( dllexport )
#else /* __YAAL_DBWRAPPER_BUILD__ */
#define M_YAAL_DBWRAPPER_PUBLIC_API __declspec( dllimport )
#endif /* not __YAAL_DBWRAPPER_BUILD__ */
#ifdef __YAAL_HCONSOLE_BUILD__
#define M_YAAL_HCONSOLE_PUBLIC_API __declspec( dllexport )
#else /* __YAAL_HCONSOLE_BUILD__ */
#define M_YAAL_HCONSOLE_PUBLIC_API __declspec( dllimport )
#endif /* not __YAAL_HCONSOLE_BUILD__ */
#ifdef __YAAL_HDATA_BUILD__
#define M_YAAL_HDATA_PUBLIC_API __declspec( dllexport )
#else /* __YAAL_HDATA_BUILD__ */
#define M_YAAL_HDATA_PUBLIC_API __declspec( dllimport )
#endif /* not __YAAL_HDATA_BUILD__ */
#else /* __MSVCXX__ */
#define M_YAAL_HCORE_PUBLIC_API /**/
#define M_YAAL_TOOLS_PUBLIC_API /**/
#define M_YAAL_DBWRAPPER_PUBLIC_API /**/
#define M_YAAL_HCONSOLE_PUBLIC_API /**/
#define M_YAAL_HDATA_PUBLIC_API /**/
#endif /* not __MSVCXX__ */

#endif /* not YAAL_HCORE_MACRO_HXX_INCLUDED */

