/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

  hexception.cxx - this file is integral part of `yaal' project.

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

#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <csignal>
#include <cxxabi.h>
#include <libintl.h>

#include "base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "hexception.hxx"
#include "memory.hxx"
#include "hlog.hxx"
#include "hfile.hxx"

using namespace yaal;

namespace yaal {

namespace hcore {

FILE* ERROR_STREAM = stderr;
bool HException::_logEnabled = true;

HException::HException( char const* fileName_,
		int const line_, char const* functionName_,
		HString const& message_, int const code_,
		HString const& name_ )
	: _code( code_ )
	, _frame( 0 )
	, _fileName( fileName_ )
	, _functionName( functionName_ )
	, _message( message_ ) {
	if ( _logEnabled ) {
		hcore::log << name_ << ": " << _message << ", code: " << _code << '.' << endl;
	}
	if ( fileName_ && functionName_ ) {
		log( fileName_, line_, functionName_ );
	} else {
		++ _frame;
	}
	return;
}

HException::HException( HException const& exception_ )
	: _code( exception_._code )
	, _frame( exception_._frame )
	, _fileName( exception_._fileName )
	, _functionName( exception_._functionName )
	, _message( exception_._message ) {
	return;
}

HException::~HException( void ) {
	return;
}

void HException::print_error( void ) const {
	fprintf( ERROR_STREAM, "\nException: %s, %d.\n", _message.c_str(), _code );
	fflush( ERROR_STREAM );
	return;
}

void HException::log( char const* fileName_, int line_,
		char const* functionName_ ) {
	if ( ! _frame
			|| ( ( _fileName != fileName_ ) && ( ! _fileName || ::strcmp( _fileName, fileName_ ) ) )
			|| ( ! _functionName && functionName_ )
			|| ( ( _functionName != functionName_ ) && ( ! _functionName || ::strcmp( _functionName, functionName_ ) ) ) ) {
		_fileName = fileName_;
		_functionName = functionName_;
		if ( _logEnabled || ( _debugLevel_ >= DEBUG_LEVEL::DUMP_EXCEPTION_STACK ) ) {
			HString frame;
			size_t length( ::strlen( fileName_ ) );
			frame.format(
						"Exception frame %2d: %16s : %4d : %s\n", _frame,
						fileName_ + ( length > 16 ? length - 16 : 0 ),
						line_, functionName_ );
			if ( _debugLevel_ >= DEBUG_LEVEL::DUMP_EXCEPTION_STACK ) {
				fprintf( ERROR_STREAM, "%s", frame.c_str() );
				fflush( ERROR_STREAM );
			}
			if ( _logEnabled ) {
				hcore::log << frame;
			}
		}
		++ _frame;
	}
	return;
}

char const* HException::what( void ) const {
	return ( _message.c_str() );
}

int HException::code( void ) const {
	return ( _code );
}

void HException::set_error_stream( void* errorStream_ ) {
	M_PROLOG
	M_ASSERT( errorStream_ );
	ERROR_STREAM = static_cast<FILE*>( errorStream_ );
	return;
	M_EPILOG
}

void HException::enable_logging( void ) {
	_logEnabled = true;
}

void HException::disable_logging( void ) {
	_logEnabled = false;
}

void HGlobalScopeExceptionHandlingPolicy::handle_exception( void ) {
	try {
		throw;
	} catch ( HException const& e ) {
		hcore::log << "Exception `" << e.what() << "' thrown from outside of main() scope." << endl;
		cerr << "Exception `" << e.what() << "' thrown from outside of main() scope." << endl;
	} catch ( ... ) {
		hcore::log << "Exception of unknown type thrown from outside of main() scope." << endl;
		cerr << "Exception of unknown type thrown from outside of main() scope." << endl;
	}
	exit( 1 );
}

HString demangle( char const* symbolName_ ) {
	int status = 0;
	HString symbol;
	char* p( abi::__cxa_demangle( symbolName_, 0, 0, &status ) );
	if ( p ) {
		symbol = p;
		memory::free( p );
	}
	return ( symbol );
}

void kill_interior( yaal::hcore::HString const& msg_ ) {
	yaal::_isKilled_ = true;
	hcore::log << "FATAL ERROR: " << msg_ << endl;
	cerr << "FATAL ERROR: " << msg_ << endl;
#ifndef NDEBUG
	if ( _debugLevel_ >= DEBUG_LEVEL::ABORT_ON_ASSERT ) {
		::abort();
	}
#endif /* #ifndef NDEBUG */
	return;
}

}

}

