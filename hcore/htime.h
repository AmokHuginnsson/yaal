/*
---           `stdhapi' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	htime.h - this file is integral part of `stdhapi' project.

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

#ifndef __HCORE_HTIME_H
#define __HCORE_HTIME_H

#include <time.h>

#include "hstring.h"

namespace stdhapi
{

namespace hcore
{

#define D_DEFAULT_TIME_FORMAT	"%a, %d %b %Y %T %z"

class HTime
	{
protected:
	/*{*/
	HString	f_oFormat;
	HString	f_oBuffer;
	time_t	f_xValue;
	tm			f_sBroken;
	/*}*/
public:
	/*{*/
	HTime ( void );
	HTime ( const HTime & );
	HTime ( char const * );
	HTime ( const time_t & );
	HTime ( const int, const int, const int, const int = 0, const int = 0,
			const int = 0 );
	virtual ~HTime ( void );
	void set_now ( void );
	void format ( char const * = D_DEFAULT_TIME_FORMAT );
	void set_time ( const int = 0, const int = 0, const int = 0 );
	void set_date ( const int, const int = 1, const int = 1 );
	void set_datetime ( const int, const int = 1, const int = 1,
						 const int = 0, const int = 0, const int = 0 );
	int get_year ( void );
	int get_month ( void );
	int get_day ( void );
	int get_hour ( void );
	int get_minute ( void );
	int get_second ( void );
	HTime & operator = ( const HTime & );
	HTime operator - ( const HTime & ) const;
	HTime & operator -= ( const HTime & );
	bool operator == ( const time_t & );
	bool operator != ( const time_t & );
	bool operator <= ( const time_t & );
	bool operator >= ( const time_t & );
	bool operator < ( const time_t & );
	bool operator > ( const time_t & );
	operator char const * ( void );
	operator time_t ( void );
	/*}*/
protected:
	/*{*/
	/*}*/
	};

}

}

#endif /* not __HCORE_HTIME_H */
