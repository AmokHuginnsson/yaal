/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	hmenucontrol.hxx - this file is integral part of `yaal' project.

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

#ifndef YAAL_HCONSOLE_HMENUCONTROL_HXX_INCLUDED
#define YAAL_HCONSOLE_HMENUCONTROL_HXX_INCLUDED

#include "hconsole/htreecontrol.hxx"
#include "hconsole/omenuitem.hxx"
#include "hcore/hpointer.hxx"

namespace yaal
{

namespace hconsole
{

/*! \brief Implementation of TUI Menu control class.
 *
 * Menu control displays menu and automatically spawns apropriate actions.
 */
class HMenuControl : public HTreeControl
	{
protected:
	typedef HMenuControl self_t;
	typedef HTreeControl hier_t;
public:
	typedef yaal::hcore::HPointer<HMenuControl> ptr_t;
protected:
	HTUIProcess* f_poProcess;
public:
	HMenuControl( HWindow*,				/* parent */
								 int,							/* row */
								 int,							/* col */
								 int,							/* height */
								 int,							/* width */
								 char const* );	/* label */
	virtual ~HMenuControl( void );
	void init( HTUIProcess*, OMenuItem* );
protected:
	int load_sub_menu( tree_t::node_t, OMenuItem* );
	virtual int do_process_input( int );
private:
	HMenuControl( HMenuControl const& );
	HMenuControl& operator = ( HMenuControl const& );
	};

}

}

#endif /* not YAAL_HCONSOLE_HMENUCONTROL_HXX_INCLUDED */
