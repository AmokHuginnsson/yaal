/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include <cstring>
#include <cstdio>
#include <cctype>

#include "base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "hstreaminterface.hxx"
#include "hstring.hxx"
#include "unicode.hxx"

namespace yaal {

namespace hcore {

static int const INVALID_CHARACTER( -256 );
static int const MAX_INTEGER_DIGIT_COUNT( 64 );
static int const MAX_FLOAT_DIGIT_COUNT( 8192 );
static int const MAX_FLOAT_FORMAT_SIZE( 64 );
char const HStreamInterface::eols[] = "\r\n"; /* order matters */

HStreamInterface::HStreamInterface( void )
	: _cache( 1, HChunk::STRATEGY::GEOMETRIC )
	, _offset( 0 )
	, _wordCache()
	, _conversionCache()
	, _fill( ' ' )
	, _width( 0 )
	, _precision( 6 )
	, _mode( MODE::TEXT )
	, _base( BASE::DEC )
	, _floatFormat( FLOAT_FORMAT::NATURAL )
	, _adjust( ADJUST::RIGHT )
	, _skipWS( true )
	, _boolAlpha( false )
	, _valid( true )
	, _fail( false ) {
	return;
}

HStreamInterface::~HStreamInterface( void ) {
	return;
}

HStreamInterface& HStreamInterface::do_output( HString const& string_ ) {
	M_PROLOG
	if ( _mode == MODE::TEXT ) {
		_wordCache = string_;
		reformat();
		_conversionCache = _wordCache;
	} else {
		_conversionCache = string_;
		do_output( static_cast<i32_t>( _conversionCache.byte_count() ) );
	}
	do_write( _conversionCache.raw(), _conversionCache.byte_count() );
	return ( *this );
	M_EPILOG
}

HStreamInterface& HStreamInterface::do_output( char const* string_ ) {
	M_PROLOG
	if ( _mode == MODE::TEXT ) {
		_wordCache = string_;
		reformat();
		_conversionCache = _wordCache;
		do_write( _conversionCache.raw(), _conversionCache.byte_count() );
	} else {
		i32_t len( static_cast<i32_t>( strlen( string_ ) ) );
		do_output( len );
		do_write( string_, len );
	}
	return ( *this );
	M_EPILOG
}

HStreamInterface& HStreamInterface::do_output( bool bool_ ) {
	M_PROLOG
	if ( _mode == MODE::TEXT ) {
		_wordCache = _boolAlpha ? ( bool_ ? "true" : "false" ) : ( bool_ ? "1" : "0" );
		reformat();
		_conversionCache = _wordCache;
		do_write( _conversionCache.raw(), _conversionCache.byte_count() );
	} else {
		i8_t buffer( bool_ ? 1 : 0 );
		do_write( &buffer, static_cast<int>( sizeof ( buffer ) ) );
	}
	return ( *this );
	M_EPILOG
}

HStreamInterface& HStreamInterface::do_output( code_point_t char_ ) {
	M_PROLOG
	if ( _mode == MODE::TEXT ) {
		_wordCache = char_;
		reformat();
		_conversionCache = _wordCache;
		do_write( _conversionCache.raw(), _conversionCache.byte_count() );
	} else {
		do_write( &char_.get(), static_cast<int>( sizeof ( char_.get() ) ) );
	}
	return ( *this );
	M_EPILOG
}

HStreamInterface& HStreamInterface::do_output( char char_ ) {
	M_PROLOG
	if ( _mode == MODE::TEXT ) {
		_wordCache = char_;
		reformat();
		_conversionCache = _wordCache;
		do_write( _conversionCache.raw(), _conversionCache.byte_count() );
	} else {
		do_write( &char_, static_cast<int>( sizeof ( char_ ) ) );
	}
	return ( *this );
	M_EPILOG
}

HStreamInterface& HStreamInterface::do_output( char unsigned charUnsigned_ ) {
	M_PROLOG
	if ( _mode == MODE::TEXT ) {
		_wordCache = charUnsigned_;
		reformat();
		_conversionCache = _wordCache;
		do_write( _conversionCache.raw(), _conversionCache.byte_count() );
	} else {
		do_write( &charUnsigned_, static_cast<int>( sizeof ( charUnsigned_ ) ) );
	}
	return ( *this );
	M_EPILOG
}

HStreamInterface& HStreamInterface::do_output( int short shortInteger_ ) {
	M_PROLOG
	if ( _mode == MODE::TEXT ) {
		int long tmp( shortInteger_ );
		do_output( tmp );
	} else {
		do_write( &shortInteger_, static_cast<int>( sizeof ( shortInteger_ ) ) );
	}
	return ( *this );
	M_EPILOG
}

HStreamInterface& HStreamInterface::do_output( int short unsigned unsignedShortInteger_ ) {
	M_PROLOG
	if ( _mode == MODE::TEXT ) {
		int long unsigned tmp( unsignedShortInteger_ );
		do_output( tmp );
	} else {
		do_write( &unsignedShortInteger_, static_cast<int>( sizeof ( unsignedShortInteger_ ) ) );
	}
	return ( *this );
	M_EPILOG
}

HStreamInterface& HStreamInterface::do_output( int integer_ ) {
	M_PROLOG
	if ( _mode == MODE::TEXT ) {
		int long tmp( integer_ );
		do_output( tmp );
	} else {
		do_write( &integer_, static_cast<int>( sizeof ( integer_ ) ) );
	}
	return ( *this );
	M_EPILOG
}

HStreamInterface& HStreamInterface::do_output( int unsigned unsignedInteger_ ) {
	M_PROLOG
	if ( _mode == MODE::TEXT ) {
		int long unsigned tmp( unsignedInteger_ );
		do_output( tmp );
	} else {
		do_write( &unsignedInteger_, static_cast<int>( sizeof ( unsignedInteger_ ) ) );
	}
	return ( *this );
	M_EPILOG
}

HStreamInterface& HStreamInterface::do_output( int long longInteger_ ) {
	M_PROLOG
	if ( _mode == MODE::TEXT ) {
		char buffer[MAX_INTEGER_DIGIT_COUNT];
		snprintf( buffer, MAX_INTEGER_DIGIT_COUNT, _base == BASE::DEC ? "%ld" : ( _base == BASE::HEX ) ? "%lx" : "%lo", longInteger_ );
		_wordCache = buffer;
		reformat();
		_conversionCache = _wordCache;
		do_write( _conversionCache.raw(), _conversionCache.byte_count() );
	} else {
		do_write( &longInteger_, static_cast<int>( sizeof ( longInteger_ ) ) );
	}
	return ( *this );
	M_EPILOG
}

HStreamInterface& HStreamInterface::do_output( int long long longLongInteger_ ) {
	M_PROLOG
	if ( _mode == MODE::TEXT ) {
		char buffer[MAX_INTEGER_DIGIT_COUNT];
		snprintf( buffer, MAX_INTEGER_DIGIT_COUNT, _base == BASE::DEC ? "%lld" : ( _base == BASE::HEX ) ? "%llx" : "%llo", longLongInteger_ );
		_wordCache = buffer;
		reformat();
		_conversionCache = _wordCache;
		do_write( _conversionCache.raw(), _conversionCache.byte_count() );
	} else {
		do_write( &longLongInteger_, static_cast<int>( sizeof ( longLongInteger_ ) ) );
	}
	return ( *this );
	M_EPILOG
}

void HStreamInterface::reformat( void ) {
	M_PROLOG
	if ( _width ) {
		int len( static_cast<int>( _wordCache.get_length() ) );
		if ( _width > len ) {
			switch ( _adjust ) {
				case ( ADJUST::LEFT ): {
					_wordCache.insert( len, _width - len, _fill );
				} break;
				case ( ADJUST::RIGHT ): {
					_wordCache.insert( 0, _width - len, _fill );
				} break;
				case ( ADJUST::CENTER ): {
					int pre( ( _width - len ) / 2 );
					_wordCache.insert( 0, pre, _fill );
					int post( ( _width - len ) - pre );
					_wordCache.insert( pre + len, post, _fill );
					break;
				}
			}
		}
		_width = 0;
	}
	return;
	M_EPILOG
}

HStreamInterface& HStreamInterface::do_output( int long unsigned unsignedLongInteger_ ) {
	M_PROLOG
	if ( _mode == MODE::TEXT ) {
		char buffer[MAX_INTEGER_DIGIT_COUNT];
		snprintf( buffer, MAX_INTEGER_DIGIT_COUNT, _base == BASE::DEC ? "%lu" : ( _base == BASE::HEX ) ? "%lx" : "%lo", unsignedLongInteger_ );
		_wordCache = buffer;
		reformat();
		_conversionCache = _wordCache;
		do_write( _conversionCache.raw(), _conversionCache.byte_count() );
	} else {
		do_write( &unsignedLongInteger_, static_cast<int>( sizeof ( unsignedLongInteger_ ) ) );
	}
	return ( *this );
	M_EPILOG
}

HStreamInterface& HStreamInterface::do_output( int long long unsigned unsignedLongLongInteger_ ) {
	M_PROLOG
	if ( _mode == MODE::TEXT ) {
		char buffer[MAX_INTEGER_DIGIT_COUNT];
		snprintf( buffer, MAX_INTEGER_DIGIT_COUNT, _base == BASE::DEC ? "%llu" : ( _base == BASE::HEX ) ? "%llx" : "%llo", unsignedLongLongInteger_ );
		_wordCache = buffer;
		reformat();
		_conversionCache = _wordCache;
		do_write( _conversionCache.raw(), _conversionCache.byte_count() );
	} else {
		do_write( &unsignedLongLongInteger_, static_cast<int>( sizeof ( unsignedLongLongInteger_ ) ) );
	}
	return ( *this );
	M_EPILOG
}

void HStreamInterface::apply_precision( void ) {
	M_PROLOG
	int dot( static_cast<int>( _wordCache.find( '.'_ycp ) ) );
	if ( dot != HString::npos ) {
		int nonZero( static_cast<int>( _wordCache.find_last_other_than( "0" ) ) );
		if ( ( nonZero != HString::npos ) && ( nonZero >= dot  ) ) {
			int len( static_cast<int>( _wordCache.get_length() ) );
			int cap( max( _floatFormat == FLOAT_FORMAT::FIXED ? _precision + 1 : 0, nonZero + ( nonZero == dot ? 0 : 1 ) ) ); /* + 1 for a dot */
			if ( cap < len ) {
				_wordCache.set_at( cap, 0_ycp );
			}
		}
	}
	return;
	M_EPILOG
}

HStreamInterface& HStreamInterface::do_output( double double_ ) {
	M_PROLOG
	if ( _mode == MODE::TEXT ) {
		char buffer[MAX_FLOAT_DIGIT_COUNT];
		char fmt[MAX_FLOAT_FORMAT_SIZE] = "%f";
		if ( _floatFormat != FLOAT_FORMAT::NATURAL ) {
			snprintf( fmt, MAX_FLOAT_FORMAT_SIZE, "%%.%d%c", _precision, _floatFormat == FLOAT_FORMAT::FIXED ? 'f' : 'e' );
		}
		snprintf( buffer, MAX_FLOAT_DIGIT_COUNT, fmt, double_ );
		_wordCache = buffer;
		apply_precision();
		reformat();
		_conversionCache = _wordCache;
		do_write( _conversionCache.raw(), _conversionCache.byte_count() );
	} else {
		do_write( &double_, static_cast<int>( sizeof ( double_ ) ) );
	}
	return ( *this );
	M_EPILOG
}

HStreamInterface& HStreamInterface::do_output( double long longDouble_ ) {
	M_PROLOG
	if ( _mode == MODE::TEXT ) {
		char buffer[MAX_FLOAT_DIGIT_COUNT];
		char fmt[MAX_FLOAT_FORMAT_SIZE] = "%.12Lf";
		if ( _floatFormat != FLOAT_FORMAT::NATURAL ) {
			snprintf( fmt, MAX_FLOAT_FORMAT_SIZE, "%%.%dL%c", _precision, _floatFormat == FLOAT_FORMAT::FIXED ? 'f' : 'e' );
		}
		snprintf( buffer, MAX_FLOAT_DIGIT_COUNT, fmt, longDouble_ );
		_wordCache = buffer;
		apply_precision();
		reformat();
		_conversionCache = _wordCache;
		do_write( _conversionCache.raw(), _conversionCache.byte_count() );
	} else {
		do_write( &longDouble_, static_cast<int>( sizeof ( longDouble_ ) ) );
	}
	return ( *this );
	M_EPILOG
}

HStreamInterface& HStreamInterface::do_output( float float_ ) {
	M_PROLOG
	if ( _mode == MODE::TEXT ) {
		char buffer[MAX_FLOAT_DIGIT_COUNT];
		char fmt[MAX_FLOAT_FORMAT_SIZE] = "%f";
		if ( _floatFormat != FLOAT_FORMAT::NATURAL ) {
			snprintf( fmt, MAX_FLOAT_FORMAT_SIZE, "%%.%d%c", _precision, _floatFormat == FLOAT_FORMAT::FIXED ? 'f' : 'e' );
		}
		snprintf( buffer, MAX_FLOAT_DIGIT_COUNT, fmt, float_ );
		_wordCache = buffer;
		apply_precision();
		reformat();
		_conversionCache = _wordCache;
		do_write( _conversionCache.raw(), _conversionCache.byte_count() );
	} else {
		do_write( &float_, static_cast<int>( sizeof ( float_ ) ) );
	}
	return ( *this );
	M_EPILOG
}

HStreamInterface& HStreamInterface::do_output( void const* ptr_ ) {
	M_PROLOG
	if ( _mode == MODE::TEXT ) {
		char buffer[MAX_INTEGER_DIGIT_COUNT];
		snprintf( buffer, MAX_INTEGER_DIGIT_COUNT, "0x%lx", reinterpret_cast<int long unsigned>( ptr_ ) );
		_wordCache = buffer;
		reformat();
		_conversionCache = _wordCache;
		do_write( _conversionCache.raw(), _conversionCache.byte_count() );
	} else {
		do_write( &ptr_, static_cast<int>( sizeof ( ptr_ ) ) );
	}
	return ( *this );
	M_EPILOG
}

HStreamInterface& HStreamInterface::do_output( manipulator_t const& HFILE_INTERFACE ) {
	M_PROLOG
	return ( HFILE_INTERFACE( *this ) );
	M_EPILOG
}

HStreamInterface& HStreamInterface::do_output( HManipulator const& manipulator_ ) {
	M_PROLOG
	manipulator_( *this );
	return ( *this );
	M_EPILOG
}

HStreamInterface& endl( HStreamInterface& file_ ) {
	M_PROLOG
	if ( file_.get_mode() != HStreamInterface::MODE::TEXT ) {
		throw HStreamInterfaceException( "Invalid output conversion for binary mode." );
	}
	file_.do_write( "\n", 1 );
	file_.do_flush();
	return ( file_ );
	M_EPILOG
}

HStreamInterface& flush( HStreamInterface& file_ ) {
	M_PROLOG
	file_.do_flush();
	return ( file_ );
	M_EPILOG
}

HStreamInterface& text( HStreamInterface& iface_ ) {
	M_PROLOG
	return ( iface_.set_mode( HStreamInterface::MODE::TEXT ) );
	M_EPILOG
}

HStreamInterface& binary( HStreamInterface& iface_ ) {
	M_PROLOG
	return ( iface_.set_mode( HStreamInterface::MODE::BINARY ) );
	M_EPILOG
}

HStreamInterface& dec( HStreamInterface& iface_ ) {
	M_PROLOG
	return ( iface_.set_base( HStreamInterface::BASE::DEC ) );
	M_EPILOG
}

HStreamInterface& hex( HStreamInterface& iface_ ) {
	M_PROLOG
	return ( iface_.set_base( HStreamInterface::BASE::HEX ) );
	M_EPILOG
}

HStreamInterface& oct( HStreamInterface& iface_ ) {
	M_PROLOG
	return ( iface_.set_base( HStreamInterface::BASE::OCT ) );
	M_EPILOG
}

HStreamInterface& skipws( HStreamInterface& iface_ ) {
	M_PROLOG
	return ( iface_.set_skipws( true ) );
	M_EPILOG
}

HStreamInterface& noskipws( HStreamInterface& iface_ ) {
	M_PROLOG
	return ( iface_.set_skipws( false ) );
	M_EPILOG
}

HStreamInterface& boolalpha( HStreamInterface& iface_ ) {
	M_PROLOG
	return ( iface_.set_boolalpha( true ) );
	M_EPILOG
}

HStreamInterface& noboolalpha( HStreamInterface& iface_ ) {
	M_PROLOG
	return ( iface_.set_boolalpha( false ) );
	M_EPILOG
}

HStreamInterface& natural( HStreamInterface& iface_ ) {
	M_PROLOG
	return ( iface_.set_float_format( HStreamInterface::FLOAT_FORMAT::NATURAL ) );
	M_EPILOG
}

HStreamInterface& fixed( HStreamInterface& iface_ ) {
	M_PROLOG
	return ( iface_.set_float_format( HStreamInterface::FLOAT_FORMAT::FIXED ) );
	M_EPILOG
}

HStreamInterface& scientific( HStreamInterface& iface_ ) {
	M_PROLOG
	return ( iface_.set_float_format( HStreamInterface::FLOAT_FORMAT::SCIENTIFIC ) );
	M_EPILOG
}

HStreamInterface& left( HStreamInterface& iface_ ) {
	M_PROLOG
	return ( iface_.set_adjust( HStreamInterface::ADJUST::LEFT ) );
	M_EPILOG
}

HStreamInterface& right( HStreamInterface& iface_ ) {
	M_PROLOG
	return ( iface_.set_adjust( HStreamInterface::ADJUST::RIGHT ) );
	M_EPILOG
}

HStreamInterface& center( HStreamInterface& iface_ ) {
	M_PROLOG
	return ( iface_.set_adjust( HStreamInterface::ADJUST::CENTER ) );
	M_EPILOG
}

HStreamInterface::HManipulator setw( int width_ ) {
	M_PROLOG
	return ( HStreamInterface::HManipulator( width_, &HStreamInterface::HManipulator::set_width ) );
	M_EPILOG
}

HStreamInterface::HManipulator setprecision( int width_ ) {
	M_PROLOG
	return ( HStreamInterface::HManipulator( width_, &HStreamInterface::HManipulator::set_precision ) );
	M_EPILOG
}

HStreamInterface::HManipulator setfill( code_point_t fill_ ) {
	M_PROLOG
	return ( HStreamInterface::HManipulator( static_cast<int>( fill_.get() ), &HStreamInterface::HManipulator::set_fill ) );
	M_EPILOG
}

int long HStreamInterface::do_read_until( HString& message_,
		char const* stopSet_, bool stripDelim_ ) {
	M_PROLOG
	return ( HStreamInterface::read_until_n( message_, meta::max_signed<int long>::value, stopSet_, stripDelim_ ) );
	M_EPILOG
}

int long HStreamInterface::do_read_until_n( HString& message_, int long maxCount_,
		char const* stopSet_, bool stripDelim_ ) {
	M_PROLOG
	return ( semantic_read( message_, maxCount_, stopSet_, stripDelim_, true ) );
	M_EPILOG
}

int long HStreamInterface::do_read_while( HString& message_,
		char const* acquire_ ) {
	M_PROLOG
	return ( HStreamInterface::do_read_while_n( message_, meta::max_signed<int long>::value, acquire_ ) );
	M_EPILOG
}

int long HStreamInterface::do_read_while_n( HString& message_, int long maxCount_,
		char const* acquire_ ) {
	M_PROLOG
	return ( semantic_read( message_, maxCount_, acquire_, false, false ) );
	M_EPILOG
}

int long HStreamInterface::read_while_retry( yaal::hcore::HString& message_,
		char const* acquire_ ) {
	M_PROLOG
	int long nRead( 0 );
	do {
		nRead = HStreamInterface::do_read_while( message_, acquire_ );
	} while ( good() && nRead < 0 );
	return ( nRead );
	M_EPILOG
}

int long HStreamInterface::read_until_retry( yaal::hcore::HString& message_,
		char const* stopSet_, bool stripDelim_ ) {
	M_PROLOG
	int long nRead( 0 );
	do {
		nRead = HStreamInterface::do_read_until( message_, stopSet_, stripDelim_ );
	} while ( good() && nRead < 0 );
	return ( nRead );
	M_EPILOG
}

int long HStreamInterface::semantic_read(
	yaal::hcore::HString& message_,
	int long maxCount_,
	char const* set_,
	bool stripDelim_,
	bool isStopSet_
) {
	M_PROLOG
	M_ASSERT( isStopSet_ || ! stripDelim_ );
	M_ASSERT( _offset >= 0 );
	int long nRead( 0 ); /* how many bytes were read in this single invocation */
	int long iPoolSize( _cache.size() );
	char* buffer( _cache.get<char>() ); /* read cache buffer */
	bool byDelim( false );
	int setLen( static_cast<int>( ::strlen( set_ ) + 1 ) ); /* + 1 for terminating \0 byte that also could be searched for */
	if ( _offset ) {
		int cached( 0 );
		int long toReadFromCache( min( _offset, static_cast<int>( maxCount_ ) ) );
		for ( ; cached < toReadFromCache; ++ cached ) {
			if ( ( byDelim = ( ::memchr( set_, buffer[ cached ], static_cast<size_t>( setLen ) ) ? isStopSet_ : ! isStopSet_ ) ) ) {
				if ( isStopSet_ ) {
					++ cached;
				}
				break;
			}
		}
		if ( ( cached == maxCount_ ) || byDelim ) {
			nRead = cached;
			message_.assign( buffer, nRead - ( ( ( nRead > 0 ) && byDelim && stripDelim_ ) ? 1 : 0 ) );
			_offset -= static_cast<int>( nRead );
			::memmove( buffer, buffer + nRead, static_cast<size_t>( _offset ) );
		}
	}
	if ( ! ( nRead || byDelim ) ) {
		do {
			/* Let's check if next read won't exceed size of our read buffer.
			 * 1 byte or terminating zero and one byte for at least one new character,
			 * hence + 2 in condition.
			 */
			if ( ( _offset + 2 ) > iPoolSize ) {
				_cache.realloc( _offset + 2 );
				buffer = _cache.get<char>(); /* update read cache buffer ptr, reallocation could move previous buffer into another memory position */
				iPoolSize = _cache.size();
			}
			/* We read only one byte at a time. */
			nRead = do_read( buffer + _offset, static_cast<int>( sizeof ( char ) ) * 1 );
			/* nRead can be one of the following:
			 * nRead > 0 - a successful read, we shall check for stop char and possibly continue reading.
			 * nRead == 0 - stream is blocking and has just been closed or has no data to read and is internally non-blocking.
			 * nRead < 0 - an error occurred, read operation could be externally interrupted.
			 */
		} while ( ( nRead > 0 ) /* We increment _offset only if read succeeded. */
				&& ( ! ( byDelim = ( ::memchr( set_, buffer[ _offset ++ ], static_cast<size_t>( setLen ) ) ? isStopSet_ : ! isStopSet_ ) ) )
				&& ( ! ( _offset >= maxCount_ ) ) );
		if ( nRead >= 0 ) {
			if ( ! nRead ) {
				_valid = false;
			}
			nRead = _offset;
			if ( byDelim && ! isStopSet_ ) {
				-- nRead;
			}
			message_.assign( buffer, nRead - ( ( ( nRead > 0 ) && byDelim && stripDelim_ ) ? 1 : 0 ) );
			if ( byDelim && ! isStopSet_ ) {
				buffer[0] = buffer[_offset - 1];
				_offset = 1;
			} else {
				_offset = 0;
			}
		} else {
			message_.clear();
		}
	}
	return ( nRead );
	M_EPILOG
}

int HStreamInterface::do_peek( void ) {
	if ( ! _offset ) {
		int long iPoolSize( _cache.size() );
		if ( iPoolSize < 1 ) {
			_cache.realloc( 1 );
		}
		int long nRead( 0 );
		do {
			nRead = do_read( _cache.raw(), 1 );
		} while ( nRead < 0 );
		if ( nRead > 0 ) {
			_offset = 1;
		}
	}
	return ( _offset > 0 ? _cache.get<char>()[ _offset - 1 ] : INVALID_CHARACTER );
}

bool HStreamInterface::read_word( void ) {
	M_PROLOG
	/* Regarding _whiteSpace_.size() + 1, about "+ 1" see comment in semantic_read in analogous context */
	if ( ! _skipWS ) {
		int peeked( HStreamInterface::do_peek() );
		if ( ( peeked == INVALID_CHARACTER )
				|| character_class<CHARACTER_CLASS::WHITESPACE>().hasz( code_point_t( static_cast<yaal::u32_t>( peeked ) ) ) ) {
			_fail = true;
		}
	}
	if ( good() ) {
		read_while_retry( _wordCache, character_class<CHARACTER_CLASS::WHITESPACE>().data() );
		if ( _skipWS ) {
			_wordCache.clear();
		}
		if ( good() ) {
			read_until_retry( _wordCache, character_class<CHARACTER_CLASS::WHITESPACE>().data(), false );
			_wordCache.trim_right( character_class<CHARACTER_CLASS::WHITESPACE>().data() );
		}
	}
	return ( _wordCache.get_length() > 0 );
	M_EPILOG
}

bool HStreamInterface::read_integer( void ) {
	M_PROLOG
	read_while_retry( _wordCache, character_class<CHARACTER_CLASS::WHITESPACE>().data() );
	_wordCache.clear();
	do {
		if ( ! good() ) {
			break;
		}
		bool neg( HStreamInterface::do_peek() == '-' );
		if ( neg ) {
			char sink( 0 );
			HStreamInterface::read( &sink, 1 );
		}
		if ( ! good() ) {
			break;
		}
		if ( _base != BASE::DEC ) {
			char zero( 0 );
			do_input( zero );
			if ( zero != '0' ) {
				_fail = true;
				break;
			}
			char base( 0 );
			do_input( base );
			if ( ( ( _base == BASE::HEX ) && ( tolower( base ) != 'x' ) ) || ( ( _base == BASE::OCT ) && ( tolower( base ) != 'o' ) ) ) {
				_fail = true;
				break;
			}
		}
		read_while_retry( _wordCache, character_class<CHARACTER_CLASS::DIGIT>().data() );
		if ( _base != BASE::DEC ) {
			_wordCache.insert( 0, _base == BASE::HEX ? "0x" : "0o" );
		}
		if ( neg ) {
			_wordCache.insert( 0, "-" );
		}
	} while ( false );
	return ( _wordCache.get_length() > 0 );
	M_EPILOG
}

bool HStreamInterface::read_floatint_point( void ) {
	M_PROLOG
	do {
		read_while_retry( _wordCache, character_class<CHARACTER_CLASS::WHITESPACE>().data() );
		_wordCache.clear();
		if ( ! good() ) {
			break;
		}
		bool neg( HStreamInterface::do_peek() == '-' );
		if ( ! good() ) {
			break;
		}
		char sink( 0 );
		if ( neg ) {
			read( &sink, 1 );
		}
		read_while_retry( _wordCache, character_class<CHARACTER_CLASS::DIGIT>().data() );
		if ( ! good() ) {
			break;
		}
		if ( neg ) {
			_wordCache.insert( 0, "-" );
		}
		bool dot( HStreamInterface::do_peek() == '.' );
		if ( dot ) {
			HStreamInterface::read( &sink, 1 );
		}
		if ( dot ) {
			HString decimal;
			read_while_retry( decimal, character_class<CHARACTER_CLASS::DIGIT>().data() );
			if ( ! decimal.is_empty() ) {
				_wordCache += sink;
				_wordCache += decimal;
			}
		}
	} while ( false );
	return ( _wordCache.get_length() > 0 );
	M_EPILOG
}

HStreamInterface& HStreamInterface::do_input( HString& word ) {
	M_PROLOG
	if ( _mode == MODE::TEXT ) {
		if ( read_word() ) {
			word = _wordCache;
		}
	} else {
		i32_t len( 0 );
		do_input( len );
		if ( _offset < len ) {
			_cache.realloc( len );
			int long toRead( len - _offset );
			_fail = do_read( _cache.get<char>() + _offset, toRead ) != toRead;
			if ( ! _fail ) {
				_offset = static_cast<int>( len );
			}
		}
		if ( good() ) {
			word.assign( _cache.get<char>(), len );
			::memmove( _cache.raw(), _cache.get<char>() + len, static_cast<size_t>( _offset - len ) );
			_offset -= static_cast<int>( len );
		}
	}
	return ( *this );
	M_EPILOG
}

HStreamInterface& HStreamInterface::do_input( bool& b ) {
	M_PROLOG
	if ( _mode == MODE::TEXT ) {
		if ( read_word() ) {
			b = lexical_cast<bool>( _wordCache );
		}
	} else {
		i8_t buffer( 0 );
		int long toRead( static_cast<int>( sizeof ( buffer ) ) );
		_fail = read( &buffer, toRead ) != toRead;
		if ( good() ) {
			b = buffer ? true : false;
		}
	}
	return ( *this );
	M_EPILOG
}

HStreamInterface& HStreamInterface::do_input( code_point_t& char_ ) {
	M_PROLOG
	if ( _mode == MODE::TEXT ) {
		/* Regarding _whiteSpace_.size() + 1, about "+ 1" see comment in semantic_read in analogous context */
		char c( 0 );
		do {
			read( &c, 1 );
		} while ( good() && _skipWS && character_class<CHARACTER_CLASS::WHITESPACE>().hasz( code_point_t( static_cast<yaal::u32_t>( c ) ) ) );
		yaal::u32_t character( static_cast<u8_t>( c ) );
		static u8_t const mask[] = { 0xff, unicode::ENC_2_BYTES_VALUE_MASK, unicode::ENC_3_BYTES_VALUE_MASK, unicode::ENC_4_BYTES_VALUE_MASK };
		int tailLength( unicode::utf8_declared_length( c ) - 1 );
		character &= mask[tailLength];
		for ( int i( 0 ); i < tailLength; ++ i ) {
			character <<= 6;
			read( &c, 1 );
			character |= ( static_cast<u8_t>( c ) & unicode::TAIL_BYTES_VALUE_MASK );
		}
		char_ = code_point_t( character );
	} else {
		u32_t buffer( 0 );
		int long toRead( static_cast<int>( sizeof ( buffer ) ) );
		_fail = read( &buffer, toRead ) != toRead;
		if ( good() ) {
			char_ = code_point_t( buffer );
		}
	}
	return ( *this );
	M_EPILOG
}

HStreamInterface& HStreamInterface::do_input( char& char_ ) {
	M_PROLOG
	char c( 0 );
	int long toRead( static_cast<int>( sizeof ( c ) ) );
	if ( _mode == MODE::TEXT ) {
		/* Regarding _whiteSpace_.size() + 1, about "+ 1" see comment in semantic_read in analogous context */
		do {
			read( &c, toRead );
		} while ( good() && _skipWS && character_class<CHARACTER_CLASS::WHITESPACE>().hasz( code_point_t( static_cast<yaal::u32_t>( c ) ) ) );
	} else {
		_fail = read( &c, toRead ) != toRead;
	}
	if ( good() ) {
		char_ = c;
	}
	return ( *this );
	M_EPILOG
}

HStreamInterface& HStreamInterface::do_input( char unsigned& cu ) {
	M_PROLOG
	char ch;
	HStreamInterface::do_input( ch );
	cu = static_cast<char unsigned>( ch );
	return ( *this );
	M_EPILOG
}

HStreamInterface& HStreamInterface::do_input( int short& is ) {
	M_PROLOG
	if ( _mode == MODE::TEXT ) {
		if ( read_integer() ) {
			is = lexical_cast<int short>( _wordCache );
		}
	} else {
		int short buffer( 0 );
		int long toRead( static_cast<int>( sizeof ( buffer ) ) );
		_fail = ( read( &buffer, toRead ) ) != toRead;
		if ( good() ) {
			is = buffer;
		}
	}
	return ( *this );
	M_EPILOG
}

HStreamInterface& HStreamInterface::do_input( int short unsigned& isu ) {
	M_PROLOG
	if ( _mode == MODE::TEXT ) {
		if ( read_integer() ) {
			isu = lexical_cast<int short unsigned>( _wordCache );
		}
	} else {
		int short unsigned buffer( 0 );
		int long toRead( static_cast<int>( sizeof ( buffer ) ) );
		_fail = ( read( &buffer, toRead ) ) != toRead;
		if ( good() ) {
			isu = buffer;
		}
	}
	return ( *this );
	M_EPILOG
}

HStreamInterface& HStreamInterface::do_input( int& i ) {
	M_PROLOG
	if ( _mode == MODE::TEXT ) {
		if ( read_integer() ) {
			i = lexical_cast<int>( _wordCache );
		}
	} else {
		int buffer( 0 );
		int long toRead( static_cast<int>( sizeof ( buffer ) ) );
		_fail = ( read( &buffer, toRead ) ) != toRead;
		if ( good() ) {
			i = buffer;
		}
	}
	return ( *this );
	M_EPILOG
}

HStreamInterface& HStreamInterface::do_input( int unsigned& iu ) {
	M_PROLOG
	if ( _mode == MODE::TEXT ) {
		if ( read_integer() ) {
			iu = lexical_cast<int unsigned>( _wordCache );
		}
	} else {
		int unsigned buffer( 0 );
		int long toRead( static_cast<int>( sizeof ( buffer ) ) );
		_fail = ( read( &buffer, toRead ) ) != toRead;
		if ( good() ) {
			iu = buffer;
		}
	}
	return ( *this );
	M_EPILOG
}

HStreamInterface& HStreamInterface::do_input( int long& il ) {
	M_PROLOG
	if ( _mode == MODE::TEXT ) {
		if ( read_integer() ) {
			il = lexical_cast<int long>( _wordCache );
		}
	} else {
		int long buffer( 0 );
		int long toRead( static_cast<int>( sizeof ( buffer ) ) );
		_fail = ( read( &buffer, toRead ) ) != toRead;
		if ( good() ) {
			il = buffer;
		}
	}
	return ( *this );
	M_EPILOG
}

HStreamInterface& HStreamInterface::do_input( int long unsigned& ilu ) {
	M_PROLOG
	if ( _mode == MODE::TEXT ) {
		if ( read_integer() ) {
			ilu = lexical_cast<int long unsigned>( _wordCache );
		}
	} else {
		int long unsigned buffer( 0 );
		int long toRead( static_cast<int>( sizeof ( buffer ) ) );
		_fail = ( read( &buffer, toRead ) ) != toRead;
		if ( good() ) {
			ilu = buffer;
		}
	}
	return ( *this );
	M_EPILOG
}

HStreamInterface& HStreamInterface::do_input( int long long& ill ) {
	M_PROLOG
	if ( _mode == MODE::TEXT ) {
		if ( read_integer() ) {
			ill = lexical_cast<int long long>( _wordCache );
		}
	} else {
		int long long buffer( 0 );
		int long toRead( static_cast<int>( sizeof ( buffer ) ) );
		_fail = ( read( &buffer, toRead ) ) != toRead;
		if ( good() ) {
			ill = buffer;
		}
	}
	return ( *this );
	M_EPILOG
}

HStreamInterface& HStreamInterface::do_input( int long long unsigned& illu ) {
	M_PROLOG
	if ( _mode == MODE::TEXT ) {
		if ( read_integer() ) {
			illu = lexical_cast<int long long unsigned>( _wordCache );
		}
	} else {
		int long long unsigned buffer( 0 );
		int long toRead( static_cast<int>( sizeof ( buffer ) ) );
		_fail = ( read( &buffer, toRead ) ) != toRead;
		if ( good() ) {
			illu = buffer;
		}
	}
	return ( *this );
	M_EPILOG
}

HStreamInterface& HStreamInterface::do_input( double& d ) {
	M_PROLOG
	if ( _mode == MODE::TEXT ) {
		if ( read_floatint_point() ) {
			d = lexical_cast<double>( _wordCache );
		}
	} else {
		double buffer( 0 );
		int long toRead( static_cast<int>( sizeof ( buffer ) ) );
		_fail = ( read( &buffer, toRead ) ) != toRead;
		if ( good() ) {
			d = buffer;
		}
	}
	return ( *this );
	M_EPILOG
}

HStreamInterface& HStreamInterface::do_input( double long& dl ) {
	M_PROLOG
	if ( _mode == MODE::TEXT ) {
		if ( read_floatint_point() ) {
			dl = lexical_cast<double long>( _wordCache );
		}
	} else {
		double long buffer( 0 );
		int long toRead( static_cast<int>( sizeof ( buffer ) ) );
		_fail = ( read( &buffer, toRead ) ) != toRead;
		if ( good() ) {
			dl = buffer;
		}
	}
	return ( *this );
	M_EPILOG
}

HStreamInterface& HStreamInterface::do_input( float& f ) {
	M_PROLOG
	if ( _mode == MODE::TEXT ) {
		if ( read_floatint_point() ) {
			f = lexical_cast<float>( _wordCache );
		}
	} else {
		float buffer( 0 );
		int long toRead( static_cast<int>( sizeof ( buffer ) ) );
		_fail = ( read( &buffer, toRead ) ) != toRead;
		if ( good() ) {
			f = buffer;
		}
	}
	return ( *this );
	M_EPILOG
}

HStreamInterface& HStreamInterface::do_input( void const*& pointer_ ) {
	M_PROLOG
	if ( _mode == MODE::TEXT ) {
		size_t val( 0 );
		do_input( val );
		pointer_ = reinterpret_cast<void const*>( val );
	} else {
		void const* buffer( nullptr );
		int long toRead( static_cast<int>( sizeof ( buffer ) ) );
		_fail = ( read( &buffer, toRead ) ) != toRead;
		if ( good() ) {
			pointer_ = buffer;
		}
	}
	return ( *this );
	M_EPILOG
}

HStreamInterface& HStreamInterface::do_input( manipulator_t const& HFILE_INTERFACE ) {
	M_PROLOG
	return ( HFILE_INTERFACE( *this ) );
	M_EPILOG
}

int long HStreamInterface::read( void* buffer_, int long size_ ) {
	M_PROLOG
	int long nRead( 0 );
	if ( _offset ) {
		void* buffer( _cache.raw() );
		if ( _offset > size_ ) {
			::memcpy( buffer_, buffer, static_cast<size_t>( nRead = size_ ) );
			::memmove( buffer, static_cast<char const*>( buffer ) + size_, static_cast<size_t>( _offset - size_ ) );
			_offset -= static_cast<int>( size_ );
		} else {
			::memcpy( buffer_, buffer, static_cast<size_t>( nRead = _offset ) );
			size_ -= _offset;
			_offset = 0;
		}
	}
	if ( size_ > 0 ) {
		int long physRead( do_read( static_cast<char*>( buffer_ ) + nRead, size_ ) );
		if ( physRead >= 0 ) {
			nRead += physRead;
		} else if ( ! nRead ) {
			nRead = physRead;
		}
		if ( ! physRead ) {
			_valid = false;
		}
	}
	return ( nRead );
	M_EPILOG
}

void HStreamInterface::seek( int long offset_, SEEK anchor_ ) {
	M_PROLOG
	_offset = 0;
	clear();
	do_seek( offset_, anchor_ );
	M_EPILOG
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpragmas"
#pragma GCC diagnostic ignored "-Wunknown-pragmas"
#pragma GCC diagnostic ignored "-Wunknown-warning-option"
#pragma GCC diagnostic ignored "-Wsuggest-attribute=noreturn"
void HStreamInterface::do_seek( int long, SEEK ) {
	M_PROLOG
	throw HStreamInterfaceException( "Seeking on this stream is not supported." );
	M_EPILOG
}
#pragma GCC diagnostic pop

int long HStreamInterface::write( void const* buffer_, int long size_ ) {
	M_PROLOG
	return ( do_write( buffer_, size_ ) );
	M_EPILOG
}

bool HStreamInterface::is_valid( void ) const {
	M_PROLOG
	return ( _valid && do_is_valid() );
	M_EPILOG
}

bool HStreamInterface::do_good( void ) const {
	return ( ! ( fail() || bad() ) );
}

bool HStreamInterface::do_fail( void ) const {
	return ( _fail || ! is_valid() );
}

bool HStreamInterface::do_bad( void ) const {
	return ( ! is_valid() );
}

void HStreamInterface::do_clear( void ) {
	_fail = false;
	_valid = true;
	return;
}

void HStreamInterface::flush( void ) {
	M_PROLOG
	do_flush();
	return;
	M_EPILOG
}

HStreamInterface& HStreamInterface::do_set_fill( code_point_t fill_ ) {
	M_PROLOG
	_fill = fill_;
	return ( *this );
	M_EPILOG
}

HStreamInterface& HStreamInterface::do_set_width( int width_ ) {
	M_PROLOG
	_width = width_;
	return ( *this );
	M_EPILOG
}

HStreamInterface& HStreamInterface::do_set_precision( int precision_ ) {
	M_PROLOG
	_precision = precision_;
	return ( *this );
	M_EPILOG
}

HStreamInterface& HStreamInterface::do_set_mode( MODE mode_ ) {
	M_PROLOG
	_mode = mode_;
	return ( *this );
	M_EPILOG
}

HStreamInterface& HStreamInterface::do_set_base( BASE base_ ) {
	M_PROLOG
	_base = base_;
	return ( *this );
	M_EPILOG
}

HStreamInterface& HStreamInterface::do_set_float_format( FLOAT_FORMAT floatFormat_ ) {
	M_PROLOG
	_floatFormat = floatFormat_;
	return ( *this );
	M_EPILOG
}

HStreamInterface& HStreamInterface::do_set_adjust( ADJUST adjust_ ) {
	M_PROLOG
	_adjust = adjust_;
	return ( *this );
	M_EPILOG
}

HStreamInterface& HStreamInterface::do_set_skipws( bool skipWS_ ) {
	M_PROLOG
	_skipWS = skipWS_;
	return ( *this );
	M_EPILOG
}

HStreamInterface& HStreamInterface::do_set_boolalpha( bool boolalpha_ ) {
	M_PROLOG
	_boolAlpha = boolalpha_;
	return ( *this );
	M_EPILOG
}

bool HStreamInterface::do_get_skipws( void ) const {
	return ( _skipWS );
}

bool HStreamInterface::do_get_boolalpha( void ) const {
	return ( _boolAlpha );
}

code_point_t HStreamInterface::do_get_fill( void ) const {
	return ( _fill );
}

int HStreamInterface::do_get_width( void ) const {
	return ( _width );
}

int HStreamInterface::do_get_precision( void ) const {
	return ( _precision );
}

HStreamInterface::MODE HStreamInterface::do_get_mode( void ) const {
	return ( _mode );
}

HStreamInterface::BASE HStreamInterface::do_get_base( void ) const {
	return ( _base );
}

HStreamInterface::FLOAT_FORMAT HStreamInterface::do_get_float_format( void ) const {
	return ( _floatFormat );
}

HStreamInterface::ADJUST HStreamInterface::do_get_adjust( void ) const {
	return ( _adjust );
}

HStreamInterface::HManipulator::HManipulator( int value_, ACTION_t action_ )
	: _value( value_ ), _action( action_ ) {
}

void HStreamInterface::HManipulator::operator()( HStreamInterface& iface_ ) const {
	M_PROLOG
	(this->*_action)( iface_ );
	return;
	M_EPILOG
}

void HStreamInterface::HManipulator::set_fill( HStreamInterface& iface_ ) const {
	M_PROLOG
	iface_.set_fill( code_point_t( static_cast<yaal::u32_t>( _value ) ) );
	return;
	M_EPILOG
}

void HStreamInterface::HManipulator::set_width( HStreamInterface& iface_ ) const {
	M_PROLOG
	iface_.set_width( _value );
	return;
	M_EPILOG
}

void HStreamInterface::HManipulator::set_precision( HStreamInterface& iface_ ) const {
	M_PROLOG
	iface_.set_precision( _value );
	return;
	M_EPILOG
}

HStreamInterface& getline( HStreamInterface& stream_, yaal::hcore::HString& store_ ) {
	M_PROLOG
	stream_.read_until( store_, "\n" );
	store_.trim_right( "\r" );
	return ( stream_ );
	M_EPILOG
}

}

}

