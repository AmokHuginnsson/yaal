/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

  hplugin.hxx - this file is integral part of `yaal' project.

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

#ifndef YAAL_TOOLS_HPLUGIN_HXX_INCLUDED
#define YAAL_TOOLS_HPLUGIN_HXX_INCLUDED 1

#include "hcore/base.hxx"
#include "hcore/hpointer.hxx"

namespace yaal {

namespace tools {

/*! \brief Dynamically load and use shared libraries.
 */
class HPlugin {
	typedef HPlugin this_type;
	void* _handle;
public:
	typedef yaal::hcore::HPointer<HPlugin> ptr_t;
	HPlugin( void );
	~HPlugin( void );
	void load( yaal::hcore::HString const& );
	void unload( void );
	bool is_loaded( void ) const;
	char const* error_message( int );
	template<typename name_t>
	void resolve( yaal::hcore::HString const&, name_t& );
	void* resolve( yaal::hcore::HString const& );
	template<typename name_t>
	void try_resolve( yaal::hcore::HString const&, name_t& );
	void* try_resolve( yaal::hcore::HString const& );
private:
	HPlugin( HPlugin const& );
	HPlugin const& operator = ( HPlugin const& );
};

template<typename name_t>
void HPlugin::resolve( yaal::hcore::HString const& name_, name_t& handle ) {
	M_PROLOG
	handle = bit_cast<name_t>( resolve( name_ ) );
	return;
	M_EPILOG
}

template<typename name_t>
void HPlugin::try_resolve( yaal::hcore::HString const& name_, name_t& handle ) {
	M_PROLOG
	handle = bit_cast<name_t>( try_resolve( name_ ) );
	return;
	M_EPILOG
}

typedef yaal::hcore::HExceptionT<HPlugin> HPluginException;

}

}

#endif /* #ifndef YAAL_TOOLS_HPLUGIN_HXX_INCLUDED */

