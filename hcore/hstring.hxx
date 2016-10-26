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

/*! \file hcore/hstring.hxx
 * \brief Declaration of HString class.
 *
 * All HString related free functions are included here.
 * HCharacterClass also resides here.
 */

#ifndef YAAL_HCORE_HSTRING_HXX_INCLUDED
#define YAAL_HCORE_HSTRING_HXX_INCLUDED 1

#include <cstddef>

#include "config.hxx"
#include "hcore/macro.hxx"
#include "hcore/base.hxx"
#include "hcore/pod.hxx"
#include "hcore/numeric.hxx"
#include "hcore/assert.hxx"

namespace yaal {

namespace hcore {

/*! \brief Character Class (as in RE's [...]) definition.
 *
 * Define set of characters with O(1) size() on set.
 */
class HCharacterClass {
	char const* _data;
	int _size;
public:
	HCharacterClass( char const* data_, int size_ )
		: _data( data_ ), _size( size_ ) {}
	/*! \brief Get characters in Character Class.
	 *
	 * \return Data in Character Class.
	 */
	char const* data( void ) const {
		return ( _data );
	}
	/*! \brief Get size of Character Class.
	 *
	 * \return Number of characters in Character Class.
	 */
	int size( void ) const {
		return ( _size );
	}
	bool has( char ) const;
};

extern M_YAAL_HCORE_PUBLIC_API HCharacterClass const _whiteSpace_;
extern M_YAAL_HCORE_PUBLIC_API HCharacterClass const _binDigit_;
extern M_YAAL_HCORE_PUBLIC_API HCharacterClass const _octDigit_;
extern M_YAAL_HCORE_PUBLIC_API HCharacterClass const _digit_;
extern M_YAAL_HCORE_PUBLIC_API HCharacterClass const _hexDigit_;
extern M_YAAL_HCORE_PUBLIC_API HCharacterClass const _letter_;
extern M_YAAL_HCORE_PUBLIC_API HCharacterClass const _lowerCaseLetter_;
extern M_YAAL_HCORE_PUBLIC_API HCharacterClass const _upperCaseLetter_;
extern M_YAAL_HCORE_PUBLIC_API HCharacterClass const _word_;
extern M_YAAL_HCORE_PUBLIC_API HCharacterClass const _vowel_;
struct CHARACTER_CLASS {
	typedef enum {
		WHITE_SPACE = 0,
		DIGIT = 1,
		LETTER = 2,
		LOWER_CASE_LETTER = 3,
		UPPER_CASE_LETTER = 4,
		WORD = 5,
		VOWEL = 6
	} character_class_t;
};
extern M_YAAL_HCORE_PUBLIC_API HCharacterClass const* _characterClass_[];

/*! \brief Implementation of high level string operations.
 */
class HString final {
public:
	static_assert( sizeof ( int long ) <= sizeof ( int_native_t ), "length type overflows native integer type" );
	static int long const MAX_STRING_LENGTH = ( meta::max_signed<int long>::value / 2 ) - 1;
	typedef HString this_type;
	class HIterator;
	class HCharRef;
private:
	static int const INPLACE_BUFFER_SIZE = sizeof ( char* ) + sizeof ( int_native_t ) + sizeof ( int_native_t );
	static int const ALLOC_FLAG_INDEX = INPLACE_BUFFER_SIZE - 1;
	static int const MAX_INPLACE_CAPACITY = INPLACE_BUFFER_SIZE - 2; /* -1 for terminating NIL, -1 for ALLOC_FLAG byte. */
	STATIC_ASSERT( sizeof ( int long ) == sizeof ( char* ) );
	union {
		int_native_t _len[ 3 ];
		char _mem[ INPLACE_BUFFER_SIZE ];
		char* _ptr;
	};
	static_assert( sizeof ( HString::_mem ) == sizeof ( HString::_len ), "buffer views are missaligned" );
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
	/*! \brief Move constructor.
	 *
	 * \param str - construct new string based on \e str.
	 */
	HString( HString&& str );
	/*! \brief Size based constructor.
	 *
	 * initialize immediately with size
	 *
	 * \param size_ - capacity of newly constructed string.
	 * \param fill_ - fill allocated space with given character.
	 */
	HString( int long size_, char fill_ );
	/*! \brief Destroy string object and deallocate all resources.
	 */
	~HString( void );
	/*! \brief Increases string capacity.
	 *
	 * String capacity is always size of (2^n)-1 or MAX_INPLACE_CAPACITY.
	 *
	 * \param capacity - new string capacity.
	 *
	 * \post String capacity has value of smallest (2^n)-1 greater or equal to \e size.
	 */
	void reserve( int long capacity );
	/*! \brief Materialize string.
	 *
	 * Used in copy-on-write implementation.
	 */
	void materialize( void );
	/*! \brief Construct string based on raw old style C string.
	 *
	 * str - old style C string to base new HString on.
	 */
	HString( char const* str );
	/*! \brief Construct string based on const iterator pair.
	 *
	 * first_ - beginning of the character range to copy.
	 * last_ - one past the end of character range to copy.
	 */
	HString( const_iterator first_, const_iterator last_ );
	/*! \brief Construct string based on character array.
	 *
	 * array - a character array where to initialize new HString object with.
	 * size - at most that many characters from array are used.
	 */
	HString( char const* array, int long size );
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
	/*! \brief Construct new HString from long long integer.
	 *
	 * longLongInteger - a long long integer to initialize new string with.
	 */
	HString( int long long longLongInteger );
	/*! \brief Construct new HString from unsigned long long integer.
	 *
	 * unsignedLongLongInteger - a long long integer to initialize new string with.
	 */
	HString( int long long unsigned unsignedLongLongInteger );
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
	HString( void const* pointer );
	HString& operator = ( HString const& );
	/*! \brief Move assignemnt operator.
	 *
	 * Move "value" of a string to another string.
	 */
	HString& operator = ( HString&& str );
	/*! \brief Append another string to this string.
	 *
	 * \param str - string to append to this string.
	 * \return This string.
	 */
	HString& operator += ( HString const& str );
	/*! \brief Get character at given position.
	 *
	 * \param position - index of a position to get character for.
	 * \return character at given position in this string.
	 */
	HString operator + ( HString const& ) const;
	char operator[] ( int position ) const;
	/*! \brief Get character at given position.
	 *
	 * \param position - index of a position to get character for.
	 * \return character at given position in this string.
	 */
	char operator[] ( int long position ) const;
	/*! \brief Check if string is empty (has zero length).
	 *
	 * \return True iff this string is empty.
	 */
	bool operator ! ( void ) const;
	/*! \brief Set givent character at given position.
	 *
	 * \param position - index of a position where given character shall be set.
	 * \param character - character to be set at given position.
	 */
	bool operator == ( HString const& ) const;
	bool operator != ( HString const& ) const;
	bool operator >= ( HString const& ) const;
	bool operator <= ( HString const& ) const;
	bool operator > ( HString const& ) const;
	bool operator < ( HString const& ) const;
	char set_at( int long position, char character );
	/*! \brief Get access to raw character data stored in this string.
	 *
	 * \return pointer to raw character data stored in this string.
	 */
	char const* raw( void ) const;
	/*! \brief Get access to raw character data stored in this string.
	 *
	 * An alias for HString::raw().
	 *
	 * \return pointer to raw character data stored in this string.
	 */
	char const* c_str( void ) const;
	/*! \brief Get access to raw character data stored in this string.
	 *
	 * An alias for HString::raw().
	 *
	 * \return pointer to raw character data stored in this string.
	 */
	char const* data( void ) const;
	/*! \brief Get access to first chaacter in the string.
	 *
	 * String must be non-empty.
	 */
	char front( void ) const;
	/*! \brief Get access to last chaacter in the string.
	 *
	 * String must be non-empty.
	 */
	char back( void ) const;
	/*! \brief Get maximum number of characters that can be stored in any string.
	 *
	 * Alias for HString::get_max_size().
	 *
	 * \return Maximum number of characters that can be stored in any string.
	 */
	int long max_size( void ) const;
	/*! \brief Get maximum number of characters that can be stored in any string.
	 *
	 * \return Maximum number of characters that can be stored in any string.
	 */
	int long get_max_size( void ) const;
	/*! \brief Get iterator pointing to beginning (a first character) of this string.
	 *
	 * \return Iterator pointing to beginning of this string.
	 */
	iterator begin( void );
	/*! \brief Get iterator pointing to end (one past last character) of this string.
	 *
	 * \return Iterator pointing to end (terminating \0) of this string.
	 */
	iterator end( void );
	/*! \brief Get iterator pointing to beginning (a first character) of this string.
	 *
	 * \return Iterator pointing to beginning of this string.
	 */
	const_iterator begin( void ) const;
	/*! \brief Get iterator pointing to end (one past last character) of this string.
	 *
	 * \return Iterator pointing to end (terminating \0) of this string.
	 */
	const_iterator end( void ) const;
	/*! \brief Check if string is empty (has zero length).
	 *
	 * \return True iff this string is empty.
	 */
	bool is_empty( void ) const;
	/*! \brief Check if string is empty (has zero length).
	 *
	 * empty() is really an alias for is_empty().
	 *
	 * \return True iff this string is empty.
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
	/*! \brief Tell how many characters excluding terminating NIL can this string store without memory reallocation.
	 *
	 * \return Number of characters excluding terminating NIL that can be stored in this string without reallocation.
	 */
	int long get_capacity( void ) const;
	/*! \brief Tell how many characters excluding terminating NIL can this string store without memory reallocation.
	 *
	 * capacity() is really an alias for get_capacity().
	 *
	 * \return Number of characters excluding terminating NIL that can be stored in this string without reallocation.
	 */
	int long capacity( void ) const;
	/*! \brief Swap contents of this string with another string.
	 *
	 * \param other - the other string to swap contents with.
	 */
	void swap( HString& other );
	/*! \brief Erase old content and assign new data from buffer.
	 *
	 * \param data - data to be assigned to this string.
	 * \param size - size of the date to be assigned.
	 * \return Self.
	 */
	HString& assign( char const* data, int long length );
	/*! \brief Erase old content and assign a substring of given string.
	 *
	 * \param str - assign substring of this string.
	 * \param offset - an offset of a substing to get.
	 * \param length - a length of substring to get.
	 * \return Self.
	 */
	HString& assign( HString const& str, int long offset = 0, int long length = npos );
	/*! \brief Erase old content and assign a range of characters to this string.
	 *
	 * first_ - beginning of the character range to copy.
	 * last_ - one past the end of character range to copy.
	 */
	HString& assign( const_iterator first_, const_iterator last_ );
	/*! \brief Erase old content and fill string with N copies of given character.
	 *
	 * \param size_ - number of copies of given character to make.
	 * \param fill_ - fill allocated space with given character.
	 */
	HString& assign( int long size_, char fill_ );
	/*! \brief Replace contents of this string with result of `C' printf() style format.
	 *
	 * \param frmt - `C's printf() style format specification.
	 */
	HString& format( char const* frmt, ... ) __attribute__(( format( printf, 2, 3 ) ));
	HString& vformat( char const*, void* ) __attribute__(( format( printf, 2, 0 ) ));
	/*! \brief Find position of given character in this string.
	 *
	 * \param character - a character to look for.
	 * \param offest - position where searching for given character starts.
	 * \return Position of given character in this string if given character is found or HString::npos otherwise.
	 */
	int long find( char character, int long offest = 0 ) const;
	/*! \brief Find position of given sub-string in this string.
	 *
	 * \param str - a sub-string to look for.
	 * \param offest - position where searching for given sub-string starts.
	 * \return Position of given sub-string (first character of it) in this string if given sub-string is found or HString::npos otherwise.
	 */
	int long find( HString const& str, int long offset = 0 ) const;
	int long nfind( HString const&, int long, int long = 0 ) const;
	/*! \brief Find distance from the end of last occurrence of given character.
	 *
	 * HString::reverse_find() is really 100% equivalent of HString::find() on reversed string,
	 * which means that given character is search from the end and returned index is counted
	 * relative to the end of the string.
	 *
	 * This method is totally different than HString::find_last().
	 *
	 * \param character - look for this character in this string.
	 * \param before - skip that many positions from the end before start looking for given character.
	 * \return Distance from the end to given character if given character can be found or HString::npos otherwise.
	 */
	int long reverse_find( char character, int long before = 0 ) const;
	/*! \brief Find index of last occurrence of given character.
	 *
	 * This method is totally different than HString::reverse_find().
	 *
	 * \param character - look for position of this character in this string.
	 * \param before - assume that this string is only that long.
	 * \return Index of last occurrence of given character if given character can be found or HString::npos otherwise.
	 */
	int long find_last( char character, int long before = npos ) const;
	/*! \brief Find index of last occurrence of given sub-string.
	 *
	 * This method is totally different than HString::reverse_find().
	 *
	 * \param str - look for position of this sub-strting in this string.
	 * \param before - assume that this string is only that long.
	 * \return Index of last occurrence of given sub-string if given sub-string can be found or HString::npos otherwise.
	 */
	int long find_last( HString const& str, int long before = npos ) const;
	int long find_one_of( char const*, int long = 0 ) const;
	int long reverse_find_one_of( char const*, int long = 0 ) const;
	int long find_last_one_of( char const*, int long = npos ) const;
	int long find_other_than( char const*, int long = 0 ) const;
	int long reverse_find_other_than( char const*, int long = 0 ) const;
	int long find_last_other_than( char const*, int long = npos ) const;
	/*! \brief Replace each occurrence of given pattern with replacement.
	 *
	 * \param pattern - search and replace all occurrences of this pattern.
	 * \param replacement - use this string as a replacement for given pattern.
	 * \return Self.
	 */
	HString& replace( HString const& pattern, HString const& replacement );
	/*! \brief Replace part of a this string with supplied string.
	 *
	 * \param pos_ - where replaced part starts.
	 * \param size_ - size of replaced part.
	 * \param replacement - replacement for given part.
	 * \return Self.
	 */
	HString& replace( int long pos_, int long size_, HString const& replacement );
	/*! \brief Replace part of a this string with supplied substring.
	 *
	 * \param pos_ - where replaced part starts.
	 * \param size_ - size of replaced part.
	 * \param replacement - replacement source for given part.
	 * \param offset_ - where replacement part starts in replacement source.
	 * \param len_ - length of replacement.
	 * \return Self.
	 */
	HString& replace( int long pos_, int long size_, HString const& replacement, int long offset_, int long len_ );
	/*! \brief Replace part of a this string with supplied character buffer.
	 *
	 * \param pos_ - where replaced part starts.
	 * \param size_ - size of replaced part.
	 * \param buffer_ - replacement source for given part.
	 * \param len_ - length of replacement.
	 * \return Self.
	 */
	HString& replace( int long pos_, int long size_, char const* buffer_, int long len_ );
	/*! \brief Replace range in this string with supplied string.
	 *
	 * \param first_ - where replaced range starts.
	 * \param last_ - whare replaced range ends.
	 * \param replacement - replacement for given part.
	 * \return Self.
	 */
	HString& replace( iterator first_, iterator last_, HString const& replacement );
	/*! \brief Convert all characters of this string to upper case.
	 *
	 * \return Self.
	 */
	HString& upper( void );
	/*! \brief Convert all characters of this string to lower case.
	 *
	 * \return Self.
	 */
	HString& lower( void );
	/*! \brief Reverse cases of all characters is this string.
	 *
	 * \return Self.
	 */
	HString& reverse( void );
	/*! \brief Get substring of this string by start position and length.
	 *
	 * \param start - position for beginning of a substring.
	 * \param length - length of a substring.
	 * \return Substring.
	 */
	HString substr( int long start, int long length = npos ) const;
	/*! \brief Get left part of this string of given length.
	 *
	 * \param len - length of left part of this string to get.
	 * \param left part of this string with at most len characters.
	 */
	HString left( int long len ) const;
	/*! \brief Get substring of this string by start position and length.
	 *
	 * mid() is really an alias for substr().
	 *
	 * \param start - position for beginning of a substring.
	 * \param length - length of a substring.
	 * \return Substring.
	 */
	HString mid( int long start, int long length = npos ) const;
	/*! \brief Get right part of this string of given length.
	 *
	 * \param len - length of right part of this string to get.
	 * \param right part of this string with at most len characters.
	 */
	HString right( int long len ) const;
	/*! \brief Trim all consecutive occurrences of given characters from beginning of the string.
	 *
	 * \param set - set of characters that shall be removed.
	 * \return Self.
	 */
	HString& trim_left( char const* set = _whiteSpace_.data() );
	/*! \brief Trim all consecutive occurrences of given characters from end of the string.
	 *
	 * \param set - set of characters that shall be removed.
	 * \return Self.
	 */
	HString& trim_right( char const* set = _whiteSpace_.data() );
	/*! \brief Trim all consecutive occurrences of given characters from both beginning and end of the string.
	 *
	 * \param set - set of characters that shall be removed.
	 * \return Self.
	 */
	HString& trim( char const* set = _whiteSpace_.data() );
	/*! \brief Remove first N characters from this string.
	 * \param len - remove that many characters from beginning of this string.
	 * \return Self.
	 */
	HString& shift_left( int long len );
	HString& shift_right( int long, char const = ' ' );
	/*! \brief Fill portion of string with constatnt value.
	 *
	 * \param value - use this value as a filler.
	 * \param position - start at this position.
	 * \param count - fill that many bytes.
	 * \return Self.
	 */
	HString& fill( char value = '\0', int long position = 0, int long length = npos );
	/*! \brief Fill portion of string with constatnt value and finish with \0 (zero) byte.
	 *
	 * \param value - use this value as a filler.
	 * \param position - start at this position.
	 * \param count - fill that many bytes.
	 * \return Self.
	 */
	HString& fillz( char value = '\0', int long position = 0, int long count = npos );
	/*! \brief Erase part of the string.
	 *
	 * \param position_ - start position for part to be erased.
	 * \param length_ - length of part to be erased.
	 * \return Self.
	 */
	HString& erase( int long position_ = 0, int long length_ = npos );
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
	 * \param len - length of string to be inserted.
	 * \param str - string to be inserted.
	 * \return Self.
	 *
	 * \pre \e str real length must not be smaller than \e len.
	 */
	HString& insert( int long pos, int long len, char const* str = nullptr );
	/*! \brief Insert given string at given position.
	 *
	 * \param pos - position where given string has to be inserted.
	 * \param str - string to be inserted.
	 * \return Self.
	 */
	HString& insert( int long pos, char const* str = nullptr );
	/*! \brief Insert given string at given position.
	 *
	 * \param pos - position where given string has to be inserted.
	 * \param str - string to be inserted.
	 * \param len - length of string to be inserted.
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
	HString& append( HString const&, int long idx_, int long len_ = npos );
	HString& append( int long count_, char val_ );
	/*! \brief Append new data from buffer to already existing data in this string.
	 *
	 * \param data - data to be appended to this string.
	 * \param size - size of the date to be appended.
	 * \return Self.
	 */
	HString& append( char const*, int long len_ );
	/*! \brief Append new data from a range of characters to already existing data in this string.
	 *
	 * first_ - beginning of the character range to copy.
	 * last_ - one past the end of character range to copy.
	 */
	HString& append( const_iterator first_, const_iterator last_ );
	/*! \brief Append single character at end of this string.
	 *
	 * \param character_ - character to be appended.
	 */
	void push_back( char character_ );
	/*! \brief Remove single character from end of this string.
	 */
	void pop_back( void );
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
		: _string( string_ ), _index( index_ ) {
		return;
	}
};

class HString::HIterator {
	HString* _owner;
	int long _index;
public:
	HIterator( void )
		: _owner( nullptr ), _index( 0 ) {
		return;
	}
	HIterator( HIterator const& it_ )
		: _owner( it_._owner ), _index( it_._index ) {
		return;
	}
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
	HIterator& operator += ( int offset_ ) {
		_index += offset_;
		return ( *this );
	}
	HIterator& operator += ( int long offset_ ) {
		_index += offset_;
		return ( *this );
	}
	HIterator& operator -= ( int offset_ ) {
		_index -= offset_;
		return ( *this );
	}
	HIterator& operator -= ( int long offset_ ) {
		_index -= offset_;
		return ( *this );
	}
	HIterator operator + ( int offset_ ) {
		return ( HIterator( _owner, _index + offset_ ) );
	}
	HIterator operator + ( int long offset_ ) {
		return ( HIterator( _owner, _index + offset_ ) );
	}
	HIterator operator - ( int offset_ ) {
		return ( HIterator( _owner, _index - offset_ ) );
	}
	HIterator operator - ( int long offset_ ) {
		return ( HIterator( _owner, _index - offset_ ) );
	}
	HCharRef operator * ( void ) {
		return ( HCharRef( *_owner, _index ) );
	}
	operator HString::const_iterator ( void ) const {
		return ( _owner->raw() + _index );
	}
	int long operator - ( HIterator const& it_ ) const {
		return ( _index - it_._index );
	}
private:
	friend class HString;
	HIterator( HString* owner_, int long index_ )
		: _owner( owner_ ), _index( index_ ) {
		return;
	}
};

HString operator "" _ys ( char const*, size_t );
HString operator + ( char const*, HString const& );
bool operator == ( char const*, HString const& );
bool operator != ( char const*, HString const& );
bool operator >= ( char const*, HString const& );
bool operator <= ( char const*, HString const& );
bool operator > ( char const*, HString const& );
bool operator < ( char const*, HString const& );
int strcasecmp( HString const&, HString const& );
HString to_string( char );
HString to_string( char unsigned );
HString to_string( int short );
HString to_string( int short unsigned );
HString to_string( int );
HString to_string( int unsigned );
HString to_string( int long );
HString to_string( int long unsigned );
HString to_string( int long long );
HString to_string( int long long unsigned );
HString to_string( float );
HString to_string( double );
HString to_string( double long );
HString to_string( char const* );
HString to_string( void const* );
inline HString to_string( HString const& v_ ) {
	return ( v_ );
}
int stoi( HString const&, int* = nullptr, int = 10 );
int long stol( HString const&, int* = nullptr, int = 10 );
int long unsigned stoul( HString const&, int* = nullptr, int = 10 );
int long long stoll( HString const&, int* = nullptr, int = 10 );
int long long unsigned stoull( HString const&, int* = nullptr, int = 10 );
float stof( HString const&, int* = nullptr );
double stod( HString const&, int* = nullptr );
double long stold( HString const&, int* = nullptr );
bool is_whitespace( char );
bool is_digit( char );
bool is_dec_digit( char );
bool is_hex_digit( char );
bool is_oct_digit( char );
bool is_bin_digit( char );
bool is_letter( char );
bool is_alpha( char );

inline void swap( yaal::hcore::HString& a, yaal::hcore::HString& b ) {
	a.swap( b );
}

inline void swap( yaal::hcore::HString::HCharRef a, yaal::hcore::HString::HCharRef b ) {
	a.swap( b );
}

}

}

#endif /* #ifndef YAAL_HCORE_HSTRING_HXX_INCLUDED */

