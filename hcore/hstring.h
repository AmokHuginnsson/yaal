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

class HString
	{
private:
	char*	f_pcBuffer;
	int	long f_lSize;
public:
	HString( void );
	HString( HString const& );
	HString( int const, bool const ); /* initialize immediately with size */
	virtual ~HString( void ) ;
	void hs_realloc( int long const );
	HString( char const* const );
	HString( char const );
	HString( int const );
	HString( int unsigned const );
	HString( int long const );
	HString( int long unsigned const );
	HString( double const );
	HString( double long const );
	HString( void const* const );
	HString& operator = ( HString const& );
	HString& operator += ( HString const& );
	HString operator + ( HString const& ) const;
	HString operator + ( char const* ) const;
	HString operator + ( char const ) const;
	HString operator + ( int const ) const;
	HString operator + ( int long const ) const;
	HString operator + ( size_t const ) const;
	HString operator + ( double const ) const;
	HString operator + ( double long const ) const;
	HString operator + ( void const* const ) const;
	char operator[] ( int const );
	char operator[] ( int long const );
	char set_at( int long, char );
	bool operator == ( HString const& ) const;
	bool operator == ( char const* const ) const;
	bool operator != ( HString const& ) const;
	bool operator != ( char const* const ) const;
	bool operator >= ( HString const& ) const;
	bool operator >= ( char const* const ) const;
	bool operator <= ( HString const& ) const;
	bool operator <= ( char const* const ) const;
	bool operator > ( HString const& ) const;
	bool operator > ( char const* const ) const;
	bool operator < ( HString const& ) const;
	bool operator < ( char const* const ) const;
	/* add_* functions in HList needs operator
		 bool() operator char * works fine here */
	operator char const* ( void ) const;
	char const* raw( void ) const;
	bool is_empty( void ) const;
	bool empty( void ) const;
	void clear( void );
	int long get_length( void ) const;
	static void swap( HString&, HString& );
	HString& format( char const* const, ... );
	HString& vformat( char const* const, void* );
	int long find( char const, int long = 0 ) const;
	int long find( char const* const, int long = 0 ) const;
	int long nfind( char const* const, int long, int long = 0 ) const;
	int long reverse_find( char const, int long = 0 ) const;
	int long find_one_of( char const* const, int long = 0 ) const;
	int long reverse_find_one_of( char const* const, int long = 0 ) const;
	int long find_other_than( char const* const, int long = 0 ) const;
	int long reverse_find_other_than( char const* const, int long = 0 ) const;
	HString& replace( char const* const, char const* const );
	HString& upper( void );
	HString& lower( void );
	HString& reverse( void );
	HString left( int const ) const;
	HString mid( int long, int long /* no const - reused */ = INT_MAX ) const;
	HString right( int long /* not const - reused */ ) const;
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

HString operator + ( char const*, HString const& );
HString operator + ( char const, HString const& );
HString operator + ( int const, HString const& );
HString operator + ( int long const, HString const& );
HString operator + ( double const, HString const& );
HString operator + ( double long const, HString const& );
HString operator + ( void const* const, HString const& );
bool operator == ( char const* const, HString const& );
bool operator != ( char const* const, HString const& );
bool operator >= ( char const* const, HString const& );
bool operator <= ( char const* const, HString const& );
bool operator > ( char const* const, HString const& );
bool operator < ( char const* const, HString const& );

extern char const n_pcWhiteSpace[];

}

}

#endif /* not __YAAL_HCORE_HSTRING_H */

