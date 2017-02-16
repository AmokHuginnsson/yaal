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
	, _flags( COMPILE::DEFAULT )
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
	, _flags( yaal::move( reg_._flags ) )
	, _compiled( yaal::move( reg_._compiled ) )
	, _lastError( reg_._lastError )
	, _errorBuffer( yaal::move( reg_._errorBuffer ) )
	, _errorCause( yaal::move( reg_._errorCause ) )
	, _errorMessage( yaal::move( reg_._errorMessage ) ) {
	M_PROLOG
	reg_._initialized = false;
	reg_._flags = COMPILE::DEFAULT;
	reg_._lastError = 0;
	return;
	M_EPILOG
}

HRegex::HRegex( HString const& pattern_, compile_t flags_ )
	: _initialized( false )
	, _pattern()
	, _flags( flags_ )
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
		swap( _flags, reg_._flags );
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
	_flags = COMPILE::DEFAULT;
	_errorCause.clear();
	_errorMessage.clear();
	return;
	M_EPILOG
}

HRegex HRegex::copy( void ) const {
	M_PROLOG
	HRegex regex( _pattern, _flags );
	return ( regex );
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
	if ( pattern_.is_empty() ) {
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
	_lastError = ::regcomp( _compiled.get<regex_t>(), pattern_.raw(), cflags );
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
			int long size( static_cast<int long>( ::regerror( _lastError, _compiled.get<regex_t>(), nullptr, 0 ) ) + 1 );
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

char const* HRegex::matches_impl( char const* string_, int* matchLength_ ) const {
	M_PROLOG
	M_ASSERT( string_ && matchLength_ );
	if ( ! _initialized ) {
		throw HRegexException( _errMsgHRegex_[ ERROR::UNINITIALIZED ] );
	}
	error_clear();
	char const* ptr = nullptr;
	int matchLength = 0;
	regmatch_t match;
	_lastError = ::regexec( _compiled.get<regex_t>(), string_, 1, &match, 0 );
	if ( ! _lastError ) {
		matchLength = static_cast<int>( match.rm_eo - match.rm_so );
		ptr = string_ + match.rm_so;
	}
	( *matchLength_ ) = matchLength;
	error_clear();
	return ( ptr );
	M_EPILOG
}

HRegex::groups_t HRegex::groups_impl( char const* string_ ) const {
	M_PROLOG
	groups_t g;
	typedef yaal::hcore::HArray<regmatch_t> matches_t;
	int expectedGroupCount( static_cast<int>( count( _pattern.begin(), _pattern.end(), '(' ) + 1 ) );
	matches_t matchesBuffer( expectedGroupCount );
	_lastError = ::regexec(
		_compiled.get<regex_t>(),
		string_,
		static_cast<size_t>( expectedGroupCount ),
		matchesBuffer.data(),
		0
	);
	if ( ! _lastError ) {
		int groupCount( 0 );
		for ( regmatch_t const& m : matchesBuffer ) {
			if ( m.rm_so >= 0 ) {
				++ groupCount;
			}
		}
		g.reserve( groupCount );
		for ( regmatch_t const& m : matchesBuffer ) {
			g.emplace_back( static_cast<int>( m.rm_so ), static_cast<int>( m.rm_eo - m.rm_so ) );
		}
	}
	return ( g );
	M_EPILOG
}

yaal::hcore::HString HRegex::replace( yaal::hcore::HString const& text_, yaal::hcore::HString const& replacement_ ) {
	M_PROLOG
	static char const errMsg[] = "Malformed back-reference in replacement string.";
	static char const BACK_REF( '$' );
	HString res;
	int end( 0 );
	while ( true ) {
		groups_t g( groups_impl( text_.raw() + end ) );
		if ( g.is_empty() ) {
			res.append( text_, end );
			break;
		}
		res.append( text_, end, g[0].start() );
		for ( HString::const_iterator r( replacement_.begin() ), e( replacement_.end() ); r != e; ) {
			if ( *r == BACK_REF ) {
				++ r;
				if ( r == e ) {
					throw HRegexException( errMsg );
				}
				if ( *r == BACK_REF ) {
					res.append( BACK_REF );
					++ r;
				} else {
					HString idS;
					while ( ( r != e ) && _digit_.has( *r ) ) {
						idS.append( *r );
						++ r;
					}
					if ( idS.is_empty() ) {
						throw HRegexException( errMsg );
					}
					int id( lexical_cast<int>( idS ) );
					if ( id >= g.get_size() ) {
						throw HRegexException( "Invalid back-reference number in replacement string: "_ys.append( id ).append( "." ) );
					}
					res.append( text_, end + g[id].start(), g[id].size() );
				}
			} else {
				res.append( *r );
				++ r;
			}
		}
		end += ( g[0].start() + g[0].size() );
	}
	return ( res );
	M_EPILOG
}

yaal::hcore::HString HRegex::replace( yaal::hcore::HString const& text_, replacer_t const& replacer_ ) {
	M_PROLOG
	HString res;
	int end( 0 );
	for ( yaal::hcore::HRegex::HMatch const& m : matches( text_ ) ) {
		res.append( text_, end, m.start() - end );
		res.append( replacer_( text_.substr( m.start(), m.size() ) ) );
		end = m.start() + m.size();
	}
	res.append( text_, end );
	return ( res );
	M_EPILOG
}

HRegex::HMatchIterator HRegex::find( char const* str_ ) const {
	M_ASSERT( str_ );
	int len( 0 );
	char const* start( matches_impl( str_, &len ) );
	HMatchIterator it( this, str_, start ? static_cast<int>( start - str_ ) : -1, len );
	return ( it );
}

HRegex::HMatchIterator HRegex::find( HString const& str_ ) const {
	return ( find( str_.raw() ) );
}

HRegex::HMatchIterator HRegex::end( void ) const {
	return ( HMatchIterator( this, nullptr, -1, 0 ) );
}

HRegex::HMatchResult HRegex::matches( HString const& str_ ) const {
	return ( HMatchResult( find( str_ ), end() ) );
}

HRegex::groups_t HRegex::groups( yaal::hcore::HString const& string_ ) const {
	M_PROLOG
	return ( groups_impl( string_.raw() ) );
	M_EPILOG
}

HRegex::HMatchResult::HMatchResult( HRegex::HMatchIterator const& it_, HRegex::HMatchIterator const& end_ )
	: _it( it_ )
	, _end( end_ ) {
}

HRegex::HMatchResult::operator safe_bool_t() const {
	return ( _it != _end ? &SemanticContext::member : nullptr );
}

HRegex::HMatchIterator HRegex::HMatchResult::begin( void ) const {
	return ( _it );
}

HRegex::HMatchIterator HRegex::HMatchResult::end( void ) const {
	return ( _end );
}

HRegex::HMatchIterator HRegex::HMatchResult::cbegin( void ) const {
	return ( _it );
}

HRegex::HMatchIterator HRegex::HMatchResult::cend( void ) const {
	return ( _end );
}

HRegex::HMatch::HMatch( int start_, int size_ )
	: _start( start_ )
	, _size( size_ ) {
}

int HRegex::HMatch::start( void ) const {
	return ( _start );
}

int HRegex::HMatch::size( void ) const {
	return ( _size );
}

HRegex::HMatchIterator::HMatchIterator(
	HRegex const* owner_,
	char const* string_,
	int start_,
	int len_
) : base_type()
	, _owner( owner_ )
	, _string( string_ )
	, _match( start_, len_ ) {
}

HRegex::HMatchIterator::HMatchIterator( HMatchIterator const& it_ )
	: base_type()
	, _owner( it_._owner )
	, _string( it_._string )
	, _match( it_._match ) {
}

HRegex::HMatchIterator& HRegex::HMatchIterator::operator = ( HRegex::HMatchIterator const& it_ ) {
	if ( &it_ != this ) {
		_owner = it_._owner;
		_match = it_._match;
		_string = it_._string;
	}
	return ( *this );
}

HRegex::HMatch const* HRegex::HMatchIterator::operator->( void ) const {
	return ( &_match );
}

HRegex::HMatch const& HRegex::HMatchIterator::operator* ( void ) const {
	return ( _match );
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
	M_ASSERT( _string && ( _match._start >= 0 ) );
	char const* match( _owner->matches_impl( _string + _match._start + 1, &_match._size ) );
	_match._start = match ? static_cast<int>( match - _string ) : -1;
	return ( *this );
	M_EPILOG
}

HRegex operator "" _yr ( char const* str_, size_t len_ ) {
	return ( HRegex( HString( str_, static_cast<int>( len_ ) ) ) );
}

}

}

