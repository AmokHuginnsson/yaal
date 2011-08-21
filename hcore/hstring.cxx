/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	hstring.cxx - this file is integral part of `yaal' project.

	i.  You may not make any changes in Copyright information.
	ii. You must attach Copyright information to any part of every copy
	    of this software.

Copyright:

 You are free to use this program as is, you can redistribute binary
 package freely but:
  1. You cannot use any part of sources of this software.
  2. You cannot redistribute any part of sources of this software.
  3. No reverse engineering is allowed.
  4. If you want redistribute binary package you cannot demand any fees
     for this software.
     You cannot even demand cost of the carrier (CD for example).
  5. You cannot include it to any commercial enterprise (for example 
     as a free add-on to payed software or payed newspaper).
 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE. Use it at your own risk.
*/

#include <cstdio>
#include <cstring>
#include <cctype>
#include <cstdarg>
#include <libintl.h>

#include "base.hxx"
M_VCSID( "$Id: "__ID__" $" )
M_VCSID( "$Id: "__TID__" $" )
#include "algorithm.hxx"
#include "hstring.hxx"
#include "memory.hxx"
#include "hchunk.hxx"

namespace yaal
{

namespace hcore
{

namespace string_helper
{

enum
	{
	OK = 0,
	NULL_PTR,
	UNINITIALIZED
	};

/* Useful helpers */
char * strrnpbrk( char const* const, char const* const,
		int long /* no const - used */ );
int long strrnspn( char const* const, char const* const, int long const );
int long kmpsearch( char const* const, int long, char const* const, int long );
}

char const _whiteSpace_[] = "\a\b \t\v\f\r\n";
#undef D_LETTER
#define D_LETTER "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
#undef D_DIGIT
#define D_DIGIT "0123456789"
char const _digit_[] = D_DIGIT;
char const _letter_[] = D_LETTER;
char const _word_[] = D_LETTER D_DIGIT "_";
#undef D_DIGIT
#undef D_LETTER

static int const ALLOC_BIT_MASK = 128;
#undef IS_INPLACE
#define IS_INPLACE ( ! ( _mem[ ALLOC_FLAG_INDEX ] & ALLOC_BIT_MASK ) )
#undef EXT_IS_INPLACE
#define EXT_IS_INPLACE( base ) ( ! ( base[ ALLOC_FLAG_INDEX ] & ALLOC_BIT_MASK ) )
#undef MEM
#define MEM ( IS_INPLACE ? _mem : *reinterpret_cast<char**>( _mem ) )
#undef EXT_MEM
#define EXT_MEM( base ) ( EXT_IS_INPLACE( base ) ? base : *reinterpret_cast<char**>( base ) )
#undef ROMEM
#define ROMEM ( IS_INPLACE ? _mem : *reinterpret_cast<char const* const*>( _mem ) )
#undef GET_SIZE
#define GET_SIZE ( IS_INPLACE ? _mem[ ALLOC_FLAG_INDEX ] : *reinterpret_cast<int long const*>( _mem + sizeof ( char* ) ) )
#undef SET_SIZE
#define SET_SIZE( size ) do { ( IS_INPLACE ? static_cast<int long>( _mem[ ALLOC_FLAG_INDEX ] = static_cast<char>( size ) ) : *reinterpret_cast<int long*>( _mem + sizeof ( char* ) ) = ( size ) ); } while ( 0 )
#undef EXT_SET_SIZE
#define EXT_SET_SIZE( base, size ) do { ( EXT_IS_INPLACE( base ) ? static_cast<int long>( base[ ALLOC_FLAG_INDEX ] = static_cast<char>( size ) ) : *reinterpret_cast<int long*>( base + sizeof ( char* ) ) = ( size ) ); } while ( 0 )
#undef GET_ALLOC_BYTES
#define GET_ALLOC_BYTES ( IS_INPLACE ? MAX_INPLACE_CAPACITY + 1 : static_cast<int long>( ( *reinterpret_cast<int long const*>( _mem + sizeof ( char* ) + sizeof ( int long ) ) ) & ( static_cast<int long unsigned>( -1 ) >> 1 ) ) )
#undef SET_ALLOC_BYTES
#define SET_ALLOC_BYTES( capacity ) do { ( *reinterpret_cast<int long*>( _mem + sizeof ( char* ) + sizeof ( int long ) ) = ( capacity ) ); _mem[ ALLOC_FLAG_INDEX ] = static_cast<char>( _mem[ ALLOC_FLAG_INDEX ] | ALLOC_BIT_MASK ); } while ( 0 )

char const* _errMsgHString_[ 3 ] =
	{
	_( "ok" ),
	_( "NULL pointer used for string operations" ),
	_( "use of uninitialized string" )
	};

HString::HString( void ) : _mem()
	{
	M_PROLOG
	return;
	M_EPILOG
	}

HString::~HString( void )
	{
	M_PROLOG
	if ( ! IS_INPLACE )
		memory::free( *reinterpret_cast<char**>( _mem ) );
	return;
	M_DESTRUCTOR_EPILOG
	}

HString::HString( HString const& string_ )
	: _mem()
	{
	M_PROLOG
	if ( ! string_.is_empty() )
		{
		int long newSize( string_.get_length() );
		hs_realloc( newSize + 1 );
		::std::strncpy( MEM, string_.raw(), newSize );
		SET_SIZE( newSize );
		}
	return;
	M_EPILOG
	}

HString::HString( int long preallocate_, bool )
	: _mem()
	{
	M_PROLOG
	hs_realloc( preallocate_ );
	::std::memset( MEM, 0, preallocate_ );
	return;
	M_EPILOG
	}

void HString::hs_realloc( int long preallocate_ )
	{
	M_PROLOG
	if ( ( preallocate_ < 1 ) || ( preallocate_ > ( MAX_STRING_LENGTH + 1 ) ) )
		M_THROW( _( "bad new buffer size requested" ), preallocate_ );
	int long oldAllocBytes( GET_ALLOC_BYTES );
	if ( preallocate_ > oldAllocBytes )
		{
		int long newAllocBytes = 1;
		while ( newAllocBytes < preallocate_ )
			newAllocBytes <<= 1;
		if ( ! IS_INPLACE )
			{
			*reinterpret_cast<char**>( _mem ) = memory::realloc<char>( MEM, newAllocBytes );
			SET_ALLOC_BYTES( newAllocBytes );
			::std::memset( MEM + oldAllocBytes, 0, newAllocBytes - oldAllocBytes );
			}
		else
			{
			char* newMem( memory::calloc<char>( newAllocBytes ) );
			int long origSize( GET_SIZE );
			::std::strncpy( newMem, _mem, origSize );
			*reinterpret_cast<char**>( _mem ) = newMem;
			SET_ALLOC_BYTES( newAllocBytes );
			SET_SIZE( origSize );
			}
		}
	return;
	M_EPILOG
	}

void HString::materialize( void )
	{
	M_PROLOG
	return;
	M_EPILOG
	}

HString::HString( char const* const str_ )
	: _mem()
	{
	M_PROLOG
	if ( str_ )
		{
		int long newSize( static_cast<int long>( ::std::strlen( str_ ) ) );
		hs_realloc( newSize + 1 );
		::std::strncpy( MEM, str_, newSize );
		SET_SIZE( newSize );
		}
	return;
	M_EPILOG
	}

HString::HString( char const* const array_, int long size_ )
	: _mem()
	{
	M_PROLOG
	if ( array_ )
		{
		int long newSize( min( static_cast<int long>( ::std::strlen( array_ ) ), size_ ) );
		hs_realloc( newSize + 1 );
		::std::strncpy( MEM, array_, newSize );
		MEM[ newSize ] = 0;
		SET_SIZE( newSize );
		}
	return;
	M_EPILOG
	}

HString::HString( char char_ ) : _mem()
	{
	M_PROLOG
	_mem[ 0 ] = char_;
	_mem[ 1 ] = 0;
	SET_SIZE( 1 );
	return;
	M_EPILOG
	}

HString::HString( char unsigned charUnsigned_ ) : _mem()
	{
	M_PROLOG
	_mem[ 0 ] = static_cast<char>( charUnsigned_ );
	_mem[ 1 ] = 0;
	SET_SIZE( 1 );
	return;
	M_EPILOG
	}

HString::HString( int short shortInt_ ) : _mem()
	{
	M_PROLOG
/* cppcheck-suppress nullPointer */
	int long newSize( ::snprintf( NULL, 0, "%hd", shortInt_ ) );
	hs_realloc( newSize + 1 );
	M_ENSURE( ::snprintf( MEM, newSize + 1, "%hd", shortInt_ ) == newSize );
	SET_SIZE( newSize );
	return;
	M_EPILOG
	}

HString::HString( int short unsigned unsignedShortInt_ ) : _mem()
	{
	M_PROLOG
/* cppcheck-suppress nullPointer */
	int long newSize( ::snprintf( NULL, 0, "%hu", unsignedShortInt_ ) );
	hs_realloc( newSize + 1 );
	M_ENSURE( ::snprintf( MEM, newSize + 1, "%hu", unsignedShortInt_ ) == newSize );
	SET_SIZE( newSize );
	return;
	M_EPILOG
	}

HString::HString( int int_ ) : _mem()
	{
	M_PROLOG
/* cppcheck-suppress nullPointer */
	int long newSize( ::snprintf( NULL, 0, "%d", int_ ) );
	hs_realloc( newSize + 1 );
	M_ENSURE( ::snprintf( MEM, newSize + 1, "%d", int_ ) == newSize );
	SET_SIZE( newSize );
	return;
	M_EPILOG
	}

HString::HString( int long long_ ) : _mem()
	{
	M_PROLOG
/* cppcheck-suppress nullPointer */
	int long newSize( ::snprintf( NULL, 0, "%ld", long_ ) );
	hs_realloc( newSize + 1 );
	M_ENSURE( ::snprintf( MEM, newSize + 1, "%ld", long_ ) == newSize );
	SET_SIZE( newSize );
	return;
	M_EPILOG
	}

HString::HString( int long unsigned long_ ) : _mem()
	{
	M_PROLOG
/* cppcheck-suppress nullPointer */
	int long newSize( ::snprintf( NULL, 0, "%lu", long_ ) );
	hs_realloc( newSize + 1 );
	M_ENSURE( ::snprintf( MEM, newSize + 1, "%lu", long_ ) == newSize );
	SET_SIZE( newSize );
	return;
	M_EPILOG
	}

HString::HString( int unsigned long_ ) : _mem()
	{
	M_PROLOG
/* cppcheck-suppress nullPointer */
	int long newSize( ::snprintf( NULL, 0, "%u", long_ ) );
	hs_realloc( newSize + 1 );
	M_ENSURE( ::snprintf( MEM, newSize + 1, "%u", long_ ) == newSize );
	SET_SIZE( newSize );
	return;
	M_EPILOG
	}

HString::HString( double double_ ) : _mem()
	{
	M_PROLOG
/* cppcheck-suppress nullPointer */
	int long newSize( ::snprintf( NULL, 0, "%f", double_ ) );
	hs_realloc( newSize + 1 );
	M_ENSURE( ::snprintf( MEM, newSize + 1, "%f", double_ ) == newSize );
	SET_SIZE( newSize );
	return;
	M_EPILOG
	}

HString::HString( double long double_ ) : _mem()
	{
	M_PROLOG
/* cppcheck-suppress nullPointer */
	int long newSize( ::snprintf( NULL, 0, "%.12Lf", double_ ) );
	hs_realloc( newSize + 1 );
	M_ENSURE( ::snprintf( MEM, newSize + 1, "%.12Lf", double_ ) == newSize );
	SET_SIZE( newSize );
	return;
	M_EPILOG
	}

HString::HString( void const* const ptrVoid_ )
	: _mem()
	{
	M_PROLOG
	/*
	 * Solaris libc omits 0x in %p conversion.
	 * Well, that sucks.
	 */
/* cppcheck-suppress nullPointer */
	int long newSize( ::snprintf( NULL, 0, "0x%lx", reinterpret_cast<int long>( ptrVoid_ ) ) );
	hs_realloc( newSize + 1 );
	M_ENSURE( ::snprintf( MEM, newSize + 1, "0x%lx", reinterpret_cast<int long>( ptrVoid_ ) ) == newSize );
	SET_SIZE( newSize );
	return;
	M_EPILOG
	}

HString& HString::operator = ( HString const& string_ )
	{
	M_PROLOG
	if ( this != &string_ )
		{
		int long newSize( string_.get_length() );
		if ( newSize >= GET_ALLOC_BYTES )
			hs_realloc( newSize + 1 );
		if ( newSize )
			::std::strncpy( MEM, string_.raw(), newSize );
		MEM[ newSize ] = 0;
		SET_SIZE( newSize );
		}
	return ( *this );
	M_EPILOG
	}

HString operator + ( HString const& string_, HString const& right )
	{
	M_PROLOG
	HString str( string_ );
	return ( str += right );
	M_EPILOG
	}

HString& HString::operator += ( HString const& string_ )
	{
	M_PROLOG
	int long otherSize( string_.get_length() );
	if ( otherSize )
		{
		int long oldSize( GET_SIZE );
		int long newSize( oldSize + otherSize );
		hs_realloc( newSize + 1 );
		::std::strcpy( MEM + oldSize, string_.raw() );
		SET_SIZE( newSize );
		}
	return ( *this );
	M_EPILOG
	}

char HString::operator[] ( int const index_ ) const
	{
	return ( operator[]( static_cast<int long>( index_ ) ) );
	}

char HString::operator[] ( int long const index_ ) const
	{
	M_PROLOG
	if ( index_ >= GET_ALLOC_BYTES )
		M_THROW( "index out of bound", index_ );
	return ( ROMEM[ index_ ] );
	M_EPILOG
	}

char HString::set_at( int long const index_, char char_ )
	{
	M_PROLOG
	if ( index_ >= GET_SIZE )
		M_THROW( "index out of bound", index_ );
	MEM[ index_ ] = char_;
	if ( ! char_ )
		SET_SIZE( index_ );
	return ( char_ );
	M_EPILOG
	}

bool operator == ( HString const& left, HString const& right )
	{
	M_PROLOG
	int long len = left.get_length();
	return ( ( &left == &right )
			|| ( ( len == right.get_length() )
				&& ( ! ( len && ::std::strcmp( left.raw(), right.raw() ) ) ) ) );
	M_EPILOG
	}

bool operator != ( HString const& left, HString const& right )
	{
	M_PROLOG
	return ( ! operator == ( left, right ) );
	M_EPILOG
	}

bool operator >= ( HString const& left, HString const& right )
	{
	M_PROLOG
	return ( ::std::strcoll( left.raw(), right.raw() ) >= 0 );
	M_EPILOG
	}

bool operator <= ( HString const& left, HString const& right )
	{
	M_PROLOG
	return ( ::std::strcoll( left.raw(), right.raw() ) <= 0 );
	M_EPILOG
	}

bool operator > ( HString const& left, HString const& right )
	{
	M_PROLOG
	return ( ! operator <= ( left, right ) );
	M_EPILOG
	}

bool operator < ( HString const& left, HString const& right )
	{
	M_PROLOG
	return ( ! operator >= ( left, right ) );
	M_EPILOG
	}

char const* HString::raw( void ) const
	{
	return ( ROMEM );
	}

char const* HString::c_str( void ) const
	{
	return ( ROMEM );
	}

HString::const_iterator HString::begin( void ) const
	{
	return ( ROMEM );
	}

HString::const_iterator HString::end( void ) const
	{
	return ( ROMEM + get_length() );
	}

bool HString::empty( void ) const
	{
	return ( is_empty() );
	}

bool HString::is_empty( void ) const
	{
	return ( ! GET_SIZE );
	}

bool HString::operator ! ( void ) const
	{
	return ( is_empty() );
	}

void HString::clear( void )
	{
	MEM[0] = 0;
	SET_SIZE( 0 );
	return;
	}

int long HString::get_length( void ) const
	{
	M_PROLOG
	M_ASSERT( ! GET_SIZE || ( GET_SIZE < GET_ALLOC_BYTES ) );
	return ( GET_SIZE );
	M_EPILOG
	}

int long HString::length( void ) const
	{
	M_PROLOG
	return ( get_length() );
	M_EPILOG
	}

int long HString::get_size( void ) const
	{
	M_PROLOG
	return ( get_length() );
	M_EPILOG
	}

int long HString::size( void ) const
	{
	M_PROLOG
	return ( get_length() );
	M_EPILOG
	}

int long HString::capacity( void ) const
	{
	M_PROLOG
	return ( get_capacity() );
	M_EPILOG
	}

int long HString::get_capacity( void ) const
	{
	M_PROLOG
	return ( GET_ALLOC_BYTES - 1 );
	M_EPILOG
	}

int long HString::get_max_size( void ) const
	{
	return ( MAX_STRING_LENGTH );
	}

int long HString::max_size( void ) const
	{
	return ( MAX_STRING_LENGTH );
	}

void HString::swap( HString& other )
	{
	if ( &other != this )
		{
		using yaal::swap_ranges;
		swap_ranges( _mem, _mem + INPLACE_BUFFER_SIZE, other._mem );
		}
	return;
	}

HString& HString::assign( char const* const data_, int long length_ )
	{
	M_PROLOG
	if ( ! data_ )
		M_THROW( _errMsgHString_[ string_helper::NULL_PTR ], errno );
	if ( length_ < 0 )
		M_THROW( _( "bad length" ), length_ );
	int long newSize( ::strnlen( data_, length_ ) );
	hs_realloc( newSize + 1 );
	::memcpy( MEM, data_, newSize );
	MEM[ newSize ] = 0;
	SET_SIZE( newSize );
	return ( *this );
	M_EPILOG
	}

HString& HString::format( char const* const format_, ... )
	{
	M_PROLOG
	::std::va_list ap;
	va_start( ap, format_ );
	try
		{
		vformat( format_, &ap );
		}
	catch ( ... )
		{
		va_end( ap );
		throw;
		}
	va_end( ap );
	return ( *this );
	M_EPILOG
	}

HString& HString::vformat( char const* const format_, void* ap_ )
	{
	M_PROLOG
	if ( ! format_ )
		M_THROW( _errMsgHString_[ string_helper::NULL_PTR ], errno );
	::std::va_list orig;
	__va_copy( orig, *static_cast< ::std::va_list*>( ap_ ) );
	int long newSize = vsnprintf( NULL, 0, format_, *static_cast< ::std::va_list*>( ap_ ) );
	hs_realloc( newSize + 1 );
	M_ENSURE ( vsnprintf( MEM, newSize + 1, format_, orig ) == newSize );
	SET_SIZE( newSize );
	va_end( orig );
	return ( *this );
	M_EPILOG
	}

int long HString::find( char char_, int long after_ ) const
	{
	M_PROLOG
	if ( after_ >= GET_SIZE )
		return ( -1 );
	if ( after_ < 0 )
		after_ = 0;
	char const* str = static_cast<char const*>( ::std::memchr( ROMEM + after_, char_, GET_SIZE - after_ ) );
	if ( ! str )
		return ( - 1 );
	return ( static_cast<int long>( str - ROMEM ) );
	M_EPILOG
	}

int long HString::find( HString const& pattern_, int long after_ ) const
	{
	M_PROLOG
	if ( pattern_.is_empty() )
		return ( -1 );
	return ( nfind( pattern_, pattern_.get_length(), after_ ) );
	M_EPILOG
	}

int long HString::nfind( HString const& pattern_, int long patternLength_, int long after_ ) const
	{
	M_PROLOG
	if ( pattern_.is_empty() )
		return ( -1 );
	if ( after_ < 0 )
		after_ = 0;
	if ( ( ! patternLength_ )
			|| ( GET_SIZE < ( after_ + patternLength_ ) ) )
		return ( -1 );
	int long idx = string_helper::kmpsearch( ROMEM + after_,
			GET_SIZE - after_, pattern_.raw(), patternLength_ );
	return ( idx >= 0 ? idx + after_ : -1 );
	M_EPILOG
	}

int long HString::find_one_of( char const* const set_,
		int long after_ ) const
	{
	M_PROLOG
	if ( ! set_ )
		return ( - 1 );
	if ( after_ < 0 )
		after_ = 0;
	if ( ( ! set_[0] )
			|| ( GET_SIZE <= after_ ) )
		return ( - 1 );
	char const* str( ::std::strpbrk( ROMEM + after_, set_ ) );
	if ( ! str )
		return ( - 1 );
	return ( static_cast<int long>( str - ROMEM ) );
	M_EPILOG
	}

int long HString::reverse_find_one_of( char const* const set_,
		int long before_ ) const
	{
	M_PROLOG
	if ( ! set_ )
		return ( - 1 );
	if ( before_ < 0 )
		before_ = 0;
	if ( ( GET_SIZE <= before_ ) || ( ! set_[0] ) )
		return ( - 1 );
	char* str( string_helper::strrnpbrk( ROMEM, set_, GET_SIZE - before_ ) );
	if ( ! str )
		return ( - 1 );
	return ( static_cast<int long>( ( GET_SIZE - 1 ) - ( str - ROMEM ) ) );
	M_EPILOG
	}

int long HString::find_last_one_of( char const* const set_,
		int long before_ ) const
	{
	M_PROLOG
	if ( ! set_ )
		return ( -1 );
	if ( before_ >= GET_SIZE )
		before_ = GET_SIZE - 1;
	if ( ( before_ < 0 ) || ( ! set_[0] ) )
		return ( -1 );
	char* str( string_helper::strrnpbrk( ROMEM, set_, before_ + 1 ) );
	if ( ! str )
		return ( -1 );
	return ( static_cast<int long>( str - ROMEM ) );
	M_EPILOG
	}

int long HString::find_other_than( char const* const set_,
		int long after_ ) const
	{
	M_PROLOG
	if ( ! set_ )
		return ( 0 );
	if ( after_ < 0 )
		after_ = 0;
	if ( after_ >= GET_SIZE )
		return ( -1 );
	if ( ! set_[0] )
		return ( after_ );
	int long index = static_cast<int long>( ::std::strspn( ROMEM + after_, set_ ) );
	if ( ( index + after_ ) >= GET_SIZE )
		return ( - 1 );
	return ( index + after_ );
	M_EPILOG
	}

int long HString::reverse_find_other_than( char const* const set_,
		int long before_ ) const
	{
	M_PROLOG
	if ( ! set_ )
		return ( 0 );
	if ( before_ < 0 )
		before_ = 0;
	if ( before_ >= GET_SIZE )
		return ( -1 );
	if ( ! set_[0] )
		return ( before_ );
	int long index( string_helper::strrnspn( ROMEM, set_, GET_SIZE - before_ ) );
	if ( index >= ( GET_SIZE - before_ ) )
		return ( - 1 );
	return ( ( GET_SIZE - 1 ) - index );
	M_EPILOG
	}

int long HString::find_last_other_than( char const* const set_,
		int long before_ ) const
	{
	M_PROLOG
	if ( ! set_ )
		return ( GET_SIZE - 1 );
	if ( before_ >= GET_SIZE )
		before_ = GET_SIZE - 1;
	if ( before_ < 0 )
		return ( -1 );
	if ( ! set_[0] )
		return ( before_ );
	int long index( string_helper::strrnspn( ROMEM, set_, before_ + 1 ) );
	if ( index > before_ )
		return ( -1 );
	return ( index );
	M_EPILOG
	}

int long HString::reverse_find( char char_, int long before_ ) const
	{
	M_PROLOG
	if ( before_ >= GET_SIZE )
		return ( -1 );
	if ( before_ < 0 )
		before_ = 0;
	char const* str = static_cast<char const*>( ::memrchr( ROMEM, char_, GET_SIZE - before_ ) );
	if ( ! str )
		return ( -1 );
	return ( static_cast<int long>( ( GET_SIZE - 1 ) - ( str - ROMEM ) ) );
	M_EPILOG
	}

int long HString::find_last( char char_, int long before_ ) const
	{
	M_PROLOG
	if ( before_ < 0 )
		return ( -1 );
	if ( before_ >= GET_SIZE )
		before_ = GET_SIZE - 1;
	char const* str( static_cast<char const*>( ::memrchr( ROMEM, char_, before_ + 1 ) ) );
	if ( ! str )
		return ( -1 );
	return ( static_cast<int long>( str - ROMEM ) );
	M_EPILOG
	}

HString& HString::replace( HString const& pattern_,
		HString const& with_ )
	{
	M_PROLOG
	if ( pattern_.is_empty() )
		return ( *this );
	int long lenPattern = pattern_.get_length();
	int long lenWith = with_.get_length();
	int long subWP = lenWith - lenPattern;
	int long index = 0;
	if ( subWP == 0 ) /* replacement is equals to pattern */
		{
		while ( ( index = find( pattern_, index ) ) > -1 )
			{
			::std::strncpy( MEM + index, with_.raw(), lenWith );
			index += lenPattern;
			}
		}
	else 
		{
		int long newSize( GET_SIZE );
		while ( ( index = find( pattern_, index ) ) > -1 )
			{
			newSize += subWP;
			index += lenPattern;
			}
		HString s;
		HString* src = NULL;
		if ( subWP > 0 ) /* replacement is longer than pattern */
			{
			s = *this;
			s.materialize();
			hs_realloc( newSize + 1 );
			src = &s;
			}
		else /* replacement is shorter than pattern */
			src = this;
		int long oldIdx = 0;
		int long newIdx = 0;
		index = 0;
		char const* with( with_.raw() );
		char const* srcBuf( src->raw() );
		while ( ( index = src->find( pattern_, index ) ) > -1 )
			{
			if ( newIdx && ( ( index - oldIdx ) != lenPattern ) )
				{
				::std::memmove( MEM + newIdx, srcBuf + oldIdx + lenPattern, ( index - oldIdx ) - lenPattern );
				newIdx += ( ( index - oldIdx ) - lenPattern );
				}
			else if ( ! newIdx )
				newIdx = index;
			oldIdx = index;
			::std::memmove( MEM + newIdx, with, lenWith );
			newIdx += lenWith;
			index += lenPattern;
			}
		if ( newIdx && ( ( GET_SIZE - oldIdx ) != lenPattern ) )
			::std::memmove( MEM + newIdx, srcBuf + oldIdx + lenPattern, ( GET_SIZE - oldIdx ) - lenPattern );
		using yaal::swap;
		SET_SIZE( newSize );
		MEM[ newSize ] = 0;
		}
	return ( *this );
	M_EPILOG
	}

HString& HString::upper( void )
	{
	M_PROLOG
	for ( int long ctr( 0 ), SIZE( GET_SIZE ); ctr < SIZE; ++ ctr )
		MEM[ ctr ] = static_cast<char>( ::std::toupper( MEM[ ctr ] ) );
	return ( *this );
	M_EPILOG
	}

HString& HString::lower( void )
	{
	M_PROLOG
	for ( int long ctr( 0 ), SIZE( GET_SIZE ); ctr < SIZE; ++ ctr )
		MEM[ ctr ] = static_cast<char>( ::std::tolower( MEM[ ctr ] ) );
	return ( *this );
	M_EPILOG
	}

HString& HString::reverse( void )
	{
	M_PROLOG
	char char_;
	for ( int long ctr( 0 ), SIZE( GET_SIZE ); ctr < SIZE; ++ ctr )
		{
		char_ = static_cast<char>( ::std::toupper( MEM[ ctr ] ) );
		if ( char_ == MEM[ ctr ] )
			MEM[ ctr ] = static_cast<char>( ::std::tolower( MEM[ ctr ] ) );
		else
			MEM[ ctr ] = char_;
		}
	return ( * this );
	M_EPILOG
	}

HString HString::left( int long to_ ) const
	{
	M_PROLOG
	HString str;
	if ( to_ < 1 )
		return ( str );
	int long newSize( min( to_, GET_SIZE ) );
	str.hs_realloc( newSize + 1 );
	::std::strncpy( EXT_MEM( str._mem ), ROMEM, newSize );
	EXT_MEM( str._mem )[ newSize ] = 0;
	EXT_SET_SIZE( str._mem, newSize );
	return ( str );
	M_EPILOG
	}

HString HString::mid( int long from_, int long length_ ) const
	{
	M_PROLOG
	HString str;
	if ( from_ < 0 )
		length_ += from_, from_ = 0;
	if ( ( length_ <= 0 ) || ( from_ >= GET_SIZE ) )
		return ( str );
	int long newSize( min( length_, GET_SIZE ) );
	if ( ( newSize + from_ ) > GET_SIZE )
		newSize = GET_SIZE - from_;
	str.hs_realloc( newSize + 1 );
	::std::strncpy( EXT_MEM( str._mem ), ROMEM + from_, newSize );
	EXT_MEM( str._mem )[ newSize ] = 0;
	EXT_SET_SIZE( str._mem, newSize );
	return ( str );
	M_EPILOG
	}

HString HString::right( int long fromEnd_ ) const
	{
	M_PROLOG
	HString str;
	if ( fromEnd_ < 1 )
		return ( str );
	int long newSize( min( fromEnd_, GET_SIZE ) );
	str.hs_realloc( newSize + 1 );
	::std::strncpy( EXT_MEM( str._mem ), ROMEM + GET_SIZE - newSize, newSize );
	EXT_MEM( str._mem )[ newSize ] = 0;
	EXT_SET_SIZE( str._mem, newSize );
	return ( str );
	M_EPILOG
	}

HString& HString::trim_left( char const* const set_ )
	{
	M_PROLOG
	int cut = 0;
	while ( MEM[ cut ] && ::std::strchr( set_, MEM[ cut ] ) )
		++ cut;
	if ( cut )
		shift_left( cut );
	return ( *this );
	M_EPILOG
	}

HString& HString::trim_right( char const* const set_ )
	{
	M_PROLOG
	int long cut = 0;
	while ( ( cut < GET_SIZE )
			&& ::std::strchr( set_, MEM[ GET_SIZE - ( cut + 1 ) ] ) )
		++ cut;
	if ( cut ) 
		{
		SET_SIZE( GET_SIZE - cut );
		MEM[ GET_SIZE ] = 0;
		}
	M_ASSERT( GET_SIZE >= 0 );
	return ( *this );
	M_EPILOG
	}

HString& HString::trim( char const* const set_ )
	{
	M_PROLOG
	trim_left( set_ );
	trim_right( set_ );
	return ( *this );
	M_EPILOG
	}

HString& HString::shift_left( int long shift_ )
	{
	M_PROLOG
	if ( shift_ < 0 )
		M_THROW( "bad left shift lenght", shift_ );
	if ( shift_ )
		{
		if ( shift_ < GET_SIZE )
			{
			SET_SIZE( GET_SIZE - shift_ );
			::std::memmove( MEM, MEM + shift_, GET_SIZE + 1 );
			}
		else
			clear();
		}
	return ( *this );
	M_EPILOG
	}

HString& HString::shift_right( int long shift_, char const filler_ )
	{
	M_PROLOG
	if ( shift_ < 0 )
		M_THROW( "bad right shift lenght", shift_ );
	if ( shift_ )
		{
		int long oldSize( GET_SIZE );
		int long newSize( oldSize + shift_ );
		hs_realloc( newSize + 1 );
		::std::memmove( MEM + shift_, MEM, oldSize + 1 );
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

HString& HString::fill( char filler_, int long offset_, int long count_ )
	{
	M_PROLOG
	if ( count_ < 0 )
		M_THROW( _( "bad length" ), count_ );
	if ( offset_ < 0 )
		M_THROW( _( "bad offset" ), offset_ );
	if ( ( offset_ + count_ ) >= GET_ALLOC_BYTES )
		M_THROW( _( "overflow" ), offset_ + count_ );
	if ( count_ == 0 )
		count_ = ( GET_ALLOC_BYTES - offset_ ) - 1; /* we maintain zero terminator (even though it is not fillz()) hence - 1 */
	if ( ( count_ + offset_ ) > GET_SIZE )
		SET_SIZE( count_ + offset_ );
	::std::memset( MEM + offset_, filler_, count_ );
	MEM[ GET_SIZE ] = 0;
	return ( *this );
	M_EPILOG
	}

HString& HString::fillz( char filler_, int long offset_, int long count_ )
	{
	M_PROLOG
	fill( filler_, offset_, count_ );
	MEM[ count_ + offset_ ] = 0;
	return ( *this );
	M_EPILOG
	}

HString& HString::erase( int long from_, int long length_ )
	{
	M_PROLOG
	if ( from_ < 0 )
		length_ += from_, from_ = 0;
	if ( ( from_ + length_ ) >= GET_SIZE )
		length_ = GET_SIZE - from_;
	if ( ( length_ > 0 ) && ( from_ < GET_SIZE ) )
		{
		::std::memmove( MEM + from_, MEM + from_ + length_, GET_SIZE - ( from_ + length_ ) );
		SET_SIZE( GET_SIZE - length_ );
		MEM[ GET_SIZE ] = 0;
		}
	return ( *this );
	M_EPILOG
	}

HString& HString::insert( int long from_, HString const& str_ )
	{
	M_PROLOG
	return ( insert( from_, str_.get_length(), str_.raw() ) );
	M_EPILOG
	}

HString& HString::insert( int long from_, int long length_, char const* chunk_ )
	{
	M_PROLOG
	if ( from_ < 0 )
		{
		if ( chunk_ )
			{
			if ( ( -from_ ) > static_cast<int long>( ::strnlen( chunk_, length_ ) ) )
				M_THROW( "negative offset caused chunk overflow", from_ );
			chunk_ += -from_;
			}
		length_ += from_;
		from_ = 0;
		}
	if ( ( length_ > 0 ) && ( from_ <= GET_SIZE ) )
		{
		int long chunkLen = chunk_ ? static_cast<int long>( ::strnlen( chunk_, length_ ) ) : 0;
		if ( chunk_ && ( length_ > chunkLen ) )
			M_THROW( "length too big for this chunk (by)", length_ - chunkLen );
		int long oldSize( GET_SIZE );
		int long newSize( oldSize + length_ );
		hs_realloc( newSize + 1 );
		::std::memmove( MEM + from_ + length_, MEM + from_, ( oldSize + 1 ) - from_ );
		if ( chunk_ )
			::std::strncpy( MEM + from_, chunk_, length_ );
		SET_SIZE( newSize );
		}
	return ( *this );
	M_EPILOG
	}

HString& HString::insert( int long from_, char const* chunk_ )
	{
	M_PROLOG
	return ( insert( from_, static_cast<int long>( ::strlen( chunk_ ) ), chunk_ ) );
	M_EPILOG
	}

HString& HString::insert( int long from_, char const* chunk_, int long length_ )
	{
	M_PROLOG
	return ( insert( from_, length_, chunk_ ) );
	M_EPILOG
	}

HString& HString::insert( int long from_, int long length_, char char_ )
	{
	M_PROLOG
	insert( from_, length_ );
	if ( ( from_ + length_ ) > 0 )
		fill( char_, from_, length_ );
	return ( *this );
	M_EPILOG
	}

HString& HString::append( HString const& str_ )
	{
	M_PROLOG
	return ( append( str_.raw(), str_.get_length() ) );
	M_EPILOG
	}

HString& HString::append( HString const& str_, int long idx_, int long len_ )
	{
	M_PROLOG
	M_ENSURE( len_ >= 0 );
	if ( ( len_ > 0 ) && ( idx_ < str_.get_length() ) )
		append( str_.raw() + ( idx_ >= 0 ? idx_ : 0 ),
				( ( idx_ + len_ ) < str_.get_length() ) ? len_ : GET_SIZE - idx_ );
	return ( *this );
	M_EPILOG
	}

HString& HString::append( int long count_, char val_ )
	{
	M_PROLOG
	int long oldSize( GET_SIZE );
	int long newSize( oldSize + count_ );
	hs_realloc( newSize + 1 );
	::memset( MEM + oldSize, val_, count_ );
	MEM[ newSize ] = 0;
	SET_SIZE( newSize );
	return ( *this );
	M_EPILOG
	}

HString& HString::append( char const* const buf_, int long len_ )
	{
	M_PROLOG
	M_ASSERT( len_ >= 0 );
	if ( len_ > 0 )
		{
		int long oldSize( GET_SIZE );
		int long newSize( oldSize + len_ );
		hs_realloc( newSize + 1 );
		::memmove( MEM + oldSize, buf_, len_ );
		MEM[ newSize ] = 0;
		SET_SIZE( newSize );
		}
	return ( *this );
	M_EPILOG
	}

#undef SET_ALLOC_BYTES
#undef GET_ALLOC_BYTES
#undef EXT_SET_SIZE
#undef SET_SIZE
#undef GET_SIZE
#undef ROMEM
#undef EXT_MEM
#undef MEM
#undef EXT_IS_INPLACE
#undef IS_INPLACE

int long hash( HString const& string_ )
	{
	char const* const buffer = string_.raw();
	int long size = string_.get_length();
	int long hash = 0;
	while ( size -- )
		{
		hash += buffer[ size ];
		hash <<= 3;
		}
	return ( hash );
	}

namespace string_helper
{

/* all str* and mem* functions takes const pointer as argument and returns
	 non const pointer */
char* strrnpbrk( char const* const buffer_,
		char const* const stopSet_, int long length_ )
	{
	M_PROLOG
	if ( length_ < 1 )
		return ( NULL );
	int long stopSetSize( static_cast<int long>( ::std::strlen( stopSet_ ) ) );
	int long index( length_ - 1 );
	while ( index >= 0 )
		{
		if ( ::std::memchr( stopSet_, buffer_[ index ], stopSetSize ) )
			return ( const_cast<char*>( buffer_ + index ) );
		-- index;
		}
	return ( NULL );
	M_EPILOG
	}

int long strrnspn( char const* const buffer_, char const* const skipSet_,
		int const long length_ )
	{
	M_PROLOG
	int long skipSetSize( static_cast<int long>( ::std::strlen( skipSet_ ) ) );
	int long index( length_ - 1 );
	while ( index >= 0 )
		{
		if ( ! ::std::memchr( skipSet_, buffer_[ index ], skipSetSize ) )
			return ( index );
		-- index;
		}
	return ( length_ );
	M_EPILOG
	}

int long kmpsearch( char const* const str, int long lenstr, char const* const pat, int long lenpat )
	{
	HChunk KMPnext( chunk_size<int>( lenpat + 1 ) );
	int* next = KMPnext.get<int>();
	int b = next[ 0 ] = -1;
	for ( int i = 1; i <= lenpat; ++ i )
		{
		while ( ( b > -1 ) && ( pat[ b ] != pat[ i - 1 ] ) )
			b = next[ b ];
		++ b;
		next[ i ] = ( pat[ i ] == pat[ b ] ) ? next[ b ] : b;
		}
	int start = -1;
	b = 0;
	for ( int i = 0; i < lenstr; ++ i )
		{
		while ( ( b > -1 ) && ( pat[ b ] != str[ i ] ) )
			b = next[ b ];
		if ( ++ b < lenpat )
			continue;
		start = i - b + 1;
		break;
		}
	return ( start );
	}

int long kmpcasesearch( char const* const str, int long lenstr, char const* const pat, int long lenpat )
	{
	HChunk KMPnext( chunk_size<int>( lenpat + 1 ) );
	int* next = KMPnext.get<int>();
	int b = next[ 0 ] = -1;
	for ( int i = 1; i <= lenpat; ++ i )
		{
		while ( ( b > -1 ) && ( tolower( pat[ b ] ) != tolower( pat[ i - 1 ] ) ) )
			b = next[ b ];
		++ b;
		next[ i ] = ( tolower( pat[ i ] ) == tolower( pat[ b ] ) ) ? next[ b ] : b;
		}
	int start = -1;
	b = 0;
	for ( int i = 0; i < lenstr; ++ i )
		{
		while ( ( b > -1 ) && ( tolower( pat[ b ] ) != tolower( str[ i ] ) ) )
			b = next[ b ];
		if ( ++ b < lenpat )
			continue;
		start = i - b + 1;
		break;
		}
	return ( start );
	}

}

int strcasecmp( HString const& left, HString const& right )
	{
	return ( ::strcasecmp( left.raw(), right.raw() ) );
	}

}

}

