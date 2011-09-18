/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	omenuitem.hxx - this file is integral part of `yaal' project.

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

#ifndef YAAL_HCONSOLE_OMENUITEM_HXX_INCLUDED
#define YAAL_HCONSOLE_OMENUITEM_HXX_INCLUDED 1

#include "hconsole/htuiprocess.hxx"

namespace yaal {

namespace hconsole {

/*! \brief TUI menu node.
 */
struct OMenuItem {
public: /* All is public for simpler usage. */
	typedef int ( HTUIProcess::* HANDLER_t )( void* );
	OMenuItem* _subMenu;
	HANDLER_t HANDLER;
	void* _param;
	yaal::hcore::HString _label;
/* Methods */
	OMenuItem( void );
	OMenuItem( OMenuItem* const, HANDLER_t const, void*, yaal::hcore::HString const& );
	OMenuItem( OMenuItem const& );
	OMenuItem& operator = ( OMenuItem const& );
	void reset( void );
	void call( HTUIProcess* );
	void swap( OMenuItem& );
};

}

inline void swap( yaal::hconsole::OMenuItem& a, yaal::hconsole::OMenuItem& b )
	{ a.swap( b ); }

}

#endif /* #ifndef YAAL_HCONSOLE_OMENUITEM_HXX_INCLUDED */

