/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include <cstdlib>  /* getenv() */
#include <cstring>  /* strcpy(), strcat() */
#include <cstdio>   /* fopen() */
#include <libintl.h> /* gettext() */
#include <unistd.h> /* getcwd() */

#include "base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "config.hxx"
#include "hprogramoptionshandler.hxx"
#include "functional.hxx"
#include "hstring.hxx"
#include "hformat.hxx"
#include "hchunk.hxx"
#include "system.hxx"
#include "hlog.hxx"
#include "hcore.hxx"

using namespace yaal;
using namespace yaal::hcore;

namespace yaal {

namespace hcore {

struct RC_PATHER {
	enum class PLACEMENT {
		ETC_PRJNAME,
		ETC_CONF,
		ETC,
		BY_EXEC,
		ENV,
		HOME_ETC_PRJNAME,
		HOME_ETC_CONF,
		HOME_ETC,
		HOME_PRJNAME,
		HOME,
		HOME_CONFIG,
		LOCAL
	};

	enum class PHASE {
		NONE,
		GLOBAL,
		USER,
		LOCAL
	};
};

bool read_rc_line( HString&, HString&, HFile&, int& );

namespace {

HString make_path( HString const& sysconfDir_, HString const& rcName_, RC_PATHER::PLACEMENT placement_ ) {
	M_PROLOG
	static const char RC[] = "rc";
	HString rcPath;
	switch ( placement_ ) {
		case ( RC_PATHER::PLACEMENT::ETC_PRJNAME ):
		case ( RC_PATHER::PLACEMENT::ETC_CONF ):
		case ( RC_PATHER::PLACEMENT::ETC ): {
			rcPath.assign( sysconfDir_ )
				.append( ( placement_ == RC_PATHER::PLACEMENT::ETC_CONF ) ? "/conf/" : "/" )
				.append( rcName_ );
			if ( placement_ == RC_PATHER::PLACEMENT::ETC_PRJNAME ) {
				rcPath.append( "/" );
			}
			rcPath.append( RC );
		} break;
		case ( RC_PATHER::PLACEMENT::BY_EXEC ): {
			rcPath.assign( system::get_self_exec_path() ).replace( "\\", "/" );
			HString::size_type sepPos( rcPath.find_last( '/'_ycp ) );
			if ( sepPos != HString::npos ) {
				rcPath.erase( sepPos + 1 ).append( rcName_ );
			} else {
				rcPath.assign( rcName_ );
			}
			rcPath.append( RC );
		} break;
		case ( RC_PATHER::PLACEMENT::ENV ): {
			HString envName( rcName_ );
			envName += RC;
			envName.upper();
			HUTF8String utf8( envName );
			char const* envPath( ::getenv( utf8.c_str() ) );
			if ( envPath ) {
				rcPath = envPath;
			}
		} break;
		case ( RC_PATHER::PLACEMENT::HOME_ETC_PRJNAME ):
		case ( RC_PATHER::PLACEMENT::HOME_ETC_CONF ):
		case ( RC_PATHER::PLACEMENT::HOME_ETC ):
		case ( RC_PATHER::PLACEMENT::HOME ):
		case ( RC_PATHER::PLACEMENT::HOME_PRJNAME ):
		case ( RC_PATHER::PLACEMENT::HOME_CONFIG ): {
			rcPath.assign( system::home_path() );
			if ( placement_ == RC_PATHER::PLACEMENT::HOME_ETC_CONF ) {
				rcPath.append( "/etc/conf/" );
			} else if ( ( placement_ == RC_PATHER::PLACEMENT::HOME_ETC_PRJNAME ) || ( placement_ == RC_PATHER::PLACEMENT::HOME_ETC ) ) {
				rcPath.append( "/etc/" );
			} else if ( ( placement_ == RC_PATHER::PLACEMENT::HOME_PRJNAME ) || ( placement_ == RC_PATHER::PLACEMENT::HOME ) ) {
				rcPath.append( "/." );
			} else {
				M_ASSERT( placement_ == RC_PATHER::PLACEMENT::HOME_CONFIG );
				rcPath.append( "/.config/" );
			}
			rcPath.append( rcName_ );
			if ( ( placement_ == RC_PATHER::PLACEMENT::HOME_ETC_PRJNAME ) || ( placement_ == RC_PATHER::PLACEMENT::HOME_PRJNAME ) ) {
				rcPath.append( "/" );
			}
			rcPath.append( placement_ != RC_PATHER::PLACEMENT::HOME_CONFIG ? RC : ".conf" );
		}	break;
		case ( RC_PATHER::PLACEMENT::LOCAL ): {
			char* cwd( ::getcwd( nullptr, 0 ) );
			rcPath.assign( cwd );
			::free( cwd );
			rcPath.append( "/." ).append( rcName_ ).append( RC );
		}
	}
	return rcPath;
	M_EPILOG
}

int rc_open( HString const& rcPath_, HFile& file_ ) {
	M_PROLOG
	int error = 0;
	if ( !! file_ ) {
		file_.close();
	}
	HScopedValueReplacement<int> saveErrno( errno, 0 );
	error = file_.open( rcPath_, HFile::OPEN::READING );
	HString message( rcPath_ );
	if ( error ) {
		message += " not found, ";
	} else {
		message = "config read from: " + rcPath_;
		message += ", ";
	}
	log << message;
	return error;
	M_EPILOG
}

}

namespace program_options_helper {

struct ORCLoader {
	HProgramOptionsHandler _optionHandler;
	HString _section;
	HProgramOptionsHandler::RC_CALLBACK_t rc_callback;
	ORCLoader( void )
		: _optionHandler()
		, _section()
		, rc_callback( nullptr ) {
	}
	ORCLoader( HProgramOptionsHandler const& optionHandler_, HString const& section_, HProgramOptionsHandler::RC_CALLBACK_t callback )
		: _optionHandler( optionHandler_ )
		, _section( section_ )
		, rc_callback( callback ) {
	}
	ORCLoader( ORCLoader const& loader )
		: _optionHandler( loader._optionHandler )
		, _section( loader._section )
		, rc_callback( loader.rc_callback ) {
	}
	ORCLoader& operator = ( ORCLoader const& loader ) {
		if ( &loader != this ) {
			_optionHandler = loader._optionHandler;
			_section = loader._section;
			rc_callback = loader.rc_callback;
		}
		return ( *this );
	}
};

class HSetup : public HSingleton<HSetup> {
public:
	typedef HSingleton<HSetup> base_type;
	typedef HSetup this_type;
	typedef HMap<HString, ORCLoader> rc_loaders_t;
	typedef rc_loaders_t::iterator iterator;
private:
	rc_loaders_t _rCLoaders;
	bool _locked;
	HSetup( void )
		: _rCLoaders()
		, _locked( false ) {
	}
	virtual ~HSetup( void ) {
	}
public:
	void add_section( HString const& section_, ORCLoader const& orcLoader_ ) {
		M_PROLOG
		_rCLoaders.insert( make_pair( section_, orcLoader_ ) );
		M_EPILOG
	}
	ORCLoader& operator()( HString const& section_ ) {
		M_PROLOG
		rc_loaders_t::iterator it( _rCLoaders.find( section_ ) );
		M_ENSURE( it != _rCLoaders.end() );
		return ( it->second );
		M_EPILOG
	}
	iterator begin() {
		M_PROLOG
		return ( _rCLoaders.begin() );
		M_EPILOG
	}
	iterator end() {
		M_PROLOG
		return ( _rCLoaders.end() );
		M_EPILOG
	}
	void lock( void ) {
		_locked = true;
	}
	void unlock( void ) {
		_locked = false;
	}
	bool is_locked( void ) const {
		return ( _locked );
	}
private:
	friend class HSingleton<HSetup>;
	friend class HDestructor<HSetup>;
};

class HLocker {
	HSetup& _setup;
public:
	HLocker( HSetup& setup_ )
		: _setup( setup_ ) {
		_setup.lock();
	}
	~HLocker( void ) {
		_setup.unlock();
	}
};

}

HProgramOptionsHandler::HOption::HOption( void )
	: _shortForm( 0 )
	, _longForm()
	, _switchType( ARGUMENT::NONE )
	, _description()
	, _argument()
	, _defaultValue()
	, _setter()
	, _getter()
	, _recipientType( TYPE::UNKNOWN )
	, _verbatim( false )
	, _valueId( 0 ) {
	return;
}

HProgramOptionsHandler::HOption::HOption( HProgramOptionsHandler::HOption const& o )
	: _shortForm( o._shortForm )
	, _longForm( o._longForm )
	, _switchType( o._switchType )
	, _description( o._description )
	, _argument( o._argument )
	, _defaultValue( o._defaultValue )
	, _setter( o._setter )
	, _getter( o._getter )
	, _recipientType( o._recipientType )
	, _verbatim( o._verbatim )
	, _valueId( o._valueId ) {
	return;
}

HProgramOptionsHandler::HOption& HProgramOptionsHandler::HOption::operator = ( HProgramOptionsHandler::HOption const& o ) {
	M_PROLOG
	if ( &o != this ) {
		HOption tmp( o );
		swap( tmp );
	}
	return ( *this );
	M_EPILOG
}

void HProgramOptionsHandler::HOption::swap( HProgramOptionsHandler::HOption& o ) {
	M_PROLOG
	if ( &o != this ) {
		using yaal::swap;
		swap( _shortForm, o._shortForm );
		swap( _longForm, o._longForm );
		swap( _switchType, o._switchType );
		swap( _description, o._description );
		swap( _argument, o._argument );
		swap( _defaultValue, o._defaultValue );
		swap( _setter, o._setter );
		swap( _getter, o._getter );
		swap( _recipientType, o._recipientType );
		swap( _verbatim, o._verbatim );
		swap( _valueId, o._valueId );
	}
	return;
	M_EPILOG
}

yaal::hcore::HString const& HProgramOptionsHandler::HOption::long_form( void ) const {
	return ( _longForm );
}

HProgramOptionsHandler::HOption& HProgramOptionsHandler::HOption::long_form( yaal::hcore::HString const& longForm_ ) {
	M_PROLOG
	_longForm = longForm_;
	return ( *this );
	M_EPILOG
}

int HProgramOptionsHandler::HOption::short_form( void ) const {
	return ( _shortForm );
}

HProgramOptionsHandler::HOption& HProgramOptionsHandler::HOption::short_form( int shortForm_ ) {
	_shortForm = shortForm_;
	return ( *this );
}

u64_t HProgramOptionsHandler::HOption::value_id( void ) const {
	M_PROLOG
	return ( _valueId );
	M_EPILOG
}

HProgramOptionsHandler::HOption::ARGUMENT HProgramOptionsHandler::HOption::switch_type( void ) const {
	return ( _switchType );
}

HProgramOptionsHandler::HOption& HProgramOptionsHandler::HOption::switch_type( ARGUMENT switchType_ ) {
	_switchType = switchType_;
	return ( *this );
}

HProgramOptionsHandler::HOption& HProgramOptionsHandler::HOption::setter( setter_t const& setter_ ) {
	M_PROLOG
	_setter = setter_;
	_valueId = reinterpret_cast<u64_t>( _setter.id() );
	return ( *this );
	M_EPILOG
}

HProgramOptionsHandler::HOption& HProgramOptionsHandler::HOption::getter( getter_t const& getter_ ) {
	M_PROLOG
	_getter = getter_;
	return ( *this );
	M_EPILOG
}

TYPE HProgramOptionsHandler::HOption::recipient_type( void ) const {
	return ( _recipientType );
}

bool HProgramOptionsHandler::HOption::used( void ) const {
	return ( !! _setter );
}

void HProgramOptionsHandler::HOption::set( yaal::hcore::HString const& value_ ) {
	M_PROLOG
	_setter( value_ );
	return;
	M_EPILOG
}

yaal::hcore::HString HProgramOptionsHandler::HOption::get( void ) const {
	M_PROLOG
	return ( !! _getter ? _getter() : "" );
	M_EPILOG
}

yaal::hcore::HString const& HProgramOptionsHandler::HOption::description( void ) const {
	return ( _description );
}

HProgramOptionsHandler::HOption& HProgramOptionsHandler::HOption::description( yaal::hcore::HString const& description_ ) {
	M_PROLOG
	_description = description_;
	return ( *this );
	M_EPILOG
}

yaal::hcore::HString const& HProgramOptionsHandler::HOption::argument_name( void ) const {
	return ( _argument );
}

HProgramOptionsHandler::HOption& HProgramOptionsHandler::HOption::argument_name( yaal::hcore::HString const& argument_ ) {
	M_PROLOG
	_argument = argument_;
	return ( *this );
	M_EPILOG
}

yaal::hcore::HString const& HProgramOptionsHandler::HOption::default_value( void ) const {
	return ( _defaultValue );
}

HProgramOptionsHandler::HOption& HProgramOptionsHandler::HOption::default_value( yaal::hcore::HString const& defaultValue_ ) {
	M_PROLOG
	_defaultValue = defaultValue_;
	return ( *this );
	M_EPILOG
}

bool HProgramOptionsHandler::HOption::verbatim( void ) const {
	return ( _verbatim );
}

HProgramOptionsHandler::HOption& HProgramOptionsHandler::HOption::verbatim( bool verbatim_ ) {
	_verbatim = verbatim_;
	return ( *this );
}

HProgramOptionsHandler::HProgramOptionsHandler( yaal::hcore::HString const& projectName_, yaal::hcore::HString const& sysconfDir_ )
	: _options()
	, _projectName( projectName_ )
	, _syscondDir( sysconfDir_ ) {
	return;
}

int HProgramOptionsHandler::process_rc_file( HString const& section_, RC_CALLBACK_t rc_callback ) {
	M_PROLOG
	program_options_helper::HSetup& setup( program_options_helper::HSetup::get_instance() );
	if ( ! setup.is_locked() ) {
		setup.add_section( section_, program_options_helper::ORCLoader( *this, section_, rc_callback ) );
	}
	struct OPlacement {
		RC_PATHER::PLACEMENT _placement;
		RC_PATHER::PHASE _placementBit;
	} const placementTab[] = {
		{ RC_PATHER::PLACEMENT::ETC_PRJNAME, RC_PATHER::PHASE::GLOBAL },
		{ RC_PATHER::PLACEMENT::ETC_CONF, RC_PATHER::PHASE::GLOBAL },
		{ RC_PATHER::PLACEMENT::ETC, RC_PATHER::PHASE::GLOBAL },
		{ RC_PATHER::PLACEMENT::BY_EXEC, RC_PATHER::PHASE::GLOBAL },
		{ RC_PATHER::PLACEMENT::ENV, RC_PATHER::PHASE::USER },
		{ RC_PATHER::PLACEMENT::HOME_ETC_PRJNAME, RC_PATHER::PHASE::USER },
		{ RC_PATHER::PLACEMENT::HOME_ETC_CONF, RC_PATHER::PHASE::USER },
		{ RC_PATHER::PLACEMENT::HOME_ETC, RC_PATHER::PHASE::USER },
		{ RC_PATHER::PLACEMENT::HOME_PRJNAME, RC_PATHER::PHASE::USER },
		{ RC_PATHER::PLACEMENT::HOME, RC_PATHER::PHASE::USER },
		{ RC_PATHER::PLACEMENT::HOME_CONFIG, RC_PATHER::PHASE::USER },
		{ RC_PATHER::PLACEMENT::LOCAL, RC_PATHER::PHASE::LOCAL }
	};
	RC_PATHER::PHASE successStory( RC_PATHER::PHASE::NONE );
	HFile rc;
	HString option, value, message;
	if ( _options.is_empty() ) {
		M_THROW( _( "bad variable count" ), _options.size() );
	}
	typedef HSet<HString> paths_t;
	paths_t paths;
	bool needLog( true );
	for ( OPlacement const& placement : placementTab ) {
		if ( needLog ) {
			needLog = false;
			log( LOG_LEVEL::INFO ) << __FUNCTION__ << ": ";
		}
		if ( ( successStory == RC_PATHER::PHASE::GLOBAL ) && ( placement._placementBit == RC_PATHER::PHASE::GLOBAL ) ) {
			continue;
		}
		if ( ( successStory == RC_PATHER::PHASE::USER ) && ( placement._placementBit == RC_PATHER::PHASE::USER ) ) {
			continue;
		}
		try {
			HString rcPath( make_path( _syscondDir, _projectName, placement._placement ) );
			if ( rcPath.is_empty() ) {
				continue;
			}
			if ( paths.insert( rcPath ).second && ! rc_open( rcPath, rc ) ) {
				successStory = placement._placementBit;
				int line( 0 );
				bool failed( false );
				bool section( false );
				while ( read_rc_line( option, value, rc, line ) ) {
					if ( ! section_.is_empty() ) {
						if ( value.is_empty() ) {
							value.assign( "[" ).append( section_ ).append( "]" );
							if ( option == value ) {
								if ( _debugLevel_ >= DEBUG_LEVEL::DEBUG_MESSAGES ) {
									HUTF8String utf8( option );
									::fprintf( stderr, "section: [%s]\n", utf8.c_str() );
								}
								log << "section: " << section_ << ", ";
								section = true;
								continue;
							} else {
								section = false;
							}
						}
						if ( ! section ) {
							continue;
						}
					}
					bool optionOK( false );
					for ( HOption& opt : _options ) {
						if ( ! stricasecmp( option, opt.long_form() ) ) {
							optionOK = true;
							set_option( opt, value );
						}
					}
					if ( rc_callback && !rc_callback( option, value ) && ! optionOK ) {
						if ( ! failed ) {
							failed = true;
							needLog = true;
							log << "failed." << endl;
						}
						message = format(
							"Error: unknown option found: `%s', "
							"with value: `%s', on line %d.\n",
							option, value, line
						);
						log( LOG_LEVEL::ERROR ) << message;
						HUTF8String utf8( message );
						::fputs( utf8.c_str(), stderr );
					}
				}
			}
		} catch ( HException const& e ) {
			needLog = true;
			log << "failed." << endl;
			log << e.what() << endl;
			::fputs( e.what(), stderr );
		}
	}
	if ( !! rc ) {
		rc.close();
	}
	set_from_env();
	log << "done." << endl;
	return ( 0 );
	M_EPILOG
}

HProgramOptionsHandler& HProgramOptionsHandler::operator()( HOption option_ ) {
	M_PROLOG
	verify_new_option( option_ );
	_options.emplace_back( option_ );
	return ( *this );
	M_EPILOG
}

void HProgramOptionsHandler::verify_new_option( HOption& option_ ) {
	M_PROLOG
	int sf( option_.short_form() );
	if ( ! sf ) {
		sf = static_cast<int>( _options.size() ) + meta::max_unsigned<char unsigned>::value + 1;
	}
	if ( option_.long_form().is_empty() && ! sf ) {
		throw HProgramOptionsHandlerException( "unnamed option encountered" );
	}
	if ( ! option_.used() ) {
		throw HProgramOptionsHandlerException(
			"unused option: "_ys + ( ! option_.long_form().is_empty() ? option_.long_form() : HString( static_cast<char>( sf ) ) )
		);
	}
	for ( options_t::const_iterator it( _options.begin() ), end( _options.end() ); it != end; ++ it ) {
		/*
		 * This complicated test is to allow for option aliases:
		 * -q, --quiet
		 * -q, --silent
		 */
		if ( option_.value_id() != it->value_id() ) {
			if ( ! option_.long_form().is_empty() && ! it->long_form().is_empty() && ! stricasecmp( it->long_form(), option_.long_form() ) ) {
				throw HProgramOptionsHandlerException( "duplicated long option: "_ys + option_.long_form() );
			}
			if ( it->short_form() == sf ) {
				throw HProgramOptionsHandlerException( "duplicated short option: "_ys + static_cast<char>( sf ) );
			}
		}
	}
	if ( ! option_.short_form() ) {
		option_.short_form( sf );
	}
	if ( ! option_.default_value().is_empty() ) {
		set_option( option_, option_.default_value() );
	}
	return;
	M_EPILOG
}

namespace {

void process_loader( program_options_helper::ORCLoader& loader ) {
	M_PROLOG
	loader._optionHandler.process_rc_file( loader._section, loader.rc_callback );
	return;
	M_EPILOG
}

/* Reads one line from file_, stores beginning of line in option_,
 * stores rest of line in value_, returns 1 if there are more lines
 * to read and 0 in other case. */

void strip_comment( HString& line_ ) {
	M_PROLOG
	bool apostrophe = false, quotation = false;
	int long ctr = 0, length = line_.get_length();
	for ( ctr = 0; ctr < length; ctr ++ ) {
		switch ( line_[ ctr ].get() ) {
			case ( '\'' ):
				apostrophe = ! apostrophe;
			break;
			case ( '"' ):
				quotation = ! quotation;
			break;
			case ( '#' ): {
				if ( ! ( quotation || apostrophe ) ) {
					line_.set_at( ctr, 0_ycp );
					return;
				}
			}
			break;
			default:
			break;
		}
	}
	return;
	M_EPILOG
}

char const _keyValueSep_[] = " \t=";

}

bool read_rc_line( HString& option_, HString& value_, HFile& file_, int& line_ ) {
	M_PROLOG
	int long index = 0, length = 0, end = 0;
	option_.clear();
	value_.clear();
	while ( file_.read_line( option_ ).good() ) {
		line_ ++;
		index = 0;
		if ( option_.is_empty() ) {
			continue; /* empty line */
		}
		/* we are looking for first non-whitespace on the line */
		index = option_.find_other_than( character_class<CHARACTER_CLASS::WHITESPACE>().data() );
		if ( ( index == HString::npos ) || ( option_[ index ] == '#' ) ) {
			continue; /* there is only white spaces or comments on that line */
		}
		/* at this point we know we have _some_ option */
		strip_comment( option_ );
		/* strip comment from end of line */
		length = option_.get_length();
		if ( index ) {
			option_.shift_left( index );
			length -= index;
		}
		/* now we look for first key-value separator after option */
		if ( ( index = option_.find_one_of( _keyValueSep_ ) ) > 0 ) {
			/* we have found a whitespace, so there is probability that */
			/* have a value :-o */
			int long endOfOption = index;
			index = option_.find_other_than( _keyValueSep_, index );
			if ( ( index > 0 ) && option_[ index ].get() ) {
				if ( count( option_.begin() + endOfOption, option_.begin() + index, '='_ycp ) > 1 ) {
					throw HProgramOptionsHandlerException( "Syntax error: redundant `=' sign.", line_ );
				}
				/* we have found a non-whitespace, so there certainly is a value */
				end = ( length - 1 ) - option_.reverse_find_other_than( character_class<CHARACTER_CLASS::WHITESPACE>().data() );
				/* now we strip apostrophe or quotation marks */
				if ( ( ( option_[ end ] == '\'' )
							|| ( option_[ end ] == '"' ) )
						&& ( option_[ end ] == option_[ index ] ) )
					index ++, end --;
				if ( ( end + 1 ) < length )
					option_.set_at( end + 1, 0_ycp );
				value_ = option_.mid( index );
			}
			option_.set_at( endOfOption, 0_ycp );
		}
		return ( true );
	}
	return ( false );
	M_EPILOG
}

void HProgramOptionsHandler::set_option( HOption& option_, HString const& value_ ) {
	M_PROLOG
	HString value( value_ );
	if ( ! option_.verbatim() ) {
		substitute_environment( value, ENV_SUBST_MODE::RECURSIVE );
	}
	if ( _debugLevel_ >= DEBUG_LEVEL::DEBUG_MESSAGES ) {
		HString name;
		if ( ! option_.long_form().is_empty() ) {
			name = option_.long_form();
		} else {
			name = static_cast<char>( option_.short_form() );
		}
		HUTF8String utf8Name( name );
		HUTF8String utf8Value( value );
		::fprintf( stderr, "option: [%s], value [%s]\n", utf8Name.c_str(), utf8Value.c_str() );
	}
	if ( option_.value_id() ) {
		if ( option_.switch_type() == HOption::ARGUMENT::NONE ) {
			TYPE t( option_.recipient_type() );
			M_ENSURE( ( t == TYPE::BOOL ) || ( t == TYPE::UNKNOWN ) );
			option_.set( "true" );
		} else {
			option_.set( value );
		}
	}
	return;
	M_EPILOG
}


int HProgramOptionsHandler::process_command_line( int argc_, char** argv_, int* invalid_ ) {
	M_PROLOG
	system::argv_t argv;
	for ( int i( 0 ); i < argc_; ++ i ) {
		argv.push_back( bytes_to_string( argv_[i] ) );
	}
	argv = process_command_line( yaal::move( argv ), argv_, invalid_ );
	return ( argc_ + 1 - static_cast<int>( argv.get_size() ) );
	M_EPILOG
}

system::argv_t HProgramOptionsHandler::process_command_line( system::argv_t&& argv_, int* unknown_ ) {
	M_PROLOG
	return ( process_command_line( yaal::move( argv_ ), nullptr, unknown_ ) );
	M_EPILOG
}

system::argv_t HProgramOptionsHandler::process_command_line( system::argv_t&& argv_, char** argvRaw_, int* invalid_ ) {
	M_PROLOG
	hcore::log( LOG_LEVEL::INFO ) << "Decoding switches ... ";
	HString optName;
	HString optValue;
	int invalid( 0 );
	int nonOption( 1 ); /* 1 because argv[0] -- a program name is first non-option */
	options_t::iterator it;
	int argc( static_cast<int>( argv_.get_size() ) );
	for ( int i( 1 ); i < argc; ++ i ) {
		HString arg( argv_[i] );
		optValue.clear();
		try {
			if ( ( arg.get_length() >= 2 ) && ( arg[0] == '-' ) && ( arg[1] == '-' ) ) {
				optName.assign( arg, 2 );
				int long optNameEnd( optName.find( '='_ycp ) );
				if ( optNameEnd != HString::npos ) {
					optValue = optName.substr( optNameEnd + 1 );
					optName.erase( optNameEnd );
				}
				HOption* opt( ( it = find_if( _options.begin(), _options.end(), [&optName]( HOption& opt_ ) { return ( opt_.long_form() == optName ); } ) ) != _options.end() ? &*it : nullptr );
				if ( opt ) {
					if ( ( opt->switch_type() == HOption::ARGUMENT::REQUIRED ) && ( optNameEnd == HString::npos ) ) {
						++ i;
						if ( i < argc ) {
							optValue = argv_[i];
						} else {
							++ invalid;
							cerr << argv_[0] << ": option '--" << optName << "' requires an argument" << endl;
							continue;
						}
					}
					set_option( *opt, optValue );
				} else {
					++ invalid;
					cerr << argv_[0] << ": unrecognized option '" << argv_[i] << "'" << endl;
				}
			} else if ( arg[0] == '-' ) {
				arg.shift_left( 1 );
				while ( ! arg.is_empty() ) {
					code_point_t on( arg.front() );
					HOption* opt( ( it = find_if( _options.begin(), _options.end(), [&on]( HOption& opt_ ) { return ( opt_.short_form() == static_cast<int>( on.get() ) ); } ) ) != _options.end() ? &*it : nullptr );
					if ( opt ) {
						/*
						 * According to `man 1 getopt` an optional argument must immediately follow and option,
						 * e.g., if `-s` is an option expecting an optional argument then:
						 * -s/bin/sh
						 * is interpreted as setting `s` option to `/bin/sh`, but
						 * -s /bin/sh
						 * is interpreted as setting `s` to empty value and passing `/bin/sh` as non-option argument.
						 */
						if ( ( opt->switch_type() == HOption::ARGUMENT::REQUIRED ) || ( opt->switch_type() == HOption::ARGUMENT::OPTIONAL ) ) {
							optValue.assign( arg, 1 );
							arg.shift_left( optValue.get_length() );
						}
						if ( ( opt->switch_type() == HOption::ARGUMENT::REQUIRED ) && optValue.is_empty() ) {
							++ i;
							if ( i < argc ) {
								optValue = argv_[i];
							} else {
								arg.shift_left( 1 );
								++ invalid;
								cerr << argv_[0] << ": option requires an argument -- '" << on << "'" << endl;
								continue;
							}
						}
						set_option( *opt, optValue );
					} else {
						++ invalid;
						cerr << argv_[0] << ": invalid option -- '" << on << "'" << endl;
					}
					arg.shift_left( 1 );
				}
			} else {
				rotate( argv_.begin() + i, argv_.begin() + i + 1, argv_.begin() + argc + nonOption - 1 );
				if ( argvRaw_ ) {
					rotate( argvRaw_ + i, argvRaw_ + i + 1, argvRaw_ + argc + nonOption - 1 );
				}
				-- i;
				-- argc;
				++ nonOption;
			}
		} catch ( HException const& e ) {
			++ invalid;
			cerr << argv_[0] << ": malformed data in argument #" << i << ": " << e.what() << endl;
		}
	}
	set_from_env();
	if ( invalid_ ) {
		*invalid_ = invalid;
	}
	argv_.erase( argv_.begin() + 1, argv_.end() - ( nonOption - 1 ) );
	hcore::log << "done." << endl;
	return ( yaal::move( argv_ ) );
	M_EPILOG
}

void HProgramOptionsHandler::set_from_env( void ) {
	M_PROLOG
	HUTF8String utf8;
	for ( options_t::iterator it = _options.begin(), end = _options.end(); it != end; ++ it ) {
		HString lo( _projectName );
		lo.append( "_" ).append( it->long_form() ).upper().replace( "-", "_" );
		utf8 = lo;
		char const* fromEnv( ::getenv( utf8.c_str() ) );
		if ( fromEnv ) {
			set_option( *it, fromEnv );
		}
	}
	return;
	M_EPILOG
}

}

namespace program_options_helper {

int reload_configuration( void ) {
	M_PROLOG
	using namespace hcore::program_options_helper;
	HSetup& setup( HSetup::get_instance() );
	HLocker lock( setup );
	hcore::log << "Reloading configuration." << endl;
	for ( HSetup::iterator it( setup.begin() ), end( setup.end() ); it != end; ++ it ) {
		process_loader( it->second );
	}
	return ( 0 );
	M_EPILOG
}

}

}

