/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include <unistd.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cctype>
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

char const* host_info_string( void ) {
	return ( HOST_INFO " " TARGET_CPU );
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

namespace {

class HPermissiveUTF8Decoder {
private:
	HChunk _buffer;
	HString::size_type _characterCount;
	int _rank;
public:
	HPermissiveUTF8Decoder( void )
		: _buffer()
		, _characterCount( 0 )
		, _rank( 1 ) {
	}

	void push_back( u32_t character_, int newRank_ ) {
		int newRank( max( _rank, newRank_ ) );
		_buffer.realloc( ( _characterCount + 1 ) * newRank );
		if ( newRank > _rank ) {
			switch ( 4 * newRank + _rank ) {
				case ( 4 * 2 + 1 ): copy_backward( _buffer.get<u8_t>(), _buffer.get<u8_t>() + _characterCount, _buffer.get<u16_t>() + _characterCount ); break;
				case ( 4 * 4 + 1 ): copy_backward( _buffer.get<u8_t>(), _buffer.get<u8_t>() + _characterCount, _buffer.get<u32_t>() + _characterCount ); break;
				case ( 4 * 4 + 2 ): copy_backward( _buffer.get<u16_t>(), _buffer.get<u16_t>() + _characterCount, _buffer.get<u32_t>() + _characterCount ); break;
			}
		}
		switch ( newRank ) {
			case ( 1 ): _buffer.get<u8_t>()[_characterCount] = static_cast<u8_t>( character_ ); break;
			case ( 2 ): _buffer.get<char16_t>()[_characterCount] = static_cast<u16_t>( character_ ); break;
			case ( 4 ): _buffer.get<char32_t>()[_characterCount] = character_; break;
		}
		++ _characterCount;
		_rank = newRank;
	}

	u8_t to_hex( u8_t val_ ) {
		char val( static_cast<char>( val_ ) );
		return ( static_cast<u8_t>( val > 9 ? 'a' + val - 10 : '0' + val ) );
	}

	void escape_invalid( u8_t invalid_ ) {
		push_back( '\\', 1 );
		push_back( 'x', 1 );
		push_back( to_hex( invalid_ / 16 ), 1 );
		push_back( to_hex( invalid_ % 16 ), 1 );
	}

	void decode_bytes( HString& str_, char const* bytes_, HString::size_type size_ ) {
		M_PROLOG
		int byteCount( 0 );
		int charBytesLeft( 0 );
		int charBytesExpected( 0 );
		int tryRank( 0 );
		u32_t cp( 0 );
		char const* p( bytes_ );
		while ( ( byteCount < size_ ) && *p ) {
			if ( charBytesLeft == 0 ) {
				if ( ! ( *p & unicode::ENC_1_BYTES_MARK_MASK ) ) {
					push_back( static_cast<u8_t>( *p ), 1 );
				} else if ( ( *p & unicode::ENC_2_BYTES_MARK_MASK ) == unicode::ENC_2_BYTES_MARK_VALUE ) {
					charBytesLeft = 1;
					charBytesExpected = charBytesLeft + 1;
					tryRank = 0; /* 3 byte UTF-8 sequence can have either _rank == 2 or _rank == 1 */
					cp = ( *p & unicode::ENC_2_BYTES_VALUE_MASK );
				} else if ( ( *p & unicode::ENC_3_BYTES_MARK_MASK ) == unicode::ENC_3_BYTES_MARK_VALUE ) {
					charBytesLeft = 2;
					charBytesExpected = charBytesLeft + 1;
					tryRank = 1; /* 3 byte UTF-8 sequence can have either _rank == 4 or _rank == 2 */
					cp = ( *p & unicode::ENC_3_BYTES_VALUE_MASK );
				} else if ( ( *p & unicode::ENC_4_BYTES_MARK_MASK ) == unicode::ENC_4_BYTES_MARK_VALUE ) {
					charBytesLeft = 3;
					charBytesExpected = charBytesLeft + 1;
					tryRank = 2;
					cp = ( *p & unicode::ENC_4_BYTES_VALUE_MASK );
				} else {
					escape_invalid( static_cast<u8_t>( *p ) );
					charBytesLeft = 0;
				}
			} else if ( ( *p & unicode::TAIL_BYTES_MARK_MASK ) == unicode::TAIL_BYTES_MARK_VALUE ) {
				-- charBytesLeft;
				cp <<= 6;
				cp |= ( *p & unicode::TAIL_BYTES_VALUE_MASK );
				if ( ! charBytesLeft ) {
					static u32_t const UCS_RANK_CUTOFF[] = {
						unicode::UCS_MAX_1_BYTE_CODE_POINT,
						unicode::UCS_MAX_2_BYTE_CODE_POINT,
						0
					};
					static int const RANKS[][3] = { { 1, 2, 4 }, { 2, 4, 4 } };
					push_back( cp, RANKS[ cp > UCS_RANK_CUTOFF[tryRank] ][tryRank] );
				}
			} else {
				int backtrackSize( charBytesExpected - charBytesLeft );
				for ( int i( 0 ); i < backtrackSize; ++ i ) {
					escape_invalid( static_cast<u8_t>( *( p - backtrackSize + i ) ) );
				}
				charBytesLeft = 0;
				continue;
			}
			++ p;
			++ byteCount;
		}
		if ( charBytesLeft > 0 ) {
			int backtrackSize( charBytesExpected - charBytesLeft );
			for ( int i( 0 ); i < backtrackSize; ++ i ) {
				escape_invalid( static_cast<u8_t>( *( p - backtrackSize + i ) ) );
			}
		}
		if ( _characterCount == 0 ) {
			str_.clear();
			return;
		}
		switch ( _rank ) {
			case ( 1 ): str_.assign( _buffer.raw(), _characterCount ); break;
			case ( 2 ): str_.assign( _buffer.get<char16_t>(), _characterCount ); break;
			case ( 4 ): str_.assign( _buffer.get<char32_t>(), _characterCount ); break;
		}
		return;
		M_EPILOG
	}
};

}

HString bytes_to_string( char const* bytes_, HString::size_type size_ ) {
	M_PROLOG
	HString str;
	HPermissiveUTF8Decoder permissiveUtf8Decoder;
	permissiveUtf8Decoder.decode_bytes( str, bytes_, size_ != HString::npos ? size_ : static_cast<HString::size_type>( ::strlen( bytes_ ) ) );
	return ( str );
	M_EPILOG
}

void bytes_to_string( HString& str_, char const* bytes_, HString::size_type size_ ) {
	M_PROLOG
	HPermissiveUTF8Decoder permissiveUtf8Decoder;
	permissiveUtf8Decoder.decode_bytes( str_, bytes_, size_ != HString::npos ? size_ : static_cast<HString::size_type>( ::strlen( bytes_ ) ) );
	return;
	M_EPILOG
}

inline char from_hex( int hi_, int lo_ ) {
	int hi( isdigit( hi_ ) ? hi_ - '0' : tolower( hi_ ) + 10 - 'a' );
	int lo( isdigit( lo_ ) ? lo_ - '0' : tolower( lo_ ) + 10 - 'a' );
	return ( static_cast<char>( hi * 16 + lo ) );
}

bytes_t string_to_bytes( HString const& str_ ) {
	M_PROLOG
	HUTF8String utf8( str_ );
	bytes_t bytes( utf8.c_str(), utf8.c_str() + utf8.byte_count() + 1 );
	char const* p( bytes.data() );
	while ( p ) {
		p = strstr( p, "\\x" );
		if ( ! p ) {
			break;
		}
		int long idx( p - bytes.data() );
		if ( idx > ( bytes.get_size() - 5 ) ) {
			break;
		}
		int hi( p[2] );
		if ( ! isxdigit( hi ) ) {
			p += 2;
			continue;
		}
		int lo( p[3] );
		if ( ! isxdigit( lo ) ) {
			p += 3;
			continue;
		}
		bytes.erase( bytes.begin() + idx + 1, bytes.begin() + idx + 4 );
		bytes[idx] = from_hex( hi, lo );
		++ p;
	}
	return ( bytes );
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
	try {
		system::mode_t tooLooseUmask( S_IROTH | S_IWOTH | S_IXOTH );
		system::mode_t curUmask( system::get_umask() );
		if ( ( ~curUmask ) & tooLooseUmask ) {
			if ( autoSanity_ ) {
				system::mode_t const modestUmask( S_IRGRP | S_IWGRP | S_IXGRP | S_IROTH | S_IWOTH | S_IXOTH );
				system::set_umask( modestUmask );
				log( LOG_LEVEL::WARNING ) << "running with too permissive umask - setting umask automatically" << endl;
			} else {
				::perror( "running with too permissive umask - bailing out" );
				::exit( 1 );
			}
		}
	} catch ( ... ) {
		::perror( SYSCALL_FAILURE );
		exit( 1 );
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

