/*
---           `stdhapi' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hstring.h - this file is integral part of `stdhapi' project.

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

#ifndef __HCORE_HSTRING_H
#define __HCORE_HSTRING_H

namespace stdhapi
{

namespace hcore
{

class HString
	{
protected:
	/*{*/
	char *	f_pcBuffer;
	int	f_iSize;
	/*}*/
public:
	/*{*/
	HString ( void );
	HString ( HString const & );
	HString ( int const, bool const ); /* initialize immediately with size */
	virtual ~HString ( void ) ;
	void hs_realloc ( int const );
	HString ( char const * const );
	HString ( char const );
	HString ( int const );
	HString ( int long const );
	HString ( double const );
	HString ( void const * const );
	HString & operator = ( HString const & );
	HString & operator += ( HString const & );
	HString & operator <<= ( int const );
	HString operator + ( HString const & ) const;
	HString operator + ( char const * ) const;
	HString operator + ( char const ) const;
	HString operator + ( int const ) const;
	HString operator + ( int long const ) const;
	HString operator + ( double const ) const;
	HString operator + ( void const * const ) const;
	char & operator [ ] ( int const );
	bool operator == ( HString const & ) const;
	bool operator == ( char const * const ) const;
	bool operator != ( HString const & ) const;
	bool operator != ( char const * const ) const;
	bool operator >= ( HString const & ) const;
	bool operator >= ( char const * const ) const;
	bool operator <= ( HString const & ) const;
	bool operator <= ( char const * const ) const;
	bool operator > ( HString const & ) const;
	bool operator > ( char const * const ) const;
	bool operator < ( HString const & ) const;
	bool operator < ( char const * const ) const;
	/* add_* functions in HList needs operator
		 bool ( ) operator char * works fine here */
	operator char const * const ( void ) const;
	char * raw ( void ) const;
	bool is_empty ( void ) const;
	int get_length ( void ) const;
	HString & format ( char const * const, ... );
	int find ( char const, int const = 0 ) const;
	int find ( char const * const, int const = 0 ) const;
	int reverse_find ( char const, int const = 0 ) const;
	int find_one_of ( char const * const, int const = 0 ) const;
	int find_other_than ( char const * const, int const = 0 ) const;
	HString & replace ( char const * const, char const * const );
	HString & upper ( void );
	HString & lower ( void );
	HString & reverse ( void );
	HString left ( int const ) const;
	HString mid ( int const, int /* no const - reused */ = 0 ) const;
	HString right  ( int /* not const - reused */ ) const;
	HString & trim_left ( char const * const = " \t\r\n" );
	HString & trim_right ( char const * const = " \t\r\n" );
	HString split ( char const * const, int const ) const;
	HString & fill ( char = '\0', int = 0, int = 0 );
	HString & fillz ( char = '\0', int = 0, int = 0 );
	/*}*/
protected:
	/*{*/
	/*}*/
	};

HString operator + ( char const *, HString const & );
HString operator + ( char const, HString const & );
HString operator + ( int const, HString const & );
HString operator + ( int long const, HString const & );
HString operator + ( double const, HString const & );
HString operator + ( void const * const, HString const & );
bool operator == ( char const * const, HString const & );
bool operator != ( char const * const, HString const & );
bool operator >= ( char const * const, HString const & );
bool operator <= ( char const * const, HString const & );
bool operator > ( char const * const, HString const & );
bool operator < ( char const * const, HString const & );

/* Useful helpers */
char * strrnpbrk ( char const * const, char const * const,
		int /* no const - used */ );
char const * strrpbrk ( char const * const, char const * const );
int strrnspn ( char const * const, char const * const, int const );
int strrspn ( char const * const, char const * const );

extern char const n_pcWhiteSpace [ ];

}

}

#endif /* not __HCORE_HSTRING_H */
