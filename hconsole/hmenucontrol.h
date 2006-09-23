/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	hmenucontrol.h - this file is integral part of `yaal' project.

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

#ifndef __YAAL_HCONSOLE_HMENUCONTROL_H
#define __YAAL_HCONSOLE_HMENUCONTROL_H

#include "hconsole/htreecontrol.h"
#include "hconsole/omenuitem.h"

namespace yaal
{

namespace hconsole
{

class HMenuControl : public HTreeControl
	{
	class HMenuNode;
	friend class HMenuNode;
	class HMenuNode : public HTreeControl::HNodeControl
		{
	protected:
		/*{*/
		/*}*/
	public:
		/*{*/
		int load_sub_menu ( OMenuItem * );
		/*}*/
	protected:
		/*{*/
		HMenuNode ( HMenuNode * );
		virtual ~HMenuNode ( void );
		/*}*/
		friend class HMenuControl;
		};
protected:
	/*{*/
	HTUIProcess * f_poProcess;
	/*}*/
public:
	/*{*/
	HMenuControl ( HWindow *,				/* parent */
								 int,							/* row */
								 int,							/* col */
								 int,							/* height */
								 int,							/* width */
								 char const * );	/* label */
	virtual ~HMenuControl ( void );
	void init ( HTUIProcess *, OMenuItem * );
	virtual int process_input( int );
	/*}*/
private:
	/*{*/
	HMenuControl ( HMenuControl const & );
	HMenuControl & operator = ( HMenuControl const & );
	/*}*/
	};

}

}

#endif /* not __YAAL_HCONSOLE_HMENUCONTROL_H */
