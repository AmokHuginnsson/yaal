/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	base.h - this file is integral part of `yaal' project.

	i.  You may not make any changes in Copyright information.
	ii. You must attach Copyright information to any part of every copy
	    of this software.

Copyright:

 You are free to use this program as is, you can redistribute binary
 package freely but:
  1. You can not use any part of sources of this software.
  2. You can not redistribute any part of sources of this software.
  3. No reverse engineering is allowed.
  4. If you want redistribute binary package you can not demand any fees
     for this software.
     You can not even demand cost of the carrier (CD for example).
  5. You can not include it to any commercial enterprise (for example 
     as a free add-on to payed software or payed newspaper).
 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE. Use it at your own risk.
*/

#ifndef __YAAL_HCORE_BASE_H
#define __YAAL_HCORE_BASE_H

#include "hcore/hexception.h"

/*! \brief All library API belongs here.
 *
 * This namaspace contains all yaal library API and code.
 * Stdhapi code and API is divided into several sections.
 *
 * yaal comes from YAAL which comes from Y.A.A.L. which comes from:
 *  - Yet
 *  - Another
 *  - Abstraction
 *  - Layer
 */
namespace yaal
{

template<bool> struct static_assert_failure;
template<> struct static_assert_failure<true> { enum { value = 1 }; };
#define STATIC_ASSERT( condition ) { typedef char SAF##__LINE__ [ static_assert_failure<( condition )>::value ]; }

#define _(string) gettext (string)

#define M_VCSID(id) namespace { static char const* const wrapper_VCSID( void ) { wrapper_VCSID(); return id; } }
#define M_VCSTID(id) namespace { static char const* const wrapper_VCSTID( void ) { wrapper_VCSTID(); return id; } }
#define M_THROW( msg, e_no ) throw ( yaal::hcore::HException ( __FILE__, __PRETTY_FUNCTION__, __LINE__, msg, static_cast < int > ( e_no ) ) )
#define M_PROLOG try{
#define M_EPILOG } catch ( yaal::hcore::HException & e ){ e.log ( __FILE__, __PRETTY_FUNCTION__, __LINE__ ); throw; }
#define M_FINAL } catch ( yaal::hcore::HException & e ){ e.log ( __FILE__, __PRETTY_FUNCTION__, __LINE__ ); e.print_error ( true ); } catch ( int const & error_code ) { exit ( error_code ); }
#define M_ENSURE( condition ) if ( ! ( condition ) ){ yaal::hcore::HException e ( __FILE__, __PRETTY_FUNCTION__, __LINE__, #condition, errno ); e.set ( strerror ( errno ) ); throw e; }
#ifndef NDEBUG
#	define M_ASSERT( condition ) if ( ! ( condition ) )yaal::hcore::HException::failed_assert ( __FILE__, __PRETTY_FUNCTION__, __LINE__, #condition )
#else /* NDEBUG */
#	define M_ASSERT( c ) /**/
#endif /* not NDEBUG */

template<int long unsigned const input>
struct binary
	{
	static int long unsigned const value = ( binary<input / 10>::value << 1 ) + ( input % 10 );
	};

template<>
struct binary<0>
	{
	static int long unsigned const value = 0;
	};

template<typename tType>
tType min( tType left, tType right )
	{
	return ( left < right ? left : right );
	}

template<typename tType>
tType max( tType left, tType right )
	{
	return ( left >= right ? left : right );
	}

template<typename tType>
tType abs( tType val )
	{
	return ( val >= 0 ? val : - val );
	}

template<typename tType>
bool less( tType const& a_rtLeft, tType const& a_rtRight )
	{
	return ( a_rtLeft < a_rtRight );
	}

template<typename tType>
inline tType const operator | ( tType const& left,
		tType const& right )
	{
	return ( static_cast < tType > ( static_cast < int long unsigned > ( left )
				| static_cast < int long unsigned > ( right ) ) );
	}
template<typename tType>
inline tType& operator |= ( tType& left, tType const& right )
	{
	left = static_cast < tType > ( static_cast < int long unsigned > ( left )
			| static_cast < int long unsigned > ( right ) );
	return ( left );
	}
template<typename tType>
inline tType const operator & ( tType const& left,
		tType const& right )
	{
	return ( static_cast < tType > ( static_cast < int long unsigned > ( left )
				& static_cast < int long unsigned > ( right ) ) );
	}
template<typename tType>
inline tType& operator &= ( tType& left, tType const& right )
	{
	left = static_cast < tType > ( static_cast < int long unsigned > ( left )
			& static_cast < int long unsigned > ( right ) );
	return ( left );
	}
template<typename tType>
inline tType const operator ^ ( tType const& left,
		tType const& right )
	{
	return ( static_cast < tType > ( static_cast < int long unsigned > ( left )
				^ static_cast < int long unsigned > ( right ) ) );
	}
template<typename tType>
inline tType& operator ^= ( tType& left, tType const& right )
	{
	left = static_cast < tType > ( static_cast < int long unsigned > ( left )
			^ static_cast < int long unsigned > ( right ) );
	return ( left );
	}
template<typename tType>
inline tType const operator ~ ( tType const& e )
	{
	return ( static_cast < tType > (
				~ static_cast < int long unsigned > ( e ) ) );
	}

/* those types definitions were in hinfo.h but this file (hexception.h)
 * is included into more files, we assume that sizeof ( int ) >= 4 */

typedef enum
	{
	D_VOID					= 0x0000,
	D_BOOL					= 0x0001,
	D_CHAR					= 0x0002,
	D_SHORT					= 0x0004,
	D_INT						= 0x0008,
	D_LONG_INT			= 0x0010,
	D_DOUBLE				= 0x0020,
	D_POINTER				= 0x0040,
	D_CHAR_POINTER	= 0x0080,
	D_HSTRING				= 0x0100,
	D_HINFO					= 0x0200,
	D_HHASHMAP			= 0x0400,
	D_HLIST					= 0x0800,
	D_HTIME					= 0x1000,
	D_MASK					= 0xffff
	} type_t;

template<typename tType>
tType& clone( tType& object )
	{
	typedef typeof( *object ) ttType;
	return ( tType( new ttType( *object ) ) );
	}

}

#endif /* not __YAAL_HCORE_BASE_H */
