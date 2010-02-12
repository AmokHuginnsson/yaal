/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hplugin.cxx - this file is integral part of `yaal' project.

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

#include <dlfcn.h>

#include "hcore/base.hxx"
M_VCSID( "$Id: "__ID__" $" )
M_VCSID( "$Id: "__TID__" $" )
#include "hplugin.hxx"

using namespace yaal::hcore;

namespace yaal
{

namespace tools
{

HPlugin::HPlugin( void ) : f_pvHandle( NULL )
	{
	}

HPlugin::~HPlugin( void )
	{
	M_PROLOG
	if ( f_pvHandle )
		unload();
	M_EPILOG
	}

void HPlugin::load( HString const& a_oPath )
	{
	M_PROLOG
	M_ENSURE_EX( ( ( f_pvHandle = dlopen( ! a_oPath.is_empty() ? a_oPath.raw() : NULL, RTLD_NOW | RTLD_GLOBAL ) ) != NULL ), a_oPath );
	return;
	M_EPILOG
	}

void HPlugin::unload( void )
	{
	M_PROLOG
#if ! defined( __HOST_OS_TYPE_FREEBSD__ )
	M_ENSURE( dlclose( f_pvHandle ) == 0 );
#else /* not __HOST_OS_TYPE_FREEBSD__ */
	dlclose( f_pvHandle );
#endif /* __HOST_OS_TYPE_FREEBSD__ */
	f_pvHandle = NULL;
	return;
	M_EPILOG
	}

char const* HPlugin::error_message( int )
	{
	M_PROLOG
	return ( dlerror() );
	M_EPILOG
	}

void* HPlugin::resolve( HString const& a_oSymbolName )
	{
	M_PROLOG
	M_ASSERT( f_pvHandle );
	void* sym = NULL;
	M_ENSURE_EX( ( ( sym = dlsym( f_pvHandle, a_oSymbolName.raw() ) ) != NULL ), a_oSymbolName );
	return ( sym );
	M_EPILOG
	}

bool HPlugin::is_loaded( void ) const
	{
	return ( f_pvHandle ? true : false );
	}

}

}

