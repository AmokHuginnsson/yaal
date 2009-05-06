/*
---         `yaal' (c) 1978 by Marcin 'Amok' Konarski           ---

	hprogramoptionshandler.cxx - this file is integral part of `yaal' project.

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

#include <cstdlib>  /* getenv() */
#include <cstring>  /* strcpy(), strcat() */
#include <cstdio>   /* fopen() */
#include <getopt.h>
#include <libintl.h> /* gettext() */

#include "base.hxx"
M_VCSID( "$Id: "__ID__" $" )
#include "hprogramoptionshandler.hxx"
#include "xalloc.hxx"
#include "hlog.hxx"
#include "hpattern.hxx"
#include "hcore.hxx"
#include "hstring.hxx"
#include "hchunk.hxx"
#include "xalloc.hxx"
#include "hlog.hxx"

using namespace yaal;
using namespace yaal::hcore;

namespace yaal
{

namespace hcore
{

struct RC_PATHER
	{
	typedef enum
		{
		ETC,
		HOME_ETC,
		HOME
		} placement_t;

	typedef enum
		{
		NONE = 0,
		GLOBAL = 1,
		LOCAL = 2
		} enum_t;
	};

typedef HStrongEnum<RC_PATHER> placement_bit_t;

int read_rc_line( HString&, HString&, HFile&, int& );

HString make_path( HString const& a_oRcName,
		RC_PATHER::placement_t a_ePlacement )
	{
	M_PROLOG
	HString l_oRcPath;
	switch ( a_ePlacement )
		{
		case ( RC_PATHER::ETC ):
			{
			l_oRcPath = "/etc/";
			l_oRcPath += a_oRcName;
			l_oRcPath += "rc";
			}
		break;
		case ( RC_PATHER::HOME_ETC ):
		case ( RC_PATHER::HOME ):
			{
			char * l_pcHomePath = getenv( "HOME" );
			if ( ! l_pcHomePath )
				{
				perror ( "rc_open: getenv()" );
				abort();
				}
			l_oRcPath = l_pcHomePath;
			if ( a_ePlacement == RC_PATHER::HOME_ETC )
				l_oRcPath += "/etc/conf/";
			else
				l_oRcPath += "/.";
			l_oRcPath += a_oRcName;
			l_oRcPath += "rc";
			}
		break;
		default :
		break;
		}
	return ( l_oRcPath );
	M_EPILOG
	}

int rc_open( HString const& a_oRcName,
		RC_PATHER::placement_t const a_ePlacament,
		HFile& a_roFile )
	{
	M_PROLOG
	int l_iError = 0;
	HString l_oRcPath = make_path( a_oRcName, a_ePlacament );
	if ( !! a_roFile )
		a_roFile.close();
	l_iError = a_roFile.open( l_oRcPath );
	if ( l_iError )
		l_oRcPath +=	" not found, ";
	else
		{
		l_oRcPath = "config read from: " + l_oRcPath;
		l_oRcPath += ", ";
		}
	log << l_oRcPath;
	return ( l_iError );
	M_EPILOG
	}

bool substitute_environment( HString& a_roString )
	{
	M_PROLOG
	bool envVarRefFound = false;
	if ( ! a_roString.is_empty() )
		{
		HPattern l_oPattern;
		M_ENSURE( l_oPattern.parse_re( "${[^{}]\\{1,\\}}" ) == 0 );
		HPattern::HMatchIterator it = l_oPattern.find( a_roString.raw() );
		if ( it != l_oPattern.end() )
			{
			HString l_oVar = a_roString.mid( it->raw() - a_roString.raw(), it->size() );
			HString l_oName = l_oVar.mid( 2, it->size() - 3 );
			char const* l_pcStart = ::getenv( l_oName.raw() );
			a_roString.replace( l_oVar, l_pcStart ? l_pcStart : "" );
			envVarRefFound = true;
			}
		}
	return ( envVarRefFound );
	M_EPILOG
	}

namespace
{

struct ORCLoader
	{
	HProgramOptionsHandler* f_poOptionHandler;
	HString f_oPath;
	HString f_oSection;
	HProgramOptionsHandler::RC_CALLBACK_t rc_callback;
 	ORCLoader( void ) :
		f_poOptionHandler( NULL ), f_oPath(), f_oSection(), rc_callback( NULL ) { }
 	ORCLoader( HProgramOptionsHandler* a_poOptionHandler, HString const& a_oRcName,
		HString const& a_oSection, HProgramOptionsHandler::RC_CALLBACK_t callback )
		: f_poOptionHandler( a_poOptionHandler ),
		f_oPath( a_oRcName ), f_oSection( a_oSection ),
		rc_callback( callback ) { }
 	ORCLoader( ORCLoader const& loader )
		: f_poOptionHandler( loader.f_poOptionHandler ),
		f_oPath( loader.f_oPath ), f_oSection( loader.f_oSection ),
		rc_callback( loader.rc_callback ) {}
	ORCLoader& operator = ( ORCLoader const& loader )
		{
		if ( &loader != this )
			{
			f_poOptionHandler = loader.f_poOptionHandler;
			f_oPath = loader.f_oPath;
			f_oSection = loader.f_oSection;
			rc_callback = loader.rc_callback;
			}
		return ( *this );
		}
	};

typedef HList<ORCLoader> rc_loaders_t;
rc_loaders_t n_oRCLoades;
bool n_bRCLoadersLocked = false;

}

class HLocker
	{
	bool& f_rbLock;
public:
	HLocker( bool& a_rbLock ) : f_rbLock( a_rbLock )
		{ f_rbLock = true; }
	~HLocker( void )
		{ f_rbLock = false; }
	};

HProgramOptionsHandler::OOption::OOption( void )
	: f_pcName( NULL ), f_oValue(),
	f_pcShortForm( NULL ), f_eSwitchType( TYPE::NONE ),
	f_pcArgument( NULL ), f_pcDescription( NULL ),
	CALLBACK() {}

HProgramOptionsHandler::OOption::OOption(
		char const* a_pcName,
		HOptionValueInterface::ptr_t a_oValue,
		char const* a_pcShortForm,
		TYPE::enum_t a_eSwitchType,
		char const* a_pcArgument,
		char const* a_pcDescription,
		HProgramOptionsHandler::simple_callback_t const& a_CALLBACK )
	: f_pcName( a_pcName ), f_oValue( a_oValue ),
	f_pcShortForm( a_pcShortForm ), f_eSwitchType( a_eSwitchType ),
	f_pcArgument( a_pcArgument ), f_pcDescription( a_pcDescription ),
	CALLBACK( a_CALLBACK ) {}

HProgramOptionsHandler::OOption::OOption( HProgramOptionsHandler::OOption const& o )
	: f_pcName( o.f_pcName ), f_oValue( o.f_oValue ),
	f_pcShortForm( o.f_pcShortForm ), f_eSwitchType( o.f_eSwitchType ),
	f_pcArgument( o.f_pcArgument ), f_pcDescription( o.f_pcDescription ),
	CALLBACK( o.CALLBACK ) {}

HProgramOptionsHandler::OOption& HProgramOptionsHandler::OOption::operator = ( HProgramOptionsHandler::OOption const& o )
	{
	M_PROLOG
	if ( &o != this )
		{
		OOption tmp( o );
		swap( tmp );
		}
	return ( *this );
	M_EPILOG
	}

void HProgramOptionsHandler::OOption::swap( HProgramOptionsHandler::OOption& o )
	{
	M_PROLOG
	if ( &o != this )
		{
		using yaal::swap;
		swap( f_pcName, o.f_pcName );
		swap( f_oValue, o.f_oValue );
		swap( f_pcShortForm, o.f_pcShortForm );
		swap( f_eSwitchType, o.f_eSwitchType );
		swap( f_pcArgument, o.f_pcArgument );
		swap( f_pcDescription, o.f_pcDescription );
		swap( CALLBACK, o.CALLBACK );
		}
	return;
	M_EPILOG
	}

void HProgramOptionsHandler::HOptionValueInterface::set( HString const& val )
	{
	M_PROLOG
	do_set( val );
	return;
	M_EPILOG
	}

type_t HProgramOptionsHandler::HOptionValueInterface::get_type( void ) const
	{
	M_PROLOG
	return ( do_get_type() );
	M_EPILOG
	}

int HProgramOptionsHandler::process_rc_file( HString const& a_oRcName,
		HString const& a_oSection, RC_CALLBACK_t rc_callback )
	{
	M_PROLOG
	if ( ! n_bRCLoadersLocked )
		n_oRCLoades.push_back( ORCLoader( this, a_oRcName, a_oSection, rc_callback ) );
	struct OPlacement
		{
		RC_PATHER::placement_t f_ePlacement;
		placement_bit_t f_ePlacementBit;
		} l_psPlacementTab [ ] = {
				{ RC_PATHER::ETC, RC_PATHER::GLOBAL },
				{ RC_PATHER::HOME_ETC, RC_PATHER::LOCAL },
				{ RC_PATHER::HOME, RC_PATHER::LOCAL } };
	bool l_bSection = false, l_bOptionOK;
	placement_bit_t l_eSuccessStory( RC_PATHER::NONE );
	size_t l_iCtrOut = 0;
	HFile l_oRc;
	HString l_oOption, l_oValue, l_oMessage;
	log ( LOG_TYPE::INFO ) << "process_rc_file(): ";
	if ( f_oOptions.is_empty() )
		M_THROW( _( "bad variable count" ), f_oOptions.size() );
	for ( l_iCtrOut = 0; l_iCtrOut < ( sizeof ( l_psPlacementTab ) / sizeof ( OPlacement ) ); l_iCtrOut ++ )
		{
		if ( ( !!( l_eSuccessStory & RC_PATHER::GLOBAL ) )
				&& ( l_psPlacementTab[ l_iCtrOut ].f_ePlacementBit == RC_PATHER::GLOBAL ) )
			continue;
		if ( !! ( l_eSuccessStory & RC_PATHER::LOCAL ) )
			break;
		if ( ! rc_open( a_oRcName, l_psPlacementTab [ l_iCtrOut ].f_ePlacement, l_oRc ) )
			{
			l_eSuccessStory |= l_psPlacementTab [ l_iCtrOut ].f_ePlacementBit;
			int l_iLine = 0;
			while ( read_rc_line( l_oOption, l_oValue, l_oRc, l_iLine ) )
				{
				if ( ! a_oSection.is_empty() )
					{
					if ( l_oValue.is_empty() )
						{
						l_oValue.format( "[%s]", a_oSection.raw() );
						if ( l_oOption == l_oValue )
							{
							if ( n_iDebugLevel )
								::fprintf( stderr, "section: [%s]\n", l_oOption.raw() );
							log << "section: " << a_oSection << ", ";
							l_bSection = true;
							continue;
							}
						else
							l_bSection = false;
						}
					if ( ! l_bSection )
						continue;
					}
				while ( substitute_environment( l_oValue ) )
					;
				if ( n_iDebugLevel )
					::fprintf( stderr, "option: [%s], value [%s]\n",
							l_oOption.raw(), l_oValue.raw() );
				l_bOptionOK = false;
				for ( options_t::iterator it = f_oOptions.begin(), end = f_oOptions.end(); it != end; ++ it )
					{
					if ( ! strcasecmp( l_oOption, it->f_pcName ) )
						l_bOptionOK = true, set_option( *it, l_oValue );
					}
				if ( rc_callback && rc_callback( l_oOption, l_oValue )
						&& ! l_bOptionOK )
					{
					log << "failed." << endl;
					l_oMessage.format ( "Error: unknown option found: `%s', "
								"with value: `%s', on line %d.\n",
								l_oOption.raw(), l_oValue.raw(), l_iLine );
					log ( LOG_TYPE::ERROR ) << l_oMessage;
					::fputs( l_oMessage.raw(), stderr );
					}
				}
			}
		}
	if ( !! l_oRc )
		l_oRc.close();
	log << "done." << endl;
	return ( 0 );
	M_EPILOG
	}

HProgramOptionsHandler& HProgramOptionsHandler::operator()(
		char const* name, HOptionValueInterface::ptr_t value,
		char const* shortForm, OOption::TYPE::enum_t const& type,
		char const* arg, char const* desc, simple_callback_t const& callback )
	{
	M_PROLOG
	OOption o( name, value, shortForm, type, arg, desc, callback );
	f_oOptions.push_back( o );
	return ( *this );
	M_EPILOG
	}

void process_loader( ORCLoader& loader )
	{
	M_PROLOG
	loader.f_poOptionHandler->process_rc_file( loader.f_oPath, loader.f_oSection, loader.rc_callback );
	return;
	M_EPILOG
	}

namespace program_options_helper
{

int reload_configuration( void )
	{
	M_PROLOG
	HLocker lock( n_bRCLoadersLocked );
	log << "Reloading configuration." << endl;
	for_each( n_oRCLoades.begin(), n_oRCLoades.end(), cref( process_loader ) );
	return ( 0 );
	M_EPILOG
	}

}

/* Reads one line from a_psFile, stores beginning of line in a_roOption, 
 * stores rest of line in a_pcValue, returns 1 if there are more lines
 * to read and 0 in other case. */

void strip_comment( HString& a_roLine )
	{
	M_PROLOG
	bool l_bApostrophe = false, l_bQuotation = false;
	int long l_iCtr = 0, l_iLenght = a_roLine.get_length();
	for ( l_iCtr = 0; l_iCtr < l_iLenght; l_iCtr ++ )
		{
		switch ( a_roLine[ l_iCtr ] )
			{
			case ( '\'' ):
				l_bApostrophe = ! l_bApostrophe;
			break;
			case ( '"' ):
				l_bQuotation = ! l_bQuotation;
			break;
			case ( '#' ):
				{
				if ( ! ( l_bQuotation || l_bApostrophe ) )
					{
					a_roLine.set_at( l_iCtr, 0 );
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

int read_rc_line( HString& a_roOption, HString& a_roValue, HFile& a_roFile,
		int& a_riLine )
	{
	M_PROLOG
	int long l_iIndex = 0, l_iLenght = 0, l_iEnd = 0;
	a_roOption = a_roValue = "";
	while ( a_roFile.read_line( a_roOption, HFile::READ::STRIP_NEWLINES ) >= 0 )
		{
		a_riLine ++;
		l_iIndex = 0;
		if ( ! a_roOption[ l_iIndex ] )
			continue; /* empty line */
		/* we are looking for first non-whitespace on the line */
		l_iIndex = a_roOption.find_other_than( n_pcWhiteSpace );
		if ( ! a_roOption[ l_iIndex ] || ( a_roOption[ l_iIndex ] == '#' ) )
			continue; /* there is only white spaces or comments on that line */
		/* at this point we know we have _some_ option */
		strip_comment( a_roOption );
		/* strip comment from end of line */
		l_iLenght = a_roOption.get_length();
		if ( l_iIndex )
			{
			a_roOption.shift_left( l_iIndex );
			l_iLenght -= l_iIndex;
			}
		/* now we look for first whitespace after option */
		if ( ( l_iIndex = a_roOption.find_one_of( n_pcWhiteSpace ) ) > 0 )
			{
			/* we have found a whitespace, so there is probability that */
			/* have a value :-o */
			int long l_iEndOfOption = l_iIndex;
			l_iIndex = a_roOption.find_other_than( n_pcWhiteSpace, l_iIndex );
			if ( ( l_iIndex > 0 ) && a_roOption[ l_iIndex ] )
				{
				/* we have found a non-whitespace, so there certainly is a value */
				l_iEnd = ( l_iLenght - 1 ) - a_roOption.reverse_find_other_than( n_pcWhiteSpace );
				/* now we strip apostrophe or quotation marks */
				if ( ( ( a_roOption[ l_iEnd ] == '\'' )
							|| ( a_roOption[ l_iEnd ] == '"' ) )
						&& ( a_roOption[ l_iEnd ] == a_roOption[ l_iIndex ] ) )
					l_iIndex ++, l_iEnd --;
				if ( ( l_iEnd + 1 ) < l_iLenght )
					a_roOption.set_at( l_iEnd + 1, 0 );
				a_roValue = a_roOption.mid( l_iIndex );
				}
			a_roOption.set_at( l_iEndOfOption, 0 );
			}
		return ( 1 );
		}
	return ( 0 );
	M_EPILOG
	}
	
void rc_set_variable( char const* const a_pcValue, bool& a_rbVariable )
	{
	M_PROLOG
	a_rbVariable = lexical_cast<bool>( a_pcValue );
	M_EPILOG
	return;
	}

void rc_set_variable( char const * const a_pcValue, char** a_ppcVariable )
	{
	if ( *a_ppcVariable )
		xfree( *a_ppcVariable );
	*a_ppcVariable = NULL;
	*a_ppcVariable = xstrdup( a_pcValue );
	return;
	}

void rc_set_variable( char const* const a_pcValue, int& a_riVariable )
	{
	a_riVariable = lexical_cast<int>( a_pcValue );
	}

void rc_set_variable( char const* const a_pcValue, double long& a_rdVariable )
	{
	a_rdVariable = lexical_cast<double long>( a_pcValue );
	}

void rc_set_variable( char const* const a_pcValue, double& a_rdVariable )
	{
	a_rdVariable = lexical_cast<double>( a_pcValue );
	}

void rc_set_variable( char const* const a_pcValue, char& a_rcVariable )
	{
	a_rcVariable = a_pcValue[ 0 ];
	}

void HProgramOptionsHandler::set_option( OOption& a_sOption, HString const& a_oValue )
	{
	M_PROLOG
	if ( !! a_sOption.f_oValue )
		{
		if ( a_sOption.f_eSwitchType == OOption::TYPE::NONE )
			{
			M_ENSURE( a_sOption.f_oValue->get_type() == TYPE::BOOL );
			a_sOption.f_oValue->set( "true" );
			}
		else
			a_sOption.f_oValue->set( a_oValue );
		}
	if ( a_sOption.CALLBACK.first )
		a_sOption.CALLBACK.first( a_sOption.CALLBACK.second );
	return;
	M_EPILOG
	}

char const* make_short_opts( HProgramOptionsHandler::options_t const& a_oOptions, HString& a_roBuffer )
	{
	M_PROLOG
	a_roBuffer = "";
	for ( HProgramOptionsHandler::options_t::const_iterator it = a_oOptions.begin(),
			end = a_oOptions.end(); it != end; ++ it )
		{
		if ( ! it->f_pcShortForm )
			continue;
		a_roBuffer += static_cast<char>( it->f_pcShortForm[0] );
		switch ( it->f_eSwitchType )
			{
			case ( HProgramOptionsHandler::OOption::TYPE::REQUIRED ):
				a_roBuffer += ':';
			break;
			case ( HProgramOptionsHandler::OOption::TYPE::OPTIONAL ):
				a_roBuffer += "::";
			break;
			case ( HProgramOptionsHandler::OOption::TYPE::NONE ):
			default :
				break;
			}
		}
	return ( a_roBuffer.raw() );
	M_EPILOG
	}

option* make_option_array( HProgramOptionsHandler::options_t const& a_oOptions, HChunk& a_roBuffer )
	{
	M_PROLOG
	option* l_psOptions = NULL;
	l_psOptions = a_roBuffer.get<option>();
	int l_iCtr = 0;
	for ( HProgramOptionsHandler::options_t::const_iterator it = a_oOptions.begin(),
			end = a_oOptions.end(); it != end; ++ it, ++ l_iCtr )
		{
		memset( &l_psOptions[ l_iCtr ], 0, sizeof ( option ) );
		/* Solaris version of `struct option' is braindead broken.
		 * Another proof that Solaris sucks big time.
		 */
		l_psOptions[ l_iCtr ].name = const_cast<char*>( it->f_pcName );
		switch ( it->f_eSwitchType )
			{
			case ( HProgramOptionsHandler::OOption::TYPE::REQUIRED ):
				l_psOptions [ l_iCtr ].has_arg = required_argument;
			break;
			case ( HProgramOptionsHandler::OOption::TYPE::OPTIONAL ):
				l_psOptions [ l_iCtr ].has_arg = optional_argument;
			break;
			case ( HProgramOptionsHandler::OOption::TYPE::NONE ):
			default :
				l_psOptions [ l_iCtr ].has_arg = no_argument;
			}
		if ( it->f_pcShortForm )
			l_psOptions[ l_iCtr ].val = it->f_pcShortForm[0];
		}
	return ( l_psOptions );
	M_EPILOG
	}

int HProgramOptionsHandler::process_command_line( int const& a_iArgc,
		char* const* const a_ppcArgv,
		int* const a_piUnknown )
	{
	M_PROLOG
	bool l_bValidSwitch = false;
	int l_iChar = 0;
	char const* l_pcShortOpts = NULL;
	option* l_psOptionArray = NULL;
	HString l_oShortOptBuffer;
	HChunk l_oLongOptBuffer( xcalloc<option>( f_oOptions.size() + 1 ) ); /* + 1 for array terminator */
	hcore::log << "Decoding switches ... ";
	l_pcShortOpts = make_short_opts( f_oOptions, l_oShortOptBuffer );
	l_psOptionArray = make_option_array( f_oOptions, l_oLongOptBuffer );
	while ( ( l_iChar = ::getopt_long( a_iArgc, a_ppcArgv, l_pcShortOpts,
					l_psOptionArray, NULL ) ) != EOF )
		{
		l_bValidSwitch = false;
		for ( options_t::iterator it = f_oOptions.begin(), end = f_oOptions.end(); it != end; ++ it )
			{
			if ( it->f_pcShortForm && ( it->f_pcShortForm[0] == l_iChar ) )
				l_bValidSwitch = true, set_option( *it, optarg );
			}
		if ( ! l_bValidSwitch && a_piUnknown )
			( *a_piUnknown ) ++;
		}
	hcore::log << "done." << endl;
	return ( optind );
	M_EPILOG
	}

}

}

