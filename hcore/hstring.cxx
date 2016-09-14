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
#include "safe_cast.hxx"

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

bool HCharacterClass::has( char char_ ) const {
	return ( ::memchr( _data, char_, static_cast<size_t>( _size ) ) != nullptr );
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
		::std::strncpy( MEM, string_.raw(), static_cast<size_t>( newSize ) );
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
	_mem[ 0 ] = char_;
	_mem[ 1 ] = 0;
	SET_SIZE( 1 );
	return;
	M_EPILOG
}

HString::HString( char unsigned charUnsigned_ )
	: _len() {
	M_PROLOG
	_mem[ 0 ] = static_cast<char>( charUnsigned_ );
	_mem[ 1 ] = 0;
	SET_SIZE( 1 );
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
	M_ENSURE( ::snprintf( MEM, static_cast<size_t>( newSize + 1 ), "%f", double_ ) == newSize );
	SET_SIZE( newSize );
	return;
	M_EPILOG
}

HString::HString( double long double_ )
	: _len() {
	M_PROLOG
	int long newSize( ::snprintf( nullptr, 0, "%.12Lf", double_ ) );
	reserve( newSize );
	M_ENSURE( ::snprintf( MEM, static_cast<size_t>( newSize + 1 ), "%.12Lf", double_ ) == newSize );
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
			::std::strncpy( MEM, string_.raw(), static_cast<size_t>( newSize ) );
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
		::std::strcpy( MEM + oldSize, string_.raw() );
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
				&& ( ! ( GET_SIZE && ::std::strcmp( MEM, other_.raw() ) ) ) ) );
	M_EPILOG
}

bool HString::operator != (  HString const& other_ ) const {
	M_PROLOG
	return ( ! operator == ( other_ ) );
	M_EPILOG
}

bool HString::operator >= ( HString const& other_ ) const {
	M_PROLOG
	return ( ::std::strcoll( MEM, other_.raw() ) >= 0 );
	M_EPILOG
}

bool HString::operator <= ( HString const& other_ ) const {
	M_PROLOG
	return ( ::std::strcoll( MEM, other_.raw() ) <= 0 );
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

char const* HString::raw( void ) const {
	return ( MEM );
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
		::memcpy( MEM, str_.raw() + offset_, static_cast<size_t>( newSize ) );
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

HString& HString::format( char const* format_, ... ) {
	M_PROLOG
	::std::va_list ap;
	va_start( ap, format_ );
	try {
		vformat( format_, &ap );
	} catch ( ... ) {
		va_end( ap );
		throw;
	}
	va_end( ap );
	return ( *this );
	M_EPILOG
}

HString& HString::vformat( char const* format_, void* ap_ ) {
	M_PROLOG
	if ( ! format_ ) {
		M_THROW( _errMsgHString_[ string_helper::NULL_PTR ], errno );
	}
	::std::va_list orig;
	__va_copy( orig, *static_cast< ::std::va_list*>( ap_ ) );
	int long newSize = vsnprintf( nullptr, 0, format_, *static_cast< ::std::va_list*>( ap_ ) );
	reserve( newSize );
	M_ENSURE( vsnprintf( MEM, static_cast<size_t>( newSize + 1 ), format_, orig ) == newSize );
	SET_SIZE( newSize );
	va_end( orig );
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
			GET_SIZE - after_, pattern_.raw(), patternLength_ );
	return ( idx >= 0 ? idx + after_ : npos );
	M_EPILOG
}

int long HString::find_one_of( char const* set_, int long after_ ) const {
	M_PROLOG
	if ( ! set_ ) {
		return ( npos );
	}
	if ( after_ < 0 ) {
		after_ = 0;
	}
	if ( ( ! set_[0] )
			|| ( GET_SIZE <= after_ ) ) {
		return ( npos );
	}
	char const* str( ::std::strpbrk( MEM + after_, set_ ) );
	if ( ! str ) {
		return ( npos );
	}
	return ( static_cast<int long>( str - MEM ) );
	M_EPILOG
}

int long HString::reverse_find_one_of( char const* set_, int long before_ ) const {
	M_PROLOG
	if ( ! set_ ) {
		return ( npos );
	}
	if ( before_ < 0 ) {
		before_ = 0;
	}
	if ( ( GET_SIZE <= before_ ) || ( ! set_[0] ) ) {
		return ( npos );
	}
	char* str( string_helper::strrnpbrk( MEM, set_, GET_SIZE - before_ ) );
	if ( ! str ) {
		return ( npos );
	}
	return ( static_cast<int long>( ( GET_SIZE - 1 ) - ( str - MEM ) ) );
	M_EPILOG
}

int long HString::find_last_one_of( char const* set_, int long before_ ) const {
	M_PROLOG
	M_ASSERT( set_ != nullptr );
	if ( ( before_ == npos ) || ( before_ >= GET_SIZE ) ) {
		before_ = GET_SIZE - 1;
	}
	if ( ( before_ < 0 ) || ( ! set_[0] ) ) {
		return ( npos );
	}
	char* str( string_helper::strrnpbrk( MEM, set_, before_ + 1 ) );
	if ( ! str ) {
		return ( npos );
	}
	return ( static_cast<int long>( str - MEM ) );
	M_EPILOG
}

int long HString::find_other_than( char const* set_, int long after_ ) const {
	M_PROLOG
	if ( ! set_ ) {
		return ( 0 );
	}
	if ( after_ < 0 ) {
		after_ = 0;
	}
	if ( after_ >= GET_SIZE ) {
		return ( npos );
	}
	if ( ! set_[0] ) {
		return ( after_ );
	}
	int long index = static_cast<int long>( ::std::strspn( MEM + after_, set_ ) );
	if ( ( index + after_ ) >= GET_SIZE ) {
		return ( npos );
	}
	return ( index + after_ );
	M_EPILOG
}

int long HString::reverse_find_other_than( char const* set_, int long before_ ) const {
	M_PROLOG
	if ( ! set_ ) {
		return ( 0 );
	}
	if ( before_ < 0 ) {
		before_ = 0;
	}
	if ( before_ >= GET_SIZE ) {
		return ( npos );
	}
	if ( ! set_[0] ) {
		return ( before_ );
	}
	int long index( string_helper::strrnspn( MEM, set_, GET_SIZE - before_ ) );
	if ( index >= ( GET_SIZE - before_ ) ) {
		return ( npos );
	}
	return ( ( GET_SIZE - 1 ) - index );
	M_EPILOG
}

int long HString::find_last_other_than( char const* set_, int long before_ ) const {
	M_PROLOG
	M_ASSERT( set_ != nullptr );
	if ( ( before_ == npos ) || ( before_ >= GET_SIZE ) ) {
		before_ = GET_SIZE - 1;
	}
	if ( before_ < 0 ) {
		return ( npos );
	}
	if ( ! set_[0] ) {
		return ( before_ );
	}
	int long index( string_helper::strrnspn( MEM, set_, before_ + 1 ) );
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
	int long index( 0 );
	if ( subWP == 0 ) { /* replacement is equal length to pattern */
		while ( ( index = find( pattern_, index ) ) != npos ) {
			::std::strncpy( MEM + index, with_.raw(), static_cast<size_t>( lenWith ) );
			index += lenPattern;
		}
	} else {
		int long newSize( GET_SIZE );
		while ( ( index = find( pattern_, index ) ) != npos ) {
			newSize += subWP;
			index += lenPattern;
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
		int long oldIdx( 0 );
		int long newIdx( 0 );
		index = 0;
		char const* with( with_.raw() );
		char const* srcBuf( src->raw() );
		while ( ( index = src->find( pattern_, index ) ) != npos ) {
			if ( newIdx && ( ( index - oldIdx ) != lenPattern ) ) {
				::std::memmove( MEM + newIdx, srcBuf + oldIdx + lenPattern, static_cast<size_t>( ( index - oldIdx ) - lenPattern ) );
				newIdx += ( ( index - oldIdx ) - lenPattern );
			} else if ( ! newIdx ) {
				newIdx = index;
			}
			oldIdx = index;
			::std::memmove( MEM + newIdx, with, static_cast<size_t>( lenWith ) );
			newIdx += lenWith;
			index += lenPattern;
		}
		if ( newIdx && ( ( GET_SIZE - oldIdx ) != lenPattern ) ) {
			::std::memmove( MEM + newIdx, srcBuf + oldIdx + lenPattern, static_cast<size_t>( ( GET_SIZE - oldIdx ) - lenPattern ) );
		}
		SET_SIZE( newSize );
		MEM[ newSize ] = 0;
	}
	return ( *this );
	M_EPILOG
}

HString& HString::replace( int long pos_, int long size_, HString const& replacement ) {
	M_PROLOG
	return ( replace( pos_, size_, replacement.raw(), replacement.get_length() ) );
	M_EPILOG
}

HString& HString::replace( int long pos_, int long size_, HString const& replacement, int long offset_, int long len_ ) {
	M_PROLOG
	if ( offset_ < 0 ) {
		M_THROW( _errMsgHString_[string_helper::BAD_OFFSET], offset_ );
	}
	return ( replace( pos_, size_, replacement.raw() + offset_, len_ ) );
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

HString& HString::trim_left( char const* set_ ) {
	M_PROLOG
	int cut( 0 );
	while ( MEM[ cut ] && ::std::strchr( set_, MEM[ cut ] ) ) {
		++ cut;
	}
	if ( cut ) {
		shift_left( cut );
	}
	return ( *this );
	M_EPILOG
}

HString& HString::trim_right( char const* set_ ) {
	M_PROLOG
	int long cut( 0 );
	while ( ( cut < GET_SIZE )
			&& ::std::strchr( set_, MEM[ GET_SIZE - ( cut + 1 ) ] ) ) {
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

HString& HString::trim( char const* set_ ) {
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
	return ( insert( from_, str_.get_length(), str_.raw() ) );
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
	return ( append( str_.raw(), str_.get_length() ) );
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
			str_.raw() + ( idx_ >= 0 ? idx_ : 0 ),
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
	return ( safe_cast<int>( stoll( str_, endIdx_, base_ ) ) );
	M_EPILOG
}

int long stol( HString const& str_, int* endIdx_, int base_ ) {
	M_PROLOG
	return ( safe_cast<int long>( stoll( str_, endIdx_, base_ ) ) );
	M_EPILOG
}

int long unsigned stoul( HString const& str_, int* endIdx_, int base_ ) {
	M_PROLOG
	return ( safe_cast<int long unsigned>( stoull( str_, endIdx_, base_ ) ) );
	M_EPILOG
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
	return ( stoll_impl( str_.raw(), endIdx_, base_ ) );
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
	return ( stoull_impl( str_.raw(), endIdx_, base_ ) );
	M_EPILOG
}

float stof( HString const& str_, int* endIdx_ ) {
	return ( static_cast<float>( stold( str_, endIdx_ ) ) );
}

double stod( HString const& str_, int* endIdx_ ) {
	return ( static_cast<double>( stold( str_, endIdx_ ) ) );
}

double long stold( HString const& str_, int* endIdx_ ) {
	return ( hcore::strtold( str_, endIdx_ ) );
}

bool is_whitespace( char char_ ) {
	return ( _whiteSpace_.has( char_ ) );
}

bool is_digit( char char_ ) {
	return ( _digit_.has( char_ ) );
}

bool is_dec_digit( char char_ ) {
	return ( _digit_.has( char_ ) );
}

bool is_hex_digit( char char_ ) {
	return ( _hexDigit_.has( char_ ) );
}

bool is_oct_digit( char char_ ) {
	return ( _octDigit_.has( char_ ) );
}

bool is_bin_digit( char char_ ) {
	return ( _binDigit_.has( char_ ) );
}

bool is_letter( char char_ ) {
	return ( _letter_.has( char_ ) );
}

bool is_alpha( char char_ ) {
	char unsigned c( static_cast<char unsigned>( char_ ) );
	return ( ( c > ' ' ) && ( c <= 127 ) );
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

int strcasecmp( HString const& left, HString const& right ) {
	return ( ::strcasecmp( left.raw(), right.raw() ) );
}

}

}

