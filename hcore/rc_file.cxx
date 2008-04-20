/*
---         `yaal' (c) 1978 by Marcin 'Amok' Konarski           ---

	rc_file.cxx - this file is integral part of `yaal' project.

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

#include <cstdlib>  /* getenv() */
#include <cstring>  /* strcpy(), strcat() */
#include <cstdio>   /* fopen() */
#include <libintl.h> /* gettext() */

#include "hexception.h"
M_VCSID ( "$Id$" )
#include "rc_file.h"
#include "xalloc.h"
#include "hlog.h"
#include "hpattern.h"
#include "hcore.h"

namespace yaal
{

namespace hcore
{

namespace rc_file
{

struct RC_PATHER
	{
	typedef enum
		{
		D_ETC,
		D_HOME_ETC,
		D_HOME
		} placement_t;

	typedef enum
		{
		D_NONE = 0,
		D_GLOBAL = 1,
		D_LOCAL = 2
		} placement_bit_t;
	};

int read_rc_line( HString&, HString&, HFile&, int& );

HString make_path ( char const * const a_pcRcName,
		RC_PATHER::placement_t a_ePlacement )
	{
	HString l_oRcPath;
	switch ( a_ePlacement )
		{
		case ( RC_PATHER::D_ETC ):
			{
			l_oRcPath = "/etc/";
			l_oRcPath += a_pcRcName;
			l_oRcPath += "rc";
			}
		break;
		case ( RC_PATHER::D_HOME_ETC ):
		case ( RC_PATHER::D_HOME ):
			{
			char * l_pcHomePath = getenv( "HOME" );
			if ( ! l_pcHomePath )
				{
				perror ( "rc_open: getenv()" );
				abort();
				}
			l_oRcPath = l_pcHomePath;
			if ( a_ePlacement == RC_PATHER::D_HOME_ETC )
				l_oRcPath += "/etc/conf/";
			else
				l_oRcPath += "/.";
			l_oRcPath += a_pcRcName;
			l_oRcPath += "rc";
			}
		break;
		default :
		break;
		}
	return ( l_oRcPath );
	}

int rc_open( char const* const a_pcRcName,
		RC_PATHER::placement_t const a_ePlacament,
		HFile& a_roFile )
	{
	M_PROLOG
	int l_iError = 0;
	HString l_oRcPath = make_path ( a_pcRcName, a_ePlacament );
	if ( a_roFile )
		a_roFile.close();
	l_iError = a_roFile.open ( l_oRcPath );
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
	int l_iLength = 0;
	char const* l_pcStart = NULL;
	HString l_oName;
	HPattern l_oPattern;
	bool envVarRefFound = false;
	if ( a_roString )
		{
		M_ENSURE( l_oPattern.parse_re( "${[^{}]\\+}" ) == 0 );
		if ( ( l_pcStart = l_oPattern.matches( a_roString, &l_iLength ) ) )
			{
			l_oName = a_roString.mid( l_pcStart - static_cast<char const* const>( a_roString ), l_iLength );
			l_pcStart = ::getenv( l_oName.mid( 2, l_iLength - 3 ) );
			a_roString.replace( l_oName, l_pcStart ? l_pcStart : "" );
			envVarRefFound = true;
			}
		}
	return ( envVarRefFound );
	}

namespace
{

struct ORCLoader
	{
	HString f_oPath;
	HString f_oSection;
	OOption const* f_psVaraibles;
	int f_iCount;
	RC_CALLBACK_t rc_callback;
 	ORCLoader( void ) :
		f_oPath(), f_oSection(), f_psVaraibles( NULL ),
		f_iCount( 0 ), rc_callback( NULL ) { }
 	ORCLoader( char const* const a_pcRcName,
		char const* const a_pcSection,
		OOption const* const a_psOptions, int const a_iCount,
		RC_CALLBACK_t callback ) :
		f_oPath( a_pcRcName ), f_oSection( a_pcSection ), f_psVaraibles( a_psOptions ),
		f_iCount( a_iCount ), rc_callback( callback ) { }
 	ORCLoader( ORCLoader const& loader ) :
		f_oPath(), f_oSection(), f_psVaraibles( NULL ),
		f_iCount( 0 ), rc_callback( NULL ) { operator = ( loader ); }
	ORCLoader& operator = ( ORCLoader const& loader )
		{
		if ( &loader != this )
			{
			f_oPath = loader.f_oPath;
			f_oSection = loader.f_oSection;
			f_psVaraibles = loader.f_psVaraibles;
			f_iCount = loader.f_iCount;
			rc_callback = loader.rc_callback;
			}
		return ( *this );
		}
	};

typedef HList<ORCLoader> rc_loaders_t;
rc_loaders_t n_oRCLoades;
bool n_bRCLoadersLocked = false;

class HLocker
	{
	bool& f_rbLock;
public:
	HLocker( bool& a_rbLock ) : f_rbLock( a_rbLock )
		{ f_rbLock = true; }
	~HLocker( void )
		{ f_rbLock = false; }
	};

}

int process_rc_file_internal( char const* const a_pcRcName,
		char const* const a_pcSection,
		OOption const* const a_psOptions, int const a_iCount,
		RC_CALLBACK_t rc_callback )
	{
	M_PROLOG
	if ( ! n_bRCLoadersLocked )
		n_oRCLoades.push_back( ORCLoader( a_pcRcName, a_pcSection, a_psOptions, a_iCount, rc_callback ) );
	struct OPlacement
		{
		RC_PATHER::placement_t f_ePlacement;
		RC_PATHER::placement_bit_t f_ePlacementBit;
		} l_psPlacementTab [ ] = {
				{ RC_PATHER::D_ETC, RC_PATHER::D_GLOBAL },
				{ RC_PATHER::D_HOME_ETC, RC_PATHER::D_LOCAL },
				{ RC_PATHER::D_HOME, RC_PATHER::D_LOCAL } };
	bool l_bSection = false, l_bOptionOK;
	int l_iCtr = 0, l_iLine = 0;
	RC_PATHER::placement_bit_t l_eSuccessStory = RC_PATHER::D_NONE;
	size_t l_iCtrOut = 0;
	HFile l_oRc;
	HString l_oOption, l_oValue, l_oMessage;
	log ( LOG_TYPE::D_INFO ) << "process_rc_file(): ";
	if ( a_iCount < 0 )
		M_THROW ( _ ( "bad variable count" ), a_iCount );
	for ( l_iCtrOut = 0; l_iCtrOut < ( sizeof ( l_psPlacementTab ) / sizeof ( OPlacement ) ); l_iCtrOut ++ )
		{
		if ( ( l_eSuccessStory & RC_PATHER::D_GLOBAL )
				&& ( l_psPlacementTab [ l_iCtrOut ].f_ePlacementBit == RC_PATHER::D_GLOBAL ) )
			continue;
		if ( l_eSuccessStory & RC_PATHER::D_LOCAL )
			break;
		if ( ! rc_open ( a_pcRcName, l_psPlacementTab [ l_iCtrOut ].f_ePlacement, l_oRc ) )
			{
			l_eSuccessStory |= l_psPlacementTab [ l_iCtrOut ].f_ePlacementBit;
			while ( read_rc_line ( l_oOption, l_oValue, l_oRc, l_iLine ) )
				{
				if ( a_pcSection )
					{
					if ( l_oValue.is_empty() )
						{
						l_oValue.format ( "[%s]", a_pcSection );
						if ( l_oOption == l_oValue )
							{
							if ( n_iDebugLevel )
								::fprintf( stderr, "section: [%s]\n",
										static_cast<char const * const>( l_oOption ) );
							log << "section: " << a_pcSection << ", ";
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
							static_cast<char const * const>( l_oOption ),
							static_cast<char const * const>( l_oValue ) );
				l_iCtr = 0;
				l_bOptionOK = false;
				while ( ( l_iCtr < a_iCount ) && a_psOptions[ l_iCtr ].f_pcName )
					{
					if ( ! ::strcasecmp( l_oOption, a_psOptions[ l_iCtr ].f_pcName ) )
						l_bOptionOK = true, set_option( a_psOptions[ l_iCtr ], l_oValue );
					l_iCtr ++;
					}
				if ( rc_callback && rc_callback ( l_oOption, l_oValue )
						&& ! l_bOptionOK )
					{
					log << "failed." << endl;
					l_oMessage.format ( "Error: unknown option found: `%s', "
								"with value: `%s', on line %d.\n",
								static_cast<char const *>( l_oOption ),
								static_cast<char const *>( l_oValue ), l_iLine );
					log ( LOG_TYPE::D_ERROR ) << l_oMessage;
					fprintf ( stderr, l_oMessage );
					}
				}
			}
		}
	if ( l_oRc )
		l_oRc.close();
	log << "done." << endl;
	return ( 0 );
	M_EPILOG
	}

void process_loader( ORCLoader& loader )
	{
	M_PROLOG
	process_rc_file_internal( loader.f_oPath, loader.f_oSection,
			loader.f_psVaraibles, loader.f_iCount, loader.rc_callback );
	return;
	M_EPILOG
	}

int reload_configuration( void )
	{
	M_PROLOG
	HLocker lock( n_bRCLoadersLocked );
	log << "Reloading configuration." << endl;
	for_each( n_oRCLoades.begin(), n_oRCLoades.end(), process_loader );
	return ( 0 );
	M_EPILOG
	}

/* Reads one line from a_psFile, stores beginning of line in a_roOption, 
 * stores rest of line in a_pcValue, returns 1 if there are more lines
 * to read and 0 in other case. */

void strip_comment( HString& a_roLine )
	{
	M_PROLOG
	bool l_bApostrophe = false, l_bQuotation = false;
	int l_iCtr = 0, l_iLenght = a_roLine.get_length();
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
	int l_iIndex = 0, l_iLenght = 0, l_iEnd = 0;
	a_roOption = a_roValue = "";
	while ( a_roFile.read_line( a_roOption, HFile::D_STRIP_NEWLINES ) >= 0 )
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
			int l_iEndOfOption = l_iIndex;
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
	a_rbVariable = to_bool( a_pcValue );
	M_EPILOG
	return;
	}

void rc_set_variable( char const * const a_pcValue, char ** a_ppcVariable )
	{
	if ( * a_ppcVariable )
		xfree ( * a_ppcVariable );
	*a_ppcVariable = NULL;
	*a_ppcVariable = xstrdup ( a_pcValue );
	return;
	}

void rc_set_variable( char const* const a_pcValue, int & a_riVariable )
	{
	a_riVariable = to_int( a_pcValue );
	}

void rc_set_variable( char const* const a_pcValue, char & a_rcVariable )
	{
	a_rcVariable = a_pcValue[ 0 ];
	}

}

}

}

