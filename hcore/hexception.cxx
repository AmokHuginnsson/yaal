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
M_VCSID( "$Id: "__TID__" $" )
#include "hexception.hxx"
#include "xalloc.hxx"
#include "hlog.hxx"
#include "hfile.hxx"

using namespace yaal;

namespace yaal
{

namespace hcore
{

void* HException::ERROR_STREAM = stderr;

char const* const _exceptionType_ = _( "Exception type" );

HException::HException( char const* const fileName_,
		char const* const functionName_,
		int const line_, char const* const message_,
		int const code_ )
	: _local( false ), _char( 0 ), _int( 0 ), _long( 0 ),
	_double( 0 ), _charPtr( NULL ), _voidPtr( NULL ), _frame( 0 ),
	_fileName( NULL ), _functionName( NULL ),
	_code( code_ ), _message( NULL )
	{
	_message = xstrdup( message_ );
	_fileName = xstrdup( fileName_ );
	_functionName = xstrdup( functionName_ );
	hcore::log << "Exception: " << _message << ", code: " << _code;
	hcore::log << '.' << endl;
	log( fileName_, functionName_, line_ );
	return;
	}

HException::HException( char const* const fileName_,
		char const* const functionName_,
		int const line_, HString const& message_,
		int const code_ )
	: _local( false ), _char( 0 ), _int( 0 ), _long( 0 ),
	_double( 0 ), _charPtr( NULL ), _voidPtr( NULL ), _frame( 0 ),
	_fileName( NULL ), _functionName( NULL ),
	_code( code_ ), _message( NULL )
	{
	_message = xstrdup( message_.raw() );
	_fileName = xstrdup( fileName_ );
	_functionName = xstrdup( functionName_ );
	hcore::log << "Exception: " << _message << ", code: " << _code;
	hcore::log << '.' << endl;
	log( fileName_, functionName_, line_ );
	return;
	}

HException::HException( HException const& exception_ )
	: _local( false ), _char( exception_._char ),
	_int( exception_._int ), _long( exception_._long ),
	_double( exception_._double ), _charPtr( NULL ),
	_voidPtr( exception_._voidPtr ),
	_frame( exception_._frame ), _fileName( NULL ),
	_functionName( NULL ), _code( exception_._code ),
	_message( NULL )
	{
	if ( exception_._charPtr )
		_charPtr = xstrdup( exception_._charPtr );
	_message = xstrdup( exception_._message );
	_fileName = xstrdup( exception_._fileName );
	_functionName = xstrdup( exception_._functionName );
	exception_._local = true;
	return;
	}

HException::~HException( void )
	{
	try
		{
		if ( ! _local )
			{
			HString regs;
			regs.format(
					"Exception registers: c:0x%02x i:%d l:%ld d:%f pv:0x%lx pc:%s\n",
					_char, _int, _long, _double,
					reinterpret_cast<int long>( _voidPtr ),
					_charPtr ? _charPtr : "(null)" );
			hcore::log << regs << endl;
			}
		}
	catch ( ... )
		{
		::fprintf( static_cast<FILE*>( ERROR_STREAM ), _( "CRITICAL FAILURE (~HException) !\n" ) );
		::exit( -1 );
		}
	if ( _charPtr )
		xfree( _charPtr );
	if ( _functionName )
		xfree( _functionName );
	if ( _fileName )
		xfree( _fileName );
	if ( _message )
		xfree( _message );
	return;
	}

void HException::set( char const char_, int const int_,
		long const long_, double const double_, char const* const str_,
		void* const voidPtr_ )
	{
	_char = char_;
	_int = int_;
	_long = long_;
	_double = double_;
	if ( _charPtr )
		xfree( _charPtr );
	if ( str_ )
		_charPtr = xstrdup( str_ );
	_voidPtr = voidPtr_;
	return;
	}

void HException::set( char const* const str_ )
	{
	if ( _charPtr )
		xfree( _charPtr );
	if ( str_ )
		_charPtr = xstrdup( str_ );
	return;
	}

void HException::set( HString const& str_ )
	{
	if ( _charPtr )
		xfree( _charPtr );
	if ( ! str_.is_empty() )
		_charPtr = xstrdup( str_.raw() );
	return;
	}

void HException::print_error( bool const full_ ) const
	{
	fprintf( static_cast<FILE*>( ERROR_STREAM ), "\nException: %s, %d.\n", _message, _code );
	if ( full_ )
		fprintf( static_cast<FILE*>( ERROR_STREAM ),
				"Exception registers:\nc:0x%02x\ti:%d\tl:%ld\td:%f\tpv:0x%lx\npc:%s\n",
				_char, _int, _long, _double,
				reinterpret_cast<int long>( _voidPtr ),
				_charPtr ? _charPtr : "(null)" );
	return;
	}

void HException::log( char const* const fileName_,
											 char const* const functionName_, int const line_ )
	{
	size_t length = ::strlen( fileName_ );
	if ( _frame
			&& ! ( ::strcmp( _fileName, fileName_ )
				|| ::strcmp( _functionName, functionName_ ) ) )
		return;
	HString frame;
	frame.format(
				"Exception frame %2d: %16s : %4d : %s\n", _frame,
				fileName_ + ( length > 16 ? length - 16 : 0 ),
				line_, functionName_ );
	if ( _debugLevel_ >= DEBUG_LEVEL::PRINT_EXCEPTION_STACK )
		fprintf( static_cast<FILE*>( ERROR_STREAM ), "%s", frame.raw() );
	hcore::log << frame;
	++ _frame;
	return;
	}

char const* HException::what( void ) const
	{
	return ( _message );
	}

int HException::code( void ) const
	{
	return ( _code );
	}

void HException::failed_assert( char const* const fileName_,
		char const* const functionName_, int const line_,
		char const* const message_ )
	{
	M_PROLOG
	hcore::log << "Failed assertion: " << message_ << " -> " << fileName_ << "(" << line_ << "): " << functionName_ << endl;
	fprintf( static_cast<FILE*>( ERROR_STREAM ), "Failed assertion: `%s' at: %s: %4d: %s\n",
			message_, fileName_, line_, functionName_ );
	if ( ! errno )
		++ errno;
	if ( _debugLevel_ >= DEBUG_LEVEL::ABORT_ON_ASSERT )
		::abort();
	throw ( HFailedAssertion( message_ ) );
	M_EPILOG
	}

void HException::set_error_stream( void* errorStream_ )
	{
	M_PROLOG
	M_ASSERT( errorStream_ );
	ERROR_STREAM = errorStream_;
	return;
	M_EPILOG
	}

char* HException::get_type_name( char const* const name_ )
	{
	int status = 0;
	return ( abi::__cxa_demangle( name_, 0, 0, &status ) );
	}

void HException::cleanup( char* ptr_ )
	{
	if ( ptr_ )
		xfree( ptr_ );
	return;
	}

void HFailedAssertion::swap( HFailedAssertion& other )
	{
	using yaal::swap;
	if ( &other != this )
		swap( _what, other._what );
	return;
	}

void HGlobalScopeExceptionHandlingPolicy::handle_exception( void )
	{
	try
		{
		throw;
		}
	catch ( HException const& e )
		{
		cerr << "Exception `" << e.what() << "' thrown from outside of main() scope." << endl;
		}
	catch ( ... )
		{
		cerr << "Exception of unknown type thrown from outside of main() scope." << endl;
		}
	exit( 1 );
	}

}

}

