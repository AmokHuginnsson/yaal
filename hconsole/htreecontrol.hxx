/*
---          `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	htreecontrol.hxx - this file is integral part of `yaal' project.

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

#ifndef YAAL_HCONSOLE_HTREECONTROL_HXX_INCLUDED
#define YAAL_HCONSOLE_HTREECONTROL_HXX_INCLUDED 1

#include "hcore/htree.hxx"
#include "hconsole/hitem.hxx"
#include "hconsole/hwindow.hxx"

namespace yaal
{

namespace hconsole
{

/*! \brief Implementation of TUI Tree control class.
 *
 * Tree control allows fancy representation of tree based data with handful
 * of display alteration methods.
 */
class HTreeControl : public virtual HControl
	{
protected:
	class HNodeControl;
	friend class HNodeControl;
	/*! \brief Basic operatable element of TUI Tree control representing single tree node.
	 */
	class HNodeControl
		{
	protected:
		bool _unfolded;	/* is this branch folded? */
		int _rowRaw;
		int	_columnRaw;
		int _widthRaw;
		HItem _data;
	public:
		HNodeControl( int = 0 );
		virtual ~HNodeControl ( void );
		yaal::hcore::HInfo& operator[]( int );
	protected:
		void expand( void );
		void collapse( void );
		bool hit_test( int, int ) const;
		void click( int );
		friend class HTreeControl;
		};
	typedef yaal::hcore::HTree<HNodeControl> tree_t;
	tree_t _tree;
	tree_t::node_t _selected;
public:
	HTreeControl( HWindow*,			/* parent */
								 int,						/* row */
								 int,						/* col */
								 int,						/* height */
								 int,						/* width */
								 char const* );	/* label */
	virtual ~HTreeControl( void );
	int draw_node( tree_t::node_t, int );
	virtual int set_focus( char = 0 );
protected:
	virtual int do_process_input( int );
	virtual int do_click( mouse::OMouse& );
	virtual void do_refresh( void );
private:
	bool do_click( tree_t::node_t, mouse::OMouse& );
	void expand( tree_t::node_t );
	void collapse( tree_t::node_t );
	tree_t::node_t next( tree_t::node_t );
	tree_t::node_t previous( tree_t::node_t, bool = false );
	HTreeControl( HTreeControl const& );
	HTreeControl& operator = ( HTreeControl const& );
	};

}

}

#endif /* #ifndef YAAL_HCONSOLE_HTREECONTROL_HXX_INCLUDED */

