/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "escape.hxx"
#include "hcore/hexception.hxx"
#include "hcore/htls.hxx"
#include "hcore/unicode.hxx"
#include "hcore/hhashmap.hxx"

using namespace yaal;
using namespace yaal::hcore;

namespace yaal {

namespace tools {

namespace util {

EscapeSet::EscapeSet( char const* data_, int size_ )
	: _data() {
	M_PROLOG
	M_ASSERT( ( size_ > 0 ) && data_ );
	for ( int i( 0 ); i < EscapeSet::ESCAPE_SET_SIZE; ++ i ) {
		_data[i] = 0;
	}
	for ( int i( 0 ); i < size_; ++ i ) {
		_data[static_cast<int>( data_[i] )] = 1;
	}
	return;
	M_EPILOG
}

bool EscapeSet::has( int ch_ ) const {
	return ( ch_ < ESCAPE_SET_SIZE ? !! _data[ch_] : false );
}

void escape( yaal::hcore::HString& string_, EscapeSet const& es_, code_point_t escape_ ) {
	M_PROLOG
	if ( string_.is_empty() ) {
		return;
	}
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
		cache_update( pos + 1 );
		if ( es_.has( static_cast<int>( (*it).get() ) ) ) {
			ptr[pos ++] = escape_;
			cache_update( pos + 1 );
		}
		ptr[ pos ] = *it;
	}
	string_.clear();
	for ( int long i( 0 ); i < pos; ++ i ) {
		string_.push_back( ptr[i] );
	}
	return;
	M_EPILOG
}

void unescape( yaal::hcore::HString& string_, yaal::code_point_t esc_ ) {
	M_PROLOG
	if ( string_.is_empty() ) {
		return;
	}
	typedef HTLS<HChunk> cache_t;
	static cache_t _cache_;
	HChunk& cache( *_cache_ );
	cache.realloc( chunk_size<code_point_t>( string_.get_length() ) );
	code_point_t* ptr( cache.get<code_point_t>() );
	bool escape( false );
	int long pos( 0 );
	for ( code_point_t cp : string_ ) {
		if ( ! escape && ( cp == esc_ ) ) {
			escape = true;
			continue;
		}
		escape = false;
		ptr[pos] = cp;
		++ pos;
	}
	string_.clear();
	for ( int long i( 0 ); i < pos; ++ i ) {
		string_.push_back( ptr[i] );
	}
	return;
	M_EPILOG
}

EscapeTable::EscapeTable( char const* raw_, int rawLen_, char const* safe_, int M_DEBUG_CODE( safeLen_ ) )
	: _rawToSafe()
	, _safeToRaw() {
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

EscapeTable const& cxx_escape_table( void ) {
	static char const raw[] = "\n\r\t\b\a\f\033\v";
	static char const safe[] = "nrtbafev";
	static EscapeTable const escapes(
		raw,  static_cast<int>( sizeof ( raw ) )  - 1,
		safe, static_cast<int>( sizeof ( safe ) ) - 1
	);
	return escapes;
}

void escape( yaal::hcore::HString& string_, EscapeTable const& et_, code_point_t escape_ ) {
	M_PROLOG
	if ( string_.is_empty() ) {
		return;
	}
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
			*it < EscapeTable::ESCAPE_TABLE_SIZE
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
	return;
	M_EPILOG
}

void unescape( yaal::hcore::HString& string_, EscapeTable const& et_, code_point_t escape_, bool dropUnmatched_ ) {
	M_PROLOG
	if ( string_.is_empty() ) {
		return;
	}
	typedef HTLS<HChunk> cache_t;
	static cache_t _cache_;
	HChunk& cache( *_cache_ );
	cache.realloc( chunk_size<code_point_t>( string_.get_length() ) );
	code_point_t* ptr( cache.get<code_point_t>() );
	int long pos( 0 );
	bool escape( false );
	for ( code_point_t cp : string_ ) {
		if ( ! escape && ( cp == escape_ ) ) {
			escape = true;
			continue;
		}
		if ( ! escape ) {
			ptr[pos] = cp;
			++ pos;
			continue;
		}
		escape = false;
		code_point_t translated(
			cp < EscapeTable::ESCAPE_TABLE_SIZE
				? code_point_t( static_cast<char unsigned>( et_._safeToRaw[static_cast<char unsigned>( cp.get() )] ) )
				: cp
		);
		if ( ( translated == cp ) && ! dropUnmatched_ ) {
			ptr[pos] = escape_;
			++ pos;
		}
		ptr[pos] = translated;
		++ pos;
	}
	string_.clear();
	for ( int long i( 0 ); i < pos; ++ i ) {
		string_.push_back( ptr[i] );
	}
	return;
	M_EPILOG
}

void semantic_unescape( yaal::hcore::HString& str_ ) {
	M_PROLOG
	for ( int i( 0 ); i < str_.get_length(); ++ i ) {
		int k( i );
		if ( ( str_[k] != '\\' ) || ( ( k + 1 ) >= str_.get_length() ) ) {
			continue;
		}
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
	return;
	M_EPILOG
}

HString escape_copy( yaal::hcore::HString string_, EscapeTable const& et_, code_point_t escape_ ) {
	M_PROLOG
	escape( string_, et_, escape_ );
	return string_;
	M_EPILOG
}

HString unescape_copy( yaal::hcore::HString string_, EscapeTable const& et_, code_point_t escape_ ) {
	M_PROLOG
	unescape( string_, et_, escape_ );
	return string_;
	M_EPILOG
}

void mask_escape( yaal::hcore::HString& string_, escape_mask_map_t& escapeMaskMap_, yaal::code_point_t escape_ ) {
	M_PROLOG
	if ( string_.is_empty() ) {
		return;
	}
	typedef HHashMap<code_point_t, escape_mask_map_t::size_type> mask_map_t;
	mask_map_t maskMap;
	typedef HTLS<HChunk> cache_t;
	static cache_t _cache_;
	HChunk& cache( *_cache_ );
	cache.realloc( chunk_size<code_point_t>( string_.get_length() ) );
	code_point_t* ptr( cache.get<code_point_t>() );
	bool escaped( false );
	for ( code_point_t c : string_ ) {
		if ( escaped ) {
			escaped = false;
			mask_map_t::insert_result insertResult( maskMap.insert( make_pair( c, escapeMaskMap_.get_size() ) ) );
			if ( insertResult.second ) {
				escapeMaskMap_.push_back( c );
			}
			c = code_point_t( unicode::CODE_POINT::SUPPLEMENTARY_PRIVATE_USE_AREA_B.get() + static_cast<code_point_t::value_type>( insertResult.first->second ) );
		} else if ( c == escape_ ) {
			escaped = true;
		}
		*ptr = c;
		++ ptr;
	}
	string_.assign( cache.get<char32_t>(), string_.get_length() );
	return;
	M_EPILOG
}

template<typename iterator_t, typename U>
inline void copy_n_cast( iterator_t src_, HString::size_type size_, U* dst_ ) {
	for ( HString::size_type i( 0 ); i < size_; ++ i, ++ src_, ++ dst_ ) {
		*dst_ = static_cast<U>( *src_ );
	}
	return;
}

void unmask_escape( yaal::hcore::HString& string_, escape_mask_map_t const& escapeMaskMap_, yaal::code_point_t escape_ ) {
	M_PROLOG
	if ( string_.is_empty() ) {
		return;
	}
	typedef HHashMap<code_point_t, escape_mask_map_t::size_type> mask_map_t;
	mask_map_t maskMap;
	typedef HTLS<HChunk> cache_t;
	static cache_t _cache_;
	HChunk& cache( *_cache_ );
	cache.realloc( chunk_size<code_point_t>( string_.get_length() ) );
	code_point_t* ptr( cache.get<code_point_t>() );
	bool escaped( false );
	int rank( 1 );
	for ( code_point_t c : string_ ) {
		if ( escaped && is_pua<CHARACTER_CLASS::SUPPLEMENTARY_PRIVATE_USE_AREA_B>( c ) ) {
			escaped = false;
			mask_map_t::size_type idx( static_cast<mask_map_t::size_type>( c.get() ) - static_cast<mask_map_t::size_type>( unicode::CODE_POINT::SUPPLEMENTARY_PRIVATE_USE_AREA_B.get() ) );
			if ( ( idx >= 0 ) && ( idx < escapeMaskMap_.get_size() ) ) {
				c = escapeMaskMap_[idx];
			}
		} else if ( escaped ) {
			escaped = false;
		} else if ( c == escape_ ) {
			escaped = true;
		}
		rank = max( unicode::rank( c ), rank );
		*ptr = c;
		++ ptr;
	}
	switch ( rank ) {
		case ( 1 ): {
			copy_n_cast( cache.get<code_point_t::value_type>(), string_.get_length(), cache.get<u8_t>() );
			string_.assign( cache.raw(), string_.get_length() );
		} break;
		case ( 2 ): {
			copy_n_cast( cache.get<code_point_t::value_type>(), string_.get_length(), cache.get<u16_t>() );
			string_.assign( cache.get<char16_t>(), string_.get_length() );
		} break;
		case ( 4 ): {
			string_.assign( cache.get<char32_t>(), string_.get_length() );
		} break;
	}
	return;
	M_EPILOG
}

namespace {

yaal::hcore::HString make_shroud( yaal::hcore::HString const& pattern_ ) {
	M_PROLOG
	HString shroud;
	for ( code_point_t c : pattern_ ) {
		shroud.push_back( code_point_t( c.get() + unicode::CODE_POINT::SUPPLEMENTARY_PRIVATE_USE_AREA_A ) );
	}
	return shroud;
	M_EPILOG
}

}

void hide( yaal::hcore::HString& string_, yaal::hcore::HString const& pattern_ ) {
	M_PROLOG
	util::escape_mask_map_t emm;
	mask_escape( string_, emm );
	string_.replace( pattern_, make_shroud( pattern_ ) );
	unmask_escape( string_, emm );
	return;
	M_EPILOG
}

void unhide( yaal::hcore::HString& string_, yaal::hcore::HString const& pattern_ ) {
	M_PROLOG
	string_.replace( make_shroud( pattern_ ), pattern_ );
	return;
	M_EPILOG
}

}

}

}

