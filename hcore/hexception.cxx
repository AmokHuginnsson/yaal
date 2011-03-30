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
#include <cxxabi.h>
#include <libintl.h>

#include "base.hxx"
M_VCSID( "$Id: "__ID__" $" )
M_VCSID( "$Id: "__TID__" $" )
#include "hexception.hxx"
#include "xalloc.hxx"
#include "hlog.hxx"
#include "hfile.hxx"
#include "introspect.hxx"

using namespace yaal;

namespace yaal
{

namespace hcore
{

namespace
{

FILE* ERROR_STREAM = stderr;

}

char const* const _exceptionType_ = _( "Exception type" );

HException::HException( HString const& fileName_,
		HString const& functionName_,
		int const line_, HString const& message_,
		int const code_ )
	: _code( code_ ), _frame( 0 ),
	_fileName( fileName_ ),
	_functionName( functionName_ ),
	_message( message_ )
	{
	hcore::log << "Exception: " << _message << ", code: " << _code;
	hcore::log << '.' << endl;
	log( fileName_.raw(), functionName_.raw(), line_ );
	return;
	}

HException::HException( HException const& exception_ )
	: _code( exception_._code ), _frame( exception_._frame ),
	_fileName( exception_._fileName ),
	_functionName( exception_._functionName ),
	_message( exception_._message )
	{
	return;
	}

HException::~HException( void )
	{
	return;
	}

void HException::print_error( void ) const
	{
	fprintf( static_cast<FILE*>( ERROR_STREAM ), "\nException: %s, %d.\n", _message.raw(), _code );
	return;
	}

void HException::log( char const* const fileName_,
											 char const* const functionName_, int const line_ )
	{
	size_t length = ::strlen( fileName_ );
	if ( _frame
			&& ( _fileName == fileName_ )
			&& ( _functionName == functionName_ ) )
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
	return ( _message.raw() );
	}

int HException::code( void ) const
	{
	return ( _code );
	}

void failed_assert( char const* const fileName_,
		char const* const functionName_, int const line_,
		char const* const message_ )
	{
	M_PROLOG
	hcore::log << "Failed assertion: " << message_ << " -> " << fileName_ << "(" << line_ << "): " << functionName_ << endl;
	fprintf( ERROR_STREAM, "Failed assertion: `%s' at: %s: %4d: %s\n",
			message_, fileName_, line_, functionName_ );
	if ( ! errno )
		++ errno;
	debug_break();
	static int const DUMP_DEPTH = 64;
	dump_call_stack( clog, DUMP_DEPTH );
	throw ( HFailedAssertion( message_ ) );
	M_EPILOG
	}

void HException::set_error_stream( void* errorStream_ )
	{
	M_PROLOG
	M_ASSERT( errorStream_ );
	ERROR_STREAM = static_cast<FILE*>( errorStream_ );
	return;
	M_EPILOG
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

HString demangle( char const* symbolName_ )
	{
	int status = 0;
	HString symbol;
	char* p( abi::__cxa_demangle( symbolName_, 0, 0, &status ) );
	if ( p )
		{
		symbol = p;
		xfree( p );
		}
	return ( symbol );
	}

void debug_break( void )
	{
	if ( _debugLevel_ >= DEBUG_LEVEL::ABORT_ON_ASSERT )
		::abort();
	return;
	}

}

}

