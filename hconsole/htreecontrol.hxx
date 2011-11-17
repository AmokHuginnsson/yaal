/*
---          `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	htreecontrol.hxx - this file is integral part of `yaal' project.

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

#ifndef YAAL_HCONSOLE_HTREECONTROL_HXX_INCLUDED
#define YAAL_HCONSOLE_HTREECONTROL_HXX_INCLUDED 1

#include "hcore/htree.hxx"
#include "hconsole/hitem.hxx"
#include "hconsole/hwindow.hxx"

namespace yaal {

namespace hconsole {

/*! \brief Implementation of TUI Tree control class.
 *
 * Tree control allows fancy representation of tree based data with handful
 * of display alteration methods.
 */
class HTreeControl : public virtual HControl {
public:
	typedef HTreeControl this_type;
	typedef HControl base_type;
protected:
	class HNodeControl;
	friend class HNodeControl;
	/*! \brief Basic operatable element of TUI Tree control representing single tree node.
	 */
	class HNodeControl {
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

typedef yaal::hcore::HExceptionT<HTreeControl, HControlException> HTreeControlException;

}

}

#endif /* #ifndef YAAL_HCONSOLE_HTREECONTROL_HXX_INCLUDED */

