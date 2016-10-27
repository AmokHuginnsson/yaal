/*
---           `yaal' (c) 1978 by Marcin 'Amok' Konarski            ---

  hsearchablewidget.cxx - this file is integral part of `yaal' project.

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
	, _searchActived( false )
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
	_searchActived = _pattern.parse( pattern_, &pf );
	if ( ! _searchActived ) {
		if ( _window ) {
			_window->status_bar()->message( "%s", _pattern.error().raw() );
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
	for ( HPattern::HMatchIterator it = _pattern.find( _varTmpBuffer.raw() ),
			end = _pattern.end(); it != end; ++ it ) {
		if ( ( _focused && ( ( currentIndex_ != ctr ) || ! current_ ) )
				|| ( ! _focused && ( currentIndex_ == ctr ) && current_ ) ) {
			cons.set_attr( _attributeSearchHighlight_ >> 8 );
		} else {
			cons.set_attr( _attributeSearchHighlight_ );
		}
		cons.mvprintf( row_,
				static_cast<int>( column_ + ( it->raw() - _varTmpBuffer.raw() ) ),
				"%.*s", it->size(), it->raw() );
		ctr ++;
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
	: HWidgetAttributes(),
	_searchable( false ),
	_searchableSet( false ) {
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
	return ( ok );
	M_EPILOG
}

}

}

