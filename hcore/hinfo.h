/*
---           `stdhapi' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hinfo.h - this file is integral part of `stdhapi' project.

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

#ifndef __HCORE_HINFO_H
#define __HCORE_HINFO_H

#include "hstring.h"
#include "htime.h"
#include "hexception.h" /* types definitions */

class HInfo
	{
protected:
	/*{*/
	int				f_iType;
	int				f_iInt;
	long int	f_lLongInt;
	double		f_dDouble;
	void *		f_pvPointer;
	HString		f_oString;
	HTime			f_oTime;
	/*}*/
public:
	/*{*/
	HInfo ( void );
	HInfo ( const int & );
	HInfo ( const long int & );
	HInfo ( const double & );
	HInfo ( const char * );
	HInfo ( const HTime & );
	HInfo ( void * );
	HInfo ( const HInfo &, int = 0 );
	virtual ~HInfo ( void );
	char & operator [ ] ( int );
	HInfo & operator = ( const HInfo & );
	int & operator = ( const int & );
	long int & operator = ( const long int & );
	double & operator = ( const double & );
	HString & operator = ( const char * );
	HTime & operator = ( const HTime & );
	void * operator = ( void * );
	operator int & ( void );
	operator long int & ( void );
	operator double & ( void );
	operator HString & ( void );
	operator HTime & ( void );
	operator void * ( void );
	operator bool ( void );
	/*}*/
protected:
	/*{*/
	void purge ( void );
	/*}*/
	};

#endif /* not __HCORE_HINFO_H */
