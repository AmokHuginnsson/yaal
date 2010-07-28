/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hpipedchild.cxx - this file is integral part of `yaal' project.

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

#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <csignal> /* for FreeBSD */
#include <sys/wait.h>
#include <unistd.h>

#include "hcore/base.hxx"
M_VCSID( "$Id: "__ID__" $" )
M_VCSID( "$Id: "__TID__" $" )
#include "hpipedchild.hxx"
#include "hfsitem.hxx"
#include "hcore/xalloc.hxx"
#include "hcore/system.hxx"

using namespace yaal::hcore;

namespace yaal
{

namespace tools
{

HPipedChild::HPipedChild( void )
	: HStreamInterface(), _pid( 0 ),
	_pipeIn( -1 ), _pipeOut( -1 ), _pipeErr( -1 ),
	_cSOI( STREAM::OUT ), _secondLineCache( _cache.size() ), _secondLineOffset( _offset )
	{
	return;
	}

HPipedChild::~HPipedChild( void )
	{
	M_PROLOG
	if ( _pid > 0 )
		finish();
	return;
	M_EPILOG
	}

HPipedChild::STATUS HPipedChild::finish( void )
	{
	M_PROLOG
	if ( _pipeErr >= 0 )
		TEMP_FAILURE_RETRY( hcore::system::close( _pipeErr ) );
	_pipeErr = -1;
	if ( _pipeOut >= 0 )
		TEMP_FAILURE_RETRY( hcore::system::close( _pipeOut ) );
	_pipeOut = -1;
	if ( _pipeIn >= 0 )
		TEMP_FAILURE_RETRY( hcore::system::close( _pipeIn ) );
	_pipeIn = -1;
	STATUS s;
	if ( _pid > 0 )
		{
		hcore::system::kill( _pid, SIGKILL );
		int status = 0;
		::waitpid( _pid, &status, 0 );
		if ( WIFEXITED( status ) )
			{
			s.type = STATUS::TYPE::NORMAL;
			s.value = WEXITSTATUS( status );
			}
		else if ( WIFSIGNALED( status ) )
			{
			s.type = STATUS::TYPE::ABORT;
			s.value = WTERMSIG( status );
			}
		}
	_pid = 0;
	return ( s );
	M_EPILOG
	}

struct OPipeResGuard
	{
	int _res[2];
	OPipeResGuard( void ) : _res()
		{
		_res[ 0 ] = _res[ 1 ] = -1;
		}
	~OPipeResGuard( void )
		{
		if ( _res[ 0 ] >= 0 )
			TEMP_FAILURE_RETRY( hcore::system::close( _res[ 0 ] ) );
		if ( _res[ 1 ] >= 0 )
			TEMP_FAILURE_RETRY( hcore::system::close( _res[ 1 ] ) );
		}
	};

static void close_and_invalidate( int& fd_ )
	{
	M_PROLOG
	if ( fd_ >= 0 )
		M_ENSURE( TEMP_FAILURE_RETRY( hcore::system::close( fd_ ) ) == 0 );
	fd_ = -1;
	return;
	M_EPILOG
	}

void HPipedChild::spawn( HString const& image_, argv_t const& argv_ )
	{
	M_PROLOG
	OPipeResGuard pipeIn, pipeOut, pipeErr;
	int* fileDesIn = pipeIn._res;
	int* fileDesOut = pipeOut._res;
	int* fileDesErr = pipeErr._res;
	HFSItem image( image_ );
	M_ENSURE( !! image && image.is_executable() );
	M_ENSURE( ( ! ::pipe( fileDesIn ) ) && ( ! ::pipe( fileDesOut ) ) && ( ! ::pipe( fileDesErr ) ) );
	_pid = ::fork();
	if ( _pid < 0 )
		M_THROW( "fork", errno );
	if ( ! _pid )
		{
		close_and_invalidate( fileDesIn[ 1 ] );
		close_and_invalidate( fileDesOut[ 0 ] );
		close_and_invalidate( fileDesErr[ 0 ] );
		if ( ( ::dup2( fileDesIn[ 0 ], fileno( stdin ) ) < 0 )
				|| ( ::dup2( fileDesOut[ 1 ], fileno( stdout ) ) < 0 )
				|| ( ::dup2( fileDesErr[ 1 ], fileno( stderr ) ) < 0 ) )
			M_THROW( "dup2", errno );
		char** argv = xcalloc<char*>( argv_.size() + 2 );
		argv[ 0 ] = xstrdup( image_.raw() );
		int i = 1;
		for ( argv_t::const_iterator it = argv_.begin(); it != argv_.end(); ++ it, ++ i )
			argv[ i ] = xstrdup( it->raw() );
		::execv( image_.raw(), argv );
		M_ENSURE( !"execv" );
		}
	else
		{
		close_and_invalidate( fileDesIn[ 0 ] );
		close_and_invalidate( fileDesOut[ 1 ] );
		close_and_invalidate( fileDesErr[ 1 ] );
		swap( _pipeIn, fileDesIn[ 1 ] );
		swap( _pipeOut, fileDesOut[ 0 ] );
		swap( _pipeErr, fileDesErr[ 0 ] );
		}
	return;
	M_EPILOG
	}

int long HPipedChild::do_read( void* const buffer_, int long const& size_ )
	{
	M_PROLOG
	M_ASSERT( ( _pipeOut >= 0 ) && ( _pipeErr >= 0 ) );
	int fd = ( ( _cSOI == STREAM::OUT ) ? _pipeOut : _pipeErr );
	return ( ::read( fd, buffer_, size_ ) );
	M_EPILOG
	}

int long HPipedChild::do_write( void const* const string_, int long const& size_ )
	{
	M_PROLOG
	M_ASSERT( _pipeIn >= 0 );
	int long nWritten( 0 );
	int long nWriteChunk( 0 );
	do
		{
		nWriteChunk = TEMP_FAILURE_RETRY( ::write( _pipeIn,
					static_cast<char const* const>( string_ ) + nWritten,
					size_ - nWritten ) );
		nWritten += nWriteChunk;
		}
	while ( ( nWriteChunk > 0 ) && ( nWritten < size_ ) );
	return ( nWritten );
	M_EPILOG
	}

void HPipedChild::do_flush( void ) const
	{
	}

bool HPipedChild::read_poll( int long* time_ )
	{
	M_PROLOG
	int fd( ( _cSOI == STREAM::OUT ) ? _pipeOut : _pipeErr );
	return ( system::wait_for_io( &fd, 1, NULL, 0, time_ ) <= 0 );
	M_EPILOG
	}

bool HPipedChild::is_running( void ) const
	{
	int err( 0 );
	if ( _pid > 0 )
		err = hcore::system::kill( _pid, 0 );
	return ( ( _pid > 0 ) && ! err );
	}

void HPipedChild::set_csoi( STREAM::stream_t const& cSOI_ )
	{
	M_PROLOG
	M_ASSERT( ( cSOI_ == STREAM::OUT ) || ( cSOI_ == STREAM::ERR ) );
	if ( cSOI_ != _cSOI )
		{
		using yaal::swap;
		swap( _offset, _secondLineOffset );
		swap( _cache, _secondLineCache );
		}
	_cSOI = cSOI_;
	return;
	M_EPILOG
	}

bool HPipedChild::do_is_valid( void ) const
	{
	M_PROLOG
	return ( is_running() && ( _pipeIn >= 0 ) && ( _pipeOut >= 0 ) && ( _pipeErr >= 0 ) );
	M_EPILOG
	}

}

}

