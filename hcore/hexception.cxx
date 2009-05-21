/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	hexception.cxx - this file is integral part of `yaal' project.

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

#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <csignal>
#include <libintl.h>
#include <cxxabi.h>

#include "base.hxx"
M_VCSID( "$Id: "__ID__" $" )
#include "hexception.hxx"
#include "xalloc.hxx"
#include "hlog.hxx"

using namespace yaal;

namespace yaal
{

namespace hcore
{

void* HException::ERROR_STREAM = stderr;

char const* const n_pcExceptionType = _( "Exception type" );

HException::HException( char const* const a_pcFileName,
												 char const* const a_pcFunctionName,
												 int const a_iLine, char const* const a_pcMessage,
												 int const a_iCode )
	: f_bLocal( false ), f_cChar( 0 ), f_iInt( 0 ), f_lLong( 0 ),
	f_dDouble( 0 ), f_pcCharPtr( NULL ), f_pvVoidPtr( NULL ), f_iFrame( 0 ),
	f_pcFileName( NULL ), f_pcFunctionName( NULL ),
	f_iCode( a_iCode ), f_pcMessage( NULL )
	{
	f_pcMessage = xstrdup( a_pcMessage );
	f_pcFileName = xstrdup( a_pcFileName );
	f_pcFunctionName = xstrdup( a_pcFunctionName );
	hcore::log << "Exception: " << f_pcMessage << ", code: " << f_iCode;
	hcore::log << '.' << endl;
	log( a_pcFileName, a_pcFunctionName, a_iLine );
	return;
	}

HException::HException( char const* const a_pcFileName,
												 char const* const a_pcFunctionName,
												 int const a_iLine, HString const& a_oMessage,
												 int const a_iCode )
	: f_bLocal( false ), f_cChar( 0 ), f_iInt( 0 ), f_lLong( 0 ),
	f_dDouble( 0 ), f_pcCharPtr( NULL ), f_pvVoidPtr( NULL ), f_iFrame( 0 ),
	f_pcFileName( NULL ), f_pcFunctionName( NULL ),
	f_iCode( a_iCode ), f_pcMessage( NULL )
	{
	f_pcMessage = xstrdup( a_oMessage.raw() );
	f_pcFileName = xstrdup( a_pcFileName );
	f_pcFunctionName = xstrdup( a_pcFunctionName );
	hcore::log << "Exception: " << f_pcMessage << ", code: " << f_iCode;
	hcore::log << '.' << endl;
	log( a_pcFileName, a_pcFunctionName, a_iLine );
	return;
	}

HException::HException( HException const& a_roException )
	: f_bLocal( false ), f_cChar( a_roException.f_cChar ),
	f_iInt( a_roException.f_iInt ), f_lLong( a_roException.f_lLong ),
	f_dDouble( a_roException.f_dDouble ), f_pcCharPtr( NULL ),
	f_pvVoidPtr( a_roException.f_pvVoidPtr ),
	f_iFrame( a_roException.f_iFrame ), f_pcFileName( NULL ),
	f_pcFunctionName( NULL ), f_iCode( a_roException.f_iCode ),
	f_pcMessage( NULL )
	{
	if ( a_roException.f_pcCharPtr )
		f_pcCharPtr = xstrdup( a_roException.f_pcCharPtr );
	f_pcMessage = xstrdup( a_roException.f_pcMessage );
	f_pcFileName = xstrdup( a_roException.f_pcFileName );
	f_pcFunctionName = xstrdup( a_roException.f_pcFunctionName );
	a_roException.f_bLocal = true;
	return;
	}

HException::~HException ( void )
	{
	try
		{
		if ( ! f_bLocal )
			hcore::log(
					"Exception registers: c:0x%02x i:%d l:%ld d:%f pv:0x%lx pc:%s\n",
					f_cChar, f_iInt, f_lLong, f_dDouble,
					reinterpret_cast<int long>( f_pvVoidPtr ),
					f_pcCharPtr ? f_pcCharPtr : "(null)" );
		}
	catch ( ... )
		{
		::fprintf( static_cast<FILE*>( ERROR_STREAM ), _( "CRITICAL FAILURE (~HException) !\n" ) );
		::exit( - 1 );
		}
	if ( f_pcCharPtr )
		xfree( f_pcCharPtr );
	if ( f_pcFunctionName )
		xfree( f_pcFunctionName );
	if ( f_pcFileName )
		xfree( f_pcFileName );
	if ( f_pcMessage )
		xfree( f_pcMessage );
	return;
	}

void HException::set( char const a_cChar, int const a_iInt,
		long const a_lLong, double const a_dDouble, char const* const a_pcStr,
		void* const a_pvVoidPtr )
	{
	f_cChar = a_cChar;
	f_iInt = a_iInt;
	f_lLong = a_lLong;
	f_dDouble = a_dDouble;
	if ( f_pcCharPtr )
		xfree( f_pcCharPtr );
	if ( a_pcStr )
		f_pcCharPtr = xstrdup( a_pcStr );
	f_pvVoidPtr = a_pvVoidPtr;
	return;
	}

void HException::set( char const* const a_pcStr )
	{
	if ( f_pcCharPtr )
		xfree( f_pcCharPtr );
	if ( a_pcStr )
		f_pcCharPtr = xstrdup( a_pcStr );
	return;
	}

void HException::set( HString const& a_oStr )
	{
	if ( f_pcCharPtr )
		xfree( f_pcCharPtr );
	if ( ! a_oStr.is_empty() )
		f_pcCharPtr = xstrdup( a_oStr.raw() );
	return;
	}

void HException::print_error( bool const a_bFull ) const
	{
	fprintf ( static_cast<FILE*>( ERROR_STREAM ), "\nException: %s, %d.\n", f_pcMessage, f_iCode );
	if ( a_bFull )
		fprintf ( static_cast<FILE*>( ERROR_STREAM ),
				"Exception registers:\nc:0x%02x\ti:%d\tl:%ld\td:%f\tpv:0x%lx\npc:%s\n",
				f_cChar, f_iInt, f_lLong, f_dDouble,
				reinterpret_cast<int long>( f_pvVoidPtr ),
				f_pcCharPtr ? f_pcCharPtr : "(null)" );
	return;
	}

void HException::log( char const* const a_pcFileName,
											 char const* const a_pcFunctionName, int const a_iLine )
	{
	size_t l_uiLength = ::strlen( a_pcFileName );
	if ( f_iFrame
			&& ! ( ::strcmp( f_pcFileName, a_pcFileName )
				|| ::strcmp( f_pcFunctionName, a_pcFunctionName ) ) )
		return;
#define M_YAAL_HCORE_HEXCEPTION_LOG_LOG_DATA \
				"Exception frame %2d: %16s : %4d : %s\n", f_iFrame,\
				a_pcFileName + ( l_uiLength > 16 ? l_uiLength - 16 : 0 ),\
				a_iLine, a_pcFunctionName
	if ( n_iDebugLevel >= DEBUG_LEVEL::PRINT_EXCEPTION_STACK )
		fprintf( static_cast<FILE*>( ERROR_STREAM ), M_YAAL_HCORE_HEXCEPTION_LOG_LOG_DATA );
	hcore::log( M_YAAL_HCORE_HEXCEPTION_LOG_LOG_DATA );
	++ f_iFrame;
#undef M_YAAL_HCORE_HEXCEPTION_LOG_LOG_DATA
	return;
	}

char const* HException::what( void ) const
	{
	return ( f_pcMessage );
	}

int HException::code( void ) const
	{
	return ( f_iCode );
	}

void HException::failed_assert( char const* const a_pcFileName,
		char const* const a_pcFunctionName, int const a_iLine,
		char const* const a_pcMessage )
	{
	M_PROLOG
	hcore::log << "Failed assertion: " << a_pcMessage << " -> " << a_pcFileName << "(" << a_iLine << "): " << a_pcFunctionName << endl;
	fprintf( static_cast<FILE*>( ERROR_STREAM ), "Failed assertion: `%s' at: %s: %4d: %s\n",
			a_pcMessage, a_pcFileName, a_iLine, a_pcFunctionName );
	if ( ! errno )
		++ errno;
	if ( n_iDebugLevel >= DEBUG_LEVEL::ABORT_ON_ASSERT )
		::abort();
	throw ( HFailedAssertion( a_pcMessage ) );
	M_EPILOG
	}

void HException::set_error_stream( void* a_pvErrorStream )
	{
	M_PROLOG
	M_ASSERT( a_pvErrorStream );
	ERROR_STREAM = a_pvErrorStream;
	return;
	M_EPILOG
	}

char* HException::get_type_name( char const* const a_pcName )
	{
	int status = 0;
	return ( abi::__cxa_demangle( a_pcName, 0, 0, &status ) );
	}

void HException::cleanup( char* a_pcPtr )
	{
	if ( a_pcPtr )
		xfree( a_pcPtr );
	return;
	}

void HFailedAssertion::swap( HFailedAssertion& other )
	{
	using yaal::swap;
	if ( &other != this )
		swap( f_pcWhat, other.f_pcWhat );
	return;
	}

}

}

