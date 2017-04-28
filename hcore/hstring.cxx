/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

  hstring.cxx - this file is integral part of `yaal' project.

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
#include <cstdlib>
#include <climits>
#include <cctype>
#include <cstdarg>
#include <cstdio>
#include <libintl.h>

#include "base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "algorithm.hxx"
#include "hstring.hxx"
#include "memory.hxx"
#include "hchunk.hxx"
#include "hcore.hxx"
#include "safe_int.hxx"
#include "utf8.hxx"

namespace yaal {

namespace hcore {

namespace string_helper {

enum {
	OK = 0,
	NULL_PTR,
	UNINITIALIZED,
	INDEX_OOB,
	BAD_LENGTH,
	BAD_OFFSET,
	OVERFLOW
};

/*! \brief HString class helpers, utility functions.
 */
char* strrnpbrk( char const*, char const*, int long );
int long strrnspn( char const*, char const*, int long );
int long kmpsearch( char const*, int long, char const*, int long );
int long kmpcasesearch( char const*, int long, char const*, int long );

}

bool HCharacterClass::has( u32_t char_ ) const {
	return ( ( char_ < 256 ) && ( ::memchr( _data, static_cast<u8_t>( char_ ), static_cast<size_t>( _size ) ) != nullptr ) );
}

#undef D_WHITE_SPACE
#define D_WHITE_SPACE "\a\b \t\v\f\r\n"
HCharacterClass const _whiteSpace_ = HCharacterClass( D_WHITE_SPACE, static_cast<int>( sizeof ( D_WHITE_SPACE ) - 1 ) );
#undef D_WHITE_SPACE
#undef D_LOWER_CASE_LETTER
#undef D_UPPER_CASE_LETTER
#undef D_LETTER
#define D_LOWER_CASE_LETTER "abcdefghijklmnopqrstuvwxyz"
#define D_UPPER_CASE_LETTER "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
#define D_VOWEL "aAeEiIoOuUyY"
#define D_LETTER D_LOWER_CASE_LETTER D_UPPER_CASE_LETTER
#undef D_BIN_DIGIT
#define D_BIN_DIGIT "01"
#undef D_OCT_DIGIT
#define D_OCT_DIGIT D_BIN_DIGIT "234567"
#undef D_DEC_DIGIT
#define D_DEC_DIGIT D_OCT_DIGIT "89"
#undef D_HEX_DIGIT
#define D_HEX_DIGIT D_DEC_DIGIT "aAbBcCdDeEfF"
HCharacterClass const _binDigit_ = HCharacterClass( D_BIN_DIGIT, static_cast<int>( sizeof ( D_BIN_DIGIT ) - 1 ) );
HCharacterClass const _octDigit_ = HCharacterClass( D_OCT_DIGIT, static_cast<int>( sizeof ( D_OCT_DIGIT ) - 1 ) );
HCharacterClass const _digit_ = HCharacterClass( D_DEC_DIGIT, static_cast<int>( sizeof ( D_DEC_DIGIT ) - 1 ) );
HCharacterClass const _hexDigit_ = HCharacterClass( D_HEX_DIGIT, static_cast<int>( sizeof ( D_HEX_DIGIT ) - 1 ) );
HCharacterClass const _letter_ = HCharacterClass( D_LETTER, static_cast<int>( sizeof ( D_LETTER ) - 1 ) );
HCharacterClass const _lowerCaseLetter_ = HCharacterClass( D_LOWER_CASE_LETTER, static_cast<int>( sizeof ( D_LOWER_CASE_LETTER ) - 1 ) );
HCharacterClass const _upperCaseLetter_ = HCharacterClass( D_UPPER_CASE_LETTER, static_cast<int>( sizeof ( D_UPPER_CASE_LETTER ) - 1 ) );
HCharacterClass const _word_ = HCharacterClass( D_LETTER D_DEC_DIGIT "_", static_cast<int>( sizeof ( D_LETTER D_DEC_DIGIT "_" ) - 1 ) );
HCharacterClass const _vowel_ = HCharacterClass( D_VOWEL, static_cast<int>( sizeof ( D_VOWEL ) - 1 ) );
#undef D_HEX_DIGIT
#undef D_DEC_DIGIT
#undef D_OCT_DIGIT
#undef D_BIN_DIGIT
#undef D_LETTER
#undef D_VOWEL
#undef D_LOWER_CASE_LETTER
#undef D_UPPER_CASE_LETTER

HCharacterClass const* _characterClass_[] = { &_whiteSpace_, &_digit_, &_letter_, &_lowerCaseLetter_, &_upperCaseLetter_, &_word_, &_vowel_ };

static int const ALLOC_BIT_MASK = 128;
#undef IS_INPLACE
#define IS_INPLACE ( ! ( _mem[ ALLOC_FLAG_INDEX ] & ALLOC_BIT_MASK ) )
#undef EXT_IS_INPLACE
#define EXT_IS_INPLACE( base ) ( ! ( base._mem[ ALLOC_FLAG_INDEX ] & ALLOC_BIT_MASK ) )
#undef MEM
#define MEM ( IS_INPLACE ? _mem : _ptr )
#undef EXT_MEM
#define EXT_MEM( base ) ( EXT_IS_INPLACE( base ) ? base._mem : base._ptr )
#undef GET_SIZE
#define GET_SIZE ( IS_INPLACE ? _mem[ ALLOC_FLAG_INDEX ] : static_cast<int long>( _len[ 1 ] ) )
#undef SET_SIZE
#define SET_SIZE( size ) \
	do { \
		( IS_INPLACE ? _mem[ ALLOC_FLAG_INDEX ] = static_cast<char>( size ) : _len[ 1 ] = ( size ) ); \
	} while ( 0 )
#undef EXT_SET_SIZE
#define EXT_SET_SIZE( base, size ) \
	do { \
		( EXT_IS_INPLACE( base ) ? base._mem[ ALLOC_FLAG_INDEX ] = static_cast<char>( size ) : base._len[ 1 ] = ( size ) ); \
	} while ( 0 )
#undef GET_ALLOC_BYTES
#define GET_ALLOC_BYTES ( IS_INPLACE ? MAX_INPLACE_CAPACITY + 1 : _len[ 2 ] & static_cast<int long>( static_cast<int long unsigned>( -1 ) >> 1 ) )
#undef SET_ALLOC_BYTES
#define SET_ALLOC_BYTES( capacity ) \
	do { \
		_len[ 2 ] = ( capacity ); _mem[ ALLOC_FLAG_INDEX ] = static_cast<char>( _mem[ ALLOC_FLAG_INDEX ] | ALLOC_BIT_MASK ); \
	} while ( 0 )

char const* _errMsgHString_[ 7 ] = {
	_( "ok" ),
	_( "nullptr pointer used for string operations" ),
	_( "use of uninitialized string" ),
	_( "index out of bound" ),
	_( "bad length" ),
	_( "bad offset" ),
	_( "overflow" )
};

#if __GCC_VERSION_LOWER_OR_EQUAL__ <= 4005002
# pragma GCC diagnostic ignored "-Weffc++"
#endif /* #if __GCC_VERSION_LOWER_OR_EQUAL__ <= 4005002 */

HString::HString( void )
	: _len() {
	M_PROLOG
	return;
	M_EPILOG
}

HString::~HString( void ) {
	M_PROLOG
	if ( ! IS_INPLACE ) {
		memory::free( _ptr );
	}
	return;
	M_DESTRUCTOR_EPILOG
}

HString::HString( HString const& string_ )
	: _len() {
	M_PROLOG
	if ( ! string_.is_empty() ) {
		int long newSize( string_.get_length() );
		reserve( newSize );
		::std::strncpy( MEM, EXT_MEM( string_ ), static_cast<size_t>( newSize ) );
		SET_SIZE( newSize );
	}
	return;
	M_EPILOG
}

HString::HString( HString&& string_ )
	: _len() {
	::memcpy( _mem, string_._mem, INPLACE_BUFFER_SIZE );
	::memset( string_._mem, 0, INPLACE_BUFFER_SIZE );
}

HString::HString( int long preallocate_, char fill_ )
	: _len() {
	M_PROLOG
	reserve( preallocate_ );
	::std::memset( MEM, fill_, static_cast<size_t>( preallocate_ ) );
	SET_SIZE( fill_ ? preallocate_ : 0 );
	return;
	M_EPILOG
}

void HString::reserve( int long preallocate_ ) {
	M_PROLOG
	if ( ( preallocate_ < 0 ) || ( preallocate_ > MAX_STRING_LENGTH ) ) {
		M_THROW( _( "bad new buffer size requested" ), preallocate_ );
	}
	/* Increase requested buffer size to accommodate space terminating NIL. */
	++ preallocate_;
	int long oldAllocBytes( GET_ALLOC_BYTES );
	if ( preallocate_ > oldAllocBytes ) {
		int long newAllocBytes = 1;
		while ( newAllocBytes < preallocate_ ) {
			newAllocBytes <<= 1;
		}
		if ( ! IS_INPLACE ) {
			_ptr = memory::realloc<char>( _ptr, newAllocBytes );
			SET_ALLOC_BYTES( newAllocBytes );
			::std::memset( _ptr + oldAllocBytes, 0, static_cast<size_t>( newAllocBytes - oldAllocBytes ) );
		} else {
			char* newMem( memory::calloc<char>( newAllocBytes ) );
			int long origSize( GET_SIZE );
			::std::strncpy( newMem, _mem, static_cast<size_t>( origSize ) );
			_ptr = newMem;
			SET_ALLOC_BYTES( newAllocBytes );
			SET_SIZE( origSize );
		}
	}
	return;
	M_EPILOG
}

HString::HString( HUTF8String const& str_ )
	: _len() {
	M_PROLOG
	int long newSize( str_.character_count() );
	reserve( newSize );
	int i( 0 );
	for ( u32_t codePoint : str_ ) {
		MEM[i] = static_cast<char>( codePoint );
		++ i;
	}
	M_ASSERT( i == newSize );
	MEM[ newSize ] = 0;
	SET_SIZE( newSize );
	return;
	M_EPILOG
}

HString::HString( char const* str_ )
	: _len() {
	M_PROLOG
	if ( str_ ) {
		int long newSize( static_cast<int long>( ::std::strlen( str_ ) ) );
		reserve( newSize );
		::std::strncpy( MEM, str_, static_cast<size_t>( newSize ) );
		SET_SIZE( newSize );
	}
	return;
	M_EPILOG
}

HString::HString( const_iterator first_, const_iterator last_ )
	: _len() {
	M_PROLOG
	int long newSize( last_ - first_ );
	reserve( newSize );
	::std::strncpy( MEM, first_, static_cast<size_t>( newSize ) );
	MEM[ newSize ] = 0;
	SET_SIZE( newSize );
	return;
	M_EPILOG
}

HString::HString( char const* array_, int long size_ )
	: _len() {
	M_PROLOG
	if ( array_ ) {
		int long newSize( static_cast<int long>( ::strnlen( array_, static_cast<size_t>( size_ ) ) ) );
		reserve( newSize );
		::std::strncpy( MEM, array_, static_cast<size_t>( newSize ) );
		MEM[ newSize ] = 0;
		SET_SIZE( newSize );
	}
	return;
	M_EPILOG
}

HString::HString( char char_ )
	: _len() {
	M_PROLOG
	if ( char_ ) {
		_mem[ 0 ] = char_;
		_mem[ 1 ] = 0;
		SET_SIZE( 1 );
	}
	return;
	M_EPILOG
}

HString::HString( char unsigned charUnsigned_ )
	: _len() {
	M_PROLOG
	if ( charUnsigned_ ) {
		_mem[ 0 ] = static_cast<char>( charUnsigned_ );
		_mem[ 1 ] = 0;
		SET_SIZE( 1 );
	}
	return;
	M_EPILOG
}

HString::HString( int short shortInt_ )
	: _len() {
	M_PROLOG
	int long newSize( ::snprintf( nullptr, 0, "%hd", shortInt_ ) );
	reserve( newSize );
	M_ENSURE( ::snprintf( MEM, static_cast<size_t>( newSize + 1 ), "%hd", shortInt_ ) == newSize );
	SET_SIZE( newSize );
	return;
	M_EPILOG
}

HString::HString( int short unsigned unsignedShortInt_ )
	: _len() {
	M_PROLOG
	int long newSize( ::snprintf( nullptr, 0, "%hu", unsignedShortInt_ ) );
	reserve( newSize );
	M_ENSURE( ::snprintf( MEM, static_cast<size_t>( newSize + 1 ), "%hu", unsignedShortInt_ ) == newSize );
	SET_SIZE( newSize );
	return;
	M_EPILOG
}

HString::HString( int int_ )
	: _len() {
	M_PROLOG
	int long newSize( ::snprintf( nullptr, 0, "%d", int_ ) );
	reserve( newSize );
	M_ENSURE( ::snprintf( MEM, static_cast<size_t>( newSize + 1 ), "%d", int_ ) == newSize );
	SET_SIZE( newSize );
	return;
	M_EPILOG
}

HString::HString( int unsigned int_ )
	: _len() {
	M_PROLOG
	int long newSize( ::snprintf( nullptr, 0, "%u", int_ ) );
	reserve( newSize );
	M_ENSURE( ::snprintf( MEM, static_cast<size_t>( newSize + 1 ), "%u", int_ ) == newSize );
	SET_SIZE( newSize );
	return;
	M_EPILOG
}

HString::HString( int long long_ )
	: _len() {
	M_PROLOG
	int long newSize( ::snprintf( nullptr, 0, "%ld", long_ ) );
	reserve( newSize );
	M_ENSURE( ::snprintf( MEM, static_cast<size_t>( newSize + 1 ), "%ld", long_ ) == newSize );
	SET_SIZE( newSize );
	return;
	M_EPILOG
}

HString::HString( int long unsigned long_ )
	: _len() {
	M_PROLOG
	int long newSize( ::snprintf( nullptr, 0, "%lu", long_ ) );
	reserve( newSize );
	M_ENSURE( ::snprintf( MEM, static_cast<size_t>( newSize + 1 ), "%lu", long_ ) == newSize );
	SET_SIZE( newSize );
	return;
	M_EPILOG
}

HString::HString( int long long longLong_ )
	: _len() {
	M_PROLOG
	int long newSize( ::snprintf( nullptr, 0, "%lld", longLong_ ) );
	reserve( newSize );
	M_ENSURE( ::snprintf( MEM, static_cast<size_t>( newSize + 1 ), "%lld", longLong_ ) == newSize );
	SET_SIZE( newSize );
	return;
	M_EPILOG
}

HString::HString( int long long unsigned longLong_ )
	: _len() {
	M_PROLOG
	int long newSize( ::snprintf( nullptr, 0, "%llu", longLong_ ) );
	reserve( newSize );
	M_ENSURE( ::snprintf( MEM, static_cast<size_t>( newSize + 1 ), "%llu", longLong_ ) == newSize );
	SET_SIZE( newSize );
	return;
	M_EPILOG
}

HString::HString( double double_ )
	: _len() {
	M_PROLOG
	int long newSize( ::snprintf( nullptr, 0, "%f", double_ ) );
	reserve( newSize );
	char* buf( MEM );
	M_ENSURE( ::snprintf( buf, static_cast<size_t>( newSize + 1 ), "%f", double_ ) == newSize );
	if ( ( newSize >= 3 /* "0.0" */ ) && ( strchr( buf, '.' ) != nullptr ) ) {
		while ( ( buf[newSize - 1] == '0' ) && ( buf[newSize - 2] != '.' ) ) {
			-- newSize;
		}
		buf[newSize] = 0;
	}
	SET_SIZE( newSize );
	return;
	M_EPILOG
}

HString::HString( double long double_ )
	: _len() {
	M_PROLOG
	int long newSize( ::snprintf( nullptr, 0, "%.12Lf", double_ ) );
	reserve( newSize );
	char* buf( MEM );
	M_ENSURE( ::snprintf( buf, static_cast<size_t>( newSize + 1 ), "%.12Lf", double_ ) == newSize );
	if ( ( newSize >= 3 /* "0.0" */ ) && ( strchr( buf, '.' ) != nullptr ) ) {
		while ( ( buf[newSize - 1] == '0' ) && ( buf[newSize - 2] != '.' ) ) {
			-- newSize;
		}
		buf[newSize] = 0;
	}
	SET_SIZE( newSize );
	return;
	M_EPILOG
}

HString::HString( void const* ptrVoid_ )
	: _len() {
	M_PROLOG
	/*
	 * Solaris libc omits 0x in %p conversion.
	 * Well, that sucks.
	 */
	int long newSize( ::snprintf( nullptr, 0, "0x%lx", reinterpret_cast<int long>( ptrVoid_ ) ) );
	reserve( newSize );
	M_ENSURE( ::snprintf( MEM, static_cast<size_t>( newSize + 1 ), "0x%lx", reinterpret_cast<int long>( ptrVoid_ ) ) == newSize );
	SET_SIZE( newSize );
	return;
	M_EPILOG
}

#if __GCC_VERSION_LOWER_OR_EQUAL__ <= 4005002
# pragma GCC diagnostic error "-Weffc++"
#endif /* #if __GCC_VERSION_LOWER_OR_EQUAL__ <= 4005002 */

void HString::materialize( void ) {
	M_PROLOG
	return;
	M_EPILOG
}

HString& HString::operator = ( HString const& string_ ) {
	M_PROLOG
	if ( this != &string_ ) {
		int long newSize( string_.get_length() );
		if ( newSize >= GET_ALLOC_BYTES ) {
			reserve( newSize );
		}
		if ( newSize ) {
			::std::strncpy( MEM, EXT_MEM( string_ ), static_cast<size_t>( newSize ) );
		}
		MEM[ newSize ] = 0;
		SET_SIZE( newSize );
	}
	return ( *this );
	M_EPILOG
}

HString& HString::operator = ( HString&& string_ ) {
	M_PROLOG
	if ( &string_ != this ) {
		if ( ! IS_INPLACE ) {
			memory::free( _ptr );
		}
		::memcpy( _mem, string_._mem, INPLACE_BUFFER_SIZE );
		::memset( string_._mem, 0, INPLACE_BUFFER_SIZE );
	}
	return ( *this );
	M_EPILOG
}

HString& HString::operator += ( HString const& string_ ) {
	M_PROLOG
	int long otherSize( string_.get_length() );
	if ( otherSize ) {
		int long oldSize( GET_SIZE );
		int long newSize( oldSize + otherSize );
		reserve( newSize );
		::std::strcpy( MEM + oldSize, EXT_MEM( string_ ) );
		SET_SIZE( newSize );
	}
	return ( *this );
	M_EPILOG
}

HString HString::operator + ( HString const& other_ ) const {
	M_PROLOG
	HString str( *this );
	return ( str += other_ );
	M_EPILOG
}

char HString::operator[] ( int index_ ) const {
	return ( operator[] ( static_cast<int long>( index_ ) ) );
}

char HString::operator[] ( int long index_ ) const {
	M_PROLOG
	if ( ( index_ < 0 ) || ( index_ > GET_SIZE ) ) {
		M_THROW( _errMsgHString_[string_helper::INDEX_OOB], index_ );
	}
	return ( MEM[ index_ ] );
	M_EPILOG
}

char HString::set_at( int long index_, char char_ ) {
	M_PROLOG
	if ( ( index_ < 0 ) || ( index_ >= GET_SIZE ) ) {
		M_THROW( _errMsgHString_[string_helper::INDEX_OOB], index_ );
	}
	MEM[ index_ ] = char_;
	if ( ! char_ ) {
		SET_SIZE( index_ );
	}
	return ( char_ );
	M_EPILOG
}

bool HString::operator == ( HString const& other_ ) const {
	M_PROLOG
	return ( ( this == &other_ )
			|| ( ( GET_SIZE == other_.get_length() )
				&& ( ! ( GET_SIZE && ::std::strcmp( MEM, EXT_MEM( other_ ) ) ) ) ) );
	M_EPILOG
}

bool HString::operator != (  HString const& other_ ) const {
	M_PROLOG
	return ( ! operator == ( other_ ) );
	M_EPILOG
}

bool HString::operator >= ( HString const& other_ ) const {
	M_PROLOG
	return ( ::std::strcoll( MEM, EXT_MEM( other_ ) ) >= 0 );
	M_EPILOG
}

bool HString::operator <= ( HString const& other_ ) const {
	M_PROLOG
	return ( ::std::strcoll( MEM, EXT_MEM( other_ ) ) <= 0 );
	M_EPILOG
}

bool HString::operator > ( HString const& other_ ) const {
	M_PROLOG
	return ( ! operator <= ( other_ ) );
	M_EPILOG
}

bool HString::operator < ( HString const& other_ ) const {
	M_PROLOG
	return ( ! operator >= ( other_ ) );
	M_EPILOG
}

HString operator + ( char const* left_, HString const& right_ ) {
	M_PROLOG
	HString tmp( left_ );
	tmp.operator += ( right_ );
	return ( tmp );
	M_EPILOG
}

bool operator == ( char const* left_, HString const& right_ ) {
	M_PROLOG
	return ( right_.operator == ( left_ ) );
	M_EPILOG
}

bool operator != ( char const* left_, HString const& right_ ) {
	M_PROLOG
	return ( right_.operator != ( left_ ) );
	M_EPILOG
}

bool operator > ( char const* left_, HString const& right_ ) {
	M_PROLOG
	return ( right_.operator < ( left_ ) );
	M_EPILOG
}

bool operator < ( char const* left_, HString const& right_ ) {
	M_PROLOG
	return ( right_.operator > ( left_ ) );
	M_EPILOG
}

bool operator >= ( char const* left_, HString const& right_ ) {
	M_PROLOG
	return ( right_.operator <= ( left_ ) );
	M_EPILOG
}

bool operator <= ( char const* left_, HString const& right_ ) {
	M_PROLOG
	return ( right_.operator >= ( left_ ) );
	M_EPILOG
}

char const* HString::c_str( void ) const {
	return ( MEM );
}

char HString::front( void ) const {
	if ( GET_SIZE == 0 ) {
		M_THROW( _errMsgHString_[string_helper::INDEX_OOB], 0 );
	}
	return ( MEM[0] );
}

char HString::back( void ) const {
	int long s( GET_SIZE );
	if ( s == 0 ) {
		M_THROW( _errMsgHString_[string_helper::INDEX_OOB], s - 1 );
	}
	return ( MEM[s - 1] );
}

HString::const_iterator HString::begin( void ) const {
	return ( MEM );
}

HString::const_iterator HString::end( void ) const {
	return ( MEM + get_length() );
}

HString::iterator HString::begin( void ) {
	return ( iterator( this, 0 ) );
}

HString::iterator HString::end( void ) {
	return ( iterator( this, get_length() ) );
}

bool HString::empty( void ) const {
	return ( is_empty() );
}

bool HString::is_empty( void ) const {
	return ( ! GET_SIZE );
}

bool HString::operator ! ( void ) const {
	return ( is_empty() );
}

void HString::clear( void ) {
	MEM[0] = 0;
	SET_SIZE( 0 );
	return;
}

int long HString::get_length( void ) const {
	M_PROLOG
	M_ASSERT( ! GET_SIZE || ( GET_SIZE < GET_ALLOC_BYTES ) );
	return ( GET_SIZE );
	M_EPILOG
}

int long HString::length( void ) const {
	M_PROLOG
	return ( get_length() );
	M_EPILOG
}

int long HString::get_size( void ) const {
	M_PROLOG
	return ( get_length() );
	M_EPILOG
}

int long HString::size( void ) const {
	M_PROLOG
	return ( get_length() );
	M_EPILOG
}

int long HString::capacity( void ) const {
	M_PROLOG
	return ( get_capacity() );
	M_EPILOG
}

int long HString::get_capacity( void ) const {
	M_PROLOG
	return ( GET_ALLOC_BYTES - 1 );
	M_EPILOG
}

int long HString::get_max_size( void ) const {
	return ( MAX_STRING_LENGTH );
}

int long HString::max_size( void ) const {
	return ( MAX_STRING_LENGTH );
}

void HString::swap( HString& other ) {
	using yaal::swap;
	swap( _len, other._len );
	return;
}

HString& HString::assign( HString const& str_, int long offset_, int long length_ ) {
	M_PROLOG
	if ( length_ == npos ) {
		length_ = MAX_STRING_LENGTH;
	}
	if ( length_ < 0 ) {
		M_THROW( _errMsgHString_[string_helper::BAD_LENGTH], length_ );
	}
	if ( offset_ < 0 ) {
		M_THROW( _errMsgHString_[string_helper::BAD_OFFSET], offset_ );
	}
	int long s( str_.get_length() );
	int long newSize( 0 );
	if ( offset_ < s ) {
		newSize = ( length_ > ( s - offset_ ) ) ? s - offset_ : length_;
		reserve( newSize );
		::memcpy( MEM, EXT_MEM( str_ ) + offset_, static_cast<size_t>( newSize ) );
	}
	MEM[ newSize ] = 0;
	SET_SIZE( newSize );
	return ( *this );
	M_EPILOG
}

HString& HString::assign( char const* data_, int long length_ ) {
	M_PROLOG
	if ( ! data_ ) {
		M_THROW( _errMsgHString_[ string_helper::NULL_PTR ], errno );
	}
	if ( length_ < 0 ) {
		M_THROW( _errMsgHString_[ string_helper::BAD_LENGTH ], length_ );
	}
	int long newSize( static_cast<int long>( ::strnlen( data_, static_cast<size_t>( length_ ) ) ) );
	reserve( newSize );
	::memcpy( MEM, data_, static_cast<size_t>( newSize ) );
	MEM[ newSize ] = 0;
	SET_SIZE( newSize );
	return ( *this );
	M_EPILOG
}

HString& HString::assign( const_iterator first_, const_iterator last_ ) {
	M_PROLOG
	if ( ! ( first_ && last_ ) ) {
		M_THROW( _errMsgHString_[ string_helper::NULL_PTR ], errno );
	}
	if ( last_ < first_ ) {
		M_THROW( _errMsgHString_[ string_helper::BAD_LENGTH ], last_ - first_ );
	}
	int long newSize( static_cast<int long>( ::strnlen( first_, static_cast<size_t>( last_ - first_ ) ) ) );
	reserve( newSize );
	::memcpy( MEM, first_, static_cast<size_t>( newSize ) );
	MEM[ newSize ] = 0;
	SET_SIZE( newSize );
	return ( *this );
	M_EPILOG
}

HString& HString::assign( int long size_, char fill_ ) {
	M_PROLOG
	if ( size_ < 0 ) {
		M_THROW( _errMsgHString_[string_helper::BAD_LENGTH], size_ );
	}
	reserve( size_ );
	::memset( MEM, fill_, static_cast<size_t>( size_ ) );
	MEM[ size_ ] = 0;
	SET_SIZE( fill_ ? size_ : 0 );
	return ( *this );
	M_EPILOG
}

int long HString::find( char char_, int long after_ ) const {
	M_PROLOG
	if ( after_ >= GET_SIZE ) {
		return ( npos );
	}
	if ( after_ < 0 ) {
		after_ = 0;
	}
	char const* str = static_cast<char const*>( ::std::memchr( MEM + after_, char_, static_cast<size_t>( GET_SIZE - after_ ) ) );
	if ( ! str ) {
		return ( npos );
	}
	return ( static_cast<int long>( str - MEM ) );
	M_EPILOG
}

int long HString::find( HString const& pattern_, int long after_ ) const {
	M_PROLOG
	return ( ! pattern_.is_empty() ? nfind( pattern_, pattern_.get_length(), after_ ) : npos );
	M_EPILOG
}

int long HString::nfind( HString const& pattern_, int long patternLength_, int long after_ ) const {
	M_PROLOG
	if ( pattern_.is_empty() ) {
		return ( npos );
	}
	if ( after_ < 0 ) {
		after_ = 0;
	}
	if ( ( ! patternLength_ )
			|| ( GET_SIZE < ( after_ + patternLength_ ) ) ) {
		return ( npos );
	}
	int long idx = string_helper::kmpsearch( MEM + after_,
			GET_SIZE - after_, EXT_MEM( pattern_ ), patternLength_ );
	return ( idx >= 0 ? idx + after_ : npos );
	M_EPILOG
}

int long HString::find_one_of( HString const& set_, int long after_ ) const {
	M_PROLOG
	if ( set_.is_empty() ) {
		return ( npos );
	}
	if ( after_ < 0 ) {
		after_ = 0;
	}
	if ( after_ >= GET_SIZE ) {
		return ( npos );
	}
	char const* str( ::std::strpbrk( MEM + after_, set_.c_str() ) );
	if ( ! str ) {
		return ( npos );
	}
	return ( static_cast<int long>( str - MEM ) );
	M_EPILOG
}

int long HString::reverse_find_one_of( HString const& set_, int long before_ ) const {
	M_PROLOG
	if ( set_.is_empty() ) {
		return ( npos );
	}
	if ( before_ < 0 ) {
		before_ = 0;
	}
	if ( before_ >= GET_SIZE ) {
		return ( npos );
	}
	char* str( string_helper::strrnpbrk( MEM, set_.c_str(), GET_SIZE - before_ ) );
	if ( ! str ) {
		return ( npos );
	}
	return ( static_cast<int long>( ( GET_SIZE - 1 ) - ( str - MEM ) ) );
	M_EPILOG
}

int long HString::find_last_one_of( HString const& set_, int long before_ ) const {
	M_PROLOG
	if ( ( before_ == npos ) || ( before_ >= GET_SIZE ) ) {
		before_ = GET_SIZE - 1;
	}
	if ( ( before_ < 0 ) || set_.is_empty() ) {
		return ( npos );
	}
	char* str( string_helper::strrnpbrk( MEM, set_.c_str(), before_ + 1 ) );
	if ( ! str ) {
		return ( npos );
	}
	return ( static_cast<int long>( str - MEM ) );
	M_EPILOG
}

int long HString::find_other_than( HString const& set_, int long after_ ) const {
	M_PROLOG
	if ( after_ < 0 ) {
		after_ = 0;
	}
	if ( after_ >= GET_SIZE ) {
		return ( npos );
	}
	if ( set_.is_empty() ) {
		return ( after_ );
	}
	int long index = static_cast<int long>( ::std::strspn( MEM + after_, set_.c_str() ) );
	if ( ( index + after_ ) >= GET_SIZE ) {
		return ( npos );
	}
	return ( index + after_ );
	M_EPILOG
}

int long HString::reverse_find_other_than( HString const& set_, int long before_ ) const {
	M_PROLOG
	if ( before_ < 0 ) {
		before_ = 0;
	}
	if ( before_ >= GET_SIZE ) {
		return ( npos );
	}
	if ( set_.is_empty() ) {
		return ( before_ );
	}
	int long index( string_helper::strrnspn( MEM, set_.c_str(), GET_SIZE - before_ ) );
	if ( index >= ( GET_SIZE - before_ ) ) {
		return ( npos );
	}
	return ( ( GET_SIZE - 1 ) - index );
	M_EPILOG
}

int long HString::find_last_other_than( HString const& set_, int long before_ ) const {
	M_PROLOG
	if ( ( before_ == npos ) || ( before_ >= GET_SIZE ) ) {
		before_ = GET_SIZE - 1;
	}
	if ( before_ < 0 ) {
		return ( npos );
	}
	if ( set_.is_empty() ) {
		return ( before_ );
	}
	int long index( string_helper::strrnspn( MEM, set_.c_str(), before_ + 1 ) );
	if ( index > before_ ) {
		return ( npos );
	}
	return ( index );
	M_EPILOG
}

int long HString::reverse_find( char char_, int long before_ ) const {
	M_PROLOG
	if ( before_ >= GET_SIZE ) {
		return ( npos );
	}
	if ( before_ < 0 ) {
		before_ = 0;
	}
	char const* str = static_cast<char const*>( ::memrchr( MEM, char_, static_cast<size_t>( GET_SIZE - before_ ) ) );
	if ( ! str ) {
		return ( npos );
	}
	return ( static_cast<int long>( ( GET_SIZE - 1 ) - ( str - MEM ) ) );
	M_EPILOG
}

int long HString::find_last( char char_, int long before_ ) const {
	M_PROLOG
	if ( ( before_ == npos ) || ( before_ >= GET_SIZE ) ) {
		before_ = GET_SIZE - 1;
	}
	if ( before_ < 0 ) {
		return ( npos );
	}
	char const* str( static_cast<char const*>( ::memrchr( MEM, char_, static_cast<size_t>( before_ + 1 ) ) ) );
	return ( str ? static_cast<int long>( str - MEM ) : npos );
	M_EPILOG
}

int long HString::find_last( HString const& str_, int long before_ ) const {
	M_PROLOG
	if ( ( before_ == npos ) || ( before_ >= GET_SIZE ) ) {
		before_ = GET_SIZE - 1;
	}
	if ( before_ < 0 ) {
		return ( npos );
	}
	HString str( MEM, min( GET_SIZE, before_ + str_.get_length() ) );
	HString pat( str_ );
	yaal::reverse( EXT_MEM( str ), EXT_MEM( str ) + str.get_length() );
	yaal::reverse( EXT_MEM( pat ), EXT_MEM( pat ) + pat.get_length() );
	int long pos( str.find( pat ) );
	return ( pos != npos ? str.get_length() - ( pos + str_.get_length() ) : npos );
	M_EPILOG
}

HString& HString::replace( HString const& pattern_,
		HString const& with_ ) {
	M_PROLOG
	if ( pattern_.is_empty() ) {
		return ( *this );
	}
	int long lenPattern( pattern_.get_length() );
	int long lenWith( with_.get_length() );
	int long subWP( lenWith - lenPattern );
	int long patPos( 0 );
	if ( subWP == 0 ) { /* replacement is equal length to pattern */
		while ( ( patPos = find( pattern_, patPos ) ) != npos ) {
			::std::strncpy( MEM + patPos, EXT_MEM( with_ ), static_cast<size_t>( lenWith ) );
			patPos += lenPattern;
		}
	} else {
		int long newSize( GET_SIZE );
		while ( ( patPos = find( pattern_, patPos ) ) != npos ) {
			newSize += subWP;
			patPos += lenPattern;
		}
		HString s;
		HString* src( nullptr );
		if ( subWP > 0 ) { /* replacement is longer than pattern */
			s = *this;
			s.materialize();
			reserve( newSize );
			src = &s;
		} else { /* replacement is shorter than pattern */
			src = this;
		}
		int long oldLen( 0 );
		int long newLen( 0 );
		patPos = 0;
		char const* with( EXT_MEM( with_ ) );
		char const* srcBuf( EXT_MEM( (*src) ) );
		char* buf( MEM );
		while ( ( patPos = src->find( pattern_, patPos ) ) != npos ) {
			if ( patPos > oldLen ) {
				::memmove( buf + newLen, srcBuf + oldLen, static_cast<size_t>( patPos - oldLen ) );
				newLen += ( patPos - oldLen );
			}
			::memmove( buf + newLen, with, static_cast<size_t>( lenWith ) );
			newLen += lenWith;
			patPos += lenPattern;
			oldLen = patPos;
		}
		if ( oldLen < GET_SIZE ) {
			::std::memmove( buf + newLen, srcBuf + oldLen, static_cast<size_t>( ( GET_SIZE - oldLen ) ) );
		}
		SET_SIZE( newSize );
		MEM[ newSize ] = 0;
	}
	return ( *this );
	M_EPILOG
}

HString& HString::replace( int long pos_, int long size_, HString const& replacement ) {
	M_PROLOG
	return ( replace( pos_, size_, EXT_MEM( replacement ), replacement.get_length() ) );
	M_EPILOG
}

HString& HString::replace( int long pos_, int long size_, HString const& replacement, int long offset_, int long len_ ) {
	M_PROLOG
	if ( offset_ < 0 ) {
		M_THROW( _errMsgHString_[string_helper::BAD_OFFSET], offset_ );
	}
	return ( replace( pos_, size_, EXT_MEM( replacement ) + offset_, len_ ) );
	M_EPILOG
}

HString& HString::replace( int long pos_, int long size_, char const* buffer_, int long len_ ) {
	M_PROLOG
	if ( size_ < 0 ) {
		M_THROW( _errMsgHString_[string_helper::BAD_LENGTH], size_ );
	}
	if ( len_ < 0 ) {
		M_THROW( _errMsgHString_[string_helper::BAD_LENGTH], len_ );
	}
	if ( pos_ < 0 ) {
		M_THROW( _errMsgHString_[string_helper::BAD_OFFSET], pos_ );
	}
	int long oldSize( GET_SIZE );
	if ( ( pos_ + size_ ) > oldSize ) {
		M_THROW( _errMsgHString_[string_helper::OVERFLOW], pos_ + size_ );
	}
	int long newSize( oldSize + ( len_ - size_ ) );
	if ( len_ > size_ ) {
		reserve( newSize );
	}
	::memmove( MEM + pos_ + len_, MEM + pos_ + size_, static_cast<size_t>( oldSize - ( pos_ + size_ ) ) );
	::memcpy( MEM + pos_, buffer_, static_cast<size_t>( len_ ) );
	SET_SIZE( newSize );
	MEM[ newSize ] = 0;
	return ( *this );
	M_EPILOG
}

HString& HString::replace( iterator first_, iterator last_, HString const& replacement ) {
	M_PROLOG
	M_ENSURE( first_._owner == this );
	M_ENSURE( last_._owner == this );
	return ( replace( static_cast<HString::const_iterator>( first_ ) - MEM, last_ - first_, replacement ) );
	M_EPILOG
}

HString& HString::upper( void ) {
	M_PROLOG
	for ( int long ctr( 0 ), SIZE( GET_SIZE ); ctr < SIZE; ++ ctr ) {
		MEM[ ctr ] = static_cast<char>( ::std::toupper( MEM[ ctr ] ) );
	}
	return ( *this );
	M_EPILOG
}

HString& HString::lower( void ) {
	M_PROLOG
	for ( int long ctr( 0 ), SIZE( GET_SIZE ); ctr < SIZE; ++ ctr ) {
		MEM[ ctr ] = static_cast<char>( ::std::tolower( MEM[ ctr ] ) );
	}
	return ( *this );
	M_EPILOG
}

HString& HString::reverse( void ) {
	M_PROLOG
	char char_;
	for ( int long ctr( 0 ), SIZE( GET_SIZE ); ctr < SIZE; ++ ctr ) {
		char_ = static_cast<char>( ::std::toupper( MEM[ ctr ] ) );
		if ( char_ == MEM[ ctr ] ) {
			MEM[ ctr ] = static_cast<char>( ::std::tolower( MEM[ ctr ] ) );
		} else {
			MEM[ ctr ] = char_;
		}
	}
	return ( *this );
	M_EPILOG
}

void HString::substr( HString& dest_, int long from_, int long length_ ) const {
	M_PROLOG
	if ( length_ == npos ) {
		length_ = MAX_STRING_LENGTH;
	}
	if ( from_ < 0 ) {
		length_ += from_;
		from_ = 0;
	}
	if ( ( length_ > 0 ) && ( from_ < GET_SIZE ) ) {
		int long newSize( min( length_, GET_SIZE ) );
		if ( ( newSize + from_ ) > GET_SIZE ) {
			newSize = GET_SIZE - from_;
		}
		dest_.reserve( newSize );
		::std::strncpy( EXT_MEM( dest_ ), MEM + from_, static_cast<size_t>( newSize ) );
		EXT_MEM( dest_ )[ newSize ] = 0;
		EXT_SET_SIZE( dest_, newSize );
	}
	return;
	M_EPILOG
}

HString HString::substr( int long from_, int long length_ ) const {
	M_PROLOG
	HString str;
	substr( str, from_, length_ );
	return ( str );
	M_EPILOG
}

HString HString::left( int long to_ ) const {
	M_PROLOG
	HString str;
	if ( to_ < 1 ) {
		return ( str );
	}
	int long newSize( min( to_, GET_SIZE ) );
	str.reserve( newSize );
	::std::strncpy( EXT_MEM( str ), MEM, static_cast<size_t>( newSize ) );
	EXT_MEM( str )[ newSize ] = 0;
	EXT_SET_SIZE( str, newSize );
	return ( str );
	M_EPILOG
}

HString HString::mid( int long from_, int long length_ ) const {
	M_PROLOG
	HString str;
	substr( str, from_, length_ );
	return ( str );
	M_EPILOG
}

HString HString::right( int long fromEnd_ ) const {
	M_PROLOG
	HString str;
	if ( fromEnd_ < 1 ) {
		return ( str );
	}
	int long newSize( min( fromEnd_, GET_SIZE ) );
	str.reserve( newSize );
	::std::strncpy( EXT_MEM( str ), MEM + GET_SIZE - newSize, static_cast<size_t>( newSize ) );
	EXT_MEM( str )[ newSize ] = 0;
	EXT_SET_SIZE( str, newSize );
	return ( str );
	M_EPILOG
}

HString& HString::trim_left( HString const& set_ ) {
	M_PROLOG
	int cut( 0 );
	while ( MEM[ cut ] && ( set_.find( MEM[ cut ] ) != npos ) ) {
		++ cut;
	}
	if ( cut ) {
		shift_left( cut );
	}
	return ( *this );
	M_EPILOG
}

HString& HString::trim_right( HString const& set_ ) {
	M_PROLOG
	int long cut( 0 );
	while ( ( cut < GET_SIZE ) && ( set_.find( MEM[ GET_SIZE - ( cut + 1 ) ] ) != npos ) ) {
		++ cut;
	}
	if ( cut ) {
		SET_SIZE( GET_SIZE - cut );
		MEM[ GET_SIZE ] = 0;
	}
	M_ASSERT( GET_SIZE >= 0 );
	return ( *this );
	M_EPILOG
}

HString& HString::trim( HString const& set_ ) {
	M_PROLOG
	trim_left( set_ );
	trim_right( set_ );
	return ( *this );
	M_EPILOG
}

HString& HString::shift_left( int long shift_ ) {
	M_PROLOG
	if ( shift_ < 0 ) {
		M_THROW( "bad left shift length", shift_ );
	}
	if ( shift_ ) {
		if ( shift_ < GET_SIZE ) {
			SET_SIZE( GET_SIZE - shift_ );
			::std::memmove( MEM, MEM + shift_, static_cast<size_t>( GET_SIZE + 1 ) );
		} else {
			clear();
		}
	}
	return ( *this );
	M_EPILOG
}

HString& HString::shift_right( int long shift_, char const filler_ ) {
	M_PROLOG
	if ( shift_ < 0 ) {
		M_THROW( "bad right shift length", shift_ );
	}
	if ( shift_ ) {
		int long oldSize( GET_SIZE );
		int long newSize( oldSize + shift_ );
		reserve( newSize );
		::std::memmove( MEM + shift_, MEM, static_cast<size_t>( oldSize + 1 ) );
		/* using SET_SIZE twice is not a bug or mistake,
		 * fill() depends on size and modifies it
		 * so it must have new size before and we need to
		 * undo fill()'s size modification after fill() is done.
		 */
		SET_SIZE( newSize );
		fill( filler_, 0, shift_ );
		SET_SIZE( newSize );
	}
	return ( *this );
	M_EPILOG
}

HString& HString::fill( char filler_, int long offset_, int long count_ ) {
	M_PROLOG
	if ( count_ == npos ) {
		count_ = ( GET_SIZE - offset_ );
	}
	if ( count_ < 0 ) {
		M_THROW( _errMsgHString_[string_helper::BAD_LENGTH], count_ );
	}
	if ( ( offset_ < 0 ) || ( offset_ > GET_SIZE ) ) {
		M_THROW( _errMsgHString_[string_helper::BAD_OFFSET], offset_ );
	}
	if ( filler_ ) {
		if ( ( count_ + offset_ ) > GET_SIZE ) {
			reserve( count_ + offset_ );
			SET_SIZE( count_ + offset_ );
		}
		::std::memset( MEM + offset_, filler_, static_cast<size_t>( count_ ) );
	} else {
		SET_SIZE( offset_ );
	}
	MEM[ GET_SIZE ] = 0;
	return ( *this );
	M_EPILOG
}

HString& HString::fillz( char filler_, int long offset_, int long count_ ) {
	M_PROLOG
	fill( filler_, offset_, count_ );
	if ( count_ == npos ) {
		count_ = ( GET_SIZE - offset_ );
	}
	MEM[ count_ + offset_ ] = 0;
	SET_SIZE( count_ + offset_ );
	return ( *this );
	M_EPILOG
}

HString& HString::erase( int long from_, int long length_ ) {
	M_PROLOG
	if ( length_ == npos ) {
		length_ = GET_SIZE;
	}
	if ( from_ < 0 ) {
		length_ += from_;
		from_ = 0;
	}
	if ( ( from_ + length_ ) > GET_SIZE ) {
		length_ = GET_SIZE - from_;
	}
	if ( ( length_ > 0 ) && ( from_ < GET_SIZE ) ) {
		::std::memmove( MEM + from_, MEM + from_ + length_, static_cast<size_t>( GET_SIZE - ( from_ + length_ ) ) );
		SET_SIZE( GET_SIZE - length_ );
		MEM[ GET_SIZE ] = 0;
	}
	return ( *this );
	M_EPILOG
}

HString& HString::insert( int long from_, HString const& str_ ) {
	M_PROLOG
	return ( insert( from_, str_.get_length(), EXT_MEM( str_ ) ) );
	M_EPILOG
}

HString& HString::insert( int long from_, int long length_, char const* chunk_ ) {
	M_PROLOG
	if ( from_ < 0 ) {
		if ( chunk_ ) {
			if ( ( -from_ ) > static_cast<int long>( ::strnlen( chunk_, static_cast<size_t>( length_ ) ) ) ) {
				M_THROW( "negative offset caused chunk overflow", from_ );
			}
			chunk_ += -from_;
		}
		length_ += from_;
		from_ = 0;
	}
	if ( ( length_ > 0 ) && ( from_ <= GET_SIZE ) ) {
		int long chunkLen = chunk_ ? static_cast<int long>( ::strnlen( chunk_, static_cast<size_t>( length_ ) ) ) : 0;
		if ( chunk_ && ( length_ > chunkLen ) ) {
			M_THROW( "length too big for this chunk (by)", length_ - chunkLen );
		}
		int long oldSize( GET_SIZE );
		int long newSize( oldSize + length_ );
		reserve( newSize );
		::std::memmove( MEM + from_ + length_, MEM + from_, static_cast<size_t>( ( oldSize + 1 ) - from_ ) );
		if ( chunk_ ) {
			::std::strncpy( MEM + from_, chunk_, static_cast<size_t>( length_ ) );
		}
		SET_SIZE( newSize );
	}
	return ( *this );
	M_EPILOG
}

HString& HString::insert( int long from_, char const* chunk_ ) {
	M_PROLOG
	return ( insert( from_, static_cast<int long>( ::strlen( chunk_ ) ), chunk_ ) );
	M_EPILOG
}

HString& HString::insert( int long from_, char const* chunk_, int long length_ ) {
	M_PROLOG
	return ( insert( from_, length_, chunk_ ) );
	M_EPILOG
}

HString& HString::insert( int long from_, int long length_, char char_ ) {
	M_PROLOG
	insert( from_, length_ );
	if ( ( from_ + length_ ) > 0 )
		fill( char_, from_, length_ );
	return ( *this );
	M_EPILOG
}

HString& HString::append( HString const& str_ ) {
	M_PROLOG
	return ( append( EXT_MEM( str_ ), str_.get_length() ) );
	M_EPILOG
}

HString& HString::append( HString const& str_, int long idx_, int long len_ ) {
	M_PROLOG
	if ( len_ == npos ) {
		len_ = MAX_STRING_LENGTH;
	}
	if ( len_ < 0 ) {
		M_THROW( _errMsgHString_[string_helper::BAD_LENGTH], len_ );
	}
	if ( idx_ < 0 ) {
		M_THROW( _errMsgHString_[string_helper::BAD_OFFSET], idx_ );
	}
	if ( ( len_ > 0 ) && ( idx_ < str_.get_length() ) ) {
		append(
			EXT_MEM( str_ ) + ( idx_ >= 0 ? idx_ : 0 ),
			( ( idx_ + len_ ) < str_.get_length() ) ? len_ : str_.get_length() - idx_
		);
	}
	return ( *this );
	M_EPILOG
}

HString& HString::append( int long count_, char val_ ) {
	M_PROLOG
	if ( val_ ) {
		int long oldSize( GET_SIZE );
		int long newSize( oldSize + count_ );
		reserve( newSize );
		::memset( MEM + oldSize, val_, static_cast<size_t>( count_ ) );
		MEM[ newSize ] = 0;
		SET_SIZE( newSize );
	}
	return ( *this );
	M_EPILOG
}

HString& HString::append( char const* buf_, int long len_ ) {
	M_PROLOG
	M_ASSERT( len_ >= 0 );
	if ( len_ > 0 ) {
		int long oldSize( GET_SIZE );
		int long newSize( oldSize + len_ );
		reserve( newSize );
		::memmove( MEM + oldSize, buf_, static_cast<size_t>( len_ ) );
		MEM[ newSize ] = 0;
		SET_SIZE( newSize );
	}
	return ( *this );
	M_EPILOG
}

HString& HString::append( const_iterator first_, const_iterator last_ ) {
	M_PROLOG
	return ( append( first_, static_cast<int long>( last_ - first_ ) ) );
	M_EPILOG
}

void HString::push_back( char character_ ) {
	M_PROLOG
	append( 1, character_ );
	return;
	M_EPILOG
}

void HString::pop_back( void ) {
	M_PROLOG
	erase( GET_SIZE - 1, 1 );
	return;
	M_EPILOG
}

HString::HCharRef& HString::HCharRef::operator = ( char ch_ ) {
	M_PROLOG
	_string.set_at( _index, ch_ );
	return ( *this );
	M_EPILOG
}

void HString::HCharRef::swap( HCharRef& charRef_ ) {
	M_PROLOG
	if ( &charRef_ != this ) {
		char l( _string[_index] );
		char r( charRef_._string[charRef_._index] );
		_string.set_at( _index, r );
		charRef_._string.set_at( charRef_._index, l );
	}
	return;
	M_EPILOG
}

namespace {

void encode( u32_t codePoint_, char*& dest_ ) {
	int r( utf8::rank( codePoint_ ) );
	dest_ += r;
	char* p( dest_ - 1 );
	u8_t head[] = { 0, 0, utf8::ENC_2_BYTES_MARK_VALUE, utf8::ENC_3_BYTES_MARK_VALUE, utf8::ENC_4_BYTES_MARK_VALUE };
	switch( r ) {
		case ( 4 ): { *p = static_cast<char>( utf8::TAIL_BYTES_MARK_VALUE | ( utf8::TAIL_BYTES_VALUE_MASK & codePoint_ ) ); --p; codePoint_ >>= 6; } /* no break - fall through */
		case ( 3 ): { *p = static_cast<char>( utf8::TAIL_BYTES_MARK_VALUE | ( utf8::TAIL_BYTES_VALUE_MASK & codePoint_ ) ); --p; codePoint_ >>= 6; } /* no break - fall through */
		case ( 2 ): { *p = static_cast<char>( utf8::TAIL_BYTES_MARK_VALUE | ( utf8::TAIL_BYTES_VALUE_MASK & codePoint_ ) ); --p; codePoint_ >>= 6; } /* no break - fall through */
		case ( 1 ): { *p = static_cast<char>( head[r] | codePoint_ ); } /* no break - fall through */
	}
}

}

HUTF8String::HUTF8String( void )
	: _characterCount( 0 )
	, _offset( 0 )
	, _byteCount( 0 )
	, _ptr( nullptr ) {
	return;
}

HUTF8String::HUTF8String( HString const& str_ )
	: HUTF8String( str_.begin(), str_.end() ) {
}

HUTF8String::HUTF8String( HString::const_iterator it_, HString::const_iterator end_ )
	: HUTF8String() {
	M_PROLOG
	assign( it_, end_ );
	return;
	M_EPILOG
}

HUTF8String::HUTF8String( char const* str_ )
	: HUTF8String() {
	M_PROLOG
	int byteCount( 0 );
	int characterCount( 0 );
	char const* p( str_ );
	int charBytesLeft( 0 );
	int r( 1 );
	while( p && *p ) {
		if ( charBytesLeft == 0 ) {
			if ( ! ( *p & utf8::ENC_1_BYTES_MARK_MASK ) ) {
				++ characterCount;
			} else if ( ( *p & utf8::ENC_2_BYTES_MARK_MASK ) == utf8::ENC_2_BYTES_MARK_VALUE ) {
				charBytesLeft = 1;
				r = max( r, 2 );
			} else if ( ( *p & utf8::ENC_3_BYTES_MARK_MASK ) == utf8::ENC_3_BYTES_MARK_VALUE ) {
				charBytesLeft = 2;
				r = max( r, 3 );
			} else if ( ( *p & utf8::ENC_4_BYTES_MARK_MASK ) == utf8::ENC_4_BYTES_MARK_VALUE ) {
				charBytesLeft = 3;
				r = max( r, 4 );
			} else {
				throw HUTF8StringException( "Invalid UTF-8 head sequence at: "_ys.append( byteCount ) );
			}
		} else {
			if ( ( *p & utf8::TAIL_BYTES_MARK_MASK ) == utf8::TAIL_BYTES_MARK_VALUE ) {
				-- charBytesLeft;
				if ( ! charBytesLeft ) {
					++ characterCount;
				}
			} else {
				throw HUTF8StringException( "Invalid UTF-8 tail sequence at: "_ys.append( byteCount ) );
			}
		}
		++ p;
		++ byteCount;
	}
	if ( byteCount > 0 ) {
		alloc( byteCount );
		_meta->_used = byteCount;
		_meta->_rank = static_cast<i8_t>( r );
		_byteCount = byteCount;
		_characterCount = characterCount;
		::strncpy( _ptr + sizeof ( OBufferMeta ), str_, static_cast<size_t>( byteCount ) );
	}
	return;
	M_EPILOG
}

HUTF8String::HUTF8String( const_iterator beg_, const_iterator end_ )
	: HUTF8String() {
	M_PROLOG
	M_ASSERT( beg_._ptr == end_._ptr );
	if ( beg_._ptr ) {
		_ptr = beg_._ptr;
		++ _meta->_refCount;
		_characterCount = end_._characterIndex - beg_._characterIndex;
		_offset = beg_._byteIndex;
		_byteCount = end_._byteIndex - beg_._byteIndex;
	}
	return;
	M_EPILOG
}

HUTF8String::HUTF8String( HUTF8String const& str_ )
	: _characterCount( str_._characterCount )
	, _offset( str_._offset )
	, _byteCount( str_._byteCount )
	, _ptr( str_._ptr ) {
	if ( _ptr ) {
		++ _meta->_refCount;
	}
	return;
}

HUTF8String::HUTF8String( HUTF8String&& str_ )
	: _characterCount( yaal::move( str_._characterCount ) )
	, _offset( yaal::move( str_._offset ) )
	, _byteCount( yaal::move( str_._byteCount ) )
	, _ptr( yaal::move( str_._ptr ) ) {
	str_._ptr = nullptr;
	str_.reset();
	return;
}

HUTF8String::~HUTF8String( void ) {
	M_PROLOG
	reset();
	return;
	M_DESTRUCTOR_EPILOG
}

void HUTF8String::reset( void ) {
	M_PROLOG
	M_ASSERT( ( _ptr == nullptr ) || ( ( _ptr != nullptr ) && ( _meta->_refCount > 0 ) ) );
	if ( _ptr != nullptr ) {
		-- _meta->_refCount;
		if ( _meta->_refCount == 0 ) {
			memory::free( _ptr );
		}
	}
	_ptr = nullptr;
	_byteCount = 0;
	_offset = 0;
	_characterCount = 0;
	return;
	M_EPILOG
}

void HUTF8String::swap( HUTF8String& other_ ) {
	M_PROLOG
	using yaal::swap;
	swap( _characterCount, other_._characterCount );
	swap( _offset, other_._offset );
	swap( _byteCount, other_._byteCount );
	swap( _ptr, other_._ptr );
	return;
	M_EPILOG
}

HUTF8String& HUTF8String::operator = ( HString const& str_ ) {
	M_PROLOG
	assign( str_ );
	return ( *this );
	M_EPILOG
}

HUTF8String& HUTF8String::operator = ( HUTF8String const& str_ ) {
	M_PROLOG
	reset();
	if ( str_._ptr ) {
		HUTF8String tmp( str_ );
		swap( tmp );
	}
	return ( *this );
	M_EPILOG
}

HUTF8String& HUTF8String::operator = ( HUTF8String&& other_ ) {
	swap( other_ );
	other_.reset();
	return ( *this );
}

bool HUTF8String::operator == ( yaal::hcore::HUTF8String const& other_ ) const {
	bool same( false );
	if ( ( _characterCount == other_._characterCount ) && ( _byteCount == other_._byteCount ) ) {
		if ( _ptr ) {
			same = ( !! other_._ptr ) && ( ::strncmp( _ptr + sizeof ( OBufferMeta ) + _offset, other_._ptr + sizeof ( OBufferMeta ) + other_._offset, static_cast<size_t>( _byteCount ) ) == 0 );
		} else {
			same = ! other_._ptr;
		}
	}
	return ( same );
}

bool HUTF8String::operator != ( yaal::hcore::HUTF8String const& other_ ) const {
	return ( ! operator == ( other_ ) );
}

bool HUTF8String::operator == ( char const* other_ ) const {
	int long byteCount( static_cast<int long>( ::strlen( other_ ) ) );
	bool same( false );
	if ( byteCount == _byteCount ) {
		if ( _ptr ) {
			same = other_ && ( ::strncmp( _ptr + sizeof ( OBufferMeta ) + _offset, other_, static_cast<size_t>( _byteCount ) ) == 0 );
		} else {
			same = ! other_;
		}
	}
	return ( same );
}

bool HUTF8String::operator != ( char const* other_ ) const {
	return ( ! operator == ( other_ ) );
}

void HUTF8String::alloc( int long size_ ) {
	M_PROLOG
	M_ASSERT( size_ > 0 );
	if ( _ptr && ( _meta->_refCount > 1 ) ) {
		reset();
	}
	int oldSize( _ptr ? _meta->_allocated : 0 );
	if ( size_ > oldSize ) {
		int long newSize( oldSize ? oldSize : 1 );
		while ( newSize < size_ ) {
			newSize *= 2;
		}
		int size( safe_int::cast<int>( newSize + 1 + static_cast<int>( sizeof ( OBufferMeta ) ) ) );
		_ptr = memory::realloc<char>( _ptr, size );
		::memset( _ptr + static_cast<int>( sizeof ( OBufferMeta ) ) + oldSize, 0, static_cast<size_t>( newSize - oldSize ) );
		_meta->_refCount = 1;
		_meta->_allocated = static_cast<int>( newSize );
	}
	_ptr[ static_cast<int>( sizeof ( OBufferMeta ) ) + size_ ] = 0;
	return;
	M_EPILOG
}

void HUTF8String::assign( HString::const_iterator it_, HString::const_iterator end_ ) {
	M_PROLOG
	u32_t maxCodePoint( 0 );
	int byteCount( 0 );
	for ( HString::const_iterator it( it_ ); it != end_; ++ it ) {
		byteCount += utf8::rank( static_cast<u8_t>( *it ) ); /* *FIXME* remove static_cast after implementation of UCS in HString is complete. */
		maxCodePoint = max( static_cast<u32_t>( static_cast<u8_t>( *it ) ), maxCodePoint ); /* *FIXME* remove static_cast after implementation of UCS in HString is complete. */
	}
	if ( byteCount > 0 ) {
		alloc( byteCount );
		char* p( _ptr + sizeof ( OBufferMeta ) );
		for ( HString::const_iterator it( it_ ); it != end_; ++ it ) {
			encode( static_cast<u8_t>( *it ), p ); /* *FIXME* remove static_cast after implementation of UCS in HString is complete. */
		}
	}
	if ( _ptr ) {
		_meta->_used = byteCount;
		_meta->_rank = static_cast<i8_t>( utf8::rank( maxCodePoint ) );
	}
	_characterCount = static_cast<int>( end_ - it_ );
	_offset = 0;
	_byteCount = byteCount;
	return;
	M_EPILOG
}

void HUTF8String::assign( HString const& str_ ) {
	M_PROLOG
	assign( str_.begin(), str_.end() );
	return;
	M_EPILOG
}

bool HUTF8String::is_empty( void ) const {
	return ( _characterCount == 0 );
}

bool HUTF8String::empty( void ) const {
	return ( _characterCount == 0 );
}

char const* HUTF8String::x_str( void ) const {
	M_ASSERT( ! _ptr || ( ( _offset + _byteCount ) == _meta->_used ) );
	return ( _ptr ? _ptr + sizeof ( OBufferMeta ) + _offset : "" );
}

char const* HUTF8String::raw( void ) const {
	return ( _ptr ? _ptr + sizeof ( OBufferMeta ) + _offset : "" );
}

int long HUTF8String::byte_count( void ) const {
	return ( _byteCount );
}

int long HUTF8String::character_count( void ) const {
	return ( _characterCount );
}

int HUTF8String::rank( void ) const {
	return ( _ptr ? _meta->_rank : 0 );
}

HUTF8String::HIterator HUTF8String::begin( void ) const {
	return ( HIterator( _ptr, _offset, 0 ) );
}

HUTF8String::HIterator HUTF8String::end( void ) const {
	return ( HIterator( _ptr, _offset + _byteCount, _characterCount ) );
}

HUTF8String::HIterator HUTF8String::cbegin( void ) const {
	return ( HIterator( _ptr, _offset, 0 ) );
}

HUTF8String::HIterator HUTF8String::cend( void ) const {
	return ( HIterator( _ptr, _offset + _byteCount, _characterCount ) );
}

HUTF8String::reverse_iterator HUTF8String::rbegin( void ) const {
	return ( end() );
}

HUTF8String::reverse_iterator HUTF8String::rend( void ) const {
	return ( begin() );
}

HUTF8String::reverse_iterator HUTF8String::crbegin( void ) const {
	return ( end() );
}

HUTF8String::reverse_iterator HUTF8String::crend( void ) const {
	return ( begin() );
}

HUTF8String HUTF8String::substr( int long from_, int long len_ ) const {
	if ( len_ == HString::npos ) {
		len_ = HString::MAX_STRING_LENGTH;
	}
	if ( from_ < 0 ) {
		from_ = 0;
	}
	if ( ( len_ + from_ ) > _characterCount ) {
		len_ = _characterCount - from_;
	}
	HIterator it( begin() );
	if ( from_ >= _characterCount ) {
		from_ = 0;
		len_ = 0;
		it = end();
	} else {
		it += from_;
	}
	HIterator endIt( len_ < ( _characterCount - from_ ) ? it + len_ : end() );
	HUTF8String s( *this );
	s._offset = it._byteIndex;
	s._byteCount = endIt._byteIndex - it._byteIndex;
	s._characterCount = safe_int::cast<int>( len_ );
	return ( s );
}

bool operator == ( char const* left_, HUTF8String const& right_ ) {
	M_PROLOG
	return ( right_.operator == ( left_ ) );
	M_EPILOG
}

bool operator != ( char const* left_, HUTF8String const& right_ ) {
	M_PROLOG
	return ( right_.operator != ( left_ ) );
	M_EPILOG
}

HUTF8String::HIterator::HIterator( void )
	: _ptr( nullptr )
	, _characterIndex( -1 )
	, _byteIndex( 0 ) {
	return;
}

HUTF8String::HIterator::HIterator( HIterator const& other_ )
	: _ptr( other_._ptr )
	, _characterIndex( other_._characterIndex )
	, _byteIndex( other_._byteIndex ) {
	if ( _ptr ) {
		++ _meta->_refCount;
	}
	return;
}

HUTF8String::HIterator::HIterator( HIterator&& other_ )
	: _ptr( yaal::move( other_._ptr ) )
	, _characterIndex( yaal::move( other_._characterIndex ) )
	, _byteIndex( yaal::move( other_._byteIndex ) ) {
	other_._ptr = nullptr;
	other_.reset();
	return;
}

HUTF8String::HIterator::HIterator( char* data_, int byteIndex_, int characterIndex_ )
	: _ptr( data_ )
	, _characterIndex( characterIndex_ )
	, _byteIndex( byteIndex_ ) {
	M_PROLOG
	if ( _ptr ) {
		++ _meta->_refCount;
	}
	return;
	M_EPILOG
}

HUTF8String::HIterator::~HIterator( void ) {
	M_PROLOG
	reset();
	return;
	M_DESTRUCTOR_EPILOG
}

HUTF8String::HIterator& HUTF8String::HIterator::operator = ( HIterator const& other_ ) {
	M_PROLOG
	reset();
	if ( other_._ptr ) {
		HIterator tmp( other_ );
		swap( tmp );
	}
	return ( *this );
	M_EPILOG
}

HUTF8String::HIterator& HUTF8String::HIterator::operator = ( HIterator&& other_ ) {
	M_PROLOG
	swap( other_ );
	other_.reset();
	return ( *this );
	M_EPILOG
}

void HUTF8String::HIterator::swap( HIterator& other_ ) {
	using yaal::swap;
	swap( _ptr, other_._ptr );
	swap( _characterIndex, other_._characterIndex );
	swap( _byteIndex, other_._byteIndex );
	return;
}

void HUTF8String::HIterator::reset( void ) {
	M_PROLOG
	M_ASSERT( ( _ptr == nullptr ) || ( ( _ptr != nullptr ) && ( _meta->_refCount > 0 ) ) );
	if ( _ptr ) {
		-- _meta->_refCount;
		if ( _meta->_refCount == 0 ) {
			memory::free( _ptr );
		}
	}
	_ptr = nullptr;
	_byteIndex = 0;
	_characterIndex = 0;
	return;
	M_EPILOG
}

HUTF8String::HIterator& HUTF8String::HIterator::operator ++ ( void ) {
	M_ASSERT( _ptr );
	char const* p( _ptr + sizeof ( HUTF8String::OBufferMeta ) + _byteIndex );
	int inc( 1 );
	if ( ! ( *p & utf8::ENC_1_BYTES_MARK_MASK ) ) {
	} else if ( ( *p & utf8::ENC_2_BYTES_MARK_MASK ) == utf8::ENC_2_BYTES_MARK_VALUE ) {
		inc = 2;
	} else if ( ( *p & utf8::ENC_3_BYTES_MARK_MASK ) == utf8::ENC_3_BYTES_MARK_VALUE ) {
		inc = 3;
	} else {
		inc = 4;
	}
	_byteIndex += inc;
	++ _characterIndex;
	return ( *this );
}

HUTF8String::HIterator& HUTF8String::HIterator::operator -- ( void ) {
	M_ASSERT( _ptr );
	-- _byteIndex;
	char const* p( _ptr + sizeof ( HUTF8String::OBufferMeta ) + _byteIndex );
	while ( ( *p & utf8::TAIL_BYTES_MARK_MASK ) == utf8::TAIL_BYTES_MARK_VALUE ) {
		-- _byteIndex;
		-- p;
	}
	-- _characterIndex;
	return ( *this );
}

HUTF8String::HIterator& HUTF8String::HIterator::operator += ( int long by_ ) {
	if ( by_ > 0 ) {
		while ( by_ -- ) {
			operator ++ ();
		}
	} else {
		while ( by_ ++ ) {
			operator -- ();
		}
	}
	return ( *this );
}

HUTF8String::HIterator& HUTF8String::HIterator::operator -= ( int long by_ ) {
	if ( by_ > 0 ) {
		while ( by_ -- ) {
			operator -- ();
		}
	} else {
		while ( by_ ++ ) {
			operator ++ ();
		}
	}
	return ( *this );
}

yaal::u32_t HUTF8String::HIterator::operator * ( void ) const {
	M_ASSERT( _ptr );
	char const* p( _ptr + sizeof ( HUTF8String::OBufferMeta ) + _byteIndex );
	u32_t character( static_cast<u8_t>( *p ) );
	if ( ! ( *p & utf8::ENC_1_BYTES_MARK_MASK ) ) {
	} else if ( ( *p & utf8::ENC_2_BYTES_MARK_MASK ) == utf8::ENC_2_BYTES_MARK_VALUE ) {
		character &= utf8::ENC_2_BYTES_VALUE_MASK;
		character <<= 6;
		++ p;
		character |= ( static_cast<u8_t>( *p ) & utf8::TAIL_BYTES_VALUE_MASK );
	} else if ( ( *p & utf8::ENC_3_BYTES_MARK_MASK ) == utf8::ENC_3_BYTES_MARK_VALUE ) {
		character &= utf8::ENC_3_BYTES_VALUE_MASK;
		for ( int i( 0 ); i < 2; ++ i ) {
			character <<= 6;
			++ p;
			character |= ( static_cast<u8_t>( *p ) & utf8::TAIL_BYTES_VALUE_MASK );
		}
	} else {
		character &= utf8::ENC_4_BYTES_VALUE_MASK;
		for ( int i( 0 ); i < 3; ++ i ) {
			character <<= 6;
			++ p;
			character |= ( static_cast<u8_t>( *p ) & utf8::TAIL_BYTES_VALUE_MASK );
		}
	}
	return ( character );
}

#undef SET_ALLOC_BYTES
#undef GET_ALLOC_BYTES
#undef EXT_SET_SIZE
#undef SET_SIZE
#undef GET_SIZE
#undef EXT_MEM
#undef MEM
#undef EXT_IS_INPLACE
#undef IS_INPLACE

HString operator "" _ys ( char const* str_, size_t len_ ) {
	return ( HString( str_, static_cast<int>( len_ ) ) );
}

HString to_string( char val_ ) {
	M_PROLOG
	return ( HString( val_ ) );
	M_EPILOG
}
HString to_string( char unsigned val_ ) {
	M_PROLOG
	return ( HString( val_ ) );
	M_EPILOG
}

HString to_string( int short val_ ) {
	M_PROLOG
	return ( HString( val_ ) );
	M_EPILOG
}

HString to_string( int short unsigned val_ ) {
	M_PROLOG
	return ( HString( val_ ) );
	M_EPILOG
}

HString to_string( int val_ ) {
	M_PROLOG
	return ( HString( val_ ) );
	M_EPILOG
}

HString to_string( int unsigned val_ ) {
	M_PROLOG
	return ( HString( val_ ) );
	M_EPILOG
}

HString to_string( int long val_ ) {
	M_PROLOG
	return ( HString( val_ ) );
	M_EPILOG
}

HString to_string( int long unsigned val_ ) {
	M_PROLOG
	return ( HString( val_ ) );
	M_EPILOG
}

HString to_string( int long long val_ ) {
	M_PROLOG
	return ( HString( val_ ) );
	M_EPILOG
}

HString to_string( int long long unsigned val_ ) {
	M_PROLOG
	return ( HString( val_ ) );
	M_EPILOG
}

HString to_string( float val_ ) {
	M_PROLOG
	return ( HString( val_ ) );
	M_EPILOG
}

HString to_string( double val_ ) {
	M_PROLOG
	return ( HString( val_ ) );
	M_EPILOG
}

HString to_string( double long val_ ) {
	M_PROLOG
	return ( HString( val_ ) );
	M_EPILOG
}

HString to_string( char const* val_ ) {
	M_PROLOG
	return ( HString( val_ ) );
	M_EPILOG
}

HString to_string( void const* val_ ) {
	M_PROLOG
	return ( HString( val_ ) );
	M_EPILOG
}

int stoi( HString const& str_, int* endIdx_, int base_ ) {
	M_PROLOG
	return ( safe_int::cast<int>( stoll( str_, endIdx_, base_ ) ) );
	M_EPILOG
}

int long stol( HString const& str_, int* endIdx_, int base_ ) {
	M_PROLOG
	return ( safe_int::cast<int long>( stoll( str_, endIdx_, base_ ) ) );
	M_EPILOG
}

int long unsigned stoul( HString const& str_, int* endIdx_, int base_ ) {
	M_PROLOG
	return ( safe_int::cast<int long unsigned>( stoull( str_, endIdx_, base_ ) ) );
	M_EPILOG
}

namespace hidden {

int copy_digits( HString const&, char*, int );

}

namespace {

static int const MAX_INTEGER_DIGIT_COUNT( 160 );

}

int long long stoll_impl( char const*, int*, int );
int long long stoll_impl( char const* str_, int* endIdx_, int base_ ) {
	M_PROLOG
	HScopedValueReplacement<int> saveErrno( errno, 0 );
	char* end( nullptr );
	int long long val( ::strtoll( str_, &end, base_ ) );
	if ( ! val || ( val == LLONG_MIN ) || ( val == LLONG_MAX ) ) {
		if ( ( errno == EINVAL ) || ( end == str_ ) ) {
			throw HInvalidArgumentException( "Invalid argument in conversion: "_ys.append( str_ ) );
		} else if ( errno == ERANGE ) {
			throw HOutOfRangeException( "Out of range value in conversion: "_ys.append( str_ ) );
		}
	}
	if ( endIdx_ )
		*endIdx_ = static_cast<int>( end - str_ );
	return ( val );
	M_EPILOG
}

int long long stoll( HString const& str_, int* endIdx_, int base_ ) {
	M_PROLOG
	char buf[MAX_INTEGER_DIGIT_COUNT];
	int skip( hidden::copy_digits( str_, buf, MAX_INTEGER_DIGIT_COUNT ) );
	int long long result( stoll_impl( buf, endIdx_, base_ ) );
	if ( endIdx_ ) {
		*endIdx_ += skip;
	}
	return ( result );
	M_EPILOG
}

int long long unsigned stoull_impl( char const*, int*, int );
int long long unsigned stoull_impl( char const* str_, int* endIdx_, int base_ ) {
	M_PROLOG
	HScopedValueReplacement<int> saveErrno( errno, 0 );
	char* end( nullptr );
	int long long unsigned val( ::strtoull( str_, &end, base_ ) );
	if ( ! val || ( val == ULLONG_MAX ) ) {
		if ( ( errno == EINVAL ) || ( end == str_ ) ) {
			throw HInvalidArgumentException( "Invalid argument in conversion: "_ys.append( str_ ) );
		} else if ( errno == ERANGE ) {
			throw HOutOfRangeException( "Out of range value in conversion: "_ys.append( str_ ) );
		}
	}
	if ( endIdx_ ) {
		*endIdx_ = static_cast<int>( end - str_ );
	}
	return ( val );
	M_EPILOG
}

int long long unsigned stoull( HString const& str_, int* endIdx_, int base_ ) {
	M_PROLOG
	char buf[MAX_INTEGER_DIGIT_COUNT];
	int skip( hidden::copy_digits( str_, buf, MAX_INTEGER_DIGIT_COUNT ) );
	int long long unsigned result( stoull_impl( buf, endIdx_, base_ ) );
	if ( endIdx_ ) {
		*endIdx_ += skip;
	}
	return ( result );
	M_EPILOG
}

float stof( HString const& str_, int* endIdx_ ) {
	return ( static_cast<float>( stold( str_, endIdx_ ) ) );
}

double stod( HString const& str_, int* endIdx_ ) {
	return ( static_cast<double>( stold( str_, endIdx_ ) ) );
}

double long stold( HString const& str_, int* endIdx_ ) {
	/*
	 * Try to call smart version of float parser if it is available.
	 *
	 * hcore::strtold can redirect call to either:
	 *
	 * ::strtold() - standard C library float parser
	 *
	 * - or -
	 *
	 * yaal::atof_ex - a full fledged expression parser (if it is available by linking with yaal::tools library)
	 */
	return ( hcore::strtold( str_, endIdx_ ) );
}

bool is_whitespace( u32_t char_ ) {
	return ( _whiteSpace_.has( char_ ) );
}

bool is_digit( u32_t char_ ) {
	return ( _digit_.has( char_ ) );
}

bool is_dec_digit( u32_t char_ ) {
	return ( _digit_.has( char_ ) );
}

bool is_hex_digit( u32_t char_ ) {
	return ( _hexDigit_.has( char_ ) );
}

bool is_oct_digit( u32_t char_ ) {
	return ( _octDigit_.has( char_ ) );
}

bool is_bin_digit( u32_t char_ ) {
	return ( _binDigit_.has( char_ ) );
}

bool is_letter( u32_t char_ ) {
	return ( _letter_.has( char_ ) );
}

bool is_alpha( u32_t char_ ) {
	return ( ( char_ > ' ' ) && ( char_ <= 127 ) );
}

namespace string_helper {

/* all str* and mem* functions takes const pointer as argument and returns
   non const pointer */
char* strrnpbrk( char const* buffer_,
		char const* stopSet_, int long length_ ) {
	M_PROLOG
	if ( length_ < 1 ) {
		return ( nullptr );
	}
	int long stopSetSize( static_cast<int long>( ::std::strlen( stopSet_ ) ) );
	int long index( length_ - 1 );
	while ( index >= 0 ) {
		if ( ::std::memchr( stopSet_, buffer_[ index ], static_cast<size_t>( stopSetSize ) ) ) {
			return ( const_cast<char*>( buffer_ + index ) );
		}
		-- index;
	}
	return ( nullptr );
	M_EPILOG
}

int long strrnspn( char const* buffer_, char const* skipSet_,
		int long length_ ) {
	M_PROLOG
	int long skipSetSize( static_cast<int long>( ::std::strlen( skipSet_ ) ) );
	int long index( length_ - 1 );
	while ( index >= 0 ) {
		if ( ! ::std::memchr( skipSet_, buffer_[ index ], static_cast<size_t>( skipSetSize ) ) ) {
			return ( index );
		}
		-- index;
	}
	return ( length_ );
	M_EPILOG
}

int long kmpsearch( char const* str, int long lenstr, char const* pat, int long lenpat ) {
	HChunk KMPnext( chunk_size<int>( lenpat + 1 ) );
	int* next( KMPnext.get<int>() );
	int b( next[ 0 ] = -1 );
	for ( int i = 1; i <= lenpat; ++ i ) {
		while ( ( b > -1 ) && ( pat[ b ] != pat[ i - 1 ] ) ) {
			b = next[ b ];
		}
		++ b;
		next[ i ] = ( pat[ i ] == pat[ b ] ) ? next[ b ] : b;
	}
	int start( -1 );
	b = 0;
	for ( int i = 0; i < lenstr; ++ i ) {
		while ( ( b > -1 ) && ( pat[ b ] != str[ i ] ) ) {
			b = next[ b ];
		}
		if ( ++ b < lenpat ) {
			continue;
		}
		start = i - b + 1;
		break;
	}
	return ( start );
}

#if ! defined( HAVE_STRCASESTR ) || ( HAVE_STRCASESTR == 0 )

int long kmpcasesearch( char const* str, int long lenstr, char const* pat, int long lenpat ) {
	HChunk KMPnext( chunk_size<int>( lenpat + 1 ) );
	int* next( KMPnext.get<int>() );
	int b( next[ 0 ] = -1 );
	for ( int i = 1; i <= lenpat; ++ i ) {
		while ( ( b > -1 ) && ( tolower( pat[ b ] ) != tolower( pat[ i - 1 ] ) ) ) {
			b = next[ b ];
		}
		++ b;
		next[ i ] = ( tolower( pat[ i ] ) == tolower( pat[ b ] ) ) ? next[ b ] : b;
	}
	int start( -1 );
	b = 0;
	for ( int i = 0; i < lenstr; ++ i ) {
		while ( ( b > -1 ) && ( tolower( pat[ b ] ) != tolower( str[ i ] ) ) ) {
			b = next[ b ];
		}
		if ( ++ b < lenpat ) {
			continue;
		}
		start = i - b + 1;
		break;
	}
	return ( start );
}

#endif /* #if ! defined( HAVE_STRCASESTR ) || ( HAVE_STRCASESTR == 0 ) */

}

int long icasesearch( HString const& haystack_, HString const& needle_ ) {
	char const* p(::strcasestr( haystack_.c_str(), needle_.c_str() ) );
	return ( p != nullptr ? p - haystack_.c_str() : HString::npos );
}

int stricasecmp( HString const& left, HString const& right ) {
	return ( ::strcasecmp( left.c_str(), right.c_str() ) );
}

}

}

