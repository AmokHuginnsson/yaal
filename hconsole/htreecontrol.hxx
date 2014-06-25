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

class HTreeControlModelInterface {
public:
	typedef HTreeControlModelInterface this_type;
	typedef yaal::hcore::HPointer<HTreeControlModelInterface> ptr_t;
	class HTreeControlModelNodeInterface {
	public:
		typedef HTreeControlModelNodeInterface this_type;
		typedef yaal::hcore::HPointer<HTreeControlModelNodeInterface> ptr_t;
	};
	virtual ~HTreeControlModelInterface( void );
	HTreeControlModelNodeInterface::ptr_t create_node_proxy( void ) {
		M_PROLOG
		return ( do_create_node_proxy() );
		M_EPILOG
	}
	void get_root( HTreeControlModelNodeInterface::ptr_t node_ ) const {
		M_PROLOG
		do_get_root( node_ );
		return;
		M_EPILOG
	}
	int long get_id( HTreeControlModelNodeInterface::ptr_t node_ ) const {
		M_PROLOG
		return ( do_get_id( node_ ) );
		M_EPILOG
	}
	int get_child_count( HTreeControlModelNodeInterface::ptr_t node_ ) const {
		M_PROLOG
		return ( do_get_child_count( node_ ) );
		M_EPILOG
	}
	void get_child( HTreeControlModelNodeInterface::ptr_t node_, int childNo_, HTreeControlModelNodeInterface::ptr_t child_ ) const {
		M_PROLOG
		do_get_child( node_, childNo_, child_ );
		return;
		M_EPILOG
	}
	void get_parent( HTreeControlModelNodeInterface::ptr_t node_, HTreeControlModelNodeInterface::ptr_t parent_ ) const {
		M_PROLOG
		do_get_parent( node_, parent_ );
		return;
		M_EPILOG
	}
protected:
	virtual HTreeControlModelNodeInterface::ptr_t do_create_node_proxy( void ) = 0;
	virtual void do_get_root( HTreeControlModelNodeInterface::ptr_t ) const = 0;
	virtual int long do_get_id( HTreeControlModelNodeInterface::ptr_t ) const;
	virtual int do_get_child_count( HTreeControlModelNodeInterface::ptr_t ) const = 0;
	virtual void do_get_child( HTreeControlModelNodeInterface::ptr_t, int, HTreeControlModelNodeInterface::ptr_t ) const = 0;
	virtual void do_get_parent( HTreeControlModelNodeInterface::ptr_t, HTreeControlModelNodeInterface::ptr_t ) const = 0;
};

template<typename T>
class HTreeControlModel : public HTreeControlModelInterface {
public:
	typedef HTreeControlModel this_type;
	typedef HTreeControlModelInterface base_type;
private:
	typedef yaal::hcore::HTree<T> data_t;
	typedef yaal::hcore::HPointer<data_t> data_ptr_t;
	class HTreeControlModelNode : public HTreeControlModelNodeInterface {
		typedef typename data_t::node_t node_t;
		typedef typename data_t::HNode::const_iterator iterator;
		node_t _node;
		iterator _iter;
		int _lastAccessedChild;
		HTreeControlModelNode( void )
			: HTreeControlModelNodeInterface(),
			_node( NULL ), _iter(), _lastAccessedChild( -1 )
			{}
		void set( node_t node_ ) {
			_node = node_;
		}
		node_t get( void ) const {
			return ( _node );
		}
		node_t nth_child( int childNo_ ) {
			M_PROLOG
			if ( childNo_ != _lastAccessedChild ) {
				_lastAccessedChild = childNo_;
			}
			return ( &*_iter );
			M_EPILOG
		}
	};
	data_ptr_t _data;
public:
	HTreeControlModel( data_ptr_t data_ )
		: _data( data_ )
		{}
protected:
	virtual HTreeControlModelNodeInterface::ptr_t do_create_node_proxy( void ) {
		M_PROLOG
		return ( hcore::make_pointer<HTreeControlModelNode>() );
		M_EPILOG
	}
	virtual void do_get_root( HTreeControlModelNodeInterface::ptr_t node_ ) const {
		M_PROLOG
		static_cast<HTreeControlModelNode*>( node_.get() )->set( _data->get_root() );
		return;
		M_EPILOG
	}
	virtual int long do_get_id( HTreeControlModelNodeInterface::ptr_t node_ ) const {
		M_PROLOG
		return ( reinterpret_cast<int long>( static_cast<HTreeControlModelNode*>( node_.get() )->get() ) );
		M_EPILOG
	}
	virtual int do_get_child_count( HTreeControlModelNodeInterface::ptr_t node_ ) const {
		M_PROLOG
		return ( static_cast<HTreeControlModelNode*>( node_.get() )->get()->child_count() );
		M_EPILOG
	}
	virtual void do_get_child( HTreeControlModelNodeInterface::ptr_t node_, int childNo_, HTreeControlModelNodeInterface::ptr_t child_ ) const {
		M_PROLOG
		typename data_t::const_iterator it( static_cast<HTreeControlModelNode*>( node_.get() )->get()->begin() );
		advance( it, childNo_ );
		static_cast<HTreeControlModelNode*>( child_.get() )->set( &*it );
		M_EPILOG
	}
	virtual void do_get_parent( HTreeControlModelNodeInterface::ptr_t node_, HTreeControlModelNodeInterface::ptr_t parent_ ) const {
		M_PROLOG
		return ( static_cast<HTreeControlModelNode*>( parent_.get() )->set( static_cast<HTreeControlModelNode*>( node_.get() )->get()->get_parent() ) );
		M_EPILOG
	}
};

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
		HInfoItem _data;
	public:
		HNodeControl( int = 0 );
		virtual ~HNodeControl ( void );
		HInfo const& operator[]( int ) const;
		HInfo& operator[]( int );
	protected:
		void expand( void );
		void collapse( void );
		bool hit_test( int, int ) const;
		void click( int );
		friend class HTreeControl;
	};
	typedef yaal::hcore::HTree<HNodeControl> tree_t;
	HTreeControlModelInterface::ptr_t _model;
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
	void set_model( HTreeControlModelInterface::ptr_t );
	HTreeControlModelInterface::ptr_t get_model( void ) const;
protected:
	virtual int do_process_input( int );
	virtual bool do_click( mouse::OMouse& );
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

