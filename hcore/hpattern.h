/*
---           `stdhapi' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hpattern.h - this file is integral part of `stdhapi' project.

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

#ifndef __HCORE_HPATTERN_H
#define __HCORE_HPATTERN_H

#include <sys/types.h>	/* why? - because POSIX says so :/ */
#include <regex.h>			/* this one is obvious */

#include "harray.h"
#include "hstring.h"

class HPattern
	{
protected:
	/*{*/
	bool		f_bInitialized;				/* is pattern initialized */
	bool		f_bIgnoreCaseDefault;	/* default value for ignore case switch */
	bool		f_bIgnoreCase;				/* self explanary */
	bool		f_bExtended;					/* simple strstr or extended RE */
	int			f_iSimpleMatchLength;	/* length of simple strstr pattern */
	regex_t	f_sCompiled;					/* compiled regular expression for
																	 search patterns */
	HString	f_oPatternInput;			/* current search pattern */
	HString	f_oPatternReal;				/* pattern with stripped switches */
	HString f_oError;							/* error message of last operation */
	/*}*/
public:
	/*{*/
	HPattern ( bool = false /* default ignore case state */ );
	virtual ~HPattern ( void );
	bool parse ( char const * /* pattern input */,
							 unsigned short int * = NULL /* additional flags to parse */,
							 int = 0 /* number of additional flags */ );
	char const * error ( void );
	char * matches ( char const *, int & );
	int count ( char const * );
	/*}*/
protected:
	/*{*/
	bool set_switch ( char, unsigned short int *, int );
	/*}*/
	};

#endif /* not __HCORE_HPATTERN_H */

