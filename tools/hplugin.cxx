/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hplugin.cxx - this file is integral part of `yaal' project.

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

#include <dlfcn.h>

#include "hcore/hexception.h"
M_VCSID ( "$Id$" )
#include "hplugin.h"

namespace yaal
{

namespace tools
{

HPlugin::HPlugin( void ) : f_pvHandle( NULL )
	{
	}

HPlugin::~HPlugin( void )
	{
	if ( f_pvHandle )
		unload();
	}

void HPlugin::load( char const* const a_pcPath )
	{
	M_PROLOG
	M_ENSURE( ( f_pvHandle = dlopen( a_pcPath, RTLD_NOW | RTLD_GLOBAL ) ) != NULL );
	return;
	M_EPILOG
	}

void HPlugin::unload( void )
	{
	M_PROLOG
	M_ENSURE( dlclose( f_pvHandle ) == 0 );
	f_pvHandle = NULL;
	return;
	M_EPILOG
	}

char const* HPlugin::error_message( int )
	{
	return ( dlerror() );
	}

void* HPlugin::resolve( char const* const a_pcSymbolName )
	{
	M_PROLOG
	M_ASSERT( f_pvHandle );
	void* sym = NULL;
	M_ENSURE( ( sym = dlsym( f_pvHandle, a_pcSymbolName ) ) != NULL );
	return ( sym );
	M_EPILOG
	}

bool HPlugin::is_loaded( void ) const
	{
	return ( f_pvHandle );
	}

}

}

