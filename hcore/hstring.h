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

#ifndef __HSTRING_H
#define __HSTRING_H

#include <stddef.h>

#include "hobject.h"

class HString : public HObject
	{
protected:
	/*{*/
	char *	f_pcBuffer;
	size_t	f_iSize;
	/*}*/
public:
	/*{*/
	HString ( void );
	HString ( const HString & );
	HString ( const size_t ); /* initialize immediately with size */
	virtual ~HString ( void ) ;
	void hs_realloc ( const size_t );
	virtual HObject * clone ( void ) const;
	HString ( const char * );
	HString ( char );
	HString ( int );
	HString ( long int );
	HString ( double );
	HString ( const void * );
	HString & operator = ( const HString & );
	HString & operator = ( const char * );
	HString & operator = ( const char );
	HString & operator = ( const int );
	HString & operator = ( const long int );
	HString & operator = ( const double );
	HString & operator = ( const void * );
	HString & operator += ( const HString & );
	HString & operator += ( const char * );
	HString & operator += ( const char );
	HString & operator += ( const int );
	HString & operator += ( const long int );
	HString & operator += ( const double );
	HString & operator += ( const void * );
	HString operator + ( const HString & );
	HString operator + ( const char * );
	HString operator + ( const char );
	HString operator + ( const int );
	HString operator + ( const long int );
	HString operator + ( const double );
	HString operator + ( const void * );
	char & operator [ ] ( const int );
	bool operator == ( const HString & ) const;
	bool operator == ( const char * ) const;
	bool operator != ( const HString & ) const;
	bool operator != ( const char * ) const;
	bool operator >= ( const HString & ) const;
	bool operator >= ( const char * ) const;
	bool operator <= ( const HString & ) const;
	bool operator <= ( const char * ) const;
	bool operator > ( const HString & ) const;
	bool operator > ( const char * ) const;
	bool operator < ( const HString & ) const;
	bool operator < ( const char * ) const;
	operator char * ( void ) const; /* add_* functions in HList needs operator
																		 bool ( ) operator char * works fine here */
	bool is_empty ( void );
	int get_length ( void ) const;
	HString & format ( const char *, ... );
	int find ( char, int = 0 );
	int find ( const char *, int = 0 );
	int reverse_find ( char, int = 0 );
	HString & replace ( const char *, const char * );
	HString & upper ( void );
	HString & lower ( void );
	HString & reverse ( void );
	HString left ( int );
	HString mid ( int, int = 0 );
	HString right  ( int );
	HString & trim_left ( const char * = " \t\r\n" );
	HString & trim_right ( const char * = " \t\r\n" );
	HString split ( const char *, int );
	/*}*/
protected:
	/*{*/
	/*}*/
	};

HString operator + ( const char *, const HString & );
HString operator + ( const char, const HString & );
HString operator + ( const int, const HString & );
HString operator + ( const long int, const HString & );
HString operator + ( const double, const HString & );
HString operator + ( const void *, const HString & );
bool operator == ( const char *, const HString & );
bool operator != ( const char *, const HString & );
bool operator >= ( const char *, const HString & );
bool operator <= ( const char *, const HString & );
bool operator > ( const char *, const HString & );
bool operator < ( const char *, const HString & );

#endif /* not __HSTRING_H */
