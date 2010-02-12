/*
---          `yaal' (c) 1978 by Marcin 'Amok' Konarski           ---

	hlog.cxx - this file is integral part of `yaal' project.

	i.  You may not make any changes in Copyright information.
	ii. You must attach Copyright information to any part of every copy
	    of this software.

Copyright:

 You are free to use this program as is, you can redistribute binary
 package freely but:
  1. You cannot use any part of sources of this software.
  2. You cannot redistribute any part of sources of this software.
  3. No reverse engineering is allowed.
  4. If you want redistribute binary package you cannot demand any fees
     for this software.
     You cannot even demand cost of the carrier (CD for example).
  5. You cannot include it to any commercial enterprise (for example 
     as a free add-on to payed software or payed newspaper).
 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE. Use it at your own risk.
*/

#include <cstdlib>
#include <cstring>
#include <cstdarg>
#include <ctime>
#include <unistd.h>
#include <pwd.h>
#include <libintl.h>

#include "config.hxx"

#ifdef HAVE_BASENAME_IN_LIBGEN
#	include <libgen.h>
#endif /* HAVE_BASENAME_IN_LIBGEN */

#include "base.hxx"
M_VCSID( "$Id: "__ID__" $" )
M_VCSID( "$Id: "__TID__" $" )
#include "hlog.hxx"
#include "xalloc.hxx"

namespace yaal
{

namespace hcore
{

namespace
	{
	static int const BUFFER_SIZE		= 1024;
	static int const HOSTNAME_SIZE	= 128;
	static int const TIMESTAMP_SIZE	= 16;
	}

int long HLog::f_lLogMask = 0;

HLog::HLog ( void ) : HStreamInterface(), f_bRealMode ( false ), f_bNewLine ( true ),
	f_lType ( 0 ), f_iBufferSize ( BUFFER_SIZE ),
	f_psStream ( NULL ), f_pcProcessName( NULL ),
	f_oLoginName ( NULL ), f_oHostName( xcalloc<char>( HOSTNAME_SIZE ) ),
	f_oBuffer ( xcalloc<char>( f_iBufferSize ) )
	{
	M_PROLOG
	uid_t l_iUid = 0;
	f_psStream = tmpfile();
	if ( ! f_psStream )
		M_THROW( "tmpfile returned", reinterpret_cast<int long>( f_psStream ) );
	::fprintf( f_psStream, "%-10xProcess started (%ld).\n",
			LOG_TYPE::NOTICE, static_cast<int long>( getpid() ) );
	l_iUid = getuid();
	passwd l_sPasswd;
	long int bsize = ::sysconf( _SC_GETPW_R_SIZE_MAX );
	HChunk buf( xcalloc<char>( bsize + 1 ) );
	passwd* any;
	if ( ! getpwuid_r( l_iUid, &l_sPasswd, buf.get<char>(), static_cast<int>( bsize ), &any ) )
		f_oLoginName.set( xstrdup( l_sPasswd.pw_name ) );
	else
		{
		f_oLoginName.set( xcalloc<char>( LOGIN_NAME_MAX + 1 ) );
		M_ENSURE( ::snprintf( f_oLoginName.get<char>(), LOGIN_NAME_MAX, "%ld",
					static_cast<int long>( l_iUid ) ) <= LOGIN_NAME_MAX );
		}
	M_ENSURE( ::gethostname( f_oHostName.get<char>(), HOSTNAME_SIZE - 1 ) == 0 );
	return;
	M_EPILOG
	}

HLog::~HLog( void )
	{
	M_PROLOG
	if ( f_lLogMask & LOG_TYPE::NOTICE )
		{
		if ( f_bNewLine )
			timestamp();
		fprintf ( f_psStream, "Process exited normally.\n" );
		}
	if ( ( f_psStream != stdout ) && ( f_psStream != stderr ) )
		fclose( f_psStream );
	f_psStream = NULL;
	return;
	M_EPILOG
	}

void HLog::rehash( FILE* a_psStream,
		char const* const a_pcProcessName )
	{
	M_PROLOG
#ifndef HAVE_GETLINE
	char* l_pcPtr = NULL;
	int l_iLen = 0;
#endif /* not HAVE_GETLINE */
	FILE* l_psTmpFile;
	f_bRealMode = true;
	if ( a_pcProcessName )
		f_pcProcessName = ::basename( a_pcProcessName );
	if ( ! a_psStream )
		M_THROW( "file parameter is", reinterpret_cast<int long>( a_psStream ) );
	l_psTmpFile = f_psStream;
	f_psStream = a_psStream;
	if ( l_psTmpFile )
		{
		::fseek( l_psTmpFile, 0, SEEK_SET );
		char* buf = f_oBuffer.get<char>();
#ifdef HAVE_GETLINE 
		while ( ::getline( &buf, &f_iBufferSize, l_psTmpFile ) > 0 )
#else /* HAVE_GETLINE */
		while ( ( l_iLen = static_cast<int>( ::fread( buf, sizeof ( char ), f_iBufferSize, l_psTmpFile ) ) ) )
#endif /* not HAVE_GETLINE */
			{
#ifndef HAVE_GETLINE
			l_pcPtr = static_cast<char*>( ::memchr( buf, '\n', l_iLen ) );
			if ( ! l_pcPtr )
				{
				::fprintf( f_psStream, buf );
				continue;
				}
			* ++ l_pcPtr = 0;
			::fseek( l_psTmpFile, l_pcPtr - buf - l_iLen, SEEK_CUR );
#endif /* not HAVE_GETLINE */
			f_lType = ::strtol( buf, NULL, 0x10 );
			if ( ! ( f_lType && f_bRealMode ) || ( f_lType & f_lLogMask ) )
				{
				timestamp();
				::fputs( buf + 10, f_psStream );
				}
			}
		if ( buf[ ::strlen( buf ) - 1 ] == '\n' )
			f_lType = 0;
		::fclose( l_psTmpFile );
		}
	return;
	M_EPILOG
	}

void HLog::rehash( HString const& a_oLogFileName,
		char const* const a_pcProcessName )
	{
	M_PROLOG
	if ( a_oLogFileName.is_empty() )
		M_THROW( "new file name argument is", a_oLogFileName.get_length() );
	rehash( ::fopen( a_oLogFileName.raw(), "a" ), a_pcProcessName );
	do_flush();
	return;
	M_EPILOG
	}

void HLog::timestamp( void )
	{
	M_PROLOG
	if ( ! f_bRealMode )
		{
		if ( f_psStream )
			::fprintf( f_psStream, "%-10lx", f_lType );
		return;
		}
	time_t l_xCurrentTime = ::time( NULL );
	tm* l_psBrokenTime = ::localtime( &l_xCurrentTime );
	char l_pcBuffer[ TIMESTAMP_SIZE ];
	::memset( l_pcBuffer, 0, TIMESTAMP_SIZE );
	/* ISO C++ does not support the `%e' strftime format */
	/* `%e': The day of the month like with `%d', but padded with blank */
	/* (range ` 1' through `31'). */
	/* This format was first standardized by POSIX.2-1992 and by ISO C99.*/
	/* I will have to wait with using `%e'. */
	int long l_iSize = ::strftime( l_pcBuffer, TIMESTAMP_SIZE, "%b %d %H:%M:%S",
			l_psBrokenTime );
	if ( l_iSize > TIMESTAMP_SIZE )
		M_THROW( _( "strftime returned more than TIMESTAMP_SIZE" ), l_iSize );
	if ( f_pcProcessName )
		::fprintf( f_psStream, "%s %s@%s->%s: ", l_pcBuffer, f_oLoginName.get<char>(),
				f_oHostName.get<char>(), f_pcProcessName );
	else
		::fprintf( f_psStream, "%s %s@%s: ", l_pcBuffer, f_oLoginName.get<char>(),
			f_oHostName.get<char>() );
	return;
	M_EPILOG
	}

int HLog::operator() ( char const* const a_pcFormat, va_list a_xAp )
	{
	M_PROLOG
	int l_iErr = 0;
	if ( f_bNewLine )
		timestamp();
	char* buf = f_oBuffer.get<char>();
	::memset( buf, 0, f_iBufferSize );
	l_iErr = ::vsnprintf( buf, f_iBufferSize, a_pcFormat, a_xAp );
	::fputs( buf, f_psStream );
	if ( buf[ ::strlen( buf ) - 1 ] != '\n' )
		f_bNewLine = false;
	else
		{
		f_lType = 0;
		f_bNewLine = true;
		M_ENSURE( ::fflush( f_psStream ) == 0 );
		}
	return ( l_iErr );
	M_EPILOG
	}

int HLog::operator() ( char const * const a_pcFormat, ... )
	{
	M_PROLOG
	int l_iErr = 0;
	va_list l_xAp;
	if ( ! ( f_lType && f_bRealMode ) || ( f_lType & f_lLogMask ) )
		{
		va_start( l_xAp, a_pcFormat );
		l_iErr = ( *this ) ( a_pcFormat, l_xAp );
		va_end( l_xAp );
		}
	return ( l_iErr );
	M_EPILOG
	}

int HLog::operator() ( int long const a_lType,
		char const* const a_pcFormat, ... )
	{
	M_PROLOG
	int l_iErr = 0;
	va_list l_xAp;
	f_lType = a_lType;
	if ( ! ( f_lType && f_bRealMode ) || ( f_lType & f_lLogMask ) )
		{
		va_start( l_xAp, a_pcFormat );
		l_iErr = ( *this ) ( a_pcFormat, l_xAp );
		va_end( l_xAp );
		}
	return ( l_iErr );
	M_EPILOG
	}

HLog & HLog::operator() ( int long const a_lType )
	{
	M_PROLOG
	f_lType = a_lType;
	return ( * this );
	M_EPILOG
	}

int long HLog::do_write( void const* const a_pcString, int long const& a_lSize )
	{
	M_PROLOG
	if ( ! a_pcString )
		return ( 0 );
	int len = 0;
	char const* const str = static_cast<char const* const>( a_pcString );
	if ( ! ( f_lType && f_bRealMode ) || ( f_lType & f_lLogMask ) )
		{
		if ( f_bNewLine )
			timestamp();
		len = static_cast<int>( ::std::fwrite( str, sizeof ( char ), a_lSize, f_psStream ) );
		if ( str[ a_lSize - 1 ] != '\n' )
			f_bNewLine = false;
		else
			{
			f_bNewLine = true;
			f_lType = 0;
			M_ENSURE( ::std::fflush( f_psStream ) == 0 );
			}
		}
	return ( len );
	M_EPILOG
	}

void HLog::do_flush( void ) const
	{
	M_PROLOG
	M_ENSURE( ::std::fflush( f_psStream ) == 0 );
	return;
	M_EPILOG
	}

int long HLog::do_read( void* const, int long const& )
	{
	return ( 0 );
	}

bool HLog::do_is_valid( void ) const
	{
	M_PROLOG
	return ( f_psStream != NULL );
	M_EPILOG
	}

}

}

