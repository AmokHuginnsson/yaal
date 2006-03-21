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

#include <ctime>

#include "hstring.h"

namespace stdhapi
{

namespace hcore
{

extern char const * const n_pcDefaultTimeFormat;

class HTime
	{
protected:
	/*{*/
	HString	f_oFormat;
	mutable HString	f_oVarTmpBuffer;
	time_t	f_xValue;
	tm			f_sBroken;
	/*}*/
public:
	/*{*/
	HTime ( void );
	HTime ( HTime const & );
	HTime ( char const * const );
	HTime ( time_t const & );
	HTime ( int const, int const, int const, int const = 0, int const = 0,
			int const = 0 );
	virtual ~HTime ( void );
	void set_now ( void );
	void set ( time_t const & );
	void format ( char const * const = n_pcDefaultTimeFormat );
	void set_time ( int const = 0, int const = 0, int const = 0 );
	void set_date ( int const, int const = 1, int const = 1 );
	void set_datetime ( int const, int const = 1, int const = 1,
						 int const = 0, int const = 0, int const = 0 );
	int get_year ( void ) const;
	int get_month ( void ) const;
	int get_day ( void ) const;
	int get_hour ( void ) const;
	int get_minute ( void ) const;
	int get_second ( void ) const;
	HTime & operator = ( HTime const & );
	HTime operator - ( HTime const & ) const;
	HTime & operator -= ( HTime const & );
	bool operator == ( time_t const & ) const;
	bool operator != ( time_t const & ) const;
	bool operator <= ( time_t const & ) const;
	bool operator >= ( time_t const & ) const;
	bool operator < ( time_t const & ) const;
	bool operator > ( time_t const & ) const;
	operator char const * ( void ) const;
	operator time_t ( void ) const;
	/*}*/
protected:
	/*{*/
	/*}*/
	};

}

}

#endif /* not __HCORE_HTIME_H */
