/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "hsearchablewidget.hxx"
#include "hwindow.hxx"
#include "hconsole.hxx"
#include "tools/hxml.hxx"

using namespace yaal;
using namespace yaal::hcore;
using namespace yaal::tools;

namespace yaal {

namespace hconsole {

HSearchableWidget::HSearchableWidget( HWidgetAttributesInterface const& attrs_ )
	: HWidget( nullptr, 0, 0, 0, 0, hcore::HString(), attrs_ )
	, _searchable( false )
	, _searchActivated( false )
	, _filtered( false )
	, _backwards( false )
	, _pattern() {
	M_PROLOG
	attrs_.apply( *this );
	return;
	M_EPILOG
}

HSearchableWidget::~HSearchableWidget ( void ) {
	M_PROLOG
	return;
	M_DESTRUCTOR_EPILOG
}

void HSearchableWidget::search( HString const& pattern_, bool backwards_ ) {
	M_PROLOG
	HPattern::pluggable_flags_t pf;
	pf.push_back( make_pair( 'f', &_filtered ) );
	_searchActivated = _pattern.parse( pattern_, &pf );
	if ( ! _searchActivated ) {
		if ( _window ) {
			_window->status_bar()->message( "%s", _pattern.error() );
		}
	} else {
		_backwards = backwards_;
		if ( _backwards ) {
			go_to_match_previous();
		} else {
			go_to_match();
		}
	}
	schedule_repaint();
	return;
	M_EPILOG
}

void HSearchableWidget::highlight( int row_, int column_,
		int currentIndex_, bool current_ ) {
	M_PROLOG
	int long ctr( 0 );
	HConsole& cons = HConsole::get_instance();
	for ( HPattern::HMatchIterator it = _pattern.find( _varTmpBuffer ),
			end = _pattern.end(); it != end; ++ it ) {
		if ( ( _focused && ( ( currentIndex_ != ctr ) || ! current_ ) )
				|| ( ! _focused && ( currentIndex_ == ctr ) && current_ ) ) {
			cons.set_attr( _attributeSearchHighlight_._label );
		} else {
			cons.set_attr( _attributeSearchHighlight_._data );
		}
		cons.mvprintf( row_, column_ + it->start(), "%.*s", it->size(), _varTmpBuffer.substr( it->start(), it->size() ) );
		++ ctr;
	}
	return;
	M_EPILOG
}

void HSearchableWidget::set_searchable( bool searchable_ ) {
	M_PROLOG
	_searchable = searchable_;
	return;
	M_EPILOG
}

HSearchableWidgetAttributes::HSearchableWidgetAttributes( void )
	: HWidgetAttributes()
	, _searchable( false )
	, _searchableSet( false ) {
	return;
}

void HSearchableWidgetAttributes::do_apply( HWidget& widget_ ) const {
	M_PROLOG
	HSearchableWidget* widget( dynamic_cast<HSearchableWidget*>( &widget_ ) );
	if ( widget ) {
		if ( _searchableSet ) {
			widget->set_searchable( _searchable );
		}
	}
	HWidgetAttributes::do_apply( widget_ );
	return;
	M_EPILOG
}

HSearchableWidgetAttributes& HSearchableWidgetAttributes::searchable( bool searchable_ ) {
	M_PROLOG
	_searchable = searchable_;
	_searchableSet = true;
	return ( *this );
	M_EPILOG
}

bool HSearchableWidgetCreator::do_prepare_attributes( HWidgetAttributesInterface& attributes_, yaal::tools::HXml::HConstNodeProxy const& node_ ) {
	M_PROLOG
	HSearchableWidgetAttributes& attrs( dynamic_cast<HSearchableWidgetAttributes&>( attributes_ ) );
	HString const& name( node_.get_name() );
	bool ok( false );
	if ( name == "searchable" ) {
		attrs.searchable( lexical_cast<bool>( xml::node_val( node_ ) ) );
		ok = true;
	}
	return ok;
	M_EPILOG
}

}

}

