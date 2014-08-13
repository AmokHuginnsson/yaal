/*
---          `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

	htreewidget.hxx - this file is integral part of `yaal' project.

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

#ifndef YAAL_HCONSOLE_HTREEWIDGET_HXX_INCLUDED
#define YAAL_HCONSOLE_HTREEWIDGET_HXX_INCLUDED 1

#include "hcore/htree.hxx"
#include "hconsole/hitem.hxx"
#include "hconsole/hwindow.hxx"

namespace yaal {

namespace hconsole {

class HAbstractTreeModel {
public:
	typedef HAbstractTreeModel this_type;
	typedef yaal::hcore::HPointer<HAbstractTreeModel> ptr_t;
	typedef u64_t id_t;
	class HAbstractTreeModelNode {
	public:
		typedef HAbstractTreeModelNode this_type;
		typedef yaal::hcore::HPointer<HAbstractTreeModelNode> ptr_t;
		id_t get_id( void ) const {
			M_PROLOG
			return ( do_get_id() );
			M_EPILOG
		}
		int get_child_count( void ) const {
			M_PROLOG
			return ( do_get_child_count() );
			M_EPILOG
		}
		HAbstractTreeModelNode::ptr_t get_child( int childNo_ ) const {
			M_PROLOG
			return ( do_get_child( childNo_ ) );
			M_EPILOG
		}
		HAbstractTreeModelNode::ptr_t get_parent( void ) const {
			M_PROLOG
			return ( do_get_parent() );
			M_EPILOG
		}
		yaal::hcore::HString const get_long( void ) const {
			M_PROLOG
			return ( do_get_long() );
			M_EPILOG
		}
		yaal::hcore::HString const get_double( void ) const {
			M_PROLOG
			return ( do_get_double() );
			M_EPILOG
		}
		yaal::hcore::HString const get_string( void ) const {
			M_PROLOG
			return ( do_get_string() );
			M_EPILOG
		}
		yaal::hcore::HString get_time( void ) const {
			M_PROLOG
			return ( do_get_time() );
			M_EPILOG
		}
	protected:
		virtual id_t do_get_id( void ) const;
		virtual int do_get_child_count( void ) const = 0;
		virtual HAbstractTreeModelNode::ptr_t do_get_child( int ) const = 0;
		virtual HAbstractTreeModelNode::ptr_t do_get_parent( void ) const = 0;
		virtual yaal::hcore::HString const do_get_long( void ) const = 0;
		virtual yaal::hcore::HString const do_get_double( void ) const = 0;
		virtual yaal::hcore::HString const do_get_string( void ) const = 0;
		virtual yaal::hcore::HString do_get_time( void ) const = 0;
	};
	virtual ~HAbstractTreeModel( void );
	HAbstractTreeModelNode::ptr_t get_root( void ) const {
		M_PROLOG
		return ( do_get_root() );
		M_EPILOG
	}
protected:
	virtual HAbstractTreeModelNode::ptr_t do_get_root( void ) const = 0;
};

template<typename T>
class HAsIsValueTreeModel : public HAbstractTreeModel {
public:
	typedef HAsIsValueTreeModel this_type;
	typedef HAbstractTreeModel base_type;
private:
	typedef yaal::hcore::HTree<T> data_t;
	typedef yaal::hcore::HPointer<data_t> data_ptr_t;
	class HAsIsValueTreeModelNode : public HAbstractTreeModelNode {
		typedef typename data_t::node_t node_t;
		node_t _node;
		HAsIsValueTreeModelNode( node_t node_ )
			: HAbstractTreeModelNode(),
			_node( node_ ) {
		}
		virtual id_t do_get_id( void ) const {
			M_PROLOG
			return ( reinterpret_cast<id_t>( _node ) );
			M_EPILOG
		}
		virtual int do_get_child_count( void ) const {
			M_PROLOG
			return ( _node->child_count() );
			M_EPILOG
		}
		virtual HAbstractTreeModelNode::ptr_t do_get_child( int childNo_ ) const {
			M_PROLOG
			typename data_t::const_iterator it( _node->begin() );
			advance( it, childNo_ );
			return ( hcore::make_pointer<HAsIsValueTreeModelNode>( &*it ) );
			M_EPILOG
		}
		virtual HAbstractTreeModelNode::ptr_t do_get_parent( void ) const {
			M_PROLOG
			return ( hcore::make_pointer<HAsIsValueTreeModelNode>( _node->get_parent() ) );
			M_EPILOG
		}
		yaal::hcore::HString do_get_long( void ) const {
			M_PROLOG
			return ( (**_node).get_int_long() );
			M_EPILOG
		}
		yaal::hcore::HString do_get_double( void ) const {
			M_PROLOG
			return ( (**_node).get_double() );
			M_EPILOG
		}
		yaal::hcore::HString do_get_string( void ) const {
			M_PROLOG
			return ( (**_node).get_string() );
			M_EPILOG
		}
		yaal::hcore::HString do_get_time( void ) const {
			M_PROLOG
			return ( (**_node).get_time().string() );
			M_EPILOG
		}
	};
	data_ptr_t _data;
public:
	HAsIsValueTreeModel( data_ptr_t data_ )
		: _data( data_ ) {
	}
protected:
	virtual void do_get_root( HAbstractTreeModelNode::ptr_t node_ ) const {
		M_PROLOG
		return ( hcore::make_pointer<HAsIsValueTreeModelNode>( _data->get_root() ) );
		M_EPILOG
	}
};

/*! \brief Implementation of TUI Tree control class.
 *
 * Tree control allows fancy representation of tree based data with handful
 * of display alteration methods.
 */
class HTreeWidget : public virtual HWidget {
public:
	typedef HTreeWidget this_type;
	typedef HWidget base_type;
protected:
	class HNodeWidget;
	friend class HNodeWidget;
	/*! \brief Basic operatable element of TUI Tree control representing single tree node.
	 */
	class HNodeWidget {
	protected:
		bool _unfolded;	/* is this branch folded? */
		int _rowRaw;
		int	_columnRaw;
		int _widthRaw;
		HInfoItem _data;
	public:
		HNodeWidget( int = 0 );
		virtual ~HNodeWidget ( void );
		HInfo const& operator[]( int ) const;
		HInfo& operator[]( int );
		bool is_unfolded( void ) const;
	protected:
		void expand( void );
		void collapse( void );
		bool hit_test( int, int ) const;
		void click( int );
		friend class HTreeWidget;
	};
	typedef yaal::hcore::HTree<HNodeWidget> tree_t;
	HAbstractTreeModel::ptr_t _model;
	tree_t _tree;
	tree_t::node_t _selected;
public:
	HTreeWidget( HWindow* parent,
								 int row,
								 int col,
								 int height,
								 int width,
								 yaal::hcore::HString const& label );
	virtual ~HTreeWidget( void );
	int draw_node( tree_t::node_t, int );
	void set_model( HAbstractTreeModel::ptr_t );
	HAbstractTreeModel::ptr_t get_model( void ) const;
protected:
	virtual int do_process_input( int );
	virtual bool do_click( mouse::OMouse& );
	virtual void do_paint( void );
private:
	bool do_click( tree_t::node_t, mouse::OMouse& );
	void expand( tree_t::node_t );
	void collapse( tree_t::node_t );
	tree_t::node_t next( tree_t::node_t );
	tree_t::node_t previous( tree_t::node_t, bool = false );
	HTreeWidget( HTreeWidget const& );
	HTreeWidget& operator = ( HTreeWidget const& );
};

typedef yaal::hcore::HExceptionT<HTreeWidget, HWidgetException> HTreeWidgetException;

}

}

#endif /* #ifndef YAAL_HCONSOLE_HTREEWIDGET_HXX_INCLUDED */

