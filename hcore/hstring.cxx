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
#include "xalloc.hxx"
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
int long kmpsearch( char const* const, int long const&, char const* const, int long const& );
}

char const _whiteSpace_[] = "\a\b \t\v\f\r\n";
#define D_LETTER "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
#define D_DIGIT "0123456789"
char const _digit_[] = D_DIGIT;
char const _letter_[] = D_LETTER;
char const _word_[] = D_LETTER D_DIGIT "_";
#undef D_DIGIT
#undef D_LETTER

char const* _errMsgHString_[ 3 ] =
	{
	_( "ok" ),
	_( "NULL pointer used for string operations" ),
	_( "use of uninitialized string" )
	};

HString::HString( void ) : _buffer( NULL ), _allocatedBytes( 0 ), _size( 0 )
	{
	M_PROLOG
	return;
	M_EPILOG
	}

HString::~HString( void )
	{
	M_PROLOG
	if ( _buffer )
		xfree( _buffer );
	return;
	M_EPILOG
	}

HString::HString( HString const& string_ )
	: _buffer( NULL ), _allocatedBytes( 0 ), _size( string_._size )
	{
	M_PROLOG
	if ( _size )
		{
		hs_realloc( _size + 1 );
		::std::strcpy( _buffer, string_._buffer );
		}
	return;
	M_EPILOG
	}

HString::HString( int const preallocate_, bool const )
	: _buffer( NULL ), _allocatedBytes( 0 ), _size( 0 )
	{
	M_PROLOG
	hs_realloc( preallocate_ );
	::std::memset( _buffer, 0, preallocate_ );
	return;
	M_EPILOG
	}

void HString::hs_realloc( int long const preallocate_ )
	{
	M_PROLOG
	if ( preallocate_ < 1 )
		M_THROW( _( "bad new buffer size requested" ), preallocate_ );
	if ( preallocate_ > _allocatedBytes )
		{
		int long oldAllocation = _allocatedBytes;
		_allocatedBytes = 1;
		while ( _allocatedBytes < preallocate_ )
			_allocatedBytes <<= 1;
		_buffer = xrealloc<char>( _buffer, _allocatedBytes );
		::std::memset( _buffer + oldAllocation, 0, _allocatedBytes - oldAllocation );
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
	: _buffer ( NULL ), _allocatedBytes ( 0 ), _size( 0 )
	{
	M_PROLOG
	if ( str_ )
		{
		_size = static_cast<int long>( ::std::strlen( str_ ) );
		hs_realloc( _size + 1 );
		::std::strcpy( _buffer, str_ );
		}
	return;
	M_EPILOG
	}

HString::HString( char const char_ ) : _buffer( NULL ), _allocatedBytes( 0 ), _size( 1 )
	{
	M_PROLOG
	hs_realloc( 2 );
	_buffer[ 0 ] = char_;
	_buffer[ 1 ] = 0;
	return;
	M_EPILOG
	}

HString::HString( int const int_ ) : _buffer( NULL ), _allocatedBytes( 0 ), _size( 0 )
	{
	M_PROLOG
	_size = ::snprintf( NULL, 0, "%d", int_ );
	hs_realloc( _size + 1 );
	M_ENSURE( ::snprintf( _buffer, _size + 1, "%d", int_ ) == _size );
	return;
	M_EPILOG
	}

HString::HString( int long const long_ ) : _buffer( NULL ), _allocatedBytes( 0 ), _size( 0 )
	{
	M_PROLOG
	_size = ::snprintf( NULL, 0, "%ld", long_ );
	hs_realloc( _size + 1 );
	M_ENSURE( ::snprintf( _buffer, _size + 1, "%ld", long_ ) == _size );
	return;
	M_EPILOG
	}

HString::HString( int long unsigned const long_ ) : _buffer( NULL ), _allocatedBytes( 0 ), _size( 0 )
	{
	M_PROLOG
	_size = ::snprintf( NULL, 0, "%lu", long_ );
	hs_realloc( _size + 1 );
	M_ENSURE( ::snprintf( _buffer, _size + 1, "%lu", long_ ) == _size );
	return;
	M_EPILOG
	}

HString::HString( int unsigned const long_ ) : _buffer( NULL ), _allocatedBytes( 0 ), _size( 0 )
	{
	M_PROLOG
	_size = ::snprintf( NULL, 0, "%u", long_ );
	hs_realloc( _size + 1 );
	M_ENSURE( ::snprintf( _buffer, _size + 1, "%u", long_ ) == _size );
	return;
	M_EPILOG
	}

HString::HString( double const double_ ) : _buffer( NULL ), _allocatedBytes( 0 ), _size( 0 )
	{
	M_PROLOG
	_size = ::snprintf( NULL, 0, "%f", double_ );
	hs_realloc( _size + 1 );
	M_ENSURE( ::snprintf( _buffer, _size + 1, "%f", double_ ) == _size );
	return;
	M_EPILOG
	}

HString::HString( double long const double_ ) : _buffer( NULL ), _allocatedBytes( 0 ), _size( 0 )
	{
	M_PROLOG
	_size = ::snprintf( NULL, 0, "%.12Lf", double_ );
	hs_realloc( _size + 1 );
	M_ENSURE( ::snprintf( _buffer, _size + 1, "%.12Lf", double_ ) == _size );
	return;
	M_EPILOG
	}

HString::HString( void const* const ptrVoid_ )
	: _buffer( NULL ), _allocatedBytes( 0 ), _size( 0 )
	{
	M_PROLOG
	/*
	 * Solaris libc omits 0x in %p conversion.
	 * Well, that sucks.
	 */
	_size = ::snprintf( NULL, 0, "0x%lx", reinterpret_cast<int long>( ptrVoid_ ) );
	hs_realloc( _size + 1 );
	M_ENSURE( ::snprintf( _buffer, _size + 1, "0x%lx", reinterpret_cast<int long>( ptrVoid_ ) ) == _size );
	return;
	M_EPILOG
	}

HString& HString::operator = ( HString const& string_ )
	{
	M_PROLOG
	if ( this != &string_ )
		{
		_size = string_._size;
		if ( _size >= _allocatedBytes )
			hs_realloc( _size + 1 );
		if ( _size )
			::std::strcpy( _buffer, string_._buffer );
		if( _buffer )
			_buffer[ _size ] = 0;
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
	if ( string_._size )
		{
		int long oldLength = _size;
		_size += string_._size;
		hs_realloc( _size + 1 );
		::std::strcpy( _buffer + oldLength, string_._buffer );
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
	if ( index_ >= _allocatedBytes )
		M_THROW( "index out of bound", index_ );
	return ( _buffer[ index_ ] );
	M_EPILOG
	}

char HString::set_at( int long const index_, char char_ )
	{
	M_PROLOG
	if ( index_ >= _size )
		M_THROW( "index out of bound", index_ );
	_buffer[ index_ ] = char_;
	if ( ! char_ )
		_size = index_;
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
	return ( _buffer ? _buffer : "" );
	}

char const* HString::c_str( void ) const
	{
	return ( _buffer ? _buffer : "" );
	}

HString::const_iterator HString::begin( void ) const
	{
	return ( _buffer );
	}

HString::const_iterator HString::end( void ) const
	{
	return ( _buffer + get_length() );
	}

bool HString::empty( void ) const
	{
	return ( is_empty() );
	}

bool HString::is_empty( void ) const
	{
	return ( ! _size );
	}

bool HString::operator ! ( void ) const
	{
	return ( is_empty() );
	}

void HString::clear( void )
	{
	if ( _buffer )
		_buffer[ 0 ] = 0;
	_size = 0;
	return;
	}

int long HString::get_length( void ) const
	{
	M_PROLOG
	M_ASSERT( ( ! _buffer && ( _size == 0 ) && ( _allocatedBytes == 0 ) ) || ( _size < _allocatedBytes ) );
	return ( _size );
	M_EPILOG
	}

int long HString::length( void ) const
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
	return ( _allocatedBytes - 1 );
	M_EPILOG
	}

void HString::swap( HString& other )
	{
	if ( &other != this )
		{
		using yaal::swap;
		swap( _buffer, other._buffer );
		swap( _allocatedBytes, other._allocatedBytes );
		swap( _size, other._size );
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
	_size = length_;
	hs_realloc( _size + 1 );
	::memcpy( _buffer, data_, _size );
	_buffer[ _size ] = 0;
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
	_size = vsnprintf( NULL, 0, format_, *static_cast< ::std::va_list*>( ap_ ) );
	hs_realloc( _size + 1 );
	M_ENSURE ( vsnprintf( _buffer, _size + 1, format_, orig ) == _size );
	va_end( orig );
	return ( *this );
	M_EPILOG
	}

int long HString::find( char const char_, int long after_ ) const
	{
	M_PROLOG
	if ( after_ >= _size )
		return ( -1 );
	if ( after_ < 0 )
		after_ = 0;
	char* str = static_cast<char*>( ::std::memchr( _buffer + after_, char_, _size - after_ ) );
	if ( ! str )
		return ( - 1 );
	return ( static_cast<int long>( str - _buffer ) );
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
			|| ( _size < ( after_ + patternLength_ ) ) )
		return ( -1 );
	int long idx = string_helper::kmpsearch( _buffer + after_,
			_size - after_, pattern_.raw(), patternLength_ );
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
	if ( ( ! ::std::strlen( set_ ) )
			|| ( _size <= after_ ) )
		return ( - 1 );
	char * str = ::std::strpbrk( _buffer + after_, set_ );
	if ( ! str )
		return ( - 1 );
	return ( static_cast<int long>( str - _buffer ) );
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
	if ( ( ! ::std::strlen( set_ ) ) || ( _size <= before_ ) )
		return ( - 1 );
	char* str = string_helper::strrnpbrk( _buffer,
			set_, _size - before_ );
	if ( ! str )
		return ( - 1 );
	return ( static_cast<int long>( ( _size - 1 ) - ( str - _buffer ) ) );
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
	if ( ! ::std::strlen( set_ ) )
		return ( 0 );
	if ( _size <= after_ )
		return ( -1 );
	int long index = static_cast<int long>( ::std::strspn( _buffer + after_, set_ ) );
	if ( ( index + after_ ) >= _size )
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
	if ( ! ::std::strlen( set_ ) )
		return ( 0 );
	if ( _size <= before_ )
		return ( -1 );
	int long index = string_helper::strrnspn( _buffer,
			set_, _size - before_ );
	if ( index >= ( _size - before_ ) )
		return ( - 1 );
	return ( ( _size - 1 ) - index );
	M_EPILOG
	}

int long HString::reverse_find( char const char_, int long before_ ) const
	{
	M_PROLOG
	if ( before_ >= _size )
		return ( -1 );
	if ( before_ < 0 )
		before_ = 0;
	char* str = static_cast<char*>( ::memrchr( _buffer, char_, _size - before_ ) );
	if ( ! str )
		return ( -1 );
	return ( static_cast<int long>( ( _size - 1 ) - ( str - _buffer ) ) );
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
			::std::strncpy( _buffer + index, with_.raw(), lenWith );
			index += lenPattern;
			}
		}
	else 
		{
		int long newSize = _size;
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
		while ( ( index = src->find( pattern_, index ) ) > -1 )
			{
			if ( newIdx && ( ( index - oldIdx ) != lenPattern ) )
				{
				::std::memmove( _buffer + newIdx, src->_buffer + oldIdx + lenPattern, ( index - oldIdx ) - lenPattern );
				newIdx += ( ( index - oldIdx ) - lenPattern );
				}
			else if ( ! newIdx )
				newIdx = index;
			oldIdx = index;
			::std::memmove( _buffer + newIdx, with_.raw(), lenWith );
			newIdx += lenWith;
			index += lenPattern;
			}
		if ( newIdx && ( ( _size - oldIdx ) != lenPattern ) )
			::std::memmove( _buffer + newIdx, src->_buffer + oldIdx + lenPattern, ( _size - oldIdx ) - lenPattern );
		using yaal::swap;
		swap( _size, newSize );
		}
	_buffer[ _size ] = 0;
	return ( *this );
	M_EPILOG
	}

HString& HString::upper( void )
	{
	M_PROLOG
	for ( int long ctr = 0; ctr < _size; ctr++ )
		_buffer[ ctr ] = static_cast<char>( ::std::toupper( _buffer[ ctr ] ) );
	return ( *this );
	M_EPILOG
	}

HString& HString::lower( void )
	{
	M_PROLOG
	for ( int long ctr = 0; ctr < _size; ctr++ )
		_buffer[ ctr ] = static_cast<char>( ::std::tolower( _buffer[ ctr ] ) );
	return ( *this );
	M_EPILOG
	}

HString& HString::reverse( void )
	{
	M_PROLOG
	char char_;
	for ( int long ctr = 0; ctr < _size; ctr++ )
		{
		char_ = static_cast<char>( ::std::toupper( _buffer[ ctr ] ) );
		if ( char_ == _buffer[ ctr ] )
			_buffer[ ctr ] = static_cast<char>( ::std::tolower( _buffer[ ctr ] ) );
		else
			_buffer[ ctr ] = char_;
		}
	return ( * this );
	M_EPILOG
	}

HString HString::left( int long const& to_ ) const
	{
	M_PROLOG
	HString str;
	if ( to_ < 1 )
		return ( str );
	str._size = ( to_ < _size ? to_ : _size );
	str.hs_realloc( str._size + 1 );
	::std::strncpy( str._buffer, _buffer, str._size );
	str._buffer[ str._size ] = 0;
	return ( str );
	M_EPILOG
	}

HString HString::mid( int long from_, int long length_ ) const
	{
	M_PROLOG
	HString str;
	if ( from_ < 0 )
		length_ += from_, from_ = 0;
	if ( ( length_ <= 0 ) || ( from_ >= _size ) )
		return ( str );
	str._size = ( length_ < _size ? length_ : _size );
	if ( ( str._size + from_ ) > _size )
		str._size = _size - from_;
	str.hs_realloc( str._size + 1 );
	::std::strncpy( str._buffer, _buffer + from_, str._size );
	str._buffer[ str._size ] = 0;
	return ( str );
	M_EPILOG
	}

HString HString::right( int long const& fromEnd_ ) const
	{
	M_PROLOG
	HString str;
	if ( fromEnd_ < 1 )
		return ( str );
	str._size = ( fromEnd_ < _size ? fromEnd_ : _size );
	str.hs_realloc( str._size + 1 );
	::std::strncpy( str._buffer, _buffer + _size - str._size,
			str._size );
	str._buffer[ str._size ] = 0;
	return ( str );
	M_EPILOG
	}

HString& HString::trim_left( char const* const set_ )
	{
	M_PROLOG
	M_ASSERT( _buffer );
	int cut = 0;
	while ( _buffer[ cut ] && ::std::strchr( set_, _buffer[ cut ] ) )
		++ cut;
	if ( cut )
		shift_left( cut );
	return ( *this );
	M_EPILOG
	}

HString& HString::trim_right( char const* const set_ )
	{
	M_PROLOG
	M_ASSERT( _buffer );
	int long cut = 0;
	while ( ( cut < _size )
			&& ::std::strchr( set_, _buffer[ _size - ( cut + 1 ) ] ) )
		++ cut;
	if ( cut ) 
		{
		_size -= cut;
		_buffer[ _size ] = 0;
		}
	M_ASSERT( _size >= 0 );
	return ( *this );
	M_EPILOG
	}

HString& HString::shift_left( int long const& shift_ )
	{
	M_PROLOG
	if ( shift_ < 0 )
		M_THROW( "bad left shift lenght", shift_ );
	if ( shift_ )
		{
		if ( shift_ < _size )
			{
			_size -= shift_;
			::std::memmove( _buffer, _buffer + shift_, _size + 1 );
			}
		else
			{
			_size = 0;
			_buffer[ _size ] = 0;
			}
		}
	return ( *this );
	M_EPILOG
	}

HString& HString::shift_right( int long const& shift_, char const filler_ )
	{
	M_PROLOG
	if ( shift_ < 0 )
		M_THROW( "bad right shift lenght", shift_ );
	if ( shift_ )
		{
		int long oldSize = _size;
		_size += shift_;
		hs_realloc( _size + 1 );
		::std::memmove( _buffer + shift_, _buffer, oldSize + 1 );
		fill( filler_, 0, shift_ );
		_size = oldSize + shift_;
		}
	return ( * this );
	M_EPILOG
	}

HString& HString::fill( char filler_, int long offset_, int long count_ )
	{
	M_PROLOG
	if ( count_ < 0 )
		M_THROW( _( "bad length" ), count_ );
	if ( offset_ < 0 )
		M_THROW( _( "bad offset" ), offset_ );
	if ( ( offset_ + count_ ) >= _allocatedBytes )
		M_THROW( _( "overflow" ), offset_ + count_ );
	if ( count_ == 0 )
		count_ = ( _allocatedBytes - offset_ ) - 1; /* we maintain zero terminator (even though it is not fillz()) hence - 1 */
	if ( ( count_ + offset_ ) > _size )
		_size = count_ + offset_;
	::std::memset( _buffer + offset_, filler_, count_ );
	_buffer[ _size ] = 0;
	return ( *this );
	M_EPILOG
	}

HString& HString::fillz( char filler_, int long offset_, int long count_ )
	{
	M_PROLOG
	fill( filler_, offset_, count_ );
	_buffer[ count_ + offset_ ] = 0;
	return ( *this );
	M_EPILOG
	}

HString& HString::erase( int long from_, int long length_ )
	{
	M_PROLOG
	if ( from_ < 0 )
		length_ += from_, from_ = 0;
	if ( ( from_ + length_ ) >= _size )
		length_ = _size - from_;
	if ( ( length_ > 0 ) && ( from_ < _size ) )
		{
		::std::memmove( _buffer + from_, _buffer + from_ + length_, _size - ( from_ + length_ ) );
		_size -= length_;
		_buffer[ _size ] = 0;
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
		length_ += from_;
		if ( chunk_ )
			{
			if ( static_cast< ::std::size_t>( -from_ ) > ::std::strlen( chunk_ ) )
				M_THROW( "negative offset caused chunk overflow", from_ );
			chunk_ += -from_;
			}
		from_ = 0;
		}
	if ( ( length_ > 0 ) && ( from_ <= _size ) )
		{
		int long chunkLen = chunk_ ? static_cast<int long>( ::std::strlen( chunk_ ) ) : 0;
		if ( chunk_ && ( length_ > chunkLen ) )
			M_THROW( "length too big for this chunk (by)", length_ - chunkLen );
		int long oldSize = _size;
		_size += length_;
		hs_realloc( _size + 1 );
		::std::memmove( _buffer + from_ + length_, _buffer + from_, ( oldSize + 1 ) - from_ );
		if ( chunk_ )
			::std::strncpy( _buffer + from_, chunk_, length_ );
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

HString& HString::insert( int long from_, int long length_, char const& char_ )
	{
	M_PROLOG
	insert( from_, length_ );
	return ( fill( char_, from_, length_ ) );
	M_EPILOG
	}

HString& HString::append( HString const& str_ )
	{
	M_PROLOG
	return ( append( str_._buffer, str_._size ) );
	M_EPILOG
	}

HString& HString::append( HString const& str_, int long const& idx_, int long const& len_ )
	{
	M_PROLOG
	M_ENSURE( len_ >= 0 );
	if ( ( len_ > 0 ) && ( idx_ < str_._size ) )
		append( str_._buffer + ( idx_ >= 0 ? idx_ : 0 ),
				( ( idx_ + len_ ) < str_._size ) ? len_ : _size - idx_ );
	return ( *this );
	M_EPILOG
	}

HString& HString::append( int long const& count_, char const& val_ )
	{
	M_PROLOG
	int long oldSize( _size );
	_size += count_;
	hs_realloc( _size + 1 );
	::memset( _buffer + oldSize, val_, count_ );
	_buffer[ _size ] = 0;
	return ( *this );
	M_EPILOG
	}

HString& HString::append( char const* const buf_, int long const& len_ )
	{
	M_PROLOG
	M_ASSERT( len_ >= 0 );
	if ( len_ > 0 )
		{
		int long oldSize( _size );
		_size += len_;
		hs_realloc( _size + 1 );
		::memmove( _buffer + oldSize, buf_, len_ );
		_buffer[ _size ] = 0;
		}
	return ( *this );
	M_EPILOG
	}

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
	while ( index )
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
	while ( index )
		{
		if ( ! ::std::memchr( skipSet_, buffer_[ index ], skipSetSize ) )
			return ( index );
		-- index;
		}
	return ( length_ );
	M_EPILOG
	}

int long kmpsearch( char const* const str, int long const& lenstr, char const* const pat, int long const& lenpat )
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

int long kmpcasesearch( char const* const str, int long const& lenstr, char const* const pat, int long const& lenpat )
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

