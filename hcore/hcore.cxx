#include <unistd.h>
#include "config.hxx"
char const COPYRIGHT [ ] =
"/*\n"
"---           `%s' %s (c) 1978 by Marcin 'Amok' Konarski            ---\n"
"\n"
/* hcore.cxx - this file is integral part of `yaal' project.
 */
"	i.  You may not make any changes in Copyright information.\n"
"	ii. You must attach Copyright information to any part of every copy\n"
"	    of this software.\n"
"\n"
"Copyright:\n"
"\n"
" You can use this software free of charge and you can redistribute its binary\n"
" package freely but:\n"
"  1. You are not allowed to use any part of sources of this software.\n"
"  2. You are not allowed to redistribute any part of sources of this software.\n"
"  3. You are not allowed to reverse engineer this software.\n"
"  4. If you want to distribute a binary package of this software you cannot\n"
"     demand any fees for it. You cannot even demand\n"
"     a return of cost of the media or distribution (CD for example).\n"
"  5. You cannot involve this software in any commercial activity (for example\n"
"     as a free add-on to paid software or newspaper).\n"
" This program is distributed in the hope that it will be useful, but WITHOUT\n"
" ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or\n"
" FITNESS FOR A PARTICULAR PURPOSE. Use it at your own risk.\n"
"*/\n";

#include <cstdio>
#include <cstdlib>
#include <libintl.h>
#include <locale.h>
#include <sys/stat.h>
#include <sys/resource.h>

#include "base.hxx"
M_VCSID( "$Id: "__ID__" $" )
M_VCSID( "$Id: "__TID__" $" )
#include "hcore.hxx"
#include "memory.hxx"
#include "hlog.hxx"
#include "hstring.hxx"
#include "htokenizer.hxx"
#include "hprogramoptionshandler.hxx"
#include "hsingleton.hxx"
#include "hopenssl.hxx"
#include "hsocket.hxx"
#include "pod.hxx"

namespace yaal {

namespace memory {

extern ON_ALLOC_FAILURE::on_alloc_failure_t _onAllocFailure_;

}

namespace hcore {

int _debugLevel_ = 0;
HLog& log( HLog::get_instance( 1000 ) );

yaal::hcore::HString HOpenSSL::_sSLKey;
yaal::hcore::HString HOpenSSL::_sSLCert;

HProgramOptionsHandler& yaal_options( void ) {
	M_PROLOG
	static HProgramOptionsHandler yaalOptions;
	return ( yaalOptions );
	M_EPILOG
}

/* mathematical macros */
static double long const EPSILON = 0.000001;
bool eq( double long left_, double long right_ ) {
	return ( ( ( ( ( left_ ) > ( right_ ) )
					? ( ( left_ ) - ( right_ ) )
					: ( ( right_ ) - ( left_ ) ) ) < EPSILON ) );
}

bool set_hcore_variables( HString& option_, HString& value_ ) {
	M_PROLOG
	bool fail( false );
	if ( ! strcasecmp( option_, "set_env" ) )
		decode_set_env( value_ );
	else if ( ! strcasecmp( option_, "log_mask" ) ) {
		HTokenizer t( value_, " \t" );
		for ( HTokenizer::HIterator it = t.begin(), end = t.end(); it != end; ++ it ) {
			if ( ! strcasecmp( *it, "LOG_DEBUG" ) )
				HLog::_logMask |= LOG_TYPE::DEBUG;
			else if ( ! strcasecmp( *it, "LOG_INFO" ) )
				HLog::_logMask |= LOG_TYPE::INFO;
			else if ( ! strcasecmp( *it, "LOG_NOTICE" ) )
				HLog::_logMask |= LOG_TYPE::NOTICE;
			else if ( ! strcasecmp( *it, "LOG_WARNING" ) )
				HLog::_logMask |= LOG_TYPE::WARNING;
			else if ( ! strcasecmp( *it, "LOG_ERROR" ) )
				HLog::_logMask |= LOG_TYPE::ERROR;
			else if ( ! strcasecmp( *it, "LOG_VCSHEADER" ) )
				HLog::_logMask |= LOG_TYPE::VCSHEADER;
			else
				fail = true;
		}
	} else if ( ! strcasecmp( option_, "semaphore_type" ) ) {
		if ( value_ == "POSIX" )
			HSemaphore::DEFAULT = HSemaphore::TYPE::POSIX;
		else if ( value_ == "YAAL" )
			HSemaphore::DEFAULT = HSemaphore::TYPE::YAAL;
		else
			fail = true;
	} else if ( ! strcasecmp( option_, "on_alloc_failure" ) ) {
		if ( value_ == "ABORT" )
			memory::_onAllocFailure_ = memory::ON_ALLOC_FAILURE::ABORT;
		else if ( value_ == "THROW" )
			memory::_onAllocFailure_ = memory::ON_ALLOC_FAILURE::THROW;
		else
			fail = true;
	} else
		fail = true;
	return ( fail );
	M_EPILOG
}

void set_env( HString const& name_, HString const& value_, bool overwrite_ ) {
	M_PROLOG
	int const TRUE = 1;
	int const FALSE = 0;
	M_ENSURE( ::setenv( name_.raw(), value_.raw(), overwrite_ ? TRUE : FALSE ) == 0 );
	return;
	M_EPILOG
}

void decode_set_env( HString line ) {
	M_PROLOG
	int long eon = 0;
	if ( ( line.length() < 3 )
			|| ( ( eon = line.find_one_of( " \t" ) ) == -1 ) ) {
		log( LOG_TYPE::ERROR ) << "bad set_env argument: `";
		log << line << '\'' << endl;
		return;
	}
	int long valPos = line.find_other_than( " \t", eon );
	if ( valPos < 0 ) {
		log( LOG_TYPE::ERROR ) << "no value for environment variable in set_env: `";
		log << line << '\'' << endl;
		return;
	}
	HString val = line.mid( valPos );
	line.set_at( eon, '\0' );
	set_env( line, val );
	return;
	M_EPILOG
}

namespace {

double long std_strtold( HString const& str_ ) {
	return ( ::strtold( str_.raw(), NULL ) );
}

}

/*! \brief Extend features of one library in another library.
 */
namespace extendable {

yaal_strtold_t acting_strtold = &std_strtold;

void set_strtold_impl( yaal_strtold_t newStrtold_ ) {
	acting_strtold = newStrtold_;
}

}

double long strtold( HString const& str_ ) {
	return ( extendable::acting_strtold( str_ ) );
}

class HCoreInitDeinit {
public:
	HCoreInitDeinit( void );
} initDeinit;

void init_locale( char const* const package_ ) {
	M_PROLOG
	char* gettextPath( ::getenv( "GETTEXT_PATH" ) );
	if ( gettextPath ) {
		bindtextdomain( package_ ? package_ : PACKAGE_NAME, gettextPath );
		if ( package_ ) {
			textdomain( package_ );
			setlocale( LC_ALL, "" );
		}
	}
	return;
	M_EPILOG
}

namespace {
static char const SYSCALL_FAILURE[] = "syscall failure - bailng out";
}

namespace {
#pragma GCC diagnostic ignored "-Wold-style-cast"
static int long const FWD_RLIM_INFINITY = RLIM_INFINITY;
#pragma GCC diagnostic error "-Wold-style-cast"
}

void ensure_limit( int resource_, char const* message_ ) {
	rlimit rl = { 0, 0 };
	if ( ::getrlimit( resource_, &rl ) != 0 ) {
		::perror( SYSCALL_FAILURE );
		::exit( 1 );
	}
	if ( static_cast<int long>( rl.rlim_cur ) == static_cast<int long>( FWD_RLIM_INFINITY ) ) {
		::perror( message_ );
		::exit( 1 );
	}
}

HCoreInitDeinit::HCoreInitDeinit( void ) {
/* cppcheck-suppress variableHidingTypedef */
	STATIC_ASSERT( sizeof( int ) >= 4 );
/* cppcheck-suppress variableHidingTypedef */
	STATIC_ASSERT( sizeof( u8_t ) == 1 );
/* cppcheck-suppress variableHidingTypedef */
	STATIC_ASSERT( sizeof( u16_t ) == 2 );
/* cppcheck-suppress variableHidingTypedef */
	STATIC_ASSERT( sizeof( u32_t ) == 4 );
	STATIC_ASSERT( sizeof( u64_t ) == 8 );
	errno = 0;
	if ( ! ( getuid() && geteuid() ) ) {
		::perror( "running with super-user privileges - bailing out" );
		::exit( 1 );
	}
	mode_t const lockOutUmask( S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IWGRP | S_IXGRP | S_IROTH | S_IWOTH | S_IXOTH );
	mode_t curUmask( ::umask( lockOutUmask ) );
	if ( ::umask( curUmask ) != lockOutUmask ) {
		::perror( SYSCALL_FAILURE );
		exit( 1 );
	}
	if ( ( ~curUmask ) & ( S_IROTH | S_IWOTH | S_IXOTH )  ) {
		::perror( "running with too permissive umask - bailing out" );
		::exit( 1 );
	}
#if ( HAVE_DECL_RLIMIT_AS == 1 )
	ensure_limit( RLIMIT_AS, "unlimited VM size - bailing out" );
#endif /* #if ( HAVE_DECL_RLIMIT_AS == 1 ) */
#ifndef __HOST_OS_TYPE_CYGWIN__
	ensure_limit( RLIMIT_DATA, "unlimited data size - bailing out" );
#endif /* #ifndef __HOST_OS_TYPE_CYGWIN__ */
	ensure_limit( RLIMIT_STACK, "unlimited stack size - bailing out" );
	ensure_limit( RLIMIT_NOFILE, "unlimited open descriptors count - bailing out" );
#if ( HAVE_DECL_RLIMIT_NPROC == 1 )
	ensure_limit( RLIMIT_NPROC, "unlimited process count - bailing out" );
#endif /* #if ( HAVE_DECL_RLIMIT_NPROC == 1 ) */
	init_locale();
	char* env( ::getenv( "YAAL_DEBUG" ) );
	if ( env )
		_debugLevel_ = lexical_cast<int>( env );
	HString dummy;
	yaal_options()( "ssl_key", program_options_helper::option_value( HOpenSSL::_sSLKey ), HProgramOptionsHandler::OOption::TYPE::REQUIRED, "Path to the OpenSSL private key file.", "path" )
		( "ssl_cert", program_options_helper::option_value( HOpenSSL::_sSLCert ), HProgramOptionsHandler::OOption::TYPE::REQUIRED, "Path to the OpenSSL certificate file.", "path" )
		( "resolve_hostnames", program_options_helper::option_value( HSocket::_resolveHostnames ), HProgramOptionsHandler::OOption::TYPE::REQUIRED, "Resolve IP address into host names." )
		( "thread_stack_size", program_options_helper::option_value( HThread::_threadStackSize ), HProgramOptionsHandler::OOption::TYPE::REQUIRED, "Set size of stack for newly created threads." )
		( "semaphore_type", program_options_helper::option_value( dummy ), HProgramOptionsHandler::OOption::TYPE::REQUIRED, "Default semaphore implementation type.", "type" )
		( "on_alloc_failure", program_options_helper::option_value( dummy ), HProgramOptionsHandler::OOption::TYPE::REQUIRED, "Memory allocation failure handling policy.", "policy" )
		( "write_timeout", program_options_helper::option_value( _writeTimeout_ ), HProgramOptionsHandler::OOption::TYPE::REQUIRED, "Time-out for low level write operations." );
	yaal_options().process_rc_file( "yaal", "core", set_hcore_variables );
	if ( _writeTimeout_ < LOW_TIMEOUT_WARNING )
		log( LOG_TYPE::WARNING ) << "Low default timout for write operations!" << endl;
	return;
}

void banner( char const* packageName_, char const* packageVersion_ ) {
	::printf( COPYRIGHT, packageName_ ? packageName_ : PACKAGE_NAME, packageVersion_ ? packageVersion_ : PACKAGE_VERSION );
	::printf(
		"Available libraries:\n"
		"\thcore\n" );
	return;
}

}

}

extern "C"
int yaal_hcore_main( int, char** ) {
	static char const dynamicLinkerPath[]
		__attribute__(( __section__(".interp") )) = __DYNAMIC_LINKER__;
	if ( dynamicLinkerPath[ 0 ] ) {
		yaal::hcore::banner();
		::exit( 0 );
	}
	return ( 0 );
}

