/*
---          `stdhapi' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	htreecontrol.h - this file is integral part of `stdhapi' project.

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

#ifndef __HCONSOLE_HTREECONTROL_H
#define __HCONSOLE_HTREECONTROL_H

#include "../hcore/htree.h"
#include "hitem.h"
#include "hwindow.h"

class HTreeControl : public HTree < HItem >, public virtual HControl
	{
protected:
	class HNodeControl;
	friend class HNodeControl;
	class HNodeControl : public HTree < HItem >::HNode
		{
	protected:
		/*{*/
		bool f_bUnfolded;	/* is this branch folded? */
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
		/*}*/
		friend class HTreeControl;
		};
protected:
	/*{*/
	/*}*/
public:
	/*{*/
	HTreeControl ( HWindow *,			/* parent */
								 int,						/* row */
								 int,						/* col */
								 int,						/* height */
								 int,						/* width */
								 const char *,	/* label */
								 bool = true,		/* draw label */
								 int = -1, 			/* disabled attribute */
								 int = -1,			/* enabled attribute */
								 int = -1 );		/* focused attribute */
	virtual ~HTreeControl ( void );
	virtual void refresh ( void );
	int draw_node ( HNodeControl *, int );
	virtual int set_focus ( char = 0 );
	int process_input( int );
	/*}*/
protected:
	/*{*/
	/*}*/
	};

#endif /* not __HCONSOLE_HTREECONTROL_H */
