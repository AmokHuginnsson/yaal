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
#include "hcore.hxx"

namespace yaal {

namespace hcore {

static int const INVALID_CHARACTER( -256 );
static int const MAX_INTEGER_DIGIT_COUNT( 64 );
static int const MAX_FLOAT_DIGIT_COUNT( 8192 );
static int const MAX_FLOAT_FORMAT_SIZE( 64 );
static int const DEFAULT_IO_BUFFER_SIZE( 1024 );
char const HStreamInterface::eol[] = "\n";

HStreamInterface::HStreamInterface( void )
	: _wordCache()
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
	, _bufferedIO( true )
	, _ioBufferSize( DEFAULT_IO_BUFFER_SIZE )
	, _valid( true )
	, _fail( false )
	, _cache( 1, HChunk::STRATEGY::GEOMETRIC )
	, _cachedBytes( 0 )
	, _cacheContent( CACHE_CONTENT::READ ) {
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
	write( _conversionCache.raw(), _conversionCache.byte_count() );
	return ( *this );
	M_EPILOG
}

HStreamInterface& HStreamInterface::do_output( char const* string_ ) {
	M_PROLOG
	if ( _mode == MODE::TEXT ) {
		_wordCache = string_;
		reformat();
		_conversionCache = _wordCache;
		write( _conversionCache.raw(), _conversionCache.byte_count() );
	} else {
		i32_t len( static_cast<i32_t>( strlen( string_ ) ) );
		do_output( len );
		write( string_, len );
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
		write( _conversionCache.raw(), _conversionCache.byte_count() );
	} else {
		i8_t buffer( bool_ ? 1 : 0 );
		write( &buffer, static_cast<int>( sizeof ( buffer ) ) );
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
		write( _conversionCache.raw(), _conversionCache.byte_count() );
	} else {
		write( &char_.get(), static_cast<int>( sizeof ( char_.get() ) ) );
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
		write( _conversionCache.raw(), _conversionCache.byte_count() );
	} else {
		write( &char_, static_cast<int>( sizeof ( char_ ) ) );
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
		write( _conversionCache.raw(), _conversionCache.byte_count() );
	} else {
		write( &charUnsigned_, static_cast<int>( sizeof ( charUnsigned_ ) ) );
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
		write( &shortInteger_, static_cast<int>( sizeof ( shortInteger_ ) ) );
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
		write( &unsignedShortInteger_, static_cast<int>( sizeof ( unsignedShortInteger_ ) ) );
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
		write( &integer_, static_cast<int>( sizeof ( integer_ ) ) );
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
		write( &unsignedInteger_, static_cast<int>( sizeof ( unsignedInteger_ ) ) );
	}
	return ( *this );
	M_EPILOG
}

HStreamInterface& HStreamInterface::do_output( int long longInteger_ ) {
	M_PROLOG
	if ( _mode == MODE::TEXT ) {
		char buffer[MAX_INTEGER_DIGIT_COUNT];
		snprintf(
			buffer,
			MAX_INTEGER_DIGIT_COUNT,
			_base == BASE::DEC
				? "%ld"
				: (
					( _base == BASE::HEX ) ? "%lx" : "%lo"
				),
			longInteger_
		);
		_wordCache = buffer;
		reformat();
		_conversionCache = _wordCache;
		write( _conversionCache.raw(), _conversionCache.byte_count() );
	} else {
		write( &longInteger_, static_cast<int>( sizeof ( longInteger_ ) ) );
	}
	return ( *this );
	M_EPILOG
}

HStreamInterface& HStreamInterface::do_output( int long long longLongInteger_ ) {
	M_PROLOG
	if ( _mode == MODE::TEXT ) {
		char buffer[MAX_INTEGER_DIGIT_COUNT];
		snprintf(
			buffer,
			MAX_INTEGER_DIGIT_COUNT,
			_base == BASE::DEC
				? "%lld"
				: (
					( _base == BASE::HEX ) ? "%llx" : "%llo"
				),
			longLongInteger_
		);
		_wordCache = buffer;
		reformat();
		_conversionCache = _wordCache;
		write( _conversionCache.raw(), _conversionCache.byte_count() );
	} else {
		write( &longLongInteger_, static_cast<int>( sizeof ( longLongInteger_ ) ) );
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
		write( _conversionCache.raw(), _conversionCache.byte_count() );
	} else {
		write( &unsignedLongInteger_, static_cast<int>( sizeof ( unsignedLongInteger_ ) ) );
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
		write( _conversionCache.raw(), _conversionCache.byte_count() );
	} else {
		write( &unsignedLongLongInteger_, static_cast<int>( sizeof ( unsignedLongLongInteger_ ) ) );
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
		write( _conversionCache.raw(), _conversionCache.byte_count() );
	} else {
		write( &double_, static_cast<int>( sizeof ( double_ ) ) );
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
		write( _conversionCache.raw(), _conversionCache.byte_count() );
	} else {
		double_long_storage doubleLongStorage;
		store_double_long( longDouble_, doubleLongStorage );
		static_assert( sizeof ( longDouble_ ) == SIZEOF_DOUBLE_LONG, "Inconsistent sizeof ( double long )." );
		write( doubleLongStorage.data, static_cast<int>( sizeof ( longDouble_ ) ) );
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
		write( _conversionCache.raw(), _conversionCache.byte_count() );
	} else {
		write( &float_, static_cast<int>( sizeof ( float_ ) ) );
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
		write( _conversionCache.raw(), _conversionCache.byte_count() );
	} else {
		write( &ptr_, static_cast<int>( sizeof ( ptr_ ) ) );
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
	file_.write( "\n", 1 );
	file_.flush();
	return file_;
	M_EPILOG
}

HStreamInterface& flush( HStreamInterface& file_ ) {
	M_PROLOG
	file_.flush();
	return file_;
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
	return ( HStreamInterface::read_until_n( message_, meta::max_signed<int>::value, stopSet_, stripDelim_ ) );
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
	return ( HStreamInterface::do_read_while_n( message_, meta::max_signed<int>::value, acquire_ ) );
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
	int long nRead( -1 );
	while ( good() && ( nRead < 0 ) ) {
		nRead = HStreamInterface::do_read_while( message_, acquire_ );
	}
	return nRead;
	M_EPILOG
}

int long HStreamInterface::read_until_retry( yaal::hcore::HString& message_,
		char const* stopSet_, bool stripDelim_ ) {
	M_PROLOG
	int long nRead( -1 );
	while ( good() && ( nRead < 0 ) ) {
		nRead = HStreamInterface::do_read_until( message_, stopSet_, stripDelim_ );
	}
	return nRead;
	M_EPILOG
}

inline char const* find_first_of( char const* buffer_, int long size_, char const* set_, int setLen_ ) {
	char const* ptr( nullptr );
	int long size( size_ );
	int long smallestIndex( size_ + 1 );
	for ( int i( 0 ); i < setLen_; ++ i ) {
		char const* p( static_cast<char const*>( ::memchr( buffer_, set_[i], static_cast<size_t>( size ) ) ) );
		if ( ! p ) {
			continue;
		}
		int long index( p - buffer_ );
		if ( index >= smallestIndex ) {
			continue;
		}
		smallestIndex = index;
		ptr = p;
	}
	return ptr;
}

inline char const* find_first_other_than( char const* buffer_, int long size_, char const* set_, int setLen_ ) {
	char const* ptr( nullptr );
	for ( int i( 0 ); i < size_; ++ i ) {
		char const* p( static_cast<char const*>( ::memchr( set_, buffer_[i], static_cast<size_t>( setLen_ ) ) ) );
		if ( p ) {
			continue;
		}
		ptr = buffer_ + i;
		break;
	}
	return ptr;
}

inline char const* find_delim( char const* buffer_, int long size_, char const* set_, int setLen_, bool isStopSet_ ) {
	return ( ( isStopSet_ ? find_first_of : find_first_other_than )( buffer_, size_, set_, setLen_ ) );
}

inline void consume( HString& message_, int long consumedSize_, char* buffer_, int& bufferSize_, bool delim_, bool stripDelim_ ) {
	int long messageByteCount( consumedSize_ - ( ( ( consumedSize_ > 0 ) && delim_ && stripDelim_ ) ? 1 : 0 ) );
	bytes_to_string( message_, buffer_, messageByteCount );
	bufferSize_ -= static_cast<int>( consumedSize_ );
	::memmove( buffer_, buffer_ + consumedSize_, static_cast<size_t>( bufferSize_ ) );
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
	M_ASSERT( _cachedBytes >= 0 );
	flush_write_buffer();
	int long nRead( 0 ); /* how many bytes were read in this single invocation */
	int long iPoolSize( _cache.size() );
	char* buffer( _cache.get<char>() ); /* read cache buffer */
	bool byDelim( false );
	int setLen( static_cast<int>( ::strlen( set_ ) + 1 ) ); /* + 1 for terminating \0 byte that also could be searched for */
	if ( _cachedBytes ) {
		int cached( 0 );
		int long toReadFromCache( min( _cachedBytes, static_cast<int>( maxCount_ ) ) );
		char const* p( find_delim( buffer, toReadFromCache, set_, setLen, isStopSet_ ) );
		if ( p ) {
			byDelim = true;
			cached = static_cast<int>( p - buffer ) + ( isStopSet_ ? 1 : 0 );
		} else {
			cached = static_cast<int>( toReadFromCache );
		}
		if ( ( cached == maxCount_ ) || byDelim ) {
			nRead = cached;
			hcore::consume( message_, nRead, buffer, _cachedBytes, byDelim, stripDelim_ );
		}
	}
	if ( ! ( nRead || byDelim ) ) {
		int long reqReadSize( _bufferedIO ? _ioBufferSize : 1 );
		typedef int long ( HStreamInterface::* phys_reader_t )( void*, int long );
		phys_reader_t physReader( _bufferedIO ? &HStreamInterface::do_read_some : &HStreamInterface::do_read );
		while ( true ) {
			int long readSize( min<int long>( reqReadSize, maxCount_ - _cachedBytes ) );
			M_ASSERT( readSize > 0 );
			/* Let's check if next read won't exceed size of our read buffer.
			 * 1 byte or terminating zero and one byte for at least one new character,
			 * hence + 2 in condition.
			 */
			if ( ( _cachedBytes + readSize ) > iPoolSize ) {
				_cache.realloc( _cachedBytes + readSize );
				buffer = _cache.get<char>(); /* update read cache buffer ptr, reallocation could move previous buffer into another memory position */
				iPoolSize = _cache.size();
			}
			nRead = (this->*physReader)( buffer + _cachedBytes, static_cast<int>( sizeof ( char ) ) * readSize );
			/*
			 * nRead can be one of the following:
			 * nRead > 0 - a successful read, we shall check for stop char and possibly continue reading.
			 * nRead == 0 - stream is blocking and has just been closed or has no data to read and is internally non-blocking.
			 * nRead < 0 - an error occurred, read operation could be externally interrupted.
			 */
			if ( nRead <= 0 ) {
				break;
			}
			char const* p( find_delim( buffer + _cachedBytes, nRead, set_, setLen, isStopSet_ ) );
			byDelim = p != nullptr;
			/* We increment _cachedBytes only if read succeeded. */
			_cachedBytes += static_cast<int>( nRead );
			_cacheContent = CACHE_CONTENT::READ;
			if ( byDelim ) {
				nRead = ( p - buffer ) + 1;
				break;
			}
			if ( _cachedBytes >= maxCount_ ) {
				nRead = maxCount_;
				break;
			}
		}
		if ( nRead >= 0 ) {
			_valid = !! nRead;
			if ( ! byDelim ) {
				nRead = _cachedBytes;
			} else if ( ! isStopSet_ ) {
				-- nRead;
			}
			hcore::consume( message_, nRead, buffer, _cachedBytes, byDelim, stripDelim_ );
		} else {
			message_.clear();
		}
	}
	/*
	 * Returns number of bytes read which can be quite different than message_.length(), because:
	 * 1. nRead can carry error information ( nRead < 0 )
	 * 2. nRead is number of *bytes* as opposed to number of *code points* in message_.
	 */
	return nRead;
	M_EPILOG
}

HStreamInterface& HStreamInterface::do_consume( yaal::hcore::HString const& pattern_ ) {
	M_PROLOG
	_conversionCache.assign( pattern_ );
	int len( static_cast<int>( _conversionCache.byte_count() ) );
	char const* p( _conversionCache.raw() );
	char dummy( 0 );
	for ( int i( 0 ); good() && ( i < len ); ++ i ) {
		int extracted( do_peek() );
		if ( ( extracted == INVALID_CHARACTER ) || ( extracted != p[i] ) ) {
			_fail = true;
			break;
		}
		read( &dummy, 1 );
	}
	return ( *this );
	M_EPILOG
}

HStreamInterface& HStreamInterface::do_ignore( int count_ ) {
	M_PROLOG
	static int const IGNORE_SIZE( 1024 );
	char buffer[IGNORE_SIZE];
	while ( good() && ( count_ > 0 ) ) {
		int toRead( min( IGNORE_SIZE, count_ ) );
		int nRead( static_cast<int>( read( buffer, toRead ) ) );
		if ( nRead != toRead ) {
			break;
		}
		count_ -= nRead;
	}
	return ( *this );
	M_EPILOG
}

int HStreamInterface::do_peek( void ) {
	flush_write_buffer();
	if ( ! _cachedBytes && good() ) {
		int long iPoolSize( _cache.size() );
		if ( iPoolSize < 1 ) {
			_cache.realloc( 1 );
		}
		int long nRead( 0 );
		do {
			nRead = do_read( _cache.raw(), 1 );
		} while ( nRead < 0 );
		if ( nRead > 0 ) {
			_cachedBytes = 1;
			_cacheContent = CACHE_CONTENT::READ;
		}
	}
	return ( _cachedBytes > 0 ? _cache.get<char>()[ 0 ] : INVALID_CHARACTER );
}

int HStreamInterface::do_immediate_read_size( void ) const {
	return ( _cacheContent == CACHE_CONTENT::READ ? _cachedBytes : 0 );
}

int HStreamInterface::do_pending_write_size( void ) const {
	return ( _cacheContent == CACHE_CONTENT::WRITE ? _cachedBytes : 0 );
}

bool HStreamInterface::read_word( void ) {
	M_PROLOG
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

bool HStreamInterface::read_floating_point( void ) {
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
		if ( _cachedBytes < len ) {
			_cache.realloc( len );
			int long toRead( len - _cachedBytes );
			_fail = do_read( _cache.get<char>() + _cachedBytes, toRead ) != toRead;
			if ( ! _fail ) {
				_cachedBytes = static_cast<int>( len );
				_cacheContent = CACHE_CONTENT::READ;
			}
		}
		if ( good() ) {
			word.assign( _cache.get<char>(), len );
			::memmove( _cache.raw(), _cache.get<char>() + len, static_cast<size_t>( _cachedBytes - len ) );
			_cachedBytes -= static_cast<int>( len );
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
			try {
				is = lexical_cast<int short>( _wordCache );
			} catch ( ... ) {
				_fail = true;
				throw;
			}
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
			try {
				isu = lexical_cast<int short unsigned>( _wordCache );
			} catch ( ... ) {
				_fail = true;
				throw;
			}
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
			try {
				i = lexical_cast<int>( _wordCache );
			} catch ( ... ) {
				_fail = true;
				throw;
			}
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
			try {
				iu = lexical_cast<int unsigned>( _wordCache );
			} catch ( ... ) {
				_fail = true;
				throw;
			}
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
			try {
				il = lexical_cast<int long>( _wordCache );
			} catch ( ... ) {
				_fail = true;
				throw;
			}
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
			try {
				ilu = lexical_cast<int long unsigned>( _wordCache );
			} catch ( ... ) {
				_fail = true;
				throw;
			}
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
			try {
				ill = lexical_cast<int long long>( _wordCache );
			} catch ( ... ) {
				_fail = true;
				throw;
			}
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
			try {
				illu = lexical_cast<int long long unsigned>( _wordCache );
			} catch ( ... ) {
				_fail = true;
				throw;
			}
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
		if ( read_floating_point() ) {
			try {
				d = lexical_cast<double>( _wordCache );
			} catch ( ... ) {
				_fail = true;
				throw;
			}
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
		if ( read_floating_point() ) {
			try {
				dl = lexical_cast<double long>( _wordCache );
			} catch ( ... ) {
				_fail = true;
				throw;
			}
		}
	} else {
		static_assert( sizeof ( dl ) == SIZEOF_DOUBLE_LONG, "Inconsistent sizeof ( double long )." );
		int long toRead( static_cast<int>( sizeof ( dl ) ) );
		double_long_storage doubleLongStorage;
		_fail = ( read( doubleLongStorage.data, toRead ) ) != toRead;
		if ( good() ) {
			dl = load_double_long( doubleLongStorage );
		}
	}
	return ( *this );
	M_EPILOG
}

HStreamInterface& HStreamInterface::do_input( float& f ) {
	M_PROLOG
	if ( _mode == MODE::TEXT ) {
		if ( read_floating_point() ) {
			try {
				f = lexical_cast<float>( _wordCache );
			} catch ( ... ) {
				_fail = true;
				throw;
			}
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

int long HStreamInterface::do_read_some( void* buffer_, int long size_ ) {
	return ( do_read( buffer_, size_ ) );
}

void HStreamInterface::do_unread( void const* buffer_, int long size_ ) {
	M_PROLOG
	flush_write_buffer();
	M_ASSERT( _cacheContent == CACHE_CONTENT::READ );
	_cache.realloc( _cachedBytes + size_ );
	char* cache( _cache.get<char>() );
	::memmove( cache + size_, cache, static_cast<size_t>( _cachedBytes ) );
	::memcpy( cache, buffer_, static_cast<size_t>( size_ ) );
	_cachedBytes += static_cast<int>( size_ );
	return;
	M_EPILOG
}

int long HStreamInterface::read( void* buffer_, int long size_ ) {
	M_PROLOG
	int long nRead( 0 );
	flush_write_buffer();
	if ( _cachedBytes ) {
		void* buffer( _cache.raw() );
		if ( _cachedBytes > size_ ) {
			::memcpy( buffer_, buffer, static_cast<size_t>( nRead = size_ ) );
			::memmove( buffer, static_cast<char const*>( buffer ) + size_, static_cast<size_t>( _cachedBytes - size_ ) );
			_cachedBytes -= static_cast<int>( size_ );
			size_ = 0;
		} else {
			::memcpy( buffer_, buffer, static_cast<size_t>( nRead = _cachedBytes ) );
			size_ -= _cachedBytes;
			_cachedBytes = 0;
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
	return nRead;
	M_EPILOG
}

void HStreamInterface::do_reset( void ) {
	M_PROLOG
	_cachedBytes = 0;
	clear();
	return;
	M_EPILOG
}

void HStreamInterface::seek( int long offset_, SEEK anchor_ ) {
	M_PROLOG
	reset();
	do_seek( offset_, anchor_ );
	return;
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
	if ( size_ == 0 ) {
		return ( 0 );
	}
	if ( ! _bufferedIO || ( ( _cacheContent == CACHE_CONTENT::READ ) && ( _cachedBytes > 0 ) ) ) {
		return ( do_write( buffer_, size_ ) );
	}
	int maxWrite( max( max( _ioBufferSize, static_cast<int>( _cache.get_size() ) ) - _cachedBytes, 0 ) );
	if ( ( size_ > maxWrite ) && ( _cachedBytes > 0 ) ) {
		do_write( _cache.raw(), _cachedBytes );
		maxWrite += _cachedBytes;
		_cachedBytes = 0;
	}
	if ( size_ > maxWrite ) {
		M_ASSERT( _cachedBytes == 0 );
		return ( do_write( buffer_, size_ ) );
	}
	_cache.realloc( _cachedBytes + size_ );
	::memcpy( static_cast<char*>( _cache.raw() ) + _cachedBytes, buffer_, static_cast<size_t>( size_ ) );
	_cachedBytes += static_cast<int>( size_ );
	_cacheContent = CACHE_CONTENT::WRITE;
	M_ASSERT( _cachedBytes <= static_cast<int>( _cache.get_size() ) );
	if ( _cachedBytes == static_cast<int>( _cache.get_size() ) ) {
		do_write( _cache.raw(), _cachedBytes );
		_cachedBytes = 0;
		_cacheContent = CACHE_CONTENT::READ;
	}
	return size_;
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

void HStreamInterface::flush_write_buffer( void ) {
	M_PROLOG
	if ( ( _cacheContent != CACHE_CONTENT::WRITE ) || ( _cachedBytes == 0 ) ) {
		return;
	}
	flush();
	return;
	M_EPILOG
}

void HStreamInterface::flush( void ) {
	M_PROLOG
	if ( ( _cacheContent == CACHE_CONTENT::WRITE ) && ( _cachedBytes > 0 ) ) {
		do_write( _cache.raw(), _cachedBytes );
		_cachedBytes = 0;
		_cacheContent = CACHE_CONTENT::READ;
	}
	do_flush();
	return;
	M_EPILOG
}

void const* HStreamInterface::data( void ) const {
	const_cast<HStreamInterface*>( this )->flush_write_buffer();
	return ( do_data() );
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

HStreamInterface& HStreamInterface::do_set_buffered_io( bool bufferedIo_ ) {
	M_PROLOG
	flush_write_buffer();
	_bufferedIO = bufferedIo_;
	return ( *this );
	M_EPILOG
}

HStreamInterface& HStreamInterface::do_set_io_buffer_size( int ioBufferSize_ ) {
	M_PROLOG
	flush_write_buffer();
	_ioBufferSize = ioBufferSize_;
	return ( *this );
	M_EPILOG
}

bool HStreamInterface::do_get_skipws( void ) const {
	return ( _skipWS );
}

bool HStreamInterface::do_get_boolalpha( void ) const {
	return ( _boolAlpha );
}

bool HStreamInterface::do_get_buffered_io( void ) const {
	return ( _bufferedIO );
}

int HStreamInterface::do_get_io_buffer_size( void ) const {
	return ( _ioBufferSize );
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
	stream_.read_until( store_, HStreamInterface::eol );
	store_.trim_right( "\r" );
	return stream_;
	M_EPILOG
}

}

}

