/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include <unistd.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
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
#include "unicode.hxx"
#include "htokenizer.hxx"
#include "hprogramoptionshandler.hxx"
#include "hsingleton.hxx"
#include "hopenssl.hxx"
#include "hsocket.hxx"
#include "pod.hxx"
#include "system.hxx"
#include "hregex.hxx"
#include "config.hxx"
#include "license.hxx"
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
	if ( ! stricasecmp( option_, "set_env" ) ) {
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
	HUTF8String name( name_ );
	HUTF8String value( value_ );
	M_ENSURE( ::setenv( name.c_str(), value.c_str(), overwrite_ ? TRUE : FALSE ) == 0 );
	return;
	M_EPILOG
}

void unset_env( HString const& name_ ) {
	M_PROLOG
	HUTF8String unicode( name_ );
	M_ENSURE( ::unsetenv( unicode.c_str() ) == 0 );
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
	line.set_at( eon, 0_ycp );
	set_env( line, val );
	return;
	M_EPILOG
}

void substitute_environment( HString& string_, ENV_SUBST_MODE mode_ ) {
	M_PROLOG
	bool envVarRefFound( false );
	do {
		envVarRefFound = false;
		if ( ! string_.is_empty() ) {
			HRegex pattern( "[$][{][^{}]+[}]" );
			HString substituted(
				pattern.replace(
					string_,
					HRegex::replacer_t(
						[]( yaal::hcore::HString const& match_ ) -> yaal::hcore::HString {
							HString name( match_.substr( 2, match_.get_length() - 3 ) );
							HString::size_type modIdx( name.find( ':'_ycp ) );
							HString option;
							while ( modIdx != HString::npos ) {
								option = name.substr( modIdx  + 1 );
								name.erase( modIdx );
								if ( option.is_empty() ) {
									break;
								}
								if ( option.front() == '-' ) {
									option.shift_left( 1 );
								} else {
									option.clear();
								}
								break;
							}
							HUTF8String utf8( name );
							char const* envVal = ::getenv( utf8.c_str() );
							return ( envVal ? envVal : option );
						}
					)
				)
			);
			envVarRefFound = ( substituted != string_ );
			string_.assign( substituted	);
		}
	} while ( ( mode_ == ENV_SUBST_MODE::RECURSIVE ) && envVarRefFound );
	return;
	M_EPILOG
}

yaal::hcore::HString substitute_environment( yaal::hcore::HString const& str_ ) {
	M_PROLOG
	HString str( str_ );
	substitute_environment( str, ENV_SUBST_MODE::RECURSIVE );
	return ( str );
	M_EPILOG
}

namespace hidden {

inline HString::const_iterator skip_whitespace( HString const& str_ ) {
	int long nonWhiteSpace( str_.find_other_than( character_class<CHARACTER_CLASS::WHITESPACE>().data() ) );
	return ( str_.begin() + ( nonWhiteSpace != HString::npos ? nonWhiteSpace : 0 ) );
}

int copy_digits( HString const&, char*, int );
int copy_digits( HString const& str_, char* buf_, int size_ ) {
	HString::const_iterator it( skip_whitespace( str_ ) );
	HString::const_iterator end( str_.end() );
	int skip( static_cast<int>( it - str_.begin() ) );
	copy_ascii( it, end, buf_, size_ );
	return ( skip );
}

double long std_strtold( HString const&, int* );
double long std_strtold( HString const& str_, int* endIdx_ ) {
	static int const MAX_FLOAT_DIGIT_COUNT( 8192 );
	char buf[MAX_FLOAT_DIGIT_COUNT];
	int skip( copy_digits( str_, buf, MAX_FLOAT_DIGIT_COUNT ) );
	char* endPtr( nullptr );
	double long value( ::strtold( buf, &endPtr ) );
	if ( endIdx_ ) {
		*endIdx_ = static_cast<int>( endPtr - buf ) + skip;
	}
	return ( value );
}

}

/*! \brief Extend features of one library in another library.
 */
namespace extendable {

yaal_strtold_t acting_strtold = &hidden::std_strtold;

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

void init_locale( char const* package_ ) {
	M_PROLOG
	char* gettextPath( ::getenv( "GETTEXT_PATH" ) );
	if ( gettextPath ) {
		bindtextdomain( package_ ? package_ : PACKAGE_NAME, gettextPath );
	}
	if ( package_ ) {
		textdomain( package_ );
	}
	setlocale( LC_ALL, "" );
	return;
	M_EPILOG
}

namespace {

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wold-style-cast"
static i64_t const FWD_RLIM_INFINITY = static_cast<i64_t>( RLIM_INFINITY );
#pragma GCC diagnostic pop

static char const YAAL_AUTO_SANITY_ENV[] = "YAAL_AUTO_SANITY";
static char const SYSCALL_FAILURE[] = "syscall failure - bailing out";

void ensure_limit( int resource_, char const* message_, bool autoSanity_ ) {
	rlimit rl = { 0, 0 };
	if ( ::getrlimit( resource_, &rl ) != 0 ) {
		::perror( SYSCALL_FAILURE );
		::exit( 1 );
	}
	if ( static_cast<i64_t>( rl.rlim_cur ) == FWD_RLIM_INFINITY ) {
		if ( autoSanity_ ) {
#ifndef __HOST_OS_TYPE_CYGWIN__
			system::HResourceInfo mem( system::get_memory_size_info() );
#endif /* #ifndef __HOST_OS_TYPE_CYGWIN__ */
			switch ( resource_ ) {
#ifndef __HOST_OS_TYPE_CYGWIN__
#if ( HAVE_DECL_RLIMIT_AS == 1 )
				case ( RLIMIT_AS ): {
					rl.rlim_cur = rl.rlim_max = static_cast<rlim_t>( ( mem.total() / 10 ) * 9 );
				} break;
#endif /* #if ( HAVE_DECL_RLIMIT_AS == 1 ) */
				case ( RLIMIT_DATA ): {
					rl.rlim_cur = rl.rlim_max = static_cast<rlim_t>( ( mem.total() / 10 ) * 9 );
				} break;
#endif /* #ifndef __HOST_OS_TYPE_CYGWIN__ */
				case ( RLIMIT_STACK ): {
					static int const MAX_STACK_SIZE( 8 * 1024 * 1024 );
					rl.rlim_cur = rl.rlim_max = MAX_STACK_SIZE;
				} break;
				case ( RLIMIT_NOFILE ): {
					static int const MAX_OPEN_FILES( 1024 );
					rl.rlim_cur = rl.rlim_max = MAX_OPEN_FILES;
				} break;
#if ( HAVE_DECL_RLIMIT_NPROC == 1 )
				case ( RLIMIT_NPROC ): {
					static int const PROC_PER_CORE( 128 );
					int nProc( system::get_core_count_info() );
					rl.rlim_cur = rl.rlim_max = static_cast<rlim_t>( nProc * PROC_PER_CORE );
				} break;
#endif /* #if ( HAVE_DECL_RLIMIT_NPROC == 1 ) */
				default: {
					M_ASSERT( !"Invalid resource type!"[0] );
				}
			}
			if ( ::setrlimit( resource_, &rl ) == 0 ) {
				log( LOG_LEVEL::NOTICE ) << message_ << " - setting limit automatically - " << rl.rlim_cur << endl;
			} else {
				int e( errno );
				if ( ::getenv( YAAL_AUTO_SANITY_ENV ) == nullptr ) {
					log( LOG_LEVEL::WARNING ) << message_ << " - failed to set limit automatically - " << rl.rlim_cur << ": " << strerror( e ) <<  endl;
				} else {
					::perror( SYSCALL_FAILURE );
					::exit( 1 );
				}
			}
		} else {
			HString message( message_ );
			message.append( " - bailing out" );
			HUTF8String unicode( message );
			::perror( unicode.c_str() );
			::exit( 1 );
		}
	}
}

void sanitize( bool autoSanity_ ) {
	mode_t const modestUmask( S_IRGRP | S_IWGRP | S_IXGRP | S_IROTH | S_IWOTH | S_IXOTH );
	mode_t const lockOutUmask( S_IRUSR | S_IWUSR | S_IXUSR | modestUmask );
	mode_t curUmask( ::umask( lockOutUmask ) );
	if ( ::umask( curUmask ) != lockOutUmask ) {
		::perror( SYSCALL_FAILURE );
		exit( 1 );
	}
	if ( ( ~curUmask ) & ( S_IROTH | S_IWOTH | S_IXOTH ) ) {
		if ( autoSanity_ ) {
			if ( ::umask( modestUmask ) != curUmask ) {
				::perror( SYSCALL_FAILURE );
				exit( 1 );
			}
			log( LOG_LEVEL::WARNING ) << "running with too permissive umask - setting umask automatically" << endl;
		} else {
			::perror( "running with too permissive umask - bailing out" );
			::exit( 1 );
		}
	}
#ifndef __HOST_OS_TYPE_CYGWIN__
#if ( HAVE_DECL_RLIMIT_AS == 1 )
	ensure_limit( RLIMIT_AS, "unlimited VM size", autoSanity_ );
#endif /* #if ( HAVE_DECL_RLIMIT_AS == 1 ) */
	ensure_limit( RLIMIT_DATA, "unlimited data size", autoSanity_ );
#endif /* #ifndef __HOST_OS_TYPE_CYGWIN__ */
	ensure_limit( RLIMIT_STACK, "unlimited stack size", autoSanity_ );
	ensure_limit( RLIMIT_NOFILE, "unlimited open descriptors count", autoSanity_ );
#if ( HAVE_DECL_RLIMIT_NPROC == 1 )
	ensure_limit( RLIMIT_NPROC, "unlimited process count", autoSanity_ );
#endif /* #if ( HAVE_DECL_RLIMIT_NPROC == 1 ) */
	return;
}

}

HCoreInitDeinit::HCoreInitDeinit( void ) {
	try {
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
		bool autoSanity( false );
		if ( ::getenv( YAAL_AUTO_SANITY_ENV ) ) {
			autoSanity = true;
		}
#ifdef YAAL_AUTO_SANITY
		autoSanity = true;
#endif
		sanitize( autoSanity );
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
						if ( ! stricasecmp( value_, LOG_LEVEL::name( static_cast<LOG_LEVEL::priority_t>( l ) ) ) ) {
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
		yaal_options().process_rc_file( "core", set_hcore_variables );
		if ( _writeTimeout_ < LOW_TIMEOUT_WARNING ) {
			log( LOG_LEVEL::WARNING ) << "Low default timeout for write operations!" << endl;
		}
	} catch ( HException const& e ) {
		fprintf( stderr, "Failed to initialize yaal-hcore: %s\n", e.what() );
		exit( 1 );
	}
	return;
}

void banner( char const* packageName_, char const* packageVersion_ ) {
	HLog::disable_auto_rehash();
	::printf( LICENSE, packageName_ ? packageName_ : PACKAGE_NAME, packageVersion_ ? packageVersion_ : PACKAGE_VERSION );
	::printf( "VCS id: %s\n\n", COMMIT_ID );
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

