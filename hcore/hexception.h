/*
---           `stdhapi' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hexception.h - this file is integral part of `stdhapi' project.

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
	 macros and global variables used acros whole stdhapi */

#ifndef __HCORE_HEXCEPTION_H
#define __HCORE_HEXCEPTION_H

#include <errno.h>

#ifndef NULL
#define NULL 0
#endif /* not NULL */

#ifdef __STDHAPI_BUILD__
#	include "config.h"
#endif /* __STDHAPI_BUILD__ */

namespace stdhapi
{

namespace hcore
{

#define _(string) gettext (string)

#define M_CVSID(id) static char __CVSID__ [ ] __attribute__((__unused__)) = id
#define M_CVSTID(id) static char __CVSTID__ [ ] __attribute__((__unused__)) = id
#define __WHERE__ __FILE__, __PRETTY_FUNCTION__, __LINE__
#ifdef __EXCEPTIONS_BY_REFERENCE__
# define D_EXCEPTION_BY &
# define M_EXCEPTION_CREATE( e, msg, e_no ) HException e ( __WHERE__, msg, e_no )
# define M_EXCEPTION_RELEASE( e ) /**/
# define M_THROW( msg, e_no ) throw ( HException ( __WHERE__, msg, e_no ) )
#else /* __EXCEPTIONS_BY_REFERENCE__ */
# define D_EXCEPTION_BY *
# define M_EXCEPTION_CREATE( e, msg, e_no ) HException * e = new HException ( __WHERE__, msg, e_no )
# define M_EXCEPTION_RELEASE( e ) delete e;
# define M_THROW( msg, e_no ) throw ( new HException ( __WHERE__, msg, e_no ) )
#endif /* not __EXCEPTIONS_BY_REFERENCE__ */
#define M_TRY try{
#define M_CATCH( e ) }catch ( HException D_EXCEPTION_BY e )
#define M_PROLOG M_TRY
#define M_EPILOG M_CATCH ( e ){e->log ( __WHERE__ );throw;}
#define M_FINAL M_CATCH ( e ){e->log ( __WHERE__ );e->print_error ( true );M_EXCEPTION_RELEASE ( e );}
#define M_ENSURE( condition ) if ( ! ( condition ) )M_THROW ( strerror ( errno ), errno )
#define M_IRV( expression )	static_cast < void > ( expression )
#define M_ASSERT( condition ) if ( ! ( condition ) )HException::failed_assert ( __WHERE__, #condition )

#define D_LOG_DEBUG			1
#define D_LOG_INFO			2
#define D_LOG_NOTICE		4
#define D_LOG_WARNING		8
#define D_LOG_ERROR			16
#define D_LOG_CVSHEADER 32

/* those types definitions were in hinfo.h but this file (hexception.h)
 * is included into more files, we assume that sizeof ( int ) >= 4 */
#define D_TYPE_NONE					0x0000
#define D_TYPE_BOOL					0x0001
#define D_TYPE_CHAR					0x0002
#define D_TYPE_SHORT				0x0004
#define D_TYPE_INT					0x0008
#define	D_TYPE_LONG					0x0010
#define	D_TYPE_LONG_INT			0x0020
#define D_TYPE_DOUBLE				0x0040
#define D_TYPE_POINTER			0x0080
#define D_TYPE_CHAR_POINTER	0x0100
#define D_TYPE_HOBJECT			0x0200
#define D_TYPE_HSTRING			0x0400
#define D_TYPE_HINFO				0x0800
#define D_TYPE_HMAP					0x1000
#define D_TYPE_HLIST				0x2000
#define D_TYPE_HTIME				0x4000
#define D_TYPE_MASK					0xffff

#define g_iErrNo errno

extern long int n_lLogMask;
extern int n_iDebugLevel;

class HException
	{
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
	HException ( char const *, char const *, int, char const *, int = 0 );
	HException ( const HException & );
	virtual ~HException ( void ) ;
	void set ( char = 0, int = 0, long = 0, double = 0, char const * = 0, void * = 0 );
	void set ( char const * );
	void print_error ( bool ) const;
	static void dump_call_stack ( int );
	void log ( char const *, char const *, int );
	HException * operator-> ( void );
	static void failed_assert ( char const *, char const *, int, char const * ) __attribute__(( __noreturn__ ));
	/*}*/
private:
	/*{*/
	HException & operator = ( const HException & );
	/*}*/
	};

}

}

#endif /* not __HCORE_HEXCEPTION_H */

