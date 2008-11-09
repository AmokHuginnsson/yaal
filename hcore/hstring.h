/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	hstring.h - this file is integral part of `yaal' project.

	i.  You may not make any changes in Copyright information.
	ii. You must attach Copyright information to any part of every copy
	    of this software.

Copyright:

 You are free to use this program as is, you can redistribute binary
 package freely but:
  1. You can not use any part of sources of this software.
  2. You can not redistribute any part of sources of this software.
  3. No reverse engineering is allowed.
  4. If you want redistribute binary package you can not demand any fees
     for this software.
     You can not even demand cost of the carrier (CD for example).
  5. You can not include it to any commercial enterprise (for example 
     as a free add-on to payed software or payed newspaper).
 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE. Use it at your own risk.
*/

#ifndef __YAAL_HCORE_HSTRING_H
#define __YAAL_HCORE_HSTRING_H

#include <limits.h>

namespace yaal
{

namespace hcore
{

/*! \brief Implementation of high level string operations.
 */
class HString
	{
	typedef HString self_t;
private:
	char*	f_pcBuffer; /*!< memory buffer pointer */
	int	long f_lAllocatedBytes; /*!< size of memory buffer */
	int	long f_lSize; /*!< lenght of string */
public:
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
	HString( char const ch );
	/*! \brief Construct new HString from integer.
	 *
	 * integer - an integer to initialize new string with.
	 */
	HString( int const integer );
	/*! \brief Construct new HString from unsigned integer.
	 *
	 * unsignedInteger - an unsigned integer to initialize new string with.
	 */
	HString( int unsigned const unsignedInteger );
	/*! \brief Construct new HString from long integer.
	 *
	 * longInteger - a long integer to initialize new string with.
	 */
	HString( int long const longInteger );
	/*! \brief Construct new HString from unsigned long integer.
	 *
	 * unsignedLongInteger - a long integer to initialize new string with.
	 */
	HString( int long unsigned const unsignedLongInteger );
	/*! \brief Construct new HString from double.
	 *
	 * real - a double to initialize new string with.
	 */
	HString( double const real );
	/*! \brief Construct new HString from long double.
	 *
	 * longDouble - a double to initialize new string with.
	 */
	HString( double long const longDouble );
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
	void swap( HString& );
	HString& format( char const* const, ... );
	HString& vformat( char const* const, void* );
	int long find( char const, int long = 0 ) const;
	int long find( HString const&, int long = 0 ) const;
	int long nfind( HString const&, int long, int long = 0 ) const;
	int long reverse_find( char const, int long = 0 ) const;
	int long find_one_of( char const* const, int long = 0 ) const;
	int long reverse_find_one_of( char const* const, int long = 0 ) const;
	int long find_other_than( char const* const, int long = 0 ) const;
	int long reverse_find_other_than( char const* const, int long = 0 ) const;
	HString& replace( HString const&, HString const& );
	HString& upper( void );
	HString& lower( void );
	HString& reverse( void );
	HString left( int const ) const;
	HString mid( int long, int long = INT_MAX ) const;
	HString right( int long const& ) const;
	HString& trim_left( char const* const = " \t\r\n" );
	HString& trim_right( char const* const = " \t\r\n" );
	HString& shift_left( int long const );
	HString& shift_right( int long const, char const = ' ' );
	HString split( char const* const, int const ) const;
	HString& fill( char = '\0', int long = 0, int long = 0 );
	HString& fillz( char = '\0', int long = 0, int long = 0 );
	HString& erase( int long, int long );
	HString& insert( int long, int long, char const* = NULL );
	};

HString operator + ( HString const&, HString const& );
int strcasecmp( HString const&, HString const& );
bool operator == ( HString const&, HString const& );
bool operator != ( HString const&, HString const& );
bool operator >= ( HString const&, HString const& );
bool operator <= ( HString const&, HString const& );
bool operator > ( HString const&, HString const& );
bool operator < ( HString const&, HString const& );

namespace string_helper
{

int long kmpcasesearch( char const* const, int long const&, char const* const, int long const& );

}

extern char const n_pcWhiteSpace[];

}

inline void swap( yaal::hcore::HString& a, yaal::hcore::HString& b )
	{ a.swap( b ); }

}

#endif /* not __YAAL_HCORE_HSTRING_H */

