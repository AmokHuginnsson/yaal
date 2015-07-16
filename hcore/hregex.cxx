/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

  hregex.cxx - this file is integral part of `yaal' project.

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

#include "config.hxx"

#ifdef YAAL_USE_PCRE
#	include <pcreposix.h>
#else /* #ifdef YAAL_USE_PCRE */
#	include <sys/types.h> /* why? - because POSIX says so :/ */
#	include <regex.h>     /* this one is obvious */
#endif /* #else #ifdef YAAL_USE_PCRE */
#include <libintl.h>

#include "base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "hregex.hxx"
#include "harray.hxx"
#include "hchunk.hxx"

namespace yaal {

namespace hcore {

namespace {
struct ERROR {
	enum {
		OK = 0,
		UNINITIALIZED,
		EMPTY_PATTERN
	};
};
}

char const* _errMsgHRegex_[ 3 ] = {
	_( "ok" ),
	_( "use of uninitialized regex" ),
	_( "empty pattern" )
};

HRegex::compile_t const HRegex::COMPILE::NONE = HRegex::compile_t::new_flag();
HRegex::compile_t const HRegex::COMPILE::EXTENDED = HRegex::compile_t::new_flag();
HRegex::compile_t const HRegex::COMPILE::IGNORE_CASE = HRegex::compile_t::new_flag();
HRegex::compile_t const HRegex::COMPILE::NEWLINE = HRegex::compile_t::new_flag();
HRegex::compile_t const HRegex::COMPILE::NO_EXCEPTION = HRegex::compile_t::new_flag();
HRegex::compile_t const HRegex::COMPILE::DEFAULT = HRegex::COMPILE::NONE;
HRegex::match_t const HRegex::MATCH::NONE = HRegex::match_t::new_flag();
HRegex::match_t const HRegex::MATCH::NOT_BEGINNING_OF_LINE = HRegex::match_t::new_flag();
HRegex::match_t const HRegex::MATCH::NOT_END_OF_LINE = HRegex::match_t::new_flag();
HRegex::match_t const HRegex::MATCH::DEFAULT = HRegex::MATCH::NONE;

HRegex::HRegex( void )
	: _initialized( false )
	, _pattern()
	, _compiled( sizeof ( regex_t ) )
	, _lastError( 0 )
	, _errorBuffer()
	, _errorCause()
	, _errorMessage() {
	M_PROLOG
	return;
	M_EPILOG
}

HRegex::HRegex( HRegex&& reg_ )
	: _initialized( reg_._initialized )
	, _pattern( yaal::move( reg_._pattern ) )
	, _compiled( yaal::move( reg_._compiled ) )
	, _lastError( reg_._lastError )
	, _errorBuffer( yaal::move( reg_._errorBuffer ) )
	, _errorCause( yaal::move( reg_._errorCause ) )
	, _errorMessage( yaal::move( reg_._errorMessage ) ) {
	M_PROLOG
	reg_._initialized = false;
	reg_._lastError = 0;
	return;
	M_EPILOG
}

HRegex::HRegex( char const* const pattern_, compile_t flags_ )
	: _initialized( false )
	, _pattern()
	, _compiled( sizeof ( regex_t ) )
	, _lastError( 0 )
	, _errorBuffer()
	, _errorCause()
	, _errorMessage() {
	M_PROLOG
	compile( pattern_, flags_ );
	return;
	M_EPILOG
}

HRegex::HRegex( HString const& pattern_, compile_t flags_ )
	: _initialized( false )
	, _pattern()
	, _compiled( sizeof ( regex_t ) )
	, _lastError( 0 )
	, _errorBuffer()
	, _errorCause()
	, _errorMessage() {
	M_PROLOG
	compile( pattern_, flags_ );
	return;
	M_EPILOG
}

HRegex::~HRegex( void ) {
	M_PROLOG
	clear();
	return;
	M_DESTRUCTOR_EPILOG
}

HRegex& HRegex::operator = ( yaal::hcore::HRegex&& reg_ ) {
	M_PROLOG
	if ( &reg_ != this ) {
		swap( reg_ );
		reg_.clear();
	}
	return ( *this );
	M_EPILOG
}

void HRegex::swap( HRegex& reg_ ) {
	if ( &reg_ != this ) {
		using yaal::swap;
		swap( _initialized, reg_._initialized );
		swap( _pattern, reg_._pattern );
		swap( _compiled, reg_._compiled );
		swap( _lastError, reg_._lastError );
		swap( _errorBuffer, reg_._errorBuffer );
		swap( _errorCause, reg_._errorCause );
		swap( _errorMessage, reg_._errorMessage );
	}
	return;
}

void HRegex::clear( void ) {
	M_PROLOG
	if ( _initialized ) {
		::regfree( _compiled.get<regex_t>() );
	}
	_initialized = false;
	_lastError = 0;
	_pattern.clear();
	_errorCause.clear();
	_errorMessage.clear();
	return;
	M_EPILOG
}

void HRegex::error_clear( void ) const {
	M_PROLOG
	_lastError = 0;
	_errorCause.clear();
	_errorMessage.clear();
	return;
	M_EPILOG
}

bool HRegex::compile( HString const& pattern_, compile_t flags_ ) {
	M_PROLOG
	return ( compile( pattern_.raw(), flags_ ) );
	M_EPILOG
}

bool HRegex::compile( char const* pattern_, compile_t flags_ ) {
	M_PROLOG
	M_ASSERT( pattern_ );
	if ( ! pattern_[0] ) {
		throw HRegexException( _errMsgHRegex_[ ERROR::EMPTY_PATTERN ] );
	}
	error_clear();
	if ( _initialized ) {
		::regfree( _compiled.get<regex_t>() );
	}
	int cflags( 0 );
	cflags |= ( ( flags_ & COMPILE::EXTENDED ) ? REG_EXTENDED : 0 );
	cflags |= ( ( flags_ & COMPILE::IGNORE_CASE ) ? REG_ICASE : 0 );
	cflags |= ( ( flags_ & COMPILE::NEWLINE ) ? REG_NEWLINE : 0 );
	_lastError = ::regcomp( _compiled.get<regex_t>(), pattern_, cflags );
	if ( ! _lastError ) {
		_initialized = true;
		_pattern = pattern_;
	} else {
		_errorCause = pattern_;
		_initialized = false;
		if ( ! ( flags_ & COMPILE::NO_EXCEPTION ) ) {
			throw HRegexException( error() );
		}
	}
	return ( _lastError == 0 );
	M_EPILOG
}

HString const& HRegex::pattern( void ) const {
	return ( _pattern );
}

bool HRegex::is_valid( void ) const {
	return ( _initialized );
}

HString const& HRegex::error( void ) const {
	if ( _errorMessage.is_empty() ) {
		if ( _lastError ) {
			int long size( static_cast<int long>( ::regerror( _lastError, _compiled.get<regex_t>(), NULL, 0 ) ) + 1 );
			_errorBuffer.realloc( size + 1 );
			M_ENSURE( static_cast<int>( ::regerror( _lastError, _compiled.get<regex_t>(),
							_errorBuffer.get<char>(), static_cast<size_t>( size ) ) ) < size );
			_errorMessage = _errorBuffer.get<char>();
			if ( ! _errorCause.is_empty() ) {
				_errorMessage += ": `";
				_errorMessage += _errorCause;
				_errorMessage += "'";
			}
		} else {
			_errorMessage = _errMsgHRegex_[ ERROR::OK ];
		}
	}
	return ( _errorMessage );
}

int HRegex::error_code( void ) const {
	return ( _lastError );
}

char const* HRegex::matches( char const* string_, int long* matchLength_ ) const {
	M_PROLOG
	M_ASSERT( string_ && matchLength_ );
	if ( ! _initialized ) {
		throw HRegexException( _errMsgHRegex_[ ERROR::UNINITIALIZED ] );
	}
	error_clear();
	char const* ptr = NULL;
	int long matchLength = 0;
	regmatch_t match;
	if ( ! ( _lastError = ::regexec( _compiled.get<regex_t>(), string_, 1, &match, 0 ) ) ) {
		matchLength = match.rm_eo - match.rm_so;
		ptr = string_ + match.rm_so;
	}
	( *matchLength_ ) = matchLength;
	return ( ptr );
	M_EPILOG
}

HRegex::HMatchIterator HRegex::find( char const* const str_ ) const {
	M_ASSERT( str_ );
	int long len( 0 );
	char const* start = matches( str_, &len );
	HMatchIterator it( this, start, len );
	return ( it );
}

HRegex::HMatchIterator HRegex::find( HString const& str_ ) const {
	return ( find( str_.raw() ) );
}

HRegex::HMatchIterator HRegex::end( void ) const {
	return ( HMatchIterator( this, NULL, 0 ) );
}

bool HRegex::matches( HString const& str_ ) const {
	return ( find( str_ ) != end() );
}

HRegex::HMatch::HMatch( char const* start_, int long size_ )
	: _size( size_ ), _start( start_ ) {
}

int long HRegex::HMatch::size( void ) const {
	return ( _size );
}

char const* HRegex::HMatch::raw( void ) const {
	return ( _start );
}

HRegex::HMatchIterator::HMatchIterator( HRegex const* owner_, char const* const start_, int long len_ )
	: base_type(), _owner( owner_ ), _match( start_, len_ ) {
}

HRegex::HMatchIterator::HMatchIterator( HMatchIterator const& it_ )
	: base_type(), _owner( it_._owner ), _match( it_._match ) {
}

HRegex::HMatchIterator& HRegex::HMatchIterator::operator = ( HRegex::HMatchIterator const& it_ ) {
	if ( &it_ != this ) {
		_owner = it_._owner;
		_match = it_._match;
	}
	return ( *this );
}

HRegex::HMatch const* HRegex::HMatchIterator::operator->( void ) const {
	return ( &_match );
}

bool HRegex::HMatchIterator::operator != ( HMatchIterator const& mi_ ) const {
	M_PROLOG
	M_ASSERT( mi_._owner == _owner );
	return ( mi_._match._start != _match._start );
	M_EPILOG
}

bool HRegex::HMatchIterator::operator == ( HMatchIterator const& mi_ ) const {
	M_PROLOG
	M_ASSERT( mi_._owner == _owner );
	return ( mi_._match._start == _match._start );
	M_EPILOG
}

HRegex::HMatchIterator& HRegex::HMatchIterator::operator ++ ( void ) {
	M_PROLOG
	M_ASSERT( _match._start );
	_match._start = _owner->matches( _match._start + 1, &_match._size );
	return ( *this );
	M_EPILOG
}

HRegex operator "" _yr ( char const* str_, size_t len_ ) {
	return ( HRegex( HString( str_, static_cast<int>( len_ ) ) ) );
}

}

}

