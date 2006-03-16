/*
---           `stdhapi' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	cl_switch.c - this file is integral part of `stdhapi' project.

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

#include <cstdlib>
#include <getopt.h>

#include "hexception.h"
M_CVSID ( "$CVSHeader$" )
#include "cl_switch.h"
#include "rc_file.h"
#include "hstring.h"
#include "hlog.h"

using namespace stdhapi;
using namespace stdhapi::hcore;
using namespace stdhapi::hcore::cl_switch;
using namespace stdhapi::hcore::rc_file;

namespace stdhapi
{

namespace hcore
{

namespace cl_switch
{

char const * const make_short_opts ( OOption * const & a_rpsOptions, int a_iCount, HString & a_roBuffer )
	{
	int l_iCtr = 0;
	a_roBuffer = "";
	for ( l_iCtr = 0; l_iCtr < a_iCount; l_iCtr ++ )
		{
		a_roBuffer += static_cast < char > ( a_rpsOptions [ l_iCtr ].f_cShortOption );
		switch ( a_rpsOptions [ l_iCtr ].f_eSwitchType )
			{
			case ( OOption::D_REQUIRED ):
				a_roBuffer += ':';
			break;
			case ( OOption::D_OPTIONAL ):
				a_roBuffer += "::";
			break;
			case ( OOption::D_NONE ):
			default :
				break;
			}
		}
	return ( a_roBuffer.raw ( ) );
	}

option * make_option_array ( OOption * const & a_rpsOptions, int a_iCount, HString & a_roBuffer )
	{
	M_PROLOG
	int l_iCtr = 0;
	option * l_psOptions = NULL;
	a_roBuffer.hs_realloc ( sizeof ( option ) * a_iCount );
	l_psOptions = reinterpret_cast < option * > ( a_roBuffer.raw ( ) );
	for ( l_iCtr = 0; l_iCtr < a_iCount; l_iCtr ++ )
		{
		memset ( & l_psOptions [ l_iCtr ], 0, sizeof ( option ) );
		l_psOptions [ l_iCtr ].name = a_rpsOptions [ l_iCtr ].f_pcLongOption;
		switch ( a_rpsOptions [ l_iCtr ].f_eSwitchType )
			{
			case ( OOption::D_REQUIRED ):
				l_psOptions [ l_iCtr ].has_arg = required_argument;
			break;
			case ( OOption::D_OPTIONAL ):
				l_psOptions [ l_iCtr ].has_arg = optional_argument;
			break;
			case ( OOption::D_NONE ):
			default :
				l_psOptions [ l_iCtr ].has_arg = no_argument;
			}
		l_psOptions [ l_iCtr ].val = a_rpsOptions [ l_iCtr ].f_cShortOption;
		}
	return ( l_psOptions );
	M_EPILOG
	}

int decode_switches ( int const a_iArgc, char * const * const a_ppcArgv,
		OOption * const & a_rpsOptions, int const a_iCount,
		int * const a_piUnknown )
	{
	M_PROLOG
	bool l_bValidSwitch = false;
	int l_iChar = 0, l_iCtr = 0;
	char const * l_pcShortOpts = NULL;
	option * l_psOptionArray = NULL;
	HString l_oLongOptBuffer, l_oShortOptBuffer;
	hcore::log << "Decoding switches ... ";
	l_pcShortOpts = make_short_opts ( a_rpsOptions, a_iCount, l_oShortOptBuffer );
	l_psOptionArray = make_option_array ( a_rpsOptions, a_iCount,
			l_oLongOptBuffer );
	while ( ( l_iChar = getopt_long ( a_iArgc, a_ppcArgv, l_pcShortOpts,
					l_psOptionArray, NULL ) ) != EOF )
		{
		l_bValidSwitch = false;
		for ( l_iCtr = 0; l_iCtr < a_iCount; l_iCtr ++ )
			{
			if ( a_rpsOptions [ l_iCtr ].f_cShortOption == l_iChar )
				{
				l_bValidSwitch = true;
				switch ( a_rpsOptions [ l_iCtr ].f_eValueType )
					{
					case ( D_BOOL ):
						rc_set_variable ( "true",
								* static_cast < bool * > ( a_rpsOptions [ l_iCtr ].f_pvValue ) );
					break;
					case ( D_CHAR ):
						if ( optarg )
							rc_set_variable ( optarg,
									* static_cast < char * > ( a_rpsOptions [ l_iCtr ].f_pvValue ) );
					break;
					case ( D_INT ):
						rc_set_variable ( optarg,
								* static_cast < int * > ( a_rpsOptions [ l_iCtr ].f_pvValue ) );
					break;
					case ( D_CHAR_POINTER ):
						rc_set_variable ( optarg,
								static_cast < char * * > ( a_rpsOptions [ l_iCtr ].f_pvValue ) );
					break;
					case ( D_HSTRING ):
						( * static_cast < HString * > ( a_rpsOptions [ l_iCtr ].f_pvValue ) ) = optarg;
					break;
					case ( D_VOID ):
						break;
					default:
						M_THROW ( "unknown type", a_rpsOptions [ l_iCtr ].f_eValueType );
					}
				if ( a_rpsOptions [ l_iCtr ].CALLBACK )
					a_rpsOptions [ l_iCtr ].CALLBACK ( );
				}
			}
		if ( ! l_bValidSwitch && a_piUnknown )
			( * a_piUnknown ) ++;
		}
	hcore::log << "done." << endl;
	return ( optind );
	M_EPILOG
	}

}

}

}

