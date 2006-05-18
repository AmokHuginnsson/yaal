/*
---          libsource 0.0.0 (c) 1978 by Marcin 'Amok' Konarski           ---

	hlog.h - this file is integral part of `stdhapi' project.

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

#ifndef __HCORE_HLOG_H
#define __HCORE_HLOG_H

#include <cstdio>

namespace stdhapi
{

namespace hcore
{

#define log_trace ( stdhapi::hcore::log << "Log: " << __FILE__ << " : " << " : " << __LINE__ << " : " << __PRETTY_FUNCTION__ << " : " )
	
namespace LOG_TYPE
	{
	static int const D_DEBUG			= 1; 
	static int const D_INFO				= 2; 
	static int const D_NOTICE			= 4; 
	static int const D_WARNING		= 8;
	static int const D_ERROR			= 16; 
	static int const D_CVSHEADER	= 32;
	}

class HLog;
HLog & endl ( HLog & );

class HLog
	{
protected:
	/*{*/
	bool			f_bRealMode;
	bool			f_bNewLine;
	int long	f_lType;
	FILE *		f_psStream;
	char *		f_pcProcessName;
	char *		f_pcLoginName;
	char *		f_pcHostName;
	char *		f_pcBuffer;
	size_t		f_iBufferSize;
	/*}*/
public:
	/*{*/
	HLog ( void );
	virtual ~HLog ( void );
	/* already opened file */
	void rehash ( FILE * = stderr, char const * const = NULL );
	/* log file name */
	void rehash ( char const * const, char const * const = NULL );
	int operator ( ) ( char const * const, va_list const );
	int operator ( ) ( char const * const, ... ); /* log ( "data %d", x );
																									 will look nice */
	/* log ( "data %d", x ); will look nice */
	int operator ( ) ( int long const, char const *, ... );
	HLog & operator ( ) ( int long const ); /* sets log type */
	HLog & operator << ( char const * const );
	HLog & operator << ( char const );
	HLog & operator << ( int const );
	HLog & operator << ( int unsigned const );
	HLog & operator << ( int long const );
	HLog & operator << ( int long unsigned const );
	HLog & operator << ( double const );
	HLog & operator << ( void * const );
	HLog & operator << ( HLog & ( * const ) ( HLog & ) );
	/*}*/
protected:
	/*{*/
	void timestamp ( FILE * = NULL );
	/*}*/
private:
	/*{*/
	HLog ( HLog const & );
	HLog & operator = ( HLog const & );
	/*}*/
	friend HLog & endl ( HLog & );
	};

extern HLog log;

}

}

#endif /* not __HCORE_HLOG_H */
