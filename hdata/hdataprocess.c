/*
---           `stdhapi' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hdataprocess.c - this file is integral part of `stdhapi' project.

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

#include "hdataprocess.h"

#include "../hcore/hexception.h"
#include "../hcore/hlog.h"
#include "../dbwrapper/db_driver_loader.h"

HDataProcess::HDataProcess ( void ) : HProcess ( )
	{
	M_PROLOG
	return;
	M_EPILOG
	}

HDataProcess::~HDataProcess ( void )
	{
	M_PROLOG
	return;
	M_EPILOG
	}

int HDataProcess::init ( const char * a_pcProcessName )
	{
	M_PROLOG
	int l_iError = HProcess::init ( a_pcProcessName );
	if ( ! dbwrapper::db_connect )
		throw new HException ( __WHERE__, "no database driver loaded", g_iErrNo );
	return ( l_iError );
	M_EPILOG
	}

HDataBase * HDataProcess::data_base ( void )
	{
	return ( & f_oDataBase );
	}

