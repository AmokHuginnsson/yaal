/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	cl_switch.cxx - this file is integral part of `yaal' project.

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

#include <cstdlib>
#include <getopt.h>

#include "base.hxx"
M_VCSID( "$Id: "__ID__" $" )
#include "cl_switch.hxx"
#include "rc_file.hxx"
#include "hstring.hxx"
#include "hchunk.hxx"
#include "xalloc.hxx"
#include "hlog.hxx"

using namespace yaal;
using namespace yaal::hcore;
using namespace yaal::hcore::cl_switch;
using namespace yaal::hcore::rc_file;

namespace yaal
{

namespace hcore
{

namespace cl_switch
{

char const* make_short_opts( OOption* const& a_rpsOptions, int a_iCount, HString& a_roBuffer )
	{
	M_PROLOG
	int l_iCtr = 0;
	a_roBuffer = "";
	for ( l_iCtr = 0; l_iCtr < a_iCount; l_iCtr ++ )
		{
		if ( ! a_rpsOptions[ l_iCtr ].f_pcShortForm )
			continue;
		a_roBuffer += static_cast<char>( a_rpsOptions[ l_iCtr ].f_pcShortForm[0] );
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
	return ( a_roBuffer.raw() );
	M_EPILOG
	}

option* make_option_array( OOption* const& a_rpsOptions, int a_iCount, HChunk& a_roBuffer )
	{
	M_PROLOG
	int l_iCtr = 0;
	option * l_psOptions = NULL;
	l_psOptions = a_roBuffer.get<option>();
	for ( l_iCtr = 0; l_iCtr < a_iCount; l_iCtr ++ )
		{
		memset( &l_psOptions[ l_iCtr ], 0, sizeof( option ) );
		l_psOptions[ l_iCtr ].name = const_cast<char*>( a_rpsOptions[ l_iCtr ].f_pcName );
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
		if ( a_rpsOptions[ l_iCtr ].f_pcShortForm )
			l_psOptions[ l_iCtr ].val = a_rpsOptions[ l_iCtr ].f_pcShortForm[0];
		}
	return ( l_psOptions );
	M_EPILOG
	}

int decode_switches( int const a_iArgc, char* const* const a_ppcArgv,
		OOption* const& a_rpsOptions, int const a_iCount,
		int* const a_piUnknown )
	{
	M_PROLOG
	bool l_bValidSwitch = false;
	int l_iChar = 0, l_iCtr = 0;
	char const * l_pcShortOpts = NULL;
	option * l_psOptionArray = NULL;
	HString l_oShortOptBuffer;
	HChunk l_oLongOptBuffer( xcalloc<option>( a_iCount ) );
	hcore::log << "Decoding switches ... ";
	l_pcShortOpts = make_short_opts( a_rpsOptions, a_iCount, l_oShortOptBuffer );
	l_psOptionArray = make_option_array( a_rpsOptions, a_iCount,
			l_oLongOptBuffer );
	while ( ( l_iChar = ::getopt_long( a_iArgc, a_ppcArgv, l_pcShortOpts,
					l_psOptionArray, NULL ) ) != EOF )
		{
		l_bValidSwitch = false;
		for ( l_iCtr = 0; l_iCtr < a_iCount; l_iCtr ++ )
			{
			if ( a_rpsOptions[ l_iCtr ].f_pcShortForm
					&& ( a_rpsOptions[ l_iCtr ].f_pcShortForm[0] == l_iChar ) )
				l_bValidSwitch = true, set_option( a_rpsOptions[ l_iCtr ], optarg );
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

}

