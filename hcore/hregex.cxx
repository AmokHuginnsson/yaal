/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include <cstring>

#include "config.hxx"

#include <pcre.h>
#include <libintl.h>

#include "base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "hregex.hxx"
#include "harray.hxx"
#include "hchunk.hxx"
#include "unicode.hxx"

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
HRegex::compile_t const HRegex::COMPILE::IGNORE_CASE = HRegex::compile_t::new_flag();
HRegex::compile_t const HRegex::COMPILE::NEWLINE = HRegex::compile_t::new_flag();
HRegex::compile_t const HRegex::COMPILE::NO_EXCEPTION = HRegex::compile_t::new_flag();
HRegex::compile_t const HRegex::COMPILE::DEFAULT = HRegex::COMPILE::NONE;
HRegex::match_t const HRegex::MATCH::NONE = HRegex::match_t::new_flag();
HRegex::match_t const HRegex::MATCH::NOT_BEGINNING_OF_LINE = HRegex::match_t::new_flag();
HRegex::match_t const HRegex::MATCH::NOT_END_OF_LINE = HRegex::match_t::new_flag();
HRegex::match_t const HRegex::MATCH::OVERLAPPING = HRegex::match_t::new_flag();
HRegex::match_t const HRegex::MATCH::DEFAULT = HRegex::MATCH::NONE;

HRegex::HRegex( void )
	: _pattern()
	, _flags( COMPILE::DEFAULT )
	, _impl( nullptr )
	, _extra( nullptr )
	, _lastError( 0 )
	, _errorMessage()
	, _utf8ConversionCache() {
	M_PROLOG
	return;
	M_EPILOG
}

HRegex::HRegex( HRegex&& reg_ )
	: _pattern( yaal::move( reg_._pattern ) )
	, _flags( yaal::move( reg_._flags ) )
	, _impl( yaal::move( reg_._impl ) )
	, _extra( yaal::move( reg_._extra ) )
	, _lastError( reg_._lastError )
	, _errorMessage( yaal::move( reg_._errorMessage ) )
	, _utf8ConversionCache( yaal::move( reg_._utf8ConversionCache ) ) {
	M_PROLOG
	reg_._extra = nullptr;
	reg_._impl = nullptr;
	reg_._flags = COMPILE::DEFAULT;
	reg_._lastError = 0;
	return;
	M_EPILOG
}

HRegex::HRegex( HString const& pattern_, compile_t flags_ )
	: _pattern()
	, _flags( flags_ )
	, _impl( nullptr )
	, _extra( nullptr )
	, _lastError( 0 )
	, _errorMessage()
	, _utf8ConversionCache() {
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
		swap( _pattern, reg_._pattern );
		swap( _flags, reg_._flags );
		swap( _impl, reg_._impl );
		swap( _extra, reg_._extra );
		swap( _lastError, reg_._lastError );
		swap( _errorMessage, reg_._errorMessage );
		swap( _utf8ConversionCache, reg_._utf8ConversionCache );
	}
	return;
}

void HRegex::clear( void ) {
	M_PROLOG
	reset();
	error_clear();
	return;
	M_EPILOG
}

void HRegex::reset( void ) {
	M_PROLOG
	if ( _extra ) {
		::pcre_free_study( static_cast<pcre_extra*>( _extra ) );
		_extra = nullptr;
	}
	if ( _impl ) {
		::pcre_free( static_cast<pcre*>( _impl ) );
		_impl = nullptr;
	}
	_pattern.clear();
	_flags = COMPILE::DEFAULT;
	return;
	M_EPILOG
}

HRegex HRegex::copy( void ) const {
	M_PROLOG
	HRegex regex( _pattern, _flags );
	return ( regex );
	M_EPILOG
}

void HRegex::error_clear( void ) {
	M_PROLOG
	_lastError = 0;
	_errorMessage.clear();
	return;
	M_EPILOG
}

bool HRegex::compile( HString const& pattern_, compile_t flags_ ) {
	M_PROLOG
	if ( pattern_.is_empty() ) {
		throw HRegexException( _errMsgHRegex_[ ERROR::EMPTY_PATTERN ] );
	}
	clear();
	int options( PCRE_UTF8 );
	options |= ( ( flags_ & COMPILE::IGNORE_CASE ) ? PCRE_CASELESS : 0 );
	options |= ( ( flags_ & COMPILE::NEWLINE ) ? PCRE_MULTILINE : 0 );
	char const* errMsg( nullptr );
	int offset( 0 );
	_utf8ConversionCache = pattern_;
	_impl = ::pcre_compile2( _utf8ConversionCache.c_str(), options, &_lastError, &errMsg, &offset, nullptr );
	if ( ! _lastError ) {
		_extra = ::pcre_study( static_cast<pcre*>( _impl ), PCRE_STUDY_JIT_COMPILE, &errMsg );
		if ( errMsg ) {
			reset();
			_lastError = 10; /* according to manual "this code is not in use" */
		}
	}
	if ( ! ( _lastError || errMsg ) ) {
		_pattern = pattern_;
	} else {
		_errorMessage.assign( pattern_ ).append( ":" ).append( offset ).append( ": " )
			.append( errMsg ? errMsg : "" ).append( errMsg ? ", " : "" ).append( _lastError );
		if ( ! ( flags_ & COMPILE::NO_EXCEPTION ) ) {
			HString msg( yaal::move( _errorMessage ) );
			clear();
			throw HRegexException( msg );
		}
		reset();
	}
	return ( _lastError == 0 );
	M_EPILOG
}

HString const& HRegex::pattern( void ) const {
	return ( _pattern );
}

bool HRegex::is_valid( void ) const {
	return ( _impl != nullptr );
}

HString const& HRegex::error( void ) const {
	return ( _errorMessage );
}

int HRegex::error_code( void ) const {
	return ( _lastError );
}

HUTF8String HRegex::matches_impl( HUTF8String const& str_, int& start_, int& matchLength_, match_t match_ ) const {
	M_PROLOG
	if ( _impl == nullptr ) {
		throw HRegexException( _errMsgHRegex_[ ERROR::UNINITIALIZED ] );
	}
	start_ = NO_MATCH;
	matchLength_ = 0;
	char const* str( str_.raw() );
	int match[3] = { NO_MATCH, NO_MATCH, NO_MATCH };
	_lastError = ::pcre_exec(
		static_cast<pcre*>( _impl ),
		static_cast<pcre_extra*>( _extra ),
		str,
		static_cast<int>( str_.byte_count() ),
		0,
		( ( match_ & MATCH::NOT_BEGINNING_OF_LINE ) ? PCRE_NOTBOL : 0 ) | ( ( match_ & MATCH::NOT_END_OF_LINE ) ? PCRE_NOTEOL : 0 ),
		match, 3
	);
	if ( ( _lastError >= 0 ) && ( match[0] >= 0 ) ) {
		start_ = unicode::count_characters( str, match[0] );
		matchLength_ = unicode::count_characters( str + match[0], static_cast<int>( match[1] - match[0] ) );
	}
	return ( start_ != NO_MATCH ? str_.substr( start_ + matchLength_ ) : HUTF8String() );
	M_EPILOG
}

HRegex::groups_t HRegex::groups_impl( HUTF8String const& string_, match_t match_ ) const {
	M_PROLOG
	if ( _impl == nullptr ) {
		throw HRegexException( _errMsgHRegex_[ ERROR::UNINITIALIZED ] );
	}
	groups_t g;
	typedef yaal::hcore::HArray<int> matches_t;
	int expectedGroupCount( static_cast<int>( count( _pattern.begin(), _pattern.end(), '('_ycp ) + 1 ) );
	matches_t matchesBuffer( expectedGroupCount * 3, NO_MATCH );
	char const* str( string_.raw() );
	_lastError = ::pcre_exec(
		static_cast<pcre*>( _impl ),
		static_cast<pcre_extra*>( _extra ),
		str,
		static_cast<int>( string_.byte_count() ),
		0,
		( ( match_ & MATCH::NOT_BEGINNING_OF_LINE ) ? PCRE_NOTBOL : 0 ) | ( ( match_ & MATCH::NOT_END_OF_LINE ) ? PCRE_NOTEOL : 0 ),
		matchesBuffer.data(),
		expectedGroupCount * 3
	);
	if ( _lastError >= 0 ) {
		int groupCount( 0 );
		for ( int i( 0 ); i < expectedGroupCount; ++ i ) {
			if ( matchesBuffer[i * 2] >= 0 ) {
				++ groupCount;
			}
		}
		if ( groupCount > 0 ) {
			g.reserve( groupCount );
			for ( int i( 0 ); i < groupCount; ++ i ) {
				g.emplace_back(
					unicode::count_characters( str, matchesBuffer[i * 2] ),
					unicode::count_characters( str + matchesBuffer[i * 2], static_cast<int>( matchesBuffer[i * 2 + 1] - matchesBuffer[i * 2] ) )
				);
			}
		}
	}
	return ( g );
	M_EPILOG
}

yaal::hcore::HString HRegex::replace( yaal::hcore::HString const& text_, yaal::hcore::HString const& replacement_, match_t match_ ) const {
	M_PROLOG
	static char const errMsg[] = "Malformed back-reference in replacement string.";
	static char const BACK_REF( '$' );
	HString res;
	int offset( 0 );
	int endPos( 0 );
	_utf8ConversionCache = text_;
	while ( true ) {
		_utf8ConversionCache = _utf8ConversionCache.substr( offset );
		groups_t g( groups_impl( _utf8ConversionCache, match_ ) );
		if ( g.is_empty() ) {
			res.append( text_, endPos );
			break;
		}
		res.append( text_, endPos, g[0].start() );
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
					while ( ( r != e ) && character_class( CHARACTER_CLASS::DIGIT ).has( *r ) ) {
						idS.push_back( *r );
						++ r;
					}
					if ( idS.is_empty() ) {
						throw HRegexException( errMsg );
					}
					int id( lexical_cast<int>( idS ) );
					if ( id >= g.get_size() ) {
						throw HRegexException( "Invalid back-reference number in replacement string: "_ys.append( id ).append( "." ) );
					}
					res.append( text_, endPos + g[id].start(), g[id].size() );
				}
			} else {
				res.push_back( *r );
				++ r;
			}
		}
		offset = ( g[0].start() + g[0].size() );
		endPos += offset;
	}
	return ( res );
	M_EPILOG
}

yaal::hcore::HString HRegex::replace( yaal::hcore::HString const& text_, replacer_t const& replacer_, match_t match_ ) const {
	M_PROLOG
	HString res;
	int endPos( 0 );
	for ( yaal::hcore::HRegex::HMatch const& m : matches( text_, match_ ) ) {
		res.append( text_, endPos, m.start() - endPos );
		res.append( replacer_( text_.substr( m.start(), m.size() ) ) );
		endPos = m.start() + m.size();
	}
	res.append( text_, endPos );
	return ( res );
	M_EPILOG
}

HRegex::HMatchIterator HRegex::find( HUTF8String const& str_, match_t match_ ) const {
	_utf8ConversionCache = str_;
	int start( 0 );
	int len( 0 );
	HUTF8String match( matches_impl( _utf8ConversionCache, start, len, match_ ) );
	HMatchIterator it( this, match_, match, start, len );
	return ( it );
}

HRegex::HMatchIterator HRegex::end( void ) const {
	return ( HMatchIterator( this, MATCH::NONE, HUTF8String(), NO_MATCH, 0 ) );
}

HRegex::HMatchResult HRegex::matches( HUTF8String const& str_, match_t match_ ) const {
	return ( HMatchResult( find( str_, match_ ), end() ) );
}

HRegex::groups_t HRegex::groups( yaal::hcore::HUTF8String const& string_, match_t match_ ) const {
	M_PROLOG
	_utf8ConversionCache = string_;
	return ( groups_impl( _utf8ConversionCache, match_ ) );
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
	HRegex::match_t flags_,
	HUTF8String const& string_,
	int start_,
	int len_
) : base_type()
	, _owner( owner_ )
	, _flags( flags_ )
	, _string( string_ )
	, _match( start_, len_ ) {
}

HRegex::HMatchIterator::HMatchIterator( HMatchIterator const& it_ )
	: base_type()
	, _owner( it_._owner )
	, _flags( it_._flags )
	, _string( it_._string )
	, _match( it_._match ) {
}

HRegex::HMatchIterator& HRegex::HMatchIterator::operator = ( HRegex::HMatchIterator const& it_ ) {
	if ( &it_ != this ) {
		_owner = it_._owner;
		_flags = it_._flags;
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
	M_ASSERT( _match._start != NO_MATCH );
	int start( NO_MATCH );
	int matchSize( _match._size );
	_string = _owner->matches_impl( _string, start, _match._size, _flags );
	_match._start = start != NO_MATCH ? _match._start + start + matchSize : NO_MATCH;
	return ( *this );
	M_EPILOG
}

HRegex operator "" _yr ( char const* str_, size_t len_ ) {
	return ( HRegex( HString( str_, static_cast<int>( len_ ) ) ) );
}

}

}

