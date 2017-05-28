/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

  hplugin.cxx - this file is integral part of `yaal' project.

  i.  You may not make any changes in Copyright information.
  ii. You must attach Copyright information to any part of every copy
      of this software.

Copyright:

 You can use this software free of charge and you can redistribute its binary
 package freely but:
  1. You are not allowed to use any part of sources of this software.
  2. You are not allowed to redistribute any part of sources of this software.
  3. You are not allowed to reverse engineer this software.
  4. If you want to distribute a binary package of this software you cannot
     demand any fees for it. You cannot even demand
     a return of cost of the media or distribution (CD for example).
  5. You cannot involve this software in any commercial activity (for example
     as a free add-on to paid software or newspaper).
 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE. Use it at your own risk.
*/

#include <dlfcn.h>

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "hplugin.hxx"

using namespace yaal::hcore;

namespace yaal {

namespace tools {

HPlugin::HPlugin( void ) : _handle( nullptr ) {
}

HPlugin::~HPlugin( void ) {
	M_PROLOG
	if ( _handle )
		unload();
	M_DESTRUCTOR_EPILOG
}

void HPlugin::load( HString const& path_ ) {
	M_PROLOG
	HUTF8String utf8( path_ );
	_handle = dlopen( ! path_.is_empty() ? utf8.c_str() : nullptr, RTLD_NOW | RTLD_GLOBAL );
	if ( _handle == reinterpret_cast<void const*>( -1 ) )
		_handle = nullptr;
	M_ENSURE( _handle != nullptr, path_ );
	return;
	M_EPILOG
}

void HPlugin::unload( void ) {
	M_PROLOG
#if ! defined( __HOST_OS_TYPE_FREEBSD__ )
	M_ENSURE( dlclose( _handle ) == 0 );
#else /* not __HOST_OS_TYPE_FREEBSD__ */
	dlclose( _handle );
#endif /* __HOST_OS_TYPE_FREEBSD__ */
	_handle = nullptr;
	return;
	M_EPILOG
}

char const* HPlugin::error_message( int ) {
	M_PROLOG
	return ( dlerror() );
	M_EPILOG
}

void* HPlugin::try_resolve( HString const& symbolName_ ) {
	M_PROLOG
	HUTF8String utf8( symbolName_ );
	return ( dlsym( _handle, utf8.c_str() ) );
	M_EPILOG
}

void* HPlugin::resolve( HString const& symbolName_ ) {
	M_PROLOG
	M_ASSERT( _handle );
	void* sym( nullptr );
	M_ENSURE( ( sym = try_resolve( symbolName_ ) ) != nullptr, symbolName_ );
	return ( sym );
	M_EPILOG
}

bool HPlugin::is_loaded( void ) const {
	return ( _handle ? true : false );
}

}

}

