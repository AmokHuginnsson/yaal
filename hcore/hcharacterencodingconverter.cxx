/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

  hcharacterencodingconverter.cxx - this file is integral part of `yaal' project.

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
#include <iconv.h>

#include "hcore/base.hxx"
#include "hcore/algorithm_low.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "hcharacterencodingconverter.hxx"

namespace yaal {

namespace hcore {

static HCharacterEncodingConverter::descriptor_t const INVALID( static_cast<HCharacterEncodingConverter::descriptor_t>( -1 ) );

HCharacterEncodingConverter::HCharacterEncodingConverter( yaal::hcore::HString const& nameFrom_, yaal::hcore::HString const& nameTo_ )
	: _nameFrom( nameFrom_ )
	, _nameTo( nameTo_ )
	, _descriptor( reinterpret_cast<descriptor_t>( iconv_open( _nameTo.c_str(), _nameFrom.c_str() ) ) )
	, _cache() {
	M_PROLOG
	M_ENSURE( _descriptor != INVALID, "iconv_open" );
	return;
	M_EPILOG
}

HCharacterEncodingConverter::HCharacterEncodingConverter( HCharacterEncodingConverter const& other_ )
	: _nameFrom( other_._nameFrom )
	, _nameTo( other_._nameTo )
	, _descriptor( reinterpret_cast<descriptor_t>( iconv_open( _nameTo.c_str(), _nameFrom.c_str() ) ) )
	, _cache() {
	M_PROLOG
	M_ENSURE( _descriptor != INVALID, "iconv_open" );
	return;
	M_EPILOG
}

HCharacterEncodingConverter::HCharacterEncodingConverter( HCharacterEncodingConverter&& other_ )
	: _nameFrom( yaal::move( other_._nameFrom ) )
	, _nameTo( yaal::move( other_._nameTo ) )
	, _descriptor( other_._descriptor )
	, _cache( yaal::move( other_._cache ) ) {
	M_PROLOG
	other_._descriptor = INVALID;
	return;
	M_EPILOG
}

HCharacterEncodingConverter& HCharacterEncodingConverter::operator = ( HCharacterEncodingConverter const& other_ ) {
	M_PROLOG
	if ( &other_ != this ) {
		HCharacterEncodingConverter tmp( other_ );
		swap( tmp );
	}
	return ( *this );
	M_EPILOG
}

HCharacterEncodingConverter& HCharacterEncodingConverter::operator = ( HCharacterEncodingConverter&& other_ ) {
	M_PROLOG
	if ( &other_ != this ) {
		swap( other_ );
		other_.reset();
	}
	return ( *this );
	M_EPILOG
}

HCharacterEncodingConverter::~HCharacterEncodingConverter( void ) {
	M_PROLOG
	reset();
	return;
	M_DESTRUCTOR_EPILOG
}

void HCharacterEncodingConverter::swap( HCharacterEncodingConverter& other_ ) {
	M_PROLOG
	if ( &other_ != this ) {
		using yaal::swap;
		swap( _nameFrom, other_._nameFrom );
		swap( _nameTo, other_._nameTo );
		swap( _descriptor, other_._descriptor );
		swap( _cache, other_._cache );
	}
	return;
	M_EPILOG
}

void HCharacterEncodingConverter::reset( void ) {
	M_PROLOG
	if ( _descriptor != INVALID ) {
		M_ENSURE( iconv_close( reinterpret_cast<iconv_t>( _descriptor ) ) != -1, "iconv_close" );
		_descriptor = INVALID;
	}
	return;
	M_EPILOG
}

yaal::hcore::HString const& HCharacterEncodingConverter::name_from( void ) const {
	return ( _nameFrom );
}

yaal::hcore::HString const& HCharacterEncodingConverter::name_to( void ) const {
	return ( _nameTo );
}

#ifdef HAVE_ICONV_INPUT_CONST
#	define M_YAAL_ICONV_CONST const
#else /* HAVE_ICONV_INPUT_CONST */
#	define M_YAAL_ICONV_CONST /**/
#endif /* not HAVE_ICONV_INPUT_CONST */

yaal::hcore::HString const& HCharacterEncodingConverter::convert( char const* data_, int long size_ ) {
	M_PROLOG
	char M_YAAL_ICONV_CONST* source( const_cast<char M_YAAL_ICONV_CONST*>( data_ ) );
	iconv_t cD( reinterpret_cast<iconv_t>( _descriptor ) );
	size_t sizeIn( static_cast<size_t>( size_ ) );
	size_t sizeOut( 0 );
	/* The longest single character in any encoding is 6 bytes long. */
	size_t const ICONV_OUTPUT_BUFFER_LENGTH = 8;
	/* Additional character for nil terminator. */
	char output[ ICONV_OUTPUT_BUFFER_LENGTH + 1 ];
	_cache.clear();
	do {
		::memset( output, 0, ICONV_OUTPUT_BUFFER_LENGTH + 1 );
		sizeOut = ICONV_OUTPUT_BUFFER_LENGTH;
		char* out = output;
		M_ENSURE(
			( ::iconv( cD, &source, &sizeIn, &out, &sizeOut ) != static_cast<size_t>( -1 ) ) || ( errno == E2BIG ),
			"iconv"
		);
		_cache += output;
	} while ( sizeIn );
	return ( _cache );
	M_EPILOG
}

#undef M_YAAL_ICONV_CONST

yaal::hcore::HString const& HCharacterEncodingConverter::convert( char const* data_ ) {
	M_PROLOG
	return ( convert( data_, static_cast<int>( ::strlen( data_ ) ) ) );
	M_EPILOG
}

yaal::hcore::HString const& HCharacterEncodingConverter::convert( yaal::hcore::HString const& data_ ) {
	M_PROLOG
	return ( convert( data_.raw(), data_.get_length() ) );
	M_EPILOG
}

}

}

