/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	hstring.hxx - this file is integral part of `yaal' project.

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

#ifndef YAAL_HCORE_HSTRING_HXX_INCLUDED
#define YAAL_HCORE_HSTRING_HXX_INCLUDED 1

#include <cstddef>

#include "hcore/macro.hxx"
#include "hcore/numeric.hxx"

namespace yaal
{

namespace hcore
{

extern M_YAAL_HCORE_PUBLIC_API char const _whiteSpace_[];
extern M_YAAL_HCORE_PUBLIC_API char const _digit_[];
extern M_YAAL_HCORE_PUBLIC_API char const _letter_[];
extern M_YAAL_HCORE_PUBLIC_API char const _word_[];

/*! \brief Implementation of high level string operations.
 */
class HString
	{
	static int long const MAX_STRING_LENGTH = ( meta::max_signed<int long>::value / 2 ) - 1;
	typedef HString this_type;
private:
	char*	_buffer; /*!< memory buffer pointer */
	int	long _allocatedBytes; /*!< size of memory buffer */
	int	long _size; /*!< lenght of string */
public:
	static int long const npos = -1;
	typedef char const* iterator; /*!< mutable iterator for string characters */
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
	HString( int const size, bool const );
	~HString( void );
	/*! \brief Increases string capacity.
	 *
	 * String capacity is always size of 2^n.
	 *
	 * \param size - new size capacity.
	 *
	 * \post String capacity has value of smallest 2^n greater or equal to \e size.
	 */
	void hs_realloc( int long const size );
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
/*
	iterator begin( void ) const;
	iterator end( void ) const;
*/
	const_iterator begin( void ) const;
	const_iterator end( void ) const;
	bool is_empty( void ) const;
	bool empty( void ) const;
	void clear( void );
	int long length( void ) const;
	int long size( void ) const;
	int long get_length( void ) const;
	int long capacity( void ) const;
	int long get_capacity( void ) const;
	void swap( HString& );
	HString& assign( char const* const, int long );
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
	HString left( int long ) const;
	/*! \brief Get substring of this string by start position and length.
	 *
	 * \param start - position for begining of a substring.
	 * \param length - length of a substring.
	 * \return Substring.
	 */
	HString mid( int long start, int long length = MAX_STRING_LENGTH ) const;
	HString right( int long ) const;
	HString& trim_left( char const* const = _whiteSpace_ );
	HString& trim_right( char const* const = _whiteSpace_ );
	HString& shift_left( int long );
	HString& shift_right( int long, char const = ' ' );
	/*! \brief Fill portion of string with constatnt value.
	 *
	 * \param value - use this value as a filler.
	 * \param position - start at this position.
	 * \param count - fill that many bytes.
	 */
	HString& fill( char value = '\0', int long position = 0, int long lenght = 0 );
	/*! \brief Fill portion of string with constatnt value and finish with \0 (zero) byte.
	 *
	 * \param value - use this value as a filler.
	 * \param position - start at this position.
	 * \param count - fill that many bytes.
	 */
	HString& fillz( char value = '\0', int long position = 0, int long count = 0 );
	/*! \brief Erase part of the string.
	 *
	 * \param position_ - start position for part to be erased.
	 * \param length_ - length of part to be erased.
	 * \return self.
	 */
	HString& erase( int long position_, int long length_ = MAX_STRING_LENGTH );
	/*! \brief Insert given string at given position.
	 *
	 * \param pos - position where given string has to be inserted.
	 * \param str - string to be inserted.
	 * \return self.
	 */
	HString& insert( int long pos, HString const& str );
	/*! \brief Insert given string at given position.
	 *
	 * \param pos - position where given string has to be inserted.
	 * \param len - lenght of string to be inserted.
	 * \param str - string to be inserted.
	 * \return self.
	 *
	 * \pre \e str real length must not be smaller than \e len.
	 */
	HString& insert( int long pos, int long len, char const* str = NULL );
	/*! \brief Insert given string at given position.
	 *
	 * \param pos - position where given string has to be inserted.
	 * \param str - string to be inserted.
	 * \return self.
	 */
	HString& insert( int long pos, char const* str = NULL );
	/*! \brief Insert given string at given position.
	 *
	 * \param pos - position where given string has to be inserted.
	 * \param str - string to be inserted.
	 * \param len - lenght of string to be inserted.
	 * \return self.
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
	HString& append( HString const&, int long idx_, int long len_ );
	HString& append( int long count_, char val_ );
	HString& append( char const* const, int long len_ );
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
namespace string_helper
{

int long kmpcasesearch( char const* const, int long, char const* const, int long );

}

}

inline void swap( yaal::hcore::HString& a, yaal::hcore::HString& b )
	{ a.swap( b ); }

}

#endif /* #ifndef YAAL_HCORE_HSTRING_HXX_INCLUDED */

