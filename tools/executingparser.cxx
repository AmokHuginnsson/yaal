/*
---           `yaal' 0.0.0 (c) 1978 by Marcin 'Amok' Konarski            ---

	executingparser.cxx - this file is integral part of `yaal' project.

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

#include <cctype>

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "executingparser.hxx"


#include "hcore/hfile.hxx"

using namespace yaal::hcore;

namespace yaal {

namespace tools {

HExecutingParser::HExecutingParser( executing_parser::HRuleBase const& rule_ )
	: _grammar( rule_.clone() ), _excutors(), _matched( false ) {
	M_PROLOG
	sanitize();
	M_EPILOG
}

bool HExecutingParser::operator()( yaal::hcore::HString const& input_ ) {
	M_PROLOG
	return ( parse( input_.begin(), input_.end() ) != input_.end() );
	M_EPILOG
}

bool HExecutingParser::operator()( yaal::hcore::HString::const_iterator first_, yaal::hcore::HString::const_iterator last_ ) {
	M_PROLOG
	return ( parse( first_, last_ ) != last_ );
	M_EPILOG
}

void HExecutingParser::operator()( void ) {
	M_PROLOG
	execute();
	return;
	M_EPILOG
}

void HExecutingParser::sanitize( void ) {
	M_PROLOG
	executing_parser::HGrammarDescription gd;
	_grammar->describe( gd );
	M_ENSURE( ! gd.is_empty() );
	return;
	M_EPILOG
}

void HExecutingParser::execute( void ) {
	M_PROLOG
	M_ENSURE( _matched );
	for ( execution_steps_t::iterator it( _excutors.begin() ), end( _excutors.end() ); it != end; ++ it )
		it->second();
	return;
	M_EPILOG
}

yaal::hcore::HString::const_iterator HExecutingParser::parse( yaal::hcore::HString::const_iterator first_, yaal::hcore::HString::const_iterator last_ ) {
	M_PROLOG
	yaal::hcore::HString::const_iterator it( _grammar->parse( this, first_, last_ ) );
	_matched = it != first_;
	return ( it );
	M_EPILOG
}

void HExecutingParser::add_execution_step( yaal::hcore::HString::const_iterator position_, executor_t const& executor_ ) {
	M_PROLOG
	_excutors.push_back( make_pair( position_, executor_ ) );
	return;
	M_EPILOG
}

void HExecutingParser::drop_execution_steps( yaal::hcore::HString::const_iterator ) {
	M_PROLOG
	return;
	M_EPILOG
}

namespace executing_parser {

HRuleBase::HRuleBase( void )
	: _action()
	{}

HRuleBase::HRuleBase( action_t const& action_ )
	: _action( action_ )
	{}

HRuleBase::ptr_t HRuleBase::clone( void ) const {
	M_PROLOG
	return ( do_clone() );
	M_EPILOG
}

bool HRuleBase::is_optional( void ) const {
	M_PROLOG
	return ( do_is_optional() );
	M_EPILOG
}

yaal::hcore::HString::const_iterator HRuleBase::parse( HExecutingParser* executingParser_, yaal::hcore::HString::const_iterator first_, yaal::hcore::HString::const_iterator last_ ) {
	M_PROLOG
	return ( do_parse( executingParser_, first_, last_ ) );
	M_EPILOG
}

void HRuleBase::describe( HGrammarDescription& gd_ ) const {
	M_PROLOG
	do_describe( gd_ );
	return;
	M_EPILOG
}

bool HRuleBase::do_is_optional( void ) const
	{ return ( false ); }

yaal::hcore::HString::const_iterator HRuleBase::skip_space( yaal::hcore::HString::const_iterator first_, yaal::hcore::HString::const_iterator last_ ) {
	M_PROLOG
	while ( ( first_ != last_ ) && isspace( *first_ ) )
		++ first_;
	return ( first_ );
	M_EPILOG
}

void HRuleBase::detach( HRuleBase const* rule_ ) {
	M_PROLOG
	do_detach( rule_ );
	return;
	M_EPILOG
}

HRuleBase::HNamedRule::HNamedRule( HRuleBase const& rule_ )
	: _name( dynamic_cast<HRule const*>( &rule_ ) ? dynamic_cast<HRule const*>( &rule_ )->get_name() : yaal::hcore::HString() ), _rule( rule_.clone() )
	{}

HRule::HRule( yaal::hcore::HString const& name_ )
	: HRuleBase(), _rule( name_, ptr_t( new HRecursiveRule() ) ), _completelyDefined( false )
	{}

HRule::HRule( void )
	: HRuleBase(), _rule( ptr_t( new HRecursiveRule() ) ), _completelyDefined( false )
	{}

HRule::HRule( HRule const& rule_ )
	: HRuleBase( rule_._action ), _rule( rule_._rule ), _completelyDefined( rule_._completelyDefined ) {
}

HRule::HRule( HRuleBase const& rule_ )
	: HRuleBase(), _rule( rule_.clone() ), _completelyDefined( true )
	{}

HRule::HRule( ptr_t const& rule_ )
	: _rule( rule_ ), _completelyDefined( true )
	{}

HRule::HRule( yaal::hcore::HString const& name_, HRuleBase const& rule_ )
	: HRuleBase(), _rule( name_, rule_.clone() ), _completelyDefined( true )
	{}

HRule::HRule( yaal::hcore::HString const& name_, ptr_t const& rule_ )
	: _rule( name_, rule_ ), _completelyDefined( true )
	{}

HRule::HRule( yaal::hcore::HString const& name_, ptr_t const& rule_, action_t const& action_ )
	: HRuleBase( action_ ), _rule( name_, rule_ ), _completelyDefined( true )
	{}

HRule::~HRule( void ) {
	M_PROLOG
	return;
	M_DESTRUCTOR_EPILOG
}

HRule& HRule::operator %= ( HRuleBase const& rule_ ) {
	M_PROLOG
	M_ENSURE( ! _completelyDefined );
	HRecursiveRule* rr( dynamic_cast<HRecursiveRule*>( _rule.id() ) );
	M_ENSURE( rr );
	rr->set_rule( rule_.clone() );
	_completelyDefined = true;
	return ( *this );
	M_EPILOG
}

HRule HRule::operator[]( action_t const& action_ ) const {
	M_PROLOG
	M_ENSURE( ! _action );
	return ( HRule( _rule._name, clone(), action_ ) );
	M_EPILOG
}

yaal::hcore::HString const& HRule::get_name( void ) const {
	M_PROLOG
	return ( _rule._name );
	M_EPILOG
}

HRule::ptr_t HRule::do_clone( void ) const {
	M_PROLOG
	M_ENSURE( !! _rule );
	return ( ! _action ? _rule._rule : ptr_t( new HRule( _rule._name, _rule._rule, _action ) ) );
	M_EPILOG
}

yaal::hcore::HString::const_iterator HRule::do_parse( HExecutingParser* executingParser_, yaal::hcore::HString::const_iterator first_, yaal::hcore::HString::const_iterator last_ ) {
	M_PROLOG
	yaal::hcore::HString::const_iterator ret( !! _rule ? _rule->parse( executingParser_, first_, last_ ) : first_ );
	if ( ( ret != first_ ) && !! _action )
		executingParser_->add_execution_step( first_, _action );
	return ( ret );
	M_EPILOG
}

void HRule::do_detach( HRuleBase const* rule_ ) {
	M_PROLOG
	HRuleBase::ptr_t r( _rule._rule );
	if ( !! r ) {
		if ( r.raw() == rule_ )
			_rule._rule = make_pointer<HRuleRef>( r );
		else
			r->detach( rule_ );
	}
	return;
	M_EPILOG
}

bool HRule::do_is_optional( void ) const
	{ return ( _rule->is_optional() ); }

void HRule::do_describe( HGrammarDescription& gd_ ) const {
	M_PROLOG
	if ( !! _rule ) {
		cout << _rule._name << " = ";
		gd_.describe( *_rule._rule );
		cout << endl;
	}
	return;
	M_EPILOG
}

HRecursiveRule::HRecursiveRule( void )
	: _rule() {
}

void HRecursiveRule::set_rule( HRuleBase::ptr_t const& rule_ ) {
	_rule = rule_;
	_rule->detach( this );
	return;
}

yaal::hcore::HString::const_iterator HRecursiveRule::do_parse( HExecutingParser* executingParser_, yaal::hcore::HString::const_iterator first_, yaal::hcore::HString::const_iterator last_ ) {
	M_PROLOG
	M_ENSURE( !! _rule );
	return ( _rule->parse( executingParser_, first_, last_ ) );
	M_EPILOG
}

HRuleBase::ptr_t HRecursiveRule::do_clone( void ) const {
	M_PROLOG
	return ( get_pointer() );
	M_EPILOG
}

bool HRecursiveRule::do_is_optional( void ) const {
	M_PROLOG
	M_ENSURE( _rule );
	return ( _rule->is_optional() );
	M_EPILOG
}

void HRecursiveRule::do_describe( HGrammarDescription& gd_ ) const {
	M_PROLOG
	if ( !! _rule )
		gd_.describe( *_rule.id() );
	return;
	M_EPILOG
}

void HRecursiveRule::do_detach( HRuleBase const* rule_ ) {
	M_PROLOG
	M_ENSURE( rule_ != this );
	if ( !! _rule )
		_rule->detach( rule_ );
	return;
	M_EPILOG
}

HRuleRef::HRuleRef( HRuleBase::ptr_t rule_ )
	: _rule( rule_ ) {
	return;
}

yaal::hcore::HString::const_iterator HRuleRef::do_parse( HExecutingParser* executingParser_, yaal::hcore::HString::const_iterator first_, yaal::hcore::HString::const_iterator last_ ) {
	M_PROLOG
	HRuleBase::ptr_t r( _rule );
	M_ENSURE( !! r );
	return ( r->parse( executingParser_, first_, last_ ) );
	M_EPILOG
}

HRuleBase::ptr_t HRuleRef::do_clone( void ) const {
	M_PROLOG
	return ( get_pointer() );
	M_EPILOG
}

bool HRuleRef::do_is_optional( void ) const {
	M_PROLOG
	HRuleBase::ptr_t r( _rule );
	M_ENSURE( !! r );
	return ( r->is_optional() );
	M_EPILOG
}

void HRuleRef::do_describe( HGrammarDescription& gd_ ) const {
	M_PROLOG
	HRuleBase::ptr_t r( _rule );
	M_ENSURE( !! r );
	r->describe( gd_ );
	return;
	M_EPILOG
}

void HRuleRef::do_detach( HRuleBase const* rule_ ) {
	M_PROLOG
	HRuleBase::ptr_t r( _rule );
	M_ENSURE( !! r );
	if ( r.raw() != rule_ )
		r->detach( rule_ );
	return;
	M_EPILOG
}

HFollows::HFollows( HRuleBase const& predecessor_, HRuleBase const& successor_ )
	: HRuleBase(), _rules() {
	M_PROLOG
	_rules.push_back( predecessor_ );
	_rules.push_back( successor_ );
	return;
	M_EPILOG
}

HFollows::HFollows( HFollows const& follows_ )
	: HRuleBase(), _rules() {
	M_PROLOG
	for ( rules_t::const_iterator it( follows_._rules.begin() ), end( follows_._rules.end() ); it != end; ++ it )
		_rules.push_back( *it );
	return;
	M_EPILOG
}

HFollows::HFollows( HFollows const& predecessors_, HRuleBase const& successor_ )
	: HRuleBase(), _rules() {
	M_PROLOG
	for ( rules_t::const_iterator it( predecessors_._rules.begin() ), end( predecessors_._rules.end() ); it != end; ++ it )
		_rules.push_back( *it );
	_rules.push_back( successor_ );
	return;
	M_EPILOG
}

HFollows::HFollows( HFollows::rules_t const& rules_, action_t const& action_ )
	: HRuleBase( action_ ), _rules( rules_ ) {
	return;
}

HFollows HFollows::operator[]( action_t const& action_ ) const {
	M_PROLOG
	return ( HFollows( _rules, action_ ) );
	M_EPILOG
}

HRuleBase::ptr_t HFollows::do_clone( void ) const {
	M_PROLOG
	return ( ptr_t( new HFollows( *this ) ) );
	M_EPILOG
}

void HFollows::do_describe( HGrammarDescription& gd_ ) const {
	M_PROLOG
	bool next( false );
	for ( rules_t::const_iterator it( _rules.begin() ), end( _rules.end() ); it != end; ++ it ) {
		if ( next )
			cout << " >> ";
		if ( ! it->_name.is_empty() ) {
			gd_._ruleOrder.push( &*it );
			cout << it->_name;
		} else
			gd_.describe( *it->id() );
		next = true;
	}
	while ( ! gd_._ruleOrder.is_empty() ) {
		HNamedRule const* nr( gd_._ruleOrder.front() );
		gd_._ruleOrder.pop();
		cout << endl << nr->_name << " = ";
		gd_.describe( *nr->_rule );
	}
	return;
	M_EPILOG
}

yaal::hcore::HString::const_iterator HFollows::do_parse( HExecutingParser* executingParser_, yaal::hcore::HString::const_iterator first_, yaal::hcore::HString::const_iterator last_ ) {
	M_PROLOG
	yaal::hcore::HString::const_iterator orig( first_ );
	bool matched( true );
	for ( rules_t::iterator it( _rules.begin() ), end( _rules.end() ); it != end; ++ it ) {
		yaal::hcore::HString::const_iterator old( first_ );
		first_ = (*it)->parse( executingParser_, first_, last_ );
		if ( ( first_ == old ) && ( !(*it)->is_optional() ) ) {
			matched = false;
			break;
		} else if ( !! _action && ( first_ != old ) )
			executingParser_->add_execution_step( old, _action );
	}
	return ( matched ? first_ : orig );
	M_EPILOG
}

void HFollows::do_detach( HRuleBase const* rule_ ) {
	M_PROLOG
	for ( rules_t::iterator it( _rules.begin() ), end( _rules.end() ); it != end; ++ it ) {
		HRuleBase::ptr_t r( it->_rule );
		if ( !! r ) {
			if ( r.raw() == rule_ )
				it->_rule = make_pointer<HRuleRef>( r );
			else
				r->detach( rule_ );
		}
	}
	return;
	M_EPILOG
}

HKleeneStar::HKleeneStar( HRuleBase const& rule_ )
	: HRuleBase(), _rule( rule_ )
	{}

HKleeneStar::HKleeneStar( HNamedRule const& rule_, action_t const& action_ )
	: HRuleBase( action_ ), _rule( rule_ )
	{}

HKleeneStar::HKleeneStar( HKleeneStar const& kleeneStar_ )
	: HRuleBase( kleeneStar_._action ), _rule( kleeneStar_._rule )
	{}

HKleeneStar HKleeneStar::operator[]( action_t const& action_ ) const {
	M_PROLOG
	M_ENSURE( ! _action );
	return ( HKleeneStar( _rule, action_ ) );
	M_EPILOG
}

HRuleBase::ptr_t HKleeneStar::do_clone( void ) const {
	M_PROLOG
	return ( ptr_t( new HKleeneStar( *this ) ) );
	M_EPILOG
}

bool HKleeneStar::do_is_optional( void ) const
	{ return ( true ); }

yaal::hcore::HString::const_iterator HKleeneStar::do_parse( HExecutingParser* executingParser_, yaal::hcore::HString::const_iterator first_, yaal::hcore::HString::const_iterator last_ ) {
	M_PROLOG
	yaal::hcore::HString::const_iterator old( last_ );
	while ( ( first_ != last_ ) && ( first_ != old ) ) {
		first_ = _rule->parse( executingParser_, old = first_, last_ );
		if ( !! _action && ( first_ != old ) )
			executingParser_->add_execution_step( old, _action );
	}
	return ( first_ );
	M_EPILOG
}

void HKleeneStar::do_describe( HGrammarDescription& gd_ ) const {
	M_PROLOG
	cout << "*(";
	gd_.describe( *_rule.id() );
	cout << ")";
	return;
	M_EPILOG
}

void HKleeneStar::do_detach( HRuleBase const* rule_ ) {
	M_PROLOG
	HRuleBase::ptr_t r( _rule._rule );
	if ( !! r ) {
		if ( r.raw() == rule_ )
			_rule._rule = make_pointer<HRuleRef>( r );
		else
			r->detach( rule_ );
	}
	return;
	M_EPILOG
}

HKleenePlus::HKleenePlus( HRuleBase const& rule_ )
	: HRuleBase(), _rule( rule_ )
	{}

HKleenePlus::HKleenePlus( HNamedRule const& rule_, action_t const& action_ )
	: HRuleBase( action_ ), _rule( rule_ )
	{}

HKleenePlus HKleenePlus::operator[]( action_t const& action_ ) const {
	M_PROLOG
	M_ENSURE( ! _action );
	return ( HKleenePlus( _rule, action_ ) );
	M_EPILOG
}

HKleenePlus::HKleenePlus( HKleenePlus const& kleenePlus_ )
	: HRuleBase( kleenePlus_._action ), _rule( kleenePlus_._rule )
	{}

HRuleBase::ptr_t HKleenePlus::do_clone( void ) const {
	M_PROLOG
	return ( ptr_t( new HKleenePlus( *this ) ) );
	M_EPILOG
}

yaal::hcore::HString::const_iterator HKleenePlus::do_parse( HExecutingParser* executingParser_, yaal::hcore::HString::const_iterator first_, yaal::hcore::HString::const_iterator last_ ) {
	M_PROLOG
	yaal::hcore::HString::const_iterator old( last_ );
	while ( ( first_ != last_ ) && ( first_ != old ) ) {
		first_ = _rule->parse( executingParser_, old = first_, last_ );
		if ( !! _action && ( first_ != old ) )
			executingParser_->add_execution_step( old, _action );
	}
	return ( first_ );
	M_EPILOG
}

void HKleenePlus::do_describe( HGrammarDescription& gd_ ) const {
	M_PROLOG
	cout << "+(";
	gd_.describe( *_rule.id() );
	cout << ")";
	return;
	M_EPILOG
}

void HKleenePlus::do_detach( HRuleBase const* rule_ ) {
	M_PROLOG
	HRuleBase::ptr_t r( _rule._rule );
	if ( !! r ) {
		if ( r.raw() == rule_ )
			_rule._rule = make_pointer<HRuleRef>( r );
		else
			r->detach( rule_ );
	}
	return;
	M_EPILOG
}

HAlternative::HAlternative( HRuleBase const& choice1_, HRuleBase const& choice2_ )
	: HRuleBase(), _rules() {
	M_PROLOG
	_rules.push_back( choice1_ );
	_rules.push_back( choice2_ );
	return;
	M_EPILOG
}

HAlternative::HAlternative( HAlternative const& alternative_ )
	: HRuleBase(), _rules() {
	M_PROLOG
	for ( rules_t::const_iterator it( alternative_._rules.begin() ), end( alternative_._rules.end() ); it != end; ++ it )
		_rules.push_back( *it );
	return;
	M_EPILOG
}

HAlternative::HAlternative( HAlternative const& alternative_, HRuleBase const& choice_ )
	: HRuleBase(), _rules() {
	M_PROLOG
	for ( rules_t::const_iterator it( alternative_._rules.begin() ), end( alternative_._rules.end() ); it != end; ++ it )
		_rules.push_back( *it );
	_rules.push_back( choice_ );
	return;
	M_EPILOG
}

HRuleBase::ptr_t HAlternative::do_clone( void ) const {
	M_PROLOG
	return ( ptr_t( new HAlternative( *this ) ) );
	M_EPILOG
}

yaal::hcore::HString::const_iterator HAlternative::do_parse( HExecutingParser*, yaal::hcore::HString::const_iterator, yaal::hcore::HString::const_iterator ) {
	return ( NULL );
}

void HAlternative::do_describe( HGrammarDescription& gd_ ) const {
	M_PROLOG
	bool next( false );
	for ( rules_t::const_iterator it( _rules.begin() ), end( _rules.end() ); it != end; ++ it ) {
		if ( next )
			cout << " | ";
		gd_.describe( *it->id() );
		next = true;
	}
	return;
	M_EPILOG
}

void HAlternative::do_detach( HRuleBase const* rule_ ) {
	M_PROLOG
	for ( rules_t::iterator it( _rules.begin() ), end( _rules.end() ); it != end; ++ it ) {
		HRuleBase::ptr_t r( it->_rule );
		if ( !! r ) {
			if ( r.raw() == rule_ )
				it->_rule = make_pointer<HRuleRef>( r );
			else
				r->detach( rule_ );
		}
	}
	return;
	M_EPILOG
}

HOptional::HOptional( HRuleBase const& rule_ )
	: HRuleBase(), _rule( rule_ ) {
}

HOptional::HOptional( HOptional const& optional_ )
	: HRuleBase( optional_._action ), _rule( optional_._rule ) {
}

HOptional::HOptional( HNamedRule const& rule_, action_t const& action_ )
	: HRuleBase( action_ ), _rule( rule_ ) {
}

HRuleBase::ptr_t HOptional::do_clone( void ) const {
	M_PROLOG
	return ( ptr_t( new HOptional( *this ) ) );
	M_EPILOG
}

HOptional HOptional::operator[]( action_t const& action_ ) const {
	M_PROLOG
	M_ENSURE( ! _action );
	return ( HOptional( _rule, action_ ) );
	M_EPILOG
}

yaal::hcore::HString::const_iterator HOptional::do_parse( HExecutingParser* executingParser_, yaal::hcore::HString::const_iterator first_, yaal::hcore::HString::const_iterator last_ ) {
	M_PROLOG
	yaal::hcore::HString::const_iterator ret( !! _rule ? _rule->parse( executingParser_, first_, last_ ) : first_ );
	if ( ( ret != first_ ) && !! _action )
		executingParser_->add_execution_step( first_, _action );
	return ( ret );
	M_EPILOG
}

bool HOptional::do_is_optional( void ) const
	{ return ( true ); }

void HOptional::do_describe( HGrammarDescription& gd_ ) const {
	M_PROLOG
	cout << "-(";
	gd_.describe( *_rule.id() );
	cout << ")";
	return;
	M_EPILOG
}

void HOptional::do_detach( HRuleBase const* rule_ ) {
	M_PROLOG
	HRuleBase::ptr_t r( _rule._rule );
	if ( !! r ) {
		if ( r.raw() == rule_ )
			_rule._rule = make_pointer<HRuleRef>( r );
		else
			r->detach( rule_ );
	}
	return;
	M_EPILOG
}

HFollows operator >> ( HRuleBase const& predecessor_, HRuleBase const& successor_ ) {
	M_PROLOG
	return ( HFollows( predecessor_, successor_ ) );
	M_EPILOG
}

HFollows operator >> ( HFollows const& predecessors_, HRuleBase const& successor_ ) {
	M_PROLOG
	return ( HFollows( predecessors_, successor_ ) );
	M_EPILOG
}

HAlternative operator | ( HRuleBase const& choice1_, HRuleBase const& choice2_ ) {
	M_PROLOG
	return ( HAlternative( choice1_, choice2_ ) );
	M_EPILOG
}

HAlternative operator | ( HAlternative const& alternative_, HRuleBase const& choice_ ) {
	M_PROLOG
	return ( HAlternative( alternative_, choice_ ) );
	M_EPILOG
}

HKleeneStar operator* ( HRuleBase const& rule_ ) {
	M_PROLOG
	return ( HKleeneStar( rule_ ) );
	M_EPILOG
}

HKleenePlus operator+ ( HRuleBase const& rule_ ) {
	M_PROLOG
	return ( HKleenePlus( rule_ ) );
	M_EPILOG
}

HOptional operator - ( HRuleBase const& rule_ ) {
	M_PROLOG
	return ( HOptional( rule_ ) );
	M_EPILOG
}

HReal::HReal( void )
	: HRuleBase(), _actionDouble(), _actionDoubleLong(),
	_actionNumber(), _actionString(), _cache()
	{}

HReal::HReal( action_t const& action_ )
	: HRuleBase( action_ ), _actionDouble(), _actionDoubleLong(),
	_actionNumber(), _actionString(), _cache()
	{}

HReal::HReal( action_double_t const& action_ )
	: HRuleBase(), _actionDouble( action_ ), _actionDoubleLong(),
	_actionNumber(), _actionString(), _cache()
	{}

HReal::HReal( action_double_long_t const& action_ )
	: HRuleBase(), _actionDouble(), _actionDoubleLong( action_ ),
	_actionNumber(), _actionString(), _cache()
	{}

HReal::HReal( action_number_t const& action_ )
	: HRuleBase(), _actionDouble(), _actionDoubleLong(),
	_actionNumber( action_ ), _actionString(), _cache()
	{}

HReal::HReal( action_string_t const& action_ )
	: HRuleBase(), _actionDouble(), _actionDoubleLong(),
	_actionNumber(), _actionString( action_ ), _cache()
	{}

HReal::HReal( HReal const& real_ )
	: HRuleBase( real_._action ), _actionDouble( real_._actionDouble ), _actionDoubleLong( real_._actionDoubleLong ),
	_actionNumber( real_._actionNumber ), _actionString( real_._actionString ), _cache( real_._cache )
	{}

HReal HReal::operator[]( action_t const& action_ ) const {
	M_PROLOG
	M_ENSURE( ( ! _action ) && ( ! _actionDouble ) && ( ! _actionDoubleLong ) && ( ! _actionNumber ) && ( ! _actionString ) );
	return ( HReal( action_ ) );
	M_EPILOG
}

HReal HReal::operator[]( action_double_t const& action_ ) const {
	M_PROLOG
	M_ENSURE( ( ! _action ) && ( ! _actionDouble ) && ( ! _actionDoubleLong ) && ( ! _actionNumber ) && ( ! _actionString ) );
	return ( HReal( action_ ) );
	M_EPILOG
}

HReal HReal::operator[]( action_double_long_t const& action_ ) const {
	M_PROLOG
	M_ENSURE( ( ! _action ) && ( ! _actionDouble ) && ( ! _actionDoubleLong ) && ( ! _actionNumber ) && ( ! _actionString ) );
	return ( HReal( action_ ) );
	M_EPILOG
}

HReal HReal::operator[]( action_number_t const& action_ ) const {
	M_PROLOG
	M_ENSURE( ( ! _action ) && ( ! _actionDouble ) && ( ! _actionDoubleLong ) && ( ! _actionNumber ) && ( ! _actionString ) );
	return ( HReal( action_ ) );
	M_EPILOG
}

HReal HReal::operator[]( action_string_t const& action_ ) const {
	M_PROLOG
	M_ENSURE( ( ! _action ) && ( ! _actionDouble ) && ( ! _actionDoubleLong ) && ( ! _actionNumber ) && ( ! _actionString ) );
	return ( HReal( action_ ) );
	M_EPILOG
}

yaal::hcore::HString::const_iterator HReal::do_parse( HExecutingParser* executingParser_, yaal::hcore::HString::const_iterator first_, yaal::hcore::HString::const_iterator last_ ) {
	M_PROLOG
	yaal::hcore::HString::const_iterator scan( skip_space( first_, last_ ) );
	_cache.clear();
	real_paring_state_t state( START );
	while ( scan != last_ ) {
		bool stop( false );
		char ch( *scan );
		switch ( state ) {
			case ( START ): {
				if ( isdigit( ch ) )
					state = INTEGRAL;
				else if ( ch == '-' )
					state = MINUS;
				else if ( ch == '.' )
					state = DOT;
				else
					stop = true;
			} break;
			case ( MINUS ):
			case ( INTEGRAL ): {
				if ( isdigit( ch ) )
					state = INTEGRAL;
				else if ( ch == '.' )
					state = DOT;
				else
					stop = true;
			} break;
			case ( DOT ):
			case ( DECIMAL ): {
				if ( isdigit( ch ) )
					state = DECIMAL;
				else
					stop = true;
			} break;
			default: {
				M_ASSERT( ! "invalid hardcoded state" );
			}
		}
		if ( stop )
			break;
		_cache.push_back( *scan );
		++ scan;
	}
	if ( state >= INTEGRAL ) {
		first_ = scan;
		if ( !! _actionDouble ) {
			double d( lexical_cast<double>( _cache ) );
			executingParser_->add_execution_step( first_, call( _actionDouble, d ) );
		} else if ( !! _actionDoubleLong ) {
			double long dl( lexical_cast<double long>( _cache ) );
			executingParser_->add_execution_step( first_, call( _actionDoubleLong, dl ) );
		} else if ( !! _actionNumber ) {
			executingParser_->add_execution_step( first_, call( _actionNumber, _cache ) );
		} else if ( !! _actionString ) {
			executingParser_->add_execution_step( first_, call( _actionString, _cache ) );
		} else if ( !! _action )
			executingParser_->add_execution_step( first_, call( _action ) );
	}
	return ( first_ );
	M_EPILOG
}

HRuleBase::ptr_t HReal::do_clone( void ) const {
	M_PROLOG
	return ( ptr_t( new HReal( *this ) ) );
	M_EPILOG
}

void HReal::do_describe( HGrammarDescription& ) const {
	M_PROLOG
	cout << "real";
	return;
	M_EPILOG
}

void HReal::do_detach( HRuleBase const* ) {
	M_PROLOG
	return;
	M_EPILOG
}

HReal const& get_real_instance( void ) {
	M_PROLOG
	static HReal realInstance;
	return ( realInstance );
	M_EPILOG
}

HReal const& real( get_real_instance() );

HInteger::HInteger( void )
	: HRuleBase(), _actionIntLong(), _actionInt(),
	_actionNumber(), _actionString(), _cache()
	{}

HInteger::HInteger( action_t const& action_ )
	: HRuleBase( action_ ), _actionIntLong(), _actionInt(),
	_actionNumber(), _actionString(), _cache()
	{}

HInteger::HInteger( action_int_long_t const& action_ )
	: HRuleBase(), _actionIntLong( action_ ), _actionInt(),
	_actionNumber(), _actionString(), _cache()
	{}

HInteger::HInteger( action_int_t const& action_ )
	: HRuleBase(), _actionIntLong(), _actionInt( action_ ),
	_actionNumber(), _actionString(), _cache()
	{}

HInteger::HInteger( action_number_t const& action_ )
	: HRuleBase(), _actionIntLong(), _actionInt(),
	_actionNumber( action_ ), _actionString(), _cache()
	{}

HInteger::HInteger( action_string_t const& action_ )
	: HRuleBase(), _actionIntLong(), _actionInt(),
	_actionNumber(), _actionString( action_ ), _cache()
	{}

HInteger::HInteger( HInteger const& integer_ )
	: HRuleBase( integer_._action ), _actionIntLong( integer_._actionIntLong ), _actionInt( integer_._actionInt ),
	_actionNumber( integer_._actionNumber ), _actionString( integer_._actionString ), _cache( integer_._cache )
	{}

HInteger HInteger::operator[]( action_t const& action_ ) const {
	M_PROLOG
	M_ENSURE( ( ! _action ) & ( ! _actionIntLong ) && ( ! _actionInt ) && ( ! _actionNumber ) && ( ! _actionString ) );
	return ( HInteger( action_ ) );
	M_EPILOG
}

HInteger HInteger::operator[]( action_int_long_t const& action_ ) const {
	M_PROLOG
	M_ENSURE( ( ! _action ) & ( ! _actionIntLong ) && ( ! _actionInt ) && ( ! _actionNumber ) && ( ! _actionString ) );
	return ( HInteger( action_ ) );
	M_EPILOG
}

HInteger HInteger::operator[]( action_int_t const& action_ ) const {
	M_PROLOG
	M_ENSURE( ( ! _action ) & ( ! _actionIntLong ) && ( ! _actionInt ) && ( ! _actionNumber ) && ( ! _actionString ) );
	return ( HInteger( action_ ) );
	M_EPILOG
}

HInteger HInteger::operator[]( action_number_t const& action_ ) const {
	M_PROLOG
	M_ENSURE( ( ! _action ) & ( ! _actionIntLong ) && ( ! _actionInt ) && ( ! _actionNumber ) && ( ! _actionString ) );
	return ( HInteger( action_ ) );
	M_EPILOG
}

HInteger HInteger::operator[]( action_string_t const& action_ ) const {
	M_PROLOG
	M_ENSURE( ( ! _action ) & ( ! _actionIntLong ) && ( ! _actionInt ) && ( ! _actionNumber ) && ( ! _actionString ) );
	return ( HInteger( action_ ) );
	M_EPILOG
}

yaal::hcore::HString::const_iterator HInteger::do_parse( HExecutingParser* executingParser_, yaal::hcore::HString::const_iterator first_, yaal::hcore::HString::const_iterator last_ ) {
	M_PROLOG
	yaal::hcore::HString::const_iterator scan( skip_space( first_, last_ ) );
	_cache.clear();
	real_paring_state_t state( START );
	while ( scan != last_ ) {
		bool stop( false );
		char ch( *scan );
		switch ( state ) {
			case ( START ): {
				if ( isdigit( ch ) )
					state = DIGIT;
				else if ( ch == '-' )
					state = MINUS;
				else
					stop = true;
			} break;
			case ( MINUS ):
			case ( DIGIT ): {
				if ( isdigit( ch ) )
					state = DIGIT;
				else
					stop = true;
			} break;
			default: {
				M_ASSERT( ! "invalid hardcoded state" );
			}
		}
		if ( stop )
			break;
		_cache.push_back( *scan );
		++ scan;
	}
	if ( state >= DIGIT ) {
		first_ = scan;
		if ( !! _actionIntLong ) {
			int long il( lexical_cast<int long>( _cache ) );
			executingParser_->add_execution_step( first_, call( _actionIntLong, il ) );
		} else if ( !! _actionInt ) {
			int i( lexical_cast<int>( _cache ) );
			executingParser_->add_execution_step( first_, call( _actionInt, i ) );
		} else if ( !! _actionNumber ) {
			executingParser_->add_execution_step( first_, call( _actionNumber, _cache ) );
		} else if ( !! _actionString ) {
			executingParser_->add_execution_step( first_, call( _actionString, _cache ) );
		} else if ( !! _action )
			executingParser_->add_execution_step( first_, call( _action ) );
	}
	return ( first_ );
	M_EPILOG
}

HRuleBase::ptr_t HInteger::do_clone( void ) const {
	M_PROLOG
	return ( ptr_t( new HInteger( *this ) ) );
	M_EPILOG
}

void HInteger::do_describe( HGrammarDescription& ) const {
	M_PROLOG
	cout << "integer";
	return;
	M_EPILOG
}

void HInteger::do_detach( HRuleBase const* ) {
	M_PROLOG
	return;
	M_EPILOG
}

HInteger const& get_integer_instance( void ) {
	M_PROLOG
	static HInteger integerInstance;
	return ( integerInstance );
	M_EPILOG
}

HInteger const& integer( get_integer_instance() );

HCharacter::HCharacter( char character_ )
	: HRuleBase(), _character( character_ ), _actionChar()
	{}

HCharacter::HCharacter( char character_, action_t const& action_ )
	: HRuleBase( action_ ), _character( character_ ), _actionChar()
	{}

HCharacter::HCharacter( char character_, action_char_t const& action_ )
	: HRuleBase(), _character( character_ ), _actionChar( action_ )
	{}

HCharacter::HCharacter( HCharacter const& character_ )
	: HRuleBase( character_._action ), _character( character_._character ), _actionChar( character_._actionChar )
	{}

HCharacter HCharacter::operator[]( action_t const& action_ ) const {
	M_PROLOG
	M_ENSURE( ( ! _action ) && ( ! _actionChar ) );
	return ( HCharacter( _character, action_ ) );
	M_EPILOG
}

HCharacter HCharacter::operator[]( action_char_t const& action_ ) const {
	M_PROLOG
	M_ENSURE( ( ! _action ) && ( ! _actionChar ) );
	return ( HCharacter( _character, action_ ) );
	M_EPILOG
}

HCharacter HCharacter::operator() ( char character_ ) const {
	M_PROLOG
	return ( HCharacter( character_, _action ) );
	M_EPILOG
}

yaal::hcore::HString::const_iterator HCharacter::do_parse( HExecutingParser* executingParser_, yaal::hcore::HString::const_iterator first_, yaal::hcore::HString::const_iterator last_ ) {
	M_PROLOG
	M_ENSURE( first_ != last_ );
	yaal::hcore::HString::const_iterator scan( skip_space( first_, last_ ) );
	char c( *scan );
	if ( _character ) {
		if ( ! _character || ( c == _character ) ) {
			if ( !! _actionChar )
				executingParser_->add_execution_step( first_, call( _actionChar, c ) );
			else if ( !! _action )
				executingParser_->add_execution_step( first_, call( _action ) );
			++ scan;
			first_ = scan;
		}
	}
	return ( first_ );
	M_EPILOG
}

HRuleBase::ptr_t HCharacter::do_clone( void ) const {
	M_PROLOG
	return ( ptr_t( new HCharacter( *this ) ) );
	M_EPILOG
}

void HCharacter::do_describe( HGrammarDescription& ) const {
	M_PROLOG
	if ( _character )
		cout << "'" << _character << "'";
	else
		cout << "character";
	return;
	M_EPILOG
}

void HCharacter::do_detach( HRuleBase const* ) {
	M_PROLOG
	return;
	M_EPILOG
}

HCharacter const& get_character_instance( void ) {
	M_PROLOG
	static HCharacter characterInstance;
	return ( characterInstance );
	M_EPILOG
}

HCharacter const& character( get_character_instance() );

HFollows operator >> ( char character_, HRuleBase const& successor_ ) {
	M_PROLOG
	return ( HFollows( character( character_ ), successor_  ) );
	M_EPILOG
}

HFollows operator >> ( HRuleBase const& predecessor_, char character_ ) {
	M_PROLOG
	return ( HFollows( predecessor_, character( character_ ) ) );
	M_EPILOG
}

HString::HString( hcore::HString const& string_ )
	: HRuleBase(), _string( string_ ), _actionString()
	{}

HString::HString( hcore::HString const& string_, action_t const& action_ )
	: HRuleBase( action_ ), _string( string_ ), _actionString()
	{}

HString::HString( hcore::HString const& string_, action_string_t const& action_ )
	: HRuleBase(), _string( string_ ), _actionString( action_ )
	{}

HString::HString( HString const& string_ )
	: HRuleBase( string_._action ), _string( string_._string ), _actionString( string_._actionString )
	{}

HString HString::operator[]( action_t const& action_ ) const {
	M_PROLOG
	M_ENSURE( ( ! _action ) && ( ! _actionString ) );
	return ( HString( _string, action_ ) );
	M_EPILOG
}

HString HString::operator[]( action_string_t const& action_ ) const {
	M_PROLOG
	M_ENSURE( ( ! _action ) && ( ! _actionString ) );
	return ( HString( _string, action_ ) );
	M_EPILOG
}

HString HString::operator() ( hcore::HString const& string_ ) const {
	M_PROLOG
	return ( HString( string_, _action ) );
	M_EPILOG
}

hcore::HString::const_iterator HString::do_parse( HExecutingParser*, hcore::HString::const_iterator first_, hcore::HString::const_iterator last_ ) {
	M_PROLOG
	M_ENSURE( first_ != last_ );
	if ( ! _string.is_empty() ) {
	}
	return ( first_ );
	M_EPILOG
}

HRuleBase::ptr_t HString::do_clone( void ) const {
	M_PROLOG
	return ( ptr_t( new HString( *this ) ) );
	M_EPILOG
}

void HString::do_describe( HGrammarDescription& ) const {
	M_PROLOG
	cout << "\"" << _string << "\"";
	return;
	M_EPILOG
}

void HString::do_detach( HRuleBase const* ) {
	M_PROLOG
	return;
	M_EPILOG
}

HString string( yaal::hcore::HString const& string_ ) {
	M_PROLOG
	return ( HString( string_ ) );
	M_EPILOG
}

HRegex::HRegex( hcore::HString const& string_ )
	: HRuleBase(), _regex( new hcore::HRegex( string_ ) ), _actionString()
	{}

HRegex::HRegex( regex_t const& regex_, action_t const& action_ )
	: HRuleBase( action_ ), _regex( regex_ ), _actionString()
	{}

HRegex::HRegex( regex_t const& regex_, action_string_t const& action_ )
	: HRuleBase(), _regex( regex_ ), _actionString( action_ )
	{}

HRegex::HRegex( HRegex const& regex_ )
	: HRuleBase( regex_._action ), _regex( regex_._regex ), _actionString( regex_._actionString )
	{}

HRegex HRegex::operator[]( action_t const& action_ ) const {
	M_PROLOG
	M_ENSURE( ( ! _action ) && ( ! _actionString ) );
	return ( HRegex( _regex, action_ ) );
	M_EPILOG
}

HRegex HRegex::operator[]( action_string_t const& action_ ) const {
	M_PROLOG
	M_ENSURE( ( ! _action ) && ( ! _actionString ) );
	return ( HRegex( _regex, action_ ) );
	M_EPILOG
}

hcore::HString::const_iterator HRegex::do_parse( HExecutingParser*, hcore::HString::const_iterator first_, hcore::HString::const_iterator last_ ) {
	M_PROLOG
	M_ENSURE( first_ != last_ );
	return ( first_ );
	M_EPILOG
}

HRuleBase::ptr_t HRegex::do_clone( void ) const {
	M_PROLOG
	return ( ptr_t( new HRegex( *this ) ) );
	M_EPILOG
}

void HRegex::do_describe( HGrammarDescription& ) const {
	M_PROLOG
	cout << "regex(\"" << _regex->pattern() << "\")";
	return;
	M_EPILOG
}

void HRegex::do_detach( HRuleBase const* ) {
	M_PROLOG
	return;
	M_EPILOG
}

HFollows operator >> ( char const* string_, HRuleBase const& successor_ ) {
	M_PROLOG
	return ( HFollows( string( string_ ), successor_  ) );
	M_EPILOG
}

HFollows operator >> ( hcore::HString const& string_, HRuleBase const& successor_ ) {
	M_PROLOG
	return ( HFollows( string( string_ ), successor_  ) );
	M_EPILOG
}

HFollows operator >> ( HRuleBase const& predecessor_, char const* string_ ) {
	M_PROLOG
	return ( HFollows( predecessor_, string( string_ ) ) );
	M_EPILOG
}

HFollows operator >> ( HRuleBase const& predecessor_, hcore::HString const& string_ ) {
	M_PROLOG
	return ( HFollows( predecessor_, string( string_ ) ) );
	M_EPILOG
}

HCharacter constant( char character_ ) {
	M_PROLOG
	return ( character( character_ ) );
	M_EPILOG
}

HString constant( yaal::hcore::HString const& string_ ) {
	M_PROLOG
	return ( string( string_ ) );
	M_EPILOG
}

HRegex regex( yaal::hcore::HString const& pattern_ ) {
	M_PROLOG
	return ( HRegex( pattern_ ) );
	M_EPILOG
}

HGrammarDescription::HGrammarDescription( void )
	: _rules(), _visited(), _namedRules(), _ruleOrder()
	{}

void HGrammarDescription::clear( void ) {
	M_PROLOG
	_rules.clear();
	_visited.clear();
	_namedRules.clear();
	_ruleOrder.clear();
	return;
	M_EPILOG
}

bool HGrammarDescription::is_empty( void ) const {
	M_PROLOG
	return ( _rules.is_empty() );
	M_EPILOG
}

HGrammarDescription::const_iterator HGrammarDescription::begin( void ) const {
	M_PROLOG
	return ( _rules.begin() );
	M_EPILOG
}

HGrammarDescription::const_iterator HGrammarDescription::end( void ) const {
	M_PROLOG
	return ( _rules.end() );
	M_EPILOG
}

bool HGrammarDescription::visited( HRuleBase const* rule_ ) const {
	M_PROLOG
	return ( _visited.count( rule_ ) > 0 );
	M_EPILOG
}

void HGrammarDescription::visiting( HRuleBase const* rule_ ) {
	M_PROLOG
	_visited.insert( rule_ );
	return;
	M_EPILOG
}

void HGrammarDescription::describe( HRuleBase const& rule_ ) {
	M_PROLOG
	if ( ! visited( &rule_ ) ) {
		visiting( &rule_ );
		rule_.describe( *this );
	}
	M_EPILOG
}

}

}

}

