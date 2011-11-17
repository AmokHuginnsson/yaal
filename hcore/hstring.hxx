/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	hstring.hxx - this file is integral part of `yaal' project.

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

#ifndef YAAL_HCORE_HSTRING_HXX_INCLUDED
#define YAAL_HCORE_HSTRING_HXX_INCLUDED 1

#include <cstddef>

#include "hcore/macro.hxx"
#include "hcore/numeric.hxx"
#include "hcore/assert.hxx"

namespace yaal {

namespace hcore {

class HCharacterClass {
	char const* _data;
	int _size;
public:
	HCharacterClass( char const* data_, int size_ )
		: _data( data_ ), _size( size_ ) {}
	char const* data( void ) const
		{ return ( _data ); }
	int size( void ) const
		{ return ( _size ); }
};

extern M_YAAL_HCORE_PUBLIC_API HCharacterClass const _whiteSpace_;
extern M_YAAL_HCORE_PUBLIC_API HCharacterClass const _digit_;
extern M_YAAL_HCORE_PUBLIC_API HCharacterClass const _letter_;
extern M_YAAL_HCORE_PUBLIC_API HCharacterClass const _lowerCaseLetter_;
extern M_YAAL_HCORE_PUBLIC_API HCharacterClass const _upperCaseLetter_;
extern M_YAAL_HCORE_PUBLIC_API HCharacterClass const _word_;
struct CHARACTER_CLASS {
	typedef enum {
		WHITE_SPACE = 0,
		DIGIT = 1,
		LETTER = 2,
		LOWER_CASE_LETTER = 3,
		UPPER_CASE_LETTER = 4,
		WORD = 5
	} character_class_t;
};
extern M_YAAL_HCORE_PUBLIC_API HCharacterClass const* _characterClass_[];

/*! \brief Implementation of high level string operations.
 */
class HString {
public:
	static int long const MAX_STRING_LENGTH = ( meta::max_signed<int long>::value / 2 ) - 1;
	typedef HString this_type;
	class HIterator;
	class HCharRef;
private:
	static int const INPLACE_BUFFER_SIZE = sizeof ( char* ) + sizeof ( int long ) + sizeof ( int long );
	static int const ALLOC_FLAG_INDEX = INPLACE_BUFFER_SIZE - 1;
	static int const MAX_INPLACE_CAPACITY = INPLACE_BUFFER_SIZE - 2; /* -1 for terminating NIL, -1 for ALLOC_FLAG byte. */
	char _mem[ INPLACE_BUFFER_SIZE ];
public:
	static int long const npos = -1;
	typedef HIterator iterator; /*!< mutable iterator for string characters */
	typedef char const* const_iterator; /*!< const iterator for string characters */
	/*! \brief Trivial constructor.
	 *
	 * \post Empty string is constructed.
	 */
	HString( void );
	/*! \brief Copy constructor.
	 *
	 * \param str - construct new string based on \e str.
	 */
	HString( HString const& str );
	/*! \brief Size based constructor.
	 *
	 * initialize immediately with size
	 *
	 * \param size - capacity of newly constructed string.
	 */
	HString( int long size, bool );
	/*! \brief Destroy string object and deallocate all resources.
	 */
	~HString( void );
	/*! \brief Increases string capacity.
	 *
	 * String capacity is always size of 2^n.
	 *
	 * \param size - new size capacity.
	 *
	 * \post String capacity has value of smallest 2^n greater or equal to \e size.
	 */
	void hs_realloc( int long size );
	/*! \brief Materialize string.
	 *
	 * Used in copy-on-write implementation.
	 */
	void materialize( void );
	/*! \brief Construct string based on raw old style C string.
	 *
	 * str - old style C string to base new HString on.
	 */
	HString( char const* const str );
	/*! \brief Construct string based on character array.
	 *
	 * array - a character array where to initialize new HString object with.
	 * size - at most that many characters from array are used.
	 */
	HString( char const* const array, int long size );
	/*! \brief Construct new HString from single character.
	 *
	 * ch - character to initialize new string with.
	 */
	HString( char ch );
	/*! \brief Construct new HString from single unsigned character.
	 *
	 * ch - an unsigned character to initialize new string with.
	 */
	HString( char unsigned ch );
	/*! \brief Construct new HString from short integer.
	 *
	 * shortInteger - an integer to initialize new string with.
	 */
	HString( int short shortInteger );
	/*! \brief Construct new HString from unsigned short integer.
	 *
	 * unsignedShortInteger - an integer to initialize new string with.
	 */
	HString( int short unsigned unsignedShortInteger );
	/*! \brief Construct new HString from integer.
	 *
	 * integer - an integer to initialize new string with.
	 */
	HString( int integer );
	/*! \brief Construct new HString from unsigned integer.
	 *
	 * unsignedInteger - an unsigned integer to initialize new string with.
	 */
	HString( int unsigned unsignedInteger );
	/*! \brief Construct new HString from long integer.
	 *
	 * longInteger - a long integer to initialize new string with.
	 */
	HString( int long longInteger );
	/*! \brief Construct new HString from unsigned long integer.
	 *
	 * unsignedLongInteger - a long integer to initialize new string with.
	 */
	HString( int long unsigned unsignedLongInteger );
	/*! \brief Construct new HString from double.
	 *
	 * real - a double to initialize new string with.
	 */
	HString( double real );
	/*! \brief Construct new HString from long double.
	 *
	 * longDouble - a double to initialize new string with.
	 */
	HString( double long longDouble );
	/*! \brief Construct new HString from pointer.
	 *
	 * pointer - an pointer to initialize new string with.
	 */
	HString( void const* const pointer );
	HString& operator = ( HString const& );
	HString& operator += ( HString const& );
	char operator[] ( int const ) const;
	char operator[] ( int long const ) const;
	bool operator ! ( void ) const;
	char set_at( int long, char );
	char const* raw( void ) const;
	char const* c_str( void ) const;
	int long max_size( void ) const;
	int long get_max_size( void ) const;
	iterator begin( void );
	iterator end( void );
	const_iterator begin( void ) const;
	const_iterator end( void ) const;
	/*! \brief Check if string is empty (has zero length).
	 *
	 * \return Tue iff this string is empty.
	 */
	bool is_empty( void ) const;
	/*! \brief Check if string is empty (has zero length).
	 *
	 * empty() is really an alias for is_empty().
	 *
	 * \return Tue iff this string is empty.
	 */
	bool empty( void ) const;
	/*! \brief Clear this string contents.
	 *
	 * \post String is empty.
	 */
	void clear( void );
	/*! \brief Get string length.
	 *
	 * \return String length in characters.
	 */
	int long get_length( void ) const;
	/*! \brief Get string length.
	 *
	 * length() is really an alias for get_length().
	 *
	 * \return String length in characters.
	 */
	int long length( void ) const;
	/*! \brief Get string length.
	 *
	 * get_size() is really an alias for get_length().
	 *
	 * \return String length in characters.
	 */
	int long get_size( void ) const;
	/*! \brief Get string length.
	 *
	 * size() is really an alias for get_length().
	 *
	 * \return String length in characters.
	 */
	int long size( void ) const;
	/*! \brief Tell how many characters can this string store without memory reallocation.
	 *
	 * \return Number of characters that can be stored in this string without reallocation.
	 */
	int long get_capacity( void ) const;
	/*! \brief Tell how many characters can this string store without memory reallocation.
	 *
	 * capacity() is really an alias for get_capacity().
	 *
	 * \return Number of characters that can be stored in this string without reallocation.
	 */
	int long capacity( void ) const;
	void swap( HString& );
	/*! \brief Erase old content and assign new data from buffer.
	 *
	 * \param data - data to be assigned to this string.
	 * \param size - size of the date to be assigned.
	 * \return Self.
	 */
	HString& assign( char const* const data, int long length );
	/*! \brief Erase old content and assign a substring of given string.
	 *
	 * \param str - assign substring of this string.
	 * \param offset - an offset of a substing to get.
	 * \param length - a length of substring to get.
	 * \return Self.
	 */
	HString& assign( HString const& str, int long offset, int long length );
	HString& format( char const* const, ... );
	HString& vformat( char const* const, void* );
	int long find( char, int long = 0 ) const;
	int long find( HString const&, int long = 0 ) const;
	int long nfind( HString const&, int long, int long = 0 ) const;
	int long reverse_find( char, int long = 0 ) const;
	int long find_last( char, int long = MAX_STRING_LENGTH ) const;
	int long find_one_of( char const* const, int long = 0 ) const;
	int long reverse_find_one_of( char const* const, int long = 0 ) const;
	int long find_last_one_of( char const* const, int long = MAX_STRING_LENGTH ) const;
	int long find_other_than( char const* const, int long = 0 ) const;
	int long reverse_find_other_than( char const* const, int long = 0 ) const;
	int long find_last_other_than( char const* const, int long = MAX_STRING_LENGTH ) const;
	HString& replace( HString const&, HString const& );
	HString& upper( void );
	HString& lower( void );
	HString& reverse( void );
	/*! \brief Get substring of this string by start position and length.
	 *
	 * \param start - position for begining of a substring.
	 * \param length - length of a substring.
	 * \return Substring.
	 */
	HString substr( int long start, int long length = MAX_STRING_LENGTH ) const;
	HString left( int long ) const;
	/*! \brief Get substring of this string by start position and length.
	 *
	 * mid() is really an alias for substr().
	 *
	 * \param start - position for begining of a substring.
	 * \param length - length of a substring.
	 * \return Substring.
	 */
	HString mid( int long start, int long length = MAX_STRING_LENGTH ) const;
	HString right( int long ) const;
	/*! \brief Trim all consecutive occurrences of given characters from beginning of the string.
	 *
	 * \param set - set of characters that shall be removed.
	 * \return Self.
	 */
	HString& trim_left( char const* const set = _whiteSpace_.data() );
	/*! \brief Trim all consecutive occurrences of given characters from end of the string.
	 *
	 * \param set - set of characters that shall be removed.
	 * \return Self.
	 */
	HString& trim_right( char const* const set = _whiteSpace_.data() );
	/*! \brief Trim all consecutive occurrences of given characters from both beginning and end of the string.
	 *
	 * \param set - set of characters that shall be removed.
	 * \return Self.
	 */
	HString& trim( char const* const set = _whiteSpace_.data() );
	HString& shift_left( int long );
	HString& shift_right( int long, char const = ' ' );
	/*! \brief Fill portion of string with constatnt value.
	 *
	 * \param value - use this value as a filler.
	 * \param position - start at this position.
	 * \param count - fill that many bytes.
	 * \return Self.
	 */
	HString& fill( char value = '\0', int long position = 0, int long lenght = 0 );
	/*! \brief Fill portion of string with constatnt value and finish with \0 (zero) byte.
	 *
	 * \param value - use this value as a filler.
	 * \param position - start at this position.
	 * \param count - fill that many bytes.
	 * \return Self.
	 */
	HString& fillz( char value = '\0', int long position = 0, int long count = 0 );
	/*! \brief Erase part of the string.
	 *
	 * \param position_ - start position for part to be erased.
	 * \param length_ - length of part to be erased.
	 * \return Self.
	 */
	HString& erase( int long position_ = 0, int long length_ = MAX_STRING_LENGTH );
	/*! \brief Insert given string at given position.
	 *
	 * \param pos - position where given string has to be inserted.
	 * \param str - string to be inserted.
	 * \return Self.
	 */
	HString& insert( int long pos, HString const& str );
	/*! \brief Insert given string at given position.
	 *
	 * \param pos - position where given string has to be inserted.
	 * \param len - lenght of string to be inserted.
	 * \param str - string to be inserted.
	 * \return Self.
	 *
	 * \pre \e str real length must not be smaller than \e len.
	 */
	HString& insert( int long pos, int long len, char const* str = NULL );
	/*! \brief Insert given string at given position.
	 *
	 * \param pos - position where given string has to be inserted.
	 * \param str - string to be inserted.
	 * \return Self.
	 */
	HString& insert( int long pos, char const* str = NULL );
	/*! \brief Insert given string at given position.
	 *
	 * \param pos - position where given string has to be inserted.
	 * \param str - string to be inserted.
	 * \param len - lenght of string to be inserted.
	 * \return Self.
	 *
	 * \pre \e str real length must not be smaller than \e len.
	 */
	HString& insert( int long pos, char const* str, int long len );
	/*! \brief Insert n copies of value before given position.
	 *
	 * \param postition - where put inserted block.
	 * \param n - number of bytes to insert.
	 * \param value - copies of this value shall be inserted.
	 */
	HString& insert( int long postition, int long n, char value );
	HString& append( HString const& );
	/*! \brief Append a substring of given string to this string.
	 *
	 * \param str - append substring of this string.
	 * \param offset - an offset of a substing to get.
	 * \param length - a length of substring to get.
	 * \return Self.
	 */
	HString& append( HString const&, int long idx_, int long len_ );
	HString& append( int long count_, char val_ );
	/*! \brief Append new data from buffer to already existing data in this string.
	 *
	 * \param data - data to be appended to this string.
	 * \param size - size of the date to be appended.
	 * \return Self.
	 */
	HString& append( char const* const, int long len_ );
	/*! \brief Append single character at end of this string.
	 *
	 * \param character_ - character to be appended.
	 */
	void push_back( char character_ );
private:
	void substr( HString&, int long, int long ) const;
};

class HString::HCharRef {
	HString& _string;
	int long _index;
public:
	operator char ( void ) {
		return ( _string[ _index ] );
	}
	HCharRef& operator = ( char ch_ );
	void swap( HCharRef& );
private:
	friend class HString;
	HCharRef( HString& string_, int long index_ )
		: _string( string_ ), _index( index_ )
		{}
};

class HString::HIterator {
	HString* _owner;
	int long _index;
public:
	HIterator( void )
		: _owner( NULL ), _index( 0 )
		{}
	HIterator( HIterator const& it_ )
		: _owner( it_._owner ), _index( it_._index )
		{}
	HIterator& operator = ( HIterator const& it_ ) {
		if ( &it_ != this ) {
			_owner = it_._owner;
			_index = it_._index;
		}
		return ( *this );
	}
	bool operator == ( HIterator const& it_ ) const {
		M_ASSERT( _owner == it_._owner );
		return ( _index == it_._index );
	}
	bool operator != ( HIterator const& it_ ) const {
		M_ASSERT( _owner == it_._owner );
		return ( _index != it_._index );
	}
	HIterator& operator ++ ( void ) {
		M_ASSERT( _owner );
		++ _index;
		return ( *this );
	}
	HIterator& operator -- ( void ) {
		M_ASSERT( _owner );
		-- _index;
		return ( *this );
	}
	HIterator operator ++ ( int ) {
		HIterator it( *this );
		++ _index;
		return ( it );
	}
	HIterator operator -- ( int ) {
		HIterator it( *this );
		-- _index;
		return ( it );
	}
	HCharRef operator * ( void ) {
		return ( HCharRef( *_owner, _index ) );
	}
	operator HString::const_iterator ( void ) const {
		return ( _owner->raw() + _index );
	}
private:
	friend class HString;
	HIterator( HString* owner_, int long index_ )
		: _owner( owner_ ), _index( index_ )
		{}
};

HString operator + ( HString const&, HString const& );
int strcasecmp( HString const&, HString const& );
bool operator == ( HString const&, HString const& );
bool operator != ( HString const&, HString const& );
bool operator >= ( HString const&, HString const& );
bool operator <= ( HString const&, HString const& );
bool operator > ( HString const&, HString const& );
bool operator < ( HString const&, HString const& );
int long hash( HString const& );

/*! \brief HString class helpers, utility functions.
 */
namespace string_helper {

int long kmpcasesearch( char const* const, int long, char const* const, int long );

}

}

inline void swap( yaal::hcore::HString& a, yaal::hcore::HString& b )
	{ a.swap( b ); }

inline void swap( yaal::hcore::HString::HCharRef a, yaal::hcore::HString::HCharRef b )
	{ a.swap( b ); }

}

#endif /* #ifndef YAAL_HCORE_HSTRING_HXX_INCLUDED */

