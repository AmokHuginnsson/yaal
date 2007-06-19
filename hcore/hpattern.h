/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	hpattern.h - this file is integral part of `yaal' project.

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

#ifndef __YAAL_HCORE_HPATTERN_H
#define __YAAL_HCORE_HPATTERN_H

#include <sys/types.h>	/* why? - because POSIX says so :/ */
#include <regex.h>			/* this one is obvious */

#include "hcore/harray.h"
#include "hcore/hstring.h"

namespace yaal
{

namespace hcore
{

class HPattern
	{
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
public:
	HPattern ( bool = false /* default ignore case state */ );
	virtual ~HPattern ( void );
	int parse ( char const * const /* pattern input */,
							 int short unsigned * const = NULL /*additional flags to parse*/,
							 int const = 0 /* number of additional flags */ );
	int parse_re ( char const * const );
	char const * error ( void ) const;
	char const * matches ( char const * const,
			int * const = NULL /* match length */,
			int * const = NULL /* error code */ );
	int count ( char const * const );
private:
	void prepare_error_message ( int const, char const * const );
	bool set_switch ( char const, int short unsigned * const, int const );
	};

}

}

#endif /* not __YAAL_HCORE_HPATTERN_H */

