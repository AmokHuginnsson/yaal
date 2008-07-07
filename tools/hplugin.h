/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hplugin.h - this file is integral part of `yaal' project.

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

#ifndef __YAAL_TOOLS_HPLUGIN_H
#define __YAAL_TOOLS_HPLUGIN_H

#include "hcore/hexception.h"
#include "hcore/hpointer.h"

namespace yaal
{

namespace tools
{

class HPlugin
	{
	template<typename tType>
	union caster_t
		{
		void* f_pvObjectPointer;
		tType FUNCTION_POINTER;
		};
	typedef HPlugin self_t;
	void* f_pvHandle;
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
private:
	HPlugin( HPlugin const& );
	HPlugin const& operator = ( HPlugin const& );
	};

template<typename name_t>
void HPlugin::resolve( yaal::hcore::HString const& a_oName, name_t& handle )
	{
	M_PROLOG
	caster_t<name_t> l_xCaster;
	l_xCaster.f_pvObjectPointer = resolve( a_oName );
	handle = l_xCaster.FUNCTION_POINTER;
	return;
	M_EPILOG
	}

typedef yaal::hcore::HExceptionT<HPlugin> HPluginException;

}

}

#endif /* not __YAAL_TOOLS_HPLUGIN_H */

