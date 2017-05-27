/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

  escape.cxx - this file is integral part of `yaal' project.

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
		cache.realloc( chunk_size<code_point_t>( string_.get_length() ) );
		int cacheSize( static_cast<int>( cache.get_size() ) / CODE_POINT_SIZE );
		int pos( 0 );
		code_point_t* ptr( cache.get<code_point_t>() );
		for ( HString::const_iterator it( string_.begin() ), end( string_.end() ); it != end; ++ it, ++ pos ) {
			code_point_t ch( unicode::rank( *it ) == 1 ? code_point_t( static_cast<yaal::u32_t>( et_._rawToSafe[static_cast<char unsigned>( (*it).get() )] ) ) : *it );
			if ( ( pos + 1 ) >= cacheSize ) {
				cache.realloc( chunk_size<code_point_t>( cacheSize * 2 ) );
				cacheSize = static_cast<int>( cache.get_size() ) / CODE_POINT_SIZE;
				ptr = cache.get<code_point_t>();
			}
			if ( ch != *it ) {
				ptr[pos ++] = escape_;
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
				ptr[pos] = unicode::rank( *it ) == 1 ? code_point_t(static_cast<yaal::u32_t>( et_._safeToRaw[static_cast<char unsigned>( (*it).get() )] ) ) : *it;
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

