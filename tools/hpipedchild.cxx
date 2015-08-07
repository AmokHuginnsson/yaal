/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

  hpipedchild.cxx - this file is integral part of `yaal' project.

  i.  You may not make any changes in Copyright information.
  ii. You must attach Copyright information to any part of every copy
      of this software.

Copyright:

 You can use this software free of charge and you can redistribute its binary
 package freely but:
  1. You are not allowed to use any part of sources of this software.
  2. You are not allowed to redistribute any part of sources of this software.
  3. You are not allowed to reverse engineer this software.
  4. If you want to distribute a binary package of this software you cannot
     demand any fees for it. You cannot even demand
     a return of cost of the media or distribution (CD for example).
  5. You cannot involve this software in any commercial activity (for example
     as a free add-on to paid software or newspaper).
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
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "hpipedchild.hxx"
#include "hfsitem.hxx"
#include "hcore/system.hxx"
#include "hcore/hfile.hxx"
#include "util.hxx"
#include "halarm.hxx"

using namespace yaal::hcore;

namespace yaal {

namespace tools {

static void close_and_invalidate( int& fd_ ) {
	M_PROLOG
	if ( fd_ >= 0 )
		M_ENSURE( M_TEMP_FAILURE_RETRY( hcore::system::close( fd_ ) ) == 0 );
	fd_ = -1;
	return;
	M_EPILOG
}

int HPipedChild::_killGracePeriod = 1000;

HPipedChild::HPipedChild( void )
	: HStreamInterface(), _pid( 0 ),
	_pipeIn( -1 ), _pipeOut( -1 ), _pipeErr( -1 ),
	_cSOI( STREAM::OUT ), _secondLineCache( _cache.size() ), _secondLineOffset( _offset ) {
	return;
}

HPipedChild::~HPipedChild( void ) {
	M_PROLOG
	if ( _pid > 0 )
		finish();
	return;
	M_DESTRUCTOR_EPILOG
}

namespace {
#pragma GCC diagnostic ignored "-Wold-style-cast"
template<typename T>
inline bool FWD_WIFEXITED( T val_ ) {
	return ( WIFEXITED( val_ ) ? true : false );
}
template<typename T>
inline int FWD_WEXITSTATUS( T val_ ) {
	return ( WEXITSTATUS( val_ ) );
}
template<typename T>
inline bool FWD_WIFSIGNALED( T val_ ) {
	return ( WIFSIGNALED( val_ ) ? true : false );
}
template<typename T>
inline int FWD_WTERMSIG( T val_ ) {
	return ( WTERMSIG( val_ ) );
}
#pragma GCC diagnostic error "-Wold-style-cast"
}

HPipedChild::STATUS HPipedChild::finish( void ) {
	M_PROLOG
	close_and_invalidate( _pipeErr );
	close_and_invalidate( _pipeOut );
	close_and_invalidate( _pipeIn );
	STATUS s;
	if ( _pid > 0 ) {
		int status( 0 );
		int pid( 0 );
		M_ENSURE( ( pid = ::waitpid( _pid, &status, WNOHANG | WUNTRACED | WCONTINUED ) ) != -1 );
		if ( pid != _pid ) {
			M_ENSURE( hcore::system::kill( _pid, SIGTERM ) == 0 ); {
				HAlarm alarm( _killGracePeriod );
				M_ENSURE( ( ( pid = ::waitpid( _pid, &status, 0 ) ) != -1 ) || ( errno == EINTR ) );
			}
			if ( pid != _pid ) {
				M_ENSURE( hcore::system::kill( _pid, SIGKILL ) == 0 );
				M_ENSURE( ::waitpid( _pid, &status, 0 ) == _pid );
			}
		}
		if ( FWD_WIFEXITED( status ) ) {
			s.type = STATUS::TYPE::NORMAL;
			s.value = FWD_WEXITSTATUS( status );
		} else if ( FWD_WIFSIGNALED( status ) ) {
			s.type = STATUS::TYPE::ABORT;
			s.value = FWD_WTERMSIG( status );
		}
	}
	_pid = 0;
	return ( s );
	M_EPILOG
}

struct OPipeResGuard {
	int _res[2];
	OPipeResGuard( void ) : _res() {
		_res[ 0 ] = _res[ 1 ] = -1;
	}
	~OPipeResGuard( void ) {
		if ( _res[ 0 ] >= 0 )
			M_ENSURE( M_TEMP_FAILURE_RETRY( hcore::system::close( _res[ 0 ] ) ) == 0 );
		if ( _res[ 1 ] >= 0 )
			M_ENSURE( M_TEMP_FAILURE_RETRY( hcore::system::close( _res[ 1 ] ) ) == 0 );
	}
};

void HPipedChild::spawn( HString const& image_, argv_t const& argv_ ) {
	M_PROLOG
	HScopedValueReplacement<int> saveErrno( errno, 0 );
	OPipeResGuard pipeIn, pipeOut, pipeErr;
	int* fileDesIn = pipeIn._res;
	int* fileDesOut = pipeOut._res;
	int* fileDesErr = pipeErr._res;
	HFSItem image( image_ );
	M_ENSURE_EX( !! image && image.is_executable(), image_ );
	M_ENSURE( ( ! ::pipe( fileDesIn ) ) && ( ! ::pipe( fileDesOut ) ) && ( ! ::pipe( fileDesErr ) ) );
	HChunk argv( chunk_size<char const*>( argv_.size() + 2 ) );
	HLock stdinLock( cin.acquire() );
	HLock sdtoutLock( cout.acquire() );
	HLock stderrLock( cerr.acquire() );
	int const stdinFd( fileno( stdin ) );
	int const stdoutFd( fileno( stdout ) );
	int const stderrFd( fileno( stderr ) );
	::fflush( stderr );
	cout << hcore::flush;
	::fflush( stdout );
	cerr << hcore::flush;
	_pid = ::fork();
	M_ENSURE_EX( _pid >= 0, "fork()" );
	if ( ! _pid ) {
		close_and_invalidate( fileDesIn[ 1 ] );
		close_and_invalidate( fileDesOut[ 0 ] );
		close_and_invalidate( fileDesErr[ 0 ] );
		if ( ( ::dup2( fileDesIn[ 0 ], stdinFd ) < 0 )
				|| ( ::dup2( fileDesOut[ 1 ], stdoutFd ) < 0 )
				|| ( ::dup2( fileDesErr[ 1 ], stderrFd ) < 0 ) )
			M_THROW( "dup2", errno );
		argv.get<char const*>()[ 0 ] = image_.raw();
		int i( 1 );
		for ( argv_t::const_iterator it( argv_.begin() ), end( argv_.end() ); it != end; ++ it, ++ i )
			argv.get<char const*>()[ i ] = it->raw();
		::execv( argv.get<char const*>()[ 0 ], const_cast<char* const*>( argv.get<char const*>() ) );
		M_ENSURE( !"execv"[0] );
	} else {
		close_and_invalidate( fileDesIn[ 0 ] );
		close_and_invalidate( fileDesOut[ 1 ] );
		close_and_invalidate( fileDesErr[ 1 ] );
		using yaal::swap;
		swap( _pipeIn, fileDesIn[ 1 ] );
		swap( _pipeOut, fileDesOut[ 0 ] );
		swap( _pipeErr, fileDesErr[ 0 ] );
	}
	return;
	M_EPILOG
}

int long HPipedChild::do_read( void* const buffer_, int long size_ ) {
	M_PROLOG
	M_ASSERT( ( _pipeOut >= 0 ) && ( _pipeErr >= 0 ) );
	int fd = ( ( _cSOI == STREAM::OUT ) ? _pipeOut : _pipeErr );
	return ( ::read( fd, buffer_, static_cast<size_t>( size_ ) ) );
	M_EPILOG
}

int long HPipedChild::do_write( void const* const string_, int long size_ ) {
	M_PROLOG
	M_ASSERT( _pipeIn >= 0 );
	int long nWritten( 0 );
	int long nWriteChunk( 0 );
	do {
		nWriteChunk = M_TEMP_FAILURE_RETRY( ::write( _pipeIn,
					static_cast<char const* const>( string_ ) + nWritten,
					static_cast<size_t>( size_ - nWritten ) ) );
		nWritten += nWriteChunk;
	} while ( ( nWriteChunk > 0 ) && ( nWritten < size_ ) );
	return ( nWritten );
	M_EPILOG
}

void HPipedChild::do_flush( void ) {
}

bool HPipedChild::read_poll( int long* time_ ) {
	M_PROLOG
	int fd( ( _cSOI == STREAM::OUT ) ? _pipeOut : _pipeErr );
	return ( hcore::system::wait_for_io( &fd, 1, NULL, 0, time_ ) <= 0 );
	M_EPILOG
}

bool HPipedChild::is_running( void ) const {
	int err( 0 );
	if ( _pid > 0 )
		err = hcore::system::kill( _pid, 0 );
	return ( ( _pid > 0 ) && ! err );
}

void HPipedChild::set_csoi( STREAM::stream_t const& cSOI_ ) {
	M_PROLOG
	M_ASSERT( ( cSOI_ == STREAM::OUT ) || ( cSOI_ == STREAM::ERR ) );
	if ( cSOI_ != _cSOI ) {
		using yaal::swap;
		swap( _offset, _secondLineOffset );
		swap( _cache, _secondLineCache );
	}
	_cSOI = cSOI_;
	return;
	M_EPILOG
}

bool HPipedChild::do_is_valid( void ) const {
	M_PROLOG
	return ( is_running() && ( _pipeIn >= 0 ) && ( _pipeOut >= 0 ) && ( _pipeErr >= 0 ) );
	M_EPILOG
}

}

}

