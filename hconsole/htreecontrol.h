/*
---          `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	htreecontrol.h - this file is integral part of `yaal' project.

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

#ifndef __YAAL_HCONSOLE_HTREECONTROL_H
#define __YAAL_HCONSOLE_HTREECONTROL_H

#include "hcore/htree.h"
#include "hconsole/hitem.h"
#include "hconsole/hwindow.h"

namespace yaal
{

namespace hconsole
{

class HTreeControl : public hcore::HTree < HItem >, public virtual HControl
	{
protected:
	class HNodeControl;
	friend class HNodeControl;
	class HNodeControl : public hcore::HTree < HItem >::HNode
		{
	protected:
		/*{*/
		bool f_bUnfolded;	/* is this branch folded? */
		int f_iRowRaw;
		int	f_iColumnRaw;
		int f_iWidthRaw;
		/*}*/
	public:
		/*{*/
		/*}*/
	protected:
		/*{*/
		HNodeControl ( HNodeControl * );
		virtual ~HNodeControl ( void );
		void expand ( void );
		void collapse ( void );
		bool hit_test ( int, int ) const;
		void click ( int );
		/*}*/
		friend class HTreeControl;
		};
public:
	/*{*/
	HTreeControl ( HWindow *,			/* parent */
								 int,						/* row */
								 int,						/* col */
								 int,						/* height */
								 int,						/* width */
								 char const *,	/* label */
								 bool = true,		/* draw label */
								 int = -1, 			/* disabled attribute */
								 int = -1,			/* enabled attribute */
								 int = -1 );		/* focused attribute */
	virtual ~HTreeControl ( void );
	virtual void refresh ( void );
	int draw_node ( HNodeControl *, int );
	virtual int set_focus ( char = 0 );
	virtual int process_input( int );
	virtual int click ( mouse::OMouse & );
	/*}*/
protected:
	/*{*/
	bool do_click ( HNodeControl *, mouse::OMouse & );
	/*}*/
	};

}

}

#endif /* not __YAAL_HCONSOLE_HTREECONTROL_H */
