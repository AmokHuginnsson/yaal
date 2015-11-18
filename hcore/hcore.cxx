#include <unistd.h>
#include "config.hxx"
char const COPYRIGHT [ ] =
"/*\n"
"---           `%s' %s (c) 1978 by Marcin 'Amok' Konarski            ---\n"
"\n"
/* hcore.cxx - this file is integral part of `yaal' project.
 */
"  i.  You may not make any changes in Copyright information.\n"
"  ii. You must attach Copyright information to any part of every copy\n"
"      of this software.\n"
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
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
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
#include "commit_id.hxx"

namespace yaal {

namespace memory {

extern ON_ALLOC_FAILURE _onAllocFailure_;

}

namespace hcore {

int _debugLevel_( 0 );
HLog& log( HLog::get_instance( 1000 ) );

yaal::hcore::HString HOpenSSL::_sSLKey;
yaal::hcore::HString HOpenSSL::_sSLCert;

HProgramOptionsHandler& yaal_options( void ) {
	M_PROLOG
	static HProgramOptionsHandler yaalOptions;
	return ( yaalOptions );
	M_EPILOG
}

char const* yaal_version( bool commitId_ ) {
	return ( commitId_ ? PACKAGE_VERSION " " COMMIT_ID : PACKAGE_VERSION );
}

/* mathematical macros */
static double long const EPSILON = 0.000001;
bool eq( double long left_, double long right_ ) {
	return ( ( ( ( ( left_ ) > ( right_ ) )
					? ( ( left_ ) - ( right_ ) )
					: ( ( right_ ) - ( left_ ) ) ) < EPSILON ) );
}

namespace {

bool set_hcore_variables( HString& option_, HString& value_ ) {
	M_PROLOG
	bool fail( false );
	if ( ! strcasecmp( option_, "set_env" ) ) {
		decode_set_env( value_ );
	} else {
		fail = true;
	}
	return ( !fail );
	M_EPILOG
}

}

void set_env( HString const& name_, HString const& value_, bool overwrite_ ) {
	M_PROLOG
	int const TRUE = 1;
	int const FALSE = 0;
	M_ENSURE( ::setenv( name_.raw(), value_.raw(), overwrite_ ? TRUE : FALSE ) == 0 );
	return;
	M_EPILOG
}

void unset_env( HString const& name_ ) {
	M_PROLOG
	M_ENSURE( ::unsetenv( name_.raw() ) == 0 );
	return;
	M_EPILOG
}

void decode_set_env( HString line ) {
	M_PROLOG
	int long eon = 0;
	if ( ( line.length() < 3 )
			|| ( ( eon = line.find_one_of( " \t" ) ) == -1 ) ) {
		log( LOG_LEVEL::ERROR ) << "bad set_env argument: `";
		log << line << '\'' << endl;
		return;
	}
	int long valPos = line.find_other_than( " \t", eon );
	if ( valPos < 0 ) {
		log( LOG_LEVEL::ERROR ) << "no value for environment variable in set_env: `";
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

double long std_strtold( HString const& str_, int* endIdx_ ) {
	char* endPtr( NULL );
	double long value( ::strtold( str_.raw(), &endPtr ) );
	if ( endIdx_ )
		*endIdx_ = static_cast<int>( endPtr - str_.raw() );
	return ( value );
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

double long strtold( HString const& str_, int* endIdx_ ) {
	return ( extendable::acting_strtold( str_, endIdx_ ) );
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
#pragma GCC diagnostic ignored "-Wold-style-cast"
static int long const FWD_RLIM_INFINITY = static_cast<int long>( RLIM_INFINITY );
#pragma GCC diagnostic error "-Wold-style-cast"
}

namespace {

static char const SYSCALL_FAILURE[] = "syscall failure - bailng out";

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

}

HCoreInitDeinit::HCoreInitDeinit( void ) {
	static_assert( sizeof( int ) >= 4, "insufficient size of `int' type" );
	static_assert( sizeof( u8_t ) == 1, "bad size of `u8_t' type" );
	static_assert( sizeof( u16_t ) == 2, "bad size of `u16_t' type" );
	static_assert( sizeof( u32_t ) == 4, "bad size of `u32_t' type" );
	static_assert( sizeof( u64_t ) == 8, "bad size of `u64_t' type" );
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
#ifndef __HOST_OS_TYPE_CYGWIN__
#if ( HAVE_DECL_RLIMIT_AS == 1 )
	ensure_limit( RLIMIT_AS, "unlimited VM size - bailing out" );
#endif /* #if ( HAVE_DECL_RLIMIT_AS == 1 ) */
	ensure_limit( RLIMIT_DATA, "unlimited data size - bailing out" );
#endif /* #ifndef __HOST_OS_TYPE_CYGWIN__ */
	ensure_limit( RLIMIT_STACK, "unlimited stack size - bailing out" );
	ensure_limit( RLIMIT_NOFILE, "unlimited open descriptors count - bailing out" );
#if ( HAVE_DECL_RLIMIT_NPROC == 1 )
	ensure_limit( RLIMIT_NPROC, "unlimited process count - bailing out" );
#endif /* #if ( HAVE_DECL_RLIMIT_NPROC == 1 ) */
	init_locale();
	char* env( ::getenv( "YAAL_DEBUG" ) );
	if ( env ) {
		_debugLevel_ = lexical_cast<int>( env );
	}
	bool enableExceptionLogging( true );
	yaal_options()(
		HProgramOptionsHandler::HOption()
		.long_form( "log_level" )
		.switch_type( HProgramOptionsHandler::HOption::ARGUMENT::REQUIRED )
		.description( "Log severity level above which messages are actually processed." )
		.argument_name( "level" )
		.setter(
			[]( yaal::hcore::HString const& value_ ) {
				for ( int l( LOG_LEVEL::MIN ); l <= LOG_LEVEL::MAX; ++ l ) {
					if ( ! strcasecmp( value_, LOG_LEVEL::name( static_cast<LOG_LEVEL::priority_t>( l ) ) ) ) {
						HLog::_logLevel = static_cast<LOG_LEVEL::priority_t>( l );
						return;
					}
				}
				throw HProgramOptionsHandlerException( "Bad severity level: "_ys.append( value_ ) );
			}
		)
		.getter(
			[]( void ) {
				return ( LOG_LEVEL::name( HLog::_logLevel ) );
			}
		)
	)(
		HProgramOptionsHandler::HOption()
		.long_form( "on_alloc_failure" )
		.switch_type( HProgramOptionsHandler::HOption::ARGUMENT::REQUIRED )
		.description( "Specify how to handle memory allocation failures." )
		.argument_name( "strategy" )
		.setter(
			[]( yaal::hcore::HString const& value_ ) {
				if ( value_ == "ABORT" ) {
					memory::_onAllocFailure_ = memory::ON_ALLOC_FAILURE::ABORT;
				} else if ( value_ == "THROW" ) {
					memory::_onAllocFailure_ = memory::ON_ALLOC_FAILURE::THROW;
				} else {
					throw HProgramOptionsHandlerException( "Bad allocation failure strategy: "_ys.append( value_ ) );
				}
			}
		)
		.getter(
			[]( void ) {
				return ( memory::_onAllocFailure_ == memory::ON_ALLOC_FAILURE::THROW ? "THROW" : "ABORT" );
			}
		)
	)(
		HProgramOptionsHandler::HOption()
		.long_form( "ssl_key" )
		.switch_type( HProgramOptionsHandler::HOption::ARGUMENT::REQUIRED )
		.description( "Path to the OpenSSL private key file." )
		.recipient( HOpenSSL::_sSLKey )
		.argument_name( "path" )
	)(
		HProgramOptionsHandler::HOption()
		.long_form( "ssl_cert" )
		.switch_type( HProgramOptionsHandler::HOption::ARGUMENT::REQUIRED )
		.description( "Path to the OpenSSL certificate file." )
		.recipient( HOpenSSL::_sSLCert )
		.argument_name( "path" )
	)(
		HProgramOptionsHandler::HOption()
		.long_form( "resolve_hostnames" )
		.switch_type( HProgramOptionsHandler::HOption::ARGUMENT::REQUIRED )
		.description( "Resolve IP address into host names." )
		.recipient( HSocket::_resolveHostnames )
	)(
		HProgramOptionsHandler::HOption()
		.long_form( "thread_stack_size" )
		.switch_type( HProgramOptionsHandler::HOption::ARGUMENT::REQUIRED )
		.description( "Set size of stack for newly created threads." )
		.recipient( HThread::_threadStackSize )
	)(
		HProgramOptionsHandler::HOption()
		.long_form( "write_timeout" )
		.switch_type( HProgramOptionsHandler::HOption::ARGUMENT::REQUIRED )
		.description( "Time-out for low level write operations." )
		.recipient( _writeTimeout_ )
	)(
		HProgramOptionsHandler::HOption()
		.long_form( "exception_logging" )
		.switch_type( HProgramOptionsHandler::HOption::ARGUMENT::REQUIRED )
		.description( "Enable automatic exception stack frames logging." )
		.recipient( enableExceptionLogging )
	);
	if ( enableExceptionLogging ) {
		HException::enable_logging();
	} else {
		HException::disable_logging();
	}
	yaal_options().process_rc_file( "yaal", "core", set_hcore_variables );
	if ( _writeTimeout_ < LOW_TIMEOUT_WARNING )
		log( LOG_LEVEL::WARNING ) << "Low default timout for write operations!" << endl;
	return;
}

void banner( char const* packageName_, char const* packageVersion_ ) {
	HLog::disable_auto_rehash();
	::printf( COPYRIGHT, packageName_ ? packageName_ : PACKAGE_NAME, packageVersion_ ? packageVersion_ : PACKAGE_VERSION );
	::printf( "\nVCS id: %s\n\n", COMMIT_ID );
	::printf(
		"Available libraries:\n"
		"\thcore\n" );
	return;
}

}

}

#ifdef __DYNAMIC_LINKER__
extern "C"
int yaal_hcore_main( int, char** ) __attribute__((noreturn));
extern "C"
int yaal_hcore_main( int, char** ) {
	static char const dynamicLinkerPath[]
#ifdef HAVE_INTERP_ALLOWED
		__attribute__(( __section__(".interp") ))
#endif /* #ifdef HAVE_INTERP_ALLOWED */
		= __DYNAMIC_LINKER__;
	if ( dynamicLinkerPath[ 0 ] ) {
		yaal::hcore::banner();
		::printf( "\n" );
	}
	::exit( 0 );
}
#else /* #ifdef __DYNAMIC_LINKER__ */
extern "C"
int yaal_hcore_main( int, char** );
extern "C"
int yaal_hcore_main( int, char** ) {
	return ( 0 );
}
#endif /* #else #ifdef __DYNAMIC_LINKER__ */

