/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include <libintl.h>

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "hresource.hxx"
#include "hcore/hfile.hxx"
#include "hconsole/hmainwindow.hxx"
#include "hconsole/hwindowfactory.hxx"

using namespace yaal;
using namespace yaal::hcore;
using namespace yaal::tools;

namespace yaal {

namespace hconsole {

HResource::HResource( HTUIProcess* tui_, yaal::hcore::HString const& path_ )
	: _tui( tui_ ), _xml() {
	_xml.load( make_pointer<HFile>( path_, HFile::OPEN::READING ), HXml::PARSER::STRIP_COMMENT );
	return;
}

void HResource::load( void ) {
	M_PROLOG
	HXml::HConstNodeSet menuXml( _xml.get_elements_by_path( "/resource/menu" ) );
	M_ENSURE( menuXml.get_size() == 1 );
	HMenuWidget::data_ptr_t menu( new ( memory::yaal ) HMenuWidget::data_t );
	build_sub_menu( menuXml[0], menu->create_new_root() );
	_tui->main_window()->set_menu( menu );
	return;
	M_EPILOG
}

void HResource::build_sub_menu( HXml::HConstNodeProxy const& xmlNode_,
		HMenuWidget::data_t::node_t node_ ) {
	M_PROLOG
	char const* const error = _( "malformed resource file (menu section)" );
	char const* const unexpected = _( ": unexpected node: " );
	if ( ! xmlNode_.child_count() ) {
		M_THROW( HString( error ) + unexpected, xmlNode_.get_line() );
	}

	if ( xmlNode_.get_name() == "menu" ) {
		for ( HXml::HConstIterator it( xmlNode_.begin() ), end( xmlNode_.end() ); it != end; ++ it ) {
			build_menu_item( *it, &*(node_->add_node()) );
		}
	} else {
		M_THROW( HString( error ) + unexpected + xmlNode_.get_name()
				+ '=' + xmlNode_.get_value(), xmlNode_.get_line() );
	}
	return;
	M_EPILOG
}

void HResource::build_menu_item( HXml::HConstNodeProxy const& xmlNode_,
		HMenuWidget::data_t::node_t node_ ) {
	M_PROLOG
	char const* const error = _( "malformed resource file (menu section)" );
	char const* const unexpected = _( ": unexpected node: " );
	if ( ! xmlNode_.has_children() ) {
		M_THROW ( HString ( error ) + unexpected, xmlNode_.get_line() );
	}

	if ( xmlNode_.get_name() == "menu_item" ) {
		for ( HXml::HConstIterator it( xmlNode_.begin() ), end( xmlNode_.end() ); it != end; ++ it ) {
			HString const& name = (*it).get_name();
			HXml::HConstIterator handlerIt = (*it).begin();
			HString contents = ( ( handlerIt != (*it).end() ) && ( (*handlerIt).get_type() == HXml::HNode::TYPE::CONTENT ) ) ? (*(*it).begin()).get_value() : "";
			if ( name == "label" ) {
				(**node_)._label = contents;
			} else if ( name == "handler" ) {
				HXml::HNode::properties_t const& props = (*it).properties();
				HXml::HNode::properties_t::const_iterator nameIt = props.find( "name" );
				if ( nameIt != props.end() ) {
					HString const& command( nameIt->second );
					if ( command == "create_window" ) {
						HXml::HNode::properties_t::const_iterator paramIt = props.find( "param" );
						if ( ! ( paramIt != props.end() ) ) {
							M_THROW( "create_window command requires parameter", xmlNode_.get_line() );
						}
						(**node_)._call = call( &HResource::create_window, this, paramIt->second );
					} else {
						M_THROW( "unknown named command: " + command, xmlNode_.get_line() );
					}
				} else {
					/* unnamed handler */
					(**node_)._call = call( &HTUIProcess::execute_command, _tui, contents );
				}
			} else if ( name == "menu" ) {
				build_sub_menu( *it, node_ );
			} else {
				M_THROW( HString( error ) + unexpected + name
						+ '=' + contents, xmlNode_.get_line() );
			}
		}
	} else {
		M_THROW( HString( error ) + unexpected + xmlNode_.get_name()
				+ '=' + xmlNode_.get_value(), xmlNode_.get_line() );
	}
	return;
	M_EPILOG
}

void HResource::create_window( yaal::hcore::HString const& id_ ) {
	M_PROLOG
	HXml::HConstNodeProxy xmlWin( _xml.get_element_by_id( id_ ) );
	if ( ! xmlWin ) {
		M_THROW( _( "cannot find resources for this window: " ) + id_, errno );
	}
	HWindow::ptr_t win( HWindowFactory::get_instance().create_window( _tui, xmlWin ) );
	if ( ! win ) {
		M_THROW( _( "failed to construct window, at: " ), xmlWin.get_line() );
	}
	_tui->add_window( win );
	return;
	M_EPILOG
}

yaal::tools::xml::value_t HResource::get_value( yaal::hcore::HString const& path_ ) const {
	M_PROLOG
	return ( xml::try_node_val_by_path( _xml.get_root(), path_ ) );
	M_EPILOG
}

yaal::tools::xml::value_t HResource::get_attribute( yaal::hcore::HString const& path_ ) const {
	M_PROLOG
	return ( xml::try_attr_val_by_path( _xml.get_root(), path_ ) );
	M_EPILOG
}

}

}

