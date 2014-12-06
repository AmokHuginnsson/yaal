/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

  hstreaminterface.cxx - this file is integral part of `yaal' project.

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

#include "base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "hstreaminterface.hxx"

#include "hcore/hstring.hxx"

namespace yaal {

namespace hcore {

static int const INVALID_CHARACTER = -256;
char const* const HStreamInterface::eols = "\r\n"; /* order matters */

HStreamInterface::HStreamInterface( void )
	: _cache( 1, HChunk::STRATEGY::GEOMETRIC ), _offset( 0 ),
	_wordCache(), _fill( ' ' ), _width( 0 ), _precision( 6 ),
	_base( BASES::DEC ), _floatFormat( FLOAT_FORMAT::NATURAL ),
	_skipWS( true ), _boolAlpha( false ), _valid( true ), _fail( false ) {
	return;
}

HStreamInterface::~HStreamInterface( void ) {
	return;
}

HStreamInterface& HStreamInterface::do_output( HString const& string_ ) {
	M_PROLOG
	_wordCache = string_;
	int long len( reformat() );
	if ( len > 0 )
		do_write( _wordCache.raw(), len );
	return ( *this );
	M_EPILOG
}

HStreamInterface& HStreamInterface::do_output( char const* string_ ) {
	M_PROLOG
	_wordCache = string_;
	int long len( reformat() );
	if ( len > 0 )
		do_write( _wordCache.raw(), len );
	return ( *this );
	M_EPILOG
}

HStreamInterface& HStreamInterface::do_output( bool bool_ ) {
	M_PROLOG
	_wordCache = _boolAlpha ? ( bool_ ? "true" : "false" ) : ( bool_ ? "1" : "0" );
	int long len( reformat() );
	do_write( _wordCache.raw(), len );
	return ( *this );
	M_EPILOG
}

HStreamInterface& HStreamInterface::do_output( char char_ ) {
	M_PROLOG
	_wordCache = char_;
	int long len( reformat() );
	do_write( _wordCache.raw(), len );
	return ( *this );
	M_EPILOG
}

HStreamInterface& HStreamInterface::do_output( char unsigned charUnsigned_ ) {
	M_PROLOG
	_wordCache = charUnsigned_;
	int long len( reformat() );
	do_write( _wordCache.raw(), len );
	return ( *this );
	M_EPILOG
}

HStreamInterface& HStreamInterface::do_output( int short shortInteger_ ) {
	M_PROLOG
	int long tmp( shortInteger_ );
	return ( do_output( tmp ) );
	M_EPILOG
}

HStreamInterface& HStreamInterface::do_output( int short unsigned unsignedShortInteger_ ) {
	M_PROLOG
	int long unsigned tmp( unsignedShortInteger_ );
	return ( do_output( tmp ) );
	M_EPILOG
}

HStreamInterface& HStreamInterface::do_output( int integer_ ) {
	M_PROLOG
	int long tmp( integer_ );
	return ( do_output( tmp ) );
	M_EPILOG
}

HStreamInterface& HStreamInterface::do_output( int unsigned unsignedInteger_ ) {
	M_PROLOG
	int long unsigned tmp( unsignedInteger_ );
	return ( do_output( tmp ) );
	M_EPILOG
}

HStreamInterface& HStreamInterface::do_output( int long longInteger_ ) {
	M_PROLOG
	_wordCache.format( _base == BASES::DEC ? "%ld" : ( _base == BASES::HEX ) ? "%lx" : "%lo", longInteger_ );
	int long len( reformat() );
	do_write( _wordCache.raw(), len );
	return ( *this );
	M_EPILOG
}

HStreamInterface& HStreamInterface::do_output( int long long longLongInteger_ ) {
	M_PROLOG
	_wordCache.format( _base == BASES::DEC ? "%lld" : ( _base == BASES::HEX ) ? "%llx" : "%llo", longLongInteger_ );
	int long len( reformat() );
	do_write( _wordCache.raw(), len );
	return ( *this );
	M_EPILOG
}

int long HStreamInterface::reformat( void ) {
	M_PROLOG
	int long len( _wordCache.get_length() );
	if ( _width ) {
		if ( _width > len ) {
			_wordCache.insert( 0, _width - len, static_cast<char>( _fill ) );
			len = _width;
		}
		_width = 0;
	}
	return ( len );
	M_EPILOG
}

HStreamInterface& HStreamInterface::do_output( int long unsigned unsignedLongInteger_ ) {
	M_PROLOG
	_wordCache.format( _base == BASES::DEC ? "%lu" : ( _base == BASES::HEX ) ? "%lx" : "%lo", unsignedLongInteger_ );
	int long len( reformat() );
	do_write( _wordCache.raw(), len );
	return ( *this );
	M_EPILOG
}

HStreamInterface& HStreamInterface::do_output( int long long unsigned unsignedLongLongInteger_ ) {
	M_PROLOG
	_wordCache.format( _base == BASES::DEC ? "%llu" : ( _base == BASES::HEX ) ? "%llx" : "%llo", unsignedLongLongInteger_ );
	int long len( reformat() );
	do_write( _wordCache.raw(), len );
	return ( *this );
	M_EPILOG
}

void HStreamInterface::apply_precision( void ) {
	M_PROLOG
	int dot( static_cast<int>( _wordCache.find( '.' ) ) );
	if ( dot != HString::npos ) {
		int nonZero( static_cast<int>( _wordCache.find_last_other_than( "0" ) ) );
		if ( ( nonZero != HString::npos ) && ( nonZero >= dot  ) ) {
			int len( static_cast<int>( _wordCache.get_length() ) );
			int cap( max( _floatFormat == FLOAT_FORMAT::FIXED ? _precision + 1 : 0, nonZero + ( nonZero == dot ? 0 : 1 ) ) ); /* + 1 for a dot */
			if ( cap < len )
				_wordCache.set_at( cap, 0 );
		}
	}
	return;
	M_EPILOG
}

HStreamInterface& HStreamInterface::do_output( double double_ ) {
	M_PROLOG
	_wordCache.format( "%f", double_ );
	apply_precision();
	int long len( reformat() );
	do_write( _wordCache.raw(), len );
	return ( *this );
	M_EPILOG
}

HStreamInterface& HStreamInterface::do_output( double long longDouble_ ) {
	M_PROLOG
	_wordCache.format( "%.12Lf", longDouble_ );
	apply_precision();
	int long len( reformat() );
	do_write( _wordCache.raw(), len );
	return ( *this );
	M_EPILOG
}

HStreamInterface& HStreamInterface::do_output( float float_ ) {
	M_PROLOG
	_wordCache.format( "%f", float_ );
	apply_precision();
	int long len( reformat() );
	do_write( _wordCache.raw(), len );
	return ( *this );
	M_EPILOG
}

HStreamInterface& HStreamInterface::do_output( void const* ptr_ ) {
	M_PROLOG
	_wordCache.format( "0x%lx", reinterpret_cast<int long unsigned>( ptr_ ) );
	int long len( reformat() );
	do_write( _wordCache.raw(), len );
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

HStreamInterface& dec( HStreamInterface& iface_ ) {
	M_PROLOG
	return ( iface_.set_base( HStreamInterface::BASES::DEC ) );
	M_EPILOG
}

HStreamInterface& hex( HStreamInterface& iface_ ) {
	M_PROLOG
	return ( iface_.set_base( HStreamInterface::BASES::HEX ) );
	M_EPILOG
}

HStreamInterface& oct( HStreamInterface& iface_ ) {
	M_PROLOG
	return ( iface_.set_base( HStreamInterface::BASES::OCT ) );
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

HStreamInterface::HManipulator setfill( int fill_ ) {
	M_PROLOG
	return ( HStreamInterface::HManipulator( fill_, &HStreamInterface::HManipulator::set_fill ) );
	M_EPILOG
}

int long HStreamInterface::do_read_until( HString& message_,
		char const* const stopSet_, bool stripDelim_ ) {
	M_PROLOG
	return ( HStreamInterface::read_until_n( message_, meta::max_signed<int long>::value, stopSet_, stripDelim_ ) );
	M_EPILOG
}

int long HStreamInterface::do_read_until_n( HString& message_, int long maxCount_,
		char const* const stopSet_, bool stripDelim_ ) {
	M_PROLOG
	return ( semantic_read( message_, maxCount_, stopSet_, stripDelim_, true ) );
	M_EPILOG
}

int long HStreamInterface::do_read_while( HString& message_,
		char const* const acquire_, bool stripDelim_ ) {
	M_PROLOG
	return ( HStreamInterface::do_read_while_n( message_, meta::max_signed<int long>::value, acquire_, stripDelim_ ) );
	M_EPILOG
}

int long HStreamInterface::do_read_while_n( HString& message_, int long maxCount_,
		char const* const acquire_, bool stripDelim_ ) {
	M_PROLOG
	return ( semantic_read( message_, maxCount_, acquire_, stripDelim_, false ) );
	M_EPILOG
}

int long HStreamInterface::read_while_retry( yaal::hcore::HString& message_,
		char const* acquire_, bool stripDelim_ ) {
	M_PROLOG
	int long nRead( 0 );
	do {
		nRead = HStreamInterface::do_read_while( message_, acquire_, stripDelim_ );
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

int long HStreamInterface::semantic_read( yaal::hcore::HString& message_, int long maxCount_, char const* const set_, bool stripDelim_, bool isStopSet_ ) {
	M_PROLOG
	M_ASSERT( _offset >= 0 );
	int long nRead( 0 ); /* how many bytes were read in this single invocation */
	int long iPoolSize( _cache.size() );
	char* buffer( _cache.get<char>() ); /* read cache buffer */
	bool byDelim( false );
	int setLen( static_cast<int>( ::strlen( set_ ) + 1 ) ); /* + 1 for terminating \0 byte that also could be searched for */
	if ( _offset ) {
		int cached( 0 );
		for ( ; ( cached < _offset ) && ( cached < maxCount_ ); ++ cached ) {
			if ( ( byDelim = ( ::memchr( set_, buffer[ cached ], static_cast<size_t>( setLen ) ) ? isStopSet_ : ! isStopSet_ ) ) )
				break;
		}
		if ( ( cached == maxCount_ ) || byDelim ) {
			nRead = cached - ( ( ( cached > 0 ) && ! stripDelim_ ) ? 1 : 0 );
			message_.assign( buffer, nRead );
			if ( nRead ) {
				_offset -= static_cast<int>( nRead );
				::memmove( buffer, buffer + nRead, static_cast<size_t>( _offset ) );
			}
			if ( byDelim )
				++ nRead;
		}
	}
	if ( ! ( nRead || byDelim ) ) {
		do {
			/* Let's check if next read wont exceed size of our read buffer.
			 * 1 byte or terminating zero and one byte for at least one new character,
			 * hence + 2 in condition.
			 */
			if ( ( _offset + 2 ) > iPoolSize ) {
				_cache.realloc( _offset + 2 );
				buffer = _cache.get<char>(); /* update read cache buffer ptr, reallocation could move previous buffer into another memory position */
				iPoolSize = _cache.size();
			}
			/* We read only one byte at a time. */
			nRead = do_read( buffer + _offset, sizeof ( char ) * 1 );
			/* nRead can be one of the following:
			 * nRead > 0 - a successful read, we shall check for stop char and possibly continue reading.
			 * nRead == 0 - stream is blocking and has just been closed or has no data to read and is internally non-blocking.
			 * nRead < 0 - an error occured, read opration could be externally interrupted.
			 */
		} while ( ( nRead > 0 ) /* We increment _offset only if read succeeded. */
				&& ( ! ( byDelim = ( ::memchr( set_, buffer[ _offset ++ ], static_cast<size_t>( setLen ) ) ? isStopSet_ : ! isStopSet_ ) ) )
				&& ( ! ( _offset >= maxCount_ ) ) );
		if ( nRead >= 0 ) {
			if ( ! nRead ) {
				_valid = false;
			}
			nRead = _offset;
			message_.assign( buffer, _offset - ( ( ( _offset > 0 ) && byDelim ) ? 1 : 0 ) );
			if ( byDelim && ! stripDelim_ ) {
				M_ASSERT( _offset > 0 );
				/*
				 * WTF???
				 */
				buffer[ 0 ] = buffer[ _offset - 1 ];
				_offset = 1;
			} else
				_offset = 0;
		} else
			message_.clear();
	}
	return ( nRead );
	M_EPILOG
}

int HStreamInterface::do_peek( void ) {
	if ( ! _offset ) {
		int long iPoolSize( _cache.size() );
		if ( iPoolSize < 1 )
			_cache.realloc( 1 );
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

HStreamInterface& HStreamInterface::do_input( HString& word ) {
	M_PROLOG
	if ( read_word() )
		word = _wordCache;
	return ( *this );
	M_EPILOG
}

bool HStreamInterface::read_word( void ) {
	M_PROLOG
	/* Regarding _whiteSpace_.size() + 1, about "+ 1" see comment in semantic_read in analoguous context */
	if ( ! _skipWS ) {
		int peeked( HStreamInterface::do_peek() );
		if ( ( peeked == INVALID_CHARACTER )
				|| ( ::memchr( _whiteSpace_.data(), peeked, static_cast<size_t>( _whiteSpace_.size() + 1 ) ) ) ) {
			_fail = true;
		}
	}
	if ( good() ) {
		read_while_retry( _wordCache, _whiteSpace_.data(), false );
		if ( _skipWS ) {
			_wordCache.clear();
		}
		if ( good() ) {
			read_until_retry( _wordCache, _whiteSpace_.data(), false );
		}
	}
	return ( _wordCache.get_length() > 0 );
	M_EPILOG
}

bool HStreamInterface::read_integer( void ) {
	M_PROLOG
	read_while_retry( _wordCache, _whiteSpace_.data(), false );
	_wordCache.clear();
	if ( good() ) {
		bool neg( HStreamInterface::do_peek() == '-' );
		if ( neg ) {
			char sink( 0 );
			HStreamInterface::read( &sink, 1 );
		}
		if ( good() ) {
			read_while_retry( _wordCache, _digit_.data(), false );
			if ( neg ) {
				_wordCache.insert( 0, "-" );
			}
		}
	}
	return ( _wordCache.get_length() > 0 );
	M_EPILOG
}

bool HStreamInterface::read_floatint_point( void ) {
	M_PROLOG
	do {
		read_while_retry( _wordCache, _whiteSpace_.data(), false );
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
		read_while_retry( _wordCache, _digit_.data(), false );
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
			read_while_retry( decimal, _digit_.data(), false );
			if ( ! decimal.is_empty() ) {
				_wordCache += sink;
				_wordCache += decimal;
			}
		}
	} while ( false );
	return ( _wordCache.get_length() > 0 );
	M_EPILOG
}

HStreamInterface& HStreamInterface::do_input( bool& b ) {
	M_PROLOG
	if ( read_word() )
		b = lexical_cast<bool>( _wordCache );
	return ( *this );
	M_EPILOG
}

HStreamInterface& HStreamInterface::do_input( char& char_ ) {
	M_PROLOG
	/* Regarding _whiteSpace_.size() + 1, about "+ 1" see comment in semantic_read in analoguous context */
	char c( 0 );
	do {
		read( &c, 1 );
	} while ( good() && _skipWS && ::memchr( _whiteSpace_.data(), c, static_cast<size_t>( _whiteSpace_.size() + 1 ) ) );
	char_ = c;
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
	if ( read_integer() )
		is = lexical_cast<int short>( _wordCache );
	return ( *this );
	M_EPILOG
}

HStreamInterface& HStreamInterface::do_input( int short unsigned& isu ) {
	M_PROLOG
	if ( read_integer() )
		isu = lexical_cast<int short unsigned>( _wordCache );
	return ( *this );
	M_EPILOG
}

HStreamInterface& HStreamInterface::do_input( int& i ) {
	M_PROLOG
	if ( read_integer() )
		i = lexical_cast<int>( _wordCache );
	return ( *this );
	M_EPILOG
}

HStreamInterface& HStreamInterface::do_input( int unsigned& iu ) {
	M_PROLOG
	if ( read_integer() ) {
		iu = lexical_cast<int unsigned>( _wordCache );
	}
	return ( *this );
	M_EPILOG
}

HStreamInterface& HStreamInterface::do_input( int long& il ) {
	M_PROLOG
	if ( read_integer() ) {
		il = lexical_cast<int long>( _wordCache );
	}
	return ( *this );
	M_EPILOG
}

HStreamInterface& HStreamInterface::do_input( int long unsigned& ilu ) {
	M_PROLOG
	if ( read_integer() ) {
		ilu = lexical_cast<int long unsigned>( _wordCache );
	}
	return ( *this );
	M_EPILOG
}

HStreamInterface& HStreamInterface::do_input( int long long& ill ) {
	M_PROLOG
	if ( read_integer() )
		ill = lexical_cast<int long long>( _wordCache );
	return ( *this );
	M_EPILOG
}

HStreamInterface& HStreamInterface::do_input( int long long unsigned& illu ) {
	M_PROLOG
	if ( read_integer() )
		illu = lexical_cast<int long long unsigned>( _wordCache );
	return ( *this );
	M_EPILOG
}

HStreamInterface& HStreamInterface::do_input( double& d ) {
	M_PROLOG
	if ( read_floatint_point() )
		d = lexical_cast<double>( _wordCache );
	return ( *this );
	M_EPILOG
}

HStreamInterface& HStreamInterface::do_input( double long& dl ) {
	M_PROLOG
	if ( read_floatint_point() )
		dl = lexical_cast<double long>( _wordCache );
	return ( *this );
	M_EPILOG
}

HStreamInterface& HStreamInterface::do_input( float& f ) {
	M_PROLOG
	if ( read_floatint_point() )
		f = lexical_cast<float>( _wordCache );
	return ( *this );
	M_EPILOG
}

HStreamInterface& HStreamInterface::do_input( void const*& ) {
	M_PROLOG
	return ( *this );
	M_EPILOG
}

HStreamInterface& HStreamInterface::do_input( manipulator_t const& HFILE_INTERFACE ) {
	M_PROLOG
	return ( HFILE_INTERFACE( *this ) );
	M_EPILOG
}

int long HStreamInterface::read( void* const buffer_, int long size_ ) {
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
		if ( physRead >= 0 )
			nRead += physRead;
		else if ( ! nRead )
			nRead = physRead;
		if ( ! physRead )
			_valid = false;
	}
	return ( nRead );
	M_EPILOG
}

int long HStreamInterface::write( void const* const buffer_, int long size_ ) {
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

void HStreamInterface::flush( void ) {
	M_PROLOG
	do_flush();
	return;
	M_EPILOG
}

HStreamInterface& HStreamInterface::do_set_fill( int fill_ ) {
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

HStreamInterface& HStreamInterface::do_set_base( BASES::enum_t base_ ) {
	M_PROLOG
	_base = base_;
	return ( *this );
	M_EPILOG
}

HStreamInterface& HStreamInterface::do_set_float_format( FLOAT_FORMAT::enum_t floatFormat_ ) {
	M_PROLOG
	_floatFormat = floatFormat_;
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

int HStreamInterface::do_get_fill( void ) const {
	return ( _fill );
}

int HStreamInterface::do_get_width( void ) const {
	return ( _width );
}

int HStreamInterface::do_get_precision( void ) const {
	return ( _precision );
}

HStreamInterface::BASES::enum_t HStreamInterface::do_get_base( void ) const {
	return ( _base );
}

HStreamInterface::FLOAT_FORMAT::enum_t HStreamInterface::do_get_float_format( void ) const {
	return ( _floatFormat );
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
	iface_.set_fill( _value );
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

}

}

