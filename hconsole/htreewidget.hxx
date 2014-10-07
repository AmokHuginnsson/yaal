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
#include "hconsole/hwidget.hxx"
#include "hconsole/hinfo.hxx"
#include "hconsole/hwidgetfactory.hxx"

namespace yaal {

namespace hconsole {

class HWindow;

class HTreeWidgetModelListener {
public:
	virtual ~HTreeWidgetModelListener( void ) {}
	void on_model_changed( void );
protected:
	virtual void do_on_model_changed( void ) = 0;
};

class HAbstractTreeModel {
public:
	typedef HAbstractTreeModel this_type;
	typedef yaal::hcore::HPointer<HAbstractTreeModel> ptr_t;
	typedef u64_t id_t;
	class HAbstractTreeModelNode {
	public:
		typedef HAbstractTreeModelNode this_type;
		typedef yaal::hcore::HPointer<HAbstractTreeModelNode> ptr_t;
		virtual ~HAbstractTreeModelNode() {
			return;
		}
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
		yaal::hcore::HString get_long( void ) const {
			M_PROLOG
			return ( do_get_long() );
			M_EPILOG
		}
		yaal::hcore::HString get_double( void ) const {
			M_PROLOG
			return ( do_get_double() );
			M_EPILOG
		}
		yaal::hcore::HString get_string( void ) const {
			M_PROLOG
			return ( do_get_string() );
			M_EPILOG
		}
		yaal::hcore::HString get_time( void ) const {
			M_PROLOG
			return ( do_get_time() );
			M_EPILOG
		}
		bool is_valid( void ) const {
			M_PROLOG
			return ( do_is_valid() );
			M_EPILOG
		}
	protected:
		virtual id_t do_get_id( void ) const = 0;
		virtual bool do_is_valid( void ) const = 0;
		virtual int do_get_child_count( void ) const = 0;
		virtual HAbstractTreeModelNode::ptr_t do_get_child( int ) const = 0;
		virtual HAbstractTreeModelNode::ptr_t do_get_parent( void ) const = 0;
		virtual yaal::hcore::HString do_get_long( void ) const = 0;
		virtual yaal::hcore::HString do_get_double( void ) const = 0;
		virtual yaal::hcore::HString do_get_string( void ) const = 0;
		virtual yaal::hcore::HString do_get_time( void ) const = 0;
	};
	virtual ~HAbstractTreeModel( void ) {
		return;
	}
	HAbstractTreeModelNode::ptr_t get_root( void ) const {
		M_PROLOG
		return ( do_get_root() );
		M_EPILOG
	}
	virtual void register_listener( HTreeWidgetModelListener* ) = 0;
protected:
	virtual HAbstractTreeModelNode::ptr_t do_get_root( void ) const = 0;
};

template<typename T = HInfoMultiVal>
class HAsIsValueTreeModel : public HAbstractTreeModel {
public:
	typedef HAsIsValueTreeModel this_type;
	typedef HAbstractTreeModel base_type;
	typedef yaal::hcore::HTree<T> data_t;
	typedef yaal::hcore::HPointer<data_t> data_ptr_t;
	class HAsIsValueTreeModelNode : public HAbstractTreeModelNode {
		typedef typename data_t::const_node_t node_t;
		node_t _node;
	public:
		HAsIsValueTreeModelNode( node_t node_ )
			: HAbstractTreeModelNode(),
			_node( node_ ) {
		}
		T const& get( void ) const {
			return ( **_node );
		}
	private:
		virtual id_t do_get_id( void ) const {
			M_PROLOG
			return ( reinterpret_cast<id_t>( _node ) );
			M_EPILOG
		}
		virtual bool do_is_valid( void ) const {
			M_PROLOG
			return ( _node != NULL );
			M_EPILOG
		}
		virtual int do_get_child_count( void ) const {
			M_PROLOG
			return ( static_cast<int>( _node->child_count() ) );
			M_EPILOG
		}
		virtual HAbstractTreeModelNode::ptr_t do_get_child( int childNo_ ) const {
			M_PROLOG
			typename data_t::HNode::const_iterator it( _node->begin() );
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
	private:
		HAsIsValueTreeModelNode( HAsIsValueTreeModelNode const& );
		HAsIsValueTreeModelNode& operator = ( HAsIsValueTreeModelNode const& );
	};
private:
	data_ptr_t _data;
	HTreeWidgetModelListener* _listener;
public:
	HAsIsValueTreeModel( data_ptr_t data_ )
		: _data( data_ ), _listener( NULL ) {
	}
	data_ptr_t get_data( void ) {
		return ( _data );
	}
protected:
	virtual HAbstractTreeModelNode::ptr_t do_get_root( void ) const {
		M_PROLOG
		return ( hcore::make_pointer<HAsIsValueTreeModelNode>( _data->get_root() ) );
		M_EPILOG
	}
	virtual void register_listener( HTreeWidgetModelListener* listener_ ) {
		_listener = listener_;
		return;
	}
private:
	HAsIsValueTreeModel( HAsIsValueTreeModel const& );
	HAsIsValueTreeModel& operator = ( HAsIsValueTreeModel const& );
};

/*! \brief Implementation of TUI Tree control class.
 *
 * Tree control allows fancy representation of tree based data with handful
 * of display alteration methods.
 */
class HTreeWidget : public virtual HWidget, public HTreeWidgetModelListener {
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
		HAbstractTreeModel::HAbstractTreeModelNode::ptr_t _data;
	public:
		HNodeWidget( HAbstractTreeModel::HAbstractTreeModelNode::ptr_t );
		virtual ~HNodeWidget ( void );
		bool is_unfolded( void ) const;
		HAbstractTreeModel::HAbstractTreeModelNode::ptr_t data( void );
	protected:
		void expand( void );
		void collapse( void );
		bool hit_test( int, int ) const;
		void click( int );
		friend class HTreeWidget;
	};
	typedef yaal::hcore::HTree<HNodeWidget> tree_view_t;
	HAbstractTreeModel::ptr_t _model;
	tree_view_t _view;
	tree_view_t::node_t _selected;
public:
	HTreeWidget( HWindow* parent,
			int row,
			int col,
			int height,
			int width,
			yaal::hcore::HString const& label,
			HWidgetAttributesInterface const& = HWidgetAttributesInterface(),
#ifndef _MSC_VER
			HAbstractTreeModel::ptr_t const& = hcore::make_pointer<HAsIsValueTreeModel<> >( hcore::make_pointer<HAsIsValueTreeModel<>::data_t>() )
#else /* #ifndef _MSC_VER */
			HAbstractTreeModel::ptr_t const& = HAsIsValueTreeModel<>::ptr_t( new ( memory::yaal ) HAsIsValueTreeModel<>( HAsIsValueTreeModel<>::data_ptr_t( new ( memory::yaal ) HAsIsValueTreeModel<>::data_t() ) ) )
#endif /* #else #ifndef _MSC_VER */
	);
	virtual ~HTreeWidget( void );
	int draw_node( tree_view_t::node_t, int );
	void set_model( HAbstractTreeModel::ptr_t );
	HAbstractTreeModel::ptr_t get_model( void ) const;
protected:
	virtual int do_process_input( int );
	virtual bool do_click( mouse::OMouse& );
	virtual void do_paint( void );
	virtual void do_on_model_changed( void );
private:
	void build_view( tree_view_t::node_t viewNode_, HAbstractTreeModel::HAbstractTreeModelNode::ptr_t modelNode_ );
	bool do_click( tree_view_t::node_t, mouse::OMouse& );
	void expand( tree_view_t::node_t );
	void collapse( tree_view_t::node_t );
	tree_view_t::node_t next( tree_view_t::node_t );
	tree_view_t::node_t previous( tree_view_t::node_t, bool = false );
	HTreeWidget( HTreeWidget const& );
	HTreeWidget& operator = ( HTreeWidget const& );
};

typedef yaal::hcore::HExceptionT<HTreeWidget, HWidgetException> HTreeWidgetException;

class HTreeWidgetAttributes : virtual public HWidgetAttributes {
};

class HTreeWidgetCreator : virtual public HWidgetCreatorInterface {
protected:
	virtual HWidget::ptr_t do_new_instance( HWindow*, yaal::tools::HXml::HConstNodeProxy const& );
	virtual bool do_prepare_attributes( HWidgetAttributesInterface&, yaal::tools::HXml::HConstNodeProxy const& );
	virtual bool do_apply_resources( HWidget::ptr_t, yaal::tools::HXml::HConstNodeProxy const& );
};

}

}

#endif /* #ifndef YAAL_HCONSOLE_HTREEWIDGET_HXX_INCLUDED */

