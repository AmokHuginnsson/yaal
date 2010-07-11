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

HPlugin::HPlugin( void ) : _handle( NULL )
	{
	}

HPlugin::~HPlugin( void )
	{
	M_PROLOG
	if ( _handle )
		unload();
	M_EPILOG
	}

void HPlugin::load( HString const& path_ )
	{
	M_PROLOG
	_handle = dlopen( ! path_.is_empty() ? path_.raw() : NULL, RTLD_NOW | RTLD_GLOBAL );
	if ( _handle == reinterpret_cast<void const*>( -1 ) )
		_handle = NULL;
	M_ENSURE_EX( _handle != NULL, path_ );
	return;
	M_EPILOG
	}

void HPlugin::unload( void )
	{
	M_PROLOG
#if ! defined( __HOST_OS_TYPE_FREEBSD__ )
	M_ENSURE( dlclose( _handle ) == 0 );
#else /* not __HOST_OS_TYPE_FREEBSD__ */
	dlclose( _handle );
#endif /* __HOST_OS_TYPE_FREEBSD__ */
	_handle = NULL;
	return;
	M_EPILOG
	}

char const* HPlugin::error_message( int )
	{
	M_PROLOG
	return ( dlerror() );
	M_EPILOG
	}

void* HPlugin::resolve( HString const& symbolName_ )
	{
	M_PROLOG
	M_ASSERT( _handle );
	void* sym = NULL;
	M_ENSURE_EX( ( sym = dlsym( _handle, symbolName_.raw() ) ) != NULL, symbolName_ );
	return ( sym );
	M_EPILOG
	}

bool HPlugin::is_loaded( void ) const
	{
	return ( _handle ? true : false );
	}

}

}

