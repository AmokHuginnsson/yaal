/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "escape.hxx"
#include "hcore/hexception.hxx"
#include "hcore/htls.hxx"
#include "hcore/unicode.hxx"

using namespace yaal;
using namespace yaal::hcore;

namespace yaal {

namespace tools {

namespace util {

EscapeTable::EscapeTable( char const* raw_, int rawLen_, char const* safe_, int M_DEBUG_CODE( safeLen_ ) )
	: _rawToSafe(), _safeToRaw() {
	M_PROLOG
	M_ASSERT( ( rawLen_ > 0 ) && ( safeLen_ == rawLen_ ) && raw_ && safe_ );
	for ( int i( 0 ); i < EscapeTable::ESCAPE_TABLE_SIZE; ++ i ) {
		_rawToSafe[i] = _safeToRaw[i] = static_cast<char>( i );
	}
	for ( int i( 0 ); i < rawLen_; ++ i ) {
		_rawToSafe[static_cast<char unsigned>( raw_[i] )] = safe_[i];
		_safeToRaw[static_cast<char unsigned>( safe_[i] )] = raw_[i];
	}
	return;
	M_EPILOG
}

void escape( yaal::hcore::HString& string_, EscapeTable const& et_, code_point_t escape_ ) {
	M_PROLOG
	if ( ! string_.is_empty() ) {
		typedef HTLS<HChunk> cache_t;
		static cache_t _cache_;
		HChunk& cache( *_cache_ );
		static int const CODE_POINT_SIZE( static_cast<int>( sizeof ( code_point_t ) ) );
		int cacheSize( 0 );
		code_point_t* ptr( nullptr );
		auto cache_update = [&cache, &cacheSize, &ptr]( int size_ ) {
			if ( size_ > cacheSize ) {
				cache.realloc( chunk_size<code_point_t>( max( cacheSize * 2, size_ ) ) );
				cacheSize = static_cast<int>( cache.get_size() ) / CODE_POINT_SIZE;
				ptr = cache.get<code_point_t>();
			}
		};
		cache_update( static_cast<int>( string_.get_length() ) );
		int pos( 0 );
		for ( HString::const_iterator it( string_.begin() ), end( string_.end() ); it != end; ++ it, ++ pos ) {
			code_point_t ch(
				unicode::rank( *it ) == 1
				? code_point_t(
						static_cast<char unsigned>(
							et_._rawToSafe[static_cast<char unsigned>( (*it).get() )]
						)
					)
				: *it
			);
			cache_update( pos + 1 );
			if ( ch != *it ) {
				ptr[pos ++] = escape_;
				cache_update( pos + 1 );
			}
			ptr[ pos ] = ch;
		}
		string_.clear();
		for ( int long i( 0 ); i < pos; ++ i ) {
			string_.push_back( ptr[i] );
		}
	}
	return;
	M_EPILOG
}

void unescape( yaal::hcore::HString& string_, EscapeTable const& et_, code_point_t escape_ ) {
	M_PROLOG
	if ( ! string_.is_empty() ) {
		typedef HTLS<HChunk> cache_t;
		static cache_t _cache_;
		HChunk& cache( *_cache_ );
		cache.realloc( chunk_size<code_point_t>( string_.get_length() ) );
		int pos( 0 );
		code_point_t* ptr( cache.get<code_point_t>() );
		for ( HString::const_iterator it( string_.begin() ), end( string_.end() ); it != end; ++ it, ++ pos ) {
			if ( *it == escape_ ) {
				++ it;
				if ( ! ( it != end ) ) {
					break;
				}
				ptr[pos] = unicode::rank( *it ) == 1
					? code_point_t(
							static_cast<char unsigned>( et_._safeToRaw[static_cast<char unsigned>( (*it).get() )] )
						)
					: *it;
			} else {
				ptr[pos] = *it;
			}
		}
		string_.clear();
		for ( int long i( 0 ); i < pos; ++ i ) {
			string_.push_back( ptr[i] );
		}
	}
	return;
	M_EPILOG
}

void semantic_unescape( yaal::hcore::HString& str_ ) {
	M_PROLOG
	for ( int i( 0 ); i < str_.get_length(); ++ i ) {
		int k( i );
		if ( ( str_[k] == '\\' ) && ( ( k + 1 ) < str_.get_length() ) ) {
			++ k;
			if ( str_[k] == '\\' ) {
				++ i;
				continue;
			}
			code_point_t prefix( str_[k] );
			int codeLen( 0 );
			switch ( prefix.get() ) {
				case ( 'x' ): { codeLen = 2; } break;
				case ( 'u' ): { codeLen = 4; } break;
				case ( 'U' ): { codeLen = 8; } break;
			}
			int base( 16 );
			bool good( false );
			char num[10];
			if ( codeLen > 0 ) {
				if ( ( k + codeLen ) < str_.get_length() ) {
					good = true;
					++ k;
					for ( int n( 0 ); n < codeLen; ++ n ) {
						code_point_t d( str_[k + n] );
						if ( ! is_hex_digit( d ) ) {
							good = false;
							break;
						}
						num[n] = static_cast<char>( d.get() );
					}
				}
			} else {
				base = 8;
				for ( int len( min( 3, static_cast<int>( str_.get_length() ) - k ) ); codeLen < len; ++ codeLen ) {
					code_point_t d( str_[k + codeLen] );
					if ( ! is_oct_digit( d ) ) {
						break;
					}
					num[codeLen] = static_cast<char>( d.get() );
					good = true;
				}
			}
			if ( good ) {
				num[codeLen] = 0;
				code_point_t c( static_cast<u32_t>( stoul( num, nullptr, base ) ) );
				if ( ( c != 0_ycp ) && ( c < unicode::UCS_MAX_4_BYTE_CODE_POINT ) ) {
					str_.replace( i, ( base == 16 ? 2 : 1 ) + codeLen, 1, c );
				}
			}
		}
	}
	return;
	M_EPILOG
}

HString escape_copy( yaal::hcore::HString string_, EscapeTable const& et_, code_point_t escape_ ) {
	M_PROLOG
	escape( string_, et_, escape_ );
	return ( string_ );
	M_EPILOG
}

HString unescape_copy( yaal::hcore::HString string_, EscapeTable const& et_, code_point_t escape_ ) {
	M_PROLOG
	unescape( string_, et_, escape_ );
	return ( string_ );
	M_EPILOG
}

}

}

}

