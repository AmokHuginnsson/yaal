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

#ifndef __HLOG_H
#define __HLOG_H

#define D_BUFFER_SIZE			1024
#define D_HOSTNAME_SIZE		16
#define D_TIMESTAMP_SIZE	16

#define M_LOG(MESG) ::log ( "Log: %16s : %4d : %s : %s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__, MESG )

#include <stdio.h>

class HLog
	{
protected:
	/*{*/
	bool		f_bRealMode;
	bool		f_bNewLine;
	FILE *	f_psStream;
	char *	f_pcProcessName;
	char *	f_pcLoginName;
	char *	f_pcHostName;
	char *	f_pcBuffer;
	size_t	f_iBufferSize;
	/*}*/
public:
	/*{*/
	HLog ( void );
	virtual ~HLog ( void );
	void rehash ( FILE * = stderr, char * = NULL ); /* already opened file */
	void rehash ( const char *, char * = NULL ); /* log file name */
	int operator ( ) ( const char *, ... ); /* log ( "data %d", x ); will look nice */
	HLog & operator << ( const char * );
	HLog & operator << ( const char );
	HLog & operator << ( const int );
	HLog & operator << ( const long int );
	HLog & operator << ( const double );
	HLog & operator << ( void * );
	HLog & operator << ( HLog & ( * ) ( HLog & ) );
	/*}*/
protected:
	/*{*/
	void timestamp ( FILE * = NULL );
	/*}*/
	friend HLog & endl ( HLog & );
	};

extern HLog log;

#endif /* __HLOG_H */
