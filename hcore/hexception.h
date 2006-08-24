/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hexception.h - this file is integral part of `yaal' project.

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

/* This file holds (except HException class declaration) main #defines,
	 macros and global variables used acros whole yaal */

#ifndef __YAAL_HCORE_HEXCEPTION_H
#define __YAAL_HCORE_HEXCEPTION_H

#include <cerrno>
#include <cstring>

#ifdef __YAAL_BUILD__
#	include "config.h"
#endif /* __YAAL_BUILD__ */
#include "base.h"

namespace yaal
{

namespace hcore
{

extern int long n_lLogMask;
extern int n_iDebugLevel;

class HException
	{
private:
	/*{*/
	mutable bool f_bLocal;
	/*}*/
protected:
	/*{*/
	char f_cChar;
	int	 f_iInt;
	long f_lLong;
	double f_dDouble;
	char * f_pcCharPtr;
	void * f_pvVoidPtr;
	int  f_iFrame;
	char * f_pcFileName;
	char * f_pcFunctionName;
	int  f_iCode;
	char * f_pcMessage;
	/*}*/
public:
	/*{*/
	HException ( char const * const, char const * const, int const,
			char const * const, int const = 0 );
	HException ( HException const & );
	virtual ~HException ( void ) ;
	void set ( char const = 0, int const = 0, long const = 0, double const = 0,
			char const * const = 0, void * const = 0 );
	void set ( char const * );
	void print_error ( bool const ) const;
	static void dump_call_stack ( int const );
	void log ( char const * const, char const * const, int const );
	char const * what ( void ) const;
	HException * operator-> ( void );
	static void failed_assert ( char const * const, char const * const, int const,
			char const * const ) __attribute__(( __noreturn__ ));
	/*}*/
private:
	/*{*/
	HException & operator = ( HException const & );
	/*}*/
	};

}

}

#endif /* not __YAAL_HCORE_HEXCEPTION_H */

