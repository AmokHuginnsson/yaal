/* Read yaal/LICENSE.md file for copyright and licensing information. */

/*! \file hcore/hstring.hxx
 * \brief Declaration of HString class and HUTF8String class.
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
#include "hcore/htaggedpod.hxx"
#include "hcore/iterator.hxx"

namespace yaal {

/*! \brief Unicode code point representation type.
 */
typedef yaal::hcore::HTaggedPOD<yaal::u32_t, struct code_point_tag> code_point_t;
inline constexpr code_point_t operator "" _ycp ( char char_ ) {
	return ( code_point_t( static_cast<yaal::u32_t>( char_ ) ) );
}
inline constexpr code_point_t operator "" _ycp ( wchar_t char_ ) {
	return ( code_point_t( static_cast<yaal::u32_t>( char_ ) ) );
}
inline constexpr code_point_t operator "" _ycp ( int long long unsigned char_ ) {
	return ( code_point_t( static_cast<yaal::u32_t>( char_ ) ) );
}
inline constexpr code_point_t operator "" _ycp ( char32_t codePoint_ ) {
	return ( code_point_t( codePoint_ ) );
}

namespace hcore {

/*! \brief Character Class (as in RE's [...]) definition.
 *
 * Define set of characters with O(1) size() on set.
 */
template<typename char_type>
class HCharacterClass {
	char_type const* _data;
	int _size;
public:
	HCharacterClass( char_type const* data_, int size_ )
		: _data( data_ )
		, _size( size_ ) {}
	/*! \brief Get characters in Character Class.
	 *
	 * \return Data in Character Class.
	 */
	char_type const* data( void ) const {
		return ( _data );
	}
	/*! \brief Get size of Character Class.
	 *
	 * \return Number of characters in Character Class.
	 */
	int size( void ) const {
		return ( _size );
	}
	bool has( code_point_t ) const;
	bool hasz( code_point_t ) const;
};
typedef HCharacterClass<char> character_class_t;

enum class CHARACTER_CLASS {
	WHITESPACE                       = 0,
	BIN_DIGIT                        = WHITESPACE                    + 1,
	OCT_DIGIT                        = BIN_DIGIT                     + 1,
	DIGIT                            = OCT_DIGIT                     + 1,
	HEX_DIGIT                        = DIGIT                         + 1,
	LETTER                           = HEX_DIGIT                     + 1,
	LOWER_CASE_LETTER                = LETTER                        + 1,
	UPPER_CASE_LETTER                = LOWER_CASE_LETTER             + 1,
	WORD                             = UPPER_CASE_LETTER             + 1,
	PUNCTUATION                      = WORD                          + 1,
	VOWEL                            = PUNCTUATION                   + 1,
	GREEK                            = VOWEL                         + 1,
	LOWER_CASE_GREEK                 = GREEK                         + 1,
	UPPER_CASE_GREEK                 = LOWER_CASE_GREEK              + 1,
	SUBSCRIPT                        = UPPER_CASE_GREEK              + 1,
	SUBSCRIPT_DIGIT                  = SUBSCRIPT                     + 1,
	SUBSCRIPT_LETTER                 = SUBSCRIPT_DIGIT               + 1,
	SUBSCRIPT_LOWER_CASE_LETTER      = SUBSCRIPT_LETTER              + 1,
	SUBSCRIPT_UPPER_CASE_LETTER      = SUBSCRIPT_LOWER_CASE_LETTER   + 1,
	SUPERSCRIPT                      = SUBSCRIPT_UPPER_CASE_LETTER   + 1,
	SUPERSCRIPT_DIGIT                = SUPERSCRIPT                   + 1,
	SUPERSCRIPT_LETTER               = SUPERSCRIPT_DIGIT             + 1,
	SUPERSCRIPT_LOWER_CASE_LETTER    = SUPERSCRIPT_LETTER            + 1,
	SUPERSCRIPT_UPPER_CASE_LETTER    = SUPERSCRIPT_LOWER_CASE_LETTER + 1,
	PRIVATE_USE_AREA                 = -1,
	SUPPLEMENTARY_PRIVATE_USE_AREA_A = PRIVATE_USE_AREA                 - 1,
	SUPPLEMENTARY_PRIVATE_USE_AREA_B = SUPPLEMENTARY_PRIVATE_USE_AREA_A - 1
};
template<typename char_type>
HCharacterClass<char_type> const& character_class( CHARACTER_CLASS );
template<CHARACTER_CLASS const characterClass>
HCharacterClass<typename trait::ternary<( characterClass >= CHARACTER_CLASS::GREEK ), char16_t, char>::type> const& character_class( void ) {
	typedef typename trait::ternary<( characterClass >= CHARACTER_CLASS::GREEK ), char16_t, char>::type char_type;
	return ( character_class<char_type>( characterClass ) );
}

class HUTF8String;

/*! \brief Implementation of high level string operations.
 *
 * HString uses adaptive UCS-n (n in {1, 2, 4}) representation,
 * which means that single character can be represented by
 * either 1, or 2 or 4 bytes.
 *
 * HString uses constant length character representation, which means
 * that the `n` in UCS-n, or string `rank` is determined by the
 * oldes code point (a character value) stored in given string.
 *
 * One of the consequences is that change of a single character
 * value in given string may result in reallocation and up
 * to 4 times more memory usage to hold the string.
 *
 * HString never exposes its internal data buffer.
 * Consequently HString does not need to store "terminating NIL" byte.
 */
class HString final {
public:
	typedef HString this_type;
	typedef int long size_type;
	static_assert( sizeof ( size_type ) <= sizeof ( int_native_t ), "length type overflows native integer type" );
	/*
	 * Divide by 4 because max string rank is 4 and string of rank
	 * 4 requires 4 bytes per character and MAX_STRING_LENGTH is expressed in number of characters.
	 */
	static size_type const MAX_STRING_LENGTH = ( meta::max_signed<size_type>::value / 4 ) - 1;
	class HConstIterator;
	class HIterator;
	class HCharRef;
private:
	static int const INPLACE_BUFFER_SIZE = sizeof ( char* ) + sizeof ( int_native_t ) + sizeof ( int_native_t );
	static int const ALLOC_FLAG_INDEX = INPLACE_BUFFER_SIZE - 1;
	static int const MAX_INPLACE_CAPACITY = INPLACE_BUFFER_SIZE - 1; /* -1 for ALLOC_FLAG byte. */
	static_assert( sizeof ( int_native_t ) == sizeof ( char* ), "invalid native integer type definition" );
	union {
		int_native_t _len[ 3 ];
		char _mem[ INPLACE_BUFFER_SIZE ];
		void* _ptr;
	};
	static_assert( sizeof ( HString::_mem ) == sizeof ( HString::_len ), "buffer views are misaligned" );
public:
	static size_type const npos = -1;
	typedef HIterator iterator; /*!< mutable iterator for string characters */
	typedef HConstIterator const_iterator; /*!< const iterator for string characters */
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
	HString( HString&& str ) noexcept;
	/*! \brief Size based constructor.
	 *
	 * initialize immediately with size
	 *
	 * \param size_ - capacity of newly constructed string.
	 * \param fill_ - fill allocated space with given character.
	 */
	HString( size_type size_, code_point_t fill_ );
	/*! \brief Destroy string object and deallocate all resources.
	 */
	~HString( void );
	/*! \brief Increases string capacity for given rank.
	 *
	 * String capacity is always size of (2^n)-1 or MAX_INPLACE_CAPACITY.
	 *
	 * \param capacity - new string capacity.
	 * \param rank - what is expected string rank, string rank is number of bytes required to represent single character.
	 *
	 * \post String capacity has value of smallest (2^n)-1 greater or equal to \e size.
	 */
	void reserve( size_type capacity, int rank );
	/*! \brief Increases string capacity for current rank.
	 *
	 * String capacity is always size of (2^n)-1 or MAX_INPLACE_CAPACITY.
	 *
	 * \param capacity - new string capacity.
	 *
	 * \post String capacity has value of smallest (2^n)-1 greater or equal to \e size.
	 */
	void reserve( size_type capacity );
	/*! \brief Construct string based on UTF-8 string.
	 *
	 * str - an UTF-8 encoded string.
	 */
	HString( HUTF8String const& str );
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
	HString( HConstIterator first_, HConstIterator last_ );
	/*! \brief Construct string based on character array.
	 *
	 * array - a character array where to initialize new HString object with.
	 * size - at most that many characters from array are used.
	 */
	HString( char const* array, size_type size );
	HString( void const* array, size_type size );
	HString( char16_t const* array, size_type size );
	HString( char32_t const* array, size_type size );
	/*! \brief Construct new HString from single code point.
	 *
	 * ch - a code point to initialize new string with.
	 */
	HString( code_point_t ch );
	/*! \brief Construct new HString from a single character.
	 *
	 * ch - a character to initialize new string with.
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
	/*! \brief Move assignment operator.
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
	/*! \brief Get character at given position.
	 *
	 * \param position - index of a position to get character for.
	 * \return character at given position in this string.
	 */
	code_point_t const operator[] ( size_type position ) const;
	/*! \brief Check if string has some content.
	 *
	 * \return True iff this string is not empty.
	 */
	explicit operator bool ( void ) const;
	/*! \brief Set given character at given position.
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
	void set_at( size_type position, code_point_t character );
	/*! \brief Get access to first character in the string.
	 *
	 * String must be non-empty.
	 */
	code_point_t front( void ) const;
	/*! \brief Get access to last character in the string.
	 *
	 * String must be non-empty.
	 */
	code_point_t back( void ) const;
	/*! \brief Get maximum number of characters that can be stored in any string.
	 *
	 * Alias for HString::get_max_size().
	 *
	 * \return Maximum number of characters that can be stored in any string.
	 */
	size_type max_size( void ) const;
	/*! \brief Get maximum number of characters that can be stored in any string.
	 *
	 * \return Maximum number of characters that can be stored in any string.
	 */
	size_type get_max_size( void ) const;
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
	const_iterator cbegin( void ) const;
	/*! \brief Get iterator pointing to end (one past last character) of this string.
	 *
	 * \return Iterator pointing to end (terminating \0) of this string.
	 */
	const_iterator end( void ) const;
	const_iterator cend( void ) const;
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
	size_type get_length( void ) const;
	/*! \brief Get string length.
	 *
	 * length() is really an alias for get_length().
	 *
	 * \return String length in characters.
	 */
	size_type length( void ) const;
	/*! \brief Get string length.
	 *
	 * get_size() is really an alias for get_length().
	 *
	 * \return String length in characters.
	 */
	size_type get_size( void ) const;
	/*! \brief Get string length.
	 *
	 * size() is really an alias for get_length().
	 *
	 * \return String length in characters.
	 */
	size_type size( void ) const;
	/*! \brief Tell how many characters excluding terminating NIL can this string store without memory reallocation.
	 *
	 * \return Number of characters excluding terminating NIL that can be stored in this string without reallocation.
	 */
	size_type get_capacity( void ) const;
	/*! \brief Tell how many characters excluding terminating NIL can this string store without memory reallocation.
	 *
	 * capacity() is really an alias for get_capacity().
	 *
	 * \return Number of characters excluding terminating NIL that can be stored in this string without reallocation.
	 */
	size_type capacity( void ) const;
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
	HString& assign( char const* data, size_type length );
	HString& assign( void const* data, size_type length );
	HString& assign( char16_t const* data, size_type length );
	HString& assign( char32_t const* data, size_type length );
	/*! \brief Erase old content and assign a substring of given string.
	 *
	 * \param str - assign substring of this string.
	 * \param offset - an offset of a substring to get.
	 * \param length - a length of substring to get.
	 * \return Self.
	 */
	HString& assign( HString const& str, size_type offset = 0, size_type length = npos );
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
	HString& assign( size_type size_, code_point_t fill_ );
	/*! \brief Find position of given character in this string.
	 *
	 * \param character - a character to look for.
	 * \param offset - position where searching for given character starts.
	 * \return Position of given character in this string if given character is found or HString::npos otherwise.
	 */
	size_type find( code_point_t character, size_type offset = 0 ) const;
	/*! \brief Find position of given sub-string in this string.
	 *
	 * \param str - a sub-string to look for.
	 * \param offset - position where searching for given sub-string starts.
	 * \return Position of given sub-string (first character of it) in this string if given sub-string is found or HString::npos otherwise.
	 */
	size_type find( HString const& str, size_type offset = 0 ) const;
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
	size_type reverse_find( code_point_t character, size_type before = 0 ) const;
	/*! \brief Find index of last occurrence of given character.
	 *
	 * This method is totally different than HString::reverse_find().
	 *
	 * \param character - look for position of this character in this string.
	 * \param before - assume that this string is only that long.
	 * \return Index of last occurrence of given character if given character can be found or HString::npos otherwise.
	 */
	size_type find_last( code_point_t character, size_type before = npos ) const;
	/*! \brief Find index of last occurrence of given sub-string.
	 *
	 * This method is totally different than HString::reverse_find().
	 *
	 * \param str - look for position of this sub-string in this string.
	 * \param before - assume that this string is only that long.
	 * \return Index of last occurrence of given sub-string if given sub-string can be found or HString::npos otherwise.
	 */
	size_type find_last( HString const& str, size_type before = npos ) const;
	size_type find_one_of( HString const&, size_type = 0 ) const;
	size_type reverse_find_one_of( HString const&, size_type = 0 ) const;
	size_type find_last_one_of( HString const&, size_type = npos ) const;
	size_type find_other_than( HString const&, size_type = 0 ) const;
	size_type reverse_find_other_than( HString const&, size_type = 0 ) const;
	size_type find_last_other_than( HString const&, size_type = npos ) const;
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
	HString& replace( size_type pos_, size_type size_, HString const& replacement );
	/*! \brief Replace part of a this string with supplied substring.
	 *
	 * \param pos_ - where replaced part starts.
	 * \param size_ - size of replaced part.
	 * \param replacement - replacement source for given part.
	 * \param offset_ - where replacement part starts in replacement source.
	 * \param len_ - length of replacement.
	 * \return Self.
	 */
	HString& replace( size_type pos_, size_type size_, HString const& replacement, size_type offset_, size_type len_ );
	/*! \brief Replace part of a this string with supplied character buffer.
	 *
	 * \param pos_ - where replaced part starts.
	 * \param size_ - size of replaced part.
	 * \param buffer_ - replacement source for given part.
	 * \param len_ - length of replacement.
	 * \return Self.
	 */
	HString& replace( size_type pos_, size_type size_, char const* buffer_, size_type len_ );
	/*! \brief Replace part of a this string with supplied character buffer.
	 *
	 * \param pos_ - where replaced part starts.
	 * \param size_ - size of replaced part.
	 * \param count_ - length of replacement.
	 * \param value_ - copies of this value shall be used of replacement.
	 * \return Self.
	 */
	HString& replace( size_type pos_, size_type size_, int long count_, code_point_t value_ );
	/*! \brief Replace range in this string with supplied string.
	 *
	 * \param first_ - where replaced range starts.
	 * \param last_ - where replaced range ends.
	 * \param replacement - replacement for given part.
	 * \return Self.
	 */
	HString& replace( iterator first_, iterator last_, HString const& replacement );
	/*! \brief Perform lexicographical comparison of the (sub-)strings.
	 *
	 * \param other - the string that shall be compared with this string.
	 * \param from - compare portion of \e other string starting at this position.
	 * \param len - use at most \e len characters for comparison.
	 * \return 0 iff compared (sub)strings are equal, -1 if this string comes lexicographically before \e other, 1 otherwise.
	 */
	int compare( HString const& other, size_type from = 0, size_type len = npos ) const;
	/*! \brief Perform lexicographical comparison of the (sub-)strings.
	 *
	 * \param thisFrom - compare portion of \e this string starting at \e thisFrom position.
	 * \param thisLen - use at most \e thisLen characters from this string for comparison.
	 * \param other - the string that shall be compared with this string.
	 * \param from - compare portion of \e other string starting at this position.
	 * \param len - use at most \e len characters from \e other string for comparison.
	 * \return 0 iff compared (sub)strings are equal, -1 if this string comes lexicographically before \e other, 1 otherwise.
	 */
	int compare( size_type thisFrom, size_type thisLen, HString const& other, size_type from = 0, size_type len = npos ) const;
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
	HString substr( size_type start, size_type length = npos ) const;
	/*! \brief Get left part of this string of given length.
	 *
	 * \param len - length of left part of this string to get.
	 * \param left part of this string with at most len characters.
	 */
	HString left( size_type len ) const;
	/*! \brief Get substring of this string by start position and length.
	 *
	 * mid() is really an alias for substr().
	 *
	 * \param start - position for beginning of a substring.
	 * \param length - length of a substring.
	 * \return Substring.
	 */
	HString mid( size_type start, size_type length = npos ) const;
	/*! \brief Get right part of this string of given length.
	 *
	 * \param len - length of right part of this string to get.
	 * \param right part of this string with at most len characters.
	 */
	HString right( size_type len ) const;
	/*! \brief Trim all consecutive occurrences of given characters from beginning of the string.
	 *
	 * \param set - set of characters that shall be removed.
	 * \return Self.
	 */
	HString& trim_left( HString const& set = character_class<CHARACTER_CLASS::WHITESPACE>().data() );
	/*! \brief Trim all consecutive occurrences of given characters from end of the string.
	 *
	 * \param set - set of characters that shall be removed.
	 * \return Self.
	 */
	HString& trim_right( HString const& set = character_class<CHARACTER_CLASS::WHITESPACE>().data() );
	/*! \brief Trim all consecutive occurrences of given characters from both beginning and end of the string.
	 *
	 * \param set - set of characters that shall be removed.
	 * \return Self.
	 */
	HString& trim( HString const& set = character_class<CHARACTER_CLASS::WHITESPACE>().data() );
	/*! \brief Remove first N characters from this string.
	 * \param len - remove that many characters from beginning of this string.
	 * \return Self.
	 */
	HString& shift_left( size_type len );
	HString& shift_right( size_type, code_point_t = ' '_ycp );
	/*! \brief Fill portion of string with constant value.
	 *
	 * \param value - use this value as a filler.
	 * \param position - start at this position.
	 * \param count - fill that many bytes.
	 * \return Self.
	 */
	HString& fill( code_point_t value = 0_ycp, size_type position = 0, size_type length = npos );
	/*! \brief Fill portion of string with constant value and finish with \0 (zero) byte.
	 *
	 * \param value - use this value as a filler.
	 * \param position - start at this position.
	 * \param count - fill that many bytes.
	 * \return Self.
	 */
	HString& fillz( code_point_t value = 0_ycp, size_type position = 0, int long count = npos );
	/*! \brief Erase part of the string.
	 *
	 * \param position_ - start position for part to be erased.
	 * \param length_ - length of part to be erased.
	 * \return Self.
	 */
	HString& erase( size_type position_ = 0, size_type length_ = npos );
	/*! \brief Insert given string at given position.
	 *
	 * \param pos - position where given string has to be inserted.
	 * \param str - string to be inserted.
	 * \return Self.
	 */
	HString& insert( size_type pos, HString const& str );
	/*! \brief Insert given string at given position.
	 *
	 * \param pos_ - position where given string has to be inserted.
	 * \param str_ - a (sub-) string to be inserted.
	 * \param offset_ - an offset of a substring to insert.
	 * \param len_ - length of a (sub-) string to be inserted.
	 * \return Self.
	 *
	 * \pre \e str real length must not be smaller than \e len.
	 */
	HString& insert( size_type pos_, HString const& str_, size_type offset_, size_type len_ = npos );
	/*! \brief Insert given string at given position.
	 *
	 * \param pos - position where given string has to be inserted.
	 * \param str - string to be inserted.
	 * \return Self.
	 */
	HString& insert( size_type pos, char const* str = nullptr );
	/*! \brief Insert given string at given position.
	 *
	 * \param pos - position where given string has to be inserted.
	 * \param str - string to be inserted.
	 * \param len - length of string to be inserted.
	 * \return Self.
	 *
	 * \pre \e str real length must not be smaller than \e len.
	 */
	HString& insert( size_type pos, char const* str, size_type len );
	/*! \brief Insert n copies of value before given position.
	 *
	 * \param position - where put inserted block.
	 * \param n - number of bytes to insert.
	 * \param value - copies of this value shall be inserted.
	 */
	HString& insert( size_type position, int long n, code_point_t value );
	HString& append( HString const& );
	/*! \brief Append a substring of given string to this string.
	 *
	 * \param str - append substring of this string.
	 * \param offset - an offset of a substring to get.
	 * \param length - a length of substring to get.
	 * \return Self.
	 */
	HString& append( HString const&, size_type idx_, size_type len_ = npos );
	/*! \brief Append n copies of value at end of the string.
	 *
	 * \param position - where put inserted block.
	 * \param n - number of bytes to insert.
	 * \param value - copies of this value shall be inserted.
	 */
	HString& append( int long count_, code_point_t val_ );
	/*! \brief Append new data from buffer to already existing data in this string.
	 *
	 * \param data - data to be appended to this string.
	 * \param size - size of the date to be appended.
	 * \return Self.
	 */
	HString& append( char const*, size_type len_ );
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
	void push_back( code_point_t character_ );
	/*! \brief Remove single character from end of this string.
	 */
	void pop_back( void );
	/*! \brief Check if given string starts with other string.
	 *
	 * \param other - string to check against.
	 * \return True iff \e other is a prefix of this string.
	 */
	bool starts_with( HString const& other ) const;
	/*! \brief Check if given string ends with other string.
	 *
	 * \param other - string to check against.
	 * \return True iff \e other is a suffix of this string.
	 */
	bool ends_with( HString const& other ) const;
private:
	void from_utf8( size_type, size_type, char const*, size_type );
	void substr( HString&, size_type, size_type ) const;
	void resize( size_type capacity, int rank );
	void replace_check( size_type, size_type, size_type, size_type, size_type );
	size_type find( char, size_type = 0 ) const = delete;
	size_type find_last( char, size_type = npos ) const = delete;
	HString& assign( size_type, char ) = delete;
	HString& append( size_type, char ) = delete;
	HString& insert( size_type, size_type, char ) = delete;
	HString& replace( size_type, size_type, size_type, char ) = delete;
};

class HString::HCharRef {
	HString& _string;
	size_type _index;
public:
	operator code_point_t ( void ) {
		return ( _string[ _index ] );
	}
	code_point_t get( void ) const {
		return ( _string[ _index ] );
	}
	HCharRef& operator = ( code_point_t ch_ );
	bool operator == ( HCharRef const& other_ ) const {
		return ( get() == other_.get() );
	}
	bool operator == ( code_point_t other_ ) const {
		return ( get() == other_ );
	}
	friend bool operator == ( code_point_t left_, HCharRef const& right_ ) {
		return ( left_ == right_.get() );
	}
	bool operator != ( HCharRef const& other_ ) const {
		return ( get() != other_.get() );
	}
	bool operator != ( code_point_t other_ ) const {
		return ( get() != other_ );
	}
	friend bool operator != ( code_point_t left_, HCharRef const& right_ ) {
		return ( left_ != right_.get() );
	}
	bool operator < ( HCharRef const& other_ ) const {
		return ( get() < other_.get() );
	}
	bool operator < ( code_point_t other_ ) const {
		return ( get() < other_ );
	}
	friend bool operator < ( code_point_t left_, HCharRef const& right_ ) {
		return ( left_ < right_.get() );
	}
	bool operator <= ( HCharRef const& other_ ) const {
		return ( get() <= other_.get() );
	}
	bool operator <= ( code_point_t other_ ) const {
		return ( get() <= other_ );
	}
	friend bool operator <= ( code_point_t left_, HCharRef const& right_ ) {
		return ( left_ <= right_.get() );
	}
	bool operator > ( HCharRef const& other_ ) const {
		return ( get() > other_.get() );
	}
	bool operator > ( code_point_t other_ ) const {
		return ( get() > other_ );
	}
	friend bool operator > ( code_point_t left_, HCharRef const& right_ ) {
		return ( left_ > right_.get() );
	}
	bool operator >= ( HCharRef const& other_ ) const {
		return ( get() >= other_.get() );
	}
	bool operator >= ( code_point_t other_ ) const {
		return ( get() >= other_ );
	}
	friend bool operator >= ( code_point_t left_, HCharRef const& right_ ) {
		return ( left_ >= right_.get() );
	}
	void swap( HCharRef& );
private:
	friend class HString;
	HCharRef( HString& string_, size_type index_ )
		: _string( string_ )
		, _index( index_ ) {
		return;
	}
};

class HString::HConstIterator : public yaal::hcore::iterator_interface<code_point_t, yaal::hcore::iterator_category::random_access> {
protected:
	HString const* _owner;
	size_type _index;
public:
	HConstIterator( void )
		: _owner( nullptr )
		, _index( HString::npos ) {
		return;
	}
	HConstIterator( HConstIterator const& it_ )
		: _owner( it_._owner )
		, _index( it_._index ) {
		return;
	}
	HConstIterator& operator = ( HConstIterator const& it_ ) {
		_owner = it_._owner;
		_index = it_._index;
		return ( *this );
	}
	bool operator == ( HConstIterator const& it_ ) const {
		M_ASSERT( _owner == it_._owner );
		return ( _index == it_._index );
	}
	bool operator != ( HConstIterator const& it_ ) const {
		M_ASSERT( _owner == it_._owner );
		return ( _index != it_._index );
	}
	HConstIterator& operator ++ ( void ) {
		M_ASSERT( _owner );
		++ _index;
		return ( *this );
	}
	HConstIterator& operator -- ( void ) {
		M_ASSERT( _owner );
		-- _index;
		return ( *this );
	}
	HConstIterator operator ++ ( int ) {
		HConstIterator it( *this );
		++ _index;
		return it;
	}
	HConstIterator operator -- ( int ) {
		HConstIterator it( *this );
		-- _index;
		return it;
	}
	HConstIterator& operator += ( int offset_ ) {
		_index += offset_;
		return ( *this );
	}
	HConstIterator& operator += ( size_type offset_ ) {
		_index += offset_;
		return ( *this );
	}
	HConstIterator& operator -= ( int offset_ ) {
		_index -= offset_;
		return ( *this );
	}
	HConstIterator& operator -= ( size_type offset_ ) {
		_index -= offset_;
		return ( *this );
	}
	HConstIterator operator + ( int offset_ ) {
		return ( HConstIterator( _owner, _index + offset_ ) );
	}
	HConstIterator operator + ( size_type offset_ ) {
		return ( HConstIterator( _owner, _index + offset_ ) );
	}
	HConstIterator operator - ( int offset_ ) {
		return ( HConstIterator( _owner, _index - offset_ ) );
	}
	HConstIterator operator - ( size_type offset_ ) {
		return ( HConstIterator( _owner, _index - offset_ ) );
	}
	bool operator < ( HConstIterator const& it_ ) const {
		return ( _index < it_._index );
	}
	bool operator <= ( HConstIterator const& it_ ) const {
		return ( _index <= it_._index );
	}
	bool operator > ( HConstIterator const& it_ ) const {
		return ( _index > it_._index );
	}
	bool operator >= ( HConstIterator const& it_ ) const {
		return ( _index >= it_._index );
	}
	size_type operator - ( HConstIterator const& it_ ) const {
		return ( _index - it_._index );
	}
	code_point_t operator * ( void ) const {
		return ( (*_owner)[ _index ] );
	}
private:
	friend class HString;
	HConstIterator( HString const* owner_, size_type index_ )
		: _owner( owner_ )
		, _index( index_ ) {
		return;
	}
};

class HString::HIterator : public HString::HConstIterator {
public:
	HIterator( void )
		: HConstIterator() {
		return;
	}
	HIterator( HConstIterator const& it_ )
		: HConstIterator( it_._owner,  it_._index ) {
		return;
	}
	HIterator( HIterator const& it_ ) = default;
	HIterator& operator = ( HIterator const& it_ ) {
		_owner = it_._owner;
		_index = it_._index;
		return ( *this );
	}
	HCharRef operator * ( void ) {
		return ( HCharRef( *const_cast<HString*>( _owner ), _index ) );
	}
private:
	friend class HString;
	HIterator( HString* owner_, size_type index_ )
		: HConstIterator( owner_,  index_ ) {
		return;
	}
};

/*! \brief HUTF8String is a class representing UTF-8 string buffer.
 *
 * This class does not allow mutating operations.
 * HUTF8String provides reference counting based (shallow) copy operation.
 * It also provides cheap substring via "views".
 * Characters viewable in given HUTF8String instance are "active".
 *
 * Let's say that:
 * U - is a head of UTF-8 sequence
 * u - is a tail of UTF-8 sequence
 * a - is an ASCII character
 * then, if:
 * HUTF8String s( "aaaUuaUuuaa" );
 *
 * s has 11 bytes, but
 * s has 8 charactes
 *
 * and if s._offset == 3 and s._byteCount == 6
 * then s is a view and:
 *
 * s has 6 bytes, but
 * s has 3 charactes
 *
 */
class HUTF8String final {
public:
	typedef HUTF8String this_type;
	typedef HString::size_type size_type;
	class HIterator;
	typedef HIterator iterator;
	typedef HIterator const_iterator;
	typedef HReverseIterator<HIterator> reverse_iterator;
	typedef reverse_iterator const_reverse_iterator;
private:

#ifndef __sun__
#	pragma pack( push, 1 )
#else /* #ifndef __sun__ */
#	pragma pack( 1 )
#endif /* #else #ifndef __sun__ */

	struct OBufferMeta {
		int _allocated;     /*!< buffer (_ptr) size not including OBufferMeta */
		int _used;          /*!< number of bytes used bu this instance */
		int _refCount;      /*!< number of copies of this string */
		yaal::i8_t _rank;   /*!< maximum number of bytes used per character */
	};

#ifndef __sun__
#	pragma pack( pop )
#else /* #ifndef __sun__ */
#	pragma pack()
#endif /* #else #ifndef __sun__ */

	int _characterCount;  /*!< active utf-8 character count */
	int _offset;          /*!< active string start offset counted in bytes!! */
	int _byteCount;       /*!< raw active bytes count */
	union {
		OBufferMeta* _meta; /*!< this string metadata view */
		char* _ptr;         /*!< shared memory buffer containing metadata at its beginning */
	};
public:
	HUTF8String( void );
	HUTF8String( HString const& );
	HUTF8String( char const* );
	HUTF8String( const_iterator, const_iterator );
	HUTF8String( HString::const_iterator, HString::const_iterator );
	HUTF8String( HUTF8String const& );
	HUTF8String( HUTF8String&& ) noexcept;
	HUTF8String& operator = ( HString const& );
	HUTF8String& operator = ( HUTF8String const& );
	HUTF8String& operator = ( HUTF8String&& );
	~HUTF8String( void );
	bool operator == ( HUTF8String const& ) const;
	bool operator != ( HUTF8String const& ) const;
	bool operator == ( char const* ) const;
	bool operator != ( char const* ) const;
	void swap( HUTF8String& );
	char const* c_str( void ) const;
	char const* raw( void ) const;
	bool is_empty( void ) const;
	bool empty( void ) const;
	size_type byte_count( void ) const;
	size_type character_count( void ) const;
	int rank( void ) const;
	void assign( HString const& );
	void assign( HString::const_iterator, HString::const_iterator );
	void reset( void );
	HIterator begin( void ) const;
	HIterator end( void ) const;
	HIterator cbegin( void ) const;
	HIterator cend( void ) const;
	reverse_iterator rbegin( void ) const;
	reverse_iterator rend( void ) const;
	reverse_iterator crbegin( void ) const;
	reverse_iterator crend( void ) const;
	HUTF8String substr( size_type, size_type = HString::npos ) const;
	code_point_t front( void ) const;
private:
	/*! Allocate memory buffer.
	 *
	 * Used from assign( it, it ) and from ctor( char const* ).
	 *
	 * \param size_ - number of bytes to be allocated NOT INCLUDING terminating NIL or OBufferMeta.
	 */
	void alloc( size_type size_ );
};

class HUTF8String::HIterator final : public iterator_interface<code_point_t, iterator_category::random_access> {
public:
	typedef code_point_t reference;
private:
	union {
		OBufferMeta* _meta; /*!< this string metadata view */
		char* _ptr;         /*!< shared memory buffer containing metadata at its beginning */
	};
	int _characterIndex;
	int _byteIndex;
public:
	HIterator( void );
	HIterator( HIterator const& );
	HIterator( HIterator&& ) noexcept;
	~HIterator( void );
	HIterator& operator = ( HIterator const& );
	HIterator& operator = ( HIterator&& );
	bool operator == ( HIterator const& other_ ) const {
		M_ASSERT( ( _ptr && other_._ptr && ( _ptr == other_._ptr ) ) || ! ( _ptr || other_._ptr ) );
		return ( ( _ptr == other_._ptr ) && ( _characterIndex == other_._characterIndex ) );
	}
	bool operator != ( HIterator const& other_ ) const {
		M_ASSERT( ( _ptr && other_._ptr && ( _ptr == other_._ptr ) ) || ! ( _ptr && other_._ptr ) );
		return ( ( _ptr != other_._ptr ) || ( _characterIndex != other_._characterIndex ) );
	}
	bool operator < ( HIterator const& other_ ) const {
		M_ASSERT( _ptr && other_._ptr && ( _ptr == other_._ptr ) );
		return ( _characterIndex < other_._characterIndex );
	}
	bool operator <= ( HIterator const& other_ ) const {
		M_ASSERT( _ptr && other_._ptr && ( _ptr == other_._ptr ) );
		return ( _characterIndex <= other_._characterIndex );
	}
	bool operator > ( HIterator const& other_ ) const {
		M_ASSERT( _ptr && other_._ptr && ( _ptr == other_._ptr ) );
		return ( _characterIndex > other_._characterIndex );
	}
	bool operator >= ( HIterator const& other_ ) const {
		M_ASSERT( _ptr && other_._ptr && ( _ptr == other_._ptr ) );
		return ( _characterIndex >= other_._characterIndex );
	}
	HIterator& operator ++ ( void );
	HIterator operator ++ ( int ) {
		HIterator it( *this );
		operator ++ ();
		return it;
	}
	HIterator& operator -- ( void );
	HIterator operator -- ( int ) {
		HIterator it( *this );
		operator -- ();
		return it;
	}
	HIterator& operator += ( size_type );
	HIterator operator + ( size_type by_ ) const {
		HIterator it( *this );
		it += by_;
		return it;
	}
	HIterator& operator -= ( size_type );
	HIterator operator - ( size_type by_ ) const {
		HIterator it( *this );
		it -= by_;
		return it;
	}
	size_type operator - ( HIterator const& other_ ) const {
		return ( _characterIndex - other_._characterIndex );
	}
	code_point_t operator * ( void ) const;
	void swap( HIterator& );
private:
	HIterator( char*, int, int );
	void reset( void );
	friend class HUTF8String;
};

HString operator "" _ys ( char const*, size_t );
HString operator + ( char const*, HString const& );
bool operator == ( char const*, HString const& );
bool operator != ( char const*, HString const& );
bool operator >= ( char const*, HString const& );
bool operator <= ( char const*, HString const& );
bool operator > ( char const*, HString const& );
bool operator < ( char const*, HString const& );
bool operator == ( char const*, HUTF8String const& );
bool operator != ( char const*, HUTF8String const& );
int stricasecmp( HString const&, HString const& );
int collate_compare( HString const&, HString const& );
int long icasesearch( HString const&, HString const& );
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
	return v_;
}
void copy_ascii( HString const&, char*, int );
void copy_ascii( HString::const_iterator, HString::const_iterator, char*, int );
void copy_all_to_ascii( HString const&, char*, int );
void copy_all_to_ascii( HString::const_iterator, HString::const_iterator, char*, int );
int stoi( HString const&, int* = nullptr, int = 10 );
int long stol( HString const&, int* = nullptr, int = 10 );
int long unsigned stoul( HString const&, int* = nullptr, int = 10 );
int long long stoll( HString const&, int* = nullptr, int = 10 );
int long long unsigned stoull( HString const&, int* = nullptr, int = 10 );
float stof( HString const&, int* = nullptr );
double stod( HString const&, int* = nullptr );
double long stold( HString const&, int* = nullptr );
bool is_whitespace( code_point_t );
bool is_digit( code_point_t );
bool is_dec_digit( code_point_t );
bool is_hex_digit( code_point_t );
bool is_oct_digit( code_point_t );
bool is_bin_digit( code_point_t );
bool is_letter( code_point_t );
bool is_upper( code_point_t );
bool is_lower( code_point_t );
bool is_alpha( code_point_t );
bool is_alnum( code_point_t );
bool is_punct( code_point_t );
bool is_ascii( code_point_t );
bool is_greek( code_point_t );
bool is_upper_greek( code_point_t );
bool is_lower_greek( code_point_t );
bool is_subscript( code_point_t );
bool is_superscript( code_point_t );
bool is_pua( code_point_t );
template<CHARACTER_CLASS>
bool is_pua( code_point_t );

inline void swap( yaal::hcore::HString& a, yaal::hcore::HString& b ) {
	a.swap( b );
}

inline void swap( yaal::hcore::HString::HCharRef a, yaal::hcore::HString::HCharRef b ) {
	a.swap( b );
}

inline void swap( yaal::hcore::HUTF8String& a, yaal::hcore::HUTF8String& b ) {
	a.swap( b );
}

}

}

#endif /* #ifndef YAAL_HCORE_HSTRING_HXX_INCLUDED */

