/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "hdatacomboboxwidget.hxx"
#include "tools/hxml.hxx"
#include "hconsole/console.hxx"
#include "hcore/hlog.hxx"

using namespace yaal::hcore;
using namespace yaal::tools;
using namespace yaal::hconsole;
using namespace yaal::dbwrapper;

namespace yaal {

namespace hdata {

HDataComboboxWidget::HDataComboboxWidget( HDataWindow* parent_,
		int row_, int column_, int height_, int width_,
		yaal::hcore::HString const& label_, HWidgetAttributesInterface const& attr_ )
	: HWidget( parent_, row_, column_, height_, width_, label_, attr_ ),
		HEditWidget( parent_, row_, column_, height_, width_, label_, attr_ ),
		HListWidget( parent_, row_, column_, height_, width_, label_, attr_ ),
		HComboboxWidget( parent_, row_, column_, height_, width_, label_, attr_ ),
		HDataWidget(),
		_indexId(),
		_dictName() {
	M_PROLOG
	attr_.apply( *this );
	init();
	return;
	M_EPILOG
}

void HDataComboboxWidget::do_update( void ) {
	M_PROLOG
	init();
	return;
	M_EPILOG
}

void HDataComboboxWidget::init( void ) {
	M_PROLOG
	M_ENSURE( ! _dictName.is_empty() );
	list_widget_helper::HAsIsValueListModel<>* model( dynamic_cast<list_widget_helper::HAsIsValueListModel<>*>( get_model().raw() ) );
	M_ASSERT( model );
	list_widget_helper::HAsIsValueListModel<>::data_ptr_t data( model->get_data() );
	data->clear();
	HComboboxWidget::do_update();
	HDataWindow* window( dynamic_cast<HDataWindow*>( _window ) );
	M_ASSERT( window );
	typedef HMap<HString, int> sorted_dict_t;
	sorted_dict_t sortedDict;
	HDictionary::ptr_t dict( window->get_dictionary( _dictName ) );
	for ( HDictionary::data_t::value_type const& val : dict->data() ) {
		sortedDict.insert( make_pair( val.second, val.first ) );
	}
	_indexId.clear();
	int index( 0 );
	for ( sorted_dict_t::value_type const& val : sortedDict ) {
		HInfoItem info( 1 );
		info[0].set_string( val.first );
		_indexId.insert( make_pair( index, val.second ) );
		data->push_back( info );
		++ index;
	}
	return;
	M_EPILOG
}

HInfo const& HDataComboboxWidget::do_get_data( void ) const {
	M_PROLOG
	int index( get_selected_index() );
	int id( index >= 0 ? _indexId.left().at( index ) : -1 );
	_infoInteger.set_integer( id );
	return ( _infoInteger );
	M_EPILOG
}

void HDataComboboxWidget::do_set_data( HInfo const& info_ ) {
	M_PROLOG
	int id( static_cast<int>( info_.get_integer() ) );
	int index( id >= 0 ? _indexId.right().at( id ) : -1 );
	select_by_index( index );
	return;
	M_EPILOG
}

void HDataComboboxWidget::set_dict_name( yaal::hcore::HString const& dictName_ ) {
	M_PROLOG
	_dictName = dictName_;
	return;
	M_EPILOG
}

HDataComboboxWidgetAttributes::HDataComboboxWidgetAttributes( void )
	: HComboboxWidgetAttributes(),
	_dictName(),
	_dictNameSet( false ) {
}

void HDataComboboxWidgetAttributes::do_apply( HWidget& widget_ ) const {
	M_PROLOG
	HComboboxWidgetAttributes::do_apply( widget_ );
	HDataComboboxWidget* widget( dynamic_cast<HDataComboboxWidget*>( &widget_ ) );
	if ( widget ) {
		if ( _dictNameSet ) {
			widget->set_dict_name( _dictName );
		}
	}
	return;
	M_EPILOG
}

HDataComboboxWidgetAttributes& HDataComboboxWidgetAttributes::dict_name( yaal::hcore::HString const& dictName_ ) {
	M_PROLOG
	_dictName = dictName_;
	_dictNameSet = true;
	return ( *this );
	M_EPILOG
}

HWidget::ptr_t HDataComboboxWidgetCreator::do_new_instance( HWindow* window_, yaal::tools::HXml::HConstNodeProxy const& node_ ) {
	M_PROLOG
	HDataComboboxWidgetAttributes attrs;
	HDataWindow* window( dynamic_cast<HDataWindow*>( window_ ) );
	M_ENSURE( window );
	prepare_attributes( attrs, node_ );
	OResource r( get_resource( node_ ) );
	HDataComboboxWidget* combobox( create_widget<HDataComboboxWidget>( window, r._row, r._column, r._height, r._width, r._label, attrs ) );
	apply_resources( combobox->get_pointer(), node_ );
	apply_role( window, combobox, node_ );
	return ( combobox->get_pointer() );
	M_EPILOG
}

bool HDataComboboxWidgetCreator::do_prepare_attributes( HWidgetAttributesInterface& attributes_, yaal::tools::HXml::HConstNodeProxy const& node_ ) {
	M_PROLOG
	bool ok( false );
	if ( !ok ) {
		ok = HComboboxWidgetCreator::do_prepare_attributes( attributes_, node_ );
	}
	if ( !ok ) {
		HDataComboboxWidgetAttributes& attrs( dynamic_cast<HDataComboboxWidgetAttributes&>( attributes_ ) );
		HString const& name( node_.get_name() );
		if ( name == "dict" ) {
			attrs.dict_name( xml::node_val( node_ ) );
		}
	}
	return ok;
	M_EPILOG
}

namespace {

bool register_creator( void ) {
	HWidgetFactory::get_instance().register_widget_creator( "datacombobox",
			HWidgetCreatorInterface::ptr_t( static_cast<HWidgetCreatorInterface*>( new ( memory::yaal ) HDataComboboxWidgetCreator() ) ) );
	return ( true );
}

bool volatile registered = register_creator();

}

}

}

