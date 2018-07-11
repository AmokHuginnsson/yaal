/* Read yaal/LICENSE.md file for copyright and licensing information. */

#include <cctype>
#include <cstdlib>

#include "hcore/base.hxx"
M_VCSID( "$Id: " __ID__ " $" )
M_VCSID( "$Id: " __TID__ " $" )
#include "executingparser.hxx"

#include "hcore/unicode.hxx"
#include "hcore/hstack.hxx"
#include "hcore/hfile.hxx"
#include "hcore/hmultimap.hxx"

using namespace yaal::hcore;
using namespace yaal::tools::util;

namespace yaal {

namespace tools {

namespace executing_parser {

namespace {

static char const _raw_[] = "\n\r\t\b\a\f\033\v";
static char const _safe_[] = "nrtbafev";
HNamedRule named_clone( HRuleBase const& rule_ ) {
	HRule const* r( dynamic_cast<HRule const*>( &rule_ ) );
	return ( HNamedRule( r ? r->get_name() : "", rule_.clone() ) );
}

}

EscapeTable const _escapes_(
	_raw_,  static_cast<int>( sizeof ( _raw_ ) )  - 1,
	_safe_, static_cast<int>( sizeof ( _safe_ ) ) - 1
);

HRecursionDetector::HRecursionDetector( void )
	: _visited()
	, _checkpoints() {
	_visited.emplace();
	_checkpoints.emplace();
	return;
}

bool HRecursionDetector::visit( HRuleBase const* rule_ ) {
	M_PROLOG
	visited_t& chk( _checkpoints.top() );
	if ( find( chk.begin(), chk.end(), rule_ ) != chk.end() ) {
		throw HRecursionDetectorException( "Infinite recursion detected: "_ys.append( rule_description( chk, rule_ ) ) );
	}
	chk.push_back( rule_ );
	bool firstTime( false );
	visited_t& vis( _visited.top() );
	if ( find( vis.begin(), vis.end(), rule_ ) == vis.end() ) {
		vis.push_back( rule_ );
		firstTime = true;
	}
	return ( firstTime );
	M_EPILOG
}

void HRecursionDetector::reset_visits( void ) {
	M_PROLOG
	_checkpoints.top().clear();
	return;
	M_EPILOG
}

/*! \brief Copy (backup) all visited nodes so far.
 *
 * Top of _checkpoints can be always messed with and throw away when no longer
 * needed when going back to the `root' on grammar tree.
 */
void HRecursionDetector::checkpoints_push( void ) {
	M_PROLOG
	visited_t visited( _visited.top() );
	_visited.emplace( yaal::move( visited ) );
	visited_t checkpoints( _checkpoints.top() );
	_checkpoints.emplace( yaal::move( checkpoints ) );
	return;
	M_EPILOG
}

void HRecursionDetector::checkpoints_pop( void ) {
	M_PROLOG
	_checkpoints.pop();
	_visited.pop();
	return;
	M_EPILOG
}

yaal::hcore::HString HRecursionDetector::rule_description( visited_t const& chk_, HRuleBase const* rule_ ) {
	M_PROLOG
	rule_use_t ru;
	chk_.front()->rule_use( ru );
	HRuleDescription rd;
	hcore::HString name;
	for ( HRuleBase const* r : chk_ ) {
		if ( dynamic_cast<HRuleRef const*>( r ) || dynamic_cast<HAction const*>( r ) ) {
			continue;
		}
		rd.clear();
		r->describe( rd, ru );
		for ( HNamedRule const* nr : rd.children() ) {
			if ( nr->id() == rule_ ) {
				name = rd.make_name( *nr );
				break;
			}
		}
		if ( ! name.is_empty() ) {
			break;
		}
	}
	rd.clear();
	if ( ! name.is_empty() ) {
		rd.desc( name );
		rd.desc( " = " );
	}
	rule_->describe( rd, ru );
	return ( rd.description() );
	M_EPILOG
}

class HRuleAggregator {
	typedef yaal::hcore::HArray<HNamedRule*> named_rules_t;
	typedef yaal::hcore::HMap<HRuleBase::ptr_t, named_rules_t> recursions_t;
	recursions_t _recursions;
	visited_t _visited;
public:
	HRuleAggregator( HRuleBase* rule_ )
		: _recursions(), _visited() {
		rule_->find_recursions( *this );
		merge();
	}
	bool visit( HRuleBase const* rule_ ) {
		M_PROLOG
		return ( _visited.insert( rule_ ).second );
		M_EPILOG
	}
	void verify( HNamedRule& nr_ ) {
		M_PROLOG
		HRuleBase::ptr_t r( nr_.rule() );
		if ( !! r ) {
			HRuleRef const* rref( dynamic_cast<HRuleRef const*>( r.raw() ) );
			if ( rref ) {
				HRuleBase::ptr_t rb( rref->get_rule() );
				M_ASSERT( !! rb );
				if ( ! have_strict( rb ) ) {
					nr_.reset( r = rb );
				}
			}
			add( r, &nr_ );
			r->find_recursions( *this );
		}
		return;
		M_EPILOG
	}
private:
	void add( HRuleBase::ptr_t rule_, HNamedRule* parent_ ) {
		M_PROLOG
		_recursions[ rule_ ].push_back( parent_ );
		return;
		M_EPILOG
	}
	bool have_strict( HRuleBase::ptr_t rule_ ) {
		M_PROLOG
		return ( _recursions.count( rule_ ) > 0 );
		M_EPILOG
	}
	void merge( void ) {
		M_PROLOG
		for ( recursions_t::iterator it( _recursions.begin() ), end( _recursions.end() ); it != end; ++ it ) {
			if ( it->second.size() > 1 ) {
				named_rules_t& nrs( it->second );
				named_rules_t::iterator i( nrs.begin() );
				++ i;
				for ( named_rules_t::iterator e( nrs.end() ); i != e; ++ i ) {
					(*i)->reset( make_pointer<HRuleRef>( it->first ) );
				}
			}
		}
		return;
		M_EPILOG
	}
};

}

HExecutingParser::HExecutingParser( executing_parser::HRuleBase const& rule_, INIT_MODE initMode_ )
	: _grammar( rule_.clone() )
	, _executors()
	, _matched( false )
	, _errorPosition( yaal::hcore::HString::npos )
	, _errorMessages()
	, _buffer()
	, _inputStart() {
	M_PROLOG
	if ( initMode_ == INIT_MODE::VERIFY_GRAMMAR ) {
		sanitize();
	}
	M_EPILOG
}

bool HExecutingParser::operator()( yaal::hcore::HString const& input_ ) {
	M_PROLOG
	return ( parse( input_.begin(), input_.end() ) );
	M_EPILOG
}

bool HExecutingParser::operator()( yaal::hcore::HString::const_iterator first_, yaal::hcore::HString::const_iterator last_ ) {
	M_PROLOG
	return ( parse( first_, last_ ) );
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
	try {
		executing_parser::HGrammarDescription gd( *_grammar );
		M_ENSURE( ! gd.is_empty() );
		executing_parser::HRecursionDetector recursionDetector;
		_grammar->detect_recursion( recursionDetector, false );
	} catch ( executing_parser::HRuleBaseException const& e ) {
		throw HExecutingParserException( e.what() );
	} catch ( executing_parser::HRecursionDetectorException const& e ) {
		throw HExecutingParserException( e.what() );
	} catch ( executing_parser::HGrammarDescriptionException const& e ) {
		throw HExecutingParserException( e.what() );
	}
	return;
	M_EPILOG
}

void HExecutingParser::execute( void* id_ ) {
	M_PROLOG
	M_ENSURE( _matched );
	for ( execution_steps_t::iterator it( _executors.begin() ), end( _executors.end() ); it != end; ++ it ) {
		if ( ! id_ || ( it->id() == id_ ) ) {
			(*it)();
		}
	}
	return;
	M_EPILOG
}

bool HExecutingParser::parse( yaal::hcore::HString::const_iterator first_, yaal::hcore::HString::const_iterator last_ ) {
	M_PROLOG
	_buffer.assign( first_, last_ );
	return ( do_parse() );
	M_EPILOG
}


bool HExecutingParser::do_parse( void ) {
	M_PROLOG
	_executors.clear();
	_inputStart = _buffer.begin();
	_errorPosition = yaal::hcore::HString::npos;
	_errorMessages.clear();
	yaal::hcore::HUTF8String::const_iterator it( _grammar->parse( this, _buffer.begin(), _buffer.end() ) );
	if ( ( it != _buffer.begin() ) && ( position( it ) != _errorPosition ) && ( executing_parser::HRuleBase::skip_space( it, _buffer.end() ) == _buffer.end() ) ) {
		_errorPosition = yaal::hcore::HString::npos;
		_errorMessages.clear();
	} else if ( _errorPosition == yaal::hcore::HString::npos ) {
		report_error( it, "failed to consume input" );
	}
	_inputStart = HUTF8String::const_iterator();
	_matched = ( it == _buffer.end() ) && ( ( _buffer.begin() != _buffer.end() ) || _grammar->is_optional() );
	return ( _matched );
	M_EPILOG
}

void HExecutingParser::add_execution_step( executor_t const& executor_ ) {
	M_PROLOG
	_executors.push_back( executor_ );
	return;
	M_EPILOG
}

void HExecutingParser::drop_execution_steps( int from_ ) {
	M_PROLOG
	_executors.erase( _executors.begin() + from_, _executors.end() );
	return;
	M_EPILOG
}

void HExecutingParser::report_error( yaal::hcore::HUTF8String::const_iterator const& position_, yaal::hcore::HString const& message_ ) {
	M_PROLOG
	int long pos( position( position_ ) );
	if ( pos > _errorPosition ) {
		_errorMessages.clear();
		_errorPosition = pos;
	}
	if ( pos == _errorPosition ) {
		_errorMessages.push_back( message_ );
	}
	return;
	M_EPILOG
}

int HExecutingParser::execution_step_count( void ) const {
	return ( static_cast<int>( _executors.get_size() ) );
}

int HExecutingParser::position( yaal::hcore::HUTF8String::const_iterator const& position_ ) {
	return ( static_cast<int>( position_ - _inputStart ) );
}

executing_parser::range_t HExecutingParser::range( yaal::hcore::HUTF8String::const_iterator const& start_, yaal::hcore::HUTF8String::const_iterator const& end_ ) {
	return ( executing_parser::HRange( static_cast<int>( start_ - _inputStart ), static_cast<int>( end_ - _inputStart ) ) );
}

int HExecutingParser::error_position( void ) const {
	M_PROLOG
	return ( static_cast<int>( _errorPosition ) );
	M_EPILOG
}

HExecutingParser::messages_t const& HExecutingParser::error_messages( void ) const {
	M_PROLOG
	return ( _errorMessages );
	M_EPILOG
}

void HExecutingParser::HProxy::add_execution_step( HExecutingParser* executingParser_, executor_t const& executor_ ) {
	M_PROLOG
	executingParser_->add_execution_step( executor_ );
	return;
	M_EPILOG
}

void HExecutingParser::HProxy::drop_execution_steps( HExecutingParser* executingParser_, int from_ ) {
	M_PROLOG
	executingParser_->drop_execution_steps( from_ );
	return;
	M_EPILOG
}

void HExecutingParser::HProxy::report_error( HExecutingParser* executingParser_, yaal::hcore::HUTF8String::const_iterator const& position_, yaal::hcore::HString const& message_ ) {
	M_PROLOG
	executingParser_->report_error( position_, message_ );
	return;
	M_EPILOG
}

int HExecutingParser::HProxy::execution_step_count( HExecutingParser const* executingParser_ ) {
	M_PROLOG
	return ( executingParser_->execution_step_count() );
	M_EPILOG
}

int HExecutingParser::HProxy::position( HExecutingParser* executingParser_, yaal::hcore::HUTF8String::const_iterator const& position_ ) {
	M_PROLOG
	return ( executingParser_->position( position_ ) );
	M_EPILOG
}

executing_parser::range_t HExecutingParser::HProxy::range( HExecutingParser* executingParser_, yaal::hcore::HUTF8String::const_iterator const& start_, yaal::hcore::HUTF8String::const_iterator const& end_ ) {
	M_PROLOG
	return ( executingParser_->range( start_, end_ ) );
	M_EPILOG
}

namespace executing_parser {

HRuleBase::HRuleBase( bool skipWS_ )
	: _action()
	, _actionPosition()
	, _skipWS( skipWS_ ) {
	return;
}

HRuleBase::HRuleBase( action_t const& action_, bool skipWS_ )
	: _action( action_ )
	, _actionPosition()
	, _skipWS( skipWS_ ) {
	return;
}

HRuleBase::HRuleBase( action_range_t const& action_, bool skipWS_ )
	: _action()
	, _actionPosition( action_ )
	, _skipWS( skipWS_ ) {
	return;
}

HRuleBase::HRuleBase( action_t const& action_, action_range_t const& actionPosition_, bool skipWS_ )
	: _action( action_ )
	, _actionPosition( actionPosition_ )
	, _skipWS( skipWS_ ) {
	return;
}

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

yaal::hcore::HUTF8String::const_iterator HRuleBase::parse( HExecutingParser* executingParser_, yaal::hcore::HUTF8String::const_iterator const& first_, yaal::hcore::HUTF8String::const_iterator const& last_ ) const {
	M_PROLOG
	M_ASSERT( ( first_ != HUTF8String::const_iterator() ) && ( last_ != HUTF8String::const_iterator() ) );
	int stepCount( HExecutingParser::HProxy::execution_step_count( executingParser_ ) );
	yaal::hcore::HUTF8String::const_iterator start( first_ );
	if ( _skipWS ) {
		skip_space( start, last_ );
	}
	yaal::hcore::HUTF8String::const_iterator it( do_parse( executingParser_, start, last_ ) );
	M_ASSERT( it != HUTF8String::const_iterator() );
	if ( it == start ) {
		if ( ! always_matches() && is_optional() ) {
			HExecutingParser::HProxy::drop_execution_steps( executingParser_, stepCount );
		}
		return ( first_ );
	}
	return ( it );
	M_EPILOG
}

void HRuleBase::add_execution_step( HExecutingParser* executingParser_, action_t const& executor_ ) const {
	M_PROLOG
	HExecutingParser::HProxy::add_execution_step( executingParser_, executor_ );
	return;
	M_EPILOG
}

void HRuleBase::drop_execution_steps( HExecutingParser* executingParser_, int from_ ) const {
	M_PROLOG
	HExecutingParser::HProxy::drop_execution_steps( executingParser_, from_ );
	return;
	M_EPILOG
}

void HRuleBase::report_error( HExecutingParser* executingParser_, yaal::hcore::HUTF8String::const_iterator const& position_, yaal::hcore::HString const& message_ ) const {
	M_PROLOG
	HExecutingParser::HProxy::report_error( executingParser_, position_, message_ );
	return;
	M_EPILOG
}

int HRuleBase::execution_step_count( HExecutingParser const* executingParser_ ) const {
	M_PROLOG
	return ( HExecutingParser::HProxy::execution_step_count( executingParser_ ) );
	M_EPILOG
}

int HRuleBase::position( HExecutingParser* executingParser_, yaal::hcore::HUTF8String::const_iterator const& position_ ) const {
	M_PROLOG
	return ( HExecutingParser::HProxy::position( executingParser_, position_ ) );
	M_EPILOG
}

executing_parser::range_t HRuleBase::range( HExecutingParser* executingParser_, yaal::hcore::HUTF8String::const_iterator const& start_, yaal::hcore::HUTF8String::const_iterator const& end_ ) const {
	M_PROLOG
	return ( executing_parser::range_t( HExecutingParser::HProxy::range( executingParser_, start_, end_ ) ) );
	M_EPILOG
}

void HRuleBase::describe( HRuleDescription& rd_, rule_use_t const& ru_ ) const {
	M_PROLOG
	int long descriptionLen( rd_.description().get_length() );
	int long childCount( rd_.children().get_size() );
	do_describe( rd_, ru_ );
	M_ENSURE( ( rd_.description().get_length() > descriptionLen ) || ( rd_.children().get_size() > childCount ), "empty rule" );
	return;
	M_EPILOG
}

void HRuleBase::rule_use( rule_use_t& ruleUse_ ) const {
	M_PROLOG
	do_rule_use( ruleUse_ );
	return;
	M_EPILOG
}

void HRuleBase::do_rule_use( rule_use_t& ruleUse_ ) const {
	M_PROLOG
	++ ruleUse_[ this ];
	return;
	M_EPILOG
}

bool HRuleBase::do_is_optional( void ) const {
	return ( false );
}

yaal::hcore::HUTF8String::const_iterator& HRuleBase::skip_space( yaal::hcore::HUTF8String::const_iterator& first_, yaal::hcore::HUTF8String::const_iterator const& last_ ) {
	M_PROLOG
	M_ASSERT( ( first_ != HUTF8String::const_iterator() ) && ( last_ != HUTF8String::const_iterator() ) );
	while ( ( first_ != last_ ) && is_whitespace( *first_ ) ) {
		++ first_;
	}
	return ( first_ );
	M_EPILOG
}

void HRuleBase::detach( HRuleBase const* rule_, visited_t& visited_, bool& detachAll_ ) {
	M_PROLOG
	visited_t::insert_result ir( visited_.insert( this ) );
	if ( ir.second ) {
		do_detach( rule_, visited_, detachAll_ );
	}
	return;
	M_EPILOG
}

bool HRuleBase::detect_recursion( HRecursionDetector& recursionDetector_, bool skipVisit_ ) const {
	M_PROLOG
	if ( ! skipVisit_ ) {
		skipVisit_ = ! recursionDetector_.visit( this );
	}
	return ( do_detect_recursion( recursionDetector_, skipVisit_ ) );
	M_EPILOG
}

void HRuleBase::find_recursions( HRuleAggregator& recursions_ ) {
	M_PROLOG
	if ( recursions_.visit( this ) ) {
		do_find_recursions( recursions_ );
	}
	return;
	M_EPILOG
}

bool HRuleBase::has_action( void ) const {
	return ( do_has_action() );
}

bool HRuleBase::skips_ws( void ) const {
	return ( _skipWS );
}

bool HRuleBase::do_has_action( void ) const {
	return ( ( !! _action ) || ( !! _actionPosition ) );
}

bool HRuleBase::do_always_matches( void ) const {
	return ( false );
}

HRuleBase const* HRuleBase::find( yaal::hcore::HString const& name_ ) const {
	M_PROLOG
	return ( do_find( name_ ) );
	M_EPILOG
}

HRuleBase const* HRuleBase::do_find( yaal::hcore::HString const& ) const {
	return ( nullptr );
}

HNamedRule::HNamedRule( yaal::hcore::HString const& name_, ptr_t rule_ )
	: _name( name_ )
	, _rule( rule_ ) {
	M_ENSURE( name_.is_empty() || ( name_[name_.get_length() - 1] != '_' ) );
}

HNamedRule::HNamedRule( HRuleBase const& rule_ )
	: _name( dynamic_cast<HRule const*>( &rule_ ) ? dynamic_cast<HRule const*>( &rule_ )->get_name() : yaal::hcore::HString() )
	, _rule( rule_.clone() ) {
	return;
}

yaal::hcore::HString const& HNamedRule::name( void ) const {
	M_PROLOG
	return ( _name );
	M_EPILOG
}

HRuleBase const* HNamedRule::id( void ) const {
	M_PROLOG
	HRuleRef const* rr( dynamic_cast<HRuleRef const*>( _rule.raw() ) );
	return ( rr ? rr->get_rule().raw() : _rule.raw() );
	M_EPILOG
}

void HNamedRule::reset( ptr_t const& rule_ ) {
	M_PROLOG
	_rule = rule_;
	return;
	M_EPILOG
}

HNamedRule::ptr_t const& HNamedRule::rule( void ) const {
	M_PROLOG
	return ( _rule );
	M_EPILOG
}

HNamedRule::ptr_t& HNamedRule::rule( void ) {
	M_PROLOG
	return ( _rule );
	M_EPILOG
}

/*
 * Rule definition may be inlined in grammar description or have independent definition stanza,
 * e.g.:
 *
 * inlined:
 * R = '{' >> ( real >> *( ',' >> real ) ) >> '}'
 *
 * or
 *
 * independent definition stanza:
 * R = '{' >> S >> '}'
 * S = real >> *( ',' >> real )
 *
 * Which of those two forms are used depends on two factors:
 * 1. does given rule have a name?
 * 2. was given rule referenced more than once?
 *
 * If any of above conditions are met the second form is used,
 * meaning, if rule have a name or was referenced more than once
 * then independent definition stanza is used.
 */
void HNamedRule::describe( HRuleDescription& rd_, rule_use_t const& ru_ ) const {
	M_PROLOG
	M_ASSERT( !! _rule );
	HRule const* r( dynamic_cast<HRule const*>( &*_rule ) );
	if ( r && ( ( ! _name.is_empty() && ( r->get_name() == _name ) ) || ( _name.is_empty() && r->has_action() ) ) ) {
		r->get_named_rule()->describe( rd_, ru_ );
	} else {
		rule_use_t::const_iterator it( ru_.find( _rule.get() ) );
		HRuleRef const* rr( dynamic_cast<HRuleRef const*>( &*_rule ) );
		if ( ( ( it != ru_.end() ) && ( it->second > 1 ) ) || ( ! _name.is_empty() ) || rr || ( r && r->has_action() ) ) {
			rd_.add( this );
			rd_.desc( rd_.make_name( *this ) );
		} else {
			_rule->describe( rd_, ru_ );
		}
	}
	return;
	M_EPILOG
}

HRule::HRule( WHITE_SPACE whiteSpace_ )
	: HRuleBase( whiteSpace_ != WHITE_SPACE::KEEP )
	, _rule( make_pointer<HRecursiveRule>() )
	, _completelyDefined( false ) {
	return;
}

HRule::HRule( action_t const& action_, WHITE_SPACE whiteSpace_ )
	: HRuleBase( action_, whiteSpace_ != WHITE_SPACE::KEEP )
	, _rule( make_pointer<HRecursiveRule>() )
	, _completelyDefined( false ) {
	return;
}

HRule::HRule( action_range_t const& action_, WHITE_SPACE whiteSpace_ )
	: HRuleBase( action_, whiteSpace_ != WHITE_SPACE::KEEP )
	, _rule( make_pointer<HRecursiveRule>() )
	, _completelyDefined( false ) {
	return;
}

HRule::HRule( yaal::hcore::HString const& name_, WHITE_SPACE whiteSpace_ )
	: HRuleBase( whiteSpace_ != WHITE_SPACE::KEEP )
	, _rule( name_, make_pointer<HRecursiveRule>() )
	, _completelyDefined( false ) {
	return;
}

HRule::HRule( yaal::hcore::HString const& name_, action_t const& action_, WHITE_SPACE whiteSpace_ )
	: HRuleBase( action_, whiteSpace_ != WHITE_SPACE::KEEP )
	, _rule( name_, make_pointer<HRecursiveRule>() )
	, _completelyDefined( false ) {
	return;
}

HRule::HRule( yaal::hcore::HString const& name_, action_range_t const& action_, WHITE_SPACE whiteSpace_ )
	: HRuleBase( action_, whiteSpace_ != WHITE_SPACE::KEEP )
	, _rule( name_, make_pointer<HRecursiveRule>() )
	, _completelyDefined( false ) {
	return;
}

HRule::HRule( HRule const& rule_ )
	: HRuleBase( rule_._action, rule_._actionPosition, rule_._skipWS )
	, _rule( rule_._rule )
	, _completelyDefined( rule_._completelyDefined ) {
	return;
}

HRule::HRule( HRuleBase const& rule_ )
	: HRuleBase( rule_.skips_ws() )
	, _rule( named_clone( rule_ ) )
	, _completelyDefined( true ) {
	return;
}

HRule::HRule( HRuleBase const& rule_, action_t const& action_ )
	: HRuleBase( action_, rule_.skips_ws() )
	, _rule( named_clone( rule_ ) )
	, _completelyDefined( true ) {
	return;
}

HRule::HRule( HRuleBase const& rule_, action_range_t const& action_ )
	: HRuleBase( action_, rule_.skips_ws() )
	, _rule( named_clone( rule_ ) )
	, _completelyDefined( true ) {
	return;
}

HRule::HRule( ptr_t const& rule_ )
	: HRuleBase( rule_->skips_ws() )
	, _rule( rule_ )
	, _completelyDefined( true ) {
	return;
}

HRule::HRule( yaal::hcore::HString const& name_, HRuleBase const& rule_ )
	: HRuleBase( rule_.skips_ws() )
	, _rule( name_, rule_.clone() )
	, _completelyDefined( true ) {
	return;
}

HRule::HRule( yaal::hcore::HString const& name_, HRuleBase const& rule_, action_t const& action_ )
	: HRuleBase( action_, rule_.skips_ws() )
	, _rule( name_, rule_.clone() )
	, _completelyDefined( true ) {
	return;
}

HRule::HRule( yaal::hcore::HString const& name_, HRuleBase const& rule_, action_range_t const& action_ )
	: HRuleBase( action_, rule_.skips_ws() )
	, _rule( name_, rule_.clone() )
	,_completelyDefined( true ) {
	return;
}

HRule::HRule( yaal::hcore::HString const& name_, ptr_t const& rule_ )
	: HRuleBase( rule_->skips_ws() )
	, _rule( name_, rule_ )
	, _completelyDefined( true ) {
	return;
}

HRule::HRule( yaal::hcore::HString const& name_, ptr_t const& rule_, action_t const& action_ )
	: HRuleBase( action_, rule_->skips_ws() )
	, _rule( name_, rule_ )
	, _completelyDefined( true ) {
	return;
}

HRule::HRule( yaal::hcore::HString const& name_, ptr_t const& rule_, action_range_t const& action_ )
	: HRuleBase( action_, rule_->skips_ws() )
	, _rule( name_, rule_ )
	, _completelyDefined( true ) {
	return;
}

HRule::~HRule( void ) {
	M_PROLOG
	return;
	M_DESTRUCTOR_EPILOG
}

HRule& HRule::operator %= ( HRuleBase const& rule_ ) {
	M_PROLOG
	M_ENSURE( ! _completelyDefined );
	HRecursiveRule* rr( dynamic_cast<HRecursiveRule*>( _rule.rule().raw() ) );
	M_ENSURE( rr );
	rr->set_rule( named_clone( rule_ ) );
	HRuleAggregator rra( this );
	_completelyDefined = true;
	return ( *this );
	M_EPILOG
}

HRule HRule::operator[]( action_t const& action_ ) const {
	M_PROLOG
	M_ENSURE( ! has_action() );
	M_ENSURE( !! _rule );
	return ( HRule( _rule.name(), _rule.rule(), action_ ) );
	M_EPILOG
}

HRule HRule::operator[]( action_range_t const& action_ ) const {
	M_PROLOG
	M_ENSURE( ! has_action() );
	M_ENSURE( !! _rule );
	return ( HRule( _rule.name(), _rule.rule(), action_ ) );
	M_EPILOG
}

yaal::hcore::HString const& HRule::get_name( void ) const {
	M_PROLOG
	return ( _rule.name() );
	M_EPILOG
}

HNamedRule const* HRule::get_named_rule( void ) const {
	M_PROLOG
	return ( &_rule );
	M_EPILOG
}

HRule::ptr_t HRule::do_clone( void ) const {
	M_PROLOG
	M_ENSURE( !! _rule );
	HRule::ptr_t rule;
	if ( ! has_action() ) {
		rule = _rule.rule();
	} else {
		if ( !! _action ) {
			rule = pointer_static_cast<HRuleBase>( make_pointer<HRule>( _rule.name(), _rule.rule(), _action ) );
		} else {
			M_ASSERT( !! _actionPosition );
			rule = pointer_static_cast<HRuleBase>( make_pointer<HRule>( _rule.name(), _rule.rule(), _actionPosition ) );
		}
	}
	return ( rule );
	M_EPILOG
}

yaal::hcore::HUTF8String::const_iterator HRule::do_parse( HExecutingParser* executingParser_, hcore::HUTF8String::const_iterator const& first_, hcore::HUTF8String::const_iterator const& last_ ) const {
	M_PROLOG
	yaal::hcore::HUTF8String::const_iterator scan( !! _rule ? _rule->parse( executingParser_, first_, last_ ) : first_ );
	if ( scan != first_ ) {
		if ( !! _action ) {
			add_execution_step( executingParser_, _action );
		} else if ( !! _actionPosition ) {
			add_execution_step( executingParser_, call( _actionPosition, range( executingParser_, first_, scan ) ) );
		}
	} else {
		scan = first_;
	}
	return ( scan );
	M_EPILOG
}

void HRule::do_detach( HRuleBase const* rule_, visited_t& visited_, bool& detachAll_ ) {
	M_PROLOG
	HRuleBase::ptr_t r( _rule.rule() );
	if ( !! r ) {
		bool detachThis( r.raw() == rule_ );
		if ( detachThis && detachAll_ ) {
			_rule.reset( make_pointer<HRuleRef>( r ) );
		} else {
			if ( detachThis ) {
				detachAll_ = true;
			}
			r->detach( rule_, visited_, detachAll_ );
		}
	}
	return;
	M_EPILOG
}

bool HRule::do_detect_recursion( HRecursionDetector& recursionDetector_, bool skipVisit_ ) const {
	M_PROLOG
	HRuleBase::ptr_t r( _rule.rule() );
	bool reset( false );
	if ( !! r ) {
		reset = r->detect_recursion( recursionDetector_, skipVisit_ );
	}
	return ( reset );
	M_EPILOG
}

void HRule::do_find_recursions( HRuleAggregator& recursions_ ) {
	M_PROLOG
	recursions_.verify( _rule );
	return;
	M_EPILOG
}

bool HRule::do_is_optional( void ) const {
	return ( _rule->is_optional() );
}

void HRule::do_describe( HRuleDescription& rd_, rule_use_t const& ru_ ) const {
	M_PROLOG
	if ( !! _rule ) {
		_rule.describe( rd_, ru_ );
	}
	return;
	M_EPILOG
}

void HRule::do_rule_use( rule_use_t& ruleUse_ ) const {
	M_PROLOG
	int use( ++ ruleUse_[ this ] );
	if ( ( use == 1 ) && !! _rule ) {
		_rule->rule_use( ruleUse_ );
	}
	return;
	M_EPILOG
}

HRuleBase const* HRule::do_find( yaal::hcore::HString const& name_ ) const {
	M_PROLOG
	return ( _rule.name() == name_ ? _rule.rule().raw() : _rule.rule()->find( name_ ) );
	M_EPILOG
}

HRecursiveRule::HRecursiveRule( void )
	: HRuleBase( false )
	, _rule() {
}

void HRecursiveRule::set_rule( HNamedRule const& rule_ ) {
	M_PROLOG
	/*
	 * HRecursiveRule can be set to other HRecursiveRule! :(
	 */
	_rule = rule_;
	visited_t visited;
	bool detachAll( true );
	_rule->detach( this, visited, detachAll );
	return;
	M_EPILOG
}

yaal::hcore::HUTF8String::const_iterator HRecursiveRule::do_parse( HExecutingParser* executingParser_, hcore::HUTF8String::const_iterator const& first_, hcore::HUTF8String::const_iterator const& last_ ) const {
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
	return ( !! _rule ? _rule->is_optional() : false );
	M_EPILOG
}

void HRecursiveRule::do_describe( HRuleDescription& rd_, rule_use_t const& ru_ ) const {
	M_PROLOG
	if ( !! _rule ) {
		_rule.describe( rd_, ru_ );
	}
	return;
	M_EPILOG
}

void HRecursiveRule::do_rule_use( rule_use_t& ruleUse_ ) const {
	M_PROLOG
	int use( ++ ruleUse_[ this ] );
	if ( ( use == 1 ) && !! _rule ) {
		_rule->rule_use( ruleUse_ );
	}
	return;
	M_EPILOG
}

void HRecursiveRule::do_detach( HRuleBase const* rule_, visited_t& visited_, bool& detachAll_ ) {
	M_PROLOG
	/*
	 * HRecursiveRule can be omitted once while detaching!
	 * It is impossible then to perform this test: M_ENSURE( rule_ != this ).
	 */
	if ( !! _rule ) {
		_rule->detach( rule_, visited_, detachAll_ );
	}
	return;
	M_EPILOG
}

bool HRecursiveRule::do_detect_recursion( HRecursionDetector& recursionDetector_, bool skipVisit_ ) const {
	M_PROLOG
	bool reset( false );
	if ( !! _rule ) {
		reset = _rule->detect_recursion( recursionDetector_, skipVisit_ );
	}
	return ( reset );
	M_EPILOG
}

void HRecursiveRule::do_find_recursions( HRuleAggregator& recursions_ ) {
	M_PROLOG
	if ( !! _rule ) {
		_rule->find_recursions( recursions_ );
	}
	return;
	M_EPILOG
}

HRuleBase const* HRecursiveRule::do_find( yaal::hcore::HString const& name_ ) const {
	M_PROLOG
	return ( _rule.name() == name_ ? _rule.rule().raw() : _rule.rule()->find( name_ ) );
	M_EPILOG
}

HRuleRef::HRuleRef( HRuleBase::ptr_t rule_ )
	: HRuleBase( false )
	, _rule( rule_ ) {
	M_ASSERT( !!rule_ );
	return;
}

yaal::hcore::HUTF8String::const_iterator HRuleRef::do_parse( HExecutingParser* executingParser_, hcore::HUTF8String::const_iterator const& first_, hcore::HUTF8String::const_iterator const& last_ ) const {
	M_PROLOG
	HRuleBase::ptr_t r( _rule );
	M_ASSERT( !! r );
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
	M_ASSERT( !! r );
	return ( r->is_optional() );
	M_EPILOG
}

void HRuleRef::do_describe( HRuleDescription&, rule_use_t const& ) const {
	M_PROLOG
	M_ASSERT( 0 && "recursive describe on rule reference" );
#ifdef NDEBUG
	return;
#endif /* #ifdef NDEBUG */
	M_EPILOG
}

void HRuleRef::do_rule_use( rule_use_t& ru_ ) const {
	M_PROLOG
	HRuleBase::ptr_t r( _rule );
	M_ASSERT( !! r );
	r->rule_use( ru_ );
	return;
	M_EPILOG
}

void HRuleRef::do_detach( HRuleBase const* rule_, visited_t& visited_, bool& detachAll_ ) {
	M_PROLOG
	HRuleBase::ptr_t r( _rule );
	M_ASSERT( !! r );
	if ( r.raw() != rule_ ) {
		r->detach( rule_, visited_, detachAll_ );
	}
	return;
	M_EPILOG
}

bool HRuleRef::do_detect_recursion( HRecursionDetector& recursionDetector_, bool skipVisit_ ) const {
	M_PROLOG
	HRuleBase::ptr_t r( _rule );
	M_ASSERT( !! r );
	return ( r->detect_recursion( recursionDetector_, skipVisit_ ) );
	M_EPILOG
}

void HRuleRef::do_find_recursions( HRuleAggregator& recursions_ ) {
	M_PROLOG
	HRuleBase::ptr_t r( _rule );
	M_ASSERT( !! r );
	r->find_recursions( recursions_ );
	return;
	M_EPILOG
}

HRuleBase::ptr_t HRuleRef::get_rule( void ) const {
	M_PROLOG
	HRuleBase::ptr_t r( _rule );
	return ( r );
	M_EPILOG
}

HFollows::HFollows( HRuleBase const& predecessor_, HRuleBase const& successor_ )
	: HRuleBase( predecessor_.skips_ws() )
	, _rules()
	, _optional( false ) {
	M_PROLOG
	_rules.push_back( predecessor_ );
	_rules.push_back( successor_ );
	HRuleAggregator rra( this );
	count_optional();
	return;
	M_EPILOG
}

HFollows::HFollows( HFollows const& follows_ )
	: HRuleBase( follows_._action, follows_._actionPosition, follows_.skips_ws() )
	, _rules( follows_._rules )
	, _optional( follows_._optional ) {
	M_PROLOG
	return;
	M_EPILOG
}

HFollows::HFollows( HFollows const& predecessors_, HRuleBase const& successor_ )
	: HRuleBase( predecessors_.skips_ws() )
	, _rules()
	, _optional( false ) {
	M_PROLOG
	if ( ! predecessors_._action && ! predecessors_._actionPosition ) {
		for ( rules_t::const_iterator it( predecessors_._rules.begin() ), end( predecessors_._rules.end() ); it != end; ++ it ) {
			_rules.push_back( *it );
		}
	} else {
		_rules.push_back( predecessors_ );
	}
	_rules.push_back( successor_ );
	HRuleAggregator rra( this );
	count_optional();
	return;
	M_EPILOG
}

HFollows::HFollows( HFollows::rules_t const& rules_, action_t const& action_, bool skipWS_ )
	: HRuleBase( action_, skipWS_ )
	, _rules( rules_ )
	, _optional( false ) {
	M_PROLOG
	count_optional();
	return;
	M_EPILOG
}

HFollows::HFollows( HFollows::rules_t const& rules_, action_range_t const& action_, bool skipWS_ )
	: HRuleBase( action_, skipWS_ )
	, _rules( rules_ )
	, _optional( false ) {
	M_PROLOG
	count_optional();
	return;
	M_EPILOG
}

HFollows HFollows::operator[]( action_t const& action_ ) const {
	M_PROLOG
	M_ENSURE( ! has_action() );
	return ( HFollows( _rules, action_, _skipWS ) );
	M_EPILOG
}

HFollows HFollows::operator[]( action_range_t const& action_ ) const {
	M_PROLOG
	M_ENSURE( ! has_action() );
	return ( HFollows( _rules, action_, _skipWS ) );
	M_EPILOG
}

bool HFollows::do_is_optional( void ) const {
	return ( _optional );
}

void HFollows::count_optional( void ) {
	M_PROLOG
	_optional = true;
	for ( rules_t::value_type const& r : _rules ) {
		if ( ! r->is_optional() ) {
			_optional = false;
			break;
		}
	}
	return;
	M_EPILOG
}

HRuleBase::ptr_t HFollows::do_clone( void ) const {
	M_PROLOG
	return ( make_pointer<HFollows>( *this ) );
	M_EPILOG
}

void HFollows::do_describe( HRuleDescription& rd_, rule_use_t const& ru_ ) const {
	M_PROLOG
	bool next( false );
	rd_.desc( "( " );
	for ( rules_t::const_iterator it( _rules.begin() ), end( _rules.end() ); it != end; ++ it ) {
		if ( next ) {
			rd_.desc( " >> " );
		}
		it->describe( rd_, ru_ );
		next = true;
	}
	rd_.desc( " )" );
	return;
	M_EPILOG
}

void HFollows::do_rule_use( rule_use_t& ruleUse_ ) const {
	M_PROLOG
	int use( ++ ruleUse_[ this ] );
	if ( use == 1 ) {
		for ( rules_t::const_iterator it( _rules.begin() ), end( _rules.end() ); it != end; ++ it ) {
			(*it)->rule_use( ruleUse_ );
		}
	}
	return;
	M_EPILOG
}

yaal::hcore::HUTF8String::const_iterator HFollows::do_parse( HExecutingParser* executingParser_, hcore::HUTF8String::const_iterator const& first_, hcore::HUTF8String::const_iterator const& last_ ) const {
	M_PROLOG
	yaal::hcore::HUTF8String::const_iterator scan( first_ );
	bool matched( true );
	for ( rules_t::const_iterator it( _rules.begin() ), end( _rules.end() ); it != end; ++ it ) {
		yaal::hcore::HUTF8String::const_iterator old( scan );
		scan = (*it)->parse( executingParser_, scan, last_ );
		M_ASSERT( scan != HUTF8String::const_iterator() );
		if ( ( scan == old ) && ( !(*it)->is_optional() ) ) {
			matched = false;
			break;
		}
	}
	if ( matched ) {
		if ( !! _action ) {
			add_execution_step( executingParser_, _action );
		} else if ( !! _actionPosition ) {
			add_execution_step( executingParser_, call( _actionPosition, range( executingParser_, first_, scan ) ) );
		}
	} else {
		scan = first_;
	}
	return ( scan );
	M_EPILOG
}

void HFollows::do_detach( HRuleBase const* rule_, visited_t& visited_, bool& detachAll_ ) {
	M_PROLOG
	for ( rules_t::iterator it( _rules.begin() ), end( _rules.end() ); it != end; ++ it ) {
		HRuleBase::ptr_t r( it->rule() );
		if ( !! r ) {
			bool detachThis( r.raw() == rule_ );
			if ( detachThis && detachAll_ ) {
				it->reset( make_pointer<HRuleRef>( r ) );
			} else {
				if ( detachThis ) {
					detachAll_ = true;
				}
				r->detach( rule_, visited_, detachAll_ );
			}
		}
	}
	return;
	M_EPILOG
}

bool HFollows::do_detect_recursion( HRecursionDetector& recursionDetector_, bool skipVisit_ ) const {
	M_PROLOG
	bool reset( false );
	for (
		rules_t::const_iterator it( _rules.begin() ), end( _rules.end() );
		! ( reset && skipVisit_ ) && ( it != end );
		++ it
	) {
		HRuleBase::ptr_t r( it->rule() );
		if ( !! r ) {
			reset = r->detect_recursion( recursionDetector_, skipVisit_ );
			if ( reset ) {
				recursionDetector_.reset_visits();
			}
		}
	}
	return ( reset );
	M_EPILOG
}

void HFollows::do_find_recursions( HRuleAggregator& recursions_ ) {
	M_PROLOG
	for ( rules_t::iterator it( _rules.begin() ), end( _rules.end() ); it != end; ++ it ) {
		recursions_.verify( *it );
	}
	return;
	M_EPILOG
}

HRuleBase const* HFollows::do_find( yaal::hcore::HString const& name_ ) const {
	M_PROLOG
	HRuleBase const* rule( nullptr );
	for ( rules_t::const_iterator it( _rules.begin() ), end( _rules.end() ); ! rule && ( it != end ); ++ it ) {
		rule = it->rule()->find( name_ );
	}
	return ( rule );
	M_EPILOG
}

HKleeneBase::HKleeneBase( HRuleBase const& rule_ )
	: HRuleBase( rule_.skips_ws() )
	, _rule( rule_ ) {
	return;
}

HKleeneBase::HKleeneBase( HNamedRule const& rule_, action_t const& action_ )
	: HRuleBase( action_, rule_.rule()->skips_ws() )
	, _rule( rule_ ) {
	return;
}

HKleeneBase::HKleeneBase( HNamedRule const& rule_, action_range_t const& action_ )
	: HRuleBase( action_, rule_.rule()->skips_ws() )
	, _rule( rule_ ) {
	return;
}

HKleeneBase::HKleeneBase( HNamedRule const& rule_, action_t const& action_, action_range_t const& actionPosition_ )
	: HRuleBase( action_, actionPosition_, rule_.rule()->skips_ws() )
	, _rule( rule_ ) {
	return;
}

yaal::hcore::HUTF8String::const_iterator HKleeneBase::do_parse( HExecutingParser* executingParser_, hcore::HUTF8String::const_iterator const& first_, hcore::HUTF8String::const_iterator const& last_ ) const {
	M_PROLOG
	yaal::hcore::HUTF8String::const_iterator scan( first_ );
	yaal::hcore::HUTF8String::const_iterator oldScan( scan );
	while ( scan != last_ ) {
		int stepCount( execution_step_count( executingParser_ ) );
		scan = _rule->parse( executingParser_, scan, last_ );
		if ( scan == oldScan ) {
			drop_execution_steps( executingParser_, stepCount );
			break;
		}
		oldScan = scan;
	}
	if ( scan != first_ ) {
		if ( !! _action ) {
			add_execution_step( executingParser_, _action );
		} else if ( !! _actionPosition ) {
			add_execution_step( executingParser_, call( _actionPosition, range( executingParser_, first_, scan ) ) );
		}
	} else {
		scan = first_;
	}
	return ( scan );
	M_EPILOG
}

void HKleeneBase::do_rule_use( rule_use_t& ruleUse_ ) const {
	M_PROLOG
	int use( ++ ruleUse_[ this ] );
	if ( use == 1 ) {
		_rule->rule_use( ruleUse_ );
	}
	return;
	M_EPILOG
}

void HKleeneBase::do_detach( HRuleBase const* rule_, visited_t& visited_, bool& detachAll_ ) {
	M_PROLOG
	HRuleBase::ptr_t r( _rule.rule() );
	if ( !! r ) {
		bool detachThis( r.raw() == rule_ );
		if ( detachThis && detachAll_ ) {
			_rule.reset( make_pointer<HRuleRef>( r ) );
		} else {
			if ( detachThis ) {
				detachAll_ = true;
			}
			r->detach( rule_, visited_, detachAll_ );
		}
	}
	return;
	M_EPILOG
}

void HKleeneBase::do_find_recursions( HRuleAggregator& recursions_ ) {
	M_PROLOG
	recursions_.verify( _rule );
	return;
	M_EPILOG
}

HRuleBase const* HKleeneBase::do_find( yaal::hcore::HString const& name_ ) const {
	M_PROLOG
	return ( _rule.name() == name_ ? _rule.rule().raw() : _rule.rule()->find( name_ ) );
	M_EPILOG
}

HKleeneStar::HKleeneStar( HRuleBase const& rule_ )
	: HKleeneBase( rule_ ) {
	return;
}

HKleeneStar::HKleeneStar( HNamedRule const& rule_, action_t const& action_ )
	: HKleeneBase( rule_, action_ ) {
	return;
}

HKleeneStar::HKleeneStar( HNamedRule const& rule_, action_range_t const& action_ )
	: HKleeneBase( rule_, action_ ) {
	return;
}

HKleeneStar::HKleeneStar( HKleeneStar const& kleeneStar_ )
	: HKleeneBase( kleeneStar_._rule, kleeneStar_._action, kleeneStar_._actionPosition ) {
	return;
}

HKleeneStar HKleeneStar::operator[]( action_t const& action_ ) const {
	M_PROLOG
	M_ENSURE( ! has_action() );
	return ( HKleeneStar( _rule, action_ ) );
	M_EPILOG
}

HKleeneStar HKleeneStar::operator[]( action_range_t const& action_ ) const {
	M_PROLOG
	M_ENSURE( ! has_action() );
	return ( HKleeneStar( _rule, action_ ) );
	M_EPILOG
}

HRuleBase::ptr_t HKleeneStar::do_clone( void ) const {
	M_PROLOG
	return ( make_pointer<HKleeneStar>( *this ) );
	M_EPILOG
}

bool HKleeneStar::do_is_optional( void ) const {
	return ( true );
}

void HKleeneStar::do_describe( HRuleDescription& rd_, rule_use_t const& ru_ ) const {
	M_PROLOG
	rd_.desc( "*" );
	_rule.describe( rd_, ru_ );
	return;
	M_EPILOG
}

bool HKleeneStar::do_detect_recursion( HRecursionDetector& recursionDetector_, bool skipVisit_ ) const {
	M_PROLOG
	if ( ! skipVisit_ ) {
		HRuleBase::ptr_t r( _rule.rule() );
		if ( !! r ) {
			recursionDetector_.checkpoints_push();
			r->detect_recursion( recursionDetector_, skipVisit_ );
			recursionDetector_.checkpoints_pop();
		}
	}
	return ( false );
	M_EPILOG
}

HKleenePlus::HKleenePlus( HRuleBase const& rule_ )
	: HKleeneBase( rule_ ) {
	return;
}

HKleenePlus::HKleenePlus( HNamedRule const& rule_, action_t const& action_ )
	: HKleeneBase( rule_, action_ ) {
	return;
}

HKleenePlus::HKleenePlus( HNamedRule const& rule_, action_range_t const& action_ )
	: HKleeneBase( rule_, action_ ) {
	return;
}

HKleenePlus HKleenePlus::operator[]( action_t const& action_ ) const {
	M_PROLOG
	M_ENSURE( ! has_action() );
	return ( HKleenePlus( _rule, action_ ) );
	M_EPILOG
}

HKleenePlus HKleenePlus::operator[]( action_range_t const& action_ ) const {
	M_PROLOG
	M_ENSURE( ! has_action() );
	return ( HKleenePlus( _rule, action_ ) );
	M_EPILOG
}

HKleenePlus::HKleenePlus( HKleenePlus const& kleenePlus_ )
	: HKleeneBase( kleenePlus_._rule, kleenePlus_._action, kleenePlus_._actionPosition ) {
	return;
}

HRuleBase::ptr_t HKleenePlus::do_clone( void ) const {
	M_PROLOG
	return ( make_pointer<HKleenePlus>( *this ) );
	M_EPILOG
}

void HKleenePlus::do_describe( HRuleDescription& rd_, rule_use_t const& ru_ ) const {
	M_PROLOG
	rd_.desc( "+" );
	_rule.describe( rd_, ru_ );
	return;
	M_EPILOG
}

bool HKleenePlus::do_detect_recursion( HRecursionDetector& recursionDetector_, bool skipVisit_ ) const {
	M_PROLOG
	HRuleBase::ptr_t r( _rule.rule() );
	bool reset( false );
	if ( !! r ) {
		reset = r->detect_recursion( recursionDetector_, skipVisit_ );
	}
	return ( reset );
	M_EPILOG
}

HAlternative::HAlternative( HRuleBase const& choice1_, HRuleBase const& choice2_ )
	: HRuleBase( choice1_.skips_ws() && choice2_.skips_ws() )
	, _rules()
	, _optional( false ) {
	M_PROLOG
	_rules.push_back( choice1_ );
	_rules.push_back( choice2_ );
	HRuleAggregator rra( this );
	count_optional();
	return;
	M_EPILOG
}

HAlternative::HAlternative( HAlternative const& alternative_ )
	: HRuleBase( alternative_._action, alternative_._actionPosition, alternative_._skipWS )
	, _rules( alternative_._rules )
	, _optional( alternative_._optional ) {
	M_PROLOG
	return;
	M_EPILOG
}

HAlternative::HAlternative( HAlternative const& alternative_, HRuleBase const& choice_ )
	: HRuleBase( alternative_.skips_ws() && choice_.skips_ws() )
	, _rules()
	, _optional( false ) {
	M_PROLOG
	if ( ! alternative_._action && ! alternative_._actionPosition ) {
		for ( rules_t::const_iterator it( alternative_._rules.begin() ), end( alternative_._rules.end() ); it != end; ++ it ) {
			_rules.push_back( *it );
		}
	} else {
		_rules.push_back( alternative_ );
	}
	_rules.push_back( choice_ );
	HRuleAggregator rra( this );
	count_optional();
	return;
	M_EPILOG
}

HAlternative::HAlternative( rules_t const& rules_, action_t const& action_, bool skipWS_ )
	: HRuleBase( action_, skipWS_ )
	, _rules( rules_ )
	, _optional( false ) {
	M_PROLOG
	count_optional();
	return;
	M_EPILOG
}

HAlternative::HAlternative( rules_t const& rules_, action_range_t const& action_, bool skipWS_ )
	: HRuleBase( action_, skipWS_ )
	, _rules( rules_ )
	, _optional( false ) {
	M_PROLOG
	count_optional();
	return;
	M_EPILOG
}

void HAlternative::count_optional( void ) {
	M_PROLOG
	_optional = false;
	for ( rules_t::value_type const& r : _rules ) {
		if ( r->is_optional() ) {
			_optional = true;
			break;
		}
	}
	return;
	M_EPILOG
}

bool HAlternative::do_is_optional( void ) const {
	return ( _optional );
}

HRuleBase::ptr_t HAlternative::do_clone( void ) const {
	M_PROLOG
	return ( make_pointer<HAlternative>( *this ) );
	M_EPILOG
}

HAlternative HAlternative::operator[]( action_t const& action_ ) const {
	M_PROLOG
	M_ENSURE( ! has_action() );
	return ( HAlternative( _rules, action_, _skipWS ) );
	M_EPILOG
}

HAlternative HAlternative::operator[]( action_range_t const& action_ ) const {
	M_PROLOG
	M_ENSURE( ! has_action() );
	return ( HAlternative( _rules, action_, _skipWS ) );
	M_EPILOG
}

yaal::hcore::HUTF8String::const_iterator HAlternative::do_parse( HExecutingParser* executingParser_, hcore::HUTF8String::const_iterator const& first_, hcore::HUTF8String::const_iterator const& last_ ) const {
	yaal::hcore::HUTF8String::const_iterator scan( first_ );
	int stepCount( execution_step_count( executingParser_ ) );
	for ( rules_t::const_iterator it( _rules.begin() ), end( _rules.end() ); it != end; ++ it ) {
		if ( ( scan = (*it)->parse( executingParser_, first_, last_ ) ) != first_ ) {
			break;
		} else {
			drop_execution_steps( executingParser_, stepCount );
		}
	}
	if ( scan != first_ ) {
		if ( !! _action ) {
			add_execution_step( executingParser_, _action );
		} else if ( !! _actionPosition ) {
			add_execution_step( executingParser_, call( _actionPosition, range( executingParser_, first_, scan ) ) );
		}
	} else {
		scan = first_;
	}
	return ( scan );
}

void HAlternative::do_describe( HRuleDescription& rd_, rule_use_t const& ru_ ) const {
	M_PROLOG
	bool next( false );
	rd_.desc( "( " );
	for ( rules_t::const_iterator it( _rules.begin() ), end( _rules.end() ); it != end; ++ it ) {
		if ( next ) {
			rd_.desc( " | " );
		}
		it->describe( rd_, ru_ );
		next = true;
	}
	rd_.desc( " )" );
	return;
	M_EPILOG
}

void HAlternative::do_rule_use( rule_use_t& ruleUse_ ) const {
	M_PROLOG
	int use( ++ ruleUse_[ this ] );
	if ( use == 1 ) {
		for ( rules_t::const_iterator it( _rules.begin() ), end( _rules.end() ); it != end; ++ it ) {
			(*it)->rule_use( ruleUse_ );
		}
	}
	return;
	M_EPILOG
}

void HAlternative::do_detach( HRuleBase const* rule_, visited_t& visited_, bool& detachAll_ ) {
	M_PROLOG
	for ( rules_t::iterator it( _rules.begin() ), end( _rules.end() ); it != end; ++ it ) {
		HRuleBase::ptr_t r( it->rule() );
		if ( !! r ) {
			bool detachThis( r.raw() == rule_ );
			if ( detachThis && detachAll_ ) {
				it->reset( make_pointer<HRuleRef>( r ) );
			} else {
				if ( detachThis ) {
					detachAll_ = true;
				}
				r->detach( rule_, visited_, detachAll_ );
			}
		}
	}
	return;
	M_EPILOG
}

bool HAlternative::do_detect_recursion( HRecursionDetector& recursionDetector_, bool skipVisit_ ) const {
	M_PROLOG
	bool reset( false );
	if ( ! skipVisit_ ) {
		reset = true;
		for ( rules_t::const_iterator it( _rules.begin() ), end( _rules.end() ); it != end; ++ it ) {
			HRuleBase::ptr_t r( it->rule() );
			if ( !! r ) {
				recursionDetector_.checkpoints_push();
				if ( ! r->detect_recursion( recursionDetector_, skipVisit_ ) ) {
					reset = false;
				}
				recursionDetector_.checkpoints_pop();
			}
		}
		if ( reset ) {
			recursionDetector_.reset_visits();
		}
	}
	return ( reset || skipVisit_ );
	M_EPILOG
}

void HAlternative::do_find_recursions( HRuleAggregator& recursions_ ) {
	M_PROLOG
	for ( rules_t::iterator it( _rules.begin() ), end( _rules.end() ); it != end; ++ it ) {
		recursions_.verify( *it );
	}
	return;
	M_EPILOG
}

HRuleBase const* HAlternative::do_find( yaal::hcore::HString const& name_ ) const {
	M_PROLOG
	HRuleBase const* rule( nullptr );
	for ( rules_t::const_iterator it( _rules.begin() ), end( _rules.end() ); ! rule && ( it != end ); ++ it ) {
		rule = it->rule()->find( name_ );
	}
	return ( rule );
	M_EPILOG
}

HOptional::HOptional( HRuleBase const& rule_ )
	: HRuleBase( rule_.skips_ws() )
	, _rule( rule_ ) {
	return;
}

HOptional::HOptional( HOptional const& optional_ )
	: HRuleBase( optional_._action, optional_._actionPosition, optional_._skipWS )
	, _rule( optional_._rule ) {
	return;
}

HOptional::HOptional( HNamedRule const& rule_, action_t const& action_ )
	: HRuleBase( action_, rule_.rule()->skips_ws() )
	, _rule( rule_ ) {
	return;
}

HOptional::HOptional( HNamedRule const& rule_, action_range_t const& action_ )
	: HRuleBase( action_, rule_.rule()->skips_ws() )
	, _rule( rule_ ) {
	return;
}

HRuleBase::ptr_t HOptional::do_clone( void ) const {
	M_PROLOG
	return ( make_pointer<HOptional>( *this ) );
	M_EPILOG
}

HOptional HOptional::operator[]( action_t const& action_ ) const {
	M_PROLOG
	M_ENSURE( ! has_action() );
	return ( HOptional( _rule, action_ ) );
	M_EPILOG
}

HOptional HOptional::operator[]( action_range_t const& action_ ) const {
	M_PROLOG
	M_ENSURE( ! has_action() );
	return ( HOptional( _rule, action_ ) );
	M_EPILOG
}

yaal::hcore::HUTF8String::const_iterator HOptional::do_parse( HExecutingParser* executingParser_, hcore::HUTF8String::const_iterator const& first_, hcore::HUTF8String::const_iterator const& last_ ) const {
	M_PROLOG
	yaal::hcore::HUTF8String::const_iterator scan( !! _rule ? _rule->parse( executingParser_, first_, last_ ) : first_ );
	if ( scan != first_ ) {
		if ( !! _action ) {
			add_execution_step( executingParser_, _action );
		} else if ( !! _actionPosition ) {
			add_execution_step( executingParser_, call( _actionPosition, range( executingParser_, first_, scan ) ) );
		}
	} else {
		scan = first_;
	}
	return ( scan );
	M_EPILOG
}

bool HOptional::do_is_optional( void ) const {
	return ( true );
}

void HOptional::do_describe( HRuleDescription& rd_, rule_use_t const& ru_ ) const {
	M_PROLOG
	rd_.desc( "-" );
	_rule.describe( rd_, ru_ );
	return;
	M_EPILOG
}

void HOptional::do_rule_use( rule_use_t& ruleUse_ ) const {
	M_PROLOG
	int use( ++ ruleUse_[ this ] );
	if ( use == 1 ) {
		_rule->rule_use( ruleUse_ );
	}
	return;
	M_EPILOG
}

void HOptional::do_detach( HRuleBase const* rule_, visited_t& visited_, bool& detachAll_ ) {
	M_PROLOG
	HRuleBase::ptr_t r( _rule.rule() );
	if ( !! r ) {
		bool detachThis( r.raw() == rule_ );
		if ( detachThis && detachAll_ ) {
			_rule.reset( make_pointer<HRuleRef>( r ) );
		} else {
			if ( detachThis ) {
				detachAll_ = true;
			}
			r->detach( rule_, visited_, detachAll_ );
		}
	}
	return;
	M_EPILOG
}

bool HOptional::do_detect_recursion( HRecursionDetector& recursionDetector_, bool skipVisit_ ) const {
	M_PROLOG
	if ( ! skipVisit_ ) {
		HRuleBase::ptr_t r( _rule.rule() );
		if ( !! r ) {
			recursionDetector_.checkpoints_push();
			r->detect_recursion( recursionDetector_, skipVisit_ );
			recursionDetector_.checkpoints_pop();
		}
	}
	return ( false );
	M_EPILOG
}

HRuleBase const* HOptional::do_find( yaal::hcore::HString const& name_ ) const {
	M_PROLOG
	return ( _rule.name() == name_ ? _rule.rule().raw() : _rule.rule()->find( name_ ) );
	M_EPILOG
}

void HOptional::do_find_recursions( HRuleAggregator& recursions_ ) {
	M_PROLOG
	recursions_.verify( _rule );
	return;
	M_EPILOG
}

HAnd::HAnd( HRuleBase const& rule_, HRuleBase const& and_ )
	: HRuleBase( rule_.skips_ws() )
	, _rule( rule_ )
	, _and( and_ ) {
	return;
}

HAnd::HAnd( HAnd const& and_ )
	: HRuleBase( and_._action, and_._actionPosition, and_._skipWS )
	, _rule( and_._rule )
	, _and( and_._and ) {
	return;
}

HAnd::HAnd( HNamedRule const& rule_, HNamedRule const& and_, action_t const& action_ )
	: HRuleBase( action_, rule_.rule()->skips_ws() )
	, _rule( rule_ )
	, _and( and_ ) {
	return;
}

HAnd::HAnd( HNamedRule const& rule_, HNamedRule const& and_, action_range_t const& action_ )
	: HRuleBase( action_, rule_.rule()->skips_ws() )
	, _rule( rule_ )
	, _and( and_ ) {
	return;
}

HRuleBase::ptr_t HAnd::do_clone( void ) const {
	M_PROLOG
	return ( make_pointer<HAnd>( *this ) );
	M_EPILOG
}

HAnd HAnd::operator[]( action_t const& action_ ) const {
	M_PROLOG
	M_ENSURE( ! has_action() );
	return ( HAnd( _rule, _and, action_ ) );
	M_EPILOG
}

HAnd HAnd::operator[]( action_range_t const& action_ ) const {
	M_PROLOG
	M_ENSURE( ! has_action() );
	return ( HAnd( _rule, _and, action_ ) );
	M_EPILOG
}

yaal::hcore::HUTF8String::const_iterator HAnd::do_parse( HExecutingParser* executingParser_, hcore::HUTF8String::const_iterator const& first_, hcore::HUTF8String::const_iterator const& last_ ) const {
	M_PROLOG
	yaal::hcore::HUTF8String::const_iterator scan( !! _rule ? _rule->parse( executingParser_, first_, last_ ) : first_ );
	int stepCount( execution_step_count( executingParser_ ) );
	yaal::hcore::HUTF8String::const_iterator andScan( !! _and ? _and->parse( executingParser_, scan, last_ ) : scan );
	if ( ( scan != first_ ) && ( andScan != scan ) ) {
		drop_execution_steps( executingParser_, stepCount );
		if ( !! _action ) {
			add_execution_step( executingParser_, _action );
		} else if ( !! _actionPosition ) {
			add_execution_step( executingParser_, call( _actionPosition, range( executingParser_, first_, scan ) ) );
		}
	} else {
		scan = first_;
	}
	return ( scan );
	M_EPILOG
}

void HAnd::do_describe( HRuleDescription& rd_, rule_use_t const& ru_ ) const {
	M_PROLOG
	rd_.desc( "( " );
	_rule.describe( rd_, ru_ );
	rd_.desc( " & " );
	_and.describe( rd_, ru_ );
	rd_.desc( " )" );
	return;
	M_EPILOG
}

void HAnd::do_rule_use( rule_use_t& ruleUse_ ) const {
	M_PROLOG
	int use( ++ ruleUse_[ this ] );
	if ( use == 1 ) {
		_rule->rule_use( ruleUse_ );
		_and->rule_use( ruleUse_ );
	}
	return;
	M_EPILOG
}

void HAnd::do_detach( HRuleBase const* rule_, visited_t& visited_, bool& detachAll_ ) {
	M_PROLOG
	HNamedRule* rules[2] = {
		&_rule,
		&_and
	};
	for ( HNamedRule* rule : rules ) {
		HRuleBase::ptr_t r( rule->rule() );
		if ( !! r ) {
			bool detachThis( r.raw() == rule_ );
			if ( detachThis && detachAll_ ) {
				rule->reset( make_pointer<HRuleRef>( r ) );
			} else {
				if ( detachThis ) {
					detachAll_ = true;
				}
				r->detach( rule_, visited_, detachAll_ );
			}
		}
	}
	return;
	M_EPILOG
}

bool HAnd::do_detect_recursion( HRecursionDetector& recursionDetector_, bool skipVisit_ ) const {
	M_PROLOG
	bool reset( false );
	HRuleBase::ptr_t r( _rule.rule() );
	if ( !! r ) {
		reset = r->detect_recursion( recursionDetector_, skipVisit_ );
	}
	r = _and.rule();
	if ( !! r ) {
		recursionDetector_.checkpoints_push();
		r->detect_recursion( recursionDetector_, skipVisit_ );
		recursionDetector_.checkpoints_pop();
	}
	return ( reset );
	M_EPILOG
}

void HAnd::do_find_recursions( HRuleAggregator& recursions_ ) {
	M_PROLOG
	recursions_.verify( _rule );
	recursions_.verify( _and );
	return;
	M_EPILOG
}

HRuleBase const* HAnd::do_find( yaal::hcore::HString const& name_ ) const {
	M_PROLOG
	return ( _rule.name() == name_ ? _rule.rule().raw() : _rule.rule()->find( name_ ) );
	M_EPILOG
}

HNot::HNot( HRuleBase const& rule_, HRuleBase const& not_ )
	: HRuleBase( rule_.skips_ws() )
	, _rule( rule_ )
	, _not( not_ ) {
	return;
}

HNot::HNot( HNot const& not_ )
	: HRuleBase( not_._action, not_._actionPosition, not_._skipWS )
	, _rule( not_._rule )
	, _not( not_._not ) {
	return;
}

HNot::HNot( HNamedRule const& rule_, HNamedRule const& not_, action_t const& action_ )
	: HRuleBase( action_, rule_.rule()->skips_ws() )
	, _rule( rule_ )
	, _not( not_ ) {
	return;
}

HNot::HNot( HNamedRule const& rule_, HNamedRule const& not_, action_range_t const& action_ )
	: HRuleBase( action_, rule_.rule()->skips_ws() )
	, _rule( rule_ )
	, _not( not_ ) {
	return;
}

HRuleBase::ptr_t HNot::do_clone( void ) const {
	M_PROLOG
	return ( make_pointer<HNot>( *this ) );
	M_EPILOG
}

HNot HNot::operator[]( action_t const& action_ ) const {
	M_PROLOG
	M_ENSURE( ! has_action() );
	return ( HNot( _rule, _not, action_ ) );
	M_EPILOG
}

HNot HNot::operator[]( action_range_t const& action_ ) const {
	M_PROLOG
	M_ENSURE( ! has_action() );
	return ( HNot( _rule, _not, action_ ) );
	M_EPILOG
}

yaal::hcore::HUTF8String::const_iterator HNot::do_parse( HExecutingParser* executingParser_, hcore::HUTF8String::const_iterator const& first_, hcore::HUTF8String::const_iterator const& last_ ) const {
	M_PROLOG
	yaal::hcore::HUTF8String::const_iterator scan( !! _rule ? _rule->parse( executingParser_, first_, last_ ) : first_ );
	yaal::hcore::HUTF8String::const_iterator notScan( !! _not ? _not->parse( executingParser_, scan, last_ ) : scan );
	if ( ( scan != first_ ) && ( notScan == scan ) ) {
		if ( !! _action ) {
			add_execution_step( executingParser_, _action );
		} else if ( !! _actionPosition ) {
			add_execution_step( executingParser_, call( _actionPosition, range( executingParser_, first_, scan ) ) );
		}
	} else {
		if ( scan != first_ ) {
			report_error( executingParser_, scan, "NOT's follower matched" );
		}
		scan = first_;
	}
	return ( scan );
	M_EPILOG
}

void HNot::do_describe( HRuleDescription& rd_, rule_use_t const& ru_ ) const {
	M_PROLOG
	rd_.desc( "( " );
	_rule.describe( rd_, ru_ );
	rd_.desc( " ^ " );
	_not.describe( rd_, ru_ );
	rd_.desc( " )" );
	return;
	M_EPILOG
}

void HNot::do_rule_use( rule_use_t& ruleUse_ ) const {
	M_PROLOG
	int use( ++ ruleUse_[ this ] );
	if ( use == 1 ) {
		_rule->rule_use( ruleUse_ );
		_not->rule_use( ruleUse_ );
	}
	return;
	M_EPILOG
}

void HNot::do_detach( HRuleBase const* rule_, visited_t& visited_, bool& detachAll_ ) {
	M_PROLOG
	HNamedRule* rules[2] = {
		&_rule,
		&_not
	};
	for ( HNamedRule* rule : rules ) {
		HRuleBase::ptr_t r( rule->rule() );
		if ( !! r ) {
			bool detachThis( r.raw() == rule_ );
			if ( detachThis && detachAll_ ) {
				rule->reset( make_pointer<HRuleRef>( r ) );
			} else {
				if ( detachThis ) {
					detachAll_ = true;
				}
				r->detach( rule_, visited_, detachAll_ );
			}
		}
	}
	return;
	M_EPILOG
}

bool HNot::do_detect_recursion( HRecursionDetector& recursionDetector_, bool skipVisit_ ) const {
	M_PROLOG
	bool reset( false );
	HRuleBase::ptr_t r( _rule.rule() );
	if ( !! r ) {
		reset = r->detect_recursion( recursionDetector_, skipVisit_ );
	}
	r = _not.rule();
	if ( !! r ) {
		recursionDetector_.checkpoints_push();
		r->detect_recursion( recursionDetector_, skipVisit_ );
		recursionDetector_.checkpoints_pop();
	}
	return ( reset );
	M_EPILOG
}

void HNot::do_find_recursions( HRuleAggregator& recursions_ ) {
	M_PROLOG
	recursions_.verify( _rule );
	recursions_.verify( _not );
	return;
	M_EPILOG
}

HRuleBase const* HNot::do_find( yaal::hcore::HString const& name_ ) const {
	M_PROLOG
	return ( _rule.name() == name_ ? _rule.rule().raw() : _rule.rule()->find( name_ ) );
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

HCharacter operator | ( HCharacter const& character_, code_point_t const& token_ ) {
	M_PROLOG
	return ( HCharacter( character_, token_ ) );
	M_EPILOG
}

HString operator | ( HString const& string_, yaal::hcore::HString const& token_ ) {
	M_PROLOG
	return ( HString( string_, token_ ) );
	M_EPILOG
}

HAnd operator & ( HRuleBase const& rule_, HRuleBase const& and_ ) {
	M_PROLOG
	return ( HAnd( rule_, and_ ) );
	M_EPILOG
}

HAnd operator & ( HRuleBase const& rule_, char character_ ) {
	M_PROLOG
	return ( HAnd( rule_, character( character_ ) ) );
	M_EPILOG
}

HAnd operator & ( HRuleBase const& rule_, char const* string_ ) {
	M_PROLOG
	return ( HAnd( rule_, string( string_ ) ) );
	M_EPILOG
}

HAnd operator & ( HRuleBase const& rule_, yaal::hcore::HString const& string_ ) {
	M_PROLOG
	return ( HAnd( rule_, string( string_ ) ) );
	M_EPILOG
}

HNot operator ^ ( HRuleBase const& rule_, HRuleBase const& not_ ) {
	M_PROLOG
	return ( HNot( rule_, not_ ) );
	M_EPILOG
}

HNot operator ^ ( HRuleBase const& rule_, char character_ ) {
	M_PROLOG
	return ( HNot( rule_, character( character_ ) ) );
	M_EPILOG
}

HNot operator ^ ( HRuleBase const& rule_, char const* string_ ) {
	M_PROLOG
	return ( HNot( rule_, string( string_ ) ) );
	M_EPILOG
}

HNot operator ^ ( HRuleBase const& rule_, yaal::hcore::HString const& string_ ) {
	M_PROLOG
	return ( HNot( rule_, string( string_ ) ) );
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

HReal::HReal( PARSE parse_ )
	: HRuleBase( true )
	, _actionDouble()
	, _actionDoublePosition()
	, _actionDoubleLong()
	, _actionDoubleLongPosition()
	, _actionNumber()
	, _actionNumberPosition()
	, _actionString()
	, _actionStringPosition()
	, _parse( parse_ )
	, _stringCache()
	, _cache() {
	return;
}

HReal::HReal( action_t const& action_, PARSE parse_ )
	: HRuleBase( action_, true )
	, _actionDouble()
	, _actionDoublePosition()
	, _actionDoubleLong()
	, _actionDoubleLongPosition()
	, _actionNumber()
	, _actionNumberPosition()
	, _actionString()
	, _actionStringPosition()
	, _parse( parse_ )
	, _stringCache()
	, _cache() {
	return;
}

HReal::HReal( action_range_t const& action_, PARSE parse_ )
	: HRuleBase( action_, true )
	, _actionDouble()
	, _actionDoublePosition()
	, _actionDoubleLong()
	, _actionDoubleLongPosition()
	, _actionNumber()
	, _actionNumberPosition()
	, _actionString()
	, _actionStringPosition()
	, _parse( parse_ )
	, _stringCache()
	, _cache() {
	return;
}

HReal::HReal( action_double_t const& action_, PARSE parse_ )
	: HRuleBase( true )
	, _actionDouble( action_ )
	, _actionDoublePosition()
	, _actionDoubleLong()
	, _actionDoubleLongPosition()
	, _actionNumber()
	, _actionNumberPosition()
	, _actionString()
	, _actionStringPosition()
	, _parse( parse_ )
	, _stringCache()
	, _cache() {
	return;
}

HReal::HReal( action_double_range_t const& action_, PARSE parse_ )
	: HRuleBase( true )
	, _actionDouble()
	, _actionDoublePosition( action_ )
	, _actionDoubleLong()
	, _actionDoubleLongPosition()
	, _actionNumber()
	, _actionNumberPosition()
	, _actionString()
	, _actionStringPosition()
	, _parse( parse_ )
	, _stringCache()
	, _cache() {
	return;
}

HReal::HReal( action_double_long_t const& action_, PARSE parse_ )
	: HRuleBase( true )
	, _actionDouble()
	, _actionDoublePosition()
	, _actionDoubleLong( action_ )
	, _actionDoubleLongPosition()
	, _actionNumber()
	, _actionNumberPosition()
	, _actionString()
	, _actionStringPosition()
	, _parse( parse_ )
	, _stringCache()
	, _cache() {
	return;
}

HReal::HReal( action_double_long_range_t const& action_, PARSE parse_ )
	: HRuleBase( true )
	, _actionDouble()
	, _actionDoublePosition()
	, _actionDoubleLong()
	, _actionDoubleLongPosition( action_ )
	, _actionNumber()
	, _actionNumberPosition()
	, _actionString()
	, _actionStringPosition()
	, _parse( parse_ )
	, _stringCache()
	, _cache() {
	return;
}

HReal::HReal( action_number_t const& action_, PARSE parse_ )
	: HRuleBase( true )
	, _actionDouble()
	, _actionDoublePosition()
	, _actionDoubleLong()
	, _actionDoubleLongPosition()
	, _actionNumber( action_ )
	, _actionNumberPosition()
	, _actionString()
	, _actionStringPosition()
	, _parse( parse_ )
	, _stringCache()
	, _cache() {
	return;
}

HReal::HReal( action_number_range_t const& action_, PARSE parse_ )
	: HRuleBase( true )
	, _actionDouble()
	, _actionDoublePosition()
	, _actionDoubleLong()
	, _actionDoubleLongPosition()
	, _actionNumber()
	, _actionNumberPosition( action_ )
	, _actionString()
	, _actionStringPosition()
	, _parse( parse_ )
	, _stringCache()
	, _cache() {
	return;
}

HReal::HReal( action_string_t const& action_, PARSE parse_ )
	: HRuleBase( true )
	, _actionDouble()
	, _actionDoublePosition()
	, _actionDoubleLong()
	, _actionDoubleLongPosition()
	, _actionNumber()
	, _actionNumberPosition()
	, _actionString( action_ )
	, _actionStringPosition()
	, _parse( parse_ )
	, _stringCache()
	, _cache() {
	return;
}

HReal::HReal( action_string_range_t const& action_, PARSE parse_ )
	: HRuleBase( true )
	, _actionDouble()
	, _actionDoublePosition()
	, _actionDoubleLong()
	, _actionDoubleLongPosition()
	, _actionNumber()
	, _actionNumberPosition()
	, _actionString()
	, _actionStringPosition( action_ )
	, _parse( parse_ )
	, _stringCache()
	, _cache() {
	return;
}

HReal::HReal( HReal const& real_ )
	: HRuleBase( real_._action, real_._actionPosition, real_._skipWS )
	, _actionDouble( real_._actionDouble )
	, _actionDoublePosition( real_._actionDoublePosition )
	, _actionDoubleLong( real_._actionDoubleLong )
	, _actionDoubleLongPosition( real_._actionDoubleLongPosition )
	, _actionNumber( real_._actionNumber )
	, _actionNumberPosition( real_._actionNumberPosition )
	, _actionString( real_._actionString )
	, _actionStringPosition( real_._actionStringPosition )
	, _parse( real_._parse )
	, _stringCache( real_._stringCache )
	, _cache() {
	return;
}

HReal HReal::operator[]( action_t const& action_ ) const {
	M_PROLOG
	M_ENSURE( ! has_action() );
	return ( HReal( action_, _parse ) );
	M_EPILOG
}

HReal HReal::operator[]( action_range_t const& action_ ) const {
	M_PROLOG
	M_ENSURE( ! has_action() );
	return ( HReal( action_, _parse ) );
	M_EPILOG
}

HReal HReal::operator[]( action_double_t const& action_ ) const {
	M_PROLOG
	M_ENSURE( ! has_action() );
	return ( HReal( action_, _parse ) );
	M_EPILOG
}

HReal HReal::operator[]( action_double_range_t const& action_ ) const {
	M_PROLOG
	M_ENSURE( ! has_action() );
	return ( HReal( action_, _parse ) );
	M_EPILOG
}

HReal HReal::operator[]( action_double_long_t const& action_ ) const {
	M_PROLOG
	M_ENSURE( ! has_action() );
	return ( HReal( action_, _parse ) );
	M_EPILOG
}

HReal HReal::operator[]( action_double_long_range_t const& action_ ) const {
	M_PROLOG
	M_ENSURE( ! has_action() );
	return ( HReal( action_, _parse ) );
	M_EPILOG
}

HReal HReal::operator[]( action_number_t const& action_ ) const {
	M_PROLOG
	M_ENSURE( ! has_action() );
	return ( HReal( action_, _parse ) );
	M_EPILOG
}

HReal HReal::operator[]( action_number_range_t const& action_ ) const {
	M_PROLOG
	M_ENSURE( ! has_action() );
	return ( HReal( action_, _parse ) );
	M_EPILOG
}

HReal HReal::operator[]( action_string_t const& action_ ) const {
	M_PROLOG
	M_ENSURE( ! has_action() );
	return ( HReal( action_, _parse ) );
	M_EPILOG
}

HReal HReal::operator[]( action_string_range_t const& action_ ) const {
	M_PROLOG
	M_ENSURE( ! has_action() );
	return ( HReal( action_, _parse ) );
	M_EPILOG
}

HReal HReal::operator()( PARSE parse_ ) const {
	M_PROLOG
	M_ENSURE( ! has_action() );
	return ( HReal( parse_ ) );
	M_EPILOG
}

bool HReal::do_has_action( void ) const {
	return ( HRuleBase::do_has_action()
		|| ( !! _actionDouble ) || ( !! _actionDoublePosition )
		|| ( !! _actionDoubleLong ) || ( !! _actionDoubleLongPosition )
		|| ( !! _actionNumber ) || ( !! _actionNumberPosition )
		|| ( !! _actionString ) || ( !! _actionStringPosition )
	);
}

yaal::hcore::HUTF8String::const_iterator HReal::do_parse( HExecutingParser* executingParser_, hcore::HUTF8String::const_iterator const& first_, hcore::HUTF8String::const_iterator const& last_ ) const {
	M_PROLOG
	yaal::hcore::HUTF8String::const_iterator scan( first_ );
	_stringCache.clear();
	real_paring_state_t state( START );
	if ( scan != last_ ) {
		while ( scan != last_ ) {
			bool stop( false );
			code_point_t ch( *scan );
			switch ( state ) {
				case ( START ): {
					if ( is_digit( ch ) ) {
						state = INTEGRAL;
					} else if ( ch == '-' ) {
						state = MINUS;
					} else if ( ch == '.' ) {
						state = DOT;
					} else {
						stop = true;
					}
				} break;
				case ( MINUS ):
				case ( INTEGRAL ): {
					if ( is_digit( ch ) ) {
						state = INTEGRAL;
					} else if ( ch == '.' ) {
						state = DOT;
					} else {
						stop = true;
					}
				} break;
				case ( DOT ):
				case ( DECIMAL ): {
					if ( is_digit( ch ) ) {
						state = DECIMAL;
					} else {
						stop = true;
					}
				} break;
				default: {
					M_ASSERT( ! "invalid hardcoded state"[0] );
				}
			}
			if ( stop ) {
				break;
			}
			_stringCache.push_back( *scan );
			++ scan;
		}
	} else {
		scan = first_;
	}
	if ( ( ( _parse == PARSE::GREEDY ) && ( state >= INTEGRAL ) ) || ( ( _parse == PARSE::STRICT ) && ( state >= DOT ) ) ) {
		range_t rng( range( executingParser_, first_, scan ) );
		if ( !! _actionDouble || !! _actionDoublePosition || !! _actionDoubleLong || !! _actionDoubleLongPosition ) {
			int bufSize( static_cast<int>( _stringCache.get_length() + 1 ) );
			_cache.realloc( bufSize );
			copy_ascii( _stringCache, _cache.get<char>(), bufSize );
		}
		if ( !! _actionDouble ) {
			double d( ::strtod( _cache.get<char>(), nullptr ) );
			add_execution_step( executingParser_, call( _actionDouble, d ) );
		} else if ( !! _actionDoublePosition ) {
			double d( ::strtod( _cache.get<char>(), nullptr ) );
			add_execution_step( executingParser_, call( _actionDoublePosition, d, rng ) );
		} else if ( !! _actionDoubleLong ) {
			double long dl( ::strtold( _cache.get<char>(), nullptr ) );
			add_execution_step( executingParser_, call( _actionDoubleLong, dl ) );
		} else if ( !! _actionDoubleLongPosition ) {
			double long dl( ::strtold( _cache.get<char>(), nullptr ) );
			add_execution_step( executingParser_, call( _actionDoubleLongPosition, dl, rng ) );
		} else if ( !! _actionNumber ) {
			add_execution_step( executingParser_, call( _actionNumber, yaal::move( _stringCache ) ) );
		} else if ( !! _actionNumberPosition ) {
			add_execution_step( executingParser_, call( _actionNumberPosition, yaal::move( _stringCache ), rng ) );
		} else if ( !! _actionString ) {
			add_execution_step( executingParser_, call( _actionString, yaal::move( _stringCache ) ) );
		} else if ( !! _actionStringPosition ) {
			add_execution_step( executingParser_, call( _actionStringPosition, yaal::move( _stringCache ), rng ) );
		} else if ( !! _action ) {
			add_execution_step( executingParser_, call( _action ) );
		} else if ( !! _actionPosition ) {
			add_execution_step( executingParser_, call( _actionPosition, rng ) );
		}
	} else {
		report_error( executingParser_, scan, "expected real number" );
		scan = first_;
	}
	return ( scan );
	M_EPILOG
}

HRuleBase::ptr_t HReal::do_clone( void ) const {
	M_PROLOG
	return ( make_pointer<HReal>( *this ) );
	M_EPILOG
}

void HReal::do_describe( HRuleDescription& rd_, rule_use_t const& ) const {
	M_PROLOG
	rd_.desc( "real" );
	return;
	M_EPILOG
}

void HReal::do_detach( HRuleBase const*, visited_t&, bool& ) {
	M_PROLOG
	return;
	M_EPILOG
}

bool HReal::do_detect_recursion( HRecursionDetector& recursionDetector_, bool ) const {
	M_PROLOG
	recursionDetector_.reset_visits();
	return ( true );
	M_EPILOG
}

void HReal::do_find_recursions( HRuleAggregator& ) {
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
	: HRuleBase( true )
	, _actionIntLongLong()
	, _actionIntLongLongPosition()
	, _actionIntLong()
	, _actionIntLongPosition()
	, _actionInt()
	, _actionIntPosition()
	, _actionNumber()
	, _actionNumberPosition()
	, _actionString()
	, _actionStringPosition()
	, _cache() {
	return;
}

HInteger::HInteger( action_t const& action_ )
	: HRuleBase( action_, true )
	, _actionIntLongLong()
	, _actionIntLongLongPosition()
	, _actionIntLong()
	, _actionIntLongPosition()
	, _actionInt()
	, _actionIntPosition()
	, _actionNumber()
	, _actionNumberPosition()
	, _actionString()
	, _actionStringPosition()
	, _cache() {
	return;
}

HInteger::HInteger( action_range_t const& action_ )
	: HRuleBase( action_, true )
	, _actionIntLongLong()
	, _actionIntLongLongPosition()
	, _actionIntLong()
	, _actionIntLongPosition()
	, _actionInt()
	, _actionIntPosition()
	, _actionNumber()
	, _actionNumberPosition()
	, _actionString()
	, _actionStringPosition()
	, _cache() {
	return;
}

HInteger::HInteger( action_int_long_long_t const& action_ )
	: HRuleBase( true )
	, _actionIntLongLong( action_ )
	, _actionIntLongLongPosition()
	, _actionIntLong()
	, _actionIntLongPosition()
	, _actionInt()
	, _actionIntPosition()
	, _actionNumber()
	, _actionNumberPosition()
	, _actionString()
	, _actionStringPosition()
	, _cache() {
	return;
}

HInteger::HInteger( action_int_long_long_range_t const& action_ )
	: HRuleBase( true )
	, _actionIntLongLong()
	, _actionIntLongLongPosition( action_ )
	, _actionIntLong()
	, _actionIntLongPosition()
	, _actionInt()
	, _actionIntPosition()
	, _actionNumber()
	, _actionNumberPosition()
	, _actionString()
	, _actionStringPosition()
	, _cache() {
	return;
}

HInteger::HInteger( action_int_long_t const& action_ )
	: HRuleBase( true )
	, _actionIntLongLong()
	, _actionIntLongLongPosition()
	, _actionIntLong( action_ )
	, _actionIntLongPosition()
	, _actionInt()
	, _actionIntPosition()
	, _actionNumber()
	, _actionNumberPosition()
	, _actionString()
	, _actionStringPosition()
	, _cache() {
	return;
}

HInteger::HInteger( action_int_long_range_t const& action_ )
	: HRuleBase( true )
	, _actionIntLongLong()
	, _actionIntLongLongPosition()
	, _actionIntLong()
	, _actionIntLongPosition( action_ )
	, _actionInt()
	, _actionIntPosition()
	, _actionNumber()
	, _actionNumberPosition()
	, _actionString()
	, _actionStringPosition()
	, _cache() {
	return;
}

HInteger::HInteger( action_int_t const& action_ )
	: HRuleBase( true )
	, _actionIntLongLong()
	, _actionIntLongLongPosition()
	, _actionIntLong()
	, _actionIntLongPosition()
	, _actionInt( action_ )
	, _actionIntPosition()
	, _actionNumber()
	, _actionNumberPosition()
	, _actionString()
	, _actionStringPosition()
	, _cache() {
	return;
}

HInteger::HInteger( action_int_range_t const& action_ )
	: HRuleBase( true )
	, _actionIntLongLong()
	, _actionIntLongLongPosition()
	, _actionIntLong()
	, _actionIntLongPosition()
	, _actionInt()
	, _actionIntPosition( action_ )
	, _actionNumber()
	, _actionNumberPosition()
	, _actionString()
	, _actionStringPosition()
	, _cache() {
	return;
}

HInteger::HInteger( action_number_t const& action_ )
	: HRuleBase( true )
	, _actionIntLongLong()
	, _actionIntLongLongPosition()
	, _actionIntLong()
	, _actionIntLongPosition()
	, _actionInt()
	, _actionIntPosition()
	, _actionNumber( action_ )
	, _actionNumberPosition()
	, _actionString()
	, _actionStringPosition()
	, _cache() {
	return;
}

HInteger::HInteger( action_number_range_t const& action_ )
	: HRuleBase( true )
	, _actionIntLongLong()
	, _actionIntLongLongPosition()
	, _actionIntLong()
	, _actionIntLongPosition()
	, _actionInt()
	, _actionIntPosition()
	, _actionNumber()
	, _actionNumberPosition( action_ )
	, _actionString()
	, _actionStringPosition()
	, _cache() {
	return;
}

HInteger::HInteger( action_string_t const& action_ )
	: HRuleBase( true )
	, _actionIntLongLong()
	, _actionIntLongLongPosition()
	, _actionIntLong()
	, _actionIntLongPosition()
	, _actionInt()
	, _actionIntPosition()
	, _actionNumber()
	, _actionNumberPosition()
	, _actionString( action_ )
	, _actionStringPosition()
	, _cache() {
	return;
}

HInteger::HInteger( action_string_range_t const& action_ )
	: HRuleBase( true )
	, _actionIntLongLong()
	, _actionIntLongLongPosition()
	, _actionIntLong()
	, _actionIntLongPosition()
	, _actionInt()
	, _actionIntPosition()
	, _actionNumber()
	, _actionNumberPosition()
	, _actionString()
	, _actionStringPosition( action_ )
	, _cache() {
	return;
}

HInteger::HInteger( HInteger const& integer_ )
	: HRuleBase( integer_._action, integer_._actionPosition, integer_._skipWS )
	, _actionIntLongLong( integer_._actionIntLongLong )
	, _actionIntLongLongPosition( integer_._actionIntLongLongPosition )
	, _actionIntLong( integer_._actionIntLong )
	, _actionIntLongPosition( integer_._actionIntLongPosition )
	, _actionInt( integer_._actionInt )
	, _actionIntPosition( integer_._actionIntPosition )
	, _actionNumber( integer_._actionNumber )
	, _actionNumberPosition( integer_._actionNumberPosition )
	, _actionString( integer_._actionString )
	, _actionStringPosition( integer_._actionStringPosition )
	, _cache( integer_._cache ) {
	return;
}

HInteger HInteger::operator[]( action_t const& action_ ) const {
	M_PROLOG
	M_ENSURE( ! has_action() );
	return ( HInteger( action_ ) );
	M_EPILOG
}

HInteger HInteger::operator[]( action_range_t const& action_ ) const {
	M_PROLOG
	M_ENSURE( ! has_action() );
	return ( HInteger( action_ ) );
	M_EPILOG
}

HInteger HInteger::operator[]( action_int_long_long_t const& action_ ) const {
	M_PROLOG
	M_ENSURE( ! has_action() );
	return ( HInteger( action_ ) );
	M_EPILOG
}

HInteger HInteger::operator[]( action_int_long_long_range_t const& action_ ) const {
	M_PROLOG
	M_ENSURE( ! has_action() );
	return ( HInteger( action_ ) );
	M_EPILOG
}

HInteger HInteger::operator[]( action_int_long_t const& action_ ) const {
	M_PROLOG
	M_ENSURE( ! has_action() );
	return ( HInteger( action_ ) );
	M_EPILOG
}

HInteger HInteger::operator[]( action_int_long_range_t const& action_ ) const {
	M_PROLOG
	M_ENSURE( ! has_action() );
	return ( HInteger( action_ ) );
	M_EPILOG
}

HInteger HInteger::operator[]( action_int_t const& action_ ) const {
	M_PROLOG
	M_ENSURE( ! has_action() );
	return ( HInteger( action_ ) );
	M_EPILOG
}

HInteger HInteger::operator[]( action_int_range_t const& action_ ) const {
	M_PROLOG
	M_ENSURE( ! has_action() );
	return ( HInteger( action_ ) );
	M_EPILOG
}

HInteger HInteger::operator[]( action_number_t const& action_ ) const {
	M_PROLOG
	M_ENSURE( ! has_action() );
	return ( HInteger( action_ ) );
	M_EPILOG
}

HInteger HInteger::operator[]( action_number_range_t const& action_ ) const {
	M_PROLOG
	M_ENSURE( ! has_action() );
	return ( HInteger( action_ ) );
	M_EPILOG
}

HInteger HInteger::operator[]( action_string_t const& action_ ) const {
	M_PROLOG
	M_ENSURE( ! has_action() );
	return ( HInteger( action_ ) );
	M_EPILOG
}

HInteger HInteger::operator[]( action_string_range_t const& action_ ) const {
	M_PROLOG
	M_ENSURE( ! has_action() );
	return ( HInteger( action_ ) );
	M_EPILOG
}

bool HInteger::do_has_action( void ) const {
	return ( HRuleBase::do_has_action()
		|| ( !! _actionIntLongLong ) || ( !! _actionIntLongLongPosition )
		|| ( !! _actionIntLong ) || ( !! _actionIntLongPosition )
		|| ( !! _actionInt ) || ( !! _actionIntPosition )
		|| ( !! _actionNumber ) || ( !! _actionNumberPosition )
		|| ( !! _actionString ) || ( !! _actionStringPosition )
	);
}

yaal::hcore::HUTF8String::const_iterator HInteger::do_parse( HExecutingParser* executingParser_, hcore::HUTF8String::const_iterator const& first_, hcore::HUTF8String::const_iterator const& last_ ) const {
	M_PROLOG
	yaal::hcore::HUTF8String::const_iterator scan( first_ );
	_cache.clear();
	bool valid( true );
	do {
		if ( scan == last_ ) {
			scan = first_;
			valid = false;
			break;
		}
		typedef bool ( *digit_test_func_t )( code_point_t );
		digit_test_func_t is_digit_test( &is_dec_digit );
		int skip( *scan == '-' ? 1 : 0 );
		bool isBin( is_binary( scan, last_ ) );
		if ( is_hexadecimal( scan, last_ ) ) {
			skip += 2;
			is_digit_test = &is_hex_digit;
		} else if ( is_octal( scan, last_ ) ) {
			skip += 1;
			if ( ( *( scan + skip ) == 'o' ) || ( *( scan + skip ) == 'O' ) ) {
				skip += 1;
			}
			is_digit_test = &is_oct_digit;
		} else if ( isBin ) {
			skip += 2;
			is_digit_test = &is_bin_digit;
		}
		scan += skip;
		while ( ( scan != last_ ) && is_digit_test( *scan ) ) {
			++ scan;
		}
		if ( scan == ( first_ + skip ) ) {
			valid = false;
			break;
		}
		if ( ( scan != last_ ) && ( character_class<CHARACTER_CLASS::WORD>().has( *scan ) ) ) {
			valid = false;
			break;
		}
		_cache = HUTF8String( first_, scan );
	} while ( false );
	if ( valid ) {
		try {
			range_t rng( range( executingParser_, first_, scan ) );
			if ( !! _actionIntLongLong ) {
				int long long ill( lexical_cast<int long long>( _cache ) );
				add_execution_step( executingParser_, call( _actionIntLongLong, ill ) );
			} else if ( !! _actionIntLongLongPosition ) {
				int long long ill( lexical_cast<int long long>( _cache ) );
				add_execution_step( executingParser_, call( _actionIntLongLongPosition, ill, rng ) );
			} else if ( !! _actionIntLong ) {
				int long il( lexical_cast<int long>( _cache ) );
				add_execution_step( executingParser_, call( _actionIntLong, il ) );
			} else if ( !! _actionIntLongPosition ) {
				int long il( lexical_cast<int long>( _cache ) );
				add_execution_step( executingParser_, call( _actionIntLongPosition, il, rng ) );
			} else if ( !! _actionInt ) {
				int i( lexical_cast<int>( _cache ) );
				add_execution_step( executingParser_, call( _actionInt, i ) );
			} else if ( !! _actionIntPosition ) {
				int i( lexical_cast<int>( _cache ) );
				add_execution_step( executingParser_, call( _actionIntPosition, i, rng ) );
			} else if ( !! _actionNumber ) {
				add_execution_step( executingParser_, call( _actionNumber, yaal::move( _cache ) ) );
			} else if ( !! _actionNumberPosition ) {
				add_execution_step( executingParser_, call( _actionNumberPosition, yaal::move( _cache ), rng ) );
			} else if ( !! _actionString ) {
				add_execution_step( executingParser_, call( _actionString, yaal::move( _cache ) ) );
			} else if ( !! _actionStringPosition ) {
				add_execution_step( executingParser_, call( _actionStringPosition, yaal::move( _cache ), rng ) );
			} else if ( !! _action ) {
				add_execution_step( executingParser_, call( _action ) );
			} else if ( !! _actionPosition ) {
				add_execution_step( executingParser_, call( _actionPosition, rng ) );
			}
		} catch ( HException const& e ) {
			report_error( executingParser_, scan, e.what() );
			scan = first_;
		}
	} else {
		report_error( executingParser_, scan, "expected integral number" );
		scan = first_;
	}
	return ( scan );
	M_EPILOG
}

HRuleBase::ptr_t HInteger::do_clone( void ) const {
	M_PROLOG
	return ( make_pointer<HInteger>( *this ) );
	M_EPILOG
}

void HInteger::do_describe( HRuleDescription& rd_, rule_use_t const& ) const {
	M_PROLOG
	rd_.desc( "integer" );
	return;
	M_EPILOG
}

void HInteger::do_detach( HRuleBase const*, visited_t&, bool& ) {
	M_PROLOG
	return;
	M_EPILOG
}

bool HInteger::do_detect_recursion( HRecursionDetector& recursionDetector_, bool ) const {
	M_PROLOG
	recursionDetector_.reset_visits();
	return ( true );
	M_EPILOG
}

void HInteger::do_find_recursions( HRuleAggregator& ) {
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

HStringLiteral::HStringLiteral( void )
	: HRuleBase( true )
	, _actionString()
	, _actionStringPosition()
	, _cache() {
}

HStringLiteral::HStringLiteral( action_t const& action_ )
	: HRuleBase( action_, true )
	, _actionString()
	, _actionStringPosition()
	, _cache() {
}

HStringLiteral::HStringLiteral( action_range_t const& action_ )
	: HRuleBase( action_, true )
	, _actionString()
	, _actionStringPosition()
	, _cache() {
}

HStringLiteral::HStringLiteral( action_string_t const& action_ )
	: HRuleBase( true )
	, _actionString( action_ )
	, _actionStringPosition()
	, _cache() {
}

HStringLiteral::HStringLiteral( action_string_range_t const& action_ )
	: HRuleBase( true )
	, _actionString()
	, _actionStringPosition( action_ )
	, _cache() {
}

HStringLiteral::HStringLiteral( HStringLiteral const& stringLiteral_ )
	: HRuleBase( stringLiteral_._action, stringLiteral_._actionPosition, stringLiteral_._skipWS )
	, _actionString( stringLiteral_._actionString )
	, _actionStringPosition( stringLiteral_._actionStringPosition )
	, _cache( stringLiteral_._cache ) {
}

HStringLiteral HStringLiteral::operator[]( action_t const& action_ ) const {
	M_PROLOG
	M_ENSURE( ! has_action() );
	return ( HStringLiteral( action_ ) );
	M_EPILOG
}

HStringLiteral HStringLiteral::operator[]( action_range_t const& action_ ) const {
	M_PROLOG
	M_ENSURE( ! has_action() );
	return ( HStringLiteral( action_ ) );
	M_EPILOG
}

HStringLiteral HStringLiteral::operator[]( action_string_t const& action_ ) const {
	M_PROLOG
	M_ENSURE( ! has_action() );
	return ( HStringLiteral( action_ ) );
	M_EPILOG
}

HStringLiteral HStringLiteral::operator[]( action_string_range_t const& action_ ) const {
	M_PROLOG
	M_ENSURE( ! has_action() );
	return ( HStringLiteral( action_ ) );
	M_EPILOG
}

bool HStringLiteral::do_has_action( void ) const {
	return ( HRuleBase::do_has_action() || ( !! _actionString ) || ( !! _actionStringPosition ) );
}

namespace {

/*
 * Only printable characters and horizontal tab are valid in string or character literals.
 */
bool is_known_character( code_point_t char_ ) {
	return ( ( char_ >= ' ' ) || ( char_ == '\t' ) );
}

void semantic_unescape( yaal::hcore::HString& str_ ) {
	M_PROLOG
	for ( int i( 0 ); i < str_.get_length(); ++ i ) {
		int k( i );
		if ( ( str_[k] == '\\' ) && ( ( k + 1 ) < str_.get_length() ) ) {
			++ k;
			if ( str_[k] == '\\' ) {
				++ i;
				continue;
			}
			code_point_t prefix( str_[k] );
			int codeLen( 0 );
			switch ( prefix.get() ) {
				case ( 'x' ): { codeLen = 2; } break;
				case ( 'u' ): { codeLen = 4; } break;
				case ( 'U' ): { codeLen = 8; } break;
			}
			int base( 16 );
			bool good( false );
			char num[10];
			if ( codeLen > 0 ) {
				if ( ( k + codeLen ) < str_.get_length() ) {
					good = true;
					++ k;
					for ( int n( 0 ); n < codeLen; ++ n ) {
						code_point_t d( str_[k + n] );
						if ( ! is_hex_digit( d ) ) {
							good = false;
							break;
						}
						num[n] = static_cast<char>( d.get() );
					}
				}
			} else {
				base = 8;
				for ( int len( min( 3, static_cast<int>( str_.get_length() ) - k ) ); codeLen < len; ++ codeLen ) {
					code_point_t d( str_[k + codeLen] );
					if ( ! is_oct_digit( d ) ) {
						break;
					}
					num[codeLen] = static_cast<char>( d.get() );
					good = true;
				}
			}
			if ( good ) {
				num[codeLen] = 0;
				code_point_t c( static_cast<u32_t>( stoul( num, nullptr, base ) ) );
				if ( ( c != 0_ycp ) && ( c < unicode::UCS_MAX_4_BYTE_CODE_POINT ) ) {
					str_.replace( i, ( base == 16 ? 2 : 1 ) + codeLen, 1, c );
				}
			}
		}
	}
	return;
	M_EPILOG
}

class HParseResult {
	yaal::hcore::HUTF8String::const_iterator _scan;
	bool _valid;
public:
	HParseResult( yaal::hcore::HUTF8String::const_iterator scan_, bool valid_ )
		: _scan( scan_ )
		, _valid( valid_ ) {
	}
	yaal::hcore::HUTF8String::const_iterator scan( void ) const {
		return ( _scan );
	}
	bool valid( void ) const {
		return ( _valid );
	}
};

HParseResult parse_quoted(
	yaal::hcore::HString& onto_,
	yaal::hcore::HUTF8String::const_iterator const& first_,
	yaal::hcore::HUTF8String::const_iterator from_,
	yaal::hcore::HUTF8String::const_iterator const& last_,
	code_point_t quote_
) {
	yaal::hcore::HUTF8String::const_iterator scan( from_ );
	bool valid( false );
	yaal::hcore::HUTF8String::const_iterator from;
	do {
		if ( scan == last_ ) {
			scan = first_;
			break;
		}
		if( *scan != quote_ ) {
			break;
		}
		++ scan;
		from = scan;
		while ( ( scan != last_ ) && ( *scan != quote_ ) ) {
			if ( *scan == '\\' ) {
				++ scan;
				if ( ! ( scan != last_ ) ) {
					break;
				}
			}
			if ( ! is_known_character( *scan ) ) {
				break;
			}
			++ scan;
		}
		if ( scan != last_ ) {
			if ( *scan != quote_ ) {
				break;
			}
			valid = true;
		}
	} while ( false );
	if ( valid ) {
		onto_ = HUTF8String( from, scan );
	}
	return ( HParseResult( scan, valid ) );
}

}

yaal::hcore::HUTF8String::const_iterator HStringLiteral::do_parse( HExecutingParser* executingParser_, hcore::HUTF8String::const_iterator const& first_, hcore::HUTF8String::const_iterator const& last_ ) const {
	M_PROLOG
	HParseResult parseResult( parse_quoted( _cache, first_, first_, last_, '"'_ycp ) );
	yaal::hcore::HUTF8String::const_iterator scan( parseResult.scan() );
	if ( parseResult.valid() ) {
		++ scan;
		semantic_unescape( _cache );
		unescape( _cache, _escapes_ );
		range_t rng( range( executingParser_, first_, scan ) );
		if ( !! _actionString ) {
			add_execution_step( executingParser_, call( _actionString, yaal::move( _cache ) ) );
		} else if ( !! _actionStringPosition ) {
			add_execution_step( executingParser_, call( _actionStringPosition, yaal::move( _cache ), rng ) );
		} else if ( !! _action ) {
			add_execution_step( executingParser_, _action );
		} else if ( !! _actionPosition ) {
			add_execution_step( executingParser_, call( _actionPosition, rng ) );
		}
	} else {
		report_error( executingParser_, scan, "expected literal string" );
		scan = first_;
	}
	return ( scan );
	M_EPILOG
}

HRuleBase::ptr_t HStringLiteral::do_clone( void ) const {
	M_PROLOG
	return ( make_pointer<HStringLiteral>( *this ) );
	M_EPILOG
}

void HStringLiteral::do_describe( HRuleDescription& rd_, rule_use_t const& ) const {
	M_PROLOG
	rd_.desc( "string_literal" );
	return;
	M_EPILOG
}

void HStringLiteral::do_detach( HRuleBase const*, visited_t&, bool& ) {
	M_PROLOG
	return;
	M_EPILOG
}

bool HStringLiteral::do_detect_recursion( HRecursionDetector& recursionDetector_, bool ) const {
	M_PROLOG
	recursionDetector_.reset_visits();
	return ( true );
	M_EPILOG
}

void HStringLiteral::do_find_recursions( HRuleAggregator& ) {
	M_PROLOG
	return;
	M_EPILOG
}

HStringLiteral const& get_string_literal_instance( void ) {
	M_PROLOG
	static HStringLiteral stringLiteralInstance;
	return ( stringLiteralInstance );
	M_EPILOG
}

HStringLiteral const& string_literal( get_string_literal_instance() );

HCharacterLiteral::HCharacterLiteral( void )
	: HRuleBase( true )
	, _actionCharacter()
	, _actionCharacterPosition()
	, _cache() {
}

HCharacterLiteral::HCharacterLiteral( action_t const& action_ )
	: HRuleBase( action_, true )
	, _actionCharacter()
	, _actionCharacterPosition()
	, _cache() {
}

HCharacterLiteral::HCharacterLiteral( action_range_t const& action_ )
	: HRuleBase( action_, true )
	, _actionCharacter()
	, _actionCharacterPosition()
	, _cache() {
}

HCharacterLiteral::HCharacterLiteral( action_character_t const& action_ )
	: HRuleBase( true )
	, _actionCharacter( action_ )
	, _actionCharacterPosition()
	, _cache() {
}

HCharacterLiteral::HCharacterLiteral( action_character_range_t const& action_ )
	: HRuleBase( true )
	, _actionCharacter()
	, _actionCharacterPosition( action_ )
	, _cache() {
}

HCharacterLiteral::HCharacterLiteral( HCharacterLiteral const& characterLiteral_ )
	: HRuleBase( characterLiteral_._action, characterLiteral_._actionPosition, characterLiteral_._skipWS )
	, _actionCharacter( characterLiteral_._actionCharacter )
	, _actionCharacterPosition( characterLiteral_._actionCharacterPosition )
	, _cache( characterLiteral_._cache ) {
}

HCharacterLiteral HCharacterLiteral::operator[]( action_t const& action_ ) const {
	M_PROLOG
	M_ENSURE( ! has_action() );
	return ( HCharacterLiteral( action_ ) );
	M_EPILOG
}

HCharacterLiteral HCharacterLiteral::operator[]( action_range_t const& action_ ) const {
	M_PROLOG
	M_ENSURE( ! has_action() );
	return ( HCharacterLiteral( action_ ) );
	M_EPILOG
}

HCharacterLiteral HCharacterLiteral::operator[]( action_character_t const& action_ ) const {
	M_PROLOG
	M_ENSURE( ! has_action() );
	return ( HCharacterLiteral( action_ ) );
	M_EPILOG
}

HCharacterLiteral HCharacterLiteral::operator[]( action_character_range_t const& action_ ) const {
	M_PROLOG
	M_ENSURE( ! has_action() );
	return ( HCharacterLiteral( action_ ) );
	M_EPILOG
}

bool HCharacterLiteral::do_has_action( void ) const {
	return ( HRuleBase::do_has_action() || ( !! _actionCharacter ) || ( !! _actionCharacterPosition ) );
}

yaal::hcore::HUTF8String::const_iterator HCharacterLiteral::do_parse( HExecutingParser* executingParser_, hcore::HUTF8String::const_iterator const& first_, hcore::HUTF8String::const_iterator const& last_ ) const {
	M_PROLOG
	HParseResult parseResult( parse_quoted( _cache, first_, first_, last_, '\''_ycp ) );
	yaal::hcore::HUTF8String::const_iterator scan( parseResult.scan() );
	static char const errMsg[] = "expected literal character";
	if ( parseResult.valid() ) {
		++ scan;
		semantic_unescape( _cache );
		unescape( _cache, _escapes_ );
		if ( _cache.get_length() == 1 ) {
			range_t rng( range( executingParser_, first_, scan ) );
			if ( !! _actionCharacter ) {
				add_execution_step( executingParser_, call( _actionCharacter, _cache[0] ) );
			} else if ( !! _actionCharacterPosition ) {
				add_execution_step( executingParser_, call( _actionCharacterPosition, _cache[0], rng ) );
			} else if ( !! _action ) {
				add_execution_step( executingParser_, _action );
			} else if ( !! _actionPosition ) {
				add_execution_step( executingParser_, call( _actionPosition, rng ) );
			}
		} else {
			report_error( executingParser_, scan, errMsg );
			scan = first_;
		}
	} else {
		report_error( executingParser_, scan, errMsg );
		scan = first_;
	}
	return ( scan );
	M_EPILOG
}

HRuleBase::ptr_t HCharacterLiteral::do_clone( void ) const {
	M_PROLOG
	return ( make_pointer<HCharacterLiteral>( *this ) );
	M_EPILOG
}

void HCharacterLiteral::do_describe( HRuleDescription& rd_, rule_use_t const& ) const {
	M_PROLOG
	rd_.desc( "character_literal" );
	return;
	M_EPILOG
}

void HCharacterLiteral::do_detach( HRuleBase const*, visited_t&, bool& ) {
	M_PROLOG
	return;
	M_EPILOG
}

bool HCharacterLiteral::do_detect_recursion( HRecursionDetector& recursionDetector_, bool ) const {
	M_PROLOG
	recursionDetector_.reset_visits();
	return ( true );
	M_EPILOG
}

void HCharacterLiteral::do_find_recursions( HRuleAggregator& ) {
	M_PROLOG
	return;
	M_EPILOG
}

HCharacterLiteral const& get_character_literal_instance( void ) {
	M_PROLOG
	static HCharacterLiteral characterLiteralInstance;
	return ( characterLiteralInstance );
	M_EPILOG
}

HCharacterLiteral const& character_literal( get_character_literal_instance() );

HCharacter::HCharacter( bool skipWS_ )
	: HRuleBase( skipWS_ )
	, _characters()
	, _actionCharacter()
	, _actionCharacterPosition()
	, _errorMessage( make_error_message() ) {
	return;
}

HCharacter::HCharacter( hcore::HString const& characters_, action_t const& action_, bool skipWS_ )
	: HRuleBase( action_, skipWS_ )
	, _characters( characters_ )
	, _actionCharacter()
	, _actionCharacterPosition()
	, _errorMessage( make_error_message() ) {
	return;
}

HCharacter::HCharacter( hcore::HString const& characters_, action_range_t const& action_, bool skipWS_ )
	: HRuleBase( action_, skipWS_ )
	, _characters( characters_ )
	, _actionCharacter()
	, _actionCharacterPosition()
	, _errorMessage( make_error_message() ) {
	return;
}

HCharacter::HCharacter( hcore::HString const& characters_, action_character_t const& action_, bool skipWS_ )
	: HRuleBase( skipWS_ )
	, _characters( characters_ )
	, _actionCharacter( action_ )
	, _actionCharacterPosition()
	, _errorMessage( make_error_message() ) {
	return;
}

HCharacter::HCharacter( hcore::HString const& characters_, action_character_range_t const& action_, bool skipWS_ )
	: HRuleBase( skipWS_ )
	, _characters( characters_ )
	, _actionCharacter()
	, _actionCharacterPosition( action_ )
	, _errorMessage( make_error_message() ) {
	return;
}

HCharacter::HCharacter( HCharacter const& character_ )
	: HRuleBase( character_._action, character_._actionPosition, character_._skipWS )
	, _characters( character_._characters )
	, _actionCharacter( character_._actionCharacter )
	, _actionCharacterPosition( character_._actionCharacterPosition )
	, _errorMessage( character_._errorMessage ) {
	return;
}

HCharacter::HCharacter( HCharacter const& character_, code_point_t const& token_ )
	: HRuleBase( character_._action, character_._actionPosition, character_._skipWS )
	, _characters( character_._characters )
	, _actionCharacter( character_._actionCharacter )
	, _actionCharacterPosition( character_._actionCharacterPosition )
	, _errorMessage( character_._errorMessage ) {
	if ( _characters.find( token_ ) != hcore::HString::npos ) {
		throw HCharacterException( "token `"_ys.append( token_ ).append( "' is already observed" ) );
	}
	_characters.push_back( token_ );
	_errorMessage = make_error_message();
	return;
}

HCharacter HCharacter::operator[]( action_t const& action_ ) const {
	M_PROLOG
	M_ENSURE( ! has_action() );
	return ( HCharacter( _characters, action_, _skipWS ) );
	M_EPILOG
}

HCharacter HCharacter::operator[]( action_range_t const& action_ ) const {
	M_PROLOG
	M_ENSURE( ! has_action() );
	return ( HCharacter( _characters, action_, _skipWS ) );
	M_EPILOG
}

HCharacter HCharacter::operator[]( action_character_t const& action_ ) const {
	M_PROLOG
	M_ENSURE( ! has_action() );
	return ( HCharacter( _characters, action_, _skipWS ) );
	M_EPILOG
}

HCharacter HCharacter::operator[]( action_character_range_t const& action_ ) const {
	M_PROLOG
	M_ENSURE( ! has_action() );
	return ( HCharacter( _characters, action_, _skipWS ) );
	M_EPILOG
}

bool HCharacter::do_has_action( void ) const {
	return ( HRuleBase::do_has_action() || ( !! _actionCharacter ) || ( !! _actionCharacterPosition ) );
}

namespace {

bool character_skip_ws( yaal::hcore::HString const& characters_, HRuleBase::WHITE_SPACE whiteSpace_ ) {
	M_PROLOG
	bool skipWS( whiteSpace_ == HRuleBase::WHITE_SPACE::SKIP );
	if ( whiteSpace_ == HRuleBase::WHITE_SPACE::AUTO ) {
		skipWS = true;
		for ( code_point_t c : characters_ ) {
			if ( is_whitespace( c ) ) {
				skipWS = false;
				break;
			}
		}
	}
	return ( skipWS );
	M_EPILOG
}

}

HCharacter HCharacter::operator() ( hcore::HString const& characters_, WHITE_SPACE whiteSpace_ ) const {
	M_PROLOG
	return ( HCharacter( characters_, _action, character_skip_ws( characters_, whiteSpace_ ) ) );
	M_EPILOG
}

HCharacter HCharacter::operator() ( WHITE_SPACE whiteSpace_ ) const {
	M_PROLOG
	return ( HCharacter( _characters, _action, character_skip_ws( _characters, whiteSpace_ ) ) );
	M_EPILOG
}

yaal::hcore::HUTF8String::const_iterator HCharacter::do_parse( HExecutingParser* executingParser_, hcore::HUTF8String::const_iterator const& first_, hcore::HUTF8String::const_iterator const& last_ ) const {
	M_PROLOG
	yaal::hcore::HUTF8String::const_iterator scan( first_ );
	bool matched( false );
	if ( scan != last_ ) {
		code_point_t c( *scan );
		if ( _characters.is_empty() || ( _characters.find( *scan ) != hcore::HString::npos ) ) {
			++ scan;
			range_t rng( range( executingParser_, first_, scan ) );
			if ( !! _actionCharacter ) {
				add_execution_step( executingParser_, call( _actionCharacter, c ) );
			} else if ( !! _actionCharacterPosition ) {
				add_execution_step( executingParser_, call( _actionCharacterPosition, c, rng ) );
			} else if ( !! _action ) {
				add_execution_step( executingParser_, call( _action ) );
			} else if ( !! _actionPosition ) {
				add_execution_step( executingParser_, call( _actionPosition, rng ) );
			}
			matched = true;
		}
	} else {
		scan = first_;
	}
	if ( ! matched ) {
		report_error( executingParser_, scan, _errorMessage );
		scan = first_;
	}
	return ( scan );
	M_EPILOG
}

yaal::hcore::HString HCharacter::make_error_message( void ) const {
	return ( "expected one of characters: "_ys.append( _characters ) );
}

HRuleBase::ptr_t HCharacter::do_clone( void ) const {
	M_PROLOG
	return ( make_pointer<HCharacter>( *this ) );
	M_EPILOG
}

void HCharacter::do_describe( HRuleDescription& rd_, rule_use_t const& ) const {
	M_PROLOG
	if ( ! _characters.is_empty() ) {
		rd_.desc( "'" );
		rd_.desc( _characters );
		rd_.desc( "'" );
	} else
		rd_.desc( "character" );
	return;
	M_EPILOG
}

void HCharacter::do_detach( HRuleBase const*, visited_t&, bool& ) {
	M_PROLOG
	return;
	M_EPILOG
}

bool HCharacter::do_detect_recursion( HRecursionDetector& recursionDetector_, bool ) const {
	M_PROLOG
	recursionDetector_.reset_visits();
	return ( true );
	M_EPILOG
}

void HCharacter::do_find_recursions( HRuleAggregator& ) {
	M_PROLOG
	return;
	M_EPILOG
}

HCharacter const& get_character_instance( void ) {
	M_PROLOG
	static HCharacter characterInstance( true );
	return ( characterInstance );
	M_EPILOG
}

HCharacter const& character( get_character_instance() );

HFollows operator >> ( char character_, HRuleBase const& successor_ ) {
	M_PROLOG
	return ( HFollows( character( character_ ), successor_  ) );
	M_EPILOG
}

HFollows operator >> ( HFollows const& predecessor_, char character_ ) {
	M_PROLOG
	return ( HFollows( predecessor_, character( character_ ) ) );
	M_EPILOG
}

HFollows operator >> ( HRuleBase const& predecessor_, char character_ ) {
	M_PROLOG
	return ( HFollows( predecessor_, character( character_ ) ) );
	M_EPILOG
}

HString::HString( hcore::HString const& string_, bool skipWS_, WORD_BOUNDARY wordBoundary_ )
	: HRuleBase( skipWS_ )
	, _dictionary( 1, string_ )
	, _actionString()
	, _actionStringPosition()
	, _wordBoundary( wordBoundary_ )
	, _errorMessage( make_error_message() ) {
	M_ASSERT( ! string_.is_empty() );
	return;
}

HString::HString( HString const& string_, yaal::hcore::HString const& token_ )
	: HRuleBase( string_._action, string_._actionPosition, string_._skipWS )
	, _dictionary( string_._dictionary )
	, _actionString( string_._actionString )
	, _actionStringPosition( string_._actionStringPosition )
	, _wordBoundary( string_._wordBoundary )
	, _errorMessage() {
	M_ASSERT( ! token_.is_empty() );
	for ( yaal::hcore::HString const& s : _dictionary ) {
		if ( token_.find( s ) == 0 ) {
			throw HStringException( "token `"_ys.append( token_ ).append( "' would be hidden" ) );
		}
	}
	_dictionary.emplace_back( token_ );
	_errorMessage = make_error_message();
	return;
}

HString::HString( hcore::HString const& string_, action_t const& action_, bool skipWS_, WORD_BOUNDARY wordBoundary_ )
	: HRuleBase( action_, skipWS_ )
	, _dictionary( 1, string_ )
	, _actionString()
	, _actionStringPosition()
	, _wordBoundary( wordBoundary_ )
	, _errorMessage( make_error_message() ) {
	M_ASSERT( ! string_.is_empty() );
	return;
}

HString::HString( hcore::HString const& string_, action_range_t const& action_, bool skipWS_, WORD_BOUNDARY wordBoundary_ )
	: HRuleBase( action_, skipWS_ )
	, _dictionary( 1, string_ )
	, _actionString()
	, _actionStringPosition()
	, _wordBoundary( wordBoundary_ )
	, _errorMessage( make_error_message() ) {
	M_ASSERT( ! string_.is_empty() );
	return;
}

HString::HString( hcore::HString const& string_, action_string_t const& action_, bool skipWS_, WORD_BOUNDARY wordBoundary_ )
	: HRuleBase( skipWS_ )
	, _dictionary( 1, string_ )
	, _actionString( action_ )
	, _actionStringPosition()
	, _wordBoundary( wordBoundary_ )
	, _errorMessage( make_error_message() ) {
	M_ASSERT( ! string_.is_empty() );
	return;
}

HString::HString( hcore::HString const& string_, action_string_range_t const& action_, bool skipWS_, WORD_BOUNDARY wordBoundary_ )
	: HRuleBase( skipWS_ )
	, _dictionary( 1, string_ )
	, _actionString()
	, _actionStringPosition( action_ )
	, _wordBoundary( wordBoundary_ )
	, _errorMessage( make_error_message() ) {
	M_ASSERT( ! string_.is_empty() );
	return;
}

HString::HString( dictionary_t const& dictionary_, bool skipWS_, WORD_BOUNDARY wordBoundary_ )
	: HRuleBase( skipWS_ )
	, _dictionary( dictionary_ )
	, _actionString()
	, _actionStringPosition()
	, _wordBoundary( wordBoundary_ )
	, _errorMessage( make_error_message() ) {
	M_ASSERT( ! _dictionary.is_empty() );
	return;
}

HString::HString( dictionary_t const& dictionary_, action_t const& action_, bool skipWS_, WORD_BOUNDARY wordBoundary_ )
	: HRuleBase( action_, skipWS_ )
	, _dictionary( dictionary_ )
	, _actionString()
	, _actionStringPosition()
	, _wordBoundary( wordBoundary_ )
	, _errorMessage( make_error_message() ) {
	M_ASSERT( ! _dictionary.is_empty() );
	return;
}

HString::HString( dictionary_t const& dictionary_, action_range_t const& action_, bool skipWS_, WORD_BOUNDARY wordBoundary_ )
	: HRuleBase( action_, skipWS_ )
	, _dictionary( dictionary_ )
	, _actionString()
	, _actionStringPosition()
	, _wordBoundary( wordBoundary_ )
	, _errorMessage( make_error_message() ) {
	M_ASSERT( ! _dictionary.is_empty() );
	return;
}

HString::HString( dictionary_t const& dictionary_, action_string_t const& action_, bool skipWS_, WORD_BOUNDARY wordBoundary_ )
	: HRuleBase( skipWS_ )
	, _dictionary( dictionary_ )
	, _actionString( action_ )
	, _actionStringPosition()
	, _wordBoundary( wordBoundary_ )
	, _errorMessage( make_error_message() ) {
	M_ASSERT( ! _dictionary.is_empty() );
	return;
}

HString::HString( dictionary_t const& dictionary_, action_string_range_t const& action_, bool skipWS_, WORD_BOUNDARY wordBoundary_ )
	: HRuleBase( skipWS_ )
	, _dictionary( dictionary_ )
	, _actionString()
	, _actionStringPosition( action_ )
	, _wordBoundary( wordBoundary_ )
	, _errorMessage( make_error_message() ) {
	M_ASSERT( ! _dictionary.is_empty() );
	return;
}

HString::HString( HString const& string_ )
	: HRuleBase( string_._action, string_._actionPosition, string_._skipWS )
	, _dictionary( string_._dictionary )
	, _actionString( string_._actionString )
	, _actionStringPosition( string_._actionStringPosition )
	, _wordBoundary( string_._wordBoundary )
	, _errorMessage( string_._errorMessage ) {
	M_ASSERT( ! _dictionary.is_empty() );
	return;
}

HString HString::operator[]( action_t const& action_ ) const {
	M_PROLOG
	M_ENSURE( ! has_action() );
	return ( HString( _dictionary, action_, _skipWS, _wordBoundary ) );
	M_EPILOG
}

HString HString::operator[]( action_range_t const& action_ ) const {
	M_PROLOG
	M_ENSURE( ! has_action() );
	return ( HString( _dictionary, action_, _skipWS, _wordBoundary ) );
	M_EPILOG
}

HString HString::operator[]( action_string_t const& action_ ) const {
	M_PROLOG
	M_ENSURE( ! has_action() );
	return ( HString( _dictionary, action_, _skipWS, _wordBoundary ) );
	M_EPILOG
}

HString HString::operator[]( action_string_range_t const& action_ ) const {
	M_PROLOG
	M_ENSURE( ! has_action() );
	return ( HString( _dictionary, action_, _skipWS, _wordBoundary ) );
	M_EPILOG
}

bool HString::do_has_action( void ) const {
	return ( HRuleBase::do_has_action() || ( !! _actionString ) || ( !! _actionStringPosition ) );
}

hcore::HUTF8String::const_iterator HString::do_parse( HExecutingParser* executingParser_, hcore::HUTF8String::const_iterator const& first_, hcore::HUTF8String::const_iterator const& last_ ) const {
	M_PROLOG
	M_ASSERT( ! _dictionary.is_empty() );
	yaal::hcore::HUTF8String::const_iterator scan( first_ );
	bool matched( false );
	if ( scan != last_ ) {
		for ( yaal::hcore::HString const& s : _dictionary ) {
			matched = true;
			scan = first_;
			for ( hcore::HString::const_iterator it( s.begin() ), end( s.end() ); it != end; ++ it, ++ scan ) {
				if ( ( scan == last_ ) || ( *scan != *it ) ) {
					matched = false;
					break;
				}
			}
			if ( matched && ( _wordBoundary == WORD_BOUNDARY::REQUIRED ) && ( scan != last_ ) ) {
				matched = exor( character_class<CHARACTER_CLASS::WORD>().has( *( scan - 1 ) ), character_class<CHARACTER_CLASS::WORD>().has( *scan ) );
			}
			if ( matched ) {
				range_t rng( range( executingParser_, first_, scan ) );
				if ( !! _actionString ) {
					add_execution_step( executingParser_, call( _actionString, s ) );
				} else if ( !! _actionStringPosition ) {
					add_execution_step( executingParser_, call( _actionStringPosition, s, rng ) );
				} else if ( !! _action ) {
					add_execution_step( executingParser_, call( _action ) );
				} else if ( !! _actionPosition ) {
					add_execution_step( executingParser_, call( _actionPosition, rng ) );
				}
				break;
			}
		}
	} else {
		scan = first_;
	}
	if ( ! matched ) {
		report_error( executingParser_, scan, _errorMessage );
		scan = first_;
	}
	return ( scan );
	M_EPILOG
}

yaal::hcore::HString HString::make_error_message( void ) const {
	return ( "expected string: "_ys.append( desc() ) );
}

yaal::hcore::HString HString::desc( void ) const {
	M_PROLOG
	hcore::HString d;
	if ( _dictionary.get_size() > 1 ) {
		d.assign( "( \"" );
		bool next( false );
		for ( yaal::hcore::HString const& s : _dictionary ) {
			if ( next ) {
				d.append( "\" | \"" );
			}
			d.append( s );
			next = true;
		}
		d.append( "\" )" );
	} else {
		d.append( "\"" ).append( _dictionary.front() ).append( "\"" );
	}
	return ( d );
	M_EPILOG
}

HRuleBase::ptr_t HString::do_clone( void ) const {
	M_PROLOG
	return ( make_pointer<HString>( *this ) );
	M_EPILOG
}

void HString::do_describe( HRuleDescription& rd_, rule_use_t const& ) const {
	M_PROLOG
	rd_.desc( desc() );
	return;
	M_EPILOG
}

void HString::do_detach( HRuleBase const*, visited_t&, bool& ) {
	M_PROLOG
	return;
	M_EPILOG
}

bool HString::do_detect_recursion( HRecursionDetector& recursionDetector_, bool ) const {
	M_PROLOG
	recursionDetector_.reset_visits();
	return ( true );
	M_EPILOG
}

void HString::do_find_recursions( HRuleAggregator& ) {
	M_PROLOG
	return;
	M_EPILOG
}

namespace {

inline bool skip_ws( hcore::HString const& string_, HRuleBase::WHITE_SPACE whiteSpace_ ) {
	return ( ( whiteSpace_ == HRuleBase::WHITE_SPACE::SKIP ) || ( ( whiteSpace_ == HRuleBase::WHITE_SPACE::AUTO ) && ! is_whitespace( string_.front() ) ) );
}

inline HString::WORD_BOUNDARY word_boundary( yaal::hcore::HString const& string_, HString::WORD_BOUNDARY wordBoundary_ ) {
	return (
		wordBoundary_ == HString::WORD_BOUNDARY::AUTO
			? (
				character_class<CHARACTER_CLASS::WORD>().has( string_.back() )
					? HString::WORD_BOUNDARY::REQUIRED
					: HString::WORD_BOUNDARY::OPTIONAL
			) : wordBoundary_
	);
}

}

HString string( yaal::hcore::HString const& string_, HRuleBase::WHITE_SPACE whiteSpace_, HString::WORD_BOUNDARY wordBoundary_ ) {
	M_PROLOG
	M_ENSURE( ! string_.is_empty() );
	return ( HString( string_, skip_ws( string_, whiteSpace_ ), word_boundary( string_, wordBoundary_ ) ) );
	M_EPILOG
}

HString string( yaal::hcore::HString const& string_, HString::action_string_t const& action_, HRuleBase::WHITE_SPACE whiteSpace_, HString::WORD_BOUNDARY wordBoundary_ ) {
	M_PROLOG
	M_ENSURE( ! string_.is_empty() );
	return ( HString( string_, action_, skip_ws( string_, whiteSpace_ ), word_boundary( string_, wordBoundary_ ) ) );
	M_EPILOG
}

HString string( yaal::hcore::HString const& string_, HString::action_string_range_t const& action_, HRuleBase::WHITE_SPACE whiteSpace_, HString::WORD_BOUNDARY wordBoundary_ ) {
	M_PROLOG
	M_ENSURE( ! string_.is_empty() );
	return ( HString( string_, action_, skip_ws( string_, whiteSpace_ ), word_boundary( string_, wordBoundary_ ) ) );
	M_EPILOG
}

HString string( yaal::hcore::HString const& string_, HString::action_t const& action_, HRuleBase::WHITE_SPACE whiteSpace_, HString::WORD_BOUNDARY wordBoundary_ ) {
	M_PROLOG
	M_ENSURE( ! string_.is_empty() );
	return ( HString( string_, action_, skip_ws( string_, whiteSpace_ ), word_boundary( string_, wordBoundary_ ) ) );
	M_EPILOG
}

HString string( yaal::hcore::HString const& string_, HString::action_range_t const& action_, HRuleBase::WHITE_SPACE whiteSpace_, HString::WORD_BOUNDARY wordBoundary_ ) {
	M_PROLOG
	M_ENSURE( ! string_.is_empty() );
	return ( HString( string_, action_, skip_ws( string_, whiteSpace_ ), word_boundary( string_, wordBoundary_ ) ) );
	M_EPILOG
}

HString string( yaal::hcore::HString const& string_, HString::WORD_BOUNDARY wordBoundary_ ) {
	M_PROLOG
	M_ENSURE( ! string_.is_empty() );
	return ( HString( string_, ! is_whitespace( string_.front() ), word_boundary( string_, wordBoundary_ ) ) );
	M_EPILOG
}

HString string( yaal::hcore::HString const& string_, HString::action_string_t const& action_, HString::WORD_BOUNDARY wordBoundary_ ) {
	M_PROLOG
	M_ENSURE( ! string_.is_empty() );
	return ( HString( string_, action_, ! is_whitespace( string_.front() ), word_boundary( string_, wordBoundary_ ) ) );
	M_EPILOG
}

HString string( yaal::hcore::HString const& string_, HString::action_string_range_t const& action_, HString::WORD_BOUNDARY wordBoundary_ ) {
	M_PROLOG
	M_ENSURE( ! string_.is_empty() );
	return ( HString( string_, action_, ! is_whitespace( string_.front() ), word_boundary( string_, wordBoundary_ ) ) );
	M_EPILOG
}

HString string( yaal::hcore::HString const& string_, HString::action_t const& action_, HString::WORD_BOUNDARY wordBoundary_ ) {
	M_PROLOG
	M_ENSURE( ! string_.is_empty() );
	return ( HString( string_, action_, ! is_whitespace( string_.front() ), word_boundary( string_, wordBoundary_ ) ) );
	M_EPILOG
}

HString string( yaal::hcore::HString const& string_, HString::action_range_t const& action_, HString::WORD_BOUNDARY wordBoundary_ ) {
	M_PROLOG
	M_ENSURE( ! string_.is_empty() );
	return ( HString( string_, action_, ! is_whitespace( string_.front() ), word_boundary( string_, wordBoundary_ ) ) );
	M_EPILOG
}

namespace {

void sanity_check( HString::dictionary_t const& strings_ ) {
	M_PROLOG
	typedef executing_parser::HString this_type;
	M_ENSURE( ! strings_.is_empty() );
	for ( int outer( 0 ), C( static_cast<int>( strings_.get_size() ) ); outer < C; ++ outer ) {
		hcore::HString const& s( strings_[outer] );
		M_ENSURE( ! s.is_empty() );
		for ( int inner( 0 ); inner < outer; ++ inner ) {
			if ( s.find( strings_[inner] ) == 0 ) {
				throw HStringException( "token `"_ys.append( s ).append( "' would be hidden" ) );
			}
		}
	}
	return;
	M_EPILOG
}

inline bool skip_ws( HString::dictionary_t const& strings_, HRuleBase::WHITE_SPACE whiteSpace_ ) {
	bool skipWS( whiteSpace_ == HRuleBase::WHITE_SPACE::SKIP );
	if ( ! skipWS && ( whiteSpace_ == HRuleBase::WHITE_SPACE::AUTO ) ) {
		skipWS = true;
		for ( hcore::HString const& s : strings_ ) {
			if ( is_whitespace( s.front() ) ) {
				skipWS = false;
				break;
			}
		}
	}
	return ( skipWS );
}

inline HString::WORD_BOUNDARY word_boundary( HString::dictionary_t const& strings_, HString::WORD_BOUNDARY wordBoundary_ ) {
	HString::WORD_BOUNDARY wordBoundary( wordBoundary_ );
	if ( wordBoundary_ == HString::WORD_BOUNDARY::AUTO ) {
		wordBoundary = HString::WORD_BOUNDARY::OPTIONAL;
		for ( hcore::HString const& s : strings_ ) {
			if ( character_class<CHARACTER_CLASS::WORD>().has( s.back() ) ) {
				wordBoundary = HString::WORD_BOUNDARY::REQUIRED;
				break;
			}
		}
	}
	return ( wordBoundary );
}

}

HString string( HString::dictionary_t const& strings_, HRuleBase::WHITE_SPACE whiteSpace_, HString::WORD_BOUNDARY wordBoundary_ ) {
	M_PROLOG
	sanity_check( strings_ );
	return ( HString( strings_, skip_ws( strings_, whiteSpace_ ), word_boundary( strings_, wordBoundary_ ) ) );
	M_EPILOG
}

HString string( HString::dictionary_t const& strings_, HString::action_string_t const& action_, HRuleBase::WHITE_SPACE whiteSpace_, HString::WORD_BOUNDARY wordBoundary_ ) {
	M_PROLOG
	sanity_check( strings_ );
	return ( HString( strings_, action_, skip_ws( strings_, whiteSpace_ ), word_boundary( strings_, wordBoundary_ ) ) );
	M_EPILOG
}

HString string( HString::dictionary_t const& strings_, HString::action_string_range_t const& action_, HRuleBase::WHITE_SPACE whiteSpace_, HString::WORD_BOUNDARY wordBoundary_ ) {
	M_PROLOG
	sanity_check( strings_ );
	return ( HString( strings_, action_, skip_ws( strings_, whiteSpace_ ), word_boundary( strings_, wordBoundary_ ) ) );
	M_EPILOG
}

HString string( HString::dictionary_t const& strings_, HString::action_t const& action_, HRuleBase::WHITE_SPACE whiteSpace_, HString::WORD_BOUNDARY wordBoundary_ ) {
	M_PROLOG
	sanity_check( strings_ );
	return ( HString( strings_, action_, skip_ws( strings_, whiteSpace_ ), word_boundary( strings_, wordBoundary_ ) ) );
	M_EPILOG
}

HString string( HString::dictionary_t const& strings_, HString::action_range_t const& action_, HRuleBase::WHITE_SPACE whiteSpace_, HString::WORD_BOUNDARY wordBoundary_ ) {
	M_PROLOG
	sanity_check( strings_ );
	return ( HString( strings_, action_, skip_ws( strings_, whiteSpace_ ), word_boundary( strings_, wordBoundary_ ) ) );
	M_EPILOG
}

HString string( HString::dictionary_t const& strings_, HString::WORD_BOUNDARY wordBoundary_ ) {
	M_PROLOG
	sanity_check( strings_ );
	return ( HString( strings_, skip_ws( strings_, HRuleBase::WHITE_SPACE::AUTO ), word_boundary( strings_, wordBoundary_ ) ) );
	M_EPILOG
}

HString string( HString::dictionary_t const& strings_, HString::action_string_t const& action_, HString::WORD_BOUNDARY wordBoundary_ ) {
	M_PROLOG
	sanity_check( strings_ );
	return ( HString( strings_, action_, skip_ws( strings_, HRuleBase::WHITE_SPACE::AUTO ), word_boundary( strings_, wordBoundary_ ) ) );
	M_EPILOG
}

HString string( HString::dictionary_t const& strings_, HString::action_string_range_t const& action_, HString::WORD_BOUNDARY wordBoundary_ ) {
	M_PROLOG
	sanity_check( strings_ );
	return ( HString( strings_, action_, skip_ws( strings_, HRuleBase::WHITE_SPACE::AUTO ), word_boundary( strings_, wordBoundary_ ) ) );
	M_EPILOG
}

HString string( HString::dictionary_t const& strings_, HString::action_t const& action_, HString::WORD_BOUNDARY wordBoundary_ ) {
	M_PROLOG
	sanity_check( strings_ );
	return ( HString( strings_, action_, skip_ws( strings_, HRuleBase::WHITE_SPACE::AUTO ), word_boundary( strings_, wordBoundary_ ) ) );
	M_EPILOG
}

HString string( HString::dictionary_t const& strings_, HString::action_range_t const& action_, HString::WORD_BOUNDARY wordBoundary_ ) {
	M_PROLOG
	sanity_check( strings_ );
	return ( HString( strings_, action_, skip_ws( strings_, HRuleBase::WHITE_SPACE::AUTO ), word_boundary( strings_, wordBoundary_ ) ) );
	M_EPILOG
}

HRegex::HRegex( hcore::HString const& string_, bool skipWS_ )
	: HRuleBase( skipWS_ )
	, _regex( make_pointer<hcore::HRegex>( string_ ) )
	, _actionString()
	, _actionStringPosition() {
	return;
}

HRegex::HRegex( hcore::HString const& string_, action_t const& action_, bool skipWS_ )
	: HRuleBase( action_, skipWS_ )
	, _regex( make_pointer<hcore::HRegex>( string_ ) )
	, _actionString()
	, _actionStringPosition() {
	return;
}

HRegex::HRegex( hcore::HString const& string_, action_range_t const& action_, bool skipWS_ )
	: HRuleBase( action_, skipWS_ )
	, _regex( make_pointer<hcore::HRegex>( string_ ) )
	, _actionString()
	, _actionStringPosition() {
	return;
}

HRegex::HRegex( hcore::HString const& string_, action_string_t const& action_, bool skipWS_ )
	: HRuleBase( skipWS_ )
	, _regex( make_pointer<hcore::HRegex>( string_ ) )
	, _actionString( action_ )
	, _actionStringPosition() {
	return;
}

HRegex::HRegex( hcore::HString const& string_, action_string_range_t const& action_, bool skipWS_ )
	: HRuleBase( skipWS_ )
	, _regex( make_pointer<hcore::HRegex>( string_ ) )
	, _actionString()
	, _actionStringPosition( action_ ) {
	return;
}

HRegex::HRegex( regex_t const& regex_, action_t const& action_, bool skipWS_ )
	: HRuleBase( action_, skipWS_ )
	, _regex( regex_ )
	, _actionString()
	, _actionStringPosition() {
	return;
}

HRegex::HRegex( regex_t const& regex_, action_range_t const& action_, bool skipWS_ )
	: HRuleBase( action_, skipWS_ )
	, _regex( regex_ )
	, _actionString()
	, _actionStringPosition() {
	return;
}

HRegex::HRegex( regex_t const& regex_, action_string_t const& action_, bool skipWS_ )
	: HRuleBase( skipWS_ )
	, _regex( regex_ )
	, _actionString( action_ )
	, _actionStringPosition() {
	return;
}

HRegex::HRegex( regex_t const& regex_, action_string_range_t const& action_, bool skipWS_ )
	: HRuleBase( skipWS_ )
	, _regex( regex_ )
	, _actionString()
	, _actionStringPosition( action_ ) {
	return;
}

HRegex::HRegex( HRegex const& regex_ )
	: HRuleBase( regex_._action, regex_._actionPosition, regex_._skipWS )
	, _regex( regex_._regex )
	, _actionString( regex_._actionString )
	, _actionStringPosition( regex_._actionStringPosition ) {
	return;
}

HRegex HRegex::operator[]( action_t const& action_ ) const {
	M_PROLOG
	M_ENSURE( ! has_action() );
	return ( HRegex( _regex, action_, _skipWS ) );
	M_EPILOG
}

HRegex HRegex::operator[]( action_range_t const& action_ ) const {
	M_PROLOG
	M_ENSURE( ! has_action() );
	return ( HRegex( _regex, action_, _skipWS ) );
	M_EPILOG
}

HRegex HRegex::operator[]( action_string_t const& action_ ) const {
	M_PROLOG
	M_ENSURE( ! has_action() );
	return ( HRegex( _regex, action_, _skipWS ) );
	M_EPILOG
}

HRegex HRegex::operator[]( action_string_range_t const& action_ ) const {
	M_PROLOG
	M_ENSURE( ! has_action() );
	return ( HRegex( _regex, action_, _skipWS ) );
	M_EPILOG
}

bool HRegex::do_has_action( void ) const {
	return ( HRuleBase::do_has_action() || ( !! _actionString ) || ( !! _actionStringPosition ) );
}

hcore::HUTF8String::const_iterator HRegex::do_parse( HExecutingParser* executingParser_, hcore::HUTF8String::const_iterator const& first_, hcore::HUTF8String::const_iterator const& last_ ) const {
	M_PROLOG
	yaal::hcore::HUTF8String::const_iterator scan( first_ );
	bool matched( false );
	if ( scan != last_ ) {
		hcore::HRegex::HMatchIterator it( _regex->find( HUTF8String( scan, last_ ) ) );
		if ( ( it != _regex->end() ) && ( it->size() <= ( last_ - scan ) ) ) {
			int pos( position( executingParser_, first_ ) );
			HRange rng( pos, pos + it->start() + it->size() );
			if ( !! _actionString ) {
				HUTF8String::const_iterator from( scan + it->start() );
				HUTF8String::const_iterator to( from + it->size() );
				add_execution_step( executingParser_, call( _actionString, hcore::HUTF8String( from, to ) ) );
			} else if ( !! _actionStringPosition ) {
				HUTF8String::const_iterator from( scan + it->start() );
				HUTF8String::const_iterator to( from + it->size() );
				add_execution_step( executingParser_, call( _actionStringPosition, hcore::HUTF8String( from, to ), rng ) );
			} else if ( !! _action ) {
				add_execution_step( executingParser_, call( _action ) );
			} else if ( !! _actionPosition ) {
				add_execution_step( executingParser_, call( _actionPosition, rng ) );
			}
			scan += ( it->start() + it->size() );
			matched = true;
		}
	} else {
		scan = first_;
	}
	if ( ! matched ) {
		report_error( executingParser_, scan, "expected character sequence matching regular expression: " + _regex->pattern() );
		scan = first_;
	}
	return ( scan );
	M_EPILOG
}

HRuleBase::ptr_t HRegex::do_clone( void ) const {
	M_PROLOG
	return ( make_pointer<HRegex>( *this ) );
	M_EPILOG
}

void HRegex::do_describe( HRuleDescription& rd_, rule_use_t const& ) const {
	M_PROLOG
	rd_.desc( "regex( \"" );
	rd_.desc( _regex->pattern().substr( 1 ) );
	rd_.desc( "\" )" );
	return;
	M_EPILOG
}

void HRegex::do_detach( HRuleBase const*, visited_t&, bool& ) {
	M_PROLOG
	return;
	M_EPILOG
}

bool HRegex::do_detect_recursion( HRecursionDetector& recursionDetector_, bool ) const {
	M_PROLOG
	recursionDetector_.reset_visits();
	return ( true );
	M_EPILOG
}

void HRegex::do_find_recursions( HRuleAggregator& ) {
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

HFollows operator >> ( HFollows const& predecessor_, char const* string_ ) {
	M_PROLOG
	return ( HFollows( predecessor_, string( string_ ) ) );
	M_EPILOG
}

HFollows operator >> ( HFollows const& predecessor_, hcore::HString const& string_ ) {
	M_PROLOG
	return ( HFollows( predecessor_, string( string_ ) ) );
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

HCharacter constant( char character_, HRuleBase::WHITE_SPACE whiteSpace_ ) {
	M_PROLOG
	return ( character( character_, whiteSpace_ ) );
	M_EPILOG
}

HCharacter constant( char character_, HRuleBase::action_t const& action_, HRuleBase::WHITE_SPACE whiteSpace_ ) {
	M_PROLOG
	return ( character( character_, whiteSpace_ )[ action_ ] );
	M_EPILOG
}

HCharacter constant( char character_, HRuleBase::action_range_t const& action_, HRuleBase::WHITE_SPACE whiteSpace_ ) {
	M_PROLOG
	return ( character( character_, whiteSpace_ )[ action_ ] );
	M_EPILOG
}

HCharacter constant( char character_, HCharacter::action_character_t const& action_, HRuleBase::WHITE_SPACE whiteSpace_ ) {
	M_PROLOG
	return ( character( character_, whiteSpace_ )[ action_ ] );
	M_EPILOG
}

HCharacter constant( char character_, HCharacter::action_character_range_t const& action_, HRuleBase::WHITE_SPACE whiteSpace_ ) {
	M_PROLOG
	return ( character( character_, whiteSpace_ )[ action_ ] );
	M_EPILOG
}

HCharacter characters( yaal::hcore::HString const& string_, HRuleBase::WHITE_SPACE whiteSpace_ ) {
	M_PROLOG
	M_ENSURE( ! string_.is_empty() );
	return ( character( string_, whiteSpace_ ) );
	M_EPILOG
}

HCharacter characters( yaal::hcore::HString const& string_, HRuleBase::action_t const& action_, HRuleBase::WHITE_SPACE whiteSpace_ ) {
	M_PROLOG
	M_ENSURE( ! string_.is_empty() );
	return ( character( string_, whiteSpace_ )[ action_ ] );
	M_EPILOG
}

HCharacter characters( yaal::hcore::HString const& string_, HRuleBase::action_range_t const& action_, HRuleBase::WHITE_SPACE whiteSpace_ ) {
	M_PROLOG
	M_ENSURE( ! string_.is_empty() );
	return ( character( string_, whiteSpace_ )[ action_ ] );
	M_EPILOG
}

HCharacter characters( yaal::hcore::HString const& string_, HCharacter::action_character_t const& action_, HRuleBase::WHITE_SPACE whiteSpace_ ) {
	M_PROLOG
	M_ENSURE( ! string_.is_empty() );
	return ( character( string_, whiteSpace_ )[ action_ ] );
	M_EPILOG
}

HCharacter characters( yaal::hcore::HString const& string_, HCharacter::action_character_range_t const& action_, HRuleBase::WHITE_SPACE whiteSpace_ ) {
	M_PROLOG
	M_ENSURE( ! string_.is_empty() );
	return ( character( string_, whiteSpace_ )[ action_ ] );
	M_EPILOG
}

HString constant( yaal::hcore::HString const& string_, HRuleBase::WHITE_SPACE whiteSpace_, HString::WORD_BOUNDARY wordBoundary_ ) {
	M_PROLOG
	M_ENSURE( ! string_.is_empty() );
	return ( string( string_, whiteSpace_, wordBoundary_ ) );
	M_EPILOG
}

HString constant( yaal::hcore::HString const& string_, HRuleBase::action_t const& action_, HRuleBase::WHITE_SPACE whiteSpace_, HString::WORD_BOUNDARY wordBoundary_ ) {
	M_PROLOG
	M_ENSURE( ! string_.is_empty() );
	return ( string( string_, whiteSpace_, wordBoundary_ )[ action_ ] );
	M_EPILOG
}

HString constant( yaal::hcore::HString const& string_, HRuleBase::action_range_t const& action_, HRuleBase::WHITE_SPACE whiteSpace_, HString::WORD_BOUNDARY wordBoundary_ ) {
	M_PROLOG
	M_ENSURE( ! string_.is_empty() );
	return ( string( string_, whiteSpace_, wordBoundary_ )[ action_ ] );
	M_EPILOG
}

HString constant( yaal::hcore::HString const& string_, HString::action_string_t const& action_, HRuleBase::WHITE_SPACE whiteSpace_, HString::WORD_BOUNDARY wordBoundary_ ) {
	M_PROLOG
	M_ENSURE( ! string_.is_empty() );
	return ( string( string_, whiteSpace_, wordBoundary_ )[ action_ ] );
	M_EPILOG
}

HString constant( yaal::hcore::HString const& string_, HString::action_string_range_t const& action_, HRuleBase::WHITE_SPACE whiteSpace_, HString::WORD_BOUNDARY wordBoundary_ ) {
	M_PROLOG
	M_ENSURE( ! string_.is_empty() );
	return ( string( string_, whiteSpace_, wordBoundary_ )[ action_ ] );
	M_EPILOG
}

HString constant( yaal::hcore::HString const& string_, HString::WORD_BOUNDARY wordBoundary_ ) {
	M_PROLOG
	M_ENSURE( ! string_.is_empty() );
	return ( string( string_, wordBoundary_ ) );
	M_EPILOG
}

HString constant( yaal::hcore::HString const& string_, HRuleBase::action_t const& action_, HString::WORD_BOUNDARY wordBoundary_ ) {
	M_PROLOG
	M_ENSURE( ! string_.is_empty() );
	return ( string( string_, wordBoundary_ )[ action_ ] );
	M_EPILOG
}

HString constant( yaal::hcore::HString const& string_, HRuleBase::action_range_t const& action_, HString::WORD_BOUNDARY wordBoundary_ ) {
	M_PROLOG
	M_ENSURE( ! string_.is_empty() );
	return ( string( string_, wordBoundary_ )[ action_ ] );
	M_EPILOG
}

HString constant( yaal::hcore::HString const& string_, HString::action_string_t const& action_, HString::WORD_BOUNDARY wordBoundary_ ) {
	M_PROLOG
	M_ENSURE( ! string_.is_empty() );
	return ( string( string_, wordBoundary_ )[ action_ ] );
	M_EPILOG
}

HString constant( yaal::hcore::HString const& string_, HString::action_string_range_t const& action_, HString::WORD_BOUNDARY wordBoundary_ ) {
	M_PROLOG
	M_ENSURE( ! string_.is_empty() );
	return ( string( string_, wordBoundary_ )[ action_ ] );
	M_EPILOG
}

HString constant( HString::dictionary_t const& strings_, HRuleBase::WHITE_SPACE whiteSpace_, HString::WORD_BOUNDARY wordBoundary_ ) {
	M_PROLOG
	M_ENSURE( ! strings_.is_empty() );
	return ( string( strings_, whiteSpace_, wordBoundary_ ) );
	M_EPILOG
}

HString constant( HString::dictionary_t const& strings_, HRuleBase::action_t const& action_, HRuleBase::WHITE_SPACE whiteSpace_, HString::WORD_BOUNDARY wordBoundary_ ) {
	M_PROLOG
	M_ENSURE( ! strings_.is_empty() );
	return ( string( strings_, whiteSpace_, wordBoundary_ )[ action_ ] );
	M_EPILOG
}

HString constant( HString::dictionary_t const& strings_, HRuleBase::action_range_t const& action_, HRuleBase::WHITE_SPACE whiteSpace_, HString::WORD_BOUNDARY wordBoundary_ ) {
	M_PROLOG
	M_ENSURE( ! strings_.is_empty() );
	return ( string( strings_, whiteSpace_, wordBoundary_ )[ action_ ] );
	M_EPILOG
}

HString constant( HString::dictionary_t const& strings_, HString::action_string_t const& action_, HRuleBase::WHITE_SPACE whiteSpace_, HString::WORD_BOUNDARY wordBoundary_ ) {
	M_PROLOG
	M_ENSURE( ! strings_.is_empty() );
	return ( string( strings_, whiteSpace_, wordBoundary_ )[ action_ ] );
	M_EPILOG
}

HString constant( HString::dictionary_t const& strings_, HString::action_string_range_t const& action_, HRuleBase::WHITE_SPACE whiteSpace_, HString::WORD_BOUNDARY wordBoundary_ ) {
	M_PROLOG
	M_ENSURE( ! strings_.is_empty() );
	return ( string( strings_, whiteSpace_, wordBoundary_ )[ action_ ] );
	M_EPILOG
}

HString constant( HString::dictionary_t const& strings_, HString::WORD_BOUNDARY wordBoundary_ ) {
	M_PROLOG
	M_ENSURE( ! strings_.is_empty() );
	return ( string( strings_, wordBoundary_ ) );
	M_EPILOG
}

HString constant( HString::dictionary_t const& strings_, HRuleBase::action_t const& action_, HString::WORD_BOUNDARY wordBoundary_ ) {
	M_PROLOG
	M_ENSURE( ! strings_.is_empty() );
	return ( string( strings_, wordBoundary_ )[ action_ ] );
	M_EPILOG
}

HString constant( HString::dictionary_t const& strings_, HRuleBase::action_range_t const& action_, HString::WORD_BOUNDARY wordBoundary_ ) {
	M_PROLOG
	M_ENSURE( ! strings_.is_empty() );
	return ( string( strings_, wordBoundary_ )[ action_ ] );
	M_EPILOG
}

HString constant( HString::dictionary_t const& strings_, HString::action_string_t const& action_, HString::WORD_BOUNDARY wordBoundary_ ) {
	M_PROLOG
	M_ENSURE( ! strings_.is_empty() );
	return ( string( strings_, wordBoundary_ )[ action_ ] );
	M_EPILOG
}

HString constant( HString::dictionary_t const& strings_, HString::action_string_range_t const& action_, HString::WORD_BOUNDARY wordBoundary_ ) {
	M_PROLOG
	M_ENSURE( ! strings_.is_empty() );
	return ( string( strings_, wordBoundary_ )[ action_ ] );
	M_EPILOG
}

HRegex regex( yaal::hcore::HString const& pattern_, bool skipWS_ ) {
	M_PROLOG
	M_ENSURE( ! pattern_.is_empty() );
	return ( HRegex( pattern_[0] == '^' ? pattern_  : "^" + pattern_, skipWS_ ) );
	M_EPILOG
}

HRegex regex( yaal::hcore::HString const& pattern_, HRuleBase::action_t const& action_, bool skipWS_ ) {
	M_PROLOG
	M_ENSURE( ! pattern_.is_empty() );
	return ( HRegex( pattern_[0] == '^' ? pattern_  : "^" + pattern_, action_, skipWS_ ) );
	M_EPILOG
}

HRegex regex( yaal::hcore::HString const& pattern_, HRuleBase::action_range_t const& action_, bool skipWS_ ) {
	M_PROLOG
	M_ENSURE( ! pattern_.is_empty() );
	return ( HRegex( pattern_[0] == '^' ? pattern_  : "^" + pattern_, action_, skipWS_ ) );
	M_EPILOG
}

HRegex regex( yaal::hcore::HString const& pattern_, HRegex::action_string_t const& action_, bool skipWS_ ) {
	M_PROLOG
	M_ENSURE( ! pattern_.is_empty() );
	return ( HRegex( pattern_[0] == '^' ? pattern_  : "^" + pattern_, action_, skipWS_ ) );
	M_EPILOG
}

HRegex regex( yaal::hcore::HString const& pattern_, HRegex::action_string_range_t const& action_, bool skipWS_ ) {
	M_PROLOG
	M_ENSURE( ! pattern_.is_empty() );
	return ( HRegex( pattern_[0] == '^' ? pattern_  : "^" + pattern_, action_, skipWS_ ) );
	M_EPILOG
}

HRule regex( yaal::hcore::HString const& name_, yaal::hcore::HString const& pattern_, bool skipWS_ ) {
	M_PROLOG
	M_ENSURE( ! name_.is_empty() );
	M_ENSURE( ! pattern_.is_empty() );
	return ( HRule( name_, HRegex( pattern_[0] == '^' ? pattern_  : "^" + pattern_, skipWS_ ) ) );
	M_EPILOG
}

HRule regex( yaal::hcore::HString const& name_, yaal::hcore::HString const& pattern_, HRegex::action_t const& action_, bool skipWS_ ) {
	M_PROLOG
	M_ENSURE( ! name_.is_empty() );
	M_ENSURE( ! pattern_.is_empty() );
	return ( HRule( name_, HRegex( pattern_[0] == '^' ? pattern_  : "^" + pattern_, action_, skipWS_ ) ) );
	M_EPILOG
}

HRule regex( yaal::hcore::HString const& name_, yaal::hcore::HString const& pattern_, HRegex::action_range_t const& action_, bool skipWS_ ) {
	M_PROLOG
	M_ENSURE( ! name_.is_empty() );
	M_ENSURE( ! pattern_.is_empty() );
	return ( HRule( name_, HRegex( pattern_[0] == '^' ? pattern_  : "^" + pattern_, action_, skipWS_ ) ) );
	M_EPILOG
}

HRule regex( yaal::hcore::HString const& name_, yaal::hcore::HString const& pattern_, HRegex::action_string_t const& action_, bool skipWS_ ) {
	M_PROLOG
	M_ENSURE( ! name_.is_empty() );
	M_ENSURE( ! pattern_.is_empty() );
	return ( HRule( name_, HRegex( pattern_[0] == '^' ? pattern_  : "^" + pattern_, action_, skipWS_ ) ) );
	M_EPILOG
}

HRule regex( yaal::hcore::HString const& name_, yaal::hcore::HString const& pattern_, HRegex::action_string_range_t const& action_, bool skipWS_ ) {
	M_PROLOG
	M_ENSURE( ! name_.is_empty() );
	M_ENSURE( ! pattern_.is_empty() );
	return ( HRule( name_, HRegex( pattern_[0] == '^' ? pattern_  : "^" + pattern_, action_, skipWS_ ) ) );
	M_EPILOG
}

HAction::HAction( HAction const& other_ )
	: HRuleBase( other_._action, other_._actionPosition, other_._skipWS ) {
	return;
}

HAction::HAction( action_t const& action_ )
	: HRuleBase( action_, false ) {
	return;
}

HAction::HAction( action_range_t const& action_ )
	: HRuleBase( action_, false ) {
	return;
}

HRuleBase::ptr_t HAction::do_clone( void ) const {
	M_PROLOG
	return ( make_pointer<HAction>( *this ) );
	M_EPILOG
}

HAction HAction::operator[]( action_t const& action_ ) const {
	M_PROLOG
	M_ENSURE( ! has_action() );
	return ( HAction( action_ ) );
	M_EPILOG
}

HAction HAction::operator[]( action_range_t const& action_ ) const {
	M_PROLOG
	M_ENSURE( ! has_action() );
	return ( HAction( action_ ) );
	M_EPILOG
}

yaal::hcore::HUTF8String::const_iterator HAction::do_parse( HExecutingParser* executingParser_, hcore::HUTF8String::const_iterator const& first_, hcore::HUTF8String::const_iterator const& ) const {
	M_PROLOG
	if ( !! _action ) {
		add_execution_step( executingParser_, _action );
	} else if ( !! _actionPosition ) {
		add_execution_step( executingParser_, call( _actionPosition, range( executingParser_, first_, first_ ) ) );
	}
	return ( first_ );
	M_EPILOG
}

bool HAction::do_is_optional( void ) const {
	return ( true );
}

void HAction::do_describe( HRuleDescription& rd_, rule_use_t const& ) const {
	M_PROLOG
	rd_.desc( "()" );
	return;
	M_EPILOG
}

void HAction::do_detach( HRuleBase const*, visited_t&, bool& ) {
	M_PROLOG
	return;
	M_EPILOG
}

bool HAction::do_detect_recursion( HRecursionDetector&, bool ) const {
	M_PROLOG
	return ( false );
	M_EPILOG
}

void HAction::do_find_recursions( HRuleAggregator& ) {
	M_PROLOG
	return;
	M_EPILOG
}

bool HAction::do_always_matches( void ) const {
	return ( true );
}

HFollows operator >> ( HRuleBase const& rule_, HRuleBase::action_range_t const& actionPosition_  ) {
	return ( HFollows( rule_, HAction( actionPosition_ ) ) );
}

HFollows operator >> ( HRuleBase const& rule_, HRuleBase::action_t const& action_ ) {
	return ( HFollows( rule_, HAction( action_ ) ) );
}

HFollows operator >> ( HRuleBase::action_range_t const& actionPosition_, HRuleBase const& rule_ ) {
	return ( HFollows( HAction( actionPosition_ ), rule_ ) );
}

HFollows operator >> ( HRuleBase::action_t const& action_, HRuleBase const& rule_ ) {
	return ( HFollows( HAction( action_ ), rule_ ) );
}

HFollows operator >> ( HFollows const& predecessor_, HRuleBase::action_range_t const& actionPosition_ ) {
	return ( HFollows( predecessor_, HAction( actionPosition_ ) ) );
}

HFollows operator >> ( HFollows const& predecessor_, HRuleBase::action_t const& action_ ) {
	return ( HFollows( predecessor_, HAction( action_ ) ) );
}

HRuleDescription::HRuleDescription( void )
	: _children()
	, _description()
	, _automaticNames() {
	return;
}

void HRuleDescription::desc( yaal::hcore::HString const& desc_ ) {
	M_PROLOG
	_description += desc_;
	return;
	M_EPILOG
}

yaal::hcore::HString const& HRuleDescription::description( void ) const {
	M_PROLOG
	return ( _description );
	M_EPILOG
}

HRuleDescription::named_children_t const& HRuleDescription::children( void ) const {
	M_PROLOG
	return ( _children );
	M_EPILOG
}

void HRuleDescription::clear( void ) {
	M_PROLOG
	_children.clear();
	_description.clear();
	return;
	M_EPILOG
}

void HRuleDescription::add( HNamedRule const* nr_ ) {
	M_PROLOG
	_children.push_back( nr_ );
	return;
	M_EPILOG
}

yaal::hcore::HString const& HRuleDescription::make_name( HNamedRule const& nr_ ) {
	M_PROLOG
	hcore::HString const* name( nullptr );

	if ( ! nr_.name().is_empty() ) {
		name = &nr_.name();
	} else {
		name = &make_name_auto( nr_.id() );
	}
	return ( *name );
	M_EPILOG
}

yaal::hcore::HString const& HRuleDescription::make_name_auto( HRuleBase const* rule_ ) {
	M_PROLOG
	hcore::HString const* name( nullptr );
	automatic_names_t::const_iterator a( _automaticNames.find( rule_ ) );
	if ( a != _automaticNames.end() ) {
		name = &a->second;
	} else {
		static int const MAX_AUTO_NAMES_COUNT = 26;
		int autoNamesCount( static_cast<int>( _automaticNames.get_size() ) );
		if ( autoNamesCount < MAX_AUTO_NAMES_COUNT ) {
			name = &( _automaticNames[rule_] = hcore::HString( static_cast<char>( 'A' + autoNamesCount ) ) + '_' );
		} else {
			autoNamesCount -= MAX_AUTO_NAMES_COUNT;
			char hi( static_cast<char>( 'A' + ( autoNamesCount / MAX_AUTO_NAMES_COUNT ) ) );
			char lo( static_cast<char>( 'A' + ( autoNamesCount % MAX_AUTO_NAMES_COUNT ) ) );
			name = &( _automaticNames[rule_] = hcore::HString( hi ) + lo + '_' );
		}
	}
	return ( *name );
	M_EPILOG
}

void HRuleDescription::reset_names( void ) {
	M_PROLOG
	_automaticNames.clear();
	return;
	M_EPILOG
}

HGrammarDescription::HGrammarDescription( HRuleBase const& rule_ )
	: _rules()
	, _visited()
	, _ruleOrder() {
	M_PROLOG
	HRuleDescription rd;
	rule_use_t ru;
	rule_.rule_use( ru );
	HRule const* rule( dynamic_cast<HRule const*>( &rule_ ) );
	hcore::HString rootName( ! rule ? rd.make_name_auto( &rule_ ) : "" );
	rule_.describe( rd, ru );
	if ( ( ( ! rule || rule->get_name().is_empty() ) && ! ( rule && ( rd.children().size() == 1 ) && ( rd.children()[0] == rule->get_named_rule() ) ) )
		|| ( rule && rule->has_action() && rule->get_name().is_empty() ) ) {
		if ( rootName.is_empty() ) {
			rd.clear();
			rd.reset_names();
			rootName = rd.make_name_auto( &rule_ );
			rule_.describe( rd, ru );
		}
		_rules.push_back( rootName + " = " + rd.description() );
	}
	copy( rd.children().begin(), rd.children().end(), push_insert_iterator( _ruleOrder ) );
	if ( ! rootName.is_empty() ) {
		_visited.insert( rootName + &rule_ );
	}
	while ( ! _ruleOrder.is_empty() ) {
		HNamedRule const* r( _ruleOrder.front() );
		_ruleOrder.pop();
		if ( _visited.insert( r->name() + r->id() ).second ) {
			rd.clear();
			rd.desc( rd.make_name( *r ) );
			rd.desc( " = " );
			r->id()->describe( rd, ru );
			_rules.push_back( rd.description() );
			copy( rd.children().begin(), rd.children().end(), push_insert_iterator( _ruleOrder ) );
		}
	}
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

}

}

}

